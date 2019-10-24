
/*
 * D48 8048/8041 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * d48.h - 8048 disassembler definitions
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

#ifndef	_D48_H_
#define	_D48_H_

#define	CPU_BIG_ENDIAN	// 8048 is big endian processor

#ifndef _DEFS_H_
#include	"defs.h"
#endif

// option table values:

#define	OPT_XFER		0x80	// unconditional transfer
#define	OPT_INVAL	0x08	// invalid opcode
#define	OPT_EXT		0x04	// extended memory reference
#define	OPT_PAGE		0x02	// memory ref in current page
#define	OPT_IMM		0x01	// immediate data
#define	OPT_NONE		0x00	// single byte, no options

//
// Prototypes
//

void	usage(void);

//
// Global variables
//

extern byte	flag41;							// 8041 flag

#endif	// _D48_H_

