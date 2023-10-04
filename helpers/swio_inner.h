/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include "swio.h"

#define SWI_TRIG_GPIO gpio_ext_pc1

#define SWI_GPIO gpio_ext_pc0
#define SWI_PORT_BASE GPIOC_BASE
#define SWI_PORT_BSSR_OFFSET 0x18
#define SWI_PORT_IDR_OFFSET 0x10
#define SWI_PORT_MODER_OFFSET 0x00
#define SWI_SET_MODE_OUTPUT_MASK GPIO_MODER_MODE0_0
#define SWI_SET_MODE_INPUT_MASK (~SWI_SET_MODE_OUTPUT_MASK)
#define SWI_SET_MASK GPIO_ODR_OD0
#define SWI_RESET_MASK (SWI_SET_MASK << 16)

#define WCH_DM_CPBR 0x7C
#define WCH_DM_CFGR 0x7D
#define WCH_DM_SHDWCFGR 0x7E

typedef enum {
    WchSwioFlasher_SWIO_Read = 0,
    WchSwioFlasher_SWIO_Write = 1,
} WchSwioFlasher_SWIO_RW;

WchSwioFlasher_Error
    WchSwioFlasher_SWIO_rxtx(uint8_t address, WchSwioFlasher_SWIO_RW rw, uint32_t* data);