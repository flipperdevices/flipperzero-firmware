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

typedef enum {

    WchSwioFlasher_Ok = 0,
    WchSwioFlasher_Error_Timeout = -1000,
    WchSwioFlasher_Error_SwdResetDetected = -1001,
    WchSwioFlasher_Error_SwdParityCheckError = -1002,
    WchSwioFlasher_Error_TargetNotKnown = -2000,
    WchSwioFlasher_Error_TargetInInvalidState = -2001,
    WchSwioFlasher_Error_ProgramNotFinishedYet = -2002,
    WchSwioFlasher_Error_ProgramRunError = -2003,
    WchSwioFlasher_Error_DirtyRegs = -3001,
    WchSwioFlasher_Error_NotImplemented = -4000,
    WchSwioFlasher_Error_InvalidArgument = -4001,
    WchSwioFlasher_Error_NoFreeMemory = -4002,

} WchSwioFlasher_Error;

char* WchSwioFlasher_ErrorToString(WchSwioFlasher_Error error);

#define ERROR_HANDLER_INIT()

#define LOG_ERR(err)                           \
    ({                                         \
        FURI_LOG_E(                            \
            TAG,                               \
            "error '%s' (0x%08X) on %s:%d",    \
            WchSwioFlasher_ErrorToString(err), \
            err,                               \
            __FUNCTION__,                      \
            __LINE__);                         \
        err;                                   \
    })

#define LOG_ERR_M(err, msg, ...)                  \
    ({                                            \
        FURI_LOG_E(                               \
            TAG,                                  \
            "error '%s' (0x%08X) on %s:%d: " msg, \
            WchSwioFlasher_ErrorToString(err),    \
            err,                                  \
            __FUNCTION__,                         \
            __LINE__,                             \
            ##__VA_ARGS__);                       \
        err;                                      \
    })

#define CHECK_ERR(fn)                  \
    {                                  \
        WchSwioFlasher_Error err = fn; \
        if(err != WchSwioFlasher_Ok) { \
            return LOG_ERR(err);       \
        }                              \
    }

#define CHECK_ERR_M(fn, msg, ...)                      \
    {                                                  \
        WchSwioFlasher_Error err = fn;                 \
        if(err != WchSwioFlasher_Ok) {                 \
            return LOG_ERR_M(err, msg, ##__VA_ARGS__); \
        }                                              \
    }
