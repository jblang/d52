
/*
 * D52 8052 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * analyzez80.h - Z80 disassembler code analyzer
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

#ifndef	_ANALYZEZ80_H_
#define	_ANALYZEZ80_H_

// Defined Constants

#define	OPCODE_LDHL			0x21		// ld hl,nn
#define	OPCODE_LDHLI		0x2a		// ld hl,(nn)
#define	OPCODE_LDNNHL		0x22		// ld (nn),hl
#define	OPCODE_LDIX			0xdd21	// ld ix,nn
#define	OPCODE_LDIY			0xfd21	// ld iy,nn
#define	OPCODE_LDIXI		0xdd2a	// ld ix,(nn)
#define	OPCODE_LDIYI		0xfd2a	// ld iy,(nn)

#define	OPCODE_JP			0xc3
#define	OPCODE_JPC			0xda
#define	OPCODE_JPNC			0xd2
#define	OPCODE_JPZ			0xca
#define	OPCODE_JPNZ			0xc2
#define	OPCODE_JPPE			0xea
#define	OPCODE_JPPO			0xe2
#define	OPCODE_JPM			0xfa
#define	OPCODE_JPP			0xf2

#define	OPCODE_JPHL			0xe9
#define	OPCODE_JPIX			0xdde9
#define	OPCODE_JPIY			0xfde9

#define	OPCODE_JR			0x18
#define	OPCODE_JRC			0x38
#define	OPCODE_JRNC			0x30
#define	OPCODE_JRZ			0x28
#define	OPCODE_JRNZ			0x20

#define	OPCODE_DJNZ			0x10

#define	OPCODE_CALL			0xcd
#define	OPCODE_CALLC		0xdc
#define	OPCODE_CALLNC		0xd4
#define	OPCODE_CALLZ		0xcc
#define	OPCODE_CALLNZ		0xc4
#define	OPCODE_CALLPE		0xec
#define	OPCODE_CALLPO		0xe4
#define	OPCODE_CALLM		0xfc
#define	OPCODE_CALLP		0xf4

#define	OPCODE_RST00		0xc7
#define	OPCODE_RST08		0xcf
#define	OPCODE_RST10		0xd7
#define	OPCODE_RST18		0xdf
#define	OPCODE_RST20		0xe7
#define	OPCODE_RST28		0xef
#define	OPCODE_RST30		0xf7
#define	OPCODE_RST38		0xff

#define	OPCODE_RET			0xc9
#define	OPCODE_RETC			0xd8
#define	OPCODE_RETNC		0xd0
#define	OPCODE_RETZ			0xc8
#define	OPCODE_RETNZ		0xc0
#define	OPCODE_RETPE		0xe8
#define	OPCODE_RETPO		0xe0
#define	OPCODE_RETM			0xf8
#define	OPCODE_RETP			0xf0

#define	OPCODE_RETI			0xed4d
#define	OPCODE_RETN			0xed45

#define	OPCODE_PUSHAF		0xf5
#define	OPCODE_PUSHBC		0xc5
#define	OPCODE_PUSHDE		0xd5
#define	OPCODE_PUSHHL		0xe5
#define	OPCODE_PUSHIX		0xdde5
#define	OPCODE_PUSHIY		0xfde5

#define	OPCODE_POPAF		0xf1
#define	OPCODE_POPBC		0xc1
#define	OPCODE_POPDE		0xd1
#define	OPCODE_POPHL		0xe1
#define	OPCODE_POPIX		0xdde1
#define	OPCODE_POPIY		0xfde1

#endif	// _ANALYZEZ80_H_
