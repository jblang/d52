
/*
 * D52 8052 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * d52.c - Main File
 *
 * Version 3.4.1 - 2007/09/02
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>

#include	"d52.h"
#include	"common.h"
#include	"analyze.h"
#include	"d52pass1.h"
#include	"d52pass2.h"
#include	"d52table.h"

//
//		Global variables
//

int	incflag;							// include file flag
int	keilflag;						// Keil A51 compatibility
int	dotpseudo = 0;

byte	dirregs[128];					// access flags for dir reg
byte	sfrflags[128];					// SFR name change flags
byte	sbflags[128];					// SFR bit name change flags
byte	mbflags[128];					// bit adrs mem name chg flgs

#include	"dispass0.c"
#include	"dispass3.c"

void usage(void)
{
	printf("\nUsage: d52 [options] <filename>\n"
		"Options may be entered Unix style (-d) or DOS style (/b)\n"
		"\t-a use ascii macro instead of db/defb for text.\n"
		"\t-b disassemble file with .bin extension.\n"
		"\t-d include address and data in the comment field.\n"
		"\t-h disassemble file with .hex extension.\n"
		"\t   If neither 'b' nor 'h' is specified, D52 will first search\n"
		"\t   for a .hex file, and if not found, then a .bin file\n"
		"\t-i put statement in output file to include 'sfr52.inc'.\n"
		"\t-k disassemble for Keil A51 (obsolete).\n"
		"\t-n use C style for hexadecimal operands\n"
		"\t-p put dot '.' at beginning of pseudo ops\n"
		"\t-s use 'defb' and 'defw' instead of 'db' and 'dw' for binary data.\n"
		"\t-t trace and analyze code before disassembly.\n"
		"\t   (-t will overwrite any existing ctl file for the 8052 file\n"
		"\t    being disassembled.)\n"
		"\t-u output labels, symbols, and mnemonics in upper case.\n"
		"\t-x add a hexadecimal offset to file addresses.\n"
		"\t-z[filename] read cycle count file.\n"
		"\nOptions may be entered in a freeform fashion as long "
		"as a dash (-) or\n"
		"a slash (/) preceeds any option that preceeds the filename."
		"\nExamples:\n"
		"\td52 filename bd\n"
		"\td52 -d filename x100\n"
		"\td52 /h filename d -x100\n\n");
	exit(GOOD_EXIT);
}

//
//		The Main Program
//

int main(int argc, char *argv[])
{
	char	c;
	int	count;
	char	*inp;
	int	line;
	char	tempstr[16];

#ifdef ALPHA
	printf("\nD52 8052 Disassembler V%d.%d.%d Alpha %d"
			 "\nCopyright (C) 1995-%d by J. L. Post%s",
		VERSION, MAJORREV, MINORREV, ALPHA, YEAR, licenseText);
#else
#ifdef BETA
	printf("\nD52 8052 Disassembler V%d.%d.%d Beta %d"
			 "\nCopyright (C) 1995-%d by J. L. Post%s",
		VERSION, MAJORREV, MINORREV, BETA, YEAR, licenseText);
#else
	printf("\nD52 8052 Disassembler V%d.%d.%d"
			 "\nCopyright (C) 1995-%d by J. L. Post%s",
		VERSION, MAJORREV, MINORREV, YEAR, licenseText);
#endif
#endif

	strcpy(defbstr, "db");		// init define byte and word strings
	strcpy(defwstr, "dw");
	strcpy(ascistr, "db");		// init define ascii string

	sym_tab = NULL;				// no symbols or labels yet
	lab_tab = NULL;
	name_tab = NULL;
	fp = NULL;
	fileflag = EITHERFILE;		// assume search for either file type
	hexflag = FALSE;				// no data in comment field
	upperflag = FALSE;
	baseflag = FALSE;
	traceflag = FALSE;
	offset = 0;						// default start at address 0
	ascii_flag = FALSE;
	keilflag = FALSE;
	incflag = FALSE;
	cycleflag = FALSE;			// no cycle counting by default
	cycle_r = NULL;				// cycles list is empty
	cycle_current = NULL;		// init current (last updated) cycle counter pointer
	cycle_exclude = FALSE;		// no exclusion from cycle counting until explicitly
										// given in control file

	if (argc < 2)
		usage();

//	find filename in command line

	for (line=1; line<argc; line++)	// find first non option string
	{
		inp = argv[line];
		c = *inp;

		if (c == '?')						// '?' without preceeding '-' or '/'
			usage();

		if (c == '-' || c == '/')
		{
			if (*(++inp) == '?')			// '?' following '-' or '/'
				usage();
		}
		else
		{										// assume first found is file name
			fileflag = parseFileName(argv[line], ".d52");
			break;
		}
	}

//	process command line options

	if (argc > 2)							// test for options
	{
		for (count=1; count<argc; count++)
		{
			inp = argv[count];			// to avoid modifying pointer in argv
			c = (char) toupper(*inp++);

			while (c)
			{
				if (c == '-' || c == '/')
					c = toupper(*inp++);		// skip over option specifier

				if (c == '?')
					usage();

				if (count == line)			// skip if already identified
					break;						// as the file name

				if (c == 'A')			// use ascii macro
				{
					strcpy(ascistr, "ascii");
					ascii_flag = TRUE;
				}
				else if (c == 'B' && fileflag == EITHERFILE)			// binary instead of hex file
				{
					fileflag = BINFILE;
					strcpy(src, baseFileName);
					strcat(src, ".bin");
				}
				else if (c == 'D')			// add data in comment field
					hexflag = TRUE;
				else if (c == 'H' && fileflag == EITHERFILE)			// force search for hex file
				{
					fileflag = HEXFILE;
					strcpy(src, baseFileName);
					strcat(src, ".hex");
				}
				else if (c == 'I')
					incflag = TRUE;
				else if (c == 'K')
					keilflag = TRUE;
				else if (c == 'N')
					baseflag = TRUE;
				else if (c == 'P')
					dotpseudo = 1;
				else if (c == 'S')			// change db/dw strings
				{
					strcpy(defbstr, "defb");
					strcpy(defwstr, "defw");
					if (!ascii_flag)
						strcpy(ascistr, "defb");
				}
				else if (c == 'T')
					traceflag = TRUE;
				else if (c == 'U')
					upperflag = TRUE;
				else if (c == 'X')			// add hex offset to program
				{
					offset = atox(inp);
					break;
				}
				else if (c == 'Z')
				{
					readcyclefile(inp);
					break;
				}

				c = (char) toupper(*inp++);
			}
		}
	}

	if (dotpseudo)
	{
		strcpy(tempstr, defbstr);
		defbstr[0] = '.';
		defbstr[1] = 0;
		strcat(defbstr, tempstr);

		strcpy(tempstr, defwstr);
		defwstr[0] = '.';
		defwstr[1] = 0;
		strcat(defwstr, tempstr);

		strcpy(tempstr, ascistr);
		ascistr[0] = '.';
		ascistr[1] = 0;
		strcat(ascistr, tempstr);

		strcpy(tempstr, equstr);
		equstr[0] = '.';
		equstr[1] = 0;
		strcat(equstr, tempstr);

		strcpy(tempstr, orgstr);
		orgstr[0] = '.';
		orgstr[1] = 0;
		strcat(orgstr, tempstr);
	}

//	allocate memory for program and flags

	if (!init_memory())
		exit(MEM_ERROR);

	for (count=0; count<32; count++)		// no register references yet
		dirregs[count] = 2;

	for ( ; count < 128; count++)
		dirregs[count] = 0;

	for (count=0; count<128; count++)
		sfrflags[count] = 0;

	for (count=0; count<128; count++)
		sbflags[count] = 0;

	for (count=0; count<128; count++)
		mbflags[count] = 0;

	getCTLoffset();					// get offset value from control file, if any
	readfile(src);						// read the disassembly file

//	Got the program in data array, now let's go to work...

	symbol_count = 0;
	label_count = 0;
	name_count = 0;
	comment_list = NULL;
	icomment_list = NULL;
	patch_list = NULL;

	if (traceflag)
	{
		printf("\nAnalyzing code...");

		if (analyzeCode("D52"))
			printf("done\n");
		else
			printf("\nAnalysis error!\n");
	}

	pass0();								// read control file
	pass1();								// find internal references
	pass2();								// disassemble to output file
	pass3();								// generate equ's for references
	printf("\nDone\n\n");			// go bye-bye
	return(GOOD_EXIT);
}		//  end of main

// end of d52.c

