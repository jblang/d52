
/*
 * D52 8052 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * analyze52.c - 8052 disassembler code analyzer specific data and routines
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

#include	"d52.h"
#include	"common.h"
#include	"analyze.h"
#include	"analyze52.h"
#include	"d52table.h"

//#define	DEBUG

// Global variables

// Vector table of traceable addresses:
// IE0, TF0, IE1, TF1, and RI+TI

int	vectortable[] = {
	0x0003, 0x000b, 0x0013, 0x001b,
	0x0023, 0
};

int	dptrval = 0;
int	dstackPtr = 0;
int	dstack[STACK_DEPTH * 4];	// dptr references stack

// Code

// Attempt to determine the type of non-code data
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

						while ((data == pgmmem[pc]) && (pc < stop))
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
								for (j=0; j<dstackPtr; j++)
								{						// check against saved dptr values
									if (dstack[j] == begin)
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
								for (j=0; j<dstackPtr; j++)
								{										// see if it might be a table pointer
									if (dstack[j] == start)		// references some kind of table...
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
	byte	code, flag;
	int	adrs, dest, dptr;
	int	pushLevelSave = 0;
	int	dptrcount, stuffdptr;
	static int	dptrtrace = 1;

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

		switch (code)
		{
			case OPCODE_LJMP:
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				adrs = pgmmem[tpc++] << 8;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				adrs |= (pgmmem[tpc] & 0xff);
				adrs &= WORD_MASK;
				tpc = adrs;
				break;

			case OPCODE_JBC:
			case OPCODE_JB:
			case OPCODE_JNB:
				adrs = tpc + 3;
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				dest = (int) pgmmem[tpc++] & 0xff;

				if (dest & 0x80)
					dest |= 0xff00;

				adrs += dest;
				adrs &= WORD_MASK;
				astack[astackPtr++] = adrs;

				if (astackPtr >= STACK_DEPTH)
				{
					analysisWarning("trace stack overflow!");
					return TRUE;
				}

				break;

			case OPCODE_LCALL:
				pushLevelSave = pushLevel;
				pushLevel = 0;
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				adrs = pgmmem[tpc++] << 8;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				adrs |= (pgmmem[tpc++] & 0xff);
				adrs &= WORD_MASK;
				astack[astackPtr++] = tpc;

				if (astackPtr >= STACK_DEPTH)
				{
					analysisWarning("trace stack overflow!");
					return TRUE;
				}

				tpc = adrs;
				break;

		// Saving dptr on vector stack at a return instruction catches those
		// sequences whereby code is accessed through a table of vectors pointed
		// to by dptr and the address has been pushed on the 8052 stack.
		// A typical sequence might be:
		//
		//		mov	dptr,#jumptable		; accumulator is table index
		//		rl	a								; double it
		//		push	acc						; and save it
		//		movc	a,@a+dptr				; fetch 8 bits of address
		//		mov	r2,a						; save address
		//		inc	dptr
		//		pop	acc						; recover offset into table
		//		movc	a,@a+dptr				; get remainder of address
		//		push	acc
		//		mov	a,r2
		//		push	acc						; push address on stack
		//		ret								; go there

			case OPCODE_RET:
				if ((pushLevel > 0) && dptr)
				{
					pushLevel = pushLevelSave;
					vstack[vstackPtr++] = dptr;

					if (vstackPtr >= STACK_DEPTH)
					{
						analysisWarning("vector stack overflow!");
						return TRUE;
					}
				}

//				break;		// flow through to check astack

			case OPCODE_RETI:
				if (astackPtr)
				{
					--astackPtr;
					tpc = astack[astackPtr];
				}
				else
					done = TRUE;

				break;

			case OPCODE_JMPDPTR:		// jmp @a+dptr
				if (astackPtr)
				{
					--astackPtr;
					tpc = astack[astackPtr];
				}
				else
					done = TRUE;

				dptr = dptrval;

				if (dptr)
				{
					analysisFlags[dptr] = ANALYZE_CODE;

					code = pgmmem[dptr];		// see if this is a jump table

					switch (code)
					{
						case OPCODE_AJMP:
							while (pgmmem[dptr] == code)
							{
								analysisFlags[dptr++] = ANALYZE_CODE;
								adrs = pgmmem[dptr] & 0xff;
								analysisFlags[dptr++] = ANALYZE_CODE;
								adrs |= (dptr & 0xff);

								if ((adrs >= offset) && (adrs <= himark))
									analysisFlags[adrs] = ANALYZE_CODE;
							}

							break;

						case OPCODE_LJMP:
							while (pgmmem[dptr] == code)
							{
								analysisFlags[dptr++] = ANALYZE_CODE;
								adrs = pgmmem[dptr] << 8;
								analysisFlags[dptr++] = ANALYZE_CODE;
								adrs |= (pgmmem[dptr] & 0xff);
								analysisFlags[dptr++] = ANALYZE_CODE;
								dptr++;		// must be on even boundary

								if ((adrs >= offset) && (adrs <= himark))
									analysisFlags[adrs] = ANALYZE_CODE;
							}

							break;

						case OPCODE_SJMP:
							while (pgmmem[dptr] == code)
							{
								analysisFlags[dptr++] = ANALYZE_CODE;
								adrs = pgmmem[dptr];
								analysisFlags[dptr++] = ANALYZE_CODE;
								adrs += dptr;

								if ((adrs >= offset) && (adrs <= himark))
									analysisFlags[adrs] = ANALYZE_CODE;
							}

							break;
					}
				}

				break;

		// conditional jumps

			case OPCODE_JC:
			case OPCODE_JNC:
			case OPCODE_JZ:
			case OPCODE_JNZ:
				adrs = tpc + 2;
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				dest = (int) pgmmem[tpc++] & 0xff;

				if (dest & 0x80)
					dest |= 0xff00;

				adrs += dest;
				adrs &= WORD_MASK;
				astack[astackPtr++] = adrs;		// save jump address on analysis stack

				if (astackPtr >= STACK_DEPTH)
				{
					analysisWarning("trace stack overflow!");
					return TRUE;
				}

				break;

		// unconditional jump

			case OPCODE_SJMP:
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

			case OPCODE_PUSH:
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				tpc++;
				pushLevel++;
				break;

			case OPCODE_POP:
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				tpc++;

				if (pushLevel > 0)
					--pushLevel;

				break;

			case OPCODE_MOVDPTR:
				tpc++;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				dptr = pgmmem[tpc++] << 8;
				analysisFlags[tpc] = ANALYZE_TAGGED;
				dptr |= (pgmmem[tpc++] & 0xff);
				dptr &= WORD_MASK;
				dptrval = dptr;

				if (dptrtrace)
				{
					for (stuffdptr = 1, dptrcount=0; dptrcount < dstackPtr; dptrcount++)
					{
						if (dptr == dstack[dptrcount])
						{
							stuffdptr = 0;
							break;
						}
					}

					if (stuffdptr)		// treat dptr stack overflow as warning, not error
					{
						dstack[dstackPtr++] = dptr;

						if (dstackPtr >= STACK_DEPTH)
						{
							analysisWarning("dptr stack overflow!");
							dptrtrace = 0;
						}
					}
				}
			
				break;

			default:
				if ((code & OPCODE_AJMP_MASK) == OPCODE_AJMP)
				{
					tpc++;
					analysisFlags[tpc] = ANALYZE_TAGGED;
					adrs = tpc + 1;
					dest = ((int) code & 0xe0) << 3;
					dest += ((int) pgmmem[tpc++] & 0xff);
					dest &= WORD_MASK;
					adrs = dest;
					tpc = adrs;
				}
				else if ((code & OPCODE_ACALL_MASK) == OPCODE_ACALL)
				{
					pushLevelSave = pushLevel;
					pushLevel = 0;
					tpc++;
					analysisFlags[tpc] = ANALYZE_TAGGED;
					adrs = tpc + 1;
					dest = ((int) code & 0xe0) << 3;
					dest |= ((int) pgmmem[tpc++] & 0xff);
					dest &= WORD_MASK;
					adrs = dest;
					astack[astackPtr++] = tpc;

					if (astackPtr >= STACK_DEPTH)
					{
						analysisWarning("trace stack overflow!");
						return TRUE;
					}

					tpc = adrs;
				}
				else if ((code == OPCODE_DJNZ1) || (code >= OPCODE_CJNE1 && code <= OPCODE_CJNE2))
				{
					tpc++;
					analysisFlags[tpc] = ANALYZE_TAGGED;
					tpc++;
					analysisFlags[tpc] = ANALYZE_TAGGED;
					adrs = tpc + 1;
					dest = (int) pgmmem[tpc++] & 0xff;

					if (dest & 0x80)
						dest |= 0xff00;

					adrs += dest;
					adrs &= WORD_MASK;
					astack[astackPtr++] = tpc;

					if (astackPtr >= STACK_DEPTH)
					{
						analysisWarning("trace stack overflow!");
						return TRUE;
					}

					tpc = adrs;
				}
				else if ((code & OPCODE_DJNZ_MASK) == OPCODE_DJNZ)
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

					if (astackPtr >= STACK_DEPTH)
					{
						analysisWarning("trace stack overflow!");
						return TRUE;
					}

					tpc = adrs;
				}
				else
				{
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

				break;
		}
	}

	return FALSE;
}

// end of analyze52.c

