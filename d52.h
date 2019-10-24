
/*
 * D52 8052 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * d52.h - 8052 disassembler definitions
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

#ifndef _D52_H_
#define _D52_H_

#define	CPU_BIG_ENDIAN	// 8052 is big endian processor

#ifndef _DEFS_H_
#include	"defs.h"
#endif

//		Defined Constants

// opttbl bits:

#define	OPT_XFER		0x80	// unconditional transfer
#define	OPT_11		0x40	// 11 bit addressing
#define	OPT_REL		0x20	// relative addressing
#define	OPT_BIT		0x10	// bit addressing
#define	OPT_DIR		0x08	// direct addressing
#define	OPT_IMM		0x04	// immediate data
#define	OPT_3			0x02	// 3 byte instruction
#define	OPT_2			0x01	// 2 byte instruction

#define	OPT_MASK	(~OPT_XFER)				// options mask
#define	OPT_NONE	0							// single byte, no options
#define	OPT_SIZE	(OPT_3   | OPT_2)		// additional byte count
#define	OPT_LREF	(OPT_IMM | OPT_3)		// ljmp, lcall or mov dptr
#define	OPT_IMM2	(OPT_IMM | OPT_2)		// 2 byte immediate data
#define	OPT_DIR2	(OPT_DIR | OPT_2)		// 2 byte direct adrs
#define	OPT_DIR3	(OPT_DIR | OPT_3)		// 3 byte direct adrs
#define	OPT_DMM3	(OPT_DIR | OPT_IMM | OPT_3)	// 3 byte dir & imm
#define	OPT_BIT2	(OPT_BIT | OPT_2)		// 2 byte bit adrs
#define	OPT_REL2	(OPT_REL | OPT_2)		// 2 byte relative adrs
#define	OPT_IR3	(OPT_REL | OPT_IMM | OPT_3)	// 3 byte rel & imm
#define	OPT_DR3	(OPT_REL | OPT_DIR | OPT_3)	// 3 byte rel & dir
#define	OPT_RELB	(OPT_REL | OPT_BIT | OPT_3)	// 3 byte rel & bit
#define	OPT_112	(OPT_11  | OPT_2)		// 2 byte 11 bit adrs

//
//		Prototypes
//

extern void	usage(void);

//
//		Global Variables
//

extern int	keilflag;						// Keil A51 compatibility
extern int	incflag;							// include file flag

extern byte	dirregs[128];					// access flags for dir reg
extern byte	sfrflags[128];					// SFR name change flags
extern byte	sbflags[128];					// SFR bit name change flags
extern byte	mbflags[128];					// bit adrs mem name chg flgs

#endif	// _D52_H_

