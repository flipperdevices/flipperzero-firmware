/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
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
 */

/*
 * This file contains the implementation of APIs which are defined in
 * os_wrapper/mutex.h by TF-M(tag: TF-Mv1.1). The implementation is based
 * on FreeRTOS mutex type semaphore.
 */

#include "os_wrapper/mutex.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "mpu_wrappers.h"

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	/*
	 * In the static allocation, the RAM is required to hold the semaphore's
	 * state.
	 */
	StaticSemaphore_t xSecureMutexBuffer;
#endif

void * os_wrapper_mutex_create( void )
{
SemaphoreHandle_t xMutexHandle = NULL;

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	xMutexHandle = xSemaphoreCreateMutex();
#elif( configSUPPORT_STATIC_ALLOCATION == 1 )
	xMutexHandle = xSemaphoreCreateMutexStatic( &xSecureMutexBuffer );
#endif
	return ( void * ) xMutexHandle;
}
/*-----------------------------------------------------------*/

uint32_t os_wrapper_mutex_acquire( void * handle, uint32_t timeout )
{
BaseType_t xRet;

	if( ! handle )
		return OS_WRAPPER_ERROR;

	xRet = xSemaphoreTake( ( SemaphoreHandle_t ) handle,
						   ( timeout == OS_WRAPPER_WAIT_FOREVER ) ?
                           portMAX_DELAY : ( TickType_t ) timeout );

	if( xRet != pdPASS )
		return OS_WRAPPER_ERROR;
	else
		return OS_WRAPPER_SUCCESS;
}
/*-----------------------------------------------------------*/

uint32_t os_wrapper_mutex_release( void * handle )
{
BaseType_t xRet;

	if( !handle )
		return OS_WRAPPER_ERROR;

	xRet = xSemaphoreGive( ( SemaphoreHandle_t ) handle );

	if( xRet != pdPASS )
		return OS_WRAPPER_ERROR;
	else
		return OS_WRAPPER_SUCCESS;
}
/*-----------------------------------------------------------*/

uint32_t os_wrapper_mutex_delete( void * handle )
{
	vSemaphoreDelete( ( SemaphoreHandle_t ) handle );

	return OS_WRAPPER_SUCCESS;
}
/*-----------------------------------------------------------*/
