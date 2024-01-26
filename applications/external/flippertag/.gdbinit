set confirm off
set trace-commands on

define target remote
target extended-remote $arg0
set mem inaccessible-by-default off
source debug/flipperapps.py
fap-set-debug-elf-root build/latest/.extapps
end
