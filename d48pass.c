
/*
 * D48 8048/8041 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * d48pass.c - Disassembly Passes 1 and 2
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

#include	"d48.h"
#include	"common.h"
#include	"d48pass.h"
#include	"d48table.h"

//
// Global variables
//
/* none */

//
// Pass one of disassembly
//
// Examine opcodes for internal references to other memory locations.
// If such references are found, flag the referenced location so that
// a label can be generated in the output file during pass two.
//

void pass1(void)
{
	int	i, j, pc, mbank;
	byte	k;
	int	mask;

	printf("\rPass 1");

	pc = mbank = 0;
	mask = (int) (PF_SPLIT | PF_MB0 | PF_MB1 | PF_FORCE);

	for (i=0; i<PMEMSIZE; )
	{
		if (!flag41)
		{
			if (pgmflags[i] & PF_MB0)		// check for forced memory bank selection
				mbank = 0;
			else if (pgmflags[i] & PF_MB1)
				mbank = 1;
		}

		if ((pgmflags[i] & PF_NOINIT) || (i > PMEMSIZE - 1))
			i++;							// ignore un-initialized data
											// ignore if last byte in address space
											// since second byte of opcode can't be
											// within the allowed address space
		else if (!(pgmflags[i] & (PF_ADRS | PF_WORD | PF_BYTE | PF_ASCII)))
		{									// if code...
			k = pgmmem[i];				// get stored opcode

			if (!flag41)
			{
				if (k == 0xe5)
					mbank = 0;				// modify if select bank code
				else if (k == 0xf5)
					mbank = 1;
			}

			j = optbl[k] & 0xe;			// get flags byte

			if (j == OPT_PAGE)			// if memory reference in current page
			{
				pc = i & 0xf00;						// get current page
				pc = pc | (pgmmem[i + 1] & 0xff);	// add address from opcode
				pgmflags[pc] = (pgmflags[pc] & ~mask) | PF_REF;	// flag reference
			}

			if (j == OPT_EXT)				/* if extended memory reference */
			{
				pc = (k & 0xe0) << 3;				// extract page number
				pc = pc | (pgmmem[i + 1] & 0xff);	// add address from opcode

				if (mbank)
					pc = pc | 0x800;

				if (pc < PMEMSIZE)		// flag reference
					pgmflags[pc] = (pgmflags[pc] & ~mask) | PF_REF;

				if (!flag41)
					mbank = (i < 2048) ? 0 : 1;
			}

			i = i + bctbl[k];				// update location pointer
		}
		else									// not executable code
			i++;

		if ((!flag41) && (i == 2048))
			mbank = 1;						// passing into upper bank
	}

	printf("\rPass 1 - Reference search complete");
}										//  End of Pass 1

//
// Pass two of disassembly
//
// Rescan data array and generate output file. Generate label if
// location flagged as referenced by some other opcode. If un-initialized
// data is encountered, ignore it but set skip flag so that an ORG
// statement can be generated when the next initialized data is found.
//

void pass2(void)
{
	char		*cptr;
	int		i, j, k, skip, mbank, year;
	int		q, temp;
	time_t	tp;

	j = -1;
	k = 0;
	mbank = 0;
	dump = 0;
	byte_cnt = 0;
	asc_cnt = 0;
	newline = FALSE;
	skip = TRUE;
	printf("\nPass 2");

	if ((fp = fopen(dst, "w")) == NULL)
	{
		printf("\n* Can't open file %s *\n", dst);
		exit(FILE_ERROR);
	}

	i = flag41 ? 8041 : 8048;
	fprintf(fp, ";\n;  D48 V%d.%d.%d %d Disassembly of %s", VERSION, MAJORREV, MINORREV, i, src);

	time(&tp);									// get current time
	date_time = localtime(&tp);			// convert to hr/min/day etc
	year = date_time->tm_year + 1900;

	fprintf(fp, "\n;  %d/%02d/%02d %02d:%02d", year,
		date_time->tm_mon + 1,
		date_time->tm_mday,
		date_time->tm_hour,
		date_time->tm_min);

	if (ascii_flag)					// if user specified A on command line...
	{
		if (upperflag)
		{
			fprintf(fp, "\n;\nASCII\tMACRO\tx");		// generate macro for text
			fprintf(fp, "\n\t%s\t'#x#'", defbstr);
			fprintf(fp, "\n\tENDM");
			strcpy(ascistr, "ASCII");
		}
		else
		{
			fprintf(fp, "\n;\nascii\tmacro\tx");		// generate macro for text
			fprintf(fp, "\n\t%s\t'#x#'", defbstr);
			fprintf(fp, "\n\tendm");
			strcpy(ascistr, "ascii");
		}
	}

//
//  Generate output file
//
	for (i=0; i<PMEMSIZE; )
	{
		if (!flag41)
		{
			if (pgmflags[i] & PF_MB0)		// check for forced memory bank selection
				mbank = 0;
			else if (pgmflags[i] & PF_MB1)
				mbank = 1;
		}

		if (pgmflags[i] & PF_CMT)
		{
			if (byte_cnt)					// dump any pending binary
				dump_bytes(i);

			if (asc_cnt)					// dump any accumulated ascii
				dump_ascii(i);

			output_comment(i);
			output_patch(i);
		}

		if (pgmflags[i] & PF_PATCH)
		{
			if (byte_cnt)					// dump any pending binary
				dump_bytes(i);

			if (asc_cnt)					// dump any accumulated ascii
				dump_ascii(i);

			output_patch(i);
		}

		if (pgmflags[i] & PF_NOINIT)	// ignore un-initialized data
		{
			if (byte_cnt)					// if binary or ascii data in buffers...
				dump_bytes(i);

			if (asc_cnt)
				dump_ascii(i);

			if (dump)
			{
				dump = 0;					// if we had to flush buffers...
				fprintf(fp, "\n;");		// do a newline, since we will
				newline = TRUE;			// be doing an org (or end)
			}

			i++;								// next program location
			skip = TRUE;					// flag skip for ORG statement
			j = -1;
		}
		else if (pgmflags[i] & (PF_ADRS | PF_WORD | PF_BYTE | PF_ASCII))
		{										// if not executable code
			if (!(j & 0x80) && !skip)
			{
				j = -1;
				fprintf(fp, "\n;");
				newline = TRUE;
			}

			switch (pgmflags[i] & (PF_ADRS | PF_WORD | PF_BYTE | PF_ASCII))	// ignore irrelevant bits
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
						skip = FALSE;
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
					if (byte_cnt)				// dump any binary or ascii in buffers
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

					chk_label(i);						// add label if referenced
					q = ((int) pgmmem[i] & 0xff) << 8;	// get word value
					temp = pgmmem[i + 1] & 0xff;
					q |= temp;
					fprintf(fp, "%s\t", defwstr);

					if (pgmflags[i] & PF_NAME)
						cptr = find_entry(i, name_count, name_val_index);
					else
						cptr = find_entry(q, symbol_count, sym_val_index);	// see if symbol exists

					if (cptr == NULL)						// if not, do hex value
						puthex(q);
					else
						fprintf(fp, "%s", cptr);		// else output symbol

					if (hexflag)							// add comment field
					{
						fprintf(fp, "\t\t; %04x - %02x %02x  %c%c",
							i, pgmmem[i] & 0xff, pgmmem[i + 1] & 0xff,
							ascii(pgmmem[i]) & 0xff, ascii(pgmmem[i + 1] & 0xff));
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

					chk_label(i);						// add label if referenced
					q = ((int) pgmmem[i]) << 8;	// get address value
					temp = pgmmem[i + 1] & 0xff;
					q |= temp;
					fprintf(fp, "%s\t", defwstr);
					cptr = find_entry(q, label_count, lab_val_index);	// see if label exists

					if (cptr == NULL)						// if not, output hex
					{
						cptr = find_entry(q, symbol_count, sym_val_index);

						if (cptr == NULL)
						{
							if (upperflag)
								fprintf(fp, "X%04X", q);
							else
								fprintf(fp, "X%04x", q);
						}
						else
							fprintf(fp, "%s", cptr);
					}
					else
						fprintf(fp, "%s", cptr);		// else output label text

					if (hexflag)							// do comment field
					{
						fprintf(fp, "\t\t; %04x   %02x %02x  %c%c",
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
			{										// if next byte is flagged as
				if (!is_ascii(pgmmem[i]))	// ascii, but is not...
					pgmflags[i] |= PF_BYTE;	// then flag it as binary
			}
		}

//
//	If previous data was an unconditional transfer, AND current
//	location is not referenced by some other opcode, AND current
//	byte is 0 or 0FFH, then treat this byte as un-initialized data.
//
		else if ((j & OPT_XFER) && (!(pgmflags[i] & PF_REF)) &&
				((!pgmmem[i]) || (pgmmem[i] == NO_DATA)) && !(pgmflags[i] & PF_FORCE))
		{
			if (byte_cnt)				// since we're going to skip some
				dump_bytes(i);			// data, output any pending ascii

			if (asc_cnt)				// or binary data remaining in buffers
				dump_ascii(i);

			if (dump)					// if ascii or binary output was done,
			{								// stick in a newline
				fprintf(fp, "\n;");
				dump = 0;
				newline = TRUE;
			}

			pgmflags[i] = PF_NOINIT;// flag as uninitialized data
			i++;
			skip = TRUE;
			byte_cnt = 0;
		}

//
//	If previous opcode was 0 or 0FFH, AND current location is not
//	referenced but is initialized, AND current opcode is 0 or 0ffh,
//	un-initialize it.
//
		else if (((k == 0) || (k == 0xff)) &&
			(!(pgmflags[i] & PF_REF)) &&
			((!pgmmem[i]) || (pgmmem[i] == 0xff)) &&
			!(pgmflags[i] & PF_NOINIT) &&
			!(pgmflags[i] & PF_FORCE))
		{
			pgmflags[i] = PF_NOINIT;	// flag as uninitialized data
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
				dump = 0;
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

			k = pgmmem[i] & 0xff;		// get opcode offset into table

			if ((!flag41) && (k == 0xe5))
				mbank = 0;					// set memory bank

			if ((!flag41) && (k == 0xf5))
				mbank = 1;

			j = optbl[k];

			if ((j & OPT_INVAL) && !newline)
			{
				fprintf(fp, "\n;");
				newline = TRUE;
			}

			chk_ref(i);						// add label if referenced
			kcnt = 8;
			doopcode(mnemtbl[k].mnem);	// output opcode

//
// Generate operands
//

			if (j == OPT_INVAL)			// if invalid opcode
				puthex(k);					// put hex defb in file

			else if (j == OPT_IMM)		// if immediate data
			{
				if (pgmflags[i + 1] & PF_NAME)
					cptr = find_entry(i + 1, name_count, name_val_index);
				else
					cptr = find_entry(pgmmem[i + 1], symbol_count, sym_val_index);

				if (cptr == NULL)
					puthex(pgmmem[i + 1] & 0xff);
				else
					kcnt += fprintf(fp, "%s", cptr);

				splitcheck(i + 1);		// test for split ref
			}
			else if ((j & 0xe) == OPT_PAGE)		// if page address
			{
				q = (i & 0xff00) | (pgmmem[i + 1] & 0xff);
				cptr = find_entry(q, label_count, lab_val_index);

				if (cptr == NULL)						// put address
				{
					if (upperflag)
						kcnt += fprintf(fp, "X%04X", q);
					else
						kcnt += fprintf(fp, "X%04x", q);
				}
				else
					kcnt += fprintf(fp, "%s", cptr);

				splitcheck(i + 1);
			}
			else if ((j & 0xe) == OPT_EXT)		// if extended address
			{
				q = ((k & 0xe0) << 3) | (pgmmem[i + 1] & 0xff);

				if (!flag41 && mbank)
					q |= 0x800;

				cptr = find_entry(q, label_count, lab_val_index);

				if (cptr == NULL)
				{
					if (upperflag)
						kcnt += fprintf(fp, "X%04X", q);
					else
						kcnt += fprintf(fp, "X%04x", q);
				}
				else
					kcnt += fprintf(fp, "%s", cptr);

				if (!flag41)
					mbank = (i < 2048) ? 0 : 1;

				splitcheck(i + 1);
			}

			if (cycleflag)				// do cycle counting
			{
				while (kcnt < TSTOP)
				{
					fprintf(fp, "\t");
					kcnt = (kcnt + 8) & 0x78;
				}

				cycle_in(i, i + bctbl[k], cycles[pgmmem[i] & 0xff], cycles2[pgmmem[i] & 0xff]);
			}

			if (hexflag)				// do comment field
			{
				while (kcnt < TSTOP)
				{
					putc('\t', fp);
					kcnt = (kcnt + 8) & 0x78;
				}

				fprintf(fp,"; %04x - %02x", i, pgmmem[i] & 0xff);

				if (bctbl[k] > 1)
					fprintf(fp, " %02x", pgmmem[i + 1] & 0xff);

				fprintf(fp, "\t%c", ascii(pgmmem[i]));

				if (bctbl[k] > 1)
					fprintf(fp, "%c", ascii(pgmmem[i + 1]));
			}

			if (pgmflags[i] & PF_ICMT)
				output_icomment(i);

			newline = FALSE;

			if ((j & OPT_XFER) | (j == OPT_INVAL))
			{									// if unconditional transfer or
				fprintf(fp, "\n;");		// invalid code, add a newline
				newline = TRUE;
			}

			i += bctbl[k];					// update location counter
		}

		if ((!flag41) && (i == 2048))
			mbank = 1;						// passing into upper bank
	}

	if (cycleflag)						// any remaining cycle count summary
		cycle_end(PMEMSIZE + 1);	// specifying a number above any possible
											// maximum address makes cycle_end to emit all
											// remaining sums (i would probably suffice, too (?))

	if (byte_cnt)							// if any remaining ascii or binary,
		dump_bytes(i);						// output it now

	if (asc_cnt)
		dump_ascii(i);

	printf("\rPass 2 - Source generation complete");
}

// end of d48pass.c
