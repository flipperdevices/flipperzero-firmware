SECTIONS
{
	.text 0x00000000 :
	{
		*(.text)
		*(.text.*)
	}

	.rodata :
	{
		*(.rodata)
		*(.rodata1)
		*(.rodata.*)
	}

	.data :
	{
		*(.data)
		*(.data1)
		*(.data.*)
	}

	.bss :
	{
		*(.bss)
		*(.bss.*)
		*(.sbss)
		*(.sbss.*)
		*(COMMON)
	}

	.ARM.attributes : 
	{
		*(.ARM.attributes)
		*(.ARM.attributes.*)	
	}

	/DISCARD/ :
	{
		*(.comment)
		*(.comment.*)
	}
}
