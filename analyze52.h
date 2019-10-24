
/*
 * D52 8052 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * analyze52.h - 8052 disassembler code analyzer
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

#ifndef	_ANALYZE52_H_
#define	_ANALYZE52_H_

// Defined Constants

#define	OPCODE_AJMP			0x01
#define	OPCODE_LJMP			0x02
#define	OPCODE_JBC			0x10
#define	OPCODE_ACALL		0x11
#define	OPCODE_LCALL		0x12
#define	OPCODE_JB			0x20
#define	OPCODE_RET			0x22
#define	OPCODE_JNB			0x30
#define	OPCODE_RETI			0x32
#define	OPCODE_JC			0x40
#define	OPCODE_JNC			0x50
#define	OPCODE_JZ			0x60
#define	OPCODE_JNZ			0x70
#define	OPCODE_JMPDPTR		0x73
#define	OPCODE_SJMP			0x80
#define	OPCODE_CJNE1		0xb4
#define	OPCODE_CJNE2		0xbf
#define	OPCODE_PUSH			0xc0
#define	OPCODE_POP			0xd0
#define	OPCODE_DJNZ1		0xd5
#define	OPCODE_DJNZ			0xd8
#define	OPCODE_ACALL_MASK	0x1f
#define	OPCODE_AJMP_MASK	0x1f
#define	OPCODE_DJNZ_MASK	0xf8
#define	OPCODE_MOVDPTR		0x90

#endif	// _ANALYZE52_H_
