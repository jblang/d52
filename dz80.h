
/*
 * Z80 Disassembler
 * Copyright (C) 1990-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * dz80.h - Z80 disassembler definitions
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

#ifndef	_DZ80_H_
#define	_DZ80_H_

#define	CPU_LITTLE_ENDIAN	// Z80 is little endian processor

#ifndef _DEFS_H_
#include	"defs.h"
#endif

//
//	Defined Constants
//

// opttbl bits:

#define	OPT_XFER		0x80	// unconditional transfer
#define	OPT_SPEC		0x40	// special codes (cd, dd, ed, fd)
#define	OPT_REL		0x20	// relative addressing
#define	OPT_DIR		0x10	// direct addressing (jp, call)
#define	OPT_PAR		0x08	// parenthesized address
#define	OPT_IMM		0x04	// immediate data
#define	OPT_3			0x02	// 3 byte instruction
#define	OPT_2			0x01	// 2 byte instruction
#define	OPT_NONE		0x00	// single byte, no options

#define	OPT_SIZE				(OPT_2 | OPT_3)
#define	OPT_IMM2				(OPT_IMM | OPT_2)
#define	OPT_PAR2				(OPT_PAR | OPT_2)
#define	OPT_DIR_IMM3		(OPT_DIR | OPT_IMM | OPT_3)
#define	OPT_PAR_IMM3		(OPT_PAR | OPT_DIR_IMM3)
#define	OPT_REL2				(OPT_REL | OPT_2)
#define	OPT_SPEC2			(OPT_SPEC | OPT_2)

// ED option bits:

#define	OPT_ED_2			0x01	// 2 byte, entry in edtbl
#define	OPT_ED_STORE	0x03	// write 16 bit register to memory
#define	OPT_ED_LD_BC	0x13	// load bc from memory
#define	OPT_ED_LD_DE	0x23	// load de from memory
#define	OPT_ED_LD_SP	0x33	// load sp from memory
#define	OPT_ED_RET		0x81	// retn or reti

// DD and FD option bits:

#define	OPT_DD_2			0x01	// 2 byte, entry in dd1tbl
#define	OPT_DD_LOAD		0x02	// 3 byte loads
#define	OPT_DD_DIR		0x03	// 4 byte direct addressing
#define	OPT_DD_ARTH		0x06	// 3 byte arithmetic codes
#define	OPT_DD_CB		0x07	// 4 byte dd/fd cb codes

struct snementry {
   char mcode[8];
};

struct entry {
	char mnem[16];
};

//
//	Prototypes
//

extern void	usage(void);

//
//	Global Variables
//
extern int	d8080;	// 8080 mnemonic flag

#endif	// _DZ80_H_
