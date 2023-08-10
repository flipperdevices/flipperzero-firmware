# this method initializes debug modules which are not affected by software reset 
# register names should be referenced including the register group name to improve performance 

proc init_debug_modules {} {
	# initialize LR to avoid invalid stack after reset
	reg "User\\/System Mode Registers/LR" = 0xFFFFFFFF

    # clear DWT function registers
	reg "Core Debug Registers/DEMCR" = 0x1000001
	reg "Data Watchpoint and Trace Unit Registers/DWT_FUNCTION0" = 0x0
	reg "Data Watchpoint and Trace Unit Registers/DWT_FUNCTION1" = 0x0
	reg "Data Watchpoint and Trace Unit Registers/DWT_FUNCTION2" = 0x0
	reg "Data Watchpoint and Trace Unit Registers/DWT_FUNCTION3" = 0x0
	# clear FPB comparators
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP0" = 0x0
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP1" = 0x0
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP2" = 0x0
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP3" = 0x0
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP4" = 0x0
	reg "Flash Patch and Breakpoint Unit Registers/FP_COMP5" = 0x0
}

proc init_trace_modules {} {
    # clear DWT registers
	reg "Data Watchpoint and Trace Unit Registers/DWT_CTRL" =0x40000000
	reg "Data Watchpoint and Trace Unit Registers/DWT_CYCCNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_CPICNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_EXCCNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_SLEEPCNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_LSUCNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_FOLDCNT" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_COMP0" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_COMP1" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_COMP2" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_COMP3" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_MASK0" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_MASK1" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_MASK2" =0x0
    reg "Data Watchpoint and Trace Unit Registers/DWT_MASK3" =0x0
	# clear ITM registers
    reg "Instrumentation Trace Macrocell Registers/ITM_LAR" =0xc5acce55
	reg "Instrumentation Trace Macrocell Registers/ITM_TER" =0x0
	reg "Instrumentation Trace Macrocell Registers/ITM_TPR" =0x0
    reg "Instrumentation Trace Macrocell Registers/ITM_TCR" =0x0
    reg "Instrumentation Trace Macrocell Registers/ITM_LAR" =0x1
    # reset Funnel registers
    reg "Embedded Trace Funnel Registers/ETF_FCR" =0x300
    # clear MCM registers
    reg "Core Platform Miscellaneous Control Module (MCM) Registers/MCM_ETBCC" =0x0
    reg "Core Platform Miscellaneous Control Module (MCM) Registers/MCM_ETBRL" =0x0
    # set SCB_VTOR register for RAM
    reg "System Control Registers/SCB_VTOR" =0x20000000
}

proc envsetup {} {
	# Environment Setup
	radix x 
	config hexprefix 0x
	config MemIdentifier p 
	config MemWidth 32 
	config MemAccess 32 
	config MemSwap off
}

#-------------------------------------------------------------------------------
# Main                                                                          
#-------------------------------------------------------------------------------

  envsetup
  init_debug_modules
  init_trace_modules
