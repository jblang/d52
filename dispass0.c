
/*
 * Pass 0 for Disassemblers
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
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

//
// Read control file, if it exists, and flag areas as code, text,
// or whatever. Also handle labels, symbols, etc.
//
// Some strange things happen here with oring and anding on the
// flag bits; this is so the user can define something as code,
// data, or whatever, and assign a label for the same location.
// We should handle the result intelligently regardless of the
// order in which such things are found in the control file.
//

#include	"common.h"

void pass0(void)
{
	int		i;
	char		*text, func, c, *ltext;
	int		start, stop, code, temp;
	FILE		*fpc;
	SYM_PTR	sym;
	CYCLE_RANGE_PTR	cr, cx, cy, cn;
	int		found;

	if (upperflag)
	{
		makeupper(defbstr);
		makeupper(defwstr);
		makeupper(ascistr);
		makeupper(orgstr);
		makeupper(equstr);
	}

	fpc = fopen(ctl, "r");

	if (!fpc)
		return;

	if (fpc != NULL)			// if control file exists...
	{
		printf("\nReading control file   ");

		while (!feof(fpc))					// until end of file...
		{
			start = stop = 0;
			*linebuffer = '\0';				// clear previous line
			text = fgets(linebuffer, MAX_LINE - 1, fpc);	// read one line
			i = 0;

			if (!text)
				break;

			while (linebuffer[i] && linebuffer[i] != '\n')
				i++;

			linebuffer[i] = '\0';
			text = &linebuffer[1];

			while (isgraph(*text))			// skip remaining chars in first word
				text++;

			text = get_adrs(text, &start);

			while (1)
			{
				c = *text++;
				if (c != ' ' && c != '\t')	// skip whitespace
					break;
			}

			if (c == '\n' || c == ';')		// if only one numeric...
				--text;							// back up to newline

			func = c;							// save operator
			ltext = text;
			--ltext;
			text = get_adrs(text, &stop);

			if (func == '+')					// check for valid operator
				stop += (start - 1);
			else if (func == '-' && !stop)
				stop = start;

			switch (toupper(linebuffer[0]))
			{
				case 'A':								// address
					if (start < PMEMSIZE && stop < PMEMSIZE)
					{
						do
						{						// get address to reference
#ifdef	CPU_BIG_ENDIAN
							code = (pgmmem[start]) << 8;
							temp = pgmmem[start + 1] & 0xff;
#else
							code = pgmmem[start] & 0xff;
							temp = (pgmmem[start + 1] & 0xff) << 8;
#endif
							code |= temp;

							if (code < PMEMSIZE)
							{
								pgmflags[code] |= PF_REF;		// flag referenced address
								pgmflags[code] &= ~PF_SPLIT;
								pgmflags[start] |= PF_ADRS;	// set flags to adrs code
								pgmflags[start++] &=
									~(PF_NOINIT | PF_WORD | PF_BYTE | PF_ASCII | PF_SPLIT);
								pgmflags[start] |= PF_ADRS;
								pgmflags[start++] &=
									~(PF_NOINIT | PF_WORD | PF_BYTE | PF_ASCII | PF_SPLIT);
							}
							else
							{
								pgmflags[start++] |= PF_WORD;
								pgmflags[start++] |= PF_WORD;
							}
						} while (start < stop);
					}
					else
						printf("\rinvalid address specified: %x, %x\n",
									start, stop);
					break;

				case 'B':								// byte binary
					if (start < PMEMSIZE && stop < PMEMSIZE)
					{
						do
						{
							pgmflags[start] |= PF_BYTE;
							pgmflags[start++] &=
								~(PF_NOINIT | PF_ADRS | PF_WORD | PF_ASCII | PF_SPLIT);
						} while (start <= stop);
					}
					else
						printf("\rinvalid address specified: %x, %x\n",
									start, stop);
					break;

				case 'C':								// code
					if (start < PMEMSIZE && stop < PMEMSIZE)
					{
						do
						{
							pgmflags[start] &=
#ifdef _D48_H_
								~(PF_NOINIT | PF_ADRS | PF_WORD | PF_BYTE | PF_ASCII | PF_SPLIT | PF_MB0 | PF_MB1);
#else
								~(PF_NOINIT | PF_ADRS | PF_WORD | PF_BYTE | PF_ASCII | PF_SPLIT);
#endif
							pgmflags[start++] |= PF_FORCE;
						} while (start <= stop);
					}
					else
						printf("\rinvalid address specified: %x, %x\n",
									start, stop);
					break;

				case 'D':								// define data type
					switch (toupper(*ltext))
					{
						case '0':
						case 'L':
							pgmflags[start] |= PF_LABEL;		// 0 or (l)abel = search label table only
							break;

						case '1':
						case 'S':
							pgmflags[start] |= PF_SYMBOL;		// 1 or (s)ymbol = search symbol table only
							break;

						case '2':
						case 'N':
							pgmflags[start] |= (PF_LABEL | PF_SYMBOL);	// 2 or (n)one = don't search either table
							break;

						default:
							printf("\nInvalid data type flag: %s\n", linebuffer);
							break;
					}

					break;

#ifdef	EXTENDED_MEM
#ifndef _D48_H_

				case 'E':								// extended memory specification
					// incomplete
					break;
#endif
#endif

#ifdef _D52_H_

				case 'F':								// modify SFR name
					if (start < 0x80 || start > 0xff)
					{
						printf("\rInvalid SFR address: 0x%x in '%s'\n",
									start, (char *) &linebuffer[2]);
					}
					else
					{
						start &= 0x7f;

						for (stop=0; stop<15; stop++)	// transfer new name
						{
							func = *ltext++;

							if (isgraph(func))
								sfr[start].dent[stop] = func;
							else
							{
								sfr[start].dent[stop] = '\0';	// terminate name
								break;
							}
						}

						sfrflags[start] = 1;

						if (stop >= 15)
							sfr[start].dent[15] = '\0';
					}
					break;

#endif

				case 'I':								// ignore
					if (start < PMEMSIZE && stop < PMEMSIZE)
					{
						do
						{
							pgmflags[start++] = PF_NOINIT;
						} while (start <= stop);
					}
					else
						printf("\rinvalid address specified: %x, %x\n",
									start, stop);
					break;

#ifdef _D52_H_

				case 'K':								// modify SFR bit name
					if (start < 0x80 || start > 0xff)
					{
						printf("\rInvalid SFR bit address: 0x%x in '%s'\n",
									start, (char *) &linebuffer[2]);
					}
					else
					{
						start &= 0x7f;

						for (stop=0; stop<15; stop++)	// transfer name
						{
							func = *ltext++;

							if (isgraph(func))
								sfrbits[start].dent[stop] = func;
							else
							{
								sfrbits[start].dent[stop] = '\0';
								break;
							}
						}

						sbflags[start] = 1;

						if (stop >= 15)
							sfrbits[start].dent[15] = '\0';
					}
					break;

#endif

				case 'L':								// label
					if (start < PMEMSIZE)
					{
						pgmflags[start] |= PF_REF;			// flag reference
						pgmflags[start] &= ~PF_SPLIT;

						if (isgraph(*ltext))
							add_entry(start, ltext, LABEL_TYPE);
					}
					else
						printf("\rinvalid address specified: %x, %x\n",
									start, stop);
					break;

#ifdef _D48_H_

				case 'M':								// force memory bank selection
					get_adrs(ltext, &stop);

					if (start)
						pgmflags[stop] |= PF_MB1;
					else
						pgmflags[stop] |= PF_MB0;
					break;

#endif

#ifdef _D52_H_

				case 'M':								// modify memory bit name
					if (start > 0x7f)
					{
						printf("\rInvalid memory bit address: 0x%x in '%s'\n",
									start, (char *) &linebuffer[2]);
					}
					else
					{
						for (stop=0; stop<15; stop++)	// transfer name
						{
							func = *ltext++;

							if (isgraph(func))
							{
								if (!keilflag)
									membits[start].dent[stop] = func;
								else
									keilmembits[start].dent[stop] = func;
							}
							else
							{
								if (!keilflag)
									membits[start].dent[stop] = '\0';
								else
									keilmembits[start].dent[stop] = '\0';
								break;
							}
						}

						mbflags[start] = 1;

						if (stop >= 15)
						{
							if (!keilflag)
								membits[start].dent[15] = '\0';
							else
								keilmembits[start].dent[15] = '\0';
						}
					}
					break;

#endif

				case 'N':
					pgmflags[start] |= PF_NOLABEL;
					break;

				case 'O':								// hex offset for program
					if (!offset)
						offset = start;
					break;

				case 'P':								// patch code (add inline code)
					pgmflags[start] |= PF_PATCH;		// flag address
					text = get_adrs((char *) &linebuffer[1], &start);
					add_patch(start, text);
					break;

#ifdef _D48_H_

				case 'R':								// register name
					if (start > 15)
					{
						printf("\rInvalid register address: 0x%x in '%s'\n",
						linebuffer[0], linebuffer);
					}
					else
					{
						for (stop=0; stop<7; stop++)	// transfer register name
						{
							func = *ltext++;

							if (upperflag)
								func = toupper(func);

							if (isalnum(func))
								rbname[start].dent[stop] = func;
							else
							{
								rbname[start].dent[stop] = '\0';
								break;
							}
						}
					}

					if (stop >= 7)
						rbname[start].dent[7] = '\0';	// null terminate reg name
					break;

#endif

#ifdef _D52_H_

				case 'R':								// modify register name
					if (start > 0x7f)
					{
						printf("\rInvalid register/memory address: 0x%x in '%s'\n",
									start, (char *) &linebuffer[2]);
					}
					else
					{
						dirregs[start] |= 2;

						for (stop=0; stop<15; stop++)	// transfer register name
						{
							func = *ltext++;

							if (upperflag)
								func = toupper(func);

							if (isgraph(func))
								rbname[start].dent[stop] = func;
							else
							{
								rbname[start].dent[stop] = '\0';
								break;
							}
						}

						if (stop >= 15)
							rbname[start].dent[15] = '\0';
					}
					break;

#endif

				case 'S':								// symbol
					add_entry(start, ltext, SYMBOL_TYPE);
					break;

				case 'T':								// ascii text
					do
					{
						pgmflags[start] |= PF_ASCII;
						pgmflags[start++] &=
							~(PF_NOINIT | PF_ADRS | PF_WORD | PF_BYTE | PF_SPLIT);
					} while (start <= stop);
					break;

#ifdef	EXTENDED_MEM
#ifndef _D48_H_

				case 'U':								// use extended memory translation
					// incomplete
					break;
#endif
#endif

				case 'W':								// word binary
					if (start < PMEMSIZE && stop < PMEMSIZE)
					{
						do
						{
							pgmflags[start] |= PF_WORD;
							pgmflags[start++] &=
								~(PF_NOINIT | PF_ADRS | PF_BYTE | PF_ASCII | PF_SPLIT);
						} while (start <= stop);
					}
					else
						printf("\rinvalid address specified: %x, %x\n",
									start, stop);
					break;

				case 'X':								// operand name
					pgmflags[start] |= PF_NAME;
					sym = add_entry(start, ltext, NAME_TYPE);
					sym->used = FALSE;
					break;

				case 'Y':								// operand name, no equ generation
					pgmflags[start] |= PF_NAME;
					sym = add_entry(start, ltext, NAME_TYPE);
					sym->used = -1;
					break;

				case 'Z':								// cycle counting
					if ((cn = (CYCLE_RANGE_PTR) malloc(sizeof(struct cycle_range))) == NULL)
						exit(1);		// error handling !!!

					cn->cnt = 0;			// fill up with default
					cn->cnt2 = 0;
					cn->min = start;
					cn->max = stop;
					cn->mul = 1;
					cn->val = 0;			// the explicit one-off value, see '=' below
					cn->child = NULL;
					cn->parent = NULL;
					cn->next = NULL;
						// patch the single-num-range--and-third-parameter cases
						// (we are unable to parse the xxxx- case, that would need to
						// rewrite a portion of parsing above which we don't want for
						// backwards compatibility reasons - let's document it, making
						// a "feature" from the bug... :-)

					if (func == '*')
					{
						cn->mul = stop;
						cn->max = stop = start;
					}
					else if (func == '=')
					{
						cn->val = stop;
						cn->max = stop = start;
					}
							// get the third numeral - I've simply stolen Jeff's code from above... -- JW
					while (1)
					{
						c = *text++;

						if (c != ' ' && c != '\t')	// skip whitespace
							break;
					}

					if (c == '\n' || c == ';')		// if no more arguments
						--text;							// back up to newline

					if (c == '*')
						text = get_adrs(text, &cn->mul);
					else if (c == '=')
						text = get_adrs(text, &cn->val);
					else if (c == '^')	// indicate maximum overdrive,
						cn->cnt2 = 2;		// sorry, "worst case" ("jump always taken")
					else if (c == '-')
						cn->mul = 0;		// zero multiplier to indicate "exclude from
												// overall count" - for the "not taken" branch

					if (stop > PMEMSIZE)
						stop = PMEMSIZE;					// sanity

					if (stop < start)						// sanity
						exit(1); //!!! error handling !!!

					cx = NULL;		// now let's find, where could this range fit
					cy = NULL;
					cr = cycle_r;

					do
					{
						found = 0;

						while ((!found) && (cr != NULL))
						{
							if ((start >= cr->min) && (stop <= cr->max))
							{			// our new range would fit into this one
								cx = cr;
								found = 1;
							}
							else if ((start <= cr->min) && (stop >= cr->max))
							{			//our new range eats this one
								if (cy == NULL)	// init conditon - only one range is there yet
									cycle_r = cn;
								else if (cy->child == cr)
								{
									cy->child = cn;
									cn->parent = cx;
								}
								else {
									cy->next = cn;		// first swap in the new for the old
									cn->parent = cy->parent;
								}

								cn->child = cr;		// the old is now child of the new
								cr->parent = cn;

								while ((cx = cr->next) != NULL)
								{		// determine, siblings of old will be siblings or children of new
									if ((start <= cx->min) && (stop >= cx->max))
									{		// remains a child - but child of cn!
										cx->parent = cr->parent;
									}
									else if ( ((cx->min < start) && (cx->max > start) && (cx->max < stop)) || ((cx->min > start) && (cx->min < stop) && (cx->max > stop)) ) {  //pathologic partial overlap
										exit(1); //!!! error handling !!!
									}
									else	// goes outside cn range so it's sibling - the
											// 4th option is impossible from context
									{
										cr->next = cx->next;		// detach from old chain
										cx->next = cn->next;
										cx->parent = cn->parent;
										cn->next = cx;				// insert into new chain
									}

									cr = cx;				// and go for the next
								}

								found = 2;	// when finished, get out of the whole new
												// range handling the shortest way
							}
							else if ( ((cr->min < start) && (cr->max > start) &&
								(cr->max < stop)) || ((cr->min > start) &&
								(cr->min < stop) && (cr->max > stop)) )
							{					// pathologic partial overlap
								exit(1);		// !!! error handling !!!
							}
							else
							{
								cy = cr;
								cr = cy->next;	// none of those - ignore - but let cy
													// always point to the currently probed one
							}
						}

						if (found == 1)
						{
							cy = cx;
							cr = cx -> child;
						}
					} while (found == 1);

					if (found == 0)	// this means the new range fits into an old ...
					{						// cx contains the would-be parent
						if (cy == NULL)	// special case: first record of the day... :-)
							cycle_r = cn;
						else if (cx == NULL)	// no matching record in the topmost level
							cy->next = cn;
						else
						{
							cn->next = cx->child;	// insert as a new child of cx
							cn->parent = cx;
							cx->child = cn;
						}
					}

					cycleflag = TRUE;		// oh yes, we almost forgot to flip on the
												// main cycle counting switch...
					break;

				case 0x00:								// ignore empty lines
				case '\n':
				case '\r':
				case '\t':
				case ' ':
				case ';':								// ignore commented out lines
					break;

				case '#':								// comment string
					pgmflags[start] |= PF_CMT;		// flag address
					text = get_adrs((char *) &linebuffer[1], &start);
					add_comment(start, text);
					break;

				case '!':								// inline comment string
					pgmflags[start] |= PF_ICMT;
					text = get_adrs((char *) &linebuffer[1], &start);
					add_icomment(start, text);
					break;

				default:						// somebody didn't read the docs...
					printf("\nUnknown control code: 0x%02x in '%s'\n",
						linebuffer[0], linebuffer);
					break;
			}
		}

		if (label_count || symbol_count || name_count)	// set up tail node for sort
		{
			tail_ptr = (struct sym *) malloc(sizeof(struct sym));

			if (tail_ptr == NULL)
			{
				printf("\nNo memory for symbol pointers!\n");
				exit(MEM_ERROR);
			}

			tail_ptr->next = tail_ptr;
			tail_ptr->name = malloc(4);
			tail_ptr->name[0] = 0xfe;		// set max values for sort
			tail_ptr->name[1] = 0;
			tail_ptr->val = 0xfffff;		// set an invalid value to mark end of list
		}

		if (label_count)						// if labels encountered...
		{
			lab_tab_last->next = tail_ptr;	// set up pointer array for sort
			lab_val_index = malloc(sizeof(SYM_PTR) * label_count);

			if (lab_val_index == NULL)
			{
				printf("\nNo memory for label pointers");
				exit(MEM_ERROR);
			}

			lab_tab = sort(lab_tab, lab_val_index, label_count);

			if (!lab_tab)
				exit(USER_ERROR);
		}

		if (symbol_count)						// if symbols encountered...
		{
			sym_tab_last->next = tail_ptr;
			sym_val_index = malloc(sizeof(SYM_PTR) * symbol_count);

			if (sym_val_index == NULL)
			{
				printf("\nNo memory for symbol pointers");
				exit(MEM_ERROR);
			}

			sym_tab = sort(sym_tab, sym_val_index, symbol_count);

			if (!sym_tab)
				exit(USER_ERROR);
		}

		if (name_count)						// if operand symbols encountered...
		{
			name_tab_last->next = tail_ptr;
			name_val_index = malloc(sizeof(SYM_PTR) * name_count);

			if (name_val_index == NULL)
			{
				printf("\nNo memory for operand name pointers!\n");
				exit(MEM_ERROR);
			}

			name_tab = sort(name_tab, name_val_index, name_count);

			if (!name_tab)
				exit(USER_ERROR);
		}

		fclose(fpc);
	}
	else
		printf("No control file found\n\n");
}

// end of dispass0.c
