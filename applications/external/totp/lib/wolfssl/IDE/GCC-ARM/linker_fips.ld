MEMORY
{
  FLASH (wx) : ORIGIN = 0x00000000, LENGTH = 256K
  RAM   (wx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
    __vectors_start__ = .;
    .vectors : { *(.vectors) } > FLASH
    __vectors_end__ = __vectors_start__ + 0x400;

    /* Custom section for wolfCrypt FIPS module */
    .wolfCryptFIPSModule_text :
    {
        . = ALIGN(4);
        KEEP(wolfcrypt_first.o (.text .text* ))
        KEEP(aes.o(.text .text* ))
        KEEP(cmac.o (.text .text* ))
        KEEP(des3.o (.text .text* ))
        KEEP(dh.o (.text .text* ))
        KEEP(ecc.o (.text .text* ))
        KEEP(fips.o (.text .text* ))
        KEEP(fips_test.o (.text .text* ))
        KEEP(hmac.o (.text .text* ))
        KEEP(random.o(.text .text* ))
        KEEP(rsa.o (.text .text* ))
        KEEP(sha.o (.text .text* ))
        KEEP(sha256.o (.text .text* ))
        KEEP(sha3.o (.text .text* ))
        KEEP(sha512.o (.text .text* ))
        KEEP(wolfcrypt_last.o(.text .text*))
        . = ALIGN(4);
    } > FLASH
    .wolfCryptFIPSModule_rodata :
    {
        . = ALIGN(4);
        KEEP(wolfcrypt_first.o (.rodata .rodata*))
        KEEP(aes.o(.rodata .rodata*))
        KEEP(cmac.o(.rodata .rodata*))
        KEEP(des3.o(.rodata .rodata*))
        KEEP(dh.o(.rodata .rodata*))
        KEEP(ecc.o(.rodata .rodata*))
        KEEP(fips.o(.rodata .rodata*))
        KEEP(fips_test.o(.rodata .rodata*))
        KEEP(hmac.o(.rodata .rodata*))
        KEEP(random.o(.rodata .rodata*))
        KEEP(rsa.o(.rodata .rodata*))
        KEEP(sha.o(.rodata .rodata*))
        KEEP(sha256.o(.rodata .rodata*))
        KEEP(sha3.o(.rodata .rodata*))
        KEEP(sha512.o(.rodata .rodata*))
        KEEP(wolfcrypt_last.o(.rodata .rodata*))
        . = ALIGN(4);
    } > FLASH

    /* Custom section for wolfCrypt and LibC to prevent FIPS hash from changing 
        when application code changes are made */
    .wolfCryptNonFIPS_text :
    {
        . = ALIGN(4);
        KEEP(*wolf*src*.o(.text .text*))
        lib_a* ( .text .text*)
        . = ALIGN(4);
    } > FLASH
    .wolfCryptNonFIPS_rodata :
    {
        . = ALIGN(4);
        KEEP(*wolf*src*.o(.rodata .rodata*))
        lib_a* (.rodata .rodata*)
        . = ALIGN(4);
    } > FLASH
    
	.sys    : { *(.sys*) }    > FLASH
    .text   : { *(.text*) }   > FLASH
    .rodata : { *(.text*) }   > FLASH

	__data_load_start__ = .;
    __data_start__ = .;
    .data   : { *(.data*) }   > RAM
    __data_end__ = __data_start__ + SIZEOF(.data);

    __bss_start__ = .;
    .bss    : { *(.bss*)  }   > RAM
    __bss_end__   = __bss_start__ + SIZEOF(.bss);

    __heap_start__ = .;
    .heap   : { *(.heap*)  }   > RAM
    __heap_end__ = __heap_start__ + SIZEOF(.heap);

    end = .;
}
