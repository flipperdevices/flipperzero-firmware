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

//#define SWIO_TXRX_DEBUG_MSG_ENABLE
//#define SWIO_TRIGGER_OUT_ENABLE

#define WCHF_VERIFY_MAX_BUFFER_SIZE 1024
#define WCHF_WAIT_FOR_WRITE_FLASH_TIMEOUT 1000

//#define NCHLNK_TXRX_DEBUG_MSG_ENABLE
#define NCHLNK_TX_SEMAPHORE_TIMEOUT 5000

#define RVD_WAIT_FOR_DM_STATUS_TIMEOUT 500
#define RVD_WAIT_FOR_CHIPINFO_TIMEOUT 500

//#define RVD_TXRX_DEBUG_MSG_ENABLE
#define RVD_CHECK_PROGRAM_UPLOAD_ENABLED
#define RVD_CHECK_PROGRAM_EXECUTION_ENABLED

#define RVD_NCHLINKEMU_TX_BUFF_SIZE 2048
#define RVD_NCHLINKEMU_RX_BUFF_SIZE 2048