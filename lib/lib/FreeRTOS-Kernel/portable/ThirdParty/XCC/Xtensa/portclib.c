/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2015-2019 Cadence Design Systems, Inc.
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#include "FreeRTOS.h"

#if XT_USE_THREAD_SAFE_CLIB

#if XSHAL_CLIB == XTHAL_CLIB_XCLIB

#include <errno.h>
#include <sys/reent.h>

#include "semphr.h"

typedef SemaphoreHandle_t       _Rmtx;

//-----------------------------------------------------------------------------
//  Override this and set to nonzero to enable locking.
//-----------------------------------------------------------------------------
int32_t _xclib_use_mt = 1;


//-----------------------------------------------------------------------------
//  Init lock.
//-----------------------------------------------------------------------------
void
_Mtxinit(_Rmtx * mtx)
{
    *mtx = xSemaphoreCreateRecursiveMutex();
}

//-----------------------------------------------------------------------------
//  Destroy lock.
//-----------------------------------------------------------------------------
void
_Mtxdst(_Rmtx * mtx)
{
    if ((mtx != NULL) && (*mtx != NULL)) {
        vSemaphoreDelete(*mtx);
    }
}

//-----------------------------------------------------------------------------
//  Lock.
//-----------------------------------------------------------------------------
void
_Mtxlock(_Rmtx * mtx)
{
    if ((mtx != NULL) && (*mtx != NULL)) {
        xSemaphoreTakeRecursive(*mtx, portMAX_DELAY);
    }
}

//-----------------------------------------------------------------------------
//  Unlock.
//-----------------------------------------------------------------------------
void
_Mtxunlock(_Rmtx * mtx)
{
    if ((mtx != NULL) && (*mtx != NULL)) {
        xSemaphoreGiveRecursive(*mtx);
    }
}

//-----------------------------------------------------------------------------
//  Called by malloc() to allocate blocks of memory from the heap.
//-----------------------------------------------------------------------------
void *
_sbrk_r (struct _reent * reent, int32_t incr)
{
    extern char _end;
    extern char _heap_sentry;
    static char * _heap_sentry_ptr = &_heap_sentry;
    static char * heap_ptr;
    char * base;

    if (!heap_ptr)
        heap_ptr = (char *) &_end;

    base = heap_ptr;
    if (heap_ptr + incr >= _heap_sentry_ptr) {
        reent->_errno = ENOMEM;
        return (char *) -1;
    }

    heap_ptr += incr;
    return base;
}

//-----------------------------------------------------------------------------
//  Global initialization for C library.
//-----------------------------------------------------------------------------
void
vPortClibInit(void)
{
}

//-----------------------------------------------------------------------------
//  Per-thread cleanup stub provided for linking, does nothing.
//-----------------------------------------------------------------------------
void
_reclaim_reent(void * ptr)
{
}

#endif /* XSHAL_CLIB == XTHAL_CLIB_XCLIB */

#if XSHAL_CLIB == XTHAL_CLIB_NEWLIB

#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semphr.h"

static SemaphoreHandle_t xClibMutex;
static uint32_t  ulClibInitDone = 0;

//-----------------------------------------------------------------------------
//  Get C library lock.
//-----------------------------------------------------------------------------
void
__malloc_lock(struct _reent * ptr)
{
    if (!ulClibInitDone)
        return;

    xSemaphoreTakeRecursive(xClibMutex, portMAX_DELAY);
}

//-----------------------------------------------------------------------------
//  Release C library lock.
//-----------------------------------------------------------------------------
void
__malloc_unlock(struct _reent * ptr)
{
    if (!ulClibInitDone)
        return;

    xSemaphoreGiveRecursive(xClibMutex);
}

//-----------------------------------------------------------------------------
//  Lock for environment. Since we have only one global lock we can just call
//  the malloc() lock function.
//-----------------------------------------------------------------------------
void
__env_lock(struct _reent * ptr)
{
    __malloc_lock(ptr);
}


//-----------------------------------------------------------------------------
//  Unlock environment.
//-----------------------------------------------------------------------------
void
__env_unlock(struct _reent * ptr)
{
    __malloc_unlock(ptr);
}

//-----------------------------------------------------------------------------
//  Called by malloc() to allocate blocks of memory from the heap.
//-----------------------------------------------------------------------------
void *
_sbrk_r (struct _reent * reent, int32_t incr)
{
    extern char _end;
    extern char _heap_sentry;
    static char * _heap_sentry_ptr = &_heap_sentry;
    static char * heap_ptr;
    char * base;

    if (!heap_ptr)
        heap_ptr = (char *) &_end;

    base = heap_ptr;
    if (heap_ptr + incr >= _heap_sentry_ptr) {
        reent->_errno = ENOMEM;
        return (char *) -1;
    }

    heap_ptr += incr;
    return base;
}

//-----------------------------------------------------------------------------
//  Global initialization for C library.
//-----------------------------------------------------------------------------
void
vPortClibInit(void)
{
    configASSERT(!ulClibInitDone);

    xClibMutex = xSemaphoreCreateRecursiveMutex();
    ulClibInitDone  = 1;
}

#endif /* XSHAL_CLIB == XTHAL_CLIB_NEWLIB */

#endif /* XT_USE_THREAD_SAFE_CLIB */
