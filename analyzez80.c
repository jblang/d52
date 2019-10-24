
/*
 * DZ80 Z80 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * analyzez80.c - Z80 disassembler code analyzer specific data and routines
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
#include	"analyzez80.h"
#include	"dz80table.h"

//#define	DEBUG

// Global variables

// Traceable reset and NMI vectors

int	vectortable[] = {
	0x0008, 0x0010, 0x0018, 0x0020,
	0x0028, 0x0030, 0x0038, 0x0066,
	0
};

// reset, rst, and nmi vector addresses
int	vectoradrstbl[] = {
	0x0001, 0x0009, 0x0011, 0x0019,
	0x0021, 0x0029, 0x0031, 0x0039,
	0x0067, 0
};
	
int	hlreg = 0, ixreg = 0, iyreg = 0;

#ifdef DEBUG
int pushLevelMax = 0;
#endif

int astackMax = 0;
int vstackMax = 0;

// Code

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

								while ((pc < PMEMSIZE) && (data == pgmmem[pc + 1]))
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

// Trace a single thread of code starting at address 'pc'.
// Return TRUE if error, else return FALSE.

bool trace(int pc)
{
	bool	done;
	byte	flag;
	int	code, adrs, dest, dptr;
	int	i, pushLevelSave = 0;

	if (!isTraceableCode(pc))		// does not appear to be executable code,
		return FALSE;					// but this is not an error

	tpc = pc;
	astackPtr = 0;
	pushLevel = 0;
	dptr = 0;
	done = FALSE;

	while (!done)
	{
		flag = analysisFlags[tpc];

		while (flag & ANALYZE_TAGGED)		// if we've already been here,
		{
			if (astackPtr)						// check for addresses on stack
			{
				--astackPtr;
				tpc = astack[astackPtr];
				flag = analysisFlags[tpc];
			}
			else									// if stack is empty, we're done
			{
				done = TRUE;
				break;
			}
		}

		if (done)
			return FALSE;

		code = pgmmem[tpc];
		analysisFlags[tpc] = ANALYZE_TAGGED;

		if (code == 0xcb || code == 0xdd || code == 0xed || code == 0xfd)
		{					// opcode prefix - two byte opcodes
			tpc++;
			code = (code << 8) | pgmmem[tpc];
			analysisFlags[tpc++] = ANALYZE_TAGGED;
			tpc -= 2;
		}

		switch (code)
		{
			case OPCODE_CALL:
			case OPCODE_CALLC:
			case OPCODE_CALLNC:
			case OPCODE_CALLZ:
			case OPCODE_CALLNZ:
			case OPCODE_CALLPE:
			case OPCODE_CALLPO:
			case OPCODE_CALLM:
			case OPCODE_CALLP:
				pushLevelSave = pushLevel;
				pushLevel = 0;
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				adrs = pgmmem[tpc++] & 0xff;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				adrs |= ((pgmmem[tpc++] & 0xff) << 8);
				adrs &= WORD_MASK;
				astack[astackPtr++] = tpc;

				if (astackPtr > astackMax)
					astackMax = astackPtr;

				if (astackPtr >= STACK_DEPTH)
				{
					analysisWarning("trace stack overflow!");
					return TRUE;
				}

				tpc = adrs;
				break;

			case OPCODE_RET:
			case OPCODE_RETI:
			case OPCODE_RETN:
				if (astackPtr)
				{
					--astackPtr;
					tpc = astack[astackPtr];
				}
				else
					done = TRUE;
				break;

			case OPCODE_JPHL:		// jp (hl) - we don't know where this will go
				if ((hlreg > offset) && (hlreg < himark))
				{
					vstack[vstackPtr++] = hlreg;

					if (vstackPtr > vstackMax)
						vstackMax = vstackPtr;

					if (vstackPtr >= STACK_DEPTH)
					{
						analysisWarning("vector stack overflow!");
						return TRUE;
					}
				}

				break;

			case OPCODE_JPIX:
				if ((ixreg > offset) && (ixreg < himark))
				{
					vstack[vstackPtr++] = ixreg;

					if (vstackPtr > vstackMax)
						vstackMax = vstackPtr;

					if (vstackPtr >= STACK_DEPTH)
					{
						analysisWarning("vector stack overflow!");
						return TRUE;
					}
				}

				break;

			case OPCODE_JPIY:
				if ((iyreg > offset) && (iyreg < himark))
				{
					vstack[vstackPtr++] = iyreg;

					if (vstackPtr > vstackMax)
						vstackMax = vstackPtr;

					if (vstackPtr >= STACK_DEPTH)
					{
						analysisWarning("vector stack overflow!");
						return TRUE;
					}
				}

				break;

			case OPCODE_JR:
				adrs = tpc + 2;
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				dest = (int) pgmmem[tpc++] & 0xff;

				if (dest & 0x80)
					dest |= 0xff00;

				adrs += dest;
				adrs &= WORD_MASK;
				tpc = adrs;
				break;

			case OPCODE_JRC:
			case OPCODE_JRNC:
			case OPCODE_JRZ:
			case OPCODE_JRNZ:
				adrs = tpc + 2;
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				dest = (int) pgmmem[tpc++] & 0xff;

				if (dest & 0x80)
					dest |= 0xff00;

				adrs += dest;
				adrs &= WORD_MASK;
				astack[astackPtr++] = adrs;

				if (astackPtr > astackMax)
					astackMax = astackPtr;

				if (astackPtr >= STACK_DEPTH)
				{
					analysisWarning("trace stack overflow!");
					return TRUE;
				}
				break;

			case OPCODE_JP:
				adrs = tpc + 3;
				analysisFlags[tpc + 1] = ANALYZE_TAGGED;
				analysisFlags[tpc + 2] = ANALYZE_TAGGED;
				tpc++;
				dest = (int) pgmmem[tpc++] & 0xff;
				dest |= (((int) pgmmem[tpc++] & 0xff) << 8);
				tpc = dest;
				break;

			case OPCODE_JPC:
			case OPCODE_JPNC:
			case OPCODE_JPZ:
			case OPCODE_JPNZ:
			case OPCODE_JPPE:
			case OPCODE_JPPO:
			case OPCODE_JPM:
			case OPCODE_JPP:
				adrs = tpc + 3;
				analysisFlags[tpc + 1] = ANALYZE_TAGGED;
				analysisFlags[tpc + 2] = ANALYZE_TAGGED;
				tpc++;
				dest = (int) pgmmem[tpc++] & 0xff;
				dest |= (((int) pgmmem[tpc++] & 0xff) << 8);
				astack[astackPtr++] = dest;

				if (astackPtr > astackMax)
					astackMax = astackPtr;

				if (astackPtr >= STACK_DEPTH)
				{
					analysisWarning("trace stack overflow!");
					return TRUE;
				}

				break;

			case OPCODE_PUSHAF:
			case OPCODE_PUSHBC:
			case OPCODE_PUSHDE:
			case OPCODE_PUSHHL:
				analysisFlags[tpc] = ANALYZE_TAGGED;
				tpc++;
				pushLevel++;

#ifdef DEBUG
				if (pushLevel > pushLevelMax)
					pushLevelMax = pushLevel;
#endif

				break;

			case OPCODE_PUSHIX:
			case OPCODE_PUSHIY:
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				tpc++;
				pushLevel++;

#ifdef DEBUG
				if (pushLevel > pushLevelMax)
					pushLevelMax = pushLevel;
#endif

				break;

			case OPCODE_POPAF:
			case OPCODE_POPBC:
			case OPCODE_POPDE:
			case OPCODE_POPHL:
				analysisFlags[tpc] = ANALYZE_TAGGED;
				tpc++;

				if (pushLevel > 0)
					--pushLevel;

				break;

			case OPCODE_POPIX:
			case OPCODE_POPIY:
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				tpc++;

				if (pushLevel > 0)
					--pushLevel;

				break;

			default:
				if (code == OPCODE_DJNZ)
				{
					tpc++;
					analysisFlags[tpc] = ANALYZE_TAGGED;
					adrs = tpc + 1;
					dest = (int) pgmmem[tpc++] & 0xff;

					if (dest & 0x80)
						dest |= 0xff00;

					adrs += dest;
					adrs &= WORD_MASK;
					astack[astackPtr++] = tpc;

					if (astackPtr > astackMax)
						astackMax = astackPtr;

					if (astackPtr >= STACK_DEPTH)
					{
						analysisWarning("trace stack overflow!");
						return TRUE;
					}

					tpc = adrs;
				}
				else if (code < 0x100)
				{
					if (code == OPCODE_LDHL)	// ld hl,nn
					{
						hlreg = pgmmem[tpc + 1] & 0xff;
						hlreg |= ((pgmmem[tpc + 2] & 0xff) << 8);
					}
					else if (code == OPCODE_LDHLI)	// ld hl,(nn)
					{
						dptr = pgmmem[tpc + 1] & 0xff;
						dptr |= ((pgmmem[tpc + 2] & 0xff) << 8);
						hlreg = pgmmem[dptr] & 0xff;
						hlreg |= ((pgmmem[dptr + 1] & 0xff) << 8);
					}
					else if (code == OPCODE_LDNNHL)	// ld (nn),hl
					{
						dptr = pgmmem[tpc + 1] & 0xff;
						dptr |= ((pgmmem[tpc + 2] & 0xff) << 8);
						i = 0;

						while (vectoradrstbl[i])
						{
							if (dptr == vectoradrstbl[i])
							{
								if ((hlreg >= offset) && (hlreg < himark))
								{
									astack[astackPtr++] = hlreg;

									if (astackPtr > astackMax)
										astackMax = astackPtr;

									if (astackPtr >= STACK_DEPTH)
									{
										analysisWarning("trace stack overflow!");
										return TRUE;
									}
								}

								break;
							}

							i++;
						}
					}

					if (opttbl[code] & OPT_2)
					{
						tpc++;
						analysisFlags[tpc] = ANALYZE_TAGGED;
						tpc++;
					}
					else if (opttbl[code] & OPT_3)
					{
						tpc++;
						analysisFlags[tpc] = ANALYZE_TAGGED;
						tpc++;
						analysisFlags[tpc] = ANALYZE_TAGGED;
						tpc++;
					}
					else
						tpc++;
				}
				else		// code >= 0x100
				{
					if (code == OPCODE_LDIX)		// ld ix,nn
					{
						ixreg = pgmmem[tpc + 1] & 0xff;
						ixreg |= ((pgmmem[tpc + 2] & 0xff) << 8);
					}
					else if (code == OPCODE_LDIXI)	// ld ix,(nn)
					{
						dptr = pgmmem[tpc + 1] & 0xff;
						dptr |= ((pgmmem[tpc + 2] & 0xff) << 8);
						ixreg = pgmmem[dptr] & 0xff;
						ixreg |= ((pgmmem[dptr + 1] & 0xff) << 8);
					}
					else if (code == OPCODE_LDIY)		// ld iy,nn
					{
						iyreg = pgmmem[tpc + 1] & 0xff;
						iyreg |= ((pgmmem[tpc + 2] & 0xff) << 8);
					}
					else if (code == OPCODE_LDIYI)	// ld iy,(nn)
					{
						dptr = pgmmem[tpc + 1] & 0xff;
						dptr |= ((pgmmem[tpc + 2] & 0xff) << 8);
						iyreg = pgmmem[dptr] & 0xff;
						iyreg |= ((pgmmem[dptr + 1] & 0xff) << 8);
					}

					tpc++;
					analysisFlags[tpc] = ANALYZE_TAGGED;
					tpc++;
				}

				break;
		}
	}

	return FALSE;
}

// end of analyzez80.c

