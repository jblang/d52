
/*
 * 8080/8085 Disassembler
 * Copyright (C) 1990-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * d80table.c - Mnemonic Tables
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
 
#include	"dz80.h"

// blank entries are defb invalid opcodes

struct mnementry mnemtbl80[] = {
    {"nop"},       {"lxi b,"},     {"stax b"},    {"inx b"},  // 00 - 03
    {"inr b"},     {"dcr b"},      {"mvi b,"},    {"rlc"},    // 04 - 07
    {""},          {"dad b"},      {"ldax b"},    {"dcx b"},  // 08 - 0b
    {"inr c"},     {"dcr c"},      {"mvi c,"},    {"rrc"},    // 0c - 0f
    {""},          {"lxi d,"},     {"stax d"},    {"inx d"},  // 10 - 13
    {"inr d"},     {"dcr d"},      {"mvi d,"},    {"ral"},    // 14 - 17
    {""},          {"dad d"},      {"ldax d"},    {"dcx d"},  // 18 - 1b
    {"inr e"},     {"dcr e"},      {"mvi e,"},    {"rar"},    // 1c - 1f
    {"rim"},       {"lxi h,"},     {"shld "},     {"inx h"},  // 20 - 23
    {"inr h"},     {"dcr h"},      {"mvi h,"},    {"daa"},    // 24 - 27
    {""},          {"dad h"},      {"lhld "},     {"dcx h"},  // 28 - 2b
    {"inr l"},     {"dcr l"},      {"mvi l,"},    {"cma"},    // 2c - 2f
    {"sim"},       {"lxi sp,"},    {"sta "},      {"inx sp"}, // 30 - 33
    {"inr m"},     {"dcr m"},      {"mvi m,"},    {"stc"},    // 34 - 37
    {""},          {"dad sp"},     {"lda "},      {"dcx sp"}, // 38 - 3b
    {"inr a"},     {"dcr a"},      {"mvi a,"},    {"cmc"},    // 3c - 3f
    {"mov b,b"},   {"mov b,c"},    {"mov b,d"},   {"mov b,e"},// 40 - 43
    {"mov b,h"},   {"mov b,l"},    {"mov b,m"},   {"mov b,a"},// 44 - 47
    {"mov c,b"},   {"mov c,c"},    {"mov c,d"},   {"mov c,e"},// 48 - 4b
    {"mov c,h"},   {"mov c,l"},    {"mov c,m"},   {"mov c,a"},// 4c - 4f
    {"mov d,b"},   {"mov d,c"},    {"mov d,d"},   {"mov d,e"},// 50 - 53
    {"mov d,h"},   {"mov d,l"},    {"mov d,m"},   {"mov d,a"},// 54 - 57
    {"mov e,b"},   {"mov e,c"},    {"mov e,d"},   {"mov e,e"},// 58 - 5b
    {"mov e,h"},   {"mov e,l"},    {"mov e,m"},   {"mov e,a"},// 5c - 5f
    {"mov h,b"},   {"mov h,c"},    {"mov h,d"},   {"mov h,e"},// 60 - 63
    {"mov h,h"},   {"mov h,l"},    {"mov h,m"},   {"mov h,a"},// 64 - 67
    {"mov l,b"},   {"mov l,c"},    {"mov l,d"},   {"mov l,e"},// 68 - 6b
    {"mov l,h"},   {"mov l,l"},    {"mov l,m"},   {"mov l,a"},// 6c - 6f
    {"mov m,b"},   {"mov m,c"},    {"mov m,d"},   {"mov m,e"},// 70 - 73
    {"mov m,h"},   {"mov m,l"},    {"hlt"},       {"mov m,a"},// 74 - 77
    {"mov a,b"},   {"mov a,c"},    {"mov a,d"},   {"mov a,e"},// 78 - 7b
    {"mov a,h"},   {"mov a,l"},    {"mov a,m"},   {"mov a,a"},// 7c - 7f
    {"add b"},     {"add c"},      {"add d"},     {"add e"},  // 80 - 83
    {"add h"},     {"add l"},      {"add m"},     {"add a"},  // 84 - 87
    {"adc b"},     {"adc c"},      {"adc d"},     {"adc e"},  // 88 - 8b
    {"adc h"},     {"adc l"},      {"adc m"},     {"adc a"},  // 8c - 8f
    {"sub b"},     {"sub c"},      {"sub d"},     {"sub e"},  // 90 - 93
    {"sub h"},     {"sub l"},      {"sub m"},     {"sub a"},  // 94 - 97
    {"sbb b"},     {"sbb c"},      {"sbb d"},     {"sbb e"},  // 98 - 9b
    {"sbb h"},     {"sbb l"},      {"sbb m"},     {"sbb a"},  // 9c - 9f
    {"ana b"},     {"ana c"},      {"ana d"},     {"ana e"},  // a0 - a3
    {"ana h"},     {"ana l"},      {"ana m"},     {"ana a"},  // a4 - a7
    {"xra b"},     {"xra c"},      {"xra d"},     {"xra e"},  // a8 - ab
    {"xra h"},     {"xra l"},      {"xra m"},     {"xra a"},  // ac - af
    {"ora b"},     {"ora c"},      {"ora d"},     {"ora e"},  // a0 - a3
    {"ora h"},     {"ora l"},      {"ora m"},     {"ora a"},  // a4 - a7
    {"cmp b"},     {"cmp c"},      {"cmp d"},     {"cmp e"},  // a8 - ab
    {"cmp h"},     {"cmp l"},      {"cmp m"},     {"cmp a"},  // ac - af
    {"rnz"},       {"pop b"},      {"jnz "},      {"jmp "},   // c0 - c3
    {"cnz "},      {"push b"},     {"adi "},      {"rst 0"},  // c4 - c7
    {"rz"},        {"ret"},        {"jz "},       {""},       // c8 - cb
    {"cz "},       {"call "},      {"aci "},      {"rst 1"},  // cc - cf
    {"rnc"},       {"pop d"},      {"jnc "},      {"out "},   // d0 - d3
    {"cnc "},      {"push d"},     {"sui "},      {"rst 2"},  // d4 - d7
    {"rc"},        {""},           {"jc "},       {"in "},    // d8 - db
    {"cc "},       {""},           {"sbi "},      {"rst 3"},  // dc - df
    {"rpo"},       {"pop h"},      {"jpo "},      {"xthl"},   // e0 - e3
    {"cpo "},      {"push h"},     {"ani "},      {"rst 4"},  // e4 - e7
    {"rpe"},       {"pchl"},       {"jpe "},      {"xchg"},   // e8 - eb
    {"cpe "},      {""},           {"xri "},      {"rst 5"},  // ec - ef
    {"rp"},        {"pop psw"},    {"jp "},       {"di"},     // f0 - f3
    {"cp "},       {"push psw"},   {"ori "},      {"rst 6"},  // f4 - f7
    {"rm"},        {"sphl"},       {"jm "},       {"ei"},     // f8 - fb
    {"cm "},       {""},           {"cpi "},      {"rst 7"}   // fc - ff
} ;

/* OPTTBL (option table) entries:

   bit 7 = unconditional transfer instruction
   bit 6 = unused
   bit 5 = unused
   bit 4 = direct reference (jmp, call, etc)
   bit 3 = invalid opcode (defb)
   bit 2 = immediate data
   bit 1 = 3 byte instruction
   bit 0 = 2 byte instruction
      if entry = 0, instruction is single byte, no options
*/

unsigned char opttbl80[] = {
   0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    // 00 - 07
   0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    // 08 - 0f
   0x08, 0x16, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    // 10 - 17
   0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,    // 18 - 1f
   0x00, 0x16, 0x16, 0x00, 0x00, 0x00, 0x05, 0x00,    // 20 - 27
   0x08, 0x00, 0x16, 0x00, 0x00, 0x00, 0x05, 0x00,    // 28 - 2f
   0x00, 0x16, 0x16, 0x00, 0x00, 0x00, 0x05, 0x00,    // 30 - 37
   0x08, 0x00, 0x16, 0x00, 0x00, 0x00, 0x05, 0x00,    // 38 - 3f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 40 - 47
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 48 - 4f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 50 - 57
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 58 - 5f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 60 - 67
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 68 - 6f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,    // 70 - 77
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 78 - 7f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 80 - 87
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 88 - 8f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 90 - 97
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 98 - 9f
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // a0 - a7
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // a8 - af
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // b0 - b7
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // b8 - bf
   0x00, 0x00, 0x16, 0x96, 0x16, 0x00, 0x05, 0x00,    // c0 - c7
   0x00, 0x80, 0x16, 0x08, 0x16, 0x16, 0x05, 0x00,    // c8 - cf
   0x00, 0x00, 0x16, 0x05, 0x16, 0x00, 0x05, 0x00,    // d0 - d7
   0x00, 0x08, 0x16, 0x05, 0x16, 0x08, 0x05, 0x00,    // d8 - df
   0x00, 0x00, 0x16, 0x00, 0x16, 0x00, 0x05, 0x00,    // e0 - e7
   0x00, 0x80, 0x16, 0x00, 0x16, 0x08, 0x05, 0x00,    // e8 - ef
   0x00, 0x00, 0x16, 0x00, 0x16, 0x00, 0x05, 0x00,    // f0 - f7
   0x00, 0x00, 0x16, 0x00, 0x16, 0x08, 0x05, 0x00     // f8 - ff
} ;

// end of d80table.c
