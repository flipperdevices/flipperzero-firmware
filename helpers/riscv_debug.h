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

#include <furi.h>
#include <furi_hal.h>
#include "../errors.h"
#include "swio.h"
#include "programs.h"

typedef struct {
    WchSwioFlasher_SWIO* swio;

    // Cached target state, must stay in sync
    int reg_count;

    uint32_t prog_cache[8];
    uint32_t prog_will_clobber; // Bits are 1 if running the current program will clober the reg

    uint32_t reg_cache[32];
    uint32_t dirty_regs; // bits are 1 if we modified the reg on device
    uint32_t cached_regs; // bits are 1 if reg_cache[i] is valid
} WchSwioFlasher_RiscVDebug;

typedef struct {
    uint32_t flash_size;
    uint32_t esig_uniid[3];
} WchSwioFlasher_RiscVDebug_ChipInfo;

WchSwioFlasher_RiscVDebug* WchSwioFlasher_RiscVDebug_create(WchSwioFlasher_SWIO* swio);

void WchSwioFlasher_RiscVDebug_destroy(WchSwioFlasher_RiscVDebug* handle);

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_init(WchSwioFlasher_RiscVDebug* handle);

typedef enum {
    WchSwioFlasher_RVD_ResetToHalt = 0,
    WchSwioFlasher_RVD_ResetToRun,
    WchSwioFlasher_RVD_ResetToRunNoCheck,
} WchSwioFlasher_RiscVDebug_ResetType;

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_reset(
    WchSwioFlasher_RiscVDebug* handle,
    WchSwioFlasher_RiscVDebug_ResetType type);

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_load_prog(
    WchSwioFlasher_RiscVDebug* handle,
    const WchSwioFlasher_RiscVProgram* program);

#define WchSwioFlasher_RiscVDebug_NO_TIMEOUT 0
WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_run_prog(WchSwioFlasher_RiscVDebug* handle, uint32_t timeout);

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_get_mem_u32(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t addr,
    uint32_t* result);

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_set_mem_u32(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t addr,
    uint32_t data);

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_get_gpr(
    WchSwioFlasher_RiscVDebug* handle,
    uint8_t index,
    uint32_t* value);

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_set_gpr(
    WchSwioFlasher_RiscVDebug* handle,
    uint8_t index,
    uint32_t value);

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_get_block_aligned(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t addr,
    void* dst,
    int size_bytes);

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_wait_for_reg(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t address,
    uint32_t mask,
    uint32_t flag,
    uint32_t timeout);

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_set_data0(WchSwioFlasher_RiscVDebug* handle, uint32_t data);

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_set_data1(WchSwioFlasher_RiscVDebug* handle, uint32_t data);

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_get_data0(WchSwioFlasher_RiscVDebug* handle, uint32_t* data);

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_get_data1(WchSwioFlasher_RiscVDebug* handle, uint32_t* data);

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_set_abstractauto(WchSwioFlasher_RiscVDebug* handle, uint32_t data);

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_wait_for_abstractcs(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t mask,
    uint32_t flag,
    uint32_t timeout);

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_get_status(WchSwioFlasher_RiscVDebug* handle, uint32_t* result);

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_machine_isa(WchSwioFlasher_RiscVDebug* handle, uint32_t* result);

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_get_chip_info(
    WchSwioFlasher_RiscVDebug* handle,
    WchSwioFlasher_RiscVDebug_ChipInfo* info);
