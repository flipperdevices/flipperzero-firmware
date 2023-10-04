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

#include <furi.h>
#include "../utils.h"
#include "riscv_debug.h"
#include "risc_debug_inner.h"
#include "debug_defines.h"
#include "programs.h"
#include "../config.h"

#define TAG "WSF_RVD"

#ifdef RVD_CHECK_PROGRAM_EXECUTION_ENABLED
static const char* abstractsc_cmderr[] = {
    "no-error",
    "cmd-exec-error", // Abstract command execution to write to command, abstractcs, abstractauto registers or read and write to data and progbuf registers
    "unsupported-cmd", // Does not support current abstract command
    "exception", // Execution of abstract command with exception
    "not-halted", // The microprocessor is not halted or unavailable and cannot execute abstract commands,
    "bus-error", // Bus error
    "parity-err", // Parity bit error during communication
    "other", // Other errors
};
#endif

WchSwioFlasher_RiscVDebug* WchSwioFlasher_RiscVDebug_create(WchSwioFlasher_SWIO* swio) {
    WchSwioFlasher_RiscVDebug* handle = malloc(sizeof(WchSwioFlasher_RiscVDebug));
    handle->swio = swio;
    handle->reg_count = 16;
    handle->prog_will_clobber = 0;
    return handle;
}

void WchSwioFlasher_RiscVDebug_destroy(WchSwioFlasher_RiscVDebug* handle) {
    free(handle);
}

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_init(WchSwioFlasher_RiscVDebug* handle) {
    for(int i = 0; i < 8; i++) {
        handle->prog_cache[i] = 0xDEADBEEF;
    }
    for(int i = 0; i < 32; i++) {
        handle->reg_cache[i] = 0xDEADBEEF;
    }
    handle->dirty_regs = 0;
    handle->cached_regs = 0;

    return WchSwioFlasher_Ok;
}

static inline WchSwioFlasher_Error
    set_dmcontrol(WchSwioFlasher_RiscVDebug* handle, uint32_t dmcontrol) {
    return WchSwioFlasher_SWIO_write(handle->swio, DM_DMCONTROL, dmcontrol);
}

static inline WchSwioFlasher_Error
    get_dmstatus(WchSwioFlasher_RiscVDebug* handle, uint32_t* dmstatus) {
    return WchSwioFlasher_SWIO_read(handle->swio, DM_DMSTATUS, dmstatus);
}

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_get_data0(WchSwioFlasher_RiscVDebug* handle, uint32_t* data) {
    return WchSwioFlasher_SWIO_read(handle->swio, DM_DATA0, data);
}

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_get_data1(WchSwioFlasher_RiscVDebug* handle, uint32_t* data) {
    return WchSwioFlasher_SWIO_read(handle->swio, DM_DATA1, data);
}

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_set_data0(WchSwioFlasher_RiscVDebug* handle, uint32_t data) {
    return WchSwioFlasher_SWIO_write(handle->swio, DM_DATA0, data);
}

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_set_data1(WchSwioFlasher_RiscVDebug* handle, uint32_t data) {
    return WchSwioFlasher_SWIO_write(handle->swio, DM_DATA1, data);
}

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_set_command(WchSwioFlasher_RiscVDebug* handle, RVD_COMMAND* command) {
    return WchSwioFlasher_SWIO_write(handle->swio, DM_COMMAND, command->raw);
}

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_get_abstractcs(WchSwioFlasher_RiscVDebug* handle, uint32_t* data) {
    return WchSwioFlasher_SWIO_read(handle->swio, DM_ABSTRACTCS, data);
}

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_set_abstractauto(WchSwioFlasher_RiscVDebug* handle, uint32_t data) {
    return WchSwioFlasher_SWIO_write(handle->swio, DM_ABSTRACTAUTO, data);
}

static WchSwioFlasher_Error wait_for_flag(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t address,
    uint32_t mask,
    uint32_t flag,
    uint32_t timeout) {
    UNUSED(handle);

    uint32_t tm = furi_get_tick();
    uint32_t val = (uint32_t)-1;
    do {
        CHECK_ERR(WchSwioFlasher_SWIO_read(handle->swio, address, &val));

        if((furi_get_tick() - tm) > timeout) {
            return LOG_ERR_M(
                WchSwioFlasher_Error_Timeout,
                "wait_for_flag timeout (flag " FMT_4HEX ", mask " FMT_4HEX
                ", register %02X, last readout " FMT_4HEX ")",
                _UI(flag),
                _UI(mask),
                _UI(address),
                _UI(val));
        }

    } while((val & mask) != flag);

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_wait_for_reg(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t address,
    uint32_t mask,
    uint32_t flag,
    uint32_t timeout) {
    UNUSED(handle);

    uint32_t tm = furi_get_tick();
    uint32_t val = (uint32_t)-1;
    do {
        CHECK_ERR(WchSwioFlasher_RiscVDebug_get_mem_u32(handle, address, &val));

        if((furi_get_tick() - tm) > timeout) {
            return LOG_ERR_M(
                WchSwioFlasher_Error_Timeout,
                "wait_for_reg timeout (flag " FMT_4HEX ", mask " FMT_4HEX ", register " FMT_4HEX
                ", last readout " FMT_4HEX ")",
                _UI(flag),
                _UI(mask),
                _UI(address),
                _UI(val));
        }

    } while((val & mask) != flag);

    return WchSwioFlasher_Ok;
}

static inline WchSwioFlasher_Error
    wait_for_dmstatus(WchSwioFlasher_RiscVDebug* handle, uint32_t mask, uint32_t flag) {
    return wait_for_flag(handle, DM_DMSTATUS, mask, flag, RVD_WAIT_FOR_DM_STATUS_TIMEOUT);
}

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_wait_for_abstractcs(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t mask,
    uint32_t flag,
    uint32_t timeout) {
    return wait_for_flag(handle, DM_ABSTRACTCS, mask, flag, timeout);
}

static WchSwioFlasher_Error
    get_csr(WchSwioFlasher_RiscVDebug* handle, uint16_t index, uint32_t* value) {
    RVD_COMMAND cmd = {.raw = 0};

    cmd.REGNO = index;
    cmd.TRANSFER = 1;
    cmd.AARSIZE = 2;

    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_command(handle, &cmd));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_get_data0(handle, value));

    return WchSwioFlasher_Ok;
}

static WchSwioFlasher_Error
    set_csr(WchSwioFlasher_RiscVDebug* handle, uint16_t index, uint32_t value) {
    RVD_COMMAND cmd = {.raw = 0};

    cmd.REGNO = index;
    cmd.WRITE = 1;
    cmd.TRANSFER = 1;
    cmd.AARSIZE = 2;

    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_data0(handle, value));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_command(handle, &cmd));

    return WchSwioFlasher_Ok;
}

static WchSwioFlasher_Error get_dcsr(WchSwioFlasher_RiscVDebug* handle, uint32_t* value) {
    return get_csr(handle, CSR_DCSR, value);
}

static WchSwioFlasher_Error set_dcsr(WchSwioFlasher_RiscVDebug* handle, uint32_t value) {
    return set_csr(handle, CSR_DCSR, value);
}

static WchSwioFlasher_Error get_dpc(WchSwioFlasher_RiscVDebug* handle, uint32_t* value) {
    return get_csr(handle, CSR_DPC, value);
}

static WchSwioFlasher_Error set_dpc(WchSwioFlasher_RiscVDebug* handle, uint32_t value) {
    return set_csr(handle, CSR_DPC, value);
}

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_get_gpr(
    WchSwioFlasher_RiscVDebug* handle,
    uint8_t index,
    uint32_t* value) {
    if(index == 16) {
        CHECK_ERR(get_dpc(handle, value));
        return WchSwioFlasher_Ok;
    }

    RVD_COMMAND cmd = {.raw = 0};

    cmd.REGNO = 0x1000 | index;
    cmd.TRANSFER = 1;
    cmd.AARSIZE = 2;

    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_command(handle, &cmd));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_get_data0(handle, value));

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_set_gpr(
    WchSwioFlasher_RiscVDebug* handle,
    uint8_t index,
    uint32_t value) {
    if(index == 16) {
        CHECK_ERR(set_dpc(handle, value));
        return WchSwioFlasher_Ok;
    }

    RVD_COMMAND cmd = {.raw = 0};

    cmd.REGNO = 0x1000 | index;
    cmd.WRITE = 1;
    cmd.TRANSFER = 1;
    cmd.AARSIZE = 2;

    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_data0(handle, value));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_command(handle, &cmd));

    return WchSwioFlasher_Ok;
}

static WchSwioFlasher_Error enable_breakpoints(WchSwioFlasher_RiscVDebug* handle) {
    FURI_LOG_D(TAG, "enable_breakpoints");

    uint32_t dmstatus = 0;
    CHECK_ERR_M(get_dmstatus(handle, &dmstatus), "unable to get DMSTATUS");

    if((dmstatus & DM_DMSTATUS_ALLHALTED) == 0) {
        return LOG_ERR(WchSwioFlasher_Error_TargetInInvalidState);
    }

    uint32_t dcsr = 0;
    CHECK_ERR_M(get_dcsr(handle, &dcsr), "unable to get DCSR");
    FURI_LOG_D(TAG, "previous DCSR = " FMT_4HEX, _UI(dcsr));

    dcsr |= CSR_DCSR_EBREAKM | CSR_DCSR_EBREAKS | CSR_DCSR_EBREAKU | CSR_DCSR_STOPCOUNT |
            CSR_DCSR_STOPTIME;
    CHECK_ERR_M(set_dcsr(handle, dcsr), "unable to set DCSR");
    FURI_LOG_D(TAG, "current DCSR = " FMT_4HEX, _UI(dcsr));

    FURI_LOG_D(TAG, "enable_breakpoints done");

    return WchSwioFlasher_Ok;
}
/*
WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_load_prog(
    WchSwioFlasher_RiscVDebug* handle,
    const char* name,
    uint32_t* prog,
    uint32_t clobber) {*/
WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_load_prog(
    WchSwioFlasher_RiscVDebug* handle,
    const WchSwioFlasher_RiscVProgram* program) {
    FURI_LOG_D(TAG, "load_prog name=%s", program->name);

    uint32_t* prog = (uint32_t*)program->data;
    // Upload any PROG{N} word that changed.
    for(int i = 0; i < 8; i++) {
        if(handle->prog_cache[i] != prog[i]) {
            CHECK_ERR(WchSwioFlasher_SWIO_write(handle->swio, DM_PROGBUF0 + i, prog[i]));
#ifdef RVD_CHECK_PROGRAM_UPLOAD_ENABLED
            uint32_t v;
            CHECK_ERR(WchSwioFlasher_SWIO_read(handle->swio, DM_PROGBUF0 + i, &v));
            if(prog[i] != v) {
                return LOG_ERR_M(
                    WchSwioFlasher_Error_TargetInInvalidState, "program upload failed");
            }
#endif
            handle->prog_cache[i] = prog[i];
        }
    }

    // Save any registers this program is going to clobber.
    for(int i = 0; i < handle->reg_count; i++) {
        if(program->clobbers & (1 << i)) {
            if(handle->cached_regs & (1 << i)) {
                if(handle->dirty_regs & (1 << i)) {
                    uint32_t val;
                    CHECK_ERR_M(
                        WchSwioFlasher_RiscVDebug_get_gpr(handle, i, &val),
                        "unable to get reg[%d]",
                        i);
                    handle->reg_cache[i] = val;
                    handle->cached_regs |= (1 << i);
                } else {
                    return LOG_ERR_M(
                        WchSwioFlasher_Error_DirtyRegs,
                        "reg %d is about to be clobbered, but we can't get a clean copy because it's already dirty",
                        i);
                }
            }
        }
    }

    handle->prog_will_clobber = program->clobbers;

    //LOG("RVDebug::load_prog() done\n");
    FURI_LOG_D(TAG, "load_prog done");

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_reset(WchSwioFlasher_RiscVDebug* handle) {
    FURI_LOG_D(TAG, "reset_cpu");

    // Halt and leave halt request set
    CHECK_ERR_M(set_dmcontrol(handle, 0x80000001), "unable to HALT ALL");
    CHECK_ERR(wait_for_dmstatus(handle, DM_DMSTATUS_ALLHALTED, DM_DMSTATUS_ALLHALTED));

    // Set reset request
    CHECK_ERR_M(set_dmcontrol(handle, 0x80000003), "unable to HAVE RESET");
    CHECK_ERR(wait_for_dmstatus(handle, DM_DMSTATUS_ALLHAVERESET, DM_DMSTATUS_ALLHAVERESET));

    // Clear reset request and hold halt request
    CHECK_ERR_M(set_dmcontrol(handle, 0x80000001), "unable to clear reset and hold halt");
    // this busywait seems to be required or we hang
    CHECK_ERR(wait_for_dmstatus(handle, DM_DMSTATUS_ALLHALTED, DM_DMSTATUS_ALLHALTED));

    // Clear HAVERESET
    CHECK_ERR_M(set_dmcontrol(handle, 0x90000001), "unable to clear HAVE RESET");
    CHECK_ERR(wait_for_dmstatus(handle, DM_DMSTATUS_ALLHAVERESET, 0));

    // Clear halt request
    CHECK_ERR_M(set_dmcontrol(handle, 0x00000001), "unable to clear halt request");

    uint32_t dmstatus;
    CHECK_ERR(get_dmstatus(handle, &dmstatus));
    FURI_LOG_D(TAG, ">>> dmstatus " FMT_4HEX, _UI(dmstatus));

    // Reset cached state
    CHECK_ERR(WchSwioFlasher_RiscVDebug_init(handle));

    // Resetting the CPU also resets DCSR, redo it.
    CHECK_ERR(enable_breakpoints(handle));

    FURI_LOG_D(TAG, "reset_cpu done");

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_run_prog(WchSwioFlasher_RiscVDebug* handle, uint32_t timeout) {
    uint32_t abstracts;
    RVD_COMMAND cmd = {.raw = 0};
    cmd.POSTEXEC = 1;

    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_command(handle, &cmd));

    if(timeout != 0) {
        CHECK_ERR(
            WchSwioFlasher_RiscVDebug_wait_for_abstractcs(handle, DM_ABSTRACTCS_BUSY, 0, timeout));
    } else {
        // It takes 40 usec to do _anything_ over the debug interface, so if the
        // program is "fast" then we should _never_ see BUSY... right?

        CHECK_ERR(WchSwioFlasher_RiscVDebug_get_abstractcs(handle, &abstracts));
        if(abstracts & DM_ABSTRACTCS_BUSY) {
            return LOG_ERR(WchSwioFlasher_Error_ProgramNotFinishedYet);
        }
    }
#ifdef RVD_CHECK_PROGRAM_EXECUTION_ENABLED
    CHECK_ERR(WchSwioFlasher_RiscVDebug_get_abstractcs(handle, &abstracts));

    uint32_t err;
    switch(abstracts & DM_ABSTRACTCS_CMDERR) {
    case DM_ABSTRACTCS_CMDERR_NONE:
        break;
    default:
        err = (abstracts & DM_ABSTRACTCS_CMDERR) >> DM_ABSTRACTCS_CMDERR_OFFSET;
        return LOG_ERR_M(
            WchSwioFlasher_Error_ProgramRunError,
            "program run failed '%s' (err: %d)",
            abstractsc_cmderr[err],
            _UI(err));
    }
#endif
    handle->dirty_regs |= handle->prog_will_clobber;

    return WchSwioFlasher_Ok;
}

static WchSwioFlasher_Error
    get_mem_u32_aligned(WchSwioFlasher_RiscVDebug* handle, uint32_t addr, uint32_t* result) {
    // Address must be aligned to 4 bytes
    if(addr & 0x03) {
        return LOG_ERR_M(
            WchSwioFlasher_Error_InvalidArgument, "bad address alignment" FMT_4HEX, _UI(addr));
    }

    CHECK_ERR(WchSwioFlasher_RiscVDebug_load_prog(
        handle, &WchSwioFlasher_RiscVDebug_get_set_u32_program));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_data1(handle, addr));

    CHECK_ERR(WchSwioFlasher_RiscVDebug_run_prog(handle, WchSwioFlasher_RiscVDebug_NO_TIMEOUT));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_get_data0(handle, result));

    return WchSwioFlasher_Ok;
}

static WchSwioFlasher_Error
    set_mem_u32_aligned(WchSwioFlasher_RiscVDebug* handle, uint32_t addr, uint32_t data) {
    // Address must be aligned to 4 bytes
    if(addr & 0x03) {
        return LOG_ERR_M(
            WchSwioFlasher_Error_InvalidArgument, "bad address alignment" FMT_4HEX, _UI(addr));
    }

    CHECK_ERR(WchSwioFlasher_RiscVDebug_load_prog(
        handle, &WchSwioFlasher_RiscVDebug_get_set_u32_program));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_data0(handle, data));
    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_data1(handle, addr | 1));

    CHECK_ERR(WchSwioFlasher_RiscVDebug_run_prog(handle, WchSwioFlasher_RiscVDebug_NO_TIMEOUT));

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_get_mem_u32(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t addr,
    uint32_t* result) {
    uint32_t offset = addr & 3;
    uint32_t addr_lo = (addr + 0) & ~3;
    uint32_t addr_hi = (addr + 3) & ~3;

    uint32_t data_lo;
    CHECK_ERR(get_mem_u32_aligned(handle, addr_lo, &data_lo));

    if(offset == 0) {
        *result = data_lo;
        return WchSwioFlasher_Ok;
    }

    uint32_t data_hi;
    CHECK_ERR(get_mem_u32_aligned(handle, addr_hi, &data_hi));

    *result = (data_lo >> (offset * 8)) | (data_hi << (32 - offset * 8));

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_set_mem_u32(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t addr,
    uint32_t data) {
    uint32_t offset = addr & 3;
    uint32_t addr_lo = (addr + 0) & ~3;
    uint32_t addr_hi = (addr + 4) & ~3;

    if(offset == 0) {
        //set_mem_u32_aligned(addr_lo, data);
        CHECK_ERR(set_mem_u32_aligned(handle, addr_lo, data));
        return WchSwioFlasher_Ok;
    }

    uint32_t data_lo;
    CHECK_ERR(get_mem_u32_aligned(handle, addr_lo, &data_lo));
    uint32_t data_hi;
    CHECK_ERR(get_mem_u32_aligned(handle, addr_hi, &data_hi));

    if(offset == 1) {
        data_lo &= 0x000000FF;
        data_hi &= 0xFFFFFF00;
        data_lo |= data << 8;
        data_hi |= data >> 24;
    } else if(offset == 2) {
        data_lo &= 0x0000FFFF;
        data_hi &= 0xFFFF0000;
        data_lo |= data << 16;
        data_hi |= data >> 16;
    } else if(offset == 3) {
        data_lo &= 0x00FFFFFF;
        data_hi &= 0xFF000000;
        data_lo |= data << 24;
        data_hi |= data >> 8;
    }

    //set_mem_u32_aligned(addr_lo, data_lo);
    //set_mem_u32_aligned(addr_hi, data_hi);
    CHECK_ERR(set_mem_u32_aligned(handle, addr_lo, data_lo));
    CHECK_ERR(set_mem_u32_aligned(handle, addr_hi, data_hi));

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error WchSwioFlasher_RiscVDebug_get_block_aligned(
    WchSwioFlasher_RiscVDebug* handle,
    uint32_t addr,
    void* dst,
    int size_bytes) {
    // Address must be aligned to 4 bytes
    if(addr & 0x03) {
        return LOG_ERR_M(
            WchSwioFlasher_Error_InvalidArgument, "bad address alignment" FMT_4HEX, _UI(addr));
    }

    // Size must be aligned to 4 bytes
    if(size_bytes & 0x03) {
        return LOG_ERR_M(
            WchSwioFlasher_Error_InvalidArgument, "bad size alignment" FMT_4HEX, _UI(addr));
    }

    CHECK_ERR(WchSwioFlasher_RiscVDebug_load_prog(
        handle, &WchSwioFlasher_RiscVDebug_get_block_aligned_program));

    CHECK_ERR(WchSwioFlasher_RiscVDebug_set_data1(handle, addr));

    int size_dwords = size_bytes / 4;
    uint32_t* cursor = (uint32_t*)dst;

    for(int i = 0; i < size_dwords; i++) {
        if(i == 0) {
            CHECK_ERR(
                WchSwioFlasher_RiscVDebug_run_prog(handle, WchSwioFlasher_RiscVDebug_NO_TIMEOUT));
            CHECK_ERR(WchSwioFlasher_RiscVDebug_set_abstractauto(handle, 0x00000001));
        }
        if(i == size_dwords - 1) {
            CHECK_ERR(WchSwioFlasher_RiscVDebug_set_abstractauto(handle, 0x00000000));
        }
        CHECK_ERR(WchSwioFlasher_RiscVDebug_get_data0(handle, &cursor[i]));
    }

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_machine_isa(WchSwioFlasher_RiscVDebug* handle, uint32_t* result) {
    CHECK_ERR(WchSwioFlasher_RiscVDebug_load_prog(
        handle, &WchSwioFlasher_RiscVDebug_get_machine_isa_program));

    CHECK_ERR(WchSwioFlasher_RiscVDebug_run_prog(handle, RVD_WAIT_FOR_CHIPINFO_TIMEOUT));

    CHECK_ERR(WchSwioFlasher_RiscVDebug_get_data0(handle, result));

    return WchSwioFlasher_Ok;
}

WchSwioFlasher_Error
    WchSwioFlasher_RiscVDebug_get_status(WchSwioFlasher_RiscVDebug* handle, uint32_t* result) {
    return get_dmstatus(handle, result);
}