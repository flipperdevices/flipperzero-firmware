/* wolfevent.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>


#ifdef HAVE_WOLF_EVENT

#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#include <wolfssl/wolfcrypt/wolfevent.h>


int wolfEvent_Init(WOLF_EVENT* event, WOLF_EVENT_TYPE type, void* context)
{
    if (event == NULL) {
        return BAD_FUNC_ARG;
    }

    if (event->state == WOLF_EVENT_STATE_PENDING) {
        WOLFSSL_MSG("Event already pending!");
        return BAD_COND_E;
    }

    XMEMSET(event, 0, sizeof(WOLF_EVENT));
    event->type = type;
    event->context = context;

    return 0;
}

int wolfEvent_Poll(WOLF_EVENT* event, WOLF_EVENT_FLAG flags)
{
    int ret = BAD_COND_E;

    /* Check hardware */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (event->type >= WOLF_EVENT_TYPE_ASYNC_FIRST &&
        event->type <= WOLF_EVENT_TYPE_ASYNC_LAST)
    {
        ret = wolfAsync_EventPoll(event, flags);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    return ret;
}

int wolfEventQueue_Init(WOLF_EVENT_QUEUE* queue)
{
    int ret = 0;

    if (queue == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(queue, 0, sizeof(WOLF_EVENT_QUEUE));
#ifndef SINGLE_THREADED
    ret = wc_InitMutex(&queue->lock);
#endif
    return ret;
}


int wolfEventQueue_Push(WOLF_EVENT_QUEUE* queue, WOLF_EVENT* event)
{
    int ret;

    if (queue == NULL || event == NULL) {
        return BAD_FUNC_ARG;
    }

#ifndef SINGLE_THREADED
    if ((ret = wc_LockMutex(&queue->lock)) != 0) {
        return ret;
    }
#endif

    ret = wolfEventQueue_Add(queue, event);

#ifndef SINGLE_THREADED
    wc_UnLockMutex(&queue->lock);
#endif

    return ret;
}

int wolfEventQueue_Pop(WOLF_EVENT_QUEUE* queue, WOLF_EVENT** event)
{
    int ret = 0;

    if (queue == NULL || event == NULL) {
        return BAD_FUNC_ARG;
    }

#ifndef SINGLE_THREADED
    /* In single threaded mode "event_queue.lock" doesn't exist */
    if ((ret = wc_LockMutex(&queue->lock)) != 0) {
        return ret;
    }
#endif

    /* Pop first item off queue */
    *event = queue->head;
    ret = wolfEventQueue_Remove(queue, *event);

#ifndef SINGLE_THREADED
    wc_UnLockMutex(&queue->lock);
#endif

    return ret;
}

/* assumes queue is locked by caller */
int wolfEventQueue_Add(WOLF_EVENT_QUEUE* queue, WOLF_EVENT* event)
{
    if (queue == NULL || event == NULL) {
        return BAD_FUNC_ARG;
    }

    event->next = NULL; /* added to end */
    event->prev = NULL;
    if (queue->tail == NULL)  {
        queue->head = event;
    }
    else {
        queue->tail->next = event;
        event->prev = queue->tail;
    }
    queue->tail = event;      /* add to the end either way */
    queue->count++;

    return 0;
}

/* assumes queue is locked by caller */
int wolfEventQueue_Remove(WOLF_EVENT_QUEUE* queue, WOLF_EVENT* event)
{
    int ret = 0;

    if (queue == NULL || event == NULL) {
        return BAD_FUNC_ARG;
    }

    if (event == queue->head && event == queue->tail) {
        queue->head = NULL;
        queue->tail = NULL;
    }
    else if (event == queue->head) {
        queue->head = event->next;
        queue->head->prev = NULL;
    }
    else if (event == queue->tail) {
        queue->tail = event->prev;
        queue->tail->next = NULL;
    }
    else {
        WOLF_EVENT* next = event->next;
        WOLF_EVENT* prev = event->prev;
        next->prev = prev;
        prev->next = next;
    }
    queue->count--;

    return ret;
}

int wolfEventQueue_Poll(WOLF_EVENT_QUEUE* queue, void* context_filter,
    WOLF_EVENT** events, int maxEvents, WOLF_EVENT_FLAG flags, int* eventCount)
{
    WOLF_EVENT* event;
    int ret = 0, count = 0;

    if (queue == NULL) {
        return BAD_FUNC_ARG;
    }

#ifndef SINGLE_THREADED
    /* In single threaded mode "event_queue.lock" doesn't exist */
    if ((ret = wc_LockMutex(&queue->lock)) != 0) {
        return ret;
    }
#endif

    /* iterate event queue */
    for (event = queue->head; event != NULL; event = event->next)
    {
        /* optional filter based on context */
        if (context_filter == NULL || event->context == context_filter) {

            /* poll event */
            ret = wolfEvent_Poll(event, flags);
            if (ret < 0) break; /* exit for */

            /* If event is done then process */
            if (event->state == WOLF_EVENT_STATE_DONE) {
                /* remove from queue */
                ret = wolfEventQueue_Remove(queue, event);
                if (ret < 0) break; /* exit for */

                /* return pointer in 'events' arg */
                if (events) {
                    events[count] = event; /* return pointer */
                }
                count++;

                /* check to make sure our event list isn't full */
                if (events && count >= maxEvents) {
                    break; /* exit for */
                }
            }
        }
    }

#ifndef SINGLE_THREADED
    wc_UnLockMutex(&queue->lock);
#endif

    /* return number of properly populated events */
    if (eventCount) {
        *eventCount = count;
    }

    return ret;
}

int wolfEventQueue_Count(WOLF_EVENT_QUEUE* queue)
{
    int ret;

    if (queue == NULL) {
        return BAD_FUNC_ARG;
    }

#ifndef SINGLE_THREADED
    /* In single threaded mode "event_queue.lock" doesn't exist */
    if ((ret = wc_LockMutex(&queue->lock)) != 0) {
        return ret;
    }
#endif

    ret = queue->count;

#ifndef SINGLE_THREADED
    wc_UnLockMutex(&queue->lock);
#endif

    return ret;
}

void wolfEventQueue_Free(WOLF_EVENT_QUEUE* queue)
{
    if (queue) {
    #ifndef SINGLE_THREADED
        wc_FreeMutex(&queue->lock);
    #endif
    }
}

#endif /* HAVE_WOLF_EVENT */
