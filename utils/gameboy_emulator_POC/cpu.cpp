#include <stdio.h>
#include "mem.h"
#include "rom.h"
#include "interrupt.h"

#define set_HL(x) do {unsigned int macro = (x); c.L = macro&0xFF; c.H = macro>>8;} while(0)
#define set_BC(x) do {unsigned int macro = (x); c.C = macro&0xFF; c.B = macro>>8;} while(0)
#define set_DE(x) do {unsigned int macro = (x); c.E = macro&0xFF; c.D = macro>>8;} while(0)
#define set_AF(x) do {unsigned int macro = (x); c.F = macro&0xFF; c.A = macro>>8;} while(0)

#define get_AF() ((c.A<<8) | c.F)
#define get_BC() ((c.B<<8) | c.C)
#define get_DE() ((c.D<<8) | c.E)
#define get_HL() ((c.H<<8) | c.L)

/* Flags */
#define set_Z(x) c.F = ((c.F&0x7F) | ((x)<<7))
#define set_N(x) c.F = ((c.F&0xBF) | ((x)<<6))
#define set_H(x) c.F = ((c.F&0xDF) | ((x)<<5))
#define set_C(x) c.F = ((c.F&0xEF) | ((x)<<4))

#define flag_Z !!((c.F & 0x80))
#define flag_N !!((c.F & 0x40))
#define flag_H !!((c.F & 0x20))
#define flag_C !!((c.F & 0x10))

struct CPU {
	unsigned char H;
	unsigned char L;

	unsigned char D;
	unsigned char E;

	unsigned char B;
	unsigned char C;

	unsigned char A;
	unsigned char F;

	unsigned short SP;
	unsigned short PC;
	unsigned int cycles;
};

static struct CPU c;
static int is_debugged;
static int halted;

void cpu_init(void)
{
	set_AF(0x01B0);
	set_BC(0x0013);
	set_DE(0x00D8);
	set_HL(0x014D);
	c.SP = 0xFFFE;
	c.PC = 0x0100;
	c.cycles = 0;
}

static void RLC(unsigned char reg)
{
	unsigned char t, old;

	switch(reg)
	{
		case 0:	/* B */
			old = !!(c.B&0x80);
			c.B = (c.B << 1) | old;
			set_C(old);
			set_Z(!c.B);
		break;
		case 1:	/* C */
			old = !!(c.C&0x80);
			set_C(old);
			c.C = c.C<<1 | old;
			set_Z(!c.C);
		break;
		case 2:	/* D */
			old = !!(c.D&0x80);
			set_C(old);
			c.D = c.D<<1 | old;
			set_Z(!c.D);
		break;
		case 3:	/* E */
			old = !!(c.E&0x80);
			set_C(old);
			c.E = c.E<<1 | old;
			set_Z(!c.E);
		break;
		case 4:	/* H */
			old = !!(c.H&0x80);
			set_C(old);
			c.H = c.H<<1 | old;
			set_Z(!c.H);
		break;
		case 5:	/* L */
			old = !!(c.L&0x80);
			set_C(old);
			c.L = c.L<<1 | old;
			set_Z(!c.L);
		break;
		case 6:	/* (HL) */
			t = mem_get_byte(get_HL());
			old = !!(t&0x80);
			set_C(old);
			t = t<<1 | old;
			mem_write_byte(get_HL(), t);
			set_Z(!t);
		break;
		case 7:	/* A */
			old = !!(c.A&0x80);
			c.A = (c.A<<1) | old;
			set_C(old);
			set_Z(!c.A);
		break;
	}

	set_N(0);
	set_H(0);
}

static void RRC(unsigned char reg)
{
	unsigned char t, old;

	switch(reg)
	{
		case 0:	/* B */
			old = c.B&1;
			set_C(old);
			c.B = c.B>>1 | old<<7;
			set_Z(!c.B);
		break;
		case 1:	/* C */
			old = c.C&1;
			set_C(old);
			c.C = c.C>>1 | old<<7;
			set_Z(!c.C);
		break;
		case 2:	/* D */
			old = c.D&1;
			set_C(old);
			c.D = c.D>>1 | old<<7;
			set_Z(!c.D);
		break;
		case 3:	/* E */
			old = c.E&1;
			set_C(old);
			c.E = c.E>>1 | old<<7;
			set_Z(!c.E);
		break;
		case 4:	/* H */
			old = c.H&1;
			set_C(old);
			c.H = c.H>>1 | old<<7;
			set_Z(!c.H);
		break;
		case 5:	/* L */
			old = c.L&1;
			set_C(old);
			c.L = c.L>>1 | old<<7;
			set_Z(!c.L);
		break;
		case 6:	/* (HL) */
			t = mem_get_byte(get_HL());
			old = t;
			set_C(old);
			t = t>>1 | old<<7;
			c.cycles += 2;
			mem_write_byte(get_HL(), t);
			set_Z(!t);
		break;
		case 7:	/* A */
			old = c.A&1;
			set_C(old);
			c.A = c.A>>1 | old<<7;
			set_Z(!c.A);
		break;
	}
	set_N(0);
	set_H(0);
}

static void RL(unsigned char reg)
{
	unsigned char t, t2;

	switch(reg)
	{
		case 0: /* B */
			t2 = flag_C;
			set_C(!!(c.B&0x80));
			c.B = (c.B << 1) | !!(t2);
			set_Z(!c.B);
		break;
		case 1: /* C */
			t2 = flag_C;
			set_C(!!(c.C&0x80));
			c.C = (c.C << 1) | !!(t2);
			set_Z(!c.C);
		break;
		case 2: /* D */
			t2 = flag_C;
			set_C(!!(c.D&0x80));
			c.D = (c.D << 1) | !!(t2);
			set_Z(!c.D);
		break;
		case 3: /* E */
			t2 = flag_C;
			set_C(!!(c.E&0x80));
			c.E = (c.E << 1) | !!(t2);
			set_Z(!c.E);
		break;
		case 4: /* H */
			t2 = flag_C;
			set_C(!!(c.H&0x80));
			c.H = (c.H << 1) | !!(t2);
			set_Z(!c.H);
		break;
		case 5: /* L */
			t2 = flag_C;
			set_C(!!(c.L&0x80));
			c.L = (c.L << 1) | !!(t2);
			set_Z(!c.L);
		break;
		case 6: /* (HL) */
			t = mem_get_byte(get_HL());
			t2 = flag_C;
			set_C(!!(t&0x80));
			t = (t << 1) | !!(t2);
			mem_write_byte(get_HL(), t);
			set_Z(!t);
			c.cycles += 2;
		break;
		case 7: /* A */
			t2 = flag_C;
			set_C(!!(c.A&0x80));
			c.A = (c.A << 1) | t2;
			set_Z(!c.A);
		break;
	}

	set_N(0);
	set_H(0);
}

static void RR(unsigned char reg)
{
	unsigned char t, t2;

	switch(reg)
	{
		case 0:	/* B */
			t2 = flag_C;
			set_C(c.B&1);
			c.B = (c.B >> 1) | t2<<7;
			set_Z(!c.B);
		break;
		case 1: /* C */
			t2 = flag_C;
			set_C(c.C&1);
			c.C = (c.C >> 1) | t2<<7;
			set_Z(!c.C);
		break;
		case 2: /* D */
			t2 = flag_C;
			set_C(c.D&1);
			c.D = (c.D >> 1) | t2<<7;
			set_Z(!c.D);
		break;
		case 3: /* E */
			t2 = flag_C;
			set_C(c.E&1);
			c.E = (c.E >> 1) | t2<<7;
			set_Z(!c.E);
		break;
		case 4: /* H */
			t2 = flag_C;
			set_C(c.H&1);
			c.H = (c.H >> 1) | t2<<7;
			set_Z(!c.H);
		break;
		case 5: /* L */
			t2 = flag_C;
			set_C(c.L&1);
			c.L = (c.L >> 1) | t2<<7;
			set_Z(!c.L);
		break;
		case 6: /* (HL) */
			t = mem_get_byte(get_HL());
			t2 = flag_C;
			set_C(t&1);
			t = (t >> 1) | t2<<7;
			set_Z(!t);
			mem_write_byte(get_HL(), t);
			c.cycles += 2;
		break;
		case 7: /* A */
			t2 = flag_C;
			set_C(c.A&1);
			c.A = (c.A >> 1) | (t2<<7);
			set_Z(!c.A);
		break;
	}
	set_N(0);
	set_H(0);
}

static void SLA(unsigned char reg)
{
	unsigned char t;

	switch(reg)
	{
		case 0: /* B */
			set_C(!!(c.B & 0x80));
			c.B = c.B << 1;
			set_Z(!c.B);
		break;
		case 1: /* C */
			set_C(!!(c.C & 0x80));
			c.C = c.C << 1;
			set_Z(!c.C);
		break;
		case 2: /* D */
			set_C(!!(c.D & 0x80));
			c.D = c.D << 1;
			set_Z(!c.D);
		break;
		case 3: /* E */
			set_C(!!(c.E & 0x80));
			c.E = c.E << 1;
			set_Z(!c.E);
		break;
		case 4: /* H */
			set_C(!!(c.H & 0x80));
			c.H = c.H << 1;
			set_Z(!c.H);
		break;
		case 5: /* L */
			set_C(!!(c.L & 0x80));
			c.L = c.L << 1;
			set_Z(!c.L);
		break;
		case 6: /* (HL) */
			t = mem_get_byte(get_HL());
			set_C(!!(t & 0x80));
			t = t << 1;
			mem_write_byte(get_HL(), t);
			set_Z(!t);
			c.cycles += 2;
		break;
		case 7: /* A */
			set_C(!!(c.A & 0x80));
			c.A = c.A << 1;
			set_Z(!c.A);
		break;
	}

	set_H(0);
	set_N(0);
}

static void SRA(unsigned char reg)
{
	unsigned char old, t;

	switch(reg)
	{
		case 0: /* B */
			set_C(c.B&1);
			old = c.B&0x80;
			c.B = c.B >> 1 | old;
			set_Z(!c.B);
		break;
		case 1: /* C */
			set_C(c.C&1);
			old = c.C&0x80;
			c.C = c.C >> 1 | old;
			set_Z(!c.C);
		break;
		case 2: /* D */
			set_C(c.D&1);
			old = c.D&0x80;
			c.D = c.D >> 1 | old;
			set_Z(!c.D);
		break;
		case 3: /* E */
			set_C(c.E&1);
			old = c.E&0x80;
			c.E = c.E >> 1 | old;
			set_Z(!c.E);
		break;
		case 4: /* H */
			set_C(c.H&1);
			old = c.H&0x80;
			c.H = c.H >> 1 | old;
			set_Z(!c.H);
		break;
		case 5: /* L */
			set_C(c.L&1);
			old = c.L&0x80;
			c.L = c.L >> 1 | old;
			set_Z(!c.L);
		break;
		case 6: /* (HL) */
			t = mem_get_byte(get_HL());
			set_C(t&1);
			old = t&0x80;
			t = t >> 1 | old;
			mem_write_byte(get_HL(), t);
			set_Z(!t);
		break;
		case 7: /* A */
			set_C(c.A&1);
			old = c.A&0x80;
			c.A = c.A >> 1 | old;
			set_Z(!c.A);
		break;
	}

	set_H(0);
	set_N(0);
}

static void SRL(unsigned char reg)
{
	unsigned char t;

	switch(reg)
	{
		case 0: /* B */
			set_C(c.B & 1);
			c.B = c.B >> 1;
			set_Z(!c.B);
		break;
		case 1: /* C */
			set_C(c.C & 1);
			c.C = c.C >> 1;
			set_Z(!c.C);
		break;
		case 2: /* D */
			set_C(c.D & 1);
			c.D = c.D >> 1;
			set_Z(!c.D);
		break;
		case 3: /* E */
			set_C(c.E & 1);
			c.E = c.E >> 1;
			set_Z(!c.E);
		break;
		case 4: /* H */
			set_C(c.H & 1);
			c.H = c.H >> 1;
			set_Z(!c.H);
		break;
		case 5: /* L */
			set_C(c.L & 1);
			c.L = c.L >> 1;
			set_Z(!c.L);
		break;
		case 6: /* (HL) */
			t = mem_get_byte(get_HL());
			set_C(t & 1);
			t = t >> 1;
			mem_write_byte(get_HL(), t);
			set_Z(!t);
			c.cycles += 2;
		break;
		case 7: /* A */
			set_C(c.A & 1);
			c.A = c.A >> 1;
			set_Z(!c.A);
		break;
	}

	set_H(0);
	set_N(0);
}

static void SWAP(unsigned char reg)
{
	unsigned char t;

	switch(reg)
	{
		case 0: /* B */
			c.B = ((c.B&0xF)<<4) | ((c.B&0xF0)>>4);
			c.F = (!c.B)<<7;
		break;
		case 1: /* C */
			c.C = ((c.C&0xF)<<4) | ((c.C&0xF0)>>4);
			c.F = (!c.C)<<7;
		break;
		case 2: /* D */
			c.D = ((c.D&0xF)<<4) | ((c.D&0xF0)>>4);
			c.F = (!c.D)<<7;
		break;
		case 3: /* E */
			c.E = ((c.E&0xF)<<4) | ((c.E&0xF0)>>4);
			c.F = (!c.E)<<7;
		break;
		case 4: /* H */
			c.H = ((c.H&0xF)<<4) | ((c.H&0xF0)>>4);
			c.F = (!c.H)<<7;
		break;
		case 5: /* L */
			c.L = ((c.L&0xF)<<4) | ((c.L&0xF0)>>4);
			c.F = (!c.L)<<7;
		break;
		case 6: /* (HL) */
			t = mem_get_byte(get_HL());
			t = ((t&0xF)<<4) | ((t&0xF0)>>4);
			mem_write_byte(get_HL(), t);
			c.F = (!t)<<7;
			c.cycles += 2;
		break;
		case 7: /* A */
			c.A = ((c.A&0xF)<<4) | ((c.A&0xF0)>>4);
			c.F = (!c.A)<<7;
		break;
	}
}

static void BIT(unsigned char bit, unsigned char reg)
{
	unsigned char t, f = 0 /* Make GCC happy */;

	switch(reg)
	{
		case 0: /* B */
		    f = !(c.B & bit);
		break;
		case 1: /* C */
		    f = !(c.C & bit);
		break;
		case 2: /* D */
		    f = !(c.D & bit);
		break;
		case 3: /* E */
		    f = !(c.E & bit);
		break;
		case 4: /* H */
		    f = !(c.H & bit);
		break;
		case 5: /* L */
		    f = !(c.L & bit);
		break;
		case 6: /* (HL) */
			t = mem_get_byte(get_HL());
			f = !(t & bit);
			c.cycles += 1;
		break;
		case 7: /* A */
		    f = !(c.A & bit);
		break;
	}

	set_Z(f);
	set_N(0);
	set_H(1);
}

static void RES(unsigned char bit, unsigned char reg)
{
	unsigned char t;

	switch(reg)
	{
		case 0: /* B */
			c.B &= ~bit;
		break;
		case 1: /* C */
			c.C &= ~bit;
		break;
		case 2: /* D */
			c.D &= ~bit;
		break;
		case 3: /* E */
			c.E &= ~bit;
		break;
		case 4: /* H */
			c.H &= ~bit;
		break;
		case 5: /* L */
			c.L &= ~bit;
		break;
		case 6: /* (HL) */
			t = mem_get_byte(get_HL());
			t &= ~bit;
			mem_write_byte(get_HL(), t);
			c.cycles += 2;
		break;
		case 7: /* A */
			c.A &= ~bit;
		break;
	}
}

static void SET(unsigned char bit, unsigned char reg)
{
	unsigned char t;

	switch(reg)
	{
		case 0: /* B */
			c.B |= bit;
		break;
		case 1: /* C */
			c.C |= bit;
		break;
		case 2: /* D */
			c.D |= bit;
		break;
		case 3: /* E */
			c.E |= bit;
		break;
		case 4: /* H */
			c.H |= bit;
		break;
		case 5: /* L */
			c.L |= bit;
		break;
		case 6: /* (HL) */
			t = mem_get_byte(get_HL());
			t |= bit;
			mem_write_byte(get_HL(), t);
			c.cycles += 2;
		break;
		case 7: /* A */
			c.A |= bit;
		break;
	}
}

/*
00000xxx = RLC xxx
00001xxx = RRC xxx
00010xxx = RL xxx
00011xxx = RR xxx
00100xxx = SLA xxx
00101xxx = SRA xxx
00110xxx = SWAP xxx
00111xxx = SRL xxx
01yyyxxx = BIT yyy, xxx
10yyyxxx = RES yyy, xxx
11yyyxxx = SET yyy, xxx
*/
static void decode_CB(unsigned char t)
{
	unsigned char reg, opcode, bit;
	void (*f[])(unsigned char) = {RLC, RRC, RL, RR, SLA, SRA, SWAP, SRL};
	void (*f2[])(unsigned char, unsigned char) = {BIT, RES, SET};

	reg = t&7;
	opcode = t>>3;
	if(opcode < 8)
	{
		f[opcode](reg);
		return;
	}

	bit = opcode&7;
	opcode >>= 3;
	f2[opcode-1](1<<bit, reg);
}

void cpu_interrupt(unsigned short vector)
{
	halted = 0;

	c.SP -= 2;
	mem_write_word(c.SP, c.PC);
	c.PC = vector;
	interrupt_disable();
}

unsigned int cpu_get_cycles(void)
{
	return c.cycles;
}

void cpu_print_debug(void)
{
	printf("%04X: %02X\n", c.PC, mem_get_byte(c.PC));
	printf("\tAF: %02X%02X, BC: %02X%02X, DE: %02X%02X, HL: %02X%02X SP: %04X, cycles %d\n",
		c.A, c.F, c.B, c.C, c.D, c.E, c.H, c.L, c.SP, c.cycles);
}

unsigned int cpu_cycle(void)
{
	unsigned char b, t;
	unsigned short s;
	unsigned int i;
	
	if(halted)
	{
		c.cycles += 1;
		return c.cycles;
	}

	if(interrupt_flush())
	{
		halted = 0;
	}


	b = mem_get_byte(c.PC);

#ifdef EBUG
//	if(c.PC == 0x2F38 && c.cycles > 10000000)
//	if(c.PC == 0xff87 && c.cycles > 14000000)
//		is_debugged = 0;
#endif
	if(is_debugged)
	{
		cpu_print_debug();
	}

	switch(b)
	{
		case 0x00:	/* NOP */
			c.PC++;
			c.cycles += 1;
		break;
		case 0x01:	/* LD BC, imm16 */
			s = mem_get_word(c.PC+1);
			set_BC(s);
			c.PC += 3;
			c.cycles += 3;
		break;
		case 0x02:	/* LD (BC), A */
			mem_write_byte(get_BC(), c.A);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x03:	/* INC BC */
			set_BC(get_BC()+1);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x04:	/* INC B */
			set_H((c.B&0xF) == 0xF);
			c.B++;
			set_Z(!c.B);
			set_N(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x05:	/* DEC B */
			c.B--;
			set_Z(!c.B);
			set_N(1);
			set_H((c.B & 0xF) == 0xF);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x06:	/* LD B, imm8 */
			c.B = mem_get_byte(c.PC+1);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0x07:	/* RLCA */
			RLC(7);
			set_Z(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x08:	/* LD (imm16), SP */
			mem_write_word(mem_get_word(c.PC+1), c.SP);
			c.PC += 3;
			c.cycles += 5;
		break;
		case 0x09:	/* ADD HL, BC */
			i = get_HL() + get_BC();
			set_N(0);
			set_C(i >= 0x10000);
			set_H((i&0xFFF) < (get_HL()&0xFFF));
			set_HL(i&0xFFFF);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x0A:	/* LD A, (BC) */
			c.A = mem_get_byte(get_BC());
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x0B:	/* DEC BC */
			s = get_BC();
			s--;
			set_BC(s);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x0C:	/* INC C */
			set_H((c.C&0xF) == 0xF);
			c.C++;
			set_Z(!c.C);
			set_N(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x0D:	/* DEC C */
			set_H((c.C&0xF) == 0);
			c.C--;
			set_Z(!c.C);
			set_N(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x0E:	/* LD C, imm8 */
			c.C = mem_get_byte(c.PC+1);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0x0F:	/* RRCA */
			RRC(7);
			set_Z(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x11:	/* LD DE, imm16 */
			s = mem_get_word(c.PC+1);
			set_DE(s);
			c.PC += 3;
			c.cycles += 3;
		break;
		case 0x12:	/* LD (DE), A */
			mem_write_byte(get_DE(), c.A);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x13:	/* INC DE */
			s = get_DE();
			s++;
			set_DE(s);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x14:	/* INC D */
			set_H((c.D&0xF) == 0xF);
			c.D++;
			set_Z(!c.D);
			set_N(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x15:	/* DEC D */
			c.D--;
			set_Z(!c.D);
			set_N(1);
			set_H((c.D & 0xF) == 0xF);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x16:	/* LD D, imm8 */
			c.D = mem_get_byte(c.PC+1);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0x17:	/* RLA */
			RL(7);
			set_Z(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x18:	/* JR rel8 */
			c.PC += (signed char)mem_get_byte(c.PC+1) + 2;
			c.cycles += 3;
		break;
		case 0x19:	/* ADD HL, DE */
			i = get_HL() + get_DE();
			set_H((i&0xFFF) < (get_HL()&0xFFF));
			set_HL(i);
			set_N(0);
			set_C(i > 0xFFFF);
			c.PC += 1;
			c.cycles += 3;
		break;
		case 0x1A:	/* LD A, (DE) */
			c.A = mem_get_byte(get_DE());
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x1B:	/* DEC DE */
			s = get_DE();
			s--;
			set_DE(s);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x1C:	/* INC E */
			set_H((c.E&0xF) == 0xF);
			c.E++;
			set_Z(!c.E);
			set_N(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x1D:	/* DEC E */
			c.E--;
			set_Z(!c.E);
			set_N(1);
			set_H((c.E & 0xF) == 0xF);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x1E:	/* LD E, imm8 */
			c.E = mem_get_byte(c.PC+1);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0x1F:	/* RR A */
			RR(7);
			set_Z(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x20:	/* JR NZ, rel8 */
			if(flag_Z == 0)
			{
				c.PC += (signed char)mem_get_byte(c.PC+1) + 2;
				c.cycles += 3;
			} else {
				c.PC += 2;
				c.cycles += 2;
			}
		break;
		case 0x21:	/* LD HL, imm16 */
			s = mem_get_word(c.PC+1);
			set_HL(s);
			c.PC += 3;
			c.cycles += 3;
		break;
		case 0x22:	/* LDI (HL), A */
			i = get_HL();
			mem_write_byte(i, c.A);
			i++;
			set_HL(i);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x23:	/* INC HL */
			s = get_HL();
			s++;
			set_HL(s);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x24:	/* INC H */
			c.H++;
			set_Z(!c.H);
			set_H((c.H&0xF) == 0);
			set_N(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x25:	/* DEC H */
			c.H--;
			set_Z(!c.H);
			set_N(1);
			set_H((c.H & 0xF) == 0xF);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x26:	/* LD H, imm8 */
			c.H = mem_get_byte(c.PC+1);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0x27:	/* DAA */
			s = c.A;

			if(flag_N)
			{
				if(flag_H)
					s = (s - 0x06)&0xFF;
				if(flag_C)
					s -= 0x60;
			}
			else
			{
				if(flag_H || (s & 0xF) > 9)
					s += 0x06;
				if(flag_C || s > 0x9F)
					s += 0x60;
			}

			c.A = s;
			set_H(0);
			set_Z(!c.A);
			if(s >= 0x100)
				set_C(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x28:	/* JR Z, rel8 */
			if(flag_Z == 1)
			{
				c.PC += (signed char)mem_get_byte(c.PC+1) + 2;
				c.cycles += 3;
			} else {
				c.PC += 2;
				c.cycles += 2;
			}
		break;
		case 0x29:	/* ADD HL, HL */
			i = get_HL()*2;
			set_H((i&0x7FF) < (get_HL()&0x7FF));
			set_C(i > 0xFFFF);
			set_HL(i);
			set_N(0);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x2A:	/* LDI A, (HL) */
			s = get_HL();
			c.A = mem_get_byte(s);
			set_HL(s+1);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x2B: 	/* DEC HL */
			set_HL(get_HL()-1);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x2C:	/* INC L */
			c.L++;
			set_Z(!c.L);
			set_N(0);
			set_H((c.L & 0xF) == 0x00);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x2D:	/* DEC L */
			c.L--;
			set_Z(!c.L);
			set_N(1);
			set_H((c.L & 0xF) == 0xF);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x2E:	/* LD L, imm8 */
			c.L = mem_get_byte(c.PC+1);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0x2F:	/* CPL */
			c.A = ~c.A;
			set_N(1);
			set_H(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x30:	/* JR NC, rel8 */
			if(flag_C == 0)
			{
				c.PC += (signed char)mem_get_byte(c.PC+1) + 2;
				c.cycles += 3;
			} else {
				c.PC += 2;
				c.cycles += 2;
			}
		break;
		case 0x31:	/* LD SP, imm16 */
			c.SP = mem_get_word(c.PC+1);
			c.PC += 3;
			c.cycles += 3;
		break;
		case 0x32:	/* LDD (HL), A */
			i = get_HL();
			mem_write_byte(i, c.A);
			set_HL(i-1);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x33:	/* INC SP */
			c.SP++;
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x34:	/* INC (HL) */
			t = mem_get_byte(get_HL());
			t++;
			mem_write_byte(get_HL(), t);
			set_Z(!t);
			set_N(0);
			set_H((t & 0xF) == 0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x35:	/* DEC (HL) */
			t = mem_get_byte(get_HL());
			t--;
			mem_write_byte(get_HL(), t);
			set_Z(!t);
			set_N(1);
			set_H((t & 0xF) == 0xF);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x36:	/* LD (HL), imm8 */
			t = mem_get_byte(c.PC+1);
			mem_write_byte(get_HL(), t);
			c.PC += 2;
			c.cycles += 3;
		break;
		case 0x37:	/* SCF */
			set_N(0);
			set_H(0);
			set_C(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x38:  /* JR C, rel8 */
			if(flag_C == 1)
			{
				c.PC += (signed char)mem_get_byte(c.PC+1) + 2;
				c.cycles += 3;
			} else {
				c.PC += 2;
				c.cycles += 2;
			}
		break;
		case 0x39:	/* ADD HL, SP */
			i = get_HL() + c.SP;
			set_H((i&0x7FF) < (get_HL()&0x7FF));
			set_C(i > 0xFFFF);
			set_N(0);
			set_HL(i);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x3A:	/* LDD A, (HL) */
			c.A = mem_get_byte(get_HL());
			set_HL(get_HL()-1);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x3B:	/* DEC SP */
			c.SP--;
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x3C:	/* INC A */
			c.A++;
			set_Z(!c.A);
			set_H((c.A&0xF) == 0);
			set_N(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x3D:	/* DEC A */
			c.A--;
			set_Z(!c.A);
			set_N(1);
			set_H((c.A & 0xF) == 0xF);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x3E:	/* LD A, imm8 */
			c.A = mem_get_byte(c.PC+1);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0x3F:	/* CCF */
			set_N(0);
			set_H(0);
			set_C(!flag_C);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x40:	/* LD B, B */
			c.B = c.B;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x41:	/* LD B, C */
			c.B = c.C;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x42:	/* LD B, D */
			c.B = c.D;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x43:	/* LD B, E */
			c.B = c.E;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x44:	/* LD B, H */
			c.B = c.H;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x45:	/* LD B, L */
			c.B = c.L;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x46:	/* LD B, (HL) */
			c.B = mem_get_byte(get_HL());
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x47:	/* LD B, A */
			c.B = c.A;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x48:	/* LD C, B */
			c.C = c.B;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x49:	/* LD C, C */
			c.C = c.C;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x4A:	/* LD C, D */
			c.C = c.D;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x4B:	/* LD C, E */
			c.C = c.E;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x4C:	/* LD C, H */
			c.C = c.H;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x4D:	/* LD C, L */
			c.C = c.L;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x4E:	/* LD C, (HL) */
			c.C = mem_get_byte(get_HL());
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x4F:	/* LD C, A */
			c.C = c.A;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x50:	/* LD D, B */
			c.D = c.B;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x51:	/* LD D, C */
			c.D = c.C;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x52:	/* LD D, D */
			c.D = c.D;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x53:	/* LD D, E */
			c.D = c.E;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x54:	/* LD D, H */
			c.D = c.H;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x55:	/* LD D, L */
			c.D = c.L;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x56:	/* LD D, (HL) */
			c.D = mem_get_byte(get_HL());
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x57:	/* LD D, A */
			c.D = c.A;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x58:	/* LD E, B */
			c.E = c.B;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x59:	/* LD E, C */
			c.E = c.C;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x5A:	/* LD E, D */
			c.E = c.D;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x5B:	/* LD E, E */
			c.E = c.E;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x5C:	/* LD E, H */
			c.E = c.H;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x5D:	/* LD E, L */
			c.E = c.L;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x5E:	/* LD E, (HL) */
			c.E = mem_get_byte(get_HL());
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x5F:	/* LD E, A */
			c.E = c.A;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x60:	/* LD H, B */
			c.H = c.B;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x61:	/* LD H, C */
			c.H = c.C;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x62:	/* LD H, D */
			c.H = c.D;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x63:	/* LD H, E */
			c.H = c.E;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x64:	/* LD H, H */
			c.H = c.H;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x65:	/* LD H, L */
			c.H = c.L;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x66:	/* LD H, (HL) */
			c.H = mem_get_byte(get_HL());
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x67:	/* LD H, A */
			c.H = c.A;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x68:	/* LD L, B */
			c.L = c.B;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x69:	/* LD L, C */
			c.L = c.C;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x6A:	/* LD L, D */
			c.L = c.D;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x6B:	/* LD L, E */
			c.L = c.E;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x6C:	/* LD L, H */
			c.L = c.H;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x6D:	/* LD L, L */
			c.L = c.L;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x6E:	/* LD L, (HL) */
			c.L = mem_get_byte(get_HL());
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x6F:	/* LD L, A */
			c.L = c.A;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x70:	/* LD (HL), B */
			mem_write_byte(get_HL(), c.B);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x71:	/* LD (HL), C */
			mem_write_byte(get_HL(), c.C);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x72:	/* LD (HL), D */
			mem_write_byte(get_HL(), c.D);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x73:	/* LD (HL), E */
			mem_write_byte(get_HL(), c.E);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x74:	/* LD (HL), H */
			mem_write_byte(get_HL(), c.H);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x75:	/* LD (HL), L */
			mem_write_byte(get_HL(), c.L);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x76:	/* HALT */
			halted = 1;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x77:	/* LD (HL), A */
			mem_write_byte(get_HL(), c.A);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x78:	/* LD A, B */
			c.A = c.B;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x79:	/* LD A, C */
			c.A = c.C;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x7A:	/* LD A, D */
			c.A = c.D;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x7B:	/* LD A, E */
			c.A = c.E;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x7C:	/* LD A, H */
			c.A = c.H;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x7D:	/* LD A, L */
			c.A = c.L;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x7E:	/* LD A, (HL) */
			c.A = mem_get_byte(get_HL());
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x7F:	/* LD A, A */
			c.A = c.A;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x80:	/* ADD B */
			i = c.A + c.B;
			set_H((c.A&0xF)+(c.B&0xF) > 0xF);
			set_C(i > 0xFF);
			set_N(0);
			c.A = i;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x81:	/* ADD C */
			i = c.A + c.C;
			set_H((c.A&0xF)+(c.C&0xF) > 0xF);
			set_C(i > 0xFF);
			set_N(0);
			c.A = i;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x82:	/* ADD D */
			i = c.A + c.D;
			set_H((c.A&0xF)+(c.D&0xF) > 0xF);
			set_C(i > 0xFF);
			set_N(0);
			c.A = i;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x83:	/* ADD E */
			i = c.A + c.E;
			set_H((c.A&0xF)+(c.E&0xF) > 0xF);
			set_C(i > 0xFF);
			set_N(0);
			c.A = i;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x84:	/* ADD H */
			i = c.A + c.H;
			set_H((c.A&0xF)+(c.H&0xF) > 0xF);
			set_C(i > 0xFF);
			set_N(0);
			c.A = i;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x85:	/* ADD L */
			i = c.A + c.L;
			set_H((c.A&0xF)+(c.L&0xF) > 0xF);
			set_C(i > 0xFF);
			set_N(0);
			c.A = i;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x86:	/* ADD (HL) */
			i = c.A + mem_get_byte(get_HL());
			set_H((i&0xF) < (c.A&0xF));
			set_C(i > 0xFF);
			set_N(0);
			c.A = i;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x87:	/* ADD A */
			i = c.A + c.A;
			set_H((c.A&0xF)+(c.A&0xF) > 0xF);
			set_C(i > 0xFF);
			set_N(0);
			c.A = i;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x88:	/* ADC B */
			i = c.A + c.B + flag_C >= 0x100;
			set_N(0);
			set_H(((c.A&0xF) + (c.B&0xF) + flag_C) >= 0x10);
			c.A = c.A + c.B + flag_C;
			set_C(i);
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x89:	/* ADC C */
			i = c.A + c.C + flag_C >= 0x100;
			set_N(0);
			set_H(((c.A&0xF) + (c.C&0xF) + flag_C) >= 0x10);
			c.A = c.A + c.C + flag_C;
			set_C(i);
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x8A:	/* ADC D */
			i = c.A + c.D + flag_C >= 0x100;
			set_N(0);
			set_H(((c.A&0xF) + (c.D&0xF) + flag_C) >= 0x10);
			c.A = c.A + c.D + flag_C;
			set_C(i);
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x8B:	/* ADC E */
			i = c.A + c.E + flag_C >= 0x100;
			set_N(0);
			set_H(((c.A&0xF) + (c.E&0xF) + flag_C) >= 0x10);
			c.A = c.A + c.E + flag_C;
			set_C(i);
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x8C:	/* ADC H */
			i = c.A + c.H + flag_C >= 0x100;
			set_N(0);
			set_H(((c.A&0xF) + (c.H&0xF) + flag_C) >= 0x10);
			c.A = c.A + c.H + flag_C;
			set_C(i);
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x8D:	/* ADC L */
			i = c.A + c.L + flag_C >= 0x100;
			set_N(0);
			set_H(((c.A&0xF) + (c.L&0xF) + flag_C) >= 0x10);
			c.A = c.A + c.L + flag_C;
			set_C(i);
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x8E:	/* ADC (HL) */
			t = mem_get_byte(get_HL());
			i = c.A + t + flag_C >= 0x100;
			set_N(0);
			set_H(((c.A&0xF) + (t&0xF) + flag_C) >= 0x10);
			c.A = c.A + t + flag_C;
			set_C(i);
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x8F:	/* ADC A */
			i = c.A + c.A + flag_C >= 0x100;
			set_N(0);
			set_H(((c.A&0xF) + (c.A&0xF) + flag_C) >= 0x10);
			c.A = c.A + c.A + flag_C;
			set_C(i);
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x90:	/* SUB B */
			set_C((c.A - c.B) < 0);
			set_H(((c.A - c.B)&0xF) > (c.A&0xF));
			c.A -= c.B;
			set_Z(!c.A);
			set_N(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x91:	/* SUB C */
			set_C((c.A - c.C) < 0);
			set_H(((c.A - c.C)&0xF) > (c.A&0xF));
			c.A -= c.C;
			set_Z(!c.A);
			set_N(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x92:	/* SUB D */
			set_C((c.A - c.D) < 0);
			set_H(((c.A - c.D)&0xF) > (c.A&0xF));
			c.A -= c.D;
			set_Z(!c.A);
			set_N(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x93:	/* SUB E */
			set_C((c.A - c.E) < 0);
			set_H(((c.A - c.E)&0xF) > (c.A&0xF));
			c.A -= c.E;
			set_Z(!c.A);
			set_N(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x94:	/* SUB H */
			set_C((c.A - c.H) < 0);
			set_H(((c.A - c.H)&0xF) > (c.A&0xF));
			c.A -= c.H;
			set_Z(!c.A);
			set_N(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x95:	/* SUB L */
			set_C((c.A - c.L) < 0);
			set_H(((c.A - c.L)&0xF) > (c.A&0xF));
			c.A -= c.L;
			set_Z(!c.A);
			set_N(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x96:	/* SUB (HL) */
			t = mem_get_byte(get_HL());
			set_C((c.A - t) < 0);
			set_H(((c.A - t)&0xF) > (c.A&0xF));
			c.A -= t;
			set_Z(!c.A);
			set_N(1);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x97:	/* SUB A */
			set_C(0);
			set_H(0);
			c.A = 0;
			set_Z(1);
			set_N(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x98:	/* SBC B */
			t = flag_C + c.B;
			set_H(((c.A&0xF) - (c.B&0xF) - flag_C) < 0);
			set_C((c.A - c.B - flag_C) < 0);
			set_N(1);
			c.A -= t;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x99:	/* SBC C */
			t = flag_C + c.C;
			set_H(((c.A&0xF) - (c.C&0xF) - flag_C) < 0);
			set_C((c.A - c.C - flag_C) < 0);
			set_N(1);
			c.A -= t;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x9A:	/* SBC D */
			t = flag_C + c.D;
			set_H(((c.A&0xF) - (c.D&0xF) - flag_C) < 0);
			set_C((c.A - c.D - flag_C) < 0);
			set_N(1);
			c.A -= t;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x9B:	/* SBC E */
			t = flag_C + c.E;
			set_H(((c.A&0xF) - (c.E&0xF) - flag_C) < 0);
			set_C((c.A - c.E - flag_C) < 0);
			set_N(1);
			c.A -= t;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x9C:	/* SBC H */
			t = flag_C + c.H;
			set_H(((c.A&0xF) - (c.H&0xF) - flag_C) < 0);
			set_C((c.A - c.H - flag_C) < 0);
			set_N(1);
			c.A -= t;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x9D:	/* SBC L */
			t = flag_C + c.L;
			set_H(((c.A&0xF) - (c.L&0xF) - flag_C) < 0);
			set_C((c.A - c.L - flag_C) < 0);
			set_N(1);
			c.A -= t;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0x9E:	/* SBC (HL) */
			t = mem_get_byte(get_HL());
			b = flag_C + t;
			set_H(((c.A&0xF) - (t&0xF) - flag_C) < 0);
			set_C((c.A - t - flag_C) < 0);
			set_N(1);
			c.A -= b;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0x9F:	/* SBC A */
			t = flag_C + c.A;
			set_H(((c.A&0xF) - (c.A&0xF) - flag_C) < 0);
			set_C((c.A - c.A - flag_C) < 0);
			set_N(1);
			c.A -= t;
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xA0:	/* AND B */
			c.A &= c.B;
			set_Z(!c.A);
			set_H(1);
			set_N(0);
			set_C(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xA1:	/* AND C */
			c.A &= c.C;
			set_Z(!c.A);
			set_H(1);
			set_N(0);
			set_C(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xA2:	/* AND D */
			c.A &= c.D;
			set_Z(!c.A);
			set_H(1);
			set_N(0);
			set_C(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xA3:	/* AND E */
			c.A &= c.E;
			set_Z(!c.A);
			set_H(1);
			set_N(0);
			set_C(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xA4:	/* AND H */
			c.A &= c.H;
			set_Z(!c.A);
			set_H(1);
			set_N(0);
			set_C(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xA5:	/* AND L */
			c.A &= c.L;
			set_Z(!c.A);
			set_H(1);
			set_N(0);
			set_C(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xA6:	/* AND (HL) */
			c.A &= mem_get_byte(get_HL());
			set_Z(!c.A);
			set_H(1);
			set_N(0);
			set_C(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xA7:	/* AND A */
			set_H(1);
			set_N(0);
			set_C(0);
			set_Z(!c.A);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xA8:	/* XOR B */
			c.A ^= c.B;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xA9:	/* XOR C */
			c.A ^= c.C;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xAA:	/* XOR D */
			c.A ^= c.D;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xAB:	/* XOR E */
			c.A ^= c.E;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xAC:	/* XOR H */
			c.A ^= c.H;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xAD:	/* XOR L */
			c.A ^= c.L;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xAE:	/* XOR (HL) */
			c.A ^= mem_get_byte(get_HL());
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xAF:	/* XOR A */
			c.A = 0;
			c.F = 0x80;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xB0:	/* OR B */
			c.A |= c.B;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xB1:	/* OR C */
			c.A |= c.C;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xB2:	/* OR D */
			c.A |= c.D;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xB3:	/* OR E */
			c.A |= c.E;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xB4:	/* OR H */
			c.A |= c.H;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xB5:	/* OR L */
			c.A |= c.L;
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xB6:	/* OR (HL) */
			c.A |= mem_get_byte(get_HL());
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0xB7:	/* OR A */
			c.F = (!c.A)<<7;
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xB8:	/* CP B */
			set_C((c.A - c.B) < 0);
			set_H(((c.A - c.B)&0xF) > (c.A&0xF));
			set_Z(c.A == c.B);
			set_N(1);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xB9:	/* CP C */
			set_Z(c.A == c.C);
			set_H(((c.A - c.C)&0xF) > (c.A&0xF));
			set_N(1);
			set_C((c.A - c.C) < 0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xBA:	/* CP D */
			set_Z(c.A == c.D);
			set_H(((c.A - c.D)&0xF) > (c.A&0xF));
			set_N(1);
			set_C((c.A - c.D) < 0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xBB:	/* CP E */
			set_Z(c.A == c.E);
			set_H(((c.A - c.E)&0xF) > (c.A&0xF));
			set_N(1);
			set_C((c.A - c.E) < 0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xBC:	/* CP H */
			set_Z(c.A == c.H);
			set_H(((c.A - c.H)&0xF) > (c.A&0xF));
			set_N(1);
			set_C((c.A - c.H) < 0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xBD:	/* CP L */
			set_Z(c.A == c.L);
			set_H(((c.A - c.L)&0xF) > (c.A&0xF));
			set_N(1);
			set_C((c.A - c.L) < 0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xBE:	/* CP (HL) */
			t = mem_get_byte(get_HL());
			set_Z(c.A == t);
			set_H(((c.A - t)&0xF) > (c.A&0xF));
			set_N(1);
			set_C((c.A - t) < 0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xBF:	/* CP A */
			set_Z(1);
			set_H(0);
			set_N(1);
			set_C(0);
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xC0:	/* RET NZ */
			if(!flag_Z)
			{
				c.PC = mem_get_word(c.SP);
				c.SP += 2;
				c.cycles += 3;
			} else {
				c.PC += 1;
				c.cycles += 1;
			}
		break;
		case 0xC1:	/* POP BC */
			s = mem_get_word(c.SP);
			set_BC(s);
			c.SP += 2;
			c.PC += 1;
			c.cycles += 3;
		break;
		case 0xC2:	/* JP NZ, mem16 */
			if(flag_Z == 0)
			{
				c.PC = mem_get_word(c.PC+1);
			} else {
				c.PC += 3;
			}
			c.cycles += 3;
		break;
		case 0xC3:	/* JP imm16 */
			c.PC = mem_get_word(c.PC+1);
			c.cycles += 4;
		break;
		case 0xC4:	/* CALL NZ, imm16 */
			if(flag_Z == 0)
			{
				c.SP -= 2;
				mem_write_word(c.SP, c.PC+3);
				c.PC = mem_get_word(c.PC+1);
				c.cycles += 6;
			} else {
				c.PC += 3;
				c.cycles += 3;
			}
		break;
		case 0xC5:	/* PUSH BC */
			c.SP -= 2;
			mem_write_word(c.SP, get_BC());
			c.PC += 1;
			c.cycles += 3;
		break;
		case 0xC6:	/* ADD A, imm8 */
			t = mem_get_byte(c.PC+1);
			set_C((c.A + t) >= 0x100);
			set_H(((c.A + t)&0xF) < (c.A&0xF));
			c.A += t;
			set_N(0);
			set_Z(!c.A);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0xC7:	/* RST 00 */
			c.SP -= 2;
			mem_write_word(c.SP, c.PC+1);
			c.PC = 0;
			c.cycles += 3;
		break;
		case 0xC8:	/* RET Z */
			if(flag_Z == 1)
			{
				c.PC = mem_get_word(c.SP);
				c.SP += 2;
				c.cycles += 3;
			} else {
				c.PC += 1;
				c.cycles += 1;
			}
		break;
		case 0xC9:	/* RET */
			c.PC = mem_get_word(c.SP);
			c.SP += 2;
			c.cycles += 3;
		break;
		case 0xCA:	/* JP z, mem16 */
			if(flag_Z == 1)
			{
				c.PC = mem_get_word(c.PC+1);
			} else {
				c.PC += 3;
			}
			c.cycles += 3;
		break;
		case 0xCB:	/* RLC/RRC/RL/RR/SLA/SRA/SWAP/SRL/BIT/RES/SET */
			decode_CB(mem_get_byte(c.PC+1));
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0xCC:	/* CALL Z, imm16 */
			if(flag_Z == 1)
			{
				c.SP -= 2;
				mem_write_word(c.SP, c.PC+3);
				c.PC = mem_get_word(c.PC+1);
				c.cycles += 6;
			} else {
				c.PC += 3;
				c.cycles += 3;
			}
		break;
		case 0xCD:	/* call imm16 */
			c.SP -= 2;
			mem_write_word(c.SP, c.PC+3);
			c.PC = mem_get_word(c.PC+1);
			c.cycles += 6;
		break;
		case 0xCE:	/* ADC a, imm8 */
			t = mem_get_byte(c.PC+1);
			i = c.A + t + flag_C >= 0x100;
			set_N(0);
			set_H(((c.A&0xF) + (t&0xF) + flag_C) >= 0x10);
			c.A = c.A + t + flag_C;
			set_C(i);
			set_Z(!c.A);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0xCF:	/* RST 08 */
			c.SP -= 2;
			mem_write_word(c.SP, c.PC+1);
			c.PC = 0x0008;
			c.cycles += 4;
		break;
		case 0xD0:	/* RET NC */
			if(flag_C == 0)
			{
				c.PC = mem_get_word(c.SP);
				c.SP += 2;
				c.cycles += 3;
			} else {
				c.PC += 1;
				c.cycles += 1;
			}
		break;
		case 0xD1:	/* POP DE */
			s = mem_get_word(c.SP);
			set_DE(s);
			c.SP += 2;
			c.PC += 1;
			c.cycles += 3;
		break;
		case 0xD2:	/* JP NC, mem16 */
			if(flag_C == 0)
			{
				c.PC = mem_get_word(c.PC+1);
			} else {
				c.PC += 3;
			}
			c.cycles += 3;
		break;
		case 0xD4:	/* CALL NC, mem16 */
			if(flag_C == 0)
			{
				c.SP -= 2;
				mem_write_word(c.SP, c.PC+3);
				c.PC = mem_get_word(c.PC+1);
				c.cycles += 6;
			} else {
				c.PC += 3;
				c.cycles += 3;
			}
		break;
		case 0xD5:	/* PUSH DE */
			c.SP -= 2;
			mem_write_word(c.SP, get_DE());
			c.PC += 1;
			c.cycles += 3;
		break;
		case 0xD6:	/* SUB A, imm8 */
			t = mem_get_byte(c.PC+1);
			set_C((c.A - t) < 0);
			set_H(((c.A - t)&0xF) > (c.A&0xF));
			c.A -= t;
			set_N(1);
			set_Z(!c.A);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0xD7:	/* RST 10 */
			c.SP -= 2;
			mem_write_word(c.SP, c.PC+1);
			c.PC = 0x0010;
			c.cycles += 4;
		break;
		case 0xD8:	/* RET C */
			if(flag_C == 1)
			{
				c.PC = mem_get_word(c.SP);
				c.SP += 2;
				c.cycles += 3;
			} else {
				c.PC += 1;
				c.cycles += 1;
			}
		break;
		case 0xDA:	/* JP C, mem16 */
			if(flag_C)
			{
				c.PC = mem_get_word(c.PC+1);
			} else {
				c.PC += 3;
			}
			c.cycles += 3;
		break;
		case 0xDC:	/* CALL C, mem16 */
			if(flag_C == 1)
			{
				c.SP -= 2;
				mem_write_word(c.SP, c.PC+3);
				c.PC = mem_get_word(c.PC+1);
				c.cycles += 6;
			} else {
				c.PC += 3;
				c.cycles += 3;
			}
		break;
		case 0xD9:	/* RETI */
			c.PC = mem_get_word(c.SP);
			c.SP += 2;
			c.cycles += 4;
			interrupt_enable();
		break;
		case 0xDE:	/* SBC A, imm8 */
			t = mem_get_byte(c.PC+1);
			b = flag_C;
			set_H(((t&0xF) + flag_C) > (c.A&0xF));
			set_C(t + flag_C > c.A);
			set_N(1);
			c.A -= (b + t);
			set_Z(!c.A);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0xDF:	/* RST 18 */
			c.SP -= 2;
			mem_write_word(c.SP, c.PC+1);
			c.PC = 0x0018;
			c.cycles += 3;
		break;
		case 0xE0:	/* LD (FF00 + imm8), A */
			t = mem_get_byte(c.PC+1);
			mem_write_byte(0xFF00 + t, c.A);
			c.PC += 2;
			c.cycles += 3;
		break;
		case 0xE1:	/* POP HL */
			i = mem_get_word(c.SP);
			set_HL(i);
			c.SP += 2;
			c.PC += 1;
			c.cycles += 3;
		break;
		case 0xE2:	/* LD (FF00 + C), A */
			s = 0xFF00 + c.C;
			mem_write_byte(s, c.A);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0xE5:	/* PUSH HL */
			c.SP -= 2;
			mem_write_word(c.SP, get_HL());
			c.PC += 1;
			c.cycles += 3;
		break;
		case 0xE6:	/* AND A, imm8 */
			t = mem_get_byte(c.PC+1);
			set_N(0);
			set_H(1);
			set_C(0);
			c.A = t & c.A;
			set_Z(!c.A);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0xE7:	/* RST 20 */
			c.SP -= 2;
			mem_write_word(c.SP, c.PC+1);
			c.PC = 0x20;
			c.cycles += 4;
		break;
		case 0xE8:	/* ADD SP, imm8 */
			i = mem_get_byte(c.PC+1);
			set_Z(0);
			set_N(0);
			set_C(((c.SP+i)&0xFF) < (c.SP&0xFF));
			set_H(((c.SP+i)&0xF) < (c.SP&0xF));
			c.SP = c.SP + (signed char)i;
			c.PC += 2;
			c.cycles += 4;
		break;
		case 0xE9:	/* JP HL */
			c.PC = get_HL();
			c.cycles += 1;
		break;
		case 0xEA:	/* LD (mem16), a */
			s = mem_get_word(c.PC+1);
			mem_write_byte(s, c.A);
			c.PC += 3;
			c.cycles += 4;
		break;
		case 0xEE:	/* XOR A, imm8 */
			c.A ^= mem_get_byte(c.PC+1);
			c.F = (!c.A)<<7;
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0xEF:	/* RST 28 */
			c.SP -= 2;
			mem_write_word(c.SP, c.PC+1);
			c.PC = 0x28;
			c.cycles += 4;
		break;
		case 0xF0:	/* LD A, (FF00 + imm8) */
			t = mem_get_byte(c.PC+1);
			c.A = mem_get_byte(0xFF00 + t);
			c.PC += 2;
			c.cycles += 3;
		break;
		case 0xF1:	/* POP AF */
			s = mem_get_word(c.SP);
			set_AF(s&0xFFF0);
			c.SP += 2;
			c.PC += 1;
			c.cycles += 3;
		break;
		case 0xF2:	/* LD A, (FF00 + c) */
			c.A = mem_get_byte(0xFF00 + c.C);
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0xF3:	/* DI */
			c.PC += 1;
			c.cycles += 1;
			interrupt_disable();
		break;
		case 0xF5:	/* PUSH AF */
			c.SP -= 2;
			mem_write_word(c.SP, get_AF());
			c.PC += 1;
			c.cycles += 3;
		break;
		case 0xF6:	/* OR A, imm8 */
			c.A |= mem_get_byte(c.PC+1);
			c.F = (!c.A)<<7;
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0xF7:	/* RST 30 */
			c.SP -= 2;
			mem_write_word(c.SP, c.PC+1);
			c.PC = 0x30;
			c.cycles += 4;
		break;
		case 0xF8:	/* LD HL, SP + imm8 */
			i = mem_get_byte(c.PC+1);
			set_N(0);
			set_Z(0);
			set_C(((c.SP+i)&0xFF) < (c.SP&0xFF));
			set_H(((c.SP+i)&0xF) < (c.SP&0xF));
			set_HL(c.SP + (signed char)i);
			c.PC += 2;
			c.cycles += 3;
		break;
		case 0xF9:	/* LD SP, HL */
			c.SP = get_HL();
			c.PC += 1;
			c.cycles += 2;
		break;
		case 0xFA:	/* LD A, (mem16) */
			s = mem_get_word(c.PC+1);
			c.A = mem_get_byte(s);
			c.PC += 3;
			c.cycles += 4;
		break;
		case 0xFB:	/* EI */
			interrupt_enable();
//			printf("Interrupts enabled, IE: %02x\n", interrupt_get_mask());
			c.PC += 1;
			c.cycles += 1;
		break;
		case 0xFE:	/* CP a, imm8 */
			t = mem_get_byte(c.PC+1);
			set_Z(c.A == t);
			set_N(1);
			set_H(((c.A - t)&0xF) > (c.A&0xF));
			set_C(c.A < t);
			c.PC += 2;
			c.cycles += 2;
		break;
		case 0xFF:	/* RST 38 */
			c.SP -= 2;
			mem_write_word(c.SP, c.PC+1);
			c.PC = 0x0038;
			c.cycles += 4;
		break;
		default:
			printf("Unhandled opcode %02X at %04X\n", b, c.PC);
			printf("cycles: %d\n", c.cycles);
			return 0;
		break;
	}

	return c.cycles;
}
