
/*
 * Disassembler common routines
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * common.h - Support routines
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

#ifndef	_COMMON_H_
#define	_COMMON_H_

#ifdef	EXTENDED_MEM
#define	EXT_PGM_SEGS	8	/* maximum number of extended segments: 8 * 64kb = 512kb */
#endif

// Global variables

extern char	licenseText[];

extern char	src[FN_LEN], dst[FN_LEN];	// file name buffers
extern char	baseFileName[FN_LEN];		// source file name without extension
extern char	ctl[FN_LEN];					// control file name
extern char	linebuffer[MAX_LINE];		// input line buffer
extern FILE	*fp;								// dos file struct
extern int	hexflag;							// append hex flag
extern int	fileflag;						// file type flag
extern int	upperflag;						// upper case output flag
extern int	baseflag;
extern bool	traceflag;						// trace and analyze code
extern int	kcnt;								// output char counter
extern int	pc;								// current program counter
extern int	himark;							// highest data adrs
extern int	offset;							// program counter offset
extern byte	*pgmmem;							// program data pointer
extern int	*pgmflags;						// pointer to program flags

#ifdef	EXTENDED_MEM
extern byte	*epgmmem[EXT_PGM_SEGS];		// extended program memory pointers
extern int	*epgmflags[EXT_PGM_SEGS];	// extended program flags pointers
#endif

extern char	string[ASCLIMIT];				// ascii data for defb
extern int	asc_cnt;							// count for string data
extern byte	byte_data[BYTELIMIT];		// binary data for defb
extern int	byte_cnt;						// count for binary data
extern int	word_data[WORDLIMIT];		// binary data for defw
extern int	word_cnt;						// count for word data
extern byte	dump;								// dump just done flag
extern byte	ascii_flag;						// use ascii string flag
extern char	defbstr[8];						// string for defined bytes
extern char	defwstr[8];						// string for defined words
extern char	ascistr[8];						// string for defined ascii
extern char	orgstr[8];						 // org pseudo-op string
extern char	equstr[8];						 // equ pseudo-op string

extern struct sym	*sym_tab;					// symbol table pointer
extern struct sym	*lab_tab;					// label table pointer
extern struct sym	*name_tab;					// operand names pointer
extern struct sym	*sym_tab_last;				// last symbol table pointer
extern struct sym	*lab_tab_last;				// lastlabel table pointer
extern struct sym	*name_tab_last;			// last name table pointer
extern int			symbol_count;				// number of symbols
extern int			label_count;				// number of labels
extern int			name_count;					// number of operand names

extern SYM_PTR		*sym_val_index;			// array of pointers
extern SYM_PTR		*lab_val_index;			//  for binary search
extern SYM_PTR		*name_val_index;
extern struct sym	*tail_ptr, *head_ptr;	// sort pointers

extern struct comment	*comment_list;		// header comment list
extern struct comment	*icomment_list;	// inline comment list
extern struct comment	*patch_list;		// patch list

extern int			newline;						// just output newline flag
extern struct tm	*date_time;					// disassembly time

extern bool	cycleflag;							// cycle counting global on/off switch
extern bool	cycle_exclude;						// cycle counting suspended flag
extern bool cycle_alwaystake;					// cycle counting "always take" (worst case) flag
extern CYCLE_RANGE_PTR	cycle_r;				// cycle counting root range
extern CYCLE_RANGE_PTR	cycle_current;		// cycle counting current range

//
// Prototypes
//

#if defined __LCC__
extern int	strcasecmp(char *s1, char *s2);
extern int	strncasecmp(char *s1, char *s2, size_t n);
#endif

extern bool			init_memory(void);

#ifdef	EXTENDED_MEM
extern byte			*get_extended_mem(int width);
#endif

extern char			*makeupper(char *str);
extern int			parseFileName(char *str, char *ext);
extern int			readfile(char *filename);
extern void			getCTLoffset(void);
extern void			getcode(char *from, byte *loc);
extern char			*get_adrs(char *text, int *val);
extern void			error(char *str1, char *str2);			// fatal error trap
extern struct sym	*sort(struct sym *list, SYM_PTR *array, int count);
extern struct sym	*sort_by_name(struct sym *list);
extern struct sym	*sort_by_value(struct sym *list);
extern struct sym	*merge_by_name(struct sym *a, struct sym *b);
extern struct sym	*merge_by_value(struct sym *a, struct sym *b);
extern void			chk_dup_name(struct sym *list, int count);
extern void			chk_dup_op_name(struct sym *list, int count);
extern void			chk_dup_value(struct sym *list, int count);
extern void			chk_dup_op_value(struct sym *list, int count);
extern char			*find_entry(int val, int count, SYM_PTR *table);
extern SYM_PTR		find_name(char *name, int count, SYM_PTR *table);
extern struct sym	*get_smem(int type, int req_size);
extern struct sym	*add_entry(int val, char *symbol, int type);
extern void			output_comment(int adrs);
extern void			output_icomment(int adrs);
extern void			output_patch(int adrs);
extern void			add_comment(int adrs, char *str);
extern void			add_icomment(int adrs, char *str);
extern void			add_patch(int adrs, char *str);
extern void			puthex(int j);
extern int			ascii(int i);
extern int			is_ascii(byte data);
extern int			atox(char *str);
extern void			splitcheck(int i);
extern void			chk_ref(int i);
extern void			chk_label(int i);
extern void			doopcode(char *mnem);
extern void			dump_ascii(int adrs);
extern void			dump_bytes(int adrs);
extern void			readcyclefile(char *c);
extern void			cycle_in(int num, int next_num, int cnt, int cnt2);
extern void			cycle_end(int num);

// --- although I know this is not properly declared here,
// I know no better way how to make it usable in common.c ---
//     (haven't I told that I am not really into C? :-) )  -- JW
// so it is assumed that all 3 ports (d52, d48, dz80) do have
// these tables implemented (in XXXtables.c)
extern unsigned char	cycles[256];
extern unsigned char	cycles2[256];

#endif	//	_COMMON_H_

