#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "flipper.pb.h"
#include "furi-hal-delay.h"
#include "furi/check.h"
#include "pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "portmacro.h"
#include <stdint.h>
#include <stdio.h>
#include <furi.h>
#include <stream_buffer.h>

#define RPC_TAG     "RPC"

typedef struct RpcInstance RpcInstance;
typedef void (*RpcReponseCallback) (void* context, uint8_t* bytes, size_t bytes_len);

typedef struct {
    StreamBufferHandle_t stream;
    RpcReponseCallback callback;
    void* context;
    RpcInstance* rpc;
    bool terminate_session;
} RpcSession;

struct RpcInstance {
    bool busy;
    osMutexId_t busy_mutex;
    RpcSession session;
    osEventFlagsId_t events;
};

#define RPC_EVENT_NEW_DATA          (1 << 0)
#define RPC_EVENT_DISCONNECT        (1 << 1)
#define RPC_EVENTS_ALL              (RPC_EVENT_DISCONNECT | RPC_EVENT_NEW_DATA)


static RpcInstance* rpc_alloc(void) {
    RpcInstance* rpc = furi_alloc(sizeof(RpcInstance));
    rpc->busy_mutex = osMutexNew(NULL);
    rpc->busy = false;
    rpc->session.stream = xStreamBufferCreate(256, 1);
    rpc->events = osEventFlagsNew(NULL);

    return rpc;
}

RpcSession* rpc_open_session(RpcInstance* rpc) {
    furi_assert(rpc);
    bool result = false;
    furi_check(osMutexAcquire(rpc->busy_mutex, osWaitForever) == osOK);
    if (rpc->busy) {
        result = false;
    } else {
        rpc->busy = true;
        result = true;
    }
    furi_check(osMutexRelease(rpc->busy_mutex) == osOK);
    return result ? &rpc->session : NULL;       /* support 1 open session for now */
}

void rpc_close_session(RpcSession* session) {
    furi_assert(session);
    RpcInstance* rpc = session->rpc;


//    return xStreamBufferSend(rpc->session.stream, encoded_bytes, size, timeout);

    while(xStreamBufferBytesAvailable(rpc->session.stream)) {
        delay(10);  /* wait for RPC task to rake up buffer */
    }

    uint8_t dummy = 0;
    furi_check(xStreamBufferSend(rpc->session.stream, &dummy, 1, 0) == osOK);
    furi_assert(rpc->busy);
    rpc->busy = false;  /* no need to guard */
}

void rpc_set_response_callback(RpcSession* session, RpcReponseCallback callback, void* context) {
    furi_assert(session);
    RpcInstance* rpc = session->rpc;
    furi_assert(rpc->busy);

    rpc->session.callback = callback;
    rpc->session.context = context;
}

size_t rpc_feed_bytes(RpcSession* session, uint8_t* encoded_bytes, size_t size, TickType_t timeout) {
    furi_assert(session);
    RpcInstance* rpc = session->rpc;
    furi_assert(rpc->busy);

    size_t bytes_sent = xStreamBufferSend(rpc->session.stream, encoded_bytes, size, timeout);
    uint32_t flags_set = osEventFlagsSet(rpc->events, RPC_EVENT_NEW_DATA);
    furi_assert(flags_set & RPC_EVENT_NEW_DATA);
    return bytes_sent;
}

bool rpc_pb_stream_read(pb_istream_t *stream, pb_byte_t *buf, size_t count) {
    RpcInstance* rpc = stream->state;
    RpcSession* session = &rpc->session;
    uint32_t flags = 0;
    size_t bytes_received = 0;

    while (1) {
        bytes_received += xStreamBufferReceive(session->stream, buf + bytes_received, count - bytes_received, 0);
        if (count == bytes_received) {
            break;
        } else {
            flags = osEventFlagsWait(rpc->events, RPC_EVENTS_ALL, 0, osWaitForever);
            if (flags & RPC_EVENT_DISCONNECT) {
                if (xStreamBufferIsEmpty(session->stream)) {
                    rpc->session.terminate_session = true;
                    break;
                } else {
                    /* Save disconnect flag and continue reading buffer */
                    osEventFlagsSet(rpc->events, RPC_EVENT_DISCONNECT);
                }
            }
        }
    }

    return (count == bytes_received);
}

///////////////////

void rpc_process_ping_request(RpcInstance* rpc, const Main_Request* msg) {
    furi_assert(rpc);
    furi_assert(msg);
    Main_Response msg_response = Main_Response_init_default;
    msg_response.last = false;
    msg_response.status = Main_Status_OK;
    msg_response.uid = msg->uid;

    pb_ostream_t stream = PB_OSTREAM_SIZING;
    bool result = pb_encode(&stream, &Main_Response_msg, &msg_response);
    furi_assert(result);

    size_t size = stream.bytes_written;
    uint8_t* buffer = furi_alloc(size);
    stream = pb_ostream_from_buffer(buffer, size);

    result = pb_encode(&stream, &Main_Response_msg, &msg_response);
    furi_assert(result);

    if (rpc->session.callback) {
        rpc->session.callback(rpc->session.context, buffer, size);
    }
    free(buffer);
}

void rpc_decode_and_process(RpcInstance* rpc, RpcBuffer* buffer) {
    Main_Request msg = Main_Request_init_default;
    pb_istream_t stream;

    stream = pb_istream_from_buffer(buffer->bytes, buffer->size);
}

// TODO: ask Aku about what is better -
// to have additional copy, or to have outstanding RpcBuffer
int32_t rpc_srv(void* p) {
    RpcInstance* rpc = rpc_alloc();
    furi_record_create("rpc", rpc);
    RpcBuffer buffer;

    while(1) {
        if (pb_decode(&stream, &Main_Request_msg, &msg)) {
            switch (msg.which_content) {
            case Main_Request_ping_request_tag:
                rpc_process_ping_request(rpc, &msg);
                break;
            case Main_Request_storage_list_request_tag:
                break;
            case Main_Request_storage_read_request_tag:
                break;
            case Main_Request_storage_write_request_tag:
                break;
            case Main_Request_storage_delete_request_tag:
                break;
            default:
                furi_assert(0);
                break;
            }
        } else {
            if (rpc->session.terminate_session) {
                FURI_LOG_I(RPC_TAG, "Session terminated\r\n");
                rpc->busy = false;
            } else {
                FURI_LOG_E(RPC_TAG, "Decode failed\r\n");
            }
        }
    }
    return 0;
}

