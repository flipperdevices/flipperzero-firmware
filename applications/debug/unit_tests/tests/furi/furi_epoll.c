#include "../test.h"
#include "core/epoll.h"
#include "core/kernel.h"
#include "core/log.h"
#include "core/message_queue.h"
#include "core/thread.h"
#include "furi_hal_random.h"
#include <furi.h>
#include <stdbool.h>
#include <stdint.h>

#define TAG "TestFuriEpoll"

#define EPOLL_EVENT_COUNT (256u)

typedef struct {
    FuriMessageQueue* mq;

    FuriEpoll* producer_epoll;
    uint32_t producer_counter;

    FuriEpoll* consumer_epoll;
    uint32_t consumer_counter;
} TestFuriData;

bool test_furi_epoll_producer_mq_callback(FuriMessageQueue* queue, void* context) {
    furi_check(context);

    TestFuriData* data = context;
    furi_check(data->mq == queue, "Invalid queue");

    FURI_LOG_I(
        TAG, "producer_mq_callback: %lu %lu", data->producer_counter, data->consumer_counter);

    if(data->producer_counter == EPOLL_EVENT_COUNT) {
        furi_epoll_stop(data->producer_epoll);
        return false;
    }

    data->producer_counter++;
    furi_check(
        furi_message_queue_put(data->mq, &data->producer_counter, 0) == FuriStatusOk,
        "furi_message_queue_put failed");
    furi_delay_us(furi_hal_random_get() % 1000);

    return true;
}

int32_t test_furi_epoll_producer(void* p) {
    furi_check(p);

    FURI_LOG_I(TAG, "producer start");

    TestFuriData* data = p;

    data->producer_epoll = furi_epoll_alloc();
    furi_epoll_message_queue_add(
        data->producer_epoll,
        data->mq,
        FuriEpollEventOut,
        test_furi_epoll_producer_mq_callback,
        data);

    furi_epoll_poll(data->producer_epoll);

    furi_epoll_message_queue_remove(data->producer_epoll, data->mq);
    furi_epoll_free(data->producer_epoll);

    FURI_LOG_I(TAG, "producer end");

    return 0;
}

bool test_furi_epoll_consumer_mq_callback(FuriMessageQueue* queue, void* context) {
    furi_check(context);

    TestFuriData* data = context;
    furi_check(data->mq == queue);

    furi_delay_us(furi_hal_random_get() % 1000);
    furi_check(furi_message_queue_get(data->mq, &data->consumer_counter, 0) == FuriStatusOk);

    FURI_LOG_I(
        TAG, "consumer_mq_callback: %lu %lu", data->producer_counter, data->consumer_counter);

    if(data->consumer_counter == EPOLL_EVENT_COUNT) {
        furi_epoll_stop(data->consumer_epoll);
        return false;
    }

    return true;
}

int32_t test_furi_epoll_consumer(void* p) {
    furi_check(p);

    FURI_LOG_I(TAG, "consumer start");

    TestFuriData* data = p;

    data->consumer_epoll = furi_epoll_alloc();
    furi_epoll_message_queue_add(
        data->consumer_epoll,
        data->mq,
        FuriEpollEventIn,
        test_furi_epoll_consumer_mq_callback,
        data);

    furi_epoll_poll(data->consumer_epoll);

    furi_epoll_message_queue_remove(data->consumer_epoll, data->mq);
    furi_epoll_free(data->consumer_epoll);

    FURI_LOG_I(TAG, "consumer end");

    return 0;
}

void test_furi_epoll(void) {
    TestFuriData data = {};

    data.mq = furi_message_queue_alloc(16, sizeof(uint32_t));

    FuriThread* producer_thread = furi_thread_alloc();
    furi_thread_set_name(producer_thread, "producer_thread");
    furi_thread_set_stack_size(producer_thread, 1 * 1024);
    furi_thread_set_callback(producer_thread, test_furi_epoll_producer);
    furi_thread_set_context(producer_thread, &data);
    furi_thread_start(producer_thread);

    FuriThread* consumer_thread = furi_thread_alloc();
    furi_thread_set_name(consumer_thread, "consumer_thread");
    furi_thread_set_stack_size(consumer_thread, 1 * 1024);
    furi_thread_set_callback(consumer_thread, test_furi_epoll_consumer);
    furi_thread_set_context(consumer_thread, &data);
    furi_thread_start(consumer_thread);

    // Wait for thread to complete their tasks
    furi_thread_join(producer_thread);
    furi_thread_join(consumer_thread);

    // The test itself
    mu_assert_int_eq(data.producer_counter, data.consumer_counter);

    // Release memory
    furi_thread_free(consumer_thread);
    furi_thread_free(producer_thread);
    furi_message_queue_free(data.mq);
}
