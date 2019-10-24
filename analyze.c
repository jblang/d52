
/*
 * DZ80 Z80 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * analyze.c - 8052/Z80 disassembler code analyzer common data and routines
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

// Common data and routines for 8052 and Z80 code analyzers

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>
#include	<time.h>

#include	"defs.h"
#include	"common.h"
#include	"analyze.h"

//#define	DEBUG

// Global variables

byte	*analysisFlags = NULL;

int	tpc;					// trace pc
int	pushLevel = 0;
int	astackPtr = 0;
int	astack[STACK_DEPTH];		// analysis stack, for returns and branches
int	vstackPtr = 0;
int	vstack[STACK_DEPTH];		// possible vector references stack
char	alertMessage[128];

#ifdef DEBUG
int pushLevelMax = 0;
#endif

FILE	*ctlfp;
int	listCount = 0;
char	fileName[256];
char	fileExt[128];
char	tempString[128];
char	dateString[64];

STRLIST	*ctlLineList;

// Code

#ifdef DEBUG
void dumpAnalysisFlags(void)
{
	int	i;

	fprintf(stderr, "\nAnalysis flags:\n");

	for (i=offset; i<himark; i++)
	{
		if (!(i & 0x0f))
			fprintf(stderr, "\n%04x: ", i);

		fprintf(stderr, " %02x", analysisFlags[i]);
	}

	fprintf(stderr, "\n\n");
	fflush(stderr);
}
#endif

/*	User has asked to analyze the program.

	Trace code from reset address (0) and all interrupt vector addresses,
	and flag all traced data as code [analyze()].

	Then attempt to identify the data type (code, binary, ascii)
	of any non-traced code, if it appears to be code, then trace from
	the code address [aPass1()].

	Now attempt to identify the type of any remaining data and
	flag what it appears to be in analysisFlags array [aPass2()].

	Finally, generate the output for the configuration file based on
	the data types identified and stored in the analysisFlags array
	[genAnalysisList()], and write it to the ctl file[writeCtlFile()].
*/

bool analyzeCode(char *dtext)
{
	int	i;

	if (!analysisFlags)
		analysisFlags = (byte *) malloc(PMEMSIZE);

	if (!analysisFlags)
	{
		analysisError("No memory for analysis flags\n");
		return FALSE;
	}

	for (i=0; i<PMEMSIZE; i++)
		analysisFlags[i] = ANALYZE_NONE;

	if (strlen(src))			// have to load a file before analyzing
	{
		if (createLineList(dtext) < 1)		// set up header for output list
		{
			analysisError("Can't init analysis data structures!\n");
			return FALSE;
		}

		if (analyze())			// trace code from reset and interrupt vectors
			return FALSE;

		if (aPass1())			// attempt to identify data types
			return FALSE;

		if (aPass2())			// aPass1() might have identified vectors, so check data again
			return FALSE;

#ifdef DEBUG
		dumpAnalysisFlags();
#endif

		genAnalysisList();	// generate control data
		writeCtlFile();		// write control data to ctl file
		deleteLineList();
	}
	else							// no source file selected
	{
		printf("\nNo file to analyze!\n");
		return FALSE;
	}

	return TRUE;
}

// Trace execution from reset address and interrupt vectors.
// Return TRUE if error, else return FALSE

bool analyze(void)
{
	int	i;
	int	dptr, vector, adrs;

	vstackPtr = 0;

	if (trace(offset))		// trace from beginning address
		return TRUE;

	if (offset == 0)
	{
		for (i=0; ; i++)
		{
			vector = vectortable[i];

			if (!vector)		// end of vector table
				break;

			if (trace(vector))
				return TRUE;
		}
	}

// Check the vector stack for possible code referenced as vectors.

	if (vstackPtr)
	{
		for (i=0; i<vstackPtr; i++)
		{
			dptr = vstack[i];

			if ((dptr > offset) && (dptr < himark))
			{
				while (1)
				{
					if (analysisFlags[dptr] != ANALYZE_NONE)
						break;

					adrs = dptr;
					vector = ((int) pgmmem[dptr++] << 8);
					vector |= ((int) pgmmem[dptr++] & 0xff);
					vector &= WORD_MASK;

					if ((vector > offset) && (vector < himark))
					{
						analysisFlags[adrs] = ANALYZE_VECTOR | ANALYZE_TRACED;
						analysisFlags[adrs + 1] = ANALYZE_VECTOR | ANALYZE_TRACED;

						if (trace(vector))
							return TRUE;
					}
					else
						break;

					if (dptr >= himark)
						break;
				}
			}
		}
	}

	return FALSE;
}

#if 0
// Attempt to determine the type of non-code data.
// (locations that haven't been traced).

bool aPass1(void)
{
	int	i, j;
	int	start, stop, pc;
	int	adrs, begin;
	byte	aflag, lastflag, data = 0, lastdata;
	char	code;

	pc = start = stop = begin = offset;
	lastflag = analysisFlags[offset];

	for (i=offset; i<himark; i++)
	{
		aflag = analysisFlags[i];

		if ((aflag != lastflag) || (i == himark - 1))	// we've got a type change, or end of code
		{
			stop = i - 1;
			code = 0;						// no output type yet

			switch (lastflag)
			{
				case ANALYZE_NONE:		// space not flagged, what kind of data is it?
					begin = start;
					pc = start;
					code = 'b';				// assume it's binary data

					while (pc <= stop)			// guess at data type for this block
					{
						if (analysisFlags[pc] != ANALYZE_NONE)	// already tagged,
						{								// end loop and tag previous
							pc = stop;				// block as last identified type
							break;
						}

						if (isString(pc, stop))	// check if it might be ascii data
						{								// if so, find end of the string
							code = 't';
							pc = getEndOfString(pc, stop);
							data = pgmmem[pc];

							if (!data)				// add terminator if it's text
								pc++;
						}
						else
						{
							start = pc;
							data = pgmmem[pc];

							if (((data == 0) || (data == 0xff)) && (data == pgmmem[pc + 1]))
							{
								code = 'i';

								while (data == pgmmem[pc + 1])
								{
									analysisFlags[pc] = ANALYZE_IGNORE;
									pc++;
								}

								if (pc < PMEMSIZE)
									analysisFlags[pc] = ANALYZE_IGNORE;
							}
							else
							{
								code = 'd';
								pc++;
							}
						}

						if (start < pc - 1)			// dump what we've accumulated
						{
							for (adrs=start; adrs<pc; adrs++)
							{							// flag all data in current block
								switch (code)
								{
									case 'b':
										analysisFlags[adrs] = ANALYZE_BINARY | ANALYZE_TRACED;
										break;

									case 't':
										analysisFlags[adrs] = ANALYZE_ASCII | ANALYZE_TRACED;
										break;

									case 'a':
										analysisFlags[adrs] = ANALYZE_VECTOR | ANALYZE_TRACED;
										break;

									case 'i':
										analysisFlags[adrs] = ANALYZE_IGNORE | ANALYZE_TRACED;
										break;
								}
							}
						}

						while (data == pgmmem[pc] && (pc < stop))
							pc++;				// skip over redundant data

						start = pc;
						data = pgmmem[pc];
						code = 'b';			// assume next data is binary

						if (isprint(data) && data != '"')
						{						// check for ascii text following
							for (j=1; j<MIN_STR_LEN; j++)
							{
								data = pgmmem[pc + j];

								if (!isprint(data) || data == '"')
									break;
							}

							if (j >= MIN_STR_LEN)	// if at min ascii chars in
								code = 't';				// a row, tag the block as text
										
							data = pgmmem[pc];
						}

						if (analysisFlags[start + 1] == ANALYZE_NONE)
						{							// check for possible vector in data
							adrs = (int) pgmmem[start] << 8;
							adrs |= ((int) pgmmem[start + 1] & 0xff);
							adrs &= WORD_MASK;

// D52 uses dstack here, DZ80 uses vstack

							if ((adrs > offset) && (adrs < himark))
							{						// if it might be a vector...
								for (j=0; j<vstackPtr; j++)
								{						// check against saved dptr values
									if (vstack[j] == begin)
									{					// seems to be a valid vector
										analysisFlags[start] = ANALYZE_VECTOR | ANALYZE_TRACED;
										analysisFlags[start + 1] = ANALYZE_VECTOR | ANALYZE_TRACED;

										if (trace(adrs))		// trace the code
											return TRUE;

										code = 0;
										pc = start + 1;
										data = pgmmem[pc];
										start = pc + 1;
									}
								}
							}
							else if (start == begin)	// apparently not a vector
							{
								for (j=0; j<vstackPtr; j++)
								{										// see if it might be a table pointer
									if (vstack[j] == start)		// references some kind of table...
									{
										pc = start;
										lastdata = ANALYZE_NONE;

										while (pc <= stop && lastdata == ANALYZE_NONE)
										{								// check the data in the table
											adrs = (int) pgmmem[pc] << 8;	// might be a vector in
											adrs |= ((int) pgmmem[pc + 1] & 0xff);	// the table
											adrs &= WORD_MASK;

											if ((adrs > offset) && (adrs < himark))		// looks like a vector
											{
												analysisFlags[pc] = ANALYZE_VECTOR | ANALYZE_TRACED;
												pc++;
												lastdata |= analysisFlags[pc];
												analysisFlags[pc] = ANALYZE_VECTOR | ANALYZE_TRACED;

												if (analysisFlags[adrs] == ANALYZE_NONE)
												{
													if (trace(adrs))
														return TRUE;
												}
											}
											else							// not a vector, must be binary
											{
												analysisFlags[pc] = ANALYZE_BINARY | ANALYZE_TRACED;
											}

											pc++;
											lastdata |= analysisFlags[pc];
											lastdata |= analysisFlags[pc + 1];
										}

										if (analysisFlags[pc] == ANALYZE_NONE)
											analysisFlags[pc] = ANALYZE_BINARY | ANALYZE_TRACED;
									}
								}
							}
						}

						if (isprint(data) && (data != '"'))	// see if it might be a text string
						{
							if (code != 't')
							{
								for (j=1; j<MIN_STR_LEN; j++)
								{
									data = pgmmem[pc + j];

									if (!isprint(data) || data == '"')
										break;
								}

								if (j >= MIN_STR_LEN)		// yup, looks like a text string
								{
									code = 't';

									for (adrs=start+1; adrs<pc; adrs++)		// tag previous as binary
									{
										if (analysisFlags[adrs] == ANALYZE_NONE)
											analysisFlags[adrs] = ANALYZE_BINARY | ANALYZE_TRACED;
									}

									start = pc;
								}
							}
						}
						else		// not text, must be binary data
						{
							if (code != 'b')
							{
								code = 'b';

								if (start >= pc)
									analysisFlags[start] = ANALYZE_BINARY | ANALYZE_TRACED;	// start might be >= pc
								else
								{
									for (adrs=start+1; adrs<pc; adrs++)		// tag previous as ascii
									{
										if (analysisFlags[adrs] == ANALYZE_NONE)
											analysisFlags[adrs] = ANALYZE_ASCII | ANALYZE_TRACED;
									}
								}

								start = pc;
							}
						}

						pc++;
					}				// end of while (pc <= stop)

					if (pc > i)
						i = pc;

					code = 0;
					break;

				case (ANALYZE_CODE + ANALYZE_TRACED):
					code = 'c';
					break;

				case (ANALYZE_VECTOR + ANALYZE_TRACED):
					code = 'a';
					break;
			}

			lastflag = aflag;
			start = i;
		}			// end of if ((aflag != lastflag) || (i == himark - 1))
	}				// end of for (i=offset; i<himark; i++)

	return FALSE;
}

#endif

// Check anything not yet identified.

bool aPass2(void)
{
	int	i, j;
	bool	tagged;
	int	start, begin, adrs;
	byte	data, lastdata;

	start = begin = offset;
	j = TRUE;
	analysisFlags[himark - 1] = ANALYZE_END;

	for (i=offset; i<himark; i++)
	{
		if (analysisFlags[i] == ANALYZE_NONE)	// not yet identified data
		{
			if (j)				// if previous block identified...
			{
				start = i;		// save start adrs of this block
				j = FALSE;		// flag not identified
			}
		}
		else						// data identified as something...
		{
			if (!j)				// if previous block not identified...
			{
				begin = start;	// then set beginning adrs
				lastdata = 0;

				while (begin < i)
				{
					data = pgmmem[begin];

					if ((data == 0 || data == 0xff) && (lastdata == 0 || lastdata == 0xff))
					{								// two 00 or ff in a row
						start = begin;
						lastdata = data;		// skip over invalid data

						while ((data == 0 || data == 0xff) && start < i)
						{
							start++;
							data = pgmmem[start];
						}

						if (begin < start - 1)
							begin = start;
					}
					else							// data not 00 or ff
					{
						lastdata = 1;			// find end of non 00 or ff data

						while ((data != 0 || data != 0xff || lastdata != 0 || lastdata != 0xff) && (begin < i))
						{
							lastdata = data;
							data = pgmmem[begin];
							begin++;

							if (analysisFlags[begin] != ANALYZE_NONE)
								break;

							if ((lastdata == 0 && data == 0) || (lastdata == 0xff && data == 0xff))
								break;
						}

						while (start < begin)
						{
							tagged = FALSE;

							if (analysisFlags[start] == ANALYZE_NONE && analysisFlags[start + 1] == ANALYZE_NONE)
							{
								adrs = (int) pgmmem[start] << 8;	// might be a vector
								adrs |= ((int) pgmmem[start + 1] & 0xff);
								adrs &= WORD_MASK;

								if ((adrs > offset) && (adrs < himark))		// looks like a vector
								{
									if (analysisFlags[adrs] == ANALYZE_NONE || analysisFlags[adrs] == ANALYZE_TAGGED)
									{
										analysisFlags[start] = ANALYZE_VECTOR | ANALYZE_TRACED;
										analysisFlags[start + 1] = ANALYZE_VECTOR | ANALYZE_TRACED;

										if (analysisFlags[adrs] == ANALYZE_NONE)
											trace(adrs);

										start++;
										tagged = TRUE;
									}
								}

								if (!tagged)
									analysisFlags[start] = ANALYZE_BINARY | ANALYZE_TRACED;
							}
							else
								analysisFlags[start] = ANALYZE_BINARY | ANALYZE_TRACED;

							start++;
						}

						start = begin;
					}

					begin++;
				}

				start = i;
				j = TRUE;
			}
		}
	}

	return FALSE;
}

// Check if data at 'pc' appears to be an ascii string.
// Return TRUE if so, else return FALSE.

bool isString(int pc, int stop)
{
	int	i;
	bool	retval = FALSE;
	byte	data;

	data = pgmmem[pc];

	if (isprint(data) && data != '"')
	{
		for (i=0; i<MIN_STR_LEN && pc<stop; i++)
		{
			data = pgmmem[pc++];

			if (!isprint(data) || data == '"')
				break;
		}

		if (i >= MIN_STR_LEN)	// if at least min ascii chars in
			retval = TRUE;			// a row, then might be ascii text
	}

	return retval;
}

// Return address of end of string at 'pc'.
// Assumes that pc does point to a string.

int getEndOfString(int pc, int stop)
{
	byte	data;

	data = pgmmem[pc];

	while (isprint(data) && data != '"' && pc < stop)
	{
		pc++;
		data = pgmmem[pc];
	}

	return pc;
}

// Check if code at 'pc' appears to be traceable.
// Return TRUE if so, else return FALSE.
// We assume it's not valid code if it's a series
// of TRACE_CHECK_LEN bytes of 0x00 or 0xff in a row.

bool isTraceableCode(int pc)
{
	int	i;
	bool	retval = FALSE;
	byte	code;

	for (i=0; i<TRACE_CHECK_LEN; i++)
	{
		code = pgmmem[pc++];

		if (code != 0x00 && code != 0xff)
		{
			retval = TRUE;
			break;
		}
	}

	return retval;
}

// Generate list to be written to control file.
// Scan through the program and add list entries
// for each data type (code, ascii, etc).

void genAnalysisList(void)
{
	int	i;
	int	start, stop;
	byte	aflag, lastflag, data;
	char	code, ostr[64], datatype[32];

	aflag = lastflag = analysisFlags[0];
	start = offset;

	if (offset > 1)
	{
		sprintf(ostr, "i 0000-%04x\t; Invalid data", offset - 1);
		addListEntry(ostr);
	}

	for (i=offset; i<himark; i++)
	{
		aflag = analysisFlags[i];
		data = pgmmem[i];

		if (aflag != lastflag)
		{
			stop = i - 1;
			datatype[0] = '\0';

			switch (lastflag & ~ANALYZE_TRACED)
			{
				case ANALYZE_NONE:
					if ((data != 0) && (data != 0xff))
					{
						code = 'c';
						sprintf(datatype, "\t; Unknown - assumed code space");
					}
					else
						code = 0;
					break;

				case ANALYZE_CODE:
					code = 'c';
					sprintf(datatype, "\t; Code space");
					break;

				case ANALYZE_VECTOR:
					code = 'a';
					sprintf(datatype, "\t; pointers");
					break;

				case ANALYZE_BINARY:
					code = 'b';
					sprintf(datatype, "\t; 8-bit data");
					break;

				case ANALYZE_ASCII:
					code = 't';
					sprintf(datatype, "\t; ASCII text");
					break;

				case ANALYZE_IGNORE:
					code = 'i';
					sprintf(datatype, "\t; ignore data");
					break;

				default:
					code = '?';
					sprintf(datatype, "\t; unknown data type");
					break;
			}

			if (stop >= start)
			{
				if (stop > start)
					sprintf(ostr, "%c %04x-%04x", code, start, stop);
				else
					sprintf(ostr, "%c %04x\t", code, start);

				strcat(ostr, datatype);
				addListEntry(ostr);
			}

			start = i;
			lastflag = aflag;
		}
	}
}

// Pass "D52" or "DZ80" in dtext

int createLineList(char *dtext)
{
	int		year;
	time_t	tp;

	if (ctlLineList)
		deleteLineList();

	ctlLineList = malloc(sizeof(STRLIST));

	if (!ctlLineList)
	{
		printf("No memory for line list.\n");

		if (ctlfp)
		{
			fclose(ctlfp);
			ctlfp = NULL;
		}
		return -1;
	}

	ctlLineList->str = NULL;
	ctlLineList->prev = NULL;
	ctlLineList->next = NULL;
	addListEntry(";");

	strcpy(tempString, "; ");
	strcat(tempString, dtext);
	strcat(tempString, " configuration file for ");
	strcat(tempString, src);
	addListEntry(tempString);

	sprintf(tempString, "; Generated by %s V%d.%d.%d on ", dtext, VERSION, MAJORREV, MINORREV);

	time(&tp);									// get current time
	date_time = localtime(&tp);			// convert to hr/min/day etc
	year = date_time->tm_year + 1900;

	sprintf(dateString, "%d/%02d/%02d %02d:%02d",
		year,
		date_time->tm_mon + 1,
		date_time->tm_mday,
		date_time->tm_hour,
		date_time->tm_min);

	strcat(tempString, dateString);
	addListEntry(tempString);
	addListEntry(";");

	if (offset)
	{
		sprintf(tempString, "o %04x\t\t; program offset", offset);
		addListEntry(tempString);
	}

	return listCount;
}

void deleteLineList(void)
{
	STRLIST	*list, *prev;

	list = ctlLineList;

	while (list)
	{
		prev = list;
		list = list->next;
		free(prev->str);
		free(prev);
	}

	ctlLineList = NULL;
	listCount = 0;
}

int writeCtlFile(void)
{
	STRLIST	*list;
	char		*str = NULL;

	list = ctlLineList;

	if (!list)
	{
		if (ctlfp)
		{
			fclose(ctlfp);
			ctlfp = NULL;
		}
		return -1;
	}

	if (ctlfp)
	{
		fclose(ctlfp);
		strcpy(fileName, ctl);
		strcat(fileName, ".backup");
		rename(ctl, fileName);
	}

	ctlfp = fopen(ctl, "w");		// reopen ctl file for writing

	if (!ctlfp)
		return -1;

	while (list)
	{
		str = list->str;

		if (str && *str)
			fprintf(ctlfp, "%s\n", str);

		list = list->next;
	}

	fflush(ctlfp);
	fclose(ctlfp);
	ctlfp = NULL;
	return 0;
}

bool addListEntry(char *str)
{
	STRLIST	*list = ctlLineList;

	if (!list)
		return FALSE;

	while (list->next)
		list = list->next;

	list->next = malloc(sizeof(STRLIST));

	if (!list->next)
		return FALSE;

	list->next->prev = list;
	list = list->next;
	list->next = NULL;
	list->str = malloc(strlen(str) + 2);

	if (!list->str)
		return FALSE;

	strcpy(list->str, str);
	listCount++;

	return TRUE;
}

void analysisWarning(char *msg)
{
	sprintf(alertMessage, "\nAnalysis warning - ");
	strcat(alertMessage, msg);
	printf("%s\n", alertMessage);
}

void analysisError(char *msg)
{
	sprintf(alertMessage, "\nAnalysis incomplete - ");
	strcat(alertMessage, msg);
	printf("%s\n", alertMessage);
}

// end of analyze.c

