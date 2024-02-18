set(STM32_F4_TYPES 
    F401xC F401xE F405xx F407xx F410Cx F410Rx F410Tx F411xE 
    F412Cx F412Rx F412Vx F412Zx F413xx F415xx F417xx F423xx
    F427xx F429xx F437xx F439xx F446xx F469xx F479xx
)
set(STM32_F4_TYPE_MATCH 
    "F401.[CB]" "F401.[ED]" "F405.." "F407.." "F410C." "F410R." "F410T." "F411.[CE]"
    "F412C." "F412R." "F412V." "F412Z." "F413.." "F415.." "F417.." "F423.."
    "F427.." "F429.." "F437.." "F439.." "F446.." "F469.." "F479.."
)
set(STM32_F4_RAM_SIZES 
     64K  96K 128K 128K  32K  32K  32K 128K
    256K 256K 256K 256K 320K 128K 128K 320K
    192K 192K 192K 192K 128K 320K 320K
)
set(STM32_F4_CCRAM_SIZES 
     0K  0K 64K 64K  0K  0K  0K  0K
     0K  0K  0K  0K  0K 64K 64K  0K
    64K 64K 64K 64K  0K 64K 64K
)

stm32_util_create_family_targets(F4)

target_compile_options(STM32::F4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
target_link_options(STM32::F4 INTERFACE 
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
)
