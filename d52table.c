
/*
 * D52 8052 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * d52table.c - Mnemonic Table
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

#include	<stdio.h>

#include	"d52.h"

struct mnementry mnemtbl[256] = {
   {"nop "},      {"ajmp "},     {"ljmp "},     {"rr a"},           // 00 - 03
   {"inc a"},     {"inc "},      {"inc @r0"},   {"inc @r1"},        // 04 - 07
   {"inc r0"},    {"inc r1"},    {"inc r2"},    {"inc r3"},         // 08 - 0b
   {"inc r4"},    {"inc r5"},    {"inc r6"},    {"inc r7"},         // 0c - 0f
   {"jbc "},      {"acall "},    {"lcall "},    {"rrc a"},          // 10 - 13
   {"dec a"},     {"dec "},      {"dec @r0"},   {"dec @r1"},        // 14 - 17
   {"dec r0"},    {"dec r1"},    {"dec r2"},    {"dec r3"},         // 18 - 1b
   {"dec r4"},    {"dec r5"},    {"dec r6"},    {"dec r7"},         // 1c - 1f
   {"jb "},       {"ajmp "},     {"ret "},      {"rl a"},           // 20 - 23
   {"add a,#"},   {"add a,"},    {"add a,@r0"}, {"add a,@r1"},      // 24 - 27
   {"add a,r0"},  {"add a,r1"},  {"add a,r2"},  {"add a,r3"},       // 28 - 2b
   {"add a,r4"},  {"add a,r5"},  {"add a,r6"},  {"add a,r7"},       // 2c - 2f
   {"jnb "},      {"acall "},    {"reti "},     {"rlc a"},          // 30 - 33
   {"addc a,#"},  {"addc a,"},   {"addc a,@r0"},{"addc a,@r1"},     // 34 - 37
   {"addc a,r0"}, {"addc a,r1"}, {"addc a,r2"}, {"addc a,r3"},      // 38 - 3b
   {"addc a,r4"}, {"addc a,r5"}, {"addc a,r6"}, {"addc a,r7"},      // 3c - 3f
   {"jc "},       {"ajmp "},     {"orl "},      {"orl "},           // 40 - 43
   {"orl a,#"},   {"orl a,"},    {"orl a,@r0"}, {"orl a,@r1"},      // 44 - 47
   {"orl a,r0"},  {"orl a,r1"},  {"orl a,r2"},  {"orl a,r3"},       // 48 - 4b
   {"orl a,r4"},  {"orl a,r5"},  {"orl a,r6"},  {"orl a,r7"},       // 4c - 4f
   {"jnc "},      {"acall "},    {"anl "},      {"anl "},           // 50 - 53
   {"anl a,#"},   {"anl a,"},    {"anl a,@r0"}, {"anl a,@r1"},      // 54 - 57
   {"anl a,r0"},  {"anl a,r1"},  {"anl a,r2"},  {"anl a,r3"},       // 58 - 5b
   {"anl a,r4"},  {"anl a,r5"},  {"anl a,r6"},  {"anl a,r7"},       // 5c - 5f
   {"jz "},       {"ajmp "},     {"xrl "},      {"xrl "},           // 60 - 63
   {"xrl a,#"},   {"xrl a,"},    {"xrl a,@r0"}, {"xrl a,@r1"},      // 64 - 67
   {"xrl a,r0"},  {"xrl a,r1"},  {"xrl a,r2"},  {"xrl a,r3"},       // 68 - 6b
   {"xrl a,r4"},  {"xrl a,r5"},  {"xrl a,r6"},  {"xrl a,r7"},       // 6c - 6f
   {"jnz "},      {"acall "},    {"orl c,"},    {"jmp @a+dptr"},    // 70 - 73
   {"mov a,#"},   {"mov "},      {"mov @r0,#"}, {"mov @r1,#"},      // 74 - 77
   {"mov r0,#"},  {"mov r1,#"},  {"mov r2,#"},  {"mov r3,#"},       // 78 - 7b
   {"mov r4,#"},  {"mov r5,#"},  {"mov r6,#"},  {"mov r7,#"},       // 7c - 7f
   {"sjmp "},     {"ajmp "},     {"anl c,"},    {"movc a,@a+pc"},   // 80 - 83
   {"div ab"},    {"mov "},      {"mov "},      {"mov "},           // 84 - 87
   {"mov "},      {"mov "},      {"mov "},      {"mov "},           // 88 - 8b
   {"mov "},      {"mov "},      {"mov "},      {"mov "},           // 8c - 8f
   {"mov dptr,#"},{"acall "},    {"mov "},      {"movc a,@a+dptr"}, // 90 -93
   {"subb a,#"},  {"subb a,"},   {"subb a,@r0"},{"subb a,@r1"},     // 94 - 97
   {"subb a,r0"}, {"subb a,r1"}, {"subb a,r2"}, {"subb a,r3"},      // 98 - 9b
   {"subb a,r4"}, {"subb a,r5"}, {"subb a,r6"}, {"subb a,r7"},      // 9c - 9f
   {"orl c,/"},   {"ajmp "},     {"mov c,"},    {"inc dptr"},       // a0 - a3
   {"mul ab"},    {"db "},       {"mov @r0,"},  {"mov @r1,"},       // a4 - a7
   {"mov r0,"},   {"mov r1,"},   {"mov r2,"},   {"mov r3,"},        // a8 - ab
   {"mov r4,"},   {"mov r5,"},   {"mov r6,"},   {"mov r7,"},        // ac - af
   {"anl c,/"},   {"acall "},    {"cpl "},      {"cpl c"},          // b0 - b3
   {"cjne a,#"},  {"cjne a,"},   {"cjne @r0,#"},{"cjne @r1,#"},     // b4 - b7
   {"cjne r0,#"}, {"cjne r1,#"}, {"cjne r2,#"}, {"cjne r3,#"},      // b8 - bb
   {"cjne r4,#"}, {"cjne r5,#"}, {"cjne r6,#"}, {"cjne r7,#"},      // bc - bf
   {"push "},     {"ajmp "},     {"clr "},      {"clr c"},          // c0 - c3
   {"swap a"},    {"xch a,"},    {"xch a,@r0"}, {"xch a,@r1"},      // c4 - c7
   {"xch a,r0"},  {"xch a,r1"},  {"xch a,r2"},  {"xch a,r3"},       // c8 - cb
   {"xch a,r4"},  {"xch a,r5"},  {"xch a,r6"},  {"xch a,r7"},       // cc - cf
   {"pop "},      {"acall "},    {"setb "},     {"setb c"},         // d0 - d3
   {"da a"},      {"djnz "},     {"xchd a,@r0"},{"xchd a,@r1"},     // d4 - d7
   {"djnz r0,"},  {"djnz r1,"},  {"djnz r2,"},  {"djnz r3,"},       // d8 - db
   {"djnz r4,"},  {"djnz r5,"},  {"djnz r6,"},  {"djnz r7,"},       // dc - df
   {"movx a,@dptr"},{"ajmp "},   {"movx a,@r0"},{"movx a,@r1"},     // e0 - e3
   {"clr a"},     {"mov a,"},    {"mov a,@r0"}, {"mov a,@r1"},      // e4 - e7
   {"mov a,r0"},  {"mov a,r1"},  {"mov a,r2"},  {"mov a,r3"},       // e8 - eb
   {"mov a,r4"},  {"mov a,r5"},  {"mov a,r6"},  {"mov a,r7"},       // ec - ef
   {"movx @dptr,a"},{"acall "},  {"movx @r0,a"},{"movx @r1,a"},     // f0 - f3
   {"cpl a"},     {"mov "},      {"mov @r0,a"}, {"mov @r1,a"},      // f4 - f7
   {"mov r0,a"},  {"mov r1,a"},  {"mov r2,a"},  {"mov r3,a"},       // f8 - fb
   {"mov r4,a"},  {"mov r5,a"},  {"mov r6,a"},  {"mov r7,a"}        // fc - ff
} ;

/* OPTTBL (option table) entries:

   bit 7 = unconditional transfer instruction
   bit 6 = 11 bit addressing
   bit 5 = relative addressing
   bit 4 = bit addressing
   bit 3 = direct addressing
   bit 2 = immediate data
   bit 1 = 3 byte instruction
   bit 0 = 2 byte instruction
      if entry = 0, instruction is single byte, no options
*/

unsigned char opttbl[256] = {
   0x00, 0xc1, 0x86, 0x00, 0x00, 0x09, 0x00, 0x00,    // 00 - 07
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 08 - 0f
   0x32, 0x41, 0x06, 0x00, 0x00, 0x09, 0x00, 0x00,    // 10 - 17
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 18 - 1f
   0x32, 0xc1, 0x80, 0x00, 0x05, 0x09, 0x00, 0x00,    // 20 - 27
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 28 - 2f
   0x32, 0x41, 0x80, 0x00, 0x05, 0x09, 0x00, 0x00,    // 30 - 37
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 38 - 3f
   0x21, 0xc1, 0x09, 0x0e, 0x05, 0x09, 0x00, 0x00,    // 40 - 47
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 48 - 4f
   0x21, 0x41, 0x09, 0x0e, 0x05, 0x09, 0x00, 0x00,    // 50 - 57
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 58 - 5f
   0x21, 0xc1, 0x09, 0x0e, 0x05, 0x09, 0x00, 0x00,    // 60 - 67
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 68 - 6f
   0x21, 0x41, 0x11, 0x80, 0x05, 0x0e, 0x05, 0x05,    // 70 - 77
   0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,    // 78 - 7f
   0xa1, 0xc1, 0x11, 0x00, 0x00, 0x0a, 0x09, 0x09,    // 80 - 87
   0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,    // 88 - 8f
   0x06, 0x41, 0x11, 0x00, 0x05, 0x09, 0x00, 0x00,    // 90 - 97
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 98 - 9f
   0x11, 0xc1, 0x11, 0x00, 0x00, 0x00, 0x09, 0x09,    // a0 - a7
   0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,    // a8 - af
   0x11, 0x41, 0x11, 0x00, 0x26, 0x2a, 0x26, 0x26,    // b0 - b7
   0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26,    // b8 - bf
   0x09, 0xc1, 0x11, 0x00, 0x00, 0x09, 0x00, 0x00,    // c0 - c7
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // c8 - cf
   0x09, 0x41, 0x11, 0x00, 0x00, 0x2a, 0x00, 0x00,    // d0 - d7
   0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21,    // d8 - df
   0x00, 0xc1, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,    // e0 - e7
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // e8 - ef
   0x00, 0x41, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00,    // f0 - f7
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     // f8 - ff
} ;

// Names for Special Function Registers

struct sfrentry sfr[128] = {
    {"p0"},     {"sp"},     {"dpl"},    {"dph"},    // 80 - 83
    {"adat"},   {"85h"},    {"86h"},    {"pcon"},   // 84 - 87
    {"tcon"},   {"tmod"},   {"tl0"},    {"tl1"},    // 88 - 8b
    {"th0"},    {"th1"},    {"pwcm"},   {"pwmp"},   // 8a - 8f
    {"p1"},     {"91h"},    {"92h"},    {"93h"},    // 90 - 93
    {"94h"},    {"95h"},    {"96h"},    {"97h"},    // 94 - 97
    {"scon"},   {"sbuf"},   {"9ah"},    {"9bh"},    // 98 - 9b
    {"9ch"},    {"9dh"},    {"9eh"},    {"9fh"},    // 9c - 9f
    {"p2"},     {"0a1h"},   {"0a2h"},   {"0a3h"},   // a0 - a3
    {"0a4h"},   {"0a5h"},   {"0a6h"},   {"0a7h"},   // a4 - a7
    {"ie"},     {"cml0"},   {"cml1"},   {"cml2"},   // a8 - ab
    {"ctl0"},   {"ctl1"},   {"ctl2"},   {"ctl3"},   // ac - af
    {"p3"},     {"0b1h"},   {"0b2h"},   {"0b3h"},   // b0 - b3
    {"0b4h"},   {"0b5h"},   {"0b6h"},   {"0b7h"},   // b4 - b7
    {"ip"},     {"0b9h"},   {"0bah"},   {"0bbh"},   // b8 - bb
    {"0bch"},   {"0bdh"},   {"0beh"},   {"0bfh"},   // bc - bf
    {"p4"},     {"0c1h"},   {"0c2h"},   {"0c3h"},   // c0 - c3
    {"p5"},     {"adcon"},  {"adch"},   {"0c7h"},   // c4 - c7
    {"t2con"},  {"cmh0"},   {"rcap2l"}, {"rcap2h"}, // c8 - cb
    {"tl2"},    {"th2"},    {"cth2"},   {"cth3"},   // cc - cf
    {"psw"},    {"0d1h"},   {"0d2h"},   {"0d3h"},   // d0 - d3
    {"0d4h"},   {"0d5h"},   {"0d6h"},   {"0d7h"},   // d4 - d7
    {"i2cfg"},  {"s1sta"},  {"s1dat"},  {"s1adr"},  // d8 - db
    {"0dch"},   {"0ddh"},   {"0deh"},   {"0dfh"},   // dc - df
    {"acc"},    {"0e1h"},   {"0e2h"},   {"0e3h"},   // e0 - e3
    {"0e4h"},   {"0e5h"},   {"0e6h"},   {"0e7h"},   // e4 - e7
    {"csr"},    {"0e9h"},   {"tm2con"}, {"ctcon"},  // e8 - eb
    {"tml2"},   {"tmh2"},   {"ste"},    {"rte"},    // ec - ef
    {"b"},      {"0f1h"},   {"0f2h"},   {"0f3h"},   // f0 - f3
    {"0f4h"},   {"0f5h"},   {"0f6h"},   {"0f7h"},   // f4 - f7
    {"i2sta"},  {"0f9h"},   {"0fah"},   {"0fbh"},   // f8 - fb
    {"pwm0"},   {"pwm1"},   {"pwena"},  {"t3"}      // fc - ff
} ;

// Names for Special Function Registers for Keil A51 compatibility

struct sfrentry keilsfr[128] = {
    {"p0"},     {"sp"},     {"dpl"},    {"dph"},    // 80 - 83
    {"84h"},    {"85h"},    {"86h"},    {"pcon"},   // 84 - 87
    {"tcon"},   {"tmod"},   {"tl0"},    {"tl1"},    // 88 - 8b
    {"th0"},    {"th1"},    {"8eh"},    {"8fh"},    // 8a - 8f
    {"p1"},     {"91h"},    {"92h"},    {"93h"},    // 90 - 93
    {"94h"},    {"95h"},    {"96h"},    {"97h"},    // 94 - 97
    {"scon"},   {"sbuf"},   {"9ah"},    {"9bh"},    // 98 - 9b
    {"9ch"},    {"9dh"},    {"9eh"},    {"9fh"},    // 9c - 9f
    {"p2"},     {"0a1h"},   {"0a2h"},   {"0a3h"},   // a0 - a3
    {"0a4h"},   {"0a5h"},   {"0a6h"},   {"0a7h"},   // a4 - a7
    {"ie"},     {"0a9h"},   {"0aah"},   {"0abh"},   // a8 - ab
    {"0ach"},   {"0adh"},   {"0aeh"},   {"0afh"},   // ac - af
    {"p3"},     {"0b1h"},   {"0b2h"},   {"0b3h"},   // b0 - b3
    {"0b4h"},   {"0b5h"},   {"0b6h"},   {"0b7h"},   // b4 - b7
    {"ip"},     {"0b9h"},   {"0bah"},   {"0bbh"},   // b8 - bb
    {"0bch"},   {"0bdh"},   {"0beh"},   {"0bfh"},   // bc - bf
    {"0c0h"},   {"0c1h"},   {"0c2h"},   {"0c3h"},   // c0 - c3
    {"0c4h"},   {"0c5h"},   {"0c6h"},   {"0c7h"},   // c4 - c7
    {"0c8h"},   {"0c9h"},   {"0cah"},   {"0cbh"},   // c8 - cb
    {"0cch"},   {"0cdh"},   {"0ceh"},   {"0cfh"},   // cc - cf
    {"psw"},    {"0d1h"},   {"0d2h"},   {"0d3h"},   // d0 - d3
    {"0d4h"},   {"0d5h"},   {"0d6h"},   {"0d7h"},   // d4 - d7
    {"0d8h"},   {"0d9h"},   {"0dah"},   {"0dbh"},   // d8 - db
    {"0dch"},   {"0ddh"},   {"0deh"},   {"0dfh"},   // dc - df
    {"acc"},    {"0e1h"},   {"0e2h"},   {"0e3h"},   // e0 - e3
    {"0e4h"},   {"0e5h"},   {"0e6h"},   {"0e7h"},   // e4 - e7
    {"0e8h"},   {"0e9h"},   {"0eah"},   {"0ebh"},   // e8 - eb
    {"0ech"},   {"0edh"},   {"0eeh"},   {"0efh"},   // ec - ef
    {"b"},      {"0f1h"},   {"0f2h"},   {"0f3h"},   // f0 - f3
    {"0f4h"},   {"0f5h"},   {"0f6h"},   {"0f7h"},   // f4 - f7
    {"i2sta"},  {"0f9h"},   {"0fah"},   {"0fbh"},   // f8 - fb
    {"0fch"},   {"0fdh"},   {"0feh"},   {"0ffh"}    // fc - ff
} ;

// Names for Special Function Register bits

struct sfrentry sfrbits[128] = {
    {"p0.0"},   {"p0.1"},   {"p0.2"},   {"p0.3"},   // 80 - 83
    {"p0.4"},   {"p0.5"},   {"p0.6"},   {"p0.7"},   // 84 - 87
    {"it0"},    {"ie0"},    {"it1"},    {"ie1"},    // 88 - 8b
    {"tr0"},    {"tf0"},    {"tr1"},    {"tf1"},    // 8c - 8f
    {"p1.0"},   {"p1.1"},   {"p1.2"},   {"p1.3"},   // 90 - 93
    {"p1.4"},   {"p1.5"},   {"p1.6"},   {"p1.7"},   // 94 - 97
    {"ri"},     {"ti"},     {"rb8"},    {"tb8"},    // 98 - 9b
    {"ren"},    {"sm2"},    {"sm1"},    {"sm0"},    // 9c - 9f
    {"p2.0"},   {"p2.1"},   {"p2.2"},   {"p2.3"},   // a0 - a3
    {"p2.4"},   {"p2.5"},   {"p2.6"},   {"p2.7"},   // a4 - a7
    {"ex0"},    {"et0"},    {"ex1"},    {"et1"},    // a8 - ab
    {"es"},     {"ie.5"},   {"ie.6"},   {"ea"},     // ac - af
    {"rxd"},    {"txd"},    {"int0"},   {"int1"},   // b0 - b3
    {"t0"},     {"t1"},     {"wr"},     {"rd"},     // b4 - b7
    {"px0"},    {"pt0"},    {"px1"},    {"pt1"},    // b8 - bb
    {"ps"},     {"ip.5"},   {"ip.6"},   {"ip.7"},   // bc - bf
    {"0c0h.0"}, {"0c0h.1"}, {"0c0h.2"}, {"0c0h.3"}, // c0 - c3
    {"0c0h.4"}, {"0c0h.5"}, {"0c0h.6"}, {"0c0h.7"}, // c4 - c7
    {"cprl2"},  {"ct2"},    {"tr2"},    {"exen2"},  // c8 - cb
    {"tclk"},   {"rclk"},   {"exf2"},   {"tf2"},    // cc - cf
    {"p"},      {"psw.1"},  {"ov"},     {"rs0"},    // d0 - d3
    {"rs1"},    {"f0"},     {"ac"},     {"cy"},     // d4 - d7
    {"ct0"},    {"ct1"},    {"i2cfg.2"},{"i2cfg.3"},// d8 - db
    {"tirun"},  {"clrti"},  {"mastrq"}, {"slaven"}, // dc - df
    {"acc.0"},  {"acc.1"},  {"acc.2"},  {"acc.3"},  // e0 - e3
    {"acc.4"},  {"acc.5"},  {"acc.6"},  {"acc.7"},  // e4 - e7
    {"ibf"},    {"obf"},    {"idsm"},   {"obfc"},   // e8 - eb
    {"ma0"},    {"ma1"},    {"mb0"},    {"mb1"},    // ec - ef
    {"b.0"},    {"b.1"},    {"b.2"},    {"b.3"},    // f0 - f3
    {"b.4"},    {"b.5"},    {"b.6"},    {"b.7"},    // f4 - f7
    {"xstp"},   {"xstr"},   {"makstp"}, {"makstr"}, // f8 - fb
    {"xactv"},  {"xdata"},  {"idle"},   {"i2sta.7"} // fc - ff
} ;

// Names for Special Function Register bits for Keil A51 compatibility

struct sfrentry keilsfrbits[128] = {
    {"p0.0"},   {"p0.1"},   {"p0.2"},   {"p0.3"},   // 80 - 83
    {"p0.4"},   {"p0.5"},   {"p0.6"},   {"p0.7"},   // 84 - 87
    {"it0"},    {"ie0"},    {"it1"},    {"ie1"},    // 88 - 8b
    {"tr0"},    {"tf0"},    {"tr1"},    {"tf1"},    // 8c - 8f
    {"p1.0"},   {"p1.1"},   {"p1.2"},   {"p1.3"},   // 90 - 93
    {"p1.4"},   {"p1.5"},   {"p1.6"},   {"p1.7"},   // 94 - 97
    {"ri"},     {"ti"},     {"rb8"},    {"tb8"},    // 98 - 9b
    {"ren"},    {"sm2"},    {"sm1"},    {"sm0"},    // 9c - 9f
    {"p2.0"},   {"p2.1"},   {"p2.2"},   {"p2.3"},   // a0 - a3
    {"p2.4"},   {"p2.5"},   {"p2.6"},   {"p2.7"},   // a4 - a7
    {"ex0"},    {"et0"},    {"ex1"},    {"et1"},    // a8 - ab
    {"es"},     {"ie.5"},   {"ie.6"},   {"ea"},     // ac - af
    {"rxd"},    {"txd"},    {"int0"},   {"int1"},   // b0 - b3
    {"t0"},     {"t1"},     {"wr"},     {"rd"},     // b4 - b7
    {"px0"},    {"pt0"},    {"px1"},    {"pt1"},    // b8 - bb
    {"ps"},     {"ip.5"},   {"ip.6"},   {"ip.7"},   // bc - bf
    {"0c0h.0"}, {"0c0h.1"}, {"0c0h.2"}, {"0c0h.3"}, // c0 - c3
    {"0c0h.4"}, {"0c0h.5"}, {"0c0h.6"}, {"0c0h.7"}, // c4 - c7
    {"0c8h.0"}, {"0c8h.1"}, {"0c8h.2"}, {"0c8h.3"}, // c8 - cb
    {"0c8h.4"}, {"0c8h.5"}, {"0c8h.6"}, {"0c8h.7"}, // cc - cf
    {"p"},      {"psw.1"},  {"ov"},     {"rs0"},    // d0 - d3
    {"rs1"},    {"f0"},     {"ac"},     {"cy"},     // d4 - d7
    {"0d8h.0"}, {"0d8h.1"}, {"0d8h.2"}, {"0d8h.3"}, // d8 - db
    {"0d8h.4"}, {"0d8h.5"}, {"0d8h.6"}, {"0d8h.7"}, // dc - df
    {"acc.0"},  {"acc.1"},  {"acc.2"},  {"acc.3"},  // e0 - e3
    {"acc.4"},  {"acc.5"},  {"acc.6"},  {"acc.7"},  // e4 - e7
    {"0e8h.0"}, {"0e8h.1"}, {"0e8h.2"}, {"0e8h.3"}, // e8 - eb
    {"0e8h.4"}, {"0e8h.5"}, {"0e8h.6"}, {"0e8h.7"}, // ec - ef
    {"b.0"},    {"b.1"},    {"b.2"},    {"b.3"},    // f0 - f3
    {"b.4"},    {"b.5"},    {"b.6"},    {"b.7"},    // f4 - f7
    {"0f8h.0"}, {"0f8h.1"}, {"0f8h.2"}, {"0f8h.3"}, // f8 - fb
    {"0f8h.4"}, {"0f8h.5"}, {"0f8h.6"}, {"0f8h.7"}  // fc - ff
} ;

// Names for bit addressable memory

struct sfrentry membits[128] = {
    {"20h.0"},  {"20h.1"},  {"20h.2"},  {"20h.3"},
    {"20h.4"},  {"20h.5"},  {"20h.6"},  {"20h.7"},
    {"21h.0"},  {"21h.1"},  {"21h.2"},  {"21h.3"},
    {"21h.4"},  {"21h.5"},  {"21h.6"},  {"21h.7"},
    {"22h.0"},  {"22h.1"},  {"22h.2"},  {"22h.3"},
    {"22h.4"},  {"22h.5"},  {"22h.6"},  {"22h.7"},
    {"23h.0"},  {"23h.1"},  {"23h.2"},  {"23h.3"},
    {"23h.4"},  {"23h.5"},  {"23h.6"},  {"23h.7"},
    {"24h.0"},  {"24h.1"},  {"24h.2"},  {"24h.3"},
    {"24h.4"},  {"24h.5"},  {"24h.6"},  {"24h.7"},
    {"25h.0"},  {"25h.1"},  {"25h.2"},  {"25h.3"},
    {"25h.4"},  {"25h.5"},  {"25h.6"},  {"25h.7"},
    {"26h.0"},  {"26h.1"},  {"26h.2"},  {"26h.3"},
    {"26h.4"},  {"26h.5"},  {"26h.6"},  {"26h.7"},
    {"27h.0"},  {"27h.1"},  {"27h.2"},  {"27h.3"},
    {"27h.4"},  {"27h.5"},  {"27h.6"},  {"27h.7"},
    {"28h.0"},  {"28h.1"},  {"28h.2"},  {"28h.3"},
    {"28h.4"},  {"28h.5"},  {"28h.6"},  {"28h.7"},
    {"29h.0"},  {"29h.1"},  {"29h.2"},  {"29h.3"},
    {"29h.4"},  {"29h.5"},  {"29h.6"},  {"29h.7"},
    {"2ah.0"},  {"2ah.1"},  {"2ah.2"},  {"2ah.3"},
    {"2ah.4"},  {"2ah.5"},  {"2ah.6"},  {"2ah.7"},
    {"2bh.0"},  {"2bh.1"},  {"2bh.2"},  {"2bh.3"},
    {"2bh.4"},  {"2bh.5"},  {"2bh.6"},  {"2bh.7"},
    {"2ch.0"},  {"2ch.1"},  {"2ch.2"},  {"2ch.3"},
    {"2ch.4"},  {"2ch.5"},  {"2ch.6"},  {"2ch.7"},
    {"2dh.0"},  {"2dh.1"},  {"2dh.2"},  {"2dh.3"},
    {"2dh.4"},  {"2dh.5"},  {"2dh.6"},  {"2dh.7"},
    {"2eh.0"},  {"2eh.1"},  {"2eh.2"},  {"2eh.3"},
    {"2eh.4"},  {"2eh.5"},  {"2eh.6"},  {"2eh.7"},
    {"2fh.0"},  {"2fh.1"},  {"2fh.2"},  {"2fh.3"},
    {"2fh.4"},  {"2fh.5"},  {"2fh.6"},  {"2fh.7"},
};

// Names for bit addressable memory for Keil A51 compatibility

struct sfrentry keilmembits[128] = {
    {"00h.0"},  {"00h.1"},  {"00h.2"},  {"00h.3"},
    {"00h.4"},  {"00h.5"},  {"00h.6"},  {"00h.7"},
    {"08h.0"},  {"08h.1"},  {"08h.2"},  {"08h.3"},
    {"08h.4"},  {"08h.5"},  {"08h.6"},  {"08h.7"},
    {"10h.0"},  {"10h.1"},  {"10h.2"},  {"10h.3"},
    {"10h.4"},  {"10h.5"},  {"10h.6"},  {"10h.7"},
    {"18h.0"},  {"18h.1"},  {"18h.2"},  {"18h.3"},
    {"18h.4"},  {"18h.5"},  {"18h.6"},  {"18h.7"},
    {"20h.0"},  {"20h.1"},  {"20h.2"},  {"20h.3"},
    {"20h.4"},  {"20h.5"},  {"20h.6"},  {"20h.7"},
    {"28h.0"},  {"28h.1"},  {"28h.2"},  {"28h.3"},
    {"28h.4"},  {"28h.5"},  {"28h.6"},  {"28h.7"},
    {"30h.0"},  {"30h.1"},  {"30h.2"},  {"30h.3"},
    {"30h.4"},  {"30h.5"},  {"30h.6"},  {"30h.7"},
    {"38h.0"},  {"38h.1"},  {"38h.2"},  {"38h.3"},
    {"38h.4"},  {"38h.5"},  {"38h.6"},  {"38h.7"},
    {"40h.0"},  {"40h.1"},  {"40h.2"},  {"40h.3"},
    {"40h.4"},  {"40h.5"},  {"40h.6"},  {"40h.7"},
    {"48h.0"},  {"48h.1"},  {"48h.2"},  {"48h.3"},
    {"48h.4"},  {"48h.5"},  {"48h.6"},  {"48h.7"},
    {"50h.0"},  {"50h.1"},  {"50h.2"},  {"50h.3"},
    {"50h.4"},  {"50h.5"},  {"50h.6"},  {"50h.7"},
    {"58h.0"},  {"58h.1"},  {"58h.2"},  {"58h.3"},
    {"58h.4"},  {"58h.5"},  {"58h.6"},  {"58h.7"},
    {"60h.0"},  {"60h.1"},  {"60h.2"},  {"60h.3"},
    {"60h.4"},  {"60h.5"},  {"60h.6"},  {"60h.7"},
    {"68h.0"},  {"68h.1"},  {"68h.2"},  {"68h.3"},
    {"68h.4"},  {"68h.5"},  {"68h.6"},  {"68h.7"},
    {"70h.0"},  {"70h.1"},  {"70h.2"},  {"70h.3"},
    {"70h.4"},  {"70h.5"},  {"70h.6"},  {"70h.7"},
    {"78h.0"},  {"78h.1"},  {"78h.2"},  {"78h.3"},
    {"78h.4"},  {"78h.5"},  {"78h.6"},  {"78h.7"},
};

// Names for direct addressing of registers and internal data memory

struct sfrentry rbname[128] = {
    {"rb0r0"},  {"rb0r1"},  {"rb0r2"},  {"rb0r3"},
    {"rb0r4"},  {"rb0r5"},  {"rb0r6"},  {"rb0r7"},
    {"rb1r0"},  {"rb1r1"},  {"rb1r2"},  {"rb1r3"},
    {"rb1r4"},  {"rb1r5"},  {"rb1r6"},  {"rb1r7"},
    {"rb2r0"},  {"rb2r1"},  {"rb2r2"},  {"rb2r3"},
    {"rb2r4"},  {"rb2r5"},  {"rb2r6"},  {"rb2r7"},
    {"rb3r0"},  {"rb3r1"},  {"rb3r2"},  {"rb3r3"},
    {"rb3r4"},  {"rb3r5"},  {"rb3r6"},  {"rb3r7"},
    {"20h"},    {"21h"},    {"22h"},    {"23h"},
    {"24h"},    {"25h"},    {"26h"},    {"27h"},
    {"28h"},    {"29h"},    {"2ah"},    {"2bh"},
    {"2ch"},    {"2dh"},    {"2eh"},    {"2fh"},
    {"30h"},    {"31h"},    {"32h"},    {"33h"},
    {"34h"},    {"35h"},    {"36h"},    {"37h"},
    {"38h"},    {"39h"},    {"3ah"},    {"3bh"},
    {"3ch"},    {"3dh"},    {"3eh"},    {"3fh"},
    {"40h"},    {"41h"},    {"42h"},    {"43h"},
    {"44h"},    {"45h"},    {"46h"},    {"47h"},
    {"48h"},    {"49h"},    {"4ah"},    {"4bh"},
    {"4ch"},    {"4dh"},    {"4eh"},    {"4fh"},
    {"50h"},    {"51h"},    {"52h"},    {"53h"},
    {"54h"},    {"55h"},    {"56h"},    {"57h"},
    {"58h"},    {"59h"},    {"5ah"},    {"5bh"},
    {"5ch"},    {"5dh"},    {"5eh"},    {"5fh"},
    {"60h"},    {"61h"},    {"62h"},    {"63h"},
    {"64h"},    {"65h"},    {"66h"},    {"67h"},
    {"68h"},    {"69h"},    {"6ah"},    {"6bh"},
    {"6ch"},    {"6dh"},    {"6eh"},    {"6fh"},
    {"70h"},    {"71h"},    {"72h"},    {"73h"},
    {"74h"},    {"75h"},    {"76h"},    {"77h"},
    {"78h"},    {"79h"},    {"7ah"},    {"7bh"},
    {"7ch"},    {"7dh"},    {"7eh"},    {"7fh"},
};

// Default cycles for standard 8051

unsigned char cycles[256] = {
	1,2,2,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 00-0f
	2,2,2,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 10-1f
	2,2,2,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 20-2f
	2,2,2,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 30-3f

	2,2,1,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 40-4f
	2,2,1,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 50-5f
	2,2,1,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 60-6f
	2,2,2,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 70-7f

	2,2,2,2, 4,2,2,2, 2,2,2,2, 2,2,2,2,	// 80-8f
	2,2,2,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 90-9f
	2,2,1,2, 4,1,2,2, 2,2,2,2, 2,2,2,2,	// a0-af
	2,2,1,1, 2,2,2,2, 2,2,2,2, 2,2,2,2,	// b0-bf

	2,2,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// c0-cf
	2,2,1,1, 1,2,1,1, 2,2,2,2, 2,2,2,2,	// d0-df
	2,2,2,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// e0-ef
	2,2,2,2, 1,1,1,1, 1,1,1,1, 1,1,1,1	// f0-ff
};

// alternative table for "taken" in conditional, and
// "non-last autorepeated (BC>0)" cycles (latter not in '51, but Z80)
// this default table is the same as cycles[] as it is true for the vanilla 8051

unsigned char cycles2[256] = {
	1,2,2,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 00-0f
	2,2,2,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 10-1f
	2,2,2,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 20-2f
	2,2,2,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 30-3f

	2,2,1,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 40-4f
	2,2,1,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 50-5f
	2,2,1,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 60-6f
	2,2,2,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 70-7f

	2,2,2,2, 4,2,2,2, 2,2,2,2, 2,2,2,2,	// 80-8f
	2,2,2,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// 90-9f
	2,2,1,2, 4,1,2,2, 2,2,2,2, 2,2,2,2,	// a0-af
	2,2,1,1, 2,2,2,2, 2,2,2,2, 2,2,2,2,	// b0-bf

	2,2,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// c0-cf
	2,2,1,1, 1,2,1,1, 2,2,2,2, 2,2,2,2,	// d0-df
	2,2,2,2, 1,1,1,1, 1,1,1,1, 1,1,1,1,	// e0-ef
	2,2,2,2, 1,1,1,1, 1,1,1,1, 1,1,1,1	// f0-ff
};

//  end of d52table.c

