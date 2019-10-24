
/*
 * Z80 Disassembler
 * Copyright (C) 1990-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * dz80.c - Main File
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
#include	<time.h>

#include	"dz80.h"
#include	"common.h"
#include	"analyze.h"
#include	"dz80pass1.h"
#include	"dz80pass2.h"
#include	"dz80table.h"
#include	"d80table.h"

//
// Global variables
//
int	d8080;			// 0 for z80, 1 for 8080, 2 for 8085
int	dotpseudo = 0;

#include	"dispass0.c"
#include	"dispass3.c"

void usage(void)
{
	printf("\nUsage: dz80 [options] <filename>\n"
		"Options may be entered Unix style (-d) or DOS style (/b)\n"
		"\t-a use ascii macro instead of db/defb for text.\n"
		"\t-b force .bin extension on input file.\n"
		"\t-c disassemble CP/M .com file (implies -x100).\n"
		"\t-d include address and data in comment field.\n"
		"\t-h force .hex extension on input file.\n"
		"\t   If neither 'b', 'c', nor 'h' is specified, DZ80 will first search\n"
		"\t   for a .hex file, and if not found, then a .bin file\n"
		"\t-n use C style for hexadecimal operands\n"
		"\t-p put dot '.' at beginning of pseudo ops\n"
		"\t-s change 'db' and 'dw' to 'defb' and 'defw'.\n"
		"\t-t trace and analyze code before disassembly.\n"
		"\t   (-t will overwrite any existing ctl file for the Z80 file\n"
		"\t    being disassembled.)\n"
		"\t-u output labels, symbols, and mnemonics in upper case.\n"
		"\t-x add hexadecimal offset to program addresses.\n"
		"\t-80 generate 8080 mnemonics.\n"
		"\t-85 generate 8085 mnemonics.\n"
		"\nOptions may be entered in a freeform fashion as long "
		"as a dash (-) or\n"
		"a slash (/) preceeds any option that preceeds the filename."
		"\nExamples:\n"
		"\tdz80 filename bd\n"
		"\tdz80 -d filename b\n"
		"\tdz80 /b -d filename\n\n");
	exit(GOOD_EXIT);
}

//
//  The Main Program
//

int main(int argc, char *argv[])
{
	char	c;
	int	count;
	char	*inp;
	int	line;
	char	tempstr[16];

#ifdef ALPHA
	printf("\nDZ80 Z80/8080/8085 Disassembler V %d.%d.%d Alpha %d"
			 "\nCopyright (C) 1990-%d by J. L. Post%s",
		VERSION, MAJORREV, MINORREV, ALPHA, YEAR, licenseText);
#else
#ifdef BETA
	printf("\nDZ80 Z80/8080/8085 Disassembler V %d.%d.%d Beta %d"
			 "\nCopyright (C) 1990-%d by J. L. Post%s",
		VERSION, MAJORREV, MINORREV, BETA, YEAR, licenseText);
#else
	printf("\nDZ80 Z80/8080/8085 Disassembler V %d.%d.%d"
			 "\nCopyright (C) 1990-%d by J. L. Post%s",
		VERSION, MAJORREV, MINORREV, YEAR, licenseText);
#endif
#endif

	if (argc < 2)
		usage();

	strcpy(defbstr, "db");				// init define byte and word strings
	strcpy(defwstr, "dw");
	strcpy(ascistr, "db");				// init define ascii string

	sym_tab = NULL;						// no symbols or labels yet
	lab_tab = NULL;
	name_tab = NULL;
	fp = NULL;
	fileflag = EITHERFILE;				// assume search for either file type
	hexflag = FALSE;						// no data in comment field
	upperflag = FALSE;
	baseflag = FALSE;
	traceflag = FALSE;
	offset = 0;								// default start at address 0
	ascii_flag = FALSE;
	d8080 = 0;

// find filename in command line

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
			fileflag = parseFileName(argv[line], ".z80");
			break;
		}
	}

// process command line options

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

				if (c == '8')
				{
					c = *inp++;

					if (c == '0')
					{
						d8080 = 1;
						strcpy(dst, baseFileName);
						strcat(dst, ".d80");
					}
					else if (c == '5')
					{
						d8080 = 2;
						strcpy(dst, baseFileName);
						strcat(dst, ".d85");
					}
					else
					{
						inp -= 2;
						printf("\n\nInvalid option: %c%c\n\n", *inp, *(inp + 1));
						inp++;
					}
				}
				else if (c == 'A')			// use ascii macro
				{
					strcpy(ascistr, "ascii");
					ascii_flag = TRUE;
				}
				else if ((c == 'B') && (fileflag == EITHERFILE))	// binary instead of hex file
				{
					fileflag = BINFILE;
					strcpy(src, baseFileName);
					strcat(src, ".bin");
				}
				else if (c == 'C' && fileflag == EITHERFILE)			// CP/M .com file
				{
					fileflag = CPMFILE;
					strcpy(src, baseFileName);
					strcat(src, ".com");
					offset = 0x100;
				}
				else if (c == 'D')			// add data in comment field
					hexflag = TRUE;
				else if ((c == 'H') && (fileflag == EITHERFILE))	// force search for hex file
				{
					fileflag = HEXFILE;
					strcpy(src, baseFileName);
					strcat(src, ".hex");
				}
				else if (c == 'N')
				{
					baseflag = TRUE;
					strcpy(mnemtbl[0xd7].mnem, "rst 0x10");
					strcpy(mnemtbl[0xdf].mnem, "rst 0x18");
					strcpy(mnemtbl[0xe7].mnem, "rst 0x20");
					strcpy(mnemtbl[0xef].mnem, "rst 0x28");
					strcpy(mnemtbl[0xf7].mnem, "rst 0x30");
					strcpy(mnemtbl[0xff].mnem, "rst 0x38");
				}
				else if (c == 'P')
					dotpseudo = 1;
				else if (c == 'S')			// change db/dw strings
				{
					strcpy(defbstr, "defb");
					strcpy(defwstr, "defw");
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

// allocate memory for program and flags

	if (!init_memory())
		exit(MEM_ERROR);

	getCTLoffset();					// get offset value from control file, if any
	readfile(src);						// read disassembly file

// Got the program in data array, now let's go to work...

	symbol_count = 0;
	label_count = 0;
	name_count = 0;
	comment_list = NULL;
	icomment_list = NULL;
	patch_list = NULL;

	if (traceflag)
	{
		printf("\nAnalyzing code...");

		if (analyzeCode("DZ80"))
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
}									//  End of Main

// end of dz80.c

