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

#include <stdint.h>
#include <furi.h>
#include <furi_hal_usb.h>

#include "helpers/riscv_debug.h"
#include "helpers/wch_flasher.h"

#include "errors.h"

typedef enum {
    WchSwioFlasher_CFF_ChipInfoCompleted = 0,
    WchSwioFlasher_CFF_OpenFileCompleted,
    WchSwioFlasher_CFF_EraseChipCompleted,
    WchSwioFlasher_CFF_WriteChipCompleted,
    WchSwioFlasher_CFF_ActionCount,
} ViewFlasher_Action;

typedef enum {
    VWchSwioFlasher_CFF_Ok = 0,
    VWchSwioFlasher_CFF_NoData,
    VWchSwioFlasher_CFF_InProgress,
    VWchSwioFlasher_CFF_ChipNotConnected,
    WchSwioFlasher_CFF_UnableToOpenFile,
    WchSwioFlasher_CFF_EmptyOrTooBigFile,
    VWchSwioFlasher_CFF_UnknownError,
} WchSwioFlasher_CFF_ResultStatus;

typedef struct {
    WchSwioFlasher_CFF_ResultStatus status;
    uint32_t flash_size;
    uint32_t esig_uniid[3];
} WchSwioFlasher_CFF_ChipInfo;

typedef struct {
    WchSwioFlasher_CFF_ResultStatus status;
} WchSwioFlasher_CFF_EraseChip;

typedef struct {
    WchSwioFlasher_CFF_ResultStatus status;
    char* path;
    float percent;
} WchSwioFlasher_CFF_WriteChip;

typedef void (*WchSwioFlasher_CFF_Callback)(void* context, ViewFlasher_Action action);

typedef struct WchSwioFlasher_Ch32vFlipperFlasher WchSwioFlasher_Ch32vFlipperFlasher;

void WchSwioFlasher_Ch32vFlipperFlasher_event_callback(
    WchSwioFlasher_Ch32vFlipperFlasher* handle,
    WchSwioFlasher_CFF_Callback cb,
    void* cb_context);

WchSwioFlasher_Ch32vFlipperFlasher* WchSwioFlasher_Ch32vFlipperFlasher_create(
    WchSwioFlasher_RiscVDebug* debugger,
    WchSwioFlasher_WchFlasher* flasher);

void WchSwioFlasher_Ch32vFlipperFlasher_destroy(WchSwioFlasher_Ch32vFlipperFlasher* handle);

void WchSwioFlasher_Ch32vFlipperFlasher_erase_chip(WchSwioFlasher_Ch32vFlipperFlasher* handle);

void WchSwioFlasher_Ch32vFlipperFlasher_erase_chip_data(
    WchSwioFlasher_Ch32vFlipperFlasher* handle,
    WchSwioFlasher_CFF_EraseChip* data);

void WchSwioFlasher_Ch32vFlipperFlasher_chip_info(WchSwioFlasher_Ch32vFlipperFlasher* handle);

void WchSwioFlasher_Ch32vFlipperFlasher_chip_info_data(
    WchSwioFlasher_Ch32vFlipperFlasher* handle,
    WchSwioFlasher_CFF_ChipInfo* data);

void WchSwioFlasher_Ch32vFlipperFlasher_write_chip(
    WchSwioFlasher_Ch32vFlipperFlasher* handle,
    char* path);

void WchSwioFlasher_Ch32vFlipperFlasher_write_chip_data(
    WchSwioFlasher_Ch32vFlipperFlasher* handle,
    WchSwioFlasher_CFF_WriteChip* data);

void WchSwioFlasher_Ch32vFlipperFlasher_detach(WchSwioFlasher_Ch32vFlipperFlasher* handle);

void WchSwioFlasher_Ch32vFlipperFlasher_attach(WchSwioFlasher_Ch32vFlipperFlasher* handle);