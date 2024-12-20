#include <furi.h>
#include "../test.h" // IWYU pragma: keep

#define MESSAGE_QUEUE_CAPACITY     (16U)
#define MESSAGE_QUEUE_ELEMENT_SIZE (sizeof(uint32_t))

#define STREAM_BUFFER_SIZE      (32U)
#define STREAM_BUFFER_TRG_LEVEL (STREAM_BUFFER_SIZE / 2U)

typedef struct {
    FuriMessageQueue* message_queue;
    FuriStreamBuffer* stream_buffer;
} TestFuriPrimitivesData;

static void test_furi_message_queue(TestFuriPrimitivesData* data) {
    FuriMessageQueue* message_queue = data->message_queue;

    mu_assert_int_eq(0, furi_message_queue_get_count(message_queue));
    mu_assert_int_eq(MESSAGE_QUEUE_CAPACITY, furi_message_queue_get_space(message_queue));
    mu_assert_int_eq(MESSAGE_QUEUE_CAPACITY, furi_message_queue_get_capacity(message_queue));
    mu_assert_int_eq(
        MESSAGE_QUEUE_ELEMENT_SIZE, furi_message_queue_get_message_size(message_queue));

    for(uint32_t i = 0;; ++i) {
        mu_assert_int_eq(MESSAGE_QUEUE_CAPACITY - i, furi_message_queue_get_space(message_queue));
        mu_assert_int_eq(i, furi_message_queue_get_count(message_queue));

        if(furi_message_queue_put(message_queue, &i, 0) != FuriStatusOk) {
            break;
        }
    }

    mu_assert_int_eq(0, furi_message_queue_get_space(message_queue));
    mu_assert_int_eq(MESSAGE_QUEUE_CAPACITY, furi_message_queue_get_count(message_queue));

    for(uint32_t i = 0;; ++i) {
        mu_assert_int_eq(i, furi_message_queue_get_space(message_queue));
        mu_assert_int_eq(MESSAGE_QUEUE_CAPACITY - i, furi_message_queue_get_count(message_queue));

        uint32_t value;
        if(furi_message_queue_get(message_queue, &value, 0) != FuriStatusOk) {
            break;
        }

        mu_assert_int_eq(i, value);
    }

    mu_assert_int_eq(0, furi_message_queue_get_count(message_queue));
    mu_assert_int_eq(MESSAGE_QUEUE_CAPACITY, furi_message_queue_get_space(message_queue));
}

static void test_furi_stream_buffer(TestFuriPrimitivesData* data) {
    FuriStreamBuffer* stream_buffer = data->stream_buffer;

    mu_assert(furi_stream_buffer_is_empty(stream_buffer), "Must be empty");
    mu_assert(!furi_stream_buffer_is_full(stream_buffer), "Must be not full");
    mu_assert_int_eq(0, furi_stream_buffer_bytes_available(stream_buffer));
    mu_assert_int_eq(STREAM_BUFFER_SIZE, furi_stream_buffer_spaces_available(stream_buffer));

    for(uint8_t i = 0;; ++i) {
        mu_assert_int_eq(i, furi_stream_buffer_bytes_available(stream_buffer));
        mu_assert_int_eq(
            STREAM_BUFFER_SIZE - i, furi_stream_buffer_spaces_available(stream_buffer));

        if(furi_stream_buffer_send(stream_buffer, &i, sizeof(uint8_t), 0) != sizeof(uint8_t)) {
            break;
        }
    }

    mu_assert(!furi_stream_buffer_is_empty(stream_buffer), "Must be not empty");
    mu_assert(furi_stream_buffer_is_full(stream_buffer), "Must be full");
    mu_assert_int_eq(STREAM_BUFFER_SIZE, furi_stream_buffer_bytes_available(stream_buffer));
    mu_assert_int_eq(0, furi_stream_buffer_spaces_available(stream_buffer));

    for(uint8_t i = 0;; ++i) {
        mu_assert_int_eq(
            STREAM_BUFFER_SIZE - i, furi_stream_buffer_bytes_available(stream_buffer));
        mu_assert_int_eq(i, furi_stream_buffer_spaces_available(stream_buffer));

        uint8_t value;
        if(furi_stream_buffer_receive(stream_buffer, &value, sizeof(uint8_t), 0) !=
           sizeof(uint8_t)) {
            break;
        }

        mu_assert_int_eq(i, value);
    }
}

// This is a stub that needs expanding
void test_furi_primitives(void) {
    TestFuriPrimitivesData data = {
        .message_queue =
            furi_message_queue_alloc(MESSAGE_QUEUE_CAPACITY, MESSAGE_QUEUE_ELEMENT_SIZE),
        .stream_buffer = furi_stream_buffer_alloc(STREAM_BUFFER_SIZE, STREAM_BUFFER_TRG_LEVEL),
    };

    test_furi_message_queue(&data);
    test_furi_stream_buffer(&data);

    furi_message_queue_free(data.message_queue);
    furi_stream_buffer_free(data.stream_buffer);
}
