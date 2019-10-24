
/*
 * D52 8052 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * analyze.h - 8052/Z80 disassembler code analyzer common code
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

#ifndef	_ANALYZE_H_
#define	_ANALYZE_H_

// Defined Constants

#define	STACK_DEPTH			1024
#define	MIN_STR_LEN			5		// minimum number of characters to ID a string
#define	TRACE_CHECK_LEN	4		// number of code bytes to check for valid code

#define	ANALYZE_NONE		0x00
#define	ANALYZE_TRACED		0x01
#define	ANALYZE_CODE		0x02
#define	ANALYZE_VECTOR		0x04
#define	ANALYZE_BINARY		0x08
#define	ANALYZE_ASCII		0x10
#define	ANALYZE_IGNORE		0x20
#define	ANALYZE_TAGGED		(ANALYZE_TRACED | ANALYZE_CODE)
#define	ANALYZE_END			0x80

typedef struct strlist {
	char				*str;
	struct strlist	*prev;
	struct strlist	*next;
} STRLIST;

// Data

extern int	vectortable[];

extern FILE	*ctlfp;
extern int	listCount;
extern char	fileName[256];
extern char	fileExt[128];
extern char	tempString[128];
extern char	dateString[64];

extern byte	*analysisFlags;

extern int	tpc;					// trace pc
extern int	pushLevel;
extern int	astackPtr;
extern int	astack[STACK_DEPTH];			// analysis stack, for returns and branches
extern int	vstackPtr;
extern int	vstack[STACK_DEPTH];			// possible vector references stack
extern char	alertMessage[128];

// Prototypes

extern bool	analyzeCode(char *dtext);
extern bool	analyze(void);
extern bool	aPass1(void);
extern bool	aPass2(void);
extern bool	trace(int pc);
extern bool	isString(int pc, int stop);
extern int	getEndOfString(int pc, int stop);
extern bool	isTraceableCode(int pc);
extern void	genAnalysisList(void);
extern int	createLineList(char *dtext);
extern void	deleteLineList(void);
extern int	writeCtlFile(void);
extern bool	addListEntry(char *str);
extern void	analysisWarning(char *msg);
extern void	analysisError(char *msg);

#endif	// _ANALYZE_H_

