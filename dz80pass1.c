
/*
 * Z80 Disassembler
 * Copyright (C) 1990-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * dz80pass1.c - Disassembly pass 1
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

#include	"dz80.h"
#include	"common.h"
#include	"dz80pass1.h"
#include	"dz80pass2.h"
#include	"dz80table.h"
#include	"d80table.h"

//
//	Defined Constants
//
  /* none */

//
//	Global variables
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
	int	i, l, pc, rel;
	byte	j, k, mask;

	printf("Pass 1 0000");

	for (i=offset; i<himark; )
	{
		l = i & 0xff;
		k = pgmmem[i];						// get stored opcode

		if ((pgmflags[i] & PF_NOINIT))	// ignore un-initialized data
			i++;
		else if (!(pgmflags[i] & (PF_ADRS | PF_WORD | PF_BYTE | PF_ASCII)))
		{										// if code...
			mask = (byte) PF_SPLIT;

			if (d8080)
				j = opttbl80[k];			// get option byte
			else
				j = opttbl[k];				// get option byte

			if (j & OPT_DIR)				// if absolute memory reference
			{
				pc = (pgmmem[i + 1] & 0xff) | ((pgmmem[i + 2] << 8) & 0xff00);

				if (pc >= offset && pc <= himark)
					mask |= PF_NOINIT;

				if (!(pgmflags[pc] & PF_NOLABEL))
					pgmflags[pc] = (pgmflags[pc] & ~mask) | PF_REF;	// flag reference
			}

			if (j & OPT_REL)				// if relative memory reference
			{
				j = pgmmem[i + 1];		// get offset
				rel = (j > 0x7f) ? j | 0xff00 : j & 0xff;	// sign extend offset
				pc = i + 2;
				pc += rel;
				pc &= WORD_MASK;

				if (pc >= offset && pc <= himark)
					mask |= PF_NOINIT;

				if (!(pgmflags[pc] & PF_NOLABEL))
					pgmflags[pc] = (pgmflags[pc] & ~mask) | PF_REF;	// flag reference
			}

			if (j & OPT_SPEC)				// if special processing
			{
				j = pgmmem[i + 1];		// get second byte

				switch (k)
				{
					case 0xed:
						if (edcode[j] & 2)
						{
							pc = (pgmmem[i + 2] & 0xff) |
								((pgmmem[i + 3] << 8) & 0xff00);

							if (pc >= offset && pc <= himark)
								mask |= PF_NOINIT;

							if (!(pgmflags[pc] & PF_NOLABEL))
								pgmflags[pc] = (pgmflags[pc] & ~mask) | PF_REF;
						}

						i += (edcode[j] & 3);
						break;

					case 0xdd:
					case 0xfd:
						if (j == 0x21 || j == 0x22 || j == 0x2a)
						{
							pc = (pgmmem[i + 2] & 0xff) |
								((pgmmem[i + 3] << 8) & 0xff00);

							if (pc >= offset && pc <= himark)
								mask |= PF_NOINIT;

							if (!(pgmflags[pc] & PF_NOLABEL))
								pgmflags[pc] = pgmflags[pc] | PF_REF;
						}

						i += (ddcode[j] & 3);
						break;
				}
			}

			if (d8080)
				i = i + (opttbl80[k] & OPT_SIZE) + 1;	// update location pointer
			else
				i = i + (opttbl[k] & OPT_SIZE) + 1;		// update location pointer
		}
		else
			i++;

		if ((i & 0xff) < l)
			printf("\rPass 1 %04x", i & 0xff00);
	}

	printf("\rPass 1 - Reference search complete");
}														//  End of Pass 1

// end of dz80pass1.c

