
/*
 * D52 8052 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * d52pass1.c - Disassembly pass 1
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
#include	"d52pass1.h"
#include	"d52pass2.h"
#include	"d52table.h"

//
// Pass one of disassembly
//
// Examine opcodes for internal references to other memory locations.
// If such references are found, flag the referenced location so that
// a label can be generated in the output file during pass two.
//

void pass1(void)
{
	int	i, l, pc, rel;
	byte	j, k, temp, mask;

	printf("\nPass 1 0000");

	for (i=offset; i<himark; )
	{
		l = i & 0xff;
		k = pgmmem[i];							// get stored opcode

		if (pgmflags[i] & PF_NOINIT)		// ignore un-initialized data
		{
			pc = i;
			i++;
			i &= WORD_MASK;
		}
		else if (!(pgmflags[i] & (PF_ADRS | PF_WORD | PF_BYTE | PF_ASCII)))
		{										// if code...
			mask = (byte) PF_SPLIT;
			j = opttbl[k];					// get option byte

			if ((j & OPT_MASK) == OPT_LREF)	// if ljmp, lcall, or mov dptr
			{
				pc = ((pgmmem[i + 1] & 0xff) << 8) | (pgmmem[i + 2] & 0xff);

				if (pc >= offset && pc <= himark)
					mask |= PF_NOINIT;

				pgmflags[pc] = (pgmflags[pc] & ~mask) | PF_REF;
															// flag reference
			}

			if (j & OPT_11)				// if 11 bit adrs (ajmp & acall)
			{
				pc = ((i + 2) & 0xf800) | ((k & 0xe0) << 3) | (pgmmem[i+1] & 0xff);

				if (pc >= offset && pc <= himark)
					mask |= PF_NOINIT;

				pgmflags[pc] = (pgmflags[pc] & ~mask) | PF_REF;
															// flag reference
			}

			if (j & OPT_REL)				// if relative reference
			{
				if (j & OPT_3)					// if 3 byte relative address
				{
					temp = pgmmem[i + 2];	// get offset
					rel = (temp > 0x7f) ? temp | 0xff00 : temp & 0xff;
					pc = i + 3;
				}
				else								// else 2 byte relative address
				{
					temp = pgmmem[i+1];				// get offset
					rel = (temp > 0x7f) ? temp | 0xff00 : temp & 0xff;
					pc = i + 2;
				}

				pc += rel;
				pc &= WORD_MASK;

				if (pc >= offset && pc <= himark)
					mask |= PF_NOINIT;

				pgmflags[pc] = (pgmflags[pc] & ~mask) | PF_REF;
														// flag reference
			}

			pc = i;
			i = i + (opttbl[k] & OPT_SIZE) + 1;		// update location pointer
			i &= WORD_MASK;
		}
		else
		{
			pc = i;
			i++;
			i &= WORD_MASK;
		}

		if (i < pc)							// oops! wrapped around...
			break;

		if ((i & 0xff) < l)
			printf("\rPass 1 %04x", i & 0xff00);
	}
	printf("\rPass 1 - Reference search complete");
}														//  End of Pass 1

// end of d52pass1.c

