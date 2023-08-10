
/* Linker Script for Zynq MP */

/* Stack and Heap increased to 64KB  */
_STACK_SIZE = DEFINED(_STACK_SIZE) ? _STACK_SIZE : 0x10000;
_HEAP_SIZE = DEFINED(_HEAP_SIZE) ? _HEAP_SIZE : 0x10000;

_EL0_STACK_SIZE = DEFINED(_EL0_STACK_SIZE) ? _EL0_STACK_SIZE : 1024;
_EL1_STACK_SIZE = DEFINED(_EL1_STACK_SIZE) ? _EL1_STACK_SIZE : 2048;
_EL2_STACK_SIZE = DEFINED(_EL2_STACK_SIZE) ? _EL2_STACK_SIZE : 1024;

/* Define Memories in the system */
MEMORY
{
   ddr4_ctrl_C0_DDR4_ADDRESS_BLOCK : ORIGIN = 0x500000000, LENGTH = 0x20000000
   psu_ddr_0_MEM_0 : ORIGIN = 0x0, LENGTH = 0x7FF00000
   psu_ddr_1_MEM_0 : ORIGIN = 0x800000000, LENGTH = 0x80000000
   psu_ocm_ram_0_MEM_0 : ORIGIN = 0xFFFC0000, LENGTH = 0x40000
   psu_qspi_linear_0_MEM_0 : ORIGIN = 0xC0000000, LENGTH = 0x20000000
}

/* Specify the default entry point to the program */
ENTRY(_vector_table)

/* Define the sections, and where they are mapped in memory */
SECTIONS
{
.text : {
   KEEP (*(.vectors))
   *(.boot)
   *(.text)
   *(.text.*)
   *(.gnu.linkonce.t.*)
   *(.plt)
   *(.gnu_warning)
   *(.gcc_execpt_table)
   *(.glue_7)
   *(.glue_7t)
   *(.ARM.extab)
   *(.gnu.linkonce.armextab.*)
} > psu_ddr_0_MEM_0

.init (ALIGN(64)) : {
   KEEP (*(.init))
} > psu_ddr_0_MEM_0

.fini (ALIGN(64)) : {
   KEEP (*(.fini))
} > psu_ddr_0_MEM_0

.interp : {
   KEEP (*(.interp))
} > psu_ddr_0_MEM_0

.note-ABI-tag : {
   KEEP (*(.note-ABI-tag))
} > psu_ddr_0_MEM_0

.rodata : {
   . = ALIGN(64);
   __rodata_start = .;
   *(.rodata)
   *(.rodata.*)
   *(.gnu.linkonce.r.*)
   __rodata_end = .;
} > psu_ddr_0_MEM_0

.rodata1 : {
   . = ALIGN(64);
   __rodata1_start = .;
   *(.rodata1)
   *(.rodata1.*)
   __rodata1_end = .;
} > psu_ddr_0_MEM_0

.sdata2 : {
   . = ALIGN(64);
   __sdata2_start = .;
   *(.sdata2)
   *(.sdata2.*)
   *(.gnu.linkonce.s2.*)
   __sdata2_end = .;
} > psu_ddr_0_MEM_0

.sbss2 : {
   . = ALIGN(64);
   __sbss2_start = .;
   *(.sbss2)
   *(.sbss2.*)
   *(.gnu.linkonce.sb2.*)
   __sbss2_end = .;
} > psu_ddr_0_MEM_0

.data : {
   . = ALIGN(64);
   __data_start = .;
   *(.data)
   *(.data.*)
   *(.gnu.linkonce.d.*)
   *(.jcr)
   *(.got)
   *(.got.plt)
   __data_end = .;
} > psu_ddr_0_MEM_0

.data1 : {
   . = ALIGN(64);
   __data1_start = .;
   *(.data1)
   *(.data1.*)
   __data1_end = .;
} > psu_ddr_0_MEM_0

.got : {
   *(.got)
} > psu_ddr_0_MEM_0

.got1 : {
   *(.got1)
} > psu_ddr_0_MEM_0

.got2 : {
   *(.got2)
} > psu_ddr_0_MEM_0

.ctors : {
   . = ALIGN(64);
   __CTOR_LIST__ = .;
   ___CTORS_LIST___ = .;
   KEEP (*crtbegin.o(.ctors))
   KEEP (*(EXCLUDE_FILE(*crtend.o) .ctors))
   KEEP (*(SORT(.ctors.*)))
   KEEP (*(.ctors))
   __CTOR_END__ = .;
   ___CTORS_END___ = .;
} > psu_ddr_0_MEM_0

.dtors : {
   . = ALIGN(64);
   __DTOR_LIST__ = .;
   ___DTORS_LIST___ = .;
   KEEP (*crtbegin.o(.dtors))
   KEEP (*(EXCLUDE_FILE(*crtend.o) .dtors))
   KEEP (*(SORT(.dtors.*)))
   KEEP (*(.dtors))
   __DTOR_END__ = .;
   ___DTORS_END___ = .;
} > psu_ddr_0_MEM_0

.fixup : {
   __fixup_start = .;
   *(.fixup)
   __fixup_end = .;
} > psu_ddr_0_MEM_0

.eh_frame : {
   *(.eh_frame)
} > psu_ddr_0_MEM_0

.eh_framehdr : {
   __eh_framehdr_start = .;
   *(.eh_framehdr)
   __eh_framehdr_end = .;
} > psu_ddr_0_MEM_0

.gcc_except_table : {
   *(.gcc_except_table)
} > psu_ddr_0_MEM_0

.mmu_tbl0 (ALIGN(4096)) : {
   __mmu_tbl0_start = .;
   *(.mmu_tbl0)
   __mmu_tbl0_end = .;
} > psu_ddr_0_MEM_0

.mmu_tbl1 (ALIGN(4096)) : {
   __mmu_tbl1_start = .;
   *(.mmu_tbl1)
   __mmu_tbl1_end = .;
} > psu_ddr_0_MEM_0

.mmu_tbl2 (ALIGN(4096)) : {
   __mmu_tbl2_start = .;
   *(.mmu_tbl2)
   __mmu_tbl2_end = .;
} > psu_ddr_0_MEM_0

.ARM.exidx : {
   __exidx_start = .;
   *(.ARM.exidx*)
   *(.gnu.linkonce.armexidix.*.*)
   __exidx_end = .;
} > psu_ddr_0_MEM_0

.preinit_array : {
   . = ALIGN(64);
   __preinit_array_start = .;
   KEEP (*(SORT(.preinit_array.*)))
   KEEP (*(.preinit_array))
   __preinit_array_end = .;
} > psu_ddr_0_MEM_0

.init_array : {
   . = ALIGN(64);
   __init_array_start = .;
   KEEP (*(SORT(.init_array.*)))
   KEEP (*(.init_array))
   __init_array_end = .;
} > psu_ddr_0_MEM_0

.fini_array : {
   . = ALIGN(64);
   __fini_array_start = .;
   KEEP (*(SORT(.fini_array.*)))
   KEEP (*(.fini_array))
   __fini_array_end = .;
} > psu_ddr_0_MEM_0

.ARM.attributes : {
   __ARM.attributes_start = .;
   *(.ARM.attributes)
   __ARM.attributes_end = .;
} > psu_ddr_0_MEM_0

.sdata : {
   . = ALIGN(64);
   __sdata_start = .;
   *(.sdata)
   *(.sdata.*)
   *(.gnu.linkonce.s.*)
   __sdata_end = .;
} > psu_ddr_0_MEM_0

.sbss (NOLOAD) : {
   . = ALIGN(64);
   __sbss_start = .;
   *(.sbss)
   *(.sbss.*)
   *(.gnu.linkonce.sb.*)
   . = ALIGN(64);
   __sbss_end = .;
} > psu_ddr_0_MEM_0

.tdata : {
   . = ALIGN(64);
   __tdata_start = .;
   *(.tdata)
   *(.tdata.*)
   *(.gnu.linkonce.td.*)
   __tdata_end = .;
} > psu_ddr_0_MEM_0

.tbss : {
   . = ALIGN(64);
   __tbss_start = .;
   *(.tbss)
   *(.tbss.*)
   *(.gnu.linkonce.tb.*)
   __tbss_end = .;
} > psu_ddr_0_MEM_0

.bss (NOLOAD) : {
   . = ALIGN(64);
   __bss_start__ = .;
   *(.bss)
   *(.bss.*)
   *(.gnu.linkonce.b.*)
   *(COMMON)
   . = ALIGN(64);
   __bss_end__ = .;
} > psu_ddr_0_MEM_0

_SDA_BASE_ = __sdata_start + ((__sbss_end - __sdata_start) / 2 );

_SDA2_BASE_ = __sdata2_start + ((__sbss2_end - __sdata2_start) / 2 );

/* Generate Stack and Heap definitions */

.heap (NOLOAD) : {
   . = ALIGN(64);
   _heap = .;
   HeapBase = .;
   _heap_start = .;
   . += _HEAP_SIZE;
   _heap_end = .;
   HeapLimit = .;
} > psu_ddr_0_MEM_0

.stack (NOLOAD) : {
   . = ALIGN(64);
   _el3_stack_end = .;
   . += _STACK_SIZE;
   __el3_stack = .;
   _el2_stack_end = .;
   . += _EL2_STACK_SIZE;
   . = ALIGN(64);
   __el2_stack = .;
   _el1_stack_end = .;
   . += _EL1_STACK_SIZE;
   . = ALIGN(64);
   __el1_stack = .;
   _el0_stack_end = .;
   . += _EL0_STACK_SIZE;
   . = ALIGN(64);
   __el0_stack = .;
} > psu_ddr_0_MEM_0

_end = .;
}
