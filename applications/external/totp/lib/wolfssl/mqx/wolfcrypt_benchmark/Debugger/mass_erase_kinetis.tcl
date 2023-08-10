# This script performs the mass erase operation for Kinetis targets

# Command mass erase with system reset and core reset
set mdmapControl [cmdwin::reg MDMAP_C -np]
set mdmapControl [expr $mdmapControl | 0x19]
cmdwin::reg MDMAP_C = $mdmapControl

# Release system reset while still holding core reset 
set mdmapControl [expr $mdmapControl & 0xFFFFFFF7]
cmdwin::reg MDMAP_C = $mdmapControl

# Wait for the mass erase operation to complete
set done 0
for {set i 0} {$i < 10} {incr i} {
	refresh 
	set mdmapControl [cmdwin::reg MDMAP_C -np]
	if {($mdmapControl & 1) == 0} {
		set done 1
		break;
	}
	wait 50
}

# Release the core reset
set mdmapControl [expr $mdmapControl & 0xFFFFFFEF]
cmdwin::reg MDMAP_C = $mdmapControl
if {$done} {
	puts "OK: Mass erase succeeded"
} else {
	puts "Error: Timeout"
}