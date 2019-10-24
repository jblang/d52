
/*
 * Pass 3 for Disassemblers
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
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

#include	<ctype.h>

#include	"common.h"

//
// Defined Constants
//

/*
	MISC_EQU_TAG is used in pass 3 to determine whether a miscellaneous
	equate statement should be generated. The equate statement will be
	written to the disassembly file if the location is referenced by some
	instruction and either it is a split reference or the location is
	uninitialized and the location does not have an entry in the label
	table. If the above requirements are met except that the location has
	an entry in the label table, then an equate statement will be generated
	as a label equate.
*/

#define	MISC_EQU_TAG ((pflag & PF_REF) && ((pflag & PF_SPLIT) | (pflag & PF_NOINIT)))

bool	isnumeric(char *str);

//
// Global variables
//
/* none */

// Return TRUE if string is a simple expression based on a defined
// symbol, label, or operand name, eg: label+10 or symbol*4

bool isnumexpression(char *str)
{
	int	i, len;
	bool	arith = FALSE, valid = FALSE;
	char	name[MAX_LINE];

	len = strlen(str);

	for (i=0; i<len && !arith; i++)
	{
		switch (str[i])
		{
			case '+':
			case '-':
			case '*':
			case '/':
				arith = TRUE;
				name[i] = '\0';
				break;

			default:
				name[i] = str[i];
				break;
		}
	}

	name[i] = '\0';

	if (arith)		// appears to be an arithmetic expression
	{
		if (find_name(name, label_count, lab_val_index))			// is it a valid label?
			valid = TRUE;
		else if (find_name(name, symbol_count, sym_val_index))	// is it a valid symbol?
			valid = TRUE;
		else if (find_name(name, name_count, name_val_index))		// is it a valid operand name?
			valid = TRUE;
	}

	if (arith && valid)	// arithmetic expression based on a defined label, symbol, or name
		return TRUE;

	return FALSE;
}

// Return TRUE if string is purely numeric (eg: "0800h" or "1024" or "10110b")

bool isnumeric(char *str)
{
	bool	result = TRUE;
	bool	hex = FALSE;
	bool	binary = FALSE;
	int	i, len;

	len = strlen(str);

	if (toupper(str[len - 1]) == 'H')
	{
		hex = TRUE;
		--len;
	}
	else if (toupper(str[len - 1]) == 'B')
	{
		binary = TRUE;
		--len;
	}

	i = 0;

	if (!hex && !binary)			// if decimal number
	{
		if (str[0] == '-' && len > 1)		// allow negative
			i = 1;
	}

	for ( ; i<len; i++)
	{
		if (!str[i] || str[i] == '\n')
			break;

		if (hex)					// hexadecimal number
		{
			if (!isxdigit(str[i]))
			{
				result = FALSE;
				break;
			}
		}
		else if (binary)		// binary number
		{
			if (str[i] != '0' && str[i] != '1')
			{
				result = FALSE;
				break;
			}
		}
		else						// decimal number
		{
			if (!isdigit(str[i]))
			{
				result = isnumexpression(str);	// allow expressions in decimal data, eg: symbol+100
				break;
			}
		}
	}

	return result;
}

//
// Pass three of disassembly
//
// Search for references to un-initialized data or split references
// and, if found, generate EQU statements for them.
//

void pass3(void)
{
	int	i, j, k, index, val, adrs, next_adrs, next_val, ok;
	struct sym	*ptr = NULL;
	char	*cptr;
	int	pflag;
	bool	isnum = FALSE;

	printf("\nPass 3 0000");

	// Sort tables by name for checking numeric entries

	if (label_count)
	{
		lab_tab = sort_by_name(lab_tab);
		ptr = lab_tab;

		for (i=0; i<label_count; i++)
		{
			lab_val_index[i] = ptr;
			ptr = ptr->next;
		}
	}

	if (symbol_count)
	{
		sym_tab = sort_by_name(sym_tab);
		ptr = sym_tab;

		for (i=0; i<symbol_count; i++)
		{
			sym_val_index[i] = ptr;
			ptr = ptr->next;
		}
	}

	if (name_count)
	{
		name_tab = sort_by_name(name_tab);
		ptr = name_tab;

		for (i=0; i<name_count; i++)
		{
			name_val_index[i] = ptr;
			ptr = ptr->next;
		}
	}

// search label table for labels referenced but not generated

	j = TRUE;

	for (index=0; index<label_count; index++)
	{
		ptr = lab_val_index[index];
		isnum = isnumeric(ptr->name);	// ignore if numeric expression

		if (ptr->used && !isnum)
		{
			val = ptr->val;
			val = pgmflags[val];
			val &= (PF_NOINIT | PF_SPLIT | PF_REF);

			if (val == (PF_REF | PF_SPLIT) || val == (PF_REF | PF_NOINIT))
			{
				if (j)								// do header if first one
				{
					j = FALSE;

					if (!newline || dump)
						fprintf(fp, "\n;");

					fprintf(fp, "\n;\tLabel equates\n;\n;"
						"  These are labels in the control file that reference\n;"
						"  the middle of a multibyte instruction or reference\n;"
						"  an address outside the initialized space\n;");
				}

				fprintf(fp, "\n%s\t%s\t", ptr->name, equstr);
				puthex(ptr->val);
				newline = FALSE;
			}
		}
	}

// now do equates for symbol table

	j = TRUE;

	for (index=0; index<symbol_count; index++)
	{
		ptr = sym_val_index[index];
		isnum = isnumeric(ptr->name);	// ignore if numeric expression

		if (ptr->used && !isnum)
		{
			if (j)							// do header if first one
			{
				j = FALSE;

				if (!newline || dump)
					fprintf(fp, "\n;");

				fprintf(fp, "\n;\tSymbol equates\n;\n;"
								"  These are symbols from the control\n;"
								"  file that are referenced in the code\n;");
			}

			fprintf(fp, "\n%s\t%s\t", ptr->name, equstr);
			puthex(ptr->val);
			newline = FALSE;
		}
	}

// now do equates for operand name table

	j = TRUE;
	ptr = name_tab;

	for (index=0; index<name_count; index++)
	{
		ok = FALSE;
		isnum = isnumeric(ptr->name);	// ignore if numeric expression

		if ((ptr->used != 255) && !isnum)
		{
			if (!strcasecmp(ptr->name, ptr->next->name))
			{
				adrs = ptr->val;
				val = pgmmem[adrs];

				if (pgmflags[adrs] & PF_2BYTE)
#ifdef CPU_BIG_ENDIAN
				{
					val <<= 8;
					val |= pgmmem[adrs + 1];
				}
#else
					val |= (pgmmem[adrs + 1] << 8);
#endif

				next_adrs = ptr->next->val;
				next_val = pgmmem[next_adrs];

				if (pgmflags[next_adrs] & PF_2BYTE)
#ifdef CPU_BIG_ENDIAN
				{
					next_val <<= 8;
					next_val |= pgmmem[next_adrs + 1];
				}
#else
					next_val |= (pgmmem[next_adrs + 1] << 8);
#endif

				if (val != next_val)
					ok = TRUE;
			}
			else
				ok = TRUE;

			if (ok)
			{
				if (j)							// do header if first one
				{
					j = FALSE;

					if (!newline || dump)
						fprintf(fp, "\n;");

					fprintf(fp, "\n;\tOperand symbol equates\n;\n;"
									"  These are operand symbols from the control\n;"
									"  file that are referenced in the code\n;");
				}

				adrs = ptr->val;
				val = pgmmem[adrs] & 0xff;

				if (pgmflags[adrs] & PF_2BYTE)
#ifdef CPU_BIG_ENDIAN
				{
					val <<= 8;
					val |= pgmmem[adrs + 1];
				}
#else
					val |= (pgmmem[adrs + 1] << 8);
#endif

				fprintf(fp, "\n%s\t%s\t", ptr->name, equstr);
				puthex(val);
				newline = FALSE;
			}
		}

		ptr = ptr->next;
	}

	// to do miscellaneous equates, we need to resort labels by value

	if (label_count)
		lab_tab = sort(lab_tab, lab_val_index, label_count);

	j = TRUE;

	for (i=0; ; )
	{
		k = i & 0xfff;
		pflag = pgmflags[i];

// if location is referenced and un-initialized or is a split ref

		if (MISC_EQU_TAG)
		{
			cptr = find_entry(i, label_count, lab_val_index);

			if ((cptr == NULL) && !(pflag & PF_LABGEN))		// if not in label list
			{
				if (j)							// do header if first one
				{
					j = FALSE;

					if (!newline || dump)
						fprintf(fp, "\n;");

					fprintf(fp, "\n;\tMiscellaneous equates\n;\n;"
									"  These are addresses referenced in the code but\n;"
									"  which are in the middle of a multibyte instruction\n;"
									"  or are addresses outside the initialized space\n;");
					newline = FALSE;
				}

				if (!upperflag)
					fprintf(fp, "\nX%04x\t%s\t", i, equstr);		// do EQU statement
				else
					fprintf(fp, "\nX%04X\t%s\t", i, equstr);

				puthex(i);
			}
		}

		i++;

		if (!(i & WORD_MASK))
			break;

		if ((i & 0xfff) < k)
			printf("\rPass 3 %04x", i);
	}

	printf("\rPass 3 - Equate generation complete");

	if (!newline || dump)
		fprintf(fp, "\n;");

	if (upperflag)
		fprintf(fp, "\n\tEND\n;\n\n");
	else
		fprintf(fp, "\n\tend\n;\n\n");

	fflush(fp);
	fclose(fp);
}							//  End of Pass 3

// end of dispass3.c
