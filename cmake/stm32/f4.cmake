set(STM32_F4_TYPES 
    401xC 401xE 405xx 407xx 410Cx 410Rx 410Tx 411xE 
    412Cx 412Rx 412Vx 412Zx 413xx 415xx 417xx 423xx
    427xx 429xx 437xx 439xx 446xx 469xx 479xx
)
set(STM32_F4_TYPE_MATCH 
    "F401.[CB]" "F401.[ED]" "F405.." "F407.." "F410C." "F410R." "F410T." "F411.[CE]"
    "F412C." "F412R." "F412V." "F412Z." "F413.." "F415.." "F417.." "F423.."
    "F427.." "F429.." "F437.." "F439.." "F446.." "F469.." "F479.."
)
set(STM32_F4_RAM_SIZES 
     64K  96K 128K 128K  32K  32K  32K 128K
    256K 256K 256K 256K 256K 128K 128K 320K
    192K 192K 192K 192K 128K 320K 320K
)
set(STM32_F4_CCRAM_SIZES 
     0K  0K 64K 64K  0K  0K  0K  0K
     0K  0K  0K  0K 64K 64K 64K  0K
    64K 64K 64K 64K  0K 64K 64K
)

stm32_util_create_family_targets(F4)

target_compile_options(STM32::F4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::F4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
