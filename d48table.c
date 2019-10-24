
/*
 * D48 8048/8041 Disassembler
 * Copyright (C) 1995-2007 by Jeffery L. Post
 * j_post <AT> pacbell <DOT> net
 *
 * d48table.c - 8048 Disassembly Tables
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

#include	"d48.h"

/* flags entry in opcode table:
    0 = single byte opcode, no options
    1 = double byte, immediate data
    2 = double byte, current page address (djnz, jnz, etc)
    4 = double byte, extended address (jmp and call)
    8 = single byte, invalid opcode (will become DB in output)
if flags + 80H, is unconditional transfer code (JMP, RET, etc)
*/

byte optbl[256] = {
    0x00,   0x00,   0x00,   0x01,   // 00 - 03
    0x85,   0x00,   0x00,   0x00,   // 04 - 07
    0x00,   0x00,   0x00,   0x00,   // 08 - 0b
    0x00,   0x00,   0x00,   0x00,   // 0c - 0f
    0x00,   0x00,   0x02,   0x01,   // 10 - 13
    0x05,   0x00,   0x02,   0x00,   // 14 - 17
    0x00,   0x00,   0x00,   0x00,   // 18 - 1b
    0x00,   0x00,   0x00,   0x00,   // 1c - 1f
    0x00,   0x00,   0x04,   0x01,   // 20 - 23
    0x85,   0x00,   0x02,   0x00,   // 24 - 27
    0x00,   0x00,   0x00,   0x00,   // 28 - 2b
    0x00,   0x00,   0x00,   0x00,   // 2c - 2f
    0x00,   0x00,   0x02,   0x04,   // 30 - 33
    0x05,   0x00,   0x02,   0x00,   // 34 - 37
    0x04,   0x00,   0x00,   0x04,   // 38 - 3b
    0x00,   0x00,   0x00,   0x00,   // 3c - 3f
    0x00,   0x00,   0x00,   0x01,   // 40 - 43
    0x85,   0x00,   0x02,   0x00,   // 44 - 47
    0x00,   0x00,   0x00,   0x00,   // 48 - 4b
    0x00,   0x00,   0x00,   0x00,   // 4c - 4f
    0x00,   0x00,   0x02,   0x01,   // 50 - 53
    0x05,   0x00,   0x02,   0x00,   // 54 - 57
    0x00,   0x00,   0x00,   0x00,   // 58 - 5b
    0x00,   0x00,   0x00,   0x00,   // 5c - 5f
    0x00,   0x00,   0x00,   0x04,   // 60 - 63
    0x85,   0x00,   0x04,   0x00,   // 64 - 67
    0x00,   0x00,   0x00,   0x00,   // 68 - 6b
    0x00,   0x00,   0x00,   0x00,   // 6c - 6f
    0x00,   0x00,   0x02,   0x04,   // 70 - 73
    0x05,   0x00,   0x02,   0x00,   // 74 - 77
    0x00,   0x00,   0x00,   0x00,   // 78 - 7b
    0x00,   0x00,   0x00,   0x00,   // 7c - 7f
    0x00,   0x00,   0x04,   0x80,   // 80 - 83
    0x85,   0x00,   0x02,   0x04,   // 84 - 87
    0x01,   0x01,   0x01,   0x04,   // 88 - 8b
    0x00,   0x00,   0x00,   0x00,   // 8c - 8f
    0x00,   0x00,   0x02,   0x80,   // 90 - 93
    0x05,   0x00,   0x02,   0x00,   // 94 - 97
    0x01,   0x01,   0x01,   0x04,   // 98 - 9b
    0x00,   0x00,   0x00,   0x00,   // 9c - 9f
    0x00,   0x00,   0x04,   0x00,   // a0 - a3
    0x85,   0x00,   0x04,   0x00,   // a4 - a7
    0x00,   0x00,   0x00,   0x00,   // a8 - ab
    0x00,   0x00,   0x00,   0x00,   // ac - af
    0x01,   0x01,   0x02,   0x80,   // b0 - b3
    0x05,   0x00,   0x02,   0x04,   // b4 - b7
    0x01,   0x01,   0x01,   0x01,   // b8 - bb
    0x01,   0x01,   0x01,   0x01,   // bc - bf
    0x04,   0x04,   0x04,   0x04,   // c0 - c3
    0x85,   0x00,   0x02,   0x00,   // c4 - c7
    0x00,   0x00,   0x00,   0x00,   // c8 - cb
    0x00,   0x00,   0x00,   0x00,   // cc - cf
    0x00,   0x00,   0x02,   0x01,   // d0 - d3
    0x05,   0x00,   0x04,   0x00,   // d4 - d7
    0x00,   0x00,   0x00,   0x00,   // d8 - db
    0x00,   0x00,   0x00,   0x00,   // dc - df
    0x04,   0x04,   0x04,   0x00,   // e0 - e3
    0x85,   0x00,   0x02,   0x00,   // e4 - e7
    0x02,   0x02,   0x02,   0x02,   // e8 - eb
    0x02,   0x02,   0x02,   0x02,   // ec - ef
    0x00,   0x00,   0x02,   0x04,   // f0 - f3
    0x05,   0x00,   0x02,   0x00,   // f4 - f7
    0x00,   0x00,   0x00,   0x00,   // f8 - fb
    0x00,   0x00,   0x00,   0x00    // fc - ff
};

byte bctbl[256] = {
    1,  1,  1,  2,  2,  1,  1,  1,      // 00 - 07
    1,  1,  1,  1,  1,  1,  1,  1,      // 08 - 0f
    1,  1,  2,  2,  2,  1,  2,  1,      // 10 - 17
    1,  1,  1,  1,  1,  1,  1,  1,      // 18 - 1f
    1,  1,  1,  2,  2,  1,  2,  1,      // 20 - 27
    1,  1,  1,  1,  1,  1,  1,  1,      // 28 - 2f
    1,  1,  2,  1,  2,  1,  2,  1,      // 30 - 37
    1,  1,  1,  1,  1,  1,  1,  1,      // 38 - 3f
    1,  1,  1,  2,  2,  1,  2,  1,      // 40 - 47
    1,  1,  1,  1,  1,  1,  1,  1,      // 48 - 4f
    1,  1,  2,  2,  2,  1,  2,  1,      // 50 - 57
    1,  1,  1,  1,  1,  1,  1,  1,      // 58 - 5f
    1,  1,  1,  1,  2,  1,  1,  1,      // 60 - 67
    1,  1,  1,  1,  1,  1,  1,  1,      // 68 - 6f
    1,  1,  2,  1,  2,  1,  2,  1,      // 70 - 77
    1,  1,  1,  1,  1,  1,  1,  1,      // 78 - 7f
    1,  1,  1,  1,  2,  1,  2,  1,      // 80 - 87
    2,  2,  2,  1,  1,  1,  1,  1,      // 88 - 8f
    1,  1,  2,  1,  2,  1,  2,  1,      // 90 - 97
    2,  2,  2,  1,  1,  1,  1,  1,      // 98 - 9f
    1,  1,  1,  1,  2,  1,  1,  1,      // a0 - a7
    1,  1,  1,  1,  1,  1,  1,  1,      // a8 - af
    2,  2,  2,  1,  2,  1,  2,  1,      // b0 - b7
    2,  2,  2,  2,  2,  2,  2,  2,      // b8 - bf
    1,  1,  1,  1,  2,  1,  2,  1,      // c0 - c7
    1,  1,  1,  1,  1,  1,  1,  1,      // c8 - cf
    1,  1,  2,  2,  2,  1,  1,  1,      // d0 - d7
    1,  1,  1,  1,  1,  1,  1,  1,      // d8 - df
    1,  1,  1,  1,  2,  1,  2,  1,      // e0 - e7
    2,  2,  2,  2,  2,  2,  2,  2,      // e8 - ef
    1,  1,  2,  1,  2,  1,  2,  1,      // f0 - f7
    1,  1,  1,  1,  1,  1,  1,  1       // f8 - ff
};

struct mnementry mnemtbl[256] =
{   {"nop"},            {"idl"},            // 00 - 01
    {"outl bus,a"},     {"add a,#"},        // 02 - 03
    {"jmp "},           {"en i"},           // 04 - 05
    {""},               {"dec a"},          // 06 - 07
    {"ins a,bus"},      {"in a,p1"},        // 08 - 09
    {"in a,p2"},        {""},               // 0a - 0b
    {"movd a,p4"},      {"movd a,p5"},      // 0c - 0d
    {"movd a,p6"},      {"movd a,p7"},      // 0e - 0f
    {"inc @r0"},        {"inc @r1"},        // 10 - 11
    {"jb0 "},           {"addc a,#"},       // 12 - 13
    {"call "},          {"dis i"},          // 14 - 15
    {"jtf "},           {"inc a"},          // 16 - 17
    {"inc r0"},         {"inc r1"},         // 18 - 19
    {"inc r2"},         {"inc r3"},         // 1a - 1b
    {"inc r4"},         {"inc r5"},         // 1c - 1d
    {"inc r6"},         {"inc r7"},         // 1e - 1f
    {"xch a,@r0"},      {"xch a,@r1"},      // 20 - 21
    {""},               {"mov a,#"},        // 22 - 23
    {"jmp "},           {"en tcnti"},       // 24 - 25
    {"jnt0 "},          {"clr a"},          // 26 - 27
    {"xch a,r0"},       {"xch a,r1"},       // 28 - 29
    {"xch a,r2"},       {"xch a,r3"},       // 2a - 2b
    {"xch a,r4"},       {"xch a,r5"},       // 2c - 2d
    {"xch a,r6"},       {"xch a,r7"},       // 2e - 2f
    {"xchd a,@r0"},     {"xchd a,@r1"},     // 30 - 31
    {"jb1 "},           {""},               // 32 - 33
    {"call "},          {"dis tcnti"},      // 34 - 35
    {"jt0 "},           {"cpl a"},          // 36 - 37
    {""},               {"outl p1,a"},      // 38 - 39
    {"outl p2,a"},      {""},               // 3a - 3b
    {"movd p4,a"},      {"movd p5,a"},      // 3c - 3d
    {"movd p6,a"},      {"movd p7,a"},      // 3e - 3f
    {"orl a,@r0"},      {"orl a,@r1"},      // 40 - 41
    {"mov a,t"},        {"orl a,#"},        // 42 - 43
    {"jmp "},           {"strt cnt"},       // 44 - 45
    {"jnt1 "},          {"swap a"},         // 46 - 47
    {"orl a,r0"},       {"orl a,r1"},       // 48 - 49
    {"orl a,r2"},       {"orl a,r3"},       // 4a - 4b
    {"orl a,r4"},       {"orl a,r5"},       // 4c - 4d
    {"orl a,r6"},       {"orl a,r7"},       // 4e - 4f
    {"anl a,@r0"},      {"anl a,@r1"},      // 50 - 51
    {"jb2 "},           {"anl a,#"},        // 52 - 53
    {"call "},          {"strt t"},         // 54 - 55
    {"jt1 "},           {"da a"},           // 56 - 57
    {"anl a,r0"},       {"anl a,r1"},       // 58 - 59
    {"anl a,r2"},       {"anl a,r3"},       // 5a - 5b
    {"anl a,r4"},       {"anl a,r5"},       // 5c - 5d
    {"anl a,r6"},       {"anl a,r7"},       // 5e - 5f
    {"add a,@r0"},      {"add a,@r1"},      // 60 - 61
    {"mov t,a"},        {""},               // 62 - 63
    {"jmp "},           {"stop tcnt"},      // 64 - 65
    {""},               {"rrc a"},          // 66 - 67
    {"add a,r0"},       {"add a,r1"},       // 68 - 69
    {"add a,r2"},       {"add a,r3"},       // 6a - 6b
    {"add a,r4"},       {"add a,r5"},       // 6c - 6d
    {"add a,r6"},       {"add a,r7"},       // 6e - 6f
    {"addc a,@r0"},     {"addc a,@r1"},     // 70 - 71
    {"jb3 "},           {""},               // 72 - 73
    {"call "},          {"ent0 clk"},       // 74 - 75
    {"jf1 "},           {"rr a"},           // 76 - 77
    {"addc a,r0"},      {"addc a,r1"},      // 78 - 79
    {"addc a,r2"},      {"addc a,r3"},      // 7a - 7b
    {"addc a,r4"},      {"addc a,r5"},      // 7c - 7d
    {"addc a,r6"},      {"addc a,r7"},      // 7e - 7f
    {"movx a,@r0"},     {"movx a,@r1"},     // 80 - 81
    {""},               {"ret"},            // 82 - 83
    {"jmp "},           {"clr f0"},         // 84 - 85
    {"jni "},           {""},               // 86 - 87
    {"orl bus,#"},      {"orl p1,#"},       // 88 - 89
    {"orl p2,#"},       {""},               // 8a - 8b
    {"orld p4,a"},      {"orld p5,a"},      // 8c - 8d
    {"orld p6,a"},      {"orld p7,a"},      // 8e - 8f
    {"movx @r0,a"},     {"movx @r1,a"},     // 90 - 91
    {"jb4 "},           {"retr"},           // 92 - 93
    {"call "},          {"cpl f0"},         // 94 - 95
    {"jnz "},           {"clr c"},          // 96 - 97
    {"anl bus,#"},      {"anl p1,#"},       // 98 - 99
    {"anl p2,#"},       {""},               // 9a - 9b
    {"anld p4,a"},      {"anld p5,a"},      // 9c - 9d
    {"anld p6,a"},      {"anld p7,a"},      // 9e - 9f
    {"mov @r0,a"},      {"mov @r1,a"},      // a0 - a1
    {""},               {"movp a,@a"},      // a2 - a3
    {"jmp "},           {"clr f1"},         // a4 - a5
    {""},               {"cpl c"},          // a6 - a7
    {"mov r0,a"},       {"mov r1,a"},       // a8 - a9
    {"mov r2,a"},       {"mov r3,a"},       // aa - ab
    {"mov r4,a"},       {"mov r5,a"},       // ac - ad
    {"mov r6,a"},       {"mov r7,a"},       // ae - af
    {"mov @r0,#"},      {"mov @r1,#"},      // b0 - b1
    {"jb5 "},           {"jmpp @a"},        // b2 - b3
    {"call "},          {"cpl f1"},         // b4 - b5
    {"jf0 "},           {""},               // b6 - b7
    {"mov r0,#"},       {"mov r1,#"},       // b8 - b9
    {"mov r2,#"},       {"mov r3,#"},       // ba - bb
    {"mov r4,#"},       {"mov r5,#"},       // bc - bd
    {"mov r6,#"},       {"mov r7,#"},       // be - bf
    {""},               {""},               // c0 - c1
    {""},               {""},               // c2 - c3
    {"jmp "},           {"sel rb0"},        // c4 - c5
    {"jz "},            {"mov a,psw"},      // c6 - c7
    {"dec r0"},         {"dec r1"},         // c8 - c9
    {"dec r2"},         {"dec r3"},         // ca - cb
    {"dec r4"},         {"dec r5"},         // cc - cd
    {"dec r6"},         {"dec r7"},         // ce - cf
    {"xrl a,@r0"},      {"xrl a,@r1"},      // d0 - d1
    {"jb6 "},           {"xrl a,#"},        // d2 - d3
    {"call "},          {"sel rb1"},        // d4 - d5
    {""},               {"mov psw,a"},      // d6 - d7
    {"xrl a,r0"},       {"xrl a,r1"},       // d8 - d9
    {"xrl a,r2"},       {"xrl a,r3"},       // da - db
    {"xrl a,r4"},       {"xrl a,r5"},       // dc - dd
    {"xrl a,r6"},       {"xrl a,r7"},       // de - df
    {""},               {""},               // e0 - e1
    {""},               {"movp3 a,@a"},     // e2 - e3
    {"jmp "},           {"sel mb0"},        // e4 - e5
    {"jnc "},           {"rl a"},           // e6 - e7
    {"djnz r0,"},       {"djnz r1,"},       // e8 - e9
    {"djnz r2,"},       {"djnz r3,"},       // ea - eb
    {"djnz r4,"},       {"djnz r5,"},       // ec - ed
    {"djnz r6,"},       {"djnz r7,"},       // ee - ef
    {"mov a,@r0"},      {"mov a,@r1"},      // f0 - f1
    {"jb7 "},           {""},               // f2 - f3
    {"call "},          {"sel mb1"},        // f4 - f5
    {"jc "},            {"rlc a"},          // f6 - f7
    {"mov a,r0"},       {"mov a,r1"},       // f8 - f9
    {"mov a,r2"},       {"mov a,r3"},       // fa - fb
    {"mov a,r4"},       {"mov a,r5"},       // fc - fd
    {"mov a,r6"},       {"mov a,r7"}        // fe - ff
};

// Register names

struct sfrentry rbname[] = {
    {"rb0r0"},  {"rb0r1"},  {"rb0r2"},  {"rb0r3"},
    {"rb0r4"},  {"rb0r5"},  {"rb0r6"},  {"rb0r7"},
    {"rb1r0"},  {"rb1r1"},  {"rb1r2"},  {"rb1r3"},
    {"rb1r4"},  {"rb1r5"},  {"rb1r6"},  {"rb1r7"},
} ;

// Default cycles for standard 8048
// No cycle count for unimplemented opcodes:
// 06, 0b, 22, 33,
// 38, 3b, 63, 66,
// 73, 82, 87, 8b,
// 9b, a2, a6, b7,
// c0, c1, c2, c3,
// d6, e0, e1, e2,
// f3

unsigned char cycles[256] = {
	1,1,2,2, 2,1,0,1, 2,2,2,0, 2,2,2,2,	// 00-0f
	1,1,2,2, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// 10-1f
	1,1,0,2, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// 20-2f
	1,1,2,0, 2,1,2,1, 0,2,2,0, 2,2,2,2,	// 30-3f

	1,1,1,2, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// 40-4f
	1,1,2,2, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// 50-5f
	1,1,1,0, 2,1,0,1, 1,1,1,1, 1,1,1,1,	// 60-6f
	1,1,2,0, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// 70-7f

	2,2,0,2, 2,1,2,0, 2,2,2,0, 2,2,2,2,	// 80-8f
	2,2,2,2, 2,1,2,1, 2,2,2,0, 2,2,2,2,	// 90-9f
	1,1,0,2, 2,1,0,1, 1,1,1,1, 1,1,1,1,	// a0-af
	2,2,2,2, 2,1,2,0, 2,2,2,2, 2,2,2,2,	// b0-bf

	0,0,0,0, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// c0-cf
	1,1,2,2, 2,1,0,1, 1,1,1,1, 1,1,1,1,	// d0-df
	0,0,0,2, 2,1,2,1, 2,2,2,2, 2,2,2,2,	// e0-ef
	1,1,2,0, 2,1,2,1, 1,1,1,1, 1,1,1,1	// f0-ff
};

// by default identical to cycles

unsigned char cycles2[256] = {
	1,1,2,2, 2,1,0,1, 2,2,2,0, 2,2,2,2,	// 00-0f
	1,1,2,2, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// 10-1f
	1,1,0,2, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// 20-2f
	1,1,2,0, 2,1,2,1, 0,2,2,0, 2,2,2,2,	// 30-3f

	1,1,1,2, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// 40-4f
	1,1,2,2, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// 50-5f
	1,1,1,0, 2,1,0,1, 1,1,1,1, 1,1,1,1,	// 60-6f
	1,1,2,0, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// 70-7f

	2,2,0,2, 2,1,2,0, 2,2,2,0, 2,2,2,2,	// 80-8f
	2,2,2,2, 2,1,2,1, 2,2,2,0, 2,2,2,2,	// 90-9f
	1,1,0,2, 2,1,0,1, 1,1,1,1, 1,1,1,1,	// a0-af
	2,2,2,2, 2,1,2,0, 2,2,2,2, 2,2,2,2,	// b0-bf

	0,0,0,0, 2,1,2,1, 1,1,1,1, 1,1,1,1,	// c0-cf
	1,1,2,2, 2,1,0,1, 1,1,1,1, 1,1,1,1,	// d0-df
	0,0,0,2, 2,1,2,1, 2,2,2,2, 2,2,2,2,	// e0-ef
	1,1,2,0, 2,1,2,1, 1,1,1,1, 1,1,1,1	// f0-ff
};

//  end of d48table.c
