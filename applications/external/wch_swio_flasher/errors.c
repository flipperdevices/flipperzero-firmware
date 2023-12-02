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

#include "errors.h"

char* WchSwioFlasher_ErrorToString(WchSwioFlasher_Error error) {
    switch(error) {
    case WchSwioFlasher_Ok:
        return "no-error";
    case WchSwioFlasher_Error_SwdResetDetected:
        return "swd-reset";
    case WchSwioFlasher_Error_Timeout:
        return "timeout";
    case WchSwioFlasher_Error_SwdParityCheckError:
        return "parity-error";
    case WchSwioFlasher_Error_ProgramRunError:
        return "program-run-error";
    case WchSwioFlasher_Error_TargetInInvalidState:
        return "target-in-invalid-state";
    case WchSwioFlasher_Error_TargetNotKnown:
        return "target-not-known";
    case WchSwioFlasher_Error_ProgramNotFinishedYet:
        return "program-not-finished-yet";
    default:
        break;
    }

    return "<unk>";
}