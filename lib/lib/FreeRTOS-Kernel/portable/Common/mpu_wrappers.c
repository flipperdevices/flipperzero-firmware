/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
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

/*
 * Implementation of the wrapper functions used to raise the processor privilege
 * before calling a standard FreeRTOS API function.
 */

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
 * all the API functions to use the MPU wrappers.  That should only be done when
 * task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"
#include "stream_buffer.h"
#include "mpu_prototypes.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE
/*-----------------------------------------------------------*/

#if ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    BaseType_t MPU_xTaskCreate( TaskFunction_t pvTaskCode,
                                const char * const pcName,
                                uint16_t usStackDepth,
                                void * pvParameters,
                                UBaseType_t uxPriority,
                                TaskHandle_t * pxCreatedTask ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
    TaskHandle_t MPU_xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                        const char * const pcName,
                                        const uint32_t ulStackDepth,
                                        void * const pvParameters,
                                        UBaseType_t uxPriority,
                                        StackType_t * const puxStackBuffer,
                                        StaticTask_t * const pxTaskBuffer ) /* FREERTOS_SYSTEM_CALL */
    {
        TaskHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskCreateStatic( pxTaskCode, pcName, ulStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* configSUPPORT_STATIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelete == 1 )
    void MPU_vTaskDelete( TaskHandle_t pxTaskToDelete ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTaskDelete( pxTaskToDelete );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskDelayUntil == 1 )
    BaseType_t MPU_xTaskDelayUntil( TickType_t * const pxPreviousWakeTime,
                                    TickType_t xTimeIncrement ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged, xReturn;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskDelayUntil( pxPreviousWakeTime, xTimeIncrement );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( INCLUDE_xTaskDelayUntil == 1 ) */
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskAbortDelay == 1 )
    BaseType_t MPU_xTaskAbortDelay( TaskHandle_t xTask ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskAbortDelay( xTask );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( INCLUDE_xTaskAbortDelay == 1 ) */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelay == 1 )
    void MPU_vTaskDelay( TickType_t xTicksToDelay ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTaskDelay( xTicksToDelay );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskPriorityGet == 1 )
    UBaseType_t MPU_uxTaskPriorityGet( const TaskHandle_t pxTask ) /* FREERTOS_SYSTEM_CALL */
    {
        UBaseType_t uxReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        uxReturn = uxTaskPriorityGet( pxTask );
        vPortResetPrivilege( xRunningPrivileged );

        return uxReturn;
    }
#endif /* if ( INCLUDE_uxTaskPriorityGet == 1 ) */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskPrioritySet == 1 )
    void MPU_vTaskPrioritySet( TaskHandle_t pxTask,
                               UBaseType_t uxNewPriority ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTaskPrioritySet( pxTask, uxNewPriority );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif /* if ( INCLUDE_vTaskPrioritySet == 1 ) */
/*-----------------------------------------------------------*/

#if ( INCLUDE_eTaskGetState == 1 )
    eTaskState MPU_eTaskGetState( TaskHandle_t pxTask ) /* FREERTOS_SYSTEM_CALL */
    {
        eTaskState eReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        eReturn = eTaskGetState( pxTask );
        vPortResetPrivilege( xRunningPrivileged );

        return eReturn;
    }
#endif /* if ( INCLUDE_eTaskGetState == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )
    void MPU_vTaskGetInfo( TaskHandle_t xTask,
                           TaskStatus_t * pxTaskStatus,
                           BaseType_t xGetFreeStackSpace,
                           eTaskState eState ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTaskGetInfo( xTask, pxTaskStatus, xGetFreeStackSpace, eState );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif /* if ( configUSE_TRACE_FACILITY == 1 ) */
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )
    TaskHandle_t MPU_xTaskGetIdleTaskHandle( void ) /* FREERTOS_SYSTEM_CALL */
    {
        TaskHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskGetIdleTaskHandle();
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( INCLUDE_xTaskGetIdleTaskHandle == 1 ) */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
    void MPU_vTaskSuspend( TaskHandle_t pxTaskToSuspend ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTaskSuspend( pxTaskToSuspend );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
    void MPU_vTaskResume( TaskHandle_t pxTaskToResume ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTaskResume( pxTaskToResume );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

void MPU_vTaskSuspendAll( void ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    vTaskSuspendAll();
    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskResumeAll( void ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xTaskResumeAll();
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

TickType_t MPU_xTaskGetTickCount( void ) /* FREERTOS_SYSTEM_CALL */
{
    TickType_t xReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xTaskGetTickCount();
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxTaskGetNumberOfTasks( void ) /* FREERTOS_SYSTEM_CALL */
{
    UBaseType_t uxReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    uxReturn = uxTaskGetNumberOfTasks();
    vPortResetPrivilege( xRunningPrivileged );

    return uxReturn;
}
/*-----------------------------------------------------------*/

char * MPU_pcTaskGetName( TaskHandle_t xTaskToQuery ) /* FREERTOS_SYSTEM_CALL */
{
    char * pcReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    pcReturn = pcTaskGetName( xTaskToQuery );
    vPortResetPrivilege( xRunningPrivileged );

    return pcReturn;
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetHandle == 1 )
    TaskHandle_t MPU_xTaskGetHandle( const char * pcNameToQuery ) /* FREERTOS_SYSTEM_CALL */
    {
        TaskHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskGetHandle( pcNameToQuery );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( INCLUDE_xTaskGetHandle == 1 ) */
/*-----------------------------------------------------------*/

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    void MPU_vTaskList( char * pcWriteBuffer ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTaskList( pcWriteBuffer );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    void MPU_vTaskGetRunTimeStats( char * pcWriteBuffer ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTaskGetRunTimeStats( pcWriteBuffer );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif
/*-----------------------------------------------------------*/

#if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( INCLUDE_xTaskGetIdleTaskHandle == 1 ) )
    configRUN_TIME_COUNTER_TYPE MPU_ulTaskGetIdleRunTimePercent( void ) /* FREERTOS_SYSTEM_CALL */
    {
        configRUN_TIME_COUNTER_TYPE xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = ulTaskGetIdleRunTimePercent();
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( INCLUDE_xTaskGetIdleTaskHandle == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( INCLUDE_xTaskGetIdleTaskHandle == 1 ) )
    configRUN_TIME_COUNTER_TYPE MPU_ulTaskGetIdleRunTimeCounter( void ) /* FREERTOS_SYSTEM_CALL */
    {
        configRUN_TIME_COUNTER_TYPE xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = ulTaskGetIdleRunTimeCounter();
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( INCLUDE_xTaskGetIdleTaskHandle == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    void MPU_vTaskSetApplicationTaskTag( TaskHandle_t xTask,
                                         TaskHookFunction_t pxTagValue ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTaskSetApplicationTaskTag( xTask, pxTagValue );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif /* if ( configUSE_APPLICATION_TASK_TAG == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    TaskHookFunction_t MPU_xTaskGetApplicationTaskTag( TaskHandle_t xTask ) /* FREERTOS_SYSTEM_CALL */
    {
        TaskHookFunction_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskGetApplicationTaskTag( xTask );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_APPLICATION_TASK_TAG == 1 ) */
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
    void MPU_vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet,
                                                BaseType_t xIndex,
                                                void * pvValue ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTaskSetThreadLocalStoragePointer( xTaskToSet, xIndex, pvValue );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif /* if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 ) */
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
    void * MPU_pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery,
                                                   BaseType_t xIndex ) /* FREERTOS_SYSTEM_CALL */
    {
        void * pvReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        pvReturn = pvTaskGetThreadLocalStoragePointer( xTaskToQuery, xIndex );
        vPortResetPrivilege( xRunningPrivileged );

        return pvReturn;
    }
#endif /* if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    BaseType_t MPU_xTaskCallApplicationTaskHook( TaskHandle_t xTask,
                                                 void * pvParameter ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskCallApplicationTaskHook( xTask, pvParameter );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_APPLICATION_TASK_TAG == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )
    UBaseType_t MPU_uxTaskGetSystemState( TaskStatus_t * pxTaskStatusArray,
                                          UBaseType_t uxArraySize,
                                          configRUN_TIME_COUNTER_TYPE * pulTotalRunTime ) /* FREERTOS_SYSTEM_CALL */
    {
        UBaseType_t uxReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        uxReturn = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, pulTotalRunTime );
        vPortResetPrivilege( xRunningPrivileged );

        return uxReturn;
    }
#endif /* if ( configUSE_TRACE_FACILITY == 1 ) */
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskCatchUpTicks( TickType_t xTicksToCatchUp ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xTaskCatchUpTicks( xTicksToCatchUp );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
    UBaseType_t MPU_uxTaskGetStackHighWaterMark( TaskHandle_t xTask ) /* FREERTOS_SYSTEM_CALL */
    {
        UBaseType_t uxReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        uxReturn = uxTaskGetStackHighWaterMark( xTask );
        vPortResetPrivilege( xRunningPrivileged );

        return uxReturn;
    }
#endif /* if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) */
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskGetStackHighWaterMark2 == 1 )
    configSTACK_DEPTH_TYPE MPU_uxTaskGetStackHighWaterMark2( TaskHandle_t xTask ) /* FREERTOS_SYSTEM_CALL */
    {
        configSTACK_DEPTH_TYPE uxReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        uxReturn = uxTaskGetStackHighWaterMark2( xTask );
        vPortResetPrivilege( xRunningPrivileged );

        return uxReturn;
    }
#endif /* if ( INCLUDE_uxTaskGetStackHighWaterMark2 == 1 ) */
/*-----------------------------------------------------------*/

#if ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) )
    TaskHandle_t MPU_xTaskGetCurrentTaskHandle( void ) /* FREERTOS_SYSTEM_CALL */
    {
        TaskHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskGetCurrentTaskHandle();
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetSchedulerState == 1 )
    BaseType_t MPU_xTaskGetSchedulerState( void ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskGetSchedulerState();
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( INCLUDE_xTaskGetSchedulerState == 1 ) */
/*-----------------------------------------------------------*/

void MPU_vTaskSetTimeOutState( TimeOut_t * const pxTimeOut ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    vTaskSetTimeOutState( pxTimeOut );
    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut,
                                     TickType_t * const pxTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xTaskCheckForTimeOut( pxTimeOut, pxTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

#if ( configUSE_TASK_NOTIFICATIONS == 1 )
    BaseType_t MPU_xTaskGenericNotify( TaskHandle_t xTaskToNotify,
                                       UBaseType_t uxIndexToNotify,
                                       uint32_t ulValue,
                                       eNotifyAction eAction,
                                       uint32_t * pulPreviousNotificationValue ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskGenericNotify( xTaskToNotify, uxIndexToNotify, ulValue, eAction, pulPreviousNotificationValue );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_TASK_NOTIFICATIONS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TASK_NOTIFICATIONS == 1 )
    BaseType_t MPU_xTaskGenericNotifyWait( UBaseType_t uxIndexToWaitOn,
                                           uint32_t ulBitsToClearOnEntry,
                                           uint32_t ulBitsToClearOnExit,
                                           uint32_t * pulNotificationValue,
                                           TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskGenericNotifyWait( uxIndexToWaitOn, ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_TASK_NOTIFICATIONS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TASK_NOTIFICATIONS == 1 )
    uint32_t MPU_ulTaskGenericNotifyTake( UBaseType_t uxIndexToWaitOn,
                                          BaseType_t xClearCountOnExit,
                                          TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
    {
        uint32_t ulReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        ulReturn = ulTaskGenericNotifyTake( uxIndexToWaitOn, xClearCountOnExit, xTicksToWait );
        vPortResetPrivilege( xRunningPrivileged );

        return ulReturn;
    }
#endif /* if ( configUSE_TASK_NOTIFICATIONS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TASK_NOTIFICATIONS == 1 )
    BaseType_t MPU_xTaskGenericNotifyStateClear( TaskHandle_t xTask,
                                                 UBaseType_t uxIndexToClear ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTaskGenericNotifyStateClear( xTask, uxIndexToClear );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_TASK_NOTIFICATIONS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TASK_NOTIFICATIONS == 1 )
    uint32_t MPU_ulTaskGenericNotifyValueClear( TaskHandle_t xTask,
                                                UBaseType_t uxIndexToClear,
                                                uint32_t ulBitsToClear ) /* FREERTOS_SYSTEM_CALL */
    {
        uint32_t ulReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        ulReturn = ulTaskGenericNotifyValueClear( xTask, uxIndexToClear, ulBitsToClear );
        vPortResetPrivilege( xRunningPrivileged );

        return ulReturn;
    }
#endif /* if ( configUSE_TASK_NOTIFICATIONS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    QueueHandle_t MPU_xQueueGenericCreate( UBaseType_t uxQueueLength,
                                           UBaseType_t uxItemSize,
                                           uint8_t ucQueueType ) /* FREERTOS_SYSTEM_CALL */
    {
        QueueHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueGenericCreate( uxQueueLength, uxItemSize, ucQueueType );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) */
/*-----------------------------------------------------------*/

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
    QueueHandle_t MPU_xQueueGenericCreateStatic( const UBaseType_t uxQueueLength,
                                                 const UBaseType_t uxItemSize,
                                                 uint8_t * pucQueueStorage,
                                                 StaticQueue_t * pxStaticQueue,
                                                 const uint8_t ucQueueType ) /* FREERTOS_SYSTEM_CALL */
    {
        QueueHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueGenericCreateStatic( uxQueueLength, uxItemSize, pucQueueStorage, pxStaticQueue, ucQueueType );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configSUPPORT_STATIC_ALLOCATION == 1 ) */
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericReset( QueueHandle_t pxQueue,
                                   BaseType_t xNewQueue ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xQueueGenericReset( pxQueue, xNewQueue );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericSend( QueueHandle_t xQueue,
                                  const void * const pvItemToQueue,
                                  TickType_t xTicksToWait,
                                  BaseType_t xCopyPosition ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xQueueGenericSend( xQueue, pvItemToQueue, xTicksToWait, xCopyPosition );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxQueueMessagesWaiting( const QueueHandle_t pxQueue ) /* FREERTOS_SYSTEM_CALL */
{
    UBaseType_t uxReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    uxReturn = uxQueueMessagesWaiting( pxQueue );
    vPortResetPrivilege( xRunningPrivileged );

    return uxReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxQueueSpacesAvailable( const QueueHandle_t xQueue ) /* FREERTOS_SYSTEM_CALL */
{
    UBaseType_t uxReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    uxReturn = uxQueueSpacesAvailable( xQueue );
    vPortResetPrivilege( xRunningPrivileged );

    return uxReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueReceive( QueueHandle_t pxQueue,
                              void * const pvBuffer,
                              TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xQueueReceive( pxQueue, pvBuffer, xTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueuePeek( QueueHandle_t xQueue,
                           void * const pvBuffer,
                           TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xQueuePeek( xQueue, pvBuffer, xTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueSemaphoreTake( QueueHandle_t xQueue,
                                    TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xQueueSemaphoreTake( xQueue, xTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

#if ( ( configUSE_MUTEXES == 1 ) && ( INCLUDE_xSemaphoreGetMutexHolder == 1 ) )
    TaskHandle_t MPU_xQueueGetMutexHolder( QueueHandle_t xSemaphore ) /* FREERTOS_SYSTEM_CALL */
    {
        void * xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueGetMutexHolder( xSemaphore );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( configUSE_MUTEXES == 1 ) && ( INCLUDE_xSemaphoreGetMutexHolder == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    QueueHandle_t MPU_xQueueCreateMutex( const uint8_t ucQueueType ) /* FREERTOS_SYSTEM_CALL */
    {
        QueueHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueCreateMutex( ucQueueType );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )
    QueueHandle_t MPU_xQueueCreateMutexStatic( const uint8_t ucQueueType,
                                               StaticQueue_t * pxStaticQueue ) /* FREERTOS_SYSTEM_CALL */
    {
        QueueHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueCreateMutexStatic( ucQueueType, pxStaticQueue );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    QueueHandle_t MPU_xQueueCreateCountingSemaphore( UBaseType_t uxCountValue,
                                                     UBaseType_t uxInitialCount ) /* FREERTOS_SYSTEM_CALL */
    {
        QueueHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueCreateCountingSemaphore( uxCountValue, uxInitialCount );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )

    QueueHandle_t MPU_xQueueCreateCountingSemaphoreStatic( const UBaseType_t uxMaxCount,
                                                           const UBaseType_t uxInitialCount,
                                                           StaticQueue_t * pxStaticQueue ) /* FREERTOS_SYSTEM_CALL */
    {
        QueueHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueCreateCountingSemaphoreStatic( uxMaxCount, uxInitialCount, pxStaticQueue );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( configUSE_RECURSIVE_MUTEXES == 1 )
    BaseType_t MPU_xQueueTakeMutexRecursive( QueueHandle_t xMutex,
                                             TickType_t xBlockTime ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueTakeMutexRecursive( xMutex, xBlockTime );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_RECURSIVE_MUTEXES == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_RECURSIVE_MUTEXES == 1 )
    BaseType_t MPU_xQueueGiveMutexRecursive( QueueHandle_t xMutex ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueGiveMutexRecursive( xMutex );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_RECURSIVE_MUTEXES == 1 ) */
/*-----------------------------------------------------------*/

#if ( ( configUSE_QUEUE_SETS == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
    QueueSetHandle_t MPU_xQueueCreateSet( UBaseType_t uxEventQueueLength ) /* FREERTOS_SYSTEM_CALL */
    {
        QueueSetHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueCreateSet( uxEventQueueLength );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( configUSE_QUEUE_SETS == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    QueueSetMemberHandle_t MPU_xQueueSelectFromSet( QueueSetHandle_t xQueueSet,
                                                    TickType_t xBlockTimeTicks ) /* FREERTOS_SYSTEM_CALL */
    {
        QueueSetMemberHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueSelectFromSet( xQueueSet, xBlockTimeTicks );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_QUEUE_SETS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    BaseType_t MPU_xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore,
                                   QueueSetHandle_t xQueueSet ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueAddToSet( xQueueOrSemaphore, xQueueSet );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_QUEUE_SETS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
    BaseType_t MPU_xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore,
                                        QueueSetHandle_t xQueueSet ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xQueueRemoveFromSet( xQueueOrSemaphore, xQueueSet );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_QUEUE_SETS == 1 ) */
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
    void MPU_vQueueAddToRegistry( QueueHandle_t xQueue,
                                  const char * pcName ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vQueueAddToRegistry( xQueue, pcName );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif /* if configQUEUE_REGISTRY_SIZE > 0 */
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
    void MPU_vQueueUnregisterQueue( QueueHandle_t xQueue ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vQueueUnregisterQueue( xQueue );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif /* if configQUEUE_REGISTRY_SIZE > 0 */
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
    const char * MPU_pcQueueGetName( QueueHandle_t xQueue ) /* FREERTOS_SYSTEM_CALL */
    {
        const char * pcReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        pcReturn = pcQueueGetName( xQueue );
        vPortResetPrivilege( xRunningPrivileged );

        return pcReturn;
    }
#endif /* if configQUEUE_REGISTRY_SIZE > 0 */
/*-----------------------------------------------------------*/

void MPU_vQueueDelete( QueueHandle_t xQueue ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    vQueueDelete( xQueue );
    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

#if ( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) )
    TimerHandle_t MPU_xTimerCreate( const char * const pcTimerName,
                                    const TickType_t xTimerPeriodInTicks,
                                    const UBaseType_t uxAutoReload,
                                    void * const pvTimerID,
                                    TimerCallbackFunction_t pxCallbackFunction ) /* FREERTOS_SYSTEM_CALL */
    {
        TimerHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTimerCreate( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) )
    TimerHandle_t MPU_xTimerCreateStatic( const char * const pcTimerName,
                                          const TickType_t xTimerPeriodInTicks,
                                          const UBaseType_t uxAutoReload,
                                          void * const pvTimerID,
                                          TimerCallbackFunction_t pxCallbackFunction,
                                          StaticTimer_t * pxTimerBuffer ) /* FREERTOS_SYSTEM_CALL */
    {
        TimerHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTimerCreateStatic( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction, pxTimerBuffer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )
    void * MPU_pvTimerGetTimerID( const TimerHandle_t xTimer ) /* FREERTOS_SYSTEM_CALL */
    {
        void * pvReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        pvReturn = pvTimerGetTimerID( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return pvReturn;
    }
#endif /* if ( configUSE_TIMERS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )
    void MPU_vTimerSetTimerID( TimerHandle_t xTimer,
                               void * pvNewID ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTimerSetTimerID( xTimer, pvNewID );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif /* if ( configUSE_TIMERS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )
    BaseType_t MPU_xTimerIsTimerActive( TimerHandle_t xTimer ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTimerIsTimerActive( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_TIMERS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )
    TaskHandle_t MPU_xTimerGetTimerDaemonTaskHandle( void ) /* FREERTOS_SYSTEM_CALL */
    {
        TaskHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTimerGetTimerDaemonTaskHandle();
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_TIMERS == 1 ) */
/*-----------------------------------------------------------*/

#if ( ( INCLUDE_xTimerPendFunctionCall == 1 ) && ( configUSE_TIMERS == 1 ) )
    BaseType_t MPU_xTimerPendFunctionCall( PendedFunction_t xFunctionToPend,
                                           void * pvParameter1,
                                           uint32_t ulParameter2,
                                           TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn, xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTimerPendFunctionCall( xFunctionToPend, pvParameter1, ulParameter2, xTicksToWait );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( ( INCLUDE_xTimerPendFunctionCall == 1 ) && ( configUSE_TIMERS == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )
    void MPU_vTimerSetReloadMode( TimerHandle_t xTimer,
                                  const UBaseType_t uxAutoReload ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        vTimerSetReloadMode( xTimer, uxAutoReload );
        vPortResetPrivilege( xRunningPrivileged );
    }
#endif /* if ( configUSE_TIMERS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )
    UBaseType_t MPU_uxTimerGetReloadMode( TimerHandle_t xTimer )
    {
        UBaseType_t uxReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        uxReturn = uxTimerGetReloadMode( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return uxReturn;
    }
#endif /* if ( configUSE_TIMERS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )
    const char * MPU_pcTimerGetName( TimerHandle_t xTimer ) /* FREERTOS_SYSTEM_CALL */
    {
        const char * pcReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        pcReturn = pcTimerGetName( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return pcReturn;
    }
#endif /* if ( configUSE_TIMERS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )
    TickType_t MPU_xTimerGetPeriod( TimerHandle_t xTimer ) /* FREERTOS_SYSTEM_CALL */
    {
        TickType_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTimerGetPeriod( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_TIMERS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )
    TickType_t MPU_xTimerGetExpiryTime( TimerHandle_t xTimer ) /* FREERTOS_SYSTEM_CALL */
    {
        TickType_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTimerGetExpiryTime( xTimer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_TIMERS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )
    BaseType_t MPU_xTimerGenericCommand( TimerHandle_t xTimer,
                                         const BaseType_t xCommandID,
                                         const TickType_t xOptionalValue,
                                         BaseType_t * const pxHigherPriorityTaskWoken,
                                         const TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
    {
        BaseType_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xTimerGenericCommand( xTimer, xCommandID, xOptionalValue, pxHigherPriorityTaskWoken, xTicksToWait );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configUSE_TIMERS == 1 ) */
/*-----------------------------------------------------------*/

#if ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    EventGroupHandle_t MPU_xEventGroupCreate( void ) /* FREERTOS_SYSTEM_CALL */
    {
        EventGroupHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xEventGroupCreate();
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) */
/*-----------------------------------------------------------*/

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
    EventGroupHandle_t MPU_xEventGroupCreateStatic( StaticEventGroup_t * pxEventGroupBuffer ) /* FREERTOS_SYSTEM_CALL */
    {
        EventGroupHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xEventGroupCreateStatic( pxEventGroupBuffer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* if ( configSUPPORT_STATIC_ALLOCATION == 1 ) */
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupWaitBits( EventGroupHandle_t xEventGroup,
                                     const EventBits_t uxBitsToWaitFor,
                                     const BaseType_t xClearOnExit,
                                     const BaseType_t xWaitForAllBits,
                                     TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
    EventBits_t xReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xEventGroupWaitBits( xEventGroup, uxBitsToWaitFor, xClearOnExit, xWaitForAllBits, xTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupClearBits( EventGroupHandle_t xEventGroup,
                                      const EventBits_t uxBitsToClear ) /* FREERTOS_SYSTEM_CALL */
{
    EventBits_t xReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xEventGroupClearBits( xEventGroup, uxBitsToClear );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupSetBits( EventGroupHandle_t xEventGroup,
                                    const EventBits_t uxBitsToSet ) /* FREERTOS_SYSTEM_CALL */
{
    EventBits_t xReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xEventGroupSetBits( xEventGroup, uxBitsToSet );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupSync( EventGroupHandle_t xEventGroup,
                                 const EventBits_t uxBitsToSet,
                                 const EventBits_t uxBitsToWaitFor,
                                 TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
    EventBits_t xReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xEventGroupSync( xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

void MPU_vEventGroupDelete( EventGroupHandle_t xEventGroup ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    vEventGroupDelete( xEventGroup );
    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferSend( StreamBufferHandle_t xStreamBuffer,
                              const void * pvTxData,
                              size_t xDataLengthBytes,
                              TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
    size_t xReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xStreamBufferSend( xStreamBuffer, pvTxData, xDataLengthBytes, xTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferNextMessageLengthBytes( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
    size_t xReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xStreamBufferNextMessageLengthBytes( xStreamBuffer );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferReceive( StreamBufferHandle_t xStreamBuffer,
                                 void * pvRxData,
                                 size_t xBufferLengthBytes,
                                 TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
    size_t xReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xStreamBufferReceive( xStreamBuffer, pvRxData, xBufferLengthBytes, xTicksToWait );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

void MPU_vStreamBufferDelete( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    vStreamBufferDelete( xStreamBuffer );
    vPortResetPrivilege( xRunningPrivileged );
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferIsFull( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xStreamBufferIsFull( xStreamBuffer );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferIsEmpty( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xStreamBufferIsEmpty( xStreamBuffer );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferReset( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xStreamBufferReset( xStreamBuffer );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferSpacesAvailable( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
    size_t xReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xStreamBufferSpacesAvailable( xStreamBuffer );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferBytesAvailable( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
    size_t xReturn;
    BaseType_t xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xStreamBufferBytesAvailable( xStreamBuffer );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferSetTriggerLevel( StreamBufferHandle_t xStreamBuffer,
                                             size_t xTriggerLevel ) /* FREERTOS_SYSTEM_CALL */
{
    BaseType_t xReturn, xRunningPrivileged;

    xPortRaisePrivilege( xRunningPrivileged );
    xReturn = xStreamBufferSetTriggerLevel( xStreamBuffer, xTriggerLevel );
    vPortResetPrivilege( xRunningPrivileged );

    return xReturn;
}
/*-----------------------------------------------------------*/

#if ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    StreamBufferHandle_t MPU_xStreamBufferGenericCreate( size_t xBufferSizeBytes,
                                                         size_t xTriggerLevelBytes,
                                                         BaseType_t xIsMessageBuffer ) /* FREERTOS_SYSTEM_CALL */
    {
        StreamBufferHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xStreamBufferGenericCreate( xBufferSizeBytes, xTriggerLevelBytes, xIsMessageBuffer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
    StreamBufferHandle_t MPU_xStreamBufferGenericCreateStatic( size_t xBufferSizeBytes,
                                                               size_t xTriggerLevelBytes,
                                                               BaseType_t xIsMessageBuffer,
                                                               uint8_t * const pucStreamBufferStorageArea,
                                                               StaticStreamBuffer_t * const pxStaticStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
    {
        StreamBufferHandle_t xReturn;
        BaseType_t xRunningPrivileged;

        xPortRaisePrivilege( xRunningPrivileged );
        xReturn = xStreamBufferGenericCreateStatic( xBufferSizeBytes, xTriggerLevelBytes, xIsMessageBuffer, pucStreamBufferStorageArea, pxStaticStreamBuffer );
        vPortResetPrivilege( xRunningPrivileged );

        return xReturn;
    }
#endif /* configSUPPORT_STATIC_ALLOCATION */
/*-----------------------------------------------------------*/


/* Functions that the application writer wants to execute in privileged mode
 * can be defined in application_defined_privileged_functions.h.  The functions
 * must take the same format as those above whereby the privilege state on exit
 * equals the privilege state on entry.  For example:
 *
 * void MPU_FunctionName( [parameters ] ) FREERTOS_SYSTEM_CALL;
 * void MPU_FunctionName( [parameters ] )
 * {
 * BaseType_t xRunningPrivileged;
 *
 * xPortRaisePrivilege( xRunningPrivileged );
 * FunctionName( [parameters ] );
 * vPortResetPrivilege( xRunningPrivileged );
 * }
 */

#if configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS == 1
    #include "application_defined_privileged_functions.h"
#endif
