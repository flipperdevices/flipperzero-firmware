#include "mbc.h"
#include "mem.h"
#include "rom.h"

enum {
	NO_FILTER_WRITE,
	FILTER_WRITE
};

static unsigned int bank_upper_bits;
static unsigned int ram_select;

/* Unfinished, no clock etc */
unsigned int MBC3_write_byte(unsigned short d, unsigned char i)
{
	int bank;

	if(d < 0x2000)
	{
		return FILTER_WRITE;
	}

	if(d < 0x4000)
	{
		bank = i & 0x7F;

		if(bank == 0)
			bank++;

		mem_bank_switch(bank);

		return FILTER_WRITE;
	}

	if(d < 0x8000)
		return FILTER_WRITE;

	return NO_FILTER_WRITE;
}
unsigned int MBC1_write_byte(unsigned short d, unsigned char i)
{
	int bank;

	if(d < 0x2000)
	{
		return FILTER_WRITE;
		/* TODO: Enable/disable SRAM */
	}

	/* Switch rom bank at 4000-7fff */
	if(d >= 0x2000 && d < 0x4000)
	{
		/* Bits 0-4 come from the value written to memory here,
		 * bits 5-6 come from a seperate write to 4000-5fff if
		 * RAM select is 1.
		 */
		bank = i & 0x1F;
		if(!ram_select)
			bank |= bank_upper_bits;

		/* "Writing to this address space selects the lower 5 bits of the
		 * ROM Bank Number (in range 01-1Fh). When 00h is written, the MBC
		 * translates that to bank 01h also."
		 * http://nocash.emubase.de/pandocs.htm#mbc1max2mbyteromandor32kbyteram
		 */

		if(bank == 0 || bank == 0x20 || bank == 0x40 || bank == 0x60)
			bank++;

		mem_bank_switch(bank);

		return FILTER_WRITE;
	}

	/* Bit 5 and 6 of the bank selection */
	if(d >= 0x4000 && d < 0x6000)
	{
		bank_upper_bits = (i & 0x3)<<5;
		return FILTER_WRITE;
	}

	if(d >= 0x6000 && d <= 0x7FFF)
	{
		ram_select = i&1;
		return FILTER_WRITE;
	}
	return NO_FILTER_WRITE;
}
