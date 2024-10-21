#include "../test.h"
#include <furi.h>
#include <furi_hal.h>

#include <FreeRTOS.h>
#include <task.h>

#define TAG "TestFuriEventLoop"

#define MESSAGE_COUNT    (256UL)
#define EVENT_FLAG_COUNT (23UL)
#define PRIMITIVE_COUNT  (2UL)
#define RUN_COUNT        (2UL)

typedef struct {
    FuriEventLoop* event_loop;
    uint32_t message_count;
    uint32_t event_flag_count;
    uint32_t primitives_tested;
} TestFuriEventLoopThread;

typedef struct {
    FuriMessageQueue* message_queue;
    FuriEventFlag* event_flag;

    TestFuriEventLoopThread producer;
    TestFuriEventLoopThread consumer;
} TestFuriEventLoopData;

static void test_furi_event_loop_pending_callback(void* context) {
    furi_check(context);

    TestFuriEventLoopThread* test_thread = context;
    furi_check(test_thread->primitives_tested < PRIMITIVE_COUNT);

    test_thread->primitives_tested++;
    FURI_LOG_I(TAG, "primitives tested: %lu", test_thread->primitives_tested);

    if(test_thread->primitives_tested == PRIMITIVE_COUNT) {
        furi_event_loop_stop(test_thread->event_loop);
    }
}

static void test_furi_event_loop_thread_init(TestFuriEventLoopThread* test_thread) {
    test_thread->event_loop = furi_event_loop_alloc();
    test_thread->message_count = 0;
    test_thread->event_flag_count = 0;
    test_thread->primitives_tested = 0;
}

static void test_furi_event_loop_thread_run_and_cleanup(TestFuriEventLoopThread* test_thread) {
    furi_event_loop_run(test_thread->event_loop);
    // 2 EventLoop index, 0xFFFFFFFF - all possible flags, emulate uncleared flags
    xTaskNotifyIndexed(xTaskGetCurrentTaskHandle(), 2, 0xFFFFFFFF, eSetBits);
    furi_event_loop_free(test_thread->event_loop);
}

static bool test_furi_event_loop_producer_message_queue_callback(
    FuriEventLoopObject* object,
    void* context) {
    furi_check(context);

    TestFuriEventLoopData* data = context;
    furi_check(data->message_queue == object);

    FURI_LOG_I(
        TAG,
        "producer MessageQueue: %lu %lu",
        data->producer.message_count,
        data->consumer.message_count);

    if(data->producer.message_count == MESSAGE_COUNT / 2) {
        furi_event_loop_unsubscribe(data->producer.event_loop, data->message_queue);
        furi_event_loop_subscribe_message_queue(
            data->producer.event_loop,
            data->message_queue,
            FuriEventLoopEventOut,
            test_furi_event_loop_producer_message_queue_callback,
            data);

    } else if(data->producer.message_count == MESSAGE_COUNT) {
        furi_event_loop_unsubscribe(data->producer.event_loop, data->message_queue);
        furi_event_loop_pend_callback(
            data->producer.event_loop, test_furi_event_loop_pending_callback, &data->producer);
        return false;
    }

    data->producer.message_count++;

    furi_check(
        furi_message_queue_put(data->message_queue, &data->producer.message_count, 0) ==
        FuriStatusOk);

    furi_delay_us(furi_hal_random_get() % 1000);

    return false;
}

static bool
    test_furi_event_loop_producer_event_flag_callback(FuriEventLoopObject* object, void* context) {
    furi_check(context);

    TestFuriEventLoopData* data = context;
    furi_check(data->event_flag == object);

    const uint32_t producer_flags = (1UL << data->producer.event_flag_count);
    const uint32_t consumer_flags = (1UL << data->consumer.event_flag_count);

    FURI_LOG_I(TAG, "producer EventFlag: 0x%06lX 0x%06lX", producer_flags, consumer_flags);

    furi_check(furi_event_flag_set(data->event_flag, producer_flags) & producer_flags);

    if(data->producer.event_flag_count == EVENT_FLAG_COUNT / 2) {
        furi_event_loop_unsubscribe(data->producer.event_loop, data->event_flag);
        furi_event_loop_subscribe_event_flag(
            data->producer.event_loop,
            data->event_flag,
            FuriEventLoopEventOut,
            test_furi_event_loop_producer_event_flag_callback,
            data);

    } else if(data->producer.event_flag_count == EVENT_FLAG_COUNT) {
        furi_event_loop_unsubscribe(data->producer.event_loop, data->event_flag);
        furi_event_loop_pend_callback(
            data->producer.event_loop, test_furi_event_loop_pending_callback, &data->producer);
        return false;
    }

    data->producer.event_flag_count++;

    furi_delay_us(furi_hal_random_get() % 1000);

    return false;
}

static int32_t test_furi_event_loop_producer(void* p) {
    furi_check(p);

    TestFuriEventLoopData* data = p;
    TestFuriEventLoopThread* producer = &data->producer;

    for(uint32_t i = 0; i < RUN_COUNT; ++i) {
        FURI_LOG_I(TAG, "producer start run %lu", i);

        test_furi_event_loop_thread_init(producer);

        furi_event_loop_subscribe_message_queue(
            producer->event_loop,
            data->message_queue,
            FuriEventLoopEventOut,
            test_furi_event_loop_producer_message_queue_callback,
            data);
        furi_event_loop_subscribe_event_flag(
            producer->event_loop,
            data->event_flag,
            FuriEventLoopEventOut,
            test_furi_event_loop_producer_event_flag_callback,
            data);

        test_furi_event_loop_thread_run_and_cleanup(producer);
    }

    FURI_LOG_I(TAG, "producer end");

    return 0;
}

static bool test_furi_event_loop_consumer_message_queue_callback(
    FuriEventLoopObject* object,
    void* context) {
    furi_check(context);

    TestFuriEventLoopData* data = context;
    furi_check(data->message_queue == object);

    furi_delay_us(furi_hal_random_get() % 1000);

    furi_check(
        furi_message_queue_get(data->message_queue, &data->consumer.message_count, 0) ==
        FuriStatusOk);

    FURI_LOG_I(
        TAG,
        "consumer MessageQueue: %lu %lu",
        data->producer.message_count,
        data->consumer.message_count);

    if(data->consumer.message_count == MESSAGE_COUNT / 2) {
        furi_event_loop_unsubscribe(data->consumer.event_loop, data->message_queue);
        furi_event_loop_subscribe_message_queue(
            data->consumer.event_loop,
            data->message_queue,
            FuriEventLoopEventIn,
            test_furi_event_loop_consumer_message_queue_callback,
            data);

    } else if(data->consumer.message_count == MESSAGE_COUNT) {
        furi_event_loop_unsubscribe(data->consumer.event_loop, data->message_queue);
        furi_event_loop_pend_callback(
            data->consumer.event_loop, test_furi_event_loop_pending_callback, &data->consumer);
    }

    return false;
}

static bool
    test_furi_event_loop_consumer_event_flag_callback(FuriEventLoopObject* object, void* context) {
    furi_check(context);

    TestFuriEventLoopData* data = context;
    furi_check(data->event_flag == object);

    furi_delay_us(furi_hal_random_get() % 1000);

    const uint32_t producer_flags = (1UL << data->producer.event_flag_count);
    const uint32_t consumer_flags = (1UL << data->consumer.event_flag_count);

    furi_check(
        furi_event_flag_wait(data->event_flag, consumer_flags, FuriFlagWaitAny, 0) &
        consumer_flags);

    FURI_LOG_I(TAG, "consumer EventFlag: 0x%06lX 0x%06lX", producer_flags, consumer_flags);

    if(data->consumer.event_flag_count == EVENT_FLAG_COUNT / 2) {
        furi_event_loop_unsubscribe(data->consumer.event_loop, data->event_flag);
        furi_event_loop_subscribe_event_flag(
            data->consumer.event_loop,
            data->event_flag,
            FuriEventLoopEventIn,
            test_furi_event_loop_consumer_event_flag_callback,
            data);

    } else if(data->consumer.event_flag_count == EVENT_FLAG_COUNT) {
        furi_event_loop_unsubscribe(data->consumer.event_loop, data->event_flag);
        furi_event_loop_pend_callback(
            data->consumer.event_loop, test_furi_event_loop_pending_callback, &data->consumer);
        return false;
    }

    data->consumer.event_flag_count++;

    return false;
}

static int32_t test_furi_event_loop_consumer(void* p) {
    furi_check(p);

    TestFuriEventLoopData* data = p;
    TestFuriEventLoopThread* consumer = &data->consumer;

    for(uint32_t i = 0; i < RUN_COUNT; ++i) {
        FURI_LOG_I(TAG, "consumer start run %lu", i);

        test_furi_event_loop_thread_init(consumer);

        furi_event_loop_subscribe_message_queue(
            consumer->event_loop,
            data->message_queue,
            FuriEventLoopEventIn,
            test_furi_event_loop_consumer_message_queue_callback,
            data);
        furi_event_loop_subscribe_event_flag(
            consumer->event_loop,
            data->event_flag,
            FuriEventLoopEventIn,
            test_furi_event_loop_consumer_event_flag_callback,
            data);

        test_furi_event_loop_thread_run_and_cleanup(consumer);
    }

    FURI_LOG_I(TAG, "consumer end");

    return 0;
}

void test_furi_event_loop(void) {
    TestFuriEventLoopData data = {};

    data.message_queue = furi_message_queue_alloc(16, sizeof(uint32_t));
    data.event_flag = furi_event_flag_alloc();

    FuriThread* producer_thread =
        furi_thread_alloc_ex("producer_thread", 1 * 1024, test_furi_event_loop_producer, &data);
    furi_thread_start(producer_thread);

    FuriThread* consumer_thread =
        furi_thread_alloc_ex("consumer_thread", 1 * 1024, test_furi_event_loop_consumer, &data);
    furi_thread_start(consumer_thread);

    // Wait for thread to complete their tasks
    furi_thread_join(producer_thread);
    furi_thread_join(consumer_thread);

    // The test itself
    mu_assert_int_eq(data.producer.message_count, data.consumer.message_count);
    mu_assert_int_eq(data.producer.message_count, MESSAGE_COUNT);
    mu_assert_int_eq(data.producer.event_flag_count, data.consumer.event_flag_count);
    mu_assert_int_eq(data.producer.event_flag_count, EVENT_FLAG_COUNT);

    // Release memory
    furi_thread_free(consumer_thread);
    furi_thread_free(producer_thread);
    furi_message_queue_free(data.message_queue);
    furi_event_flag_free(data.event_flag);
}
