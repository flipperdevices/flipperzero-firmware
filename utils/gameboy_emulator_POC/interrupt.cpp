#include "interrupt.h"
#include "cpu.h"

static int enabled;
static int pending;

/* Pending interrupt flags */
static unsigned int vblank;
static unsigned int lcdstat;
static unsigned int timer;
static unsigned int serial;
static unsigned int joypad;

/* Interrupt masks */
static unsigned int vblank_masked = 1;
static unsigned int lcdstat_masked = 1;
static unsigned int timer_masked = 1;
static unsigned int serial_masked = 1;
static unsigned int joypad_masked = 1;

/* Returns true if the cpu should be unhalted */
int interrupt_flush(void)
{
	/* Flush the highest priority interrupt and/or resume the cpu */
	if(pending == 2)
	{
		pending--;
		return 0;
	}

	pending = 0;

	/* There's a pending interrupt but interrupts are disabled, just resume the cpu */
	if(!enabled && ((vblank && !vblank_masked) 
		|| (lcdstat && !lcdstat_masked)
		|| (timer && !timer_masked)
		|| (serial && !serial_masked) 
		|| (joypad && !joypad_masked))
	)
		return 1;

	/* Interrupts are enabled - Check if any need to fire */
	if(vblank && !vblank_masked)
	{
		vblank = 0;
		cpu_interrupt(0x40);
	}
	else if(lcdstat && !lcdstat_masked)
	{
		lcdstat = 0;
		cpu_interrupt(0x48);
	}
	else if(timer && !timer_masked)
	{
		timer = 0;
		cpu_interrupt(0x50);
	}
	else if(serial && !serial_masked)
	{
		serial = 0;
		cpu_interrupt(0x58);
	}
	else if(joypad && !joypad_masked)
	{
		joypad = 0;
		cpu_interrupt(0x60);
	}

	return 0;
}

void interrupt_enable(void)
{
	enabled = 1;
	pending = 2;
}

void interrupt_disable(void)
{
	enabled = 0;
}

void interrupt(unsigned int n)
{
	/* Add this interrupt to pending queue */
	switch(n)
	{
		case INTR_VBLANK:
			vblank = 1;
		break;
		case INTR_LCDSTAT:
			lcdstat = 1;
		break;
		case INTR_TIMER:
			timer = 1;
		break;
		case INTR_SERIAL:
			serial = 1;
		break;
		case INTR_JOYPAD:
			joypad = 1;
		break;
	}

	/* If interrupts are already enabled, flush one now, otherwise wait for
	 * interrupts to be re-enabled.
	 */
	if(enabled)
		interrupt_flush();
}

unsigned char interrupt_get_IF(void)
{
	unsigned char mask = 0xE0;

	mask |= (vblank  << 0);
	mask |= (lcdstat << 1);
	mask |= (timer   << 2);
	mask |= (serial  << 3);
	mask |= (joypad  << 4);

	return mask;
}

void interrupt_set_IF(unsigned char mask)
{
//	printf("IF set to %02x\n", mask);
	vblank  = !!(mask & 0x01);
	lcdstat = !!(mask & 0x02);
	timer   = !!(mask & 0x04);
	serial  = !!(mask & 0x08);
	joypad  = !!(mask & 0x10);

	if(enabled && mask)
		pending = 1;
}

unsigned char interrupt_get_mask(void)
{
	unsigned char mask = 0;

	mask |= (!vblank_masked  << 0);
	mask |= (!lcdstat_masked << 1);
	mask |= (!timer_masked   << 2);
	mask |= (!serial_masked  << 3);
	mask |= (!joypad_masked  << 4);

	return mask;
}

void interrupt_set_mask(unsigned char mask)
{
	vblank_masked  = !(mask & 0x01);
	lcdstat_masked = !(mask & 0x02);
	timer_masked   = !(mask & 0x04);
	serial_masked  = !(mask & 0x08);
	joypad_masked  = !(mask & 0x10);
}
