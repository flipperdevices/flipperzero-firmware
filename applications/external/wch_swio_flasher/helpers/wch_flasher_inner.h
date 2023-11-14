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

/**
This file is based on modified original from https://github.com/aappleby/PicoRVD
*/

#pragma once

#include <stdint.h>

typedef union {
    struct {
        uint32_t PG : 1; // Program enable
        uint32_t PER : 1; // Perform sector erase
        uint32_t MER : 1; // Perform full erase
        uint32_t PAD0 : 1;
        uint32_t OBG : 1; // Perform user-selected word programming
        uint32_t OBER : 1; // Perform user-selected word erasure
        uint32_t STRT : 1; // Start
        uint32_t LOCK : 1; // Flash lock status

        uint32_t PAD1 : 1;
        uint32_t OBWRE : 1; // User-selected word write enable
        uint32_t ERRIE : 1; // Error status interrupt control
        uint32_t PAD2 : 1;
        uint32_t EOPIE : 1; // EOP interrupt control
        uint32_t PAD3 : 2;
        uint32_t FLOCK : 1; // Fast programming mode lock

        uint32_t FTPG : 1; // Fast page programming?
        uint32_t FTER : 1; // Fast erase
        uint32_t BUFLOAD : 1; // "Cache data into BUF"
        uint32_t BUFRST : 1; // "BUF reset operation"
        uint32_t PAD4 : 12;
    };
    uint32_t raw;
} RWCH_FLASH_CTLR;

typedef union {
    struct {
        uint32_t BUSY : 1; // True if flash busy
        uint32_t PAD0 : 3;
        uint32_t WRPRTERR : 1; // True if the flash was written while locked
        uint32_t EOP : 1; // True if flash finished programming
        uint32_t PAD1 : 8;
        uint32_t MODE : 1; // Something to do with boot area flash?
        uint32_t BOOT_LOCK : 1; // True if boot flash locked
        uint32_t PAD2 : 16;
    };
    uint32_t raw;
} RWCH_FLASH_STATR;

#define ADDR_ESIG_FLACAP 0x1FFFF7E0 // Flash capacity register 0xXXXX
#define ADDR_ESIG_UNIID1 0x1FFFF7E8 // UID register 1 0xXXXXXXXX
#define ADDR_ESIG_UNIID2 0x1FFFF7EC // UID register 2 0xXXXXXXXX
#define ADDR_ESIG_UNIID3 0x1FFFF7F0 // UID register 3 0xXXXXXXXX

#define ADDR_FLASH_ACTLR 0x40022000
#define ADDR_FLASH_KEYR 0x40022004
#define ADDR_FLASH_OBKEYR 0x40022008
#define ADDR_FLASH_STATR 0x4002200C
#define ADDR_FLASH_CTLR 0x40022010
#define ADDR_FLASH_ADDR 0x40022014
#define ADDR_FLASH_OBR 0x4002201C
#define ADDR_FLASH_WPR 0x40022020
#define ADDR_FLASH_MKEYR 0x40022024
#define ADDR_FLASH_BKEYR 0x40022028

#define BIT_CTLR_PG (1 << 0)
#define BIT_CTLR_PER (1 << 1)
#define BIT_CTLR_MER (1 << 2)
#define BIT_CTLR_OBG (1 << 4)
#define BIT_CTLR_OBER (1 << 5)
#define BIT_CTLR_STRT (1 << 6)
#define BIT_CTLR_LOCK (1 << 7)
#define BIT_CTLR_OBWRE (1 << 9)
#define BIT_CTLR_ERRIE (1 << 10)
#define BIT_CTLR_EOPIE (1 << 12)
#define BIT_CTLR_FLOCK (1 << 15)
#define BIT_CTLR_FTPG (1 << 16)
#define BIT_CTLR_FTER (1 << 17)
#define BIT_CTLR_BUFLOAD (1 << 18)
#define BIT_CTLR_BUFRST (1 << 19)

#define BIT_STATR_BUSY (1 << 0)
#define BIT_STATR_WRPRTERR (1 << 4)
#define BIT_STATR_EOP (1 << 5)
#define BIT_STATR_MODE (1 << 14)
#define BIT_STATR_LOCK (1 << 15)

#define WCH_REG_S0 8
#define WCH_REG_S1 9
#define WCH_REG_A0 10
#define WCH_REG_A1 11
#define WCH_REG_A2 12
#define WCH_REG_A3 13
#define WCH_REG_A4 14
#define WCH_REG_A5 15