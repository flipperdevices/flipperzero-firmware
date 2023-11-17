#include "timer.h"
#include "interrupt.h"
#include "cpu.h"

static unsigned int prev_time;
static unsigned int elapsed;
static unsigned int ticks;

static unsigned char tac;
static unsigned int started;
static unsigned int speed;
static unsigned int counter;
static unsigned int divider;
static unsigned int modulo;

void timer_set_div(unsigned char v)
{
	(void) v;
	divider = 0;
}

unsigned char timer_get_div(void)
{
	return divider;
}

void timer_set_counter(unsigned char v)
{
	counter = v;
}

unsigned char timer_get_counter(void)
{
	return counter;
}

void timer_set_modulo(unsigned char v)
{
	modulo = v;
}

unsigned char timer_get_modulo(void)
{
	return modulo;
}

void timer_set_tac(unsigned char v)
{
	int speeds[] = {64, 1, 4, 16};
	tac = v;
	started = v&4;
	speed = speeds[v&3];
}

unsigned char timer_get_tac(void)
{
	return tac;
}

static void timer_tick(void)
{
	/* 1/262144Hz has elapsed */
	ticks++;

	/* Divider updates at 16384Hz */
	if(ticks == 16)
	{
		divider++;
		ticks = 0;
	}

	if(!started)
		return;

	if(ticks == speed)
	{
		counter++;
		ticks = 0;
	}

	if(counter == 0x100)
	{
		interrupt(INTR_TIMER);
		counter = modulo;
	}
}

void timer_cycle(unsigned int cycles)
{
	/* The amount of ticks since we last ran */ 
	unsigned int delta = cycles - prev_time;
	prev_time = cycles;

	elapsed += delta * 4; /* 4 cycles to a timer tick */
	if(elapsed >= 16)
	{
		timer_tick();
		elapsed -= 16;	/* keep track of the time overflow */
	}
}