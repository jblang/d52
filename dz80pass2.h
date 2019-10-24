
/*
 * Z80 Disassembler
 * Copyright (C) 1990-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * dz80pass2.h - Disassembly pass 2
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

#ifndef	_DZ80PASS2_H_
#define	_DZ80PASS2_H_

//
//	Prototypes
//

extern void	pass2(void);
extern void	invalided(byte k);
extern int	doindex(int i, char index);
extern void	indxld1(int i, char reg, char idx);
extern void	indxld2(int i, char reg, char idx);

//
//	Global variables
//
/* none */

#endif	// _DZ80PASS2_H_

