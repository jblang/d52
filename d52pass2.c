
/*
 * D52 8052 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * d52pass2.C - Disassembly pass 2
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
#include	"d52pass1.h"
#include	"d52pass2.h"
#include	"d52table.h"

//
//		Defined Constants
//
/* none */

//
//		Global variables
//
/* none */

//
// Pass two of disassembly. Rescan data array and generate output file.
// Generate label if location flagged as referenced by some other opcode.
// If un-initialized data is encountered, ignore it but set skip flag so
// that an ORG statement can be generated when the next initialized data
// is found.
//

void pass2(void)
{
	char		*cptr;
	int		skip, year;
	int		i, l, q, temp, oldpc;
	byte		j, k;
	int		pflag;
	time_t	tp;

	if (upperflag)			// convert SFR, register, etc text to upper case
	{
		for (i=0; i<128; i++)
			makeupper(sfr[i].dent);
		for (i=0; i<128; i++)
			makeupper(keilsfr[i].dent);
		for (i=0; i<128; i++)
			makeupper(sfrbits[i].dent);
		for (i=0; i<128; i++)
			makeupper(keilsfrbits[i].dent);
		for (i=0; i<128; i++)
			makeupper(membits[i].dent);
		for (i=0; i<128; i++)
			makeupper(keilmembits[i].dent);
		for (i=0; i<128; i++)
			makeupper(rbname[i].dent);
	}

	k = 0;
	j = 0xff;
	skip = TRUE;
	dump = FALSE;
	byte_cnt = 0;
	asc_cnt = 0;
	newline = FALSE;

	printf("\nPass 2 0000");

	fp = fopen(dst, "w");							// open output source file

	if (!fp)
	{
		printf("\n* Can't open output file %s *\n", dst);
		exit(FILE_ERROR);
	}
															// output header
	fprintf(fp, ";\n;  D52 V%d.%d.%d 8052 Disassembly of %s", VERSION, MAJORREV, MINORREV, src);

	time(&tp);									// get current time
	date_time = localtime(&tp);			// convert to hr/min/day etc
	year = date_time->tm_year + 1900;

	fprintf(fp, "\n;  %d/%02d/%02d %02d:%02d", year,
		date_time->tm_mon + 1,
		date_time->tm_mday,
		date_time->tm_hour,
		date_time->tm_min);

	if (incflag)
		fprintf(fp, "\n;\n\tinclude\t\"sfr52.inc\"");

	if (ascii_flag)					// if user specified A on command line...
	{
		fprintf(fp, "\n;\nascii\tmacro\tx");		// generate macro for text
		fprintf(fp, "\n\t%s\t'#x#'", defbstr);
		fprintf(fp, "\n\tendm");
		strcpy(ascistr, "ascii");
	}

//
//  Generate output file
//
	for (i=offset; i<=himark; )
	{
		if (pgmflags[i] & PF_CMT)
		{
			if (byte_cnt)							// dump any pending binary
				dump_bytes(i);

			if (asc_cnt)							// dump any accumulated ascii
				dump_ascii(i);

			output_comment(i);
		}

		if (pgmflags[i] & PF_PATCH)
		{
			if (byte_cnt)					// dump any pending binary
				dump_bytes(i);

			if (asc_cnt)					// dump any accumulated ascii
				dump_ascii(i);

			output_patch(i);
		}

		oldpc = i;
		l = i & 0xff;
		pflag = pgmflags[i];

		if (pflag & PF_NOINIT)					// ignore un-initialized data
		{
			if (byte_cnt)				// if binary or ascii data in buffers...
				dump_bytes(i);			// output them now

			if (asc_cnt)
				dump_ascii(i);

			if (dump)					// if we had to flush buffers...
			{								// do a newline, since we will
				dump = FALSE;			// be doing an org (or end)
				fprintf(fp, "\n;");
				newline = TRUE;
			}
			i++;							// next program location
			skip = TRUE;				// flag skip for ORG statement
			j = -1;
		}
		else if (pflag & (PF_ADRS | PF_WORD | PF_BYTE | PF_ASCII))
		{												// if not executable code
			if (!(j & 0x80) && !skip)
			{
				j = -1;
				fprintf(fp, "\n;");
				newline = TRUE;
			}

			switch (pflag & (PF_ADRS | PF_WORD | PF_BYTE | PF_ASCII))	// ignore irrelevant bits
			{
				case PF_ASCII:						// if ascii text...
					if (byte_cnt)					// dump any pending binary
						dump_bytes(i);

					if (skip)						// insert ORG statement
					{
						if (!newline)
							fprintf(fp, "\n;");

						fprintf(fp, "\n\t%s\t", orgstr);
						puthex(i);
						fprintf(fp, "\n;");
						newline = TRUE;
						skip = FALSE;				// reset skip flag
					}

					if (is_ascii(pgmmem[i]))	// if it really is ascii...
					{
						string[asc_cnt] = pgmmem[i];
						asc_cnt++;

						if (asc_cnt >= ASCLIMIT)
							dump_ascii(i + 1);
					}
					else								// else treat it as binary data
					{
						pgmflags[i] |= PF_BYTE;

						if (asc_cnt)				// dump any accumulated ascii

							dump_ascii(i);
						byte_data[byte_cnt] = pgmmem[i];	// add data to buffer
						byte_cnt++;
					}
					break;

				case PF_WORD:					// if word data...
					if (byte_cnt)			// dump any binary or ascii in buffers
						dump_bytes(i);

					if (asc_cnt)
						dump_ascii(i);

					if (skip)					// insert ORG statement
					{
						if (!newline)
							fprintf(fp, "\n;");

						fprintf(fp, "\n\t%s\t", orgstr);
						puthex(i);
						fprintf(fp, "\n;");
						newline = TRUE;
						skip = FALSE;			// reset skip flag
					}

					chk_label(i);							// add label if referenced
					q = ((int) pgmmem[i] & 0xff) << 8;	// get word value
					temp = pgmmem[i + 1] & 0xff;
					q |= temp;
					fprintf(fp, "%s\t", defwstr);
					cptr = find_entry(q, symbol_count, sym_val_index);	// see if symbol exists

					if (cptr == NULL)						// if not, do hex value
						puthex(q);
					else
						fprintf(fp, "%s", cptr);		// else output symbol

					if (hexflag)							// add comment field
					{
						fprintf(fp, "\t\t; %04x   %02x %02x      %c%c",
							i, pgmmem[i], pgmmem[i + 1], ascii(pgmmem[i]),
							ascii(pgmmem[i + 1]));
					}

					if (pgmflags[i] & PF_ICMT)
						output_icomment(i);

					i++;

					if (pgmflags[i + 2] != PF_ADRS && pgmflags[i + 2] != PF_WORD)
					{
						fprintf(fp, "\n;");
						newline = TRUE;
					}
					break;

				case PF_ADRS:					// if address data...
					if (byte_cnt)				// output any pending binary or
						dump_bytes(i);			// ascii data from buffers

					if (asc_cnt)
						dump_ascii(i);

					if (skip)					// insert ORG statement
					{
						if (!newline)
							fprintf(fp, "\n;");

						fprintf(fp, "\n\t%s\t", orgstr);
						puthex(i);
						fprintf(fp, "\n;");
						newline = TRUE;
						skip = FALSE;			// reset skip flag
					}

					chk_label(i);							// add label if referenced
					q = ((int) pgmmem[i]) << 8;	// get address value
					temp = pgmmem[i + 1] & 0xff;
					q |= temp;
					fprintf(fp, "%s\t", defwstr);
					cptr = find_entry(q, label_count, lab_val_index);
																// see if label exists
					if (cptr == NULL)						// if not, output hex
					{
						if (!pgmflags[i] & PF_LABEL)	// search symbol table only if not defined as label
							cptr = find_entry(q, symbol_count, sym_val_index);

						if (cptr == NULL)
						{
							if (!upperflag)
								fprintf(fp, "X%04x", q);
							else
								fprintf(fp, "X%04X", q);
						}
						else
							fprintf(fp, "%s", cptr);
					}
					else
						fprintf(fp, "%s", cptr);		// else output label text

					if (hexflag)							// do comment field
					{
						fprintf(fp, "\t\t; %04x   %02x %02x      %c%c",
							i, pgmmem[i], pgmmem[i + 1],
							ascii(pgmmem[i]), ascii(pgmmem[i + 1]));
					}

					if (pgmflags[i] & PF_ICMT)
						output_icomment(i);

					i++;

					if (pgmflags[i + 2] != PF_ADRS)
					{
						fprintf(fp, "\n;");
						newline = TRUE;
					}
					break;

				default:							// default = binary data...
					if (asc_cnt)				// output any pending ascii data
						dump_ascii(i);

					if (skip)					// insert ORG statement
					{
						if (byte_cnt)
							dump_bytes(i);

						if (!newline)
							fprintf(fp, "\n;");

						fprintf(fp, "\n\t%s\t", orgstr);
						puthex(i);
						fprintf(fp, "\n;");
						newline = TRUE;
						skip = FALSE;			// reset skip flag
					}

					byte_data[byte_cnt] = pgmmem[i];	// add data to buffer
					byte_cnt++;

					if (byte_cnt >= BYTELIMIT)			// if end of buffer...
						dump_bytes(i + 1);				// dump accumulated data
			}

			i++;									// next program location

			if (!(pgmflags[i] & PF_NOINIT) && (pgmflags[i] & PF_ASCII))
			{											// if next byte is flagged as
				if (!is_ascii(pgmmem[i]))		// ascii, but is not...
					pgmflags[i] |= PF_BYTE;		// then flag it as binary
			}
		}

//
// If previous data was an unconditional transfer, AND
// current location is not referenced by some other opcode, AND
// current byte is 0 or 0FFH, THEN
// treat this byte as un-initialized data.
//
		else if ((j & 0x80) && (!(pflag & PF_REF)) &&
			((pgmmem[i] == 0) || (pgmmem[i] == NO_DATA)) &&
			(!(pflag & PF_FORCE)))
		{
			if (byte_cnt)				// since we're going to skip some
				dump_bytes(i);			// data, output any pending ascii

			if (asc_cnt)				// or binary data remaining in buffers
				dump_ascii(i);

			if (dump)					// if ascii or binary output was done,
			{								// stick in a newline
				fprintf(fp, "\n;");
				dump = FALSE;
				newline = TRUE;
			}

			pgmflags[i] = PF_NOINIT;// flag as uninitialized data
			i++;
			skip = TRUE;
			byte_cnt = 0;
		}

//
// If previous opcode was 0 or 0ffH, AND current location is not
// referenced but is initialized, AND current opcode is 0 or 0ffH,
// un-initialize it.
//
		else if (((k == 0) || (k == 0xff)) &&
			(!(pflag & PF_REF)) &&
			((pgmmem[i] == 0) || (pgmmem[i] == NO_DATA)) &&
			!(pflag & PF_NOINIT) &&
			!(pflag & PF_FORCE))
		{
			pgmflags[i] = PF_NOINIT;
			i++;
			skip = TRUE;
			j = -1;
		}

		else								// IT'S EXECUTABLE CODE!
		{
			pgmflags[i] &= ~PF_NOINIT;	// clear for label search in pass 3

			if (byte_cnt)				// if any ascii or binary data remains
				dump_bytes(i);			// in the buffers, output them now

			if (asc_cnt)
				dump_ascii(i);

			if (dump)
			{
				fprintf(fp, "\n;");
				dump = FALSE;
				newline = TRUE;
			}

			byte_cnt = 0;

			if (cycleflag)				// cycle count summary
				cycle_end(i);

			if (skip)						// insert ORG statement
			{
				if (!newline)
					fprintf(fp, "\n;");

				fprintf(fp, "\n\t%s\t", orgstr);
				puthex(i);
				fprintf(fp, "\n;");
				newline = TRUE;
				skip = FALSE;				// reset skip flag
			}

			k = pgmmem[i];						// get opcode

			if (k == 0xa5 && !newline)		// if invalid opcode
			{
				fprintf(fp, "\n;");
				newline = TRUE;
			}

			chk_ref(i);							// add label if referenced
			kcnt = 8;
			j = opttbl[k];						// get options
			doopcode(mnemtbl[k].mnem);		// output mnemonic

//
// Generate operands
//
			switch (j & OPT_MASK)
			{
				case OPT_NONE:					// single byte - no options
					break;

				case OPT_IMM2:					// 2 byte immediate data
					q = (int) pgmmem[i + 1] & 0xff;

					if (pgmflags[i + 1] & PF_NAME)
						cptr = find_entry(i + 1, name_count, name_val_index);
					else
						cptr = find_entry(q, symbol_count, sym_val_index);

					if (cptr == NULL)
						puthex(q);
					else
						kcnt += fprintf(fp, "%s", cptr);

					splitcheck(i + 1);
					break;

				case OPT_LREF:					// 3 byte immediate data
					q = ((pgmmem[i + 1] & 0xff) << 8) | (pgmmem[i + 2] & 0xff);

					if (pgmflags[i + 1] & PF_NAME)
					{
						pgmflags[i + 1] |= PF_2BYTE;
						cptr = find_entry(i + 1, name_count, name_val_index);
					}
					else
					{
						cptr = find_entry(q, label_count, lab_val_index);

						if (cptr == NULL)
						{
							cptr = find_entry(q, symbol_count, sym_val_index);

							if (cptr == NULL)
							{
								if (!upperflag)
									kcnt += fprintf(fp, "X%04x", q);
								else
									kcnt += fprintf(fp, "X%04X", q);
							}
						}
					}

					if (cptr != NULL)
						kcnt += fprintf(fp, "%s", cptr);

					splitcheck(i + 1);
					splitcheck(i + 2);
					break;

				case OPT_DIR2:					// 2 byte direct addressing
					dodir(fp, i + 1);

					if ((k & 0xf) == 2 || k == 0xf5)
					{
						if (!upperflag)
							kcnt += fprintf(fp, ",a");
						else
							kcnt += fprintf(fp, ",A");
					}
					else if ((k & 0xf0) == 0x80)
					{
						kcnt += fprintf(fp, ",");

						if (k < 0x88)
						{
							if (!upperflag)
								kcnt +=  fprintf(fp, "@r%d", k & 1);
							else
								kcnt +=  fprintf(fp, "@R%d", k & 1);
						}
						else
						{
							if (!upperflag)
								kcnt += fprintf(fp, "r%d", k & 7);
							else
								kcnt += fprintf(fp, "R%d", k & 7);
						}
					}

					splitcheck(i + 1);
					break;

				case OPT_DIR3:					// mov dir,dir
					dodir(fp, i + 2);
					kcnt += fprintf(fp, ",");
					dodir(fp, i + 1);
					splitcheck(i + 1);
					splitcheck(i + 2);
					break;

				case OPT_DMM3:					// 3 byte direct and immediate addressing
					dodir(fp, i + 1);
					kcnt += fprintf(fp, ",#");
					q = (int) pgmmem[i + 2] & 0xff;

					if (pgmflags[i + 2] & PF_NAME)
						cptr = find_entry(i + 2, name_count, name_val_index);
					else
						cptr = find_entry(q, symbol_count, sym_val_index);

					if (cptr == NULL)
						puthex(q);
					else
						kcnt += fprintf(fp, "%s", cptr);

					splitcheck(i + 1);
					splitcheck(i + 2);
					break;

				case OPT_BIT2:					// 2 byte bit addressing
					dobit(fp, pgmmem[i + 1]);

					if (k == 0x92)
					{
						if (!upperflag)
							kcnt += fprintf(fp, ",c");
						else
							kcnt += fprintf(fp, ",C");
					}

					splitcheck(i + 1);
					break;

				case OPT_REL2:					// 2 byte relative addressing
					q = pgmmem[i + 1] & 0xff;
					q = (q > 0x7f) ? q | 0xff00 : q;
					q = i + 2 + q;
					q &= WORD_MASK;

					if (pgmflags[i + 1] & PF_NAME)
						cptr = find_entry(i + 1, name_count, name_val_index);
					else
						cptr = find_entry(q, label_count, lab_val_index);

					if (cptr == NULL)
					{
						if (!upperflag)
							kcnt += fprintf(fp, "X%04x", q);
						else
							kcnt += fprintf(fp, "X%04X", q);
					}
					else
						kcnt += fprintf(fp, "%s", cptr);

					splitcheck(i + 1);
					break;

				case OPT_IR3:					// 3 byte immediate and relative addressing
					q = (int) pgmmem[i + 1] & 0xff;

					if (pgmflags[i + 1] & PF_NAME)
						cptr = find_entry(i + 1, name_count, name_val_index);
					else
						cptr = find_entry(q, symbol_count, sym_val_index);

					if (cptr == NULL)
						puthex(q);
					else
						kcnt += fprintf(fp, "%s", cptr);

					q = pgmmem[i + 2] & 0xff;
					q = (q > 0x7f) ? q | 0xff00 : q;
					q = i + 3 + q;
					q &= WORD_MASK;

					if (pgmflags[i + 2] & PF_NAME)
						cptr = find_entry(i + 2, name_count, name_val_index);
					else
						cptr = find_entry(q, label_count, lab_val_index);

					if (cptr == NULL)
					{
						if (!upperflag)
							kcnt += fprintf(fp, ",X%04x", q);
						else
							kcnt += fprintf(fp, ",X%04X", q);
					}
					else
						kcnt += fprintf(fp, ",%s", cptr);

					splitcheck(i + 1);
					splitcheck(i + 2);
					break;

				case OPT_DR3:					// 3 byte direct and relative addressing
					dodir(fp, i + 1);
					q = pgmmem[i + 2] & 0xff;
					q = (q > 0x7f) ? q | 0xff00 : q;
					q = i + 3 + q;
					q &= WORD_MASK;

					if (pgmflags[i + 2] & PF_NAME)
						cptr = find_entry(i + 2, name_count, name_val_index);
					else
						cptr = find_entry(q, label_count, lab_val_index);

					if (cptr == NULL)
					{
						if (!upperflag)
							kcnt += fprintf(fp, ",X%04x", q);
						else
							kcnt += fprintf(fp, ",X%04X", q);
					}
					else
						kcnt += fprintf(fp, ",%s", cptr);

					splitcheck(i + 1);
					splitcheck(i + 2);
					break;

				case OPT_RELB:					// 3 byte bit and relative addressing
					dobit(fp, pgmmem[i + 1]);
					q = pgmmem[i + 2] & 0xff;
					q = (q > 0x7f) ? q | 0xff00 : q;
					q = i + 3 + q;
					q &= WORD_MASK;

					if (pgmflags[i + 2] & PF_NAME)
						cptr = find_entry(i + 2, name_count, name_val_index);
					else
						cptr = find_entry(q, label_count, lab_val_index);

					if (cptr == NULL)
					{
						if (!upperflag)
							kcnt += fprintf(fp, ",X%04x", q);
						else
							kcnt += fprintf(fp, ",X%04X", q);
					}
					else
						kcnt += fprintf(fp, ",%s", cptr);

					splitcheck(i + 1);
					splitcheck(i + 2);
					break;

				case OPT_112:					// 2 byte 11 bit adrs (ajmp & acall)
					q = ((k & 0xe0) << 3) | ((i + 2) & 0xf800);
					q = q | (pgmmem[i + 1] & 0xff);

					if (pgmflags[i + 1] & PF_NAME)
						cptr = find_entry(i + 1, name_count, name_val_index);
					else
						cptr = find_entry(q, label_count, lab_val_index);

					if (cptr == NULL)
					{
						if (!upperflag)
							kcnt += fprintf(fp, "X%04x", q);
						else
							kcnt += fprintf(fp, "X%04X", q);
					}
					else
						kcnt += fprintf(fp, "%s", cptr);

					splitcheck(i + 1);
					break;
			}

			if (k == 0xa5)				// if invalid opcode
				puthex(k);				// put hex defb in file

			if (cycleflag)				// do cycle counting and printout per instruction
			{
				while (kcnt < TSTOP)
				{
					fprintf(fp, "\t");
					kcnt = (kcnt + 8) & 0x78;
				}

				cycle_in(i, i + (opttbl[k] & OPT_SIZE) + 1,
					cycles[pgmmem[i] & 0xff], cycles2[pgmmem[i] & 0xff]);
			}

			if (hexflag)				// do comment field
			{
				while (kcnt < TSTOP)
				{
					fprintf(fp, "\t");
					kcnt = (kcnt + 8) & 0x78;
				}

				fprintf(fp,"; %04x   %02x", i, pgmmem[i] & 0xff);

				switch (opttbl[k] & OPT_SIZE)			// additional data bytes
				{
					case 0:
						fprintf(fp, "      ");
						break;

					case 1:
						fprintf(fp, " %02x   ", pgmmem[i + 1] & 0xff);
						break;

					case 2:
						fprintf(fp, " %02x %02x",
							pgmmem[i + 1] & 0xff, pgmmem[i + 2] & 0xff);
				}

				fprintf(fp, "   %c", ascii(pgmmem[i]));

				switch (opttbl[k] & OPT_SIZE)			// additional ascii
				{
					case 1:
						fprintf(fp, "%c", ascii(pgmmem[i + 1]));
						break;

					case 2:
						fprintf(fp, "%c%c",
							ascii(pgmmem[i + 1]), ascii(pgmmem[i + 2]));
				}
			}

			if (pgmflags[i] & PF_ICMT)
				output_icomment(i);

			newline = FALSE;
			i = i + (opttbl[k] & OPT_SIZE) + 1;	// update location counter

			if (j & OPT_XFER || k == 0xa5)	// if unconditional transfer or
			{											// invalid code, add a newline
				fprintf(fp, "\n;");
				newline = TRUE;
			}
		}

		if (i < oldpc)							// oops! wrapped around...
			break;

		if ((i & 0xff) < l)
			printf("\rPass 2 %04x", i & 0xff00);
	}

	if (cycleflag)						// any remaining cycle count summary
		cycle_end(PMEMSIZE + 1);	// specifying a number above any possible
											// maximum address makes cycle_end to emit all
											// remaining sums (i would probably suffice, too (?))

	if (byte_cnt)					// if any remaining ascii or binary,
		dump_bytes(i);				// output it now

	if (asc_cnt)
		dump_ascii(i);

	printf("\rPass 2 - Source generation complete");

//
// add equates for register names
//
	j = 0;

	for (i=0; i<128; i++)
	{
		if (dirregs[i] == 3)
		{
			if (!j)
			{
				if (!newline || dump)
					fprintf(fp, "\n;");

				fprintf(fp, "\n;\tRegister/Memory Equates\n;");
				newline = FALSE;
			}

			fprintf(fp, "\n%s\t%s\t", &rbname[i].dent[0], equstr);
			puthex(i);
			j = 1;
		}
	}

//
// add equates for SFR names
//
	j = 0;

	for (i=0; i<128; i++)
	{
		if (sfrflags[i] && isalpha(sfr[i].dent[0]))
		{
			if (!j)
			{
				if (!newline || dump)
					fprintf(fp, "\n;");

				fprintf(fp, "\n;\tSFR Equates\n;");
				newline = FALSE;
			}

			fprintf(fp, "\n%s\t%s\t", &sfr[i].dent[0], equstr);
			puthex(i + 0x80);
			j = 1;
		}
	}

//
// add equates for SFR bit names
//
	j = 0;

	for (i=0; i<128; i++)
	{
		if (sbflags[i] && isalpha(sfrbits[i].dent[0]))
		{
			if (!j)
			{
				if (!newline || dump)
					fprintf(fp, "\n;");

				fprintf(fp, "\n;\tSFR bit Equates\n;");
				newline = FALSE;
			}

			fprintf(fp, "\n%s\t%s\t", &sfrbits[i].dent[0], equstr);
			puthex(i + 0x80);
			j = 1;
		}
	}

//
// add equates for memory bit names
//
	j = 0;

	for (i=0; i<128; i++)
	{
		if (mbflags[i])
		{
			if (!j)
			{
				if (!newline || dump)
					fprintf(fp, "\n;");

				fprintf(fp, "\n;\tMemory bit Equates\n;");
				newline = FALSE;
			}

			if (!keilflag)
				fprintf(fp, "\n%s\t%s\t", &membits[i].dent[0], equstr);
			else
				fprintf(fp, "\n%s\t%s\t", &keilmembits[i].dent[0], equstr);

			puthex(i);
			j = 1;
		}
	}
}		// end of Pass 2

//
// Output operand for direct addressing
//

void dodir(FILE *fp, int adrs)
{
	int	dir;
	char	*cptr;

	dir = pgmmem[adrs];

	if (pgmflags[adrs] & PF_NAME)
	{
		cptr = find_entry(adrs, name_count, name_val_index);
		kcnt += fprintf(fp, "%s", cptr);
	}
	else if (dir < 0x80)
	{
		kcnt += fprintf(fp, "%s", rbname[dir].dent);
		dirregs[dir] |= 1;
	}
	else
	{
		if (keilflag)
			kcnt += fprintf(fp, "%s", keilsfr[dir & 0x7f].dent);
		else
			kcnt += fprintf(fp, "%s", sfr[dir & 0x7f].dent);
	}
}

//
// Output sfr bit name
//

void dobit(FILE *fp, int bit)
{
	bit &= 0xff;

	if (bit < 0x80)
	{
		if (!keilflag)
			kcnt += fprintf(fp, "%s", membits[bit].dent);
		else
			kcnt += fprintf(fp, "%s", keilmembits[bit].dent);
	}
	else
	{
		if (!keilflag)
			kcnt += fprintf(fp, "%s", sfrbits[bit & 0x7f].dent);
		else
			kcnt += fprintf(fp, "%s", keilsfrbits[bit & 0x7f].dent);
	}
}

// end of d52pass2.c

