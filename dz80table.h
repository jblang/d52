
/*
 * Z80 Disassembler
 * Copyright (C) 1990-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * dz80table.h - Z80 disassembler tables
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

#ifndef	_DZ80TABLE_H_
#define	_DZ80TABLE_H_

extern struct mnementry	mnemtbl[];
extern struct mnementry	cbtbl[];
extern struct mnementry	regtbl[];
extern struct mnementry	ddcbtbl[];
extern struct mnementry	dd1tbl[];
extern struct mnementry	dd2tbl[];
extern struct mnementry	edtbl[];
extern unsigned char		opttbl[];
extern unsigned char		edcode[];
extern unsigned char		ddcode[];
extern unsigned char		cbcycles[256];
extern unsigned char		ddcycles[256];
extern unsigned char		edcycles[256];
extern unsigned char		edcycles2[256];
extern unsigned char		fdcycles[256];
extern unsigned char		ddcbcycles[256];
extern unsigned char		fdcbcycles[256];

#endif	// _DZ80TABLE_H_
