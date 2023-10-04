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

#include "wch_flasher.h"
#include "wch_flasher_inner.h"
#include "debug_defines.h"
#include "programs.h"
#include "../utils.h"
#include "../config.h"

#define TAG "WSF_FLSH"

WchSwioFlasher_WchFlasher* WchSwioFlasher_WchFlasher_create(WchSwioFlasher_RiscVDebug* debug) {
    WchSwioFlasher_WchFlasher* handle = malloc(sizeof(WchSwioFlasher_WchFlasher));
    handle->debug = debug;
    return handle;
}

void WchSwioFlasher_WchFlasher_destroy(WchSwioFlasher_WchFlasher* handle) {
    free(handle);
}

WchSwioFlasher_Error WchSwioFlasher_WchFlasher_lock_flash(WchSwioFlasher_WchFlasher* handle) {
    RWCH_FLASH_CTLR ctrl = {.raw = 0};
    FURI_LOG_D(TAG, "lock_flash");

    // Read reg
    CHECK_ERR(WchSwioFlasher_RiscVDebug_get_mem_u32(handle->debug, ADDR_FLASH_CTLR, &ctrl.raw));

    // Lock flash
    ctrl.LOCK = 1;
    ctrl.FLOCK = 1;
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_mem_u32(handle->debug, ADDR_FLASH_CTLR, ctrl.raw));

    // Check lock status
    CHECK_ERR(WchSwioFlasher_RiscVDebug_get_mem_u32(handle->debug, ADDR_FLASH_CTLR, &ctrl.raw))

    if(!ctrl.LOCK) {
        return LOG_ERR_M(WchSwioFlasher_Error_TargetInInvalidState, "flash did not lock");
    }

    if(!ctrl.FLOCK) {
        return LOG_ERR_M(
            WchSwioFlasher_Error_TargetInInvalidState, "flash did not lock fast mode");
    }

    FURI_LOG_D(TAG, "lock_flash done");

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_WchFlasher_unlock_flash(WchSwioFlasher_WchFlasher* handle) {
    FURI_LOG_D(TAG, "unlock_flash");
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_mem_u32(handle->debug, ADDR_FLASH_KEYR, 0x45670123));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_mem_u32(handle->debug, ADDR_FLASH_KEYR, 0xCDEF89AB));

    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_mem_u32(handle->debug, ADDR_FLASH_MKEYR, 0x45670123));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_mem_u32(handle->debug, ADDR_FLASH_MKEYR, 0xCDEF89AB));

    // Check lock status
    RWCH_FLASH_CTLR ctrl = {.raw = 0};
    CHECK_ERR(WchSwioFlasher_RiscVDebug_get_mem_u32(handle->debug, ADDR_FLASH_CTLR, &ctrl.raw))

    if(ctrl.LOCK) {
        return LOG_ERR_M(WchSwioFlasher_Error_TargetInInvalidState, "flash did not unlock");
    }

    if(ctrl.FLOCK) {
        return LOG_ERR_M(
            WchSwioFlasher_Error_TargetInInvalidState, "flash did not unlock fast mode");
    }

    FURI_LOG_D(TAG, "unlock_flash done");
    return WchSwioFlasher_Ok;
}

static WchSwioFlasher_Error run_flash_command(
    WchSwioFlasher_WchFlasher* handle,
    uint32_t addr,
    uint32_t ctl1,
    uint32_t ctl2) {
    CHECK_ERR(WchSwioFlasher_RiscVDebug_load_prog(
        handle->debug, &WchSwioFlasher_RiscVDebug_program_flash_command_program));

    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_gpr(handle->debug, 10, 0x40022000));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_gpr(handle->debug, 11, addr));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_gpr(handle->debug, 12, ctl1));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_gpr(handle->debug, 13, ctl2));

    CHECK_ERR(WchSwioFlasher_RiscVDebug_run_prog(handle->debug, 500));
    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error
    WchSwioFlasher_WchFlasher_wipe_page(WchSwioFlasher_WchFlasher* handle, uint32_t dst_addr) {
    CHECK_ERR(WchSwioFlasher_WchFlasher_unlock_flash(handle));
    dst_addr |= 0x08000000;
    run_flash_command(handle, dst_addr, BIT_CTLR_FTER, BIT_CTLR_FTER | BIT_CTLR_STRT);
    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error
    WchSwioFlasher_WchFlasher_wipe_sector(WchSwioFlasher_WchFlasher* handle, uint32_t dst_addr) {
    CHECK_ERR(WchSwioFlasher_WchFlasher_unlock_flash(handle));
    dst_addr |= 0x08000000;
    run_flash_command(handle, dst_addr, BIT_CTLR_PER, BIT_CTLR_PER | BIT_CTLR_STRT);
    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_WchFlasher_wipe_chip(WchSwioFlasher_WchFlasher* handle) {
    CHECK_ERR(WchSwioFlasher_WchFlasher_unlock_flash(handle));
    uint32_t dst_addr = 0x08000000;
    run_flash_command(handle, dst_addr, BIT_CTLR_MER, BIT_CTLR_MER | BIT_CTLR_STRT);
    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_WchFlasher_write_flash(
    WchSwioFlasher_WchFlasher* handle,
    uint32_t dst_addr,
    void* blob,
    int size) {
    ERROR_HANDLER_INIT();

    FURI_LOG_D(TAG, "write_flash");

    CHECK_ERR(WchSwioFlasher_WchFlasher_unlock_flash(handle));

    if(size & 0x03) {
        return LOG_ERR_M(
            WchSwioFlasher_Error_InvalidArgument, "bad size alignment" FMT_4HEX, _UI(size));
    }

    int size_dwords = size / 4;

    dst_addr |= 0x08000000;

    // setup FLASH registers and clear internal 64B buffer
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_mem_u32(handle->debug, ADDR_FLASH_ADDR, dst_addr));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_mem_u32(
        handle->debug, ADDR_FLASH_CTLR, BIT_CTLR_FTPG | BIT_CTLR_BUFRST));

    // Wait until buffer is cleared (until is FLASH busy)
    CHECK_ERR(WchSwioFlasher_RiscVDebug_wait_for_reg(
        handle->debug,
        ADDR_FLASH_STATR,
        BIT_STATR_EOP,
        BIT_STATR_EOP,
        WCHF_WAIT_FOR_WRITE_FLASH_TIMEOUT));

    // Load flash program
    CHECK_ERR(WchSwioFlasher_RiscVDebug_load_prog(
        handle->debug, &WchSwioFlasher_RiscVDebug_program_write_flash_program));

    // Setup GPR
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_gpr(
        handle->debug, WCH_REG_A0, 0x40022000)); // flash registers base
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_gpr(
        handle->debug, WCH_REG_A1, 0xE00000F4)); // DATA0 @ 0xE00000F4
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_gpr(handle->debug, WCH_REG_A2, dst_addr));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_gpr(
        handle->debug, WCH_REG_A3, BIT_CTLR_FTPG | BIT_CTLR_BUFLOAD));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_gpr(
        handle->debug, WCH_REG_A4, BIT_CTLR_FTPG | BIT_CTLR_STRT));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_gpr(
        handle->debug, WCH_REG_A5, BIT_CTLR_FTPG | BIT_CTLR_BUFRST));

    bool first_word = true;
    int page_count = (size_dwords + 15) / 16;

    uint32_t page_size = 64; // TODO move to config

    // Start feeding dwords to prog_write_flash.

    //uint32_t busy_time = 0;

    for(int page = 0; page < page_count; page++) {
        for(int dword_idx = 0; dword_idx < 16; dword_idx++) {
            int offset = (page * page_size) + (dword_idx * sizeof(uint32_t));
            uint32_t* src = (uint32_t*)((uint8_t*)blob + offset);

            // We have to write full pages only, so if we run out of source data we
            // write 0xDEADBEEF in the empty space.
            CHECK_ERR(WchSwioFlasher_RiscVDebug_set_data0(
                handle->debug, dword_idx < size_dwords ? *src : 0xDEADBEEF));

            if(first_word) {
                // There's a chip bug here - we can't set AUTOCMD before COMMAND or
                // things break all weird

                // This run_prog _must_ include a busywait
                CHECK_ERR(WchSwioFlasher_RiscVDebug_run_prog(
                    handle->debug, WCHF_WAIT_FOR_WRITE_FLASH_TIMEOUT));
                CHECK_ERR(WchSwioFlasher_RiscVDebug_set_abstractauto(handle->debug, 0x00000001));
                first_word = false;
            } else {
                // We can write flash slightly faster if we only busy-wait at the end
                // of each page, but I am wary...
                // Waiting here takes 54443 us to write 564 bytes
                //uint32_t time_a = time_us_32();
                CHECK_ERR(WchSwioFlasher_RiscVDebug_wait_for_abstractcs(
                    handle->debug, DM_ABSTRACTCS_BUSY, 0, WCHF_WAIT_FOR_WRITE_FLASH_TIMEOUT));

                //uint32_t time_b = time_us_32();
                //busy_time += time_b - time_a;
            }
        }
        // This is the end of a page
        // Waiting here instead of the above takes 42847 us to write 564 bytes
        //uint32_t time_a = time_us_32();
        //while (rvd->get_abstractcs().BUSY) {}
        //uint32_t time_b = time_us_32();
        //busy_time += time_b - time_a;
    }

    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_abstractauto(handle->debug, 0x00000000));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_mem_u32(handle->debug, ADDR_FLASH_CTLR, 0));

    // Write 1 to clear EOP. Not sure if we need to do this...
    RWCH_FLASH_STATR statr = {.raw = 0};
    CHECK_ERR(WchSwioFlasher_RiscVDebug_get_mem_u32(handle->debug, ADDR_FLASH_STATR, &statr.raw));
    statr.EOP = 1;
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_mem_u32(handle->debug, ADDR_FLASH_STATR, statr.raw));

    //printf("busy_time %d\n", busy_time);

    FURI_LOG_D(TAG, "write_flash done");

    return WchSwioFlasher_Ok;
}

/*
//WchSwioFlasher_Error WchSwioFlasher_WchFlasher_wipe_chip(WchSwioFlasher_WchFlasher* handle) {
WchSwioFlasher_Error WchSwioFlasher_WchFlasher_verify_flash(
    WchSwioFlasher_WchFlasher* handle,
    uint32_t dst_addr,
    void* blob,
    int size) {
    FURI_LOG_D(TAG, "verify_flash");
    //LOG("WCHFlash::verify_flash(0x%08x, 0x%08x, %d)\n", dst_addr, blob, size);

    if(size > MAX_VERIFY_SIZE) {
        return LOG_ERR_M(
            WchSwioFlasher_Error_InvalidArgument,
            "maximum size for verify is %d",
            MAX_VERIFY_SIZE);
    }

    dst_addr |= 0x08000000;

    uint8_t* readback = malloc(size);
    if(!readback) {
        return LOG_ERR_M(WchSwioFlasher_Error_NoFreeMemory, "unable to allocate readback buffer");
    }

    rvd->get_block_aligned(dst_addr, readback, size);

    uint8_t* data = (uint8_t*)blob;
    bool mismatch = false;
    for(int i = 0; i < size; i++) {
        if(data[i] != readback[i]) {
            LOG_R(
                "Flash readback failed at address 0x%08x - want 0x%02x, got 0x%02x\n",
                dst_addr + i,
                data[i],
                readback[i]);
            mismatch = true;
        }
    }

    delete[] readback;

    FURI_LOG_D(TAG, "verify_flash done");
    free(readback);
    return !mismatch;
}

// read flash size
NHCLinkWriteReg32: A3 05 E0 F7 FF 1F 00 00
NHCLinkWriteReg32: A3 17 00 10 24 00 00 00
NHCLinkReadReg32 TX: A2 16 00 00 00 00 00 00
NHCLinkReadReg32 RX: A2 02 00 00 08 00 00 00
NHCLinkReadReg32 TX: A2 04 00 10 24 00 00 00
NHCLinkReadReg32 RX: A2 10 00 FF FF 00 00 00
*/