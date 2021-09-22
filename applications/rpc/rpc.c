#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "flipper.pb.h"
#include "furi-hal-delay.h"
#include "furi/check.h"
#include "pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "portmacro.h"
#include "status.pb.h"
#include <stdint.h>
#include <stdio.h>
#include <furi.h>
#include <stream_buffer.h>
#include <m-dict.h>
#include "rpc_i.h"

#define RPC_TAG     "RPC"

#define RPC_EVENT_NEW_DATA          (1 << 0)
#define RPC_EVENT_DISCONNECT        (1 << 1)
#define RPC_EVENTS_ALL              (RPC_EVENT_DISCONNECT | RPC_EVENT_NEW_DATA)

DICT_DEF2(RpcHandlerDict, pb_size_t, M_DEFAULT_OPLIST, RpcHandler, M_POD_OPLIST)

typedef struct {
    RpcSystemAlloc alloc;
    RpcSystemFree free;
    void* context;
} RpcSystemCallbacks;

static RpcSystemCallbacks rpc_systems[] = {
    {
        .alloc = rpc_system_status_alloc,
        .free = NULL,
    },
};

struct RpcSession {
    RpcSendBytesCallback send_bytes_callback;
    void* send_bytes_context;
    RpcInstance* rpc;
    bool terminate_session;
    void** system_contexts;
};

struct RpcInstance {
    bool busy;
    osMutexId_t busy_mutex;
    RpcSession session;
    osEventFlagsId_t events;
    StreamBufferHandle_t stream;
    RpcHandlerDict_t handlers;
    PB_Main* decoded_message;
};


static bool content_callback(pb_istream_t *stream, const pb_field_t *field, void **arg);


static RpcInstance* rpc_alloc(void) {
    RpcInstance* rpc = furi_alloc(sizeof(RpcInstance));
    rpc->busy_mutex = osMutexNew(NULL);
    rpc->busy = false;
    rpc->events = osEventFlagsNew(NULL);
    rpc->stream = xStreamBufferCreate(256, 1);

    rpc->decoded_message = furi_alloc(sizeof(PB_Main));
    rpc->decoded_message->cb_content.funcs.decode = content_callback;
    rpc->decoded_message->cb_content.arg = rpc;

    RpcHandlerDict_init(rpc->handlers);

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

    if (result) {
        RpcSession* session = &rpc->session;
        session->system_contexts = furi_alloc(COUNT_OF(rpc_systems) * sizeof(void*));
        for (int i = 0; i < COUNT_OF(rpc_systems); ++i) {
            session->system_contexts[i] = rpc_systems[i].alloc(rpc);
        }
    }

    return result ? &rpc->session : NULL;       /* support 1 open session for now */
}

void rpc_close_session(RpcSession* session) {
    furi_assert(session);
    furi_assert(session->rpc);
    furi_assert(session->rpc->busy);

    RpcInstance* rpc = session->rpc;
    uint32_t flags_set = osEventFlagsSet(rpc->events, RPC_EVENT_DISCONNECT);
    furi_assert(flags_set & RPC_EVENT_DISCONNECT);
}

void rpc_send_bytes_callback(RpcSession* session, RpcSendBytesCallback callback, void* context) {
    furi_assert(session);
    furi_assert(session->rpc);
    furi_assert(session->rpc->busy);

    session->send_bytes_callback = callback;
    session->send_bytes_context = context;
}

size_t rpc_feed_bytes(RpcSession* session, uint8_t* encoded_bytes, size_t size, TickType_t timeout) {
    furi_assert(session);
    RpcInstance* rpc = session->rpc;
    furi_assert(rpc->busy);

    size_t bytes_sent = xStreamBufferSend(rpc->stream, encoded_bytes, size, timeout);
    uint32_t flags_set = osEventFlagsSet(rpc->events, RPC_EVENT_NEW_DATA);
    furi_assert(flags_set & RPC_EVENT_NEW_DATA);
    return bytes_sent;
}

bool rpc_pb_stream_read(pb_istream_t *istream, pb_byte_t *buf, size_t count) {
    RpcInstance* rpc = istream->state;
    uint32_t flags = 0;
    size_t bytes_received = 0;

    while (1) {
        bytes_received += xStreamBufferReceive(rpc->stream, buf + bytes_received, count - bytes_received, 0);
        if (count == bytes_received) {
            break;
        } else {
            flags = osEventFlagsWait(rpc->events, RPC_EVENTS_ALL, 0, osWaitForever);
            if (flags & RPC_EVENT_DISCONNECT) {
                if (xStreamBufferIsEmpty(rpc->stream)) {
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

void rpc_encode_and_send(RpcInstance* rpc, const PB_Main* main_message) {
    furi_assert(rpc);
    furi_assert(main_message);

    pb_ostream_t ostream = PB_OSTREAM_SIZING;
    bool result = pb_encode(&ostream, &PB_Main_msg, &main_message);
    furi_assert(result);

    size_t size = ostream.bytes_written;
    uint8_t* buffer = furi_alloc(size);
    ostream = pb_ostream_from_buffer(buffer, size);

    result = pb_encode(&ostream, &PB_Main_msg, &main_message);
    furi_assert(result);

    if (rpc->session.send_bytes_callback) {
        rpc->session.send_bytes_callback(rpc->session.send_bytes_context, buffer, size);
    }
    free(buffer);
}

static bool content_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
    furi_assert(stream);
    RpcInstance* rpc = *arg;

    RpcHandler* handler = RpcHandlerDict_get(rpc->handlers, field->tag);

    if (handler && handler->decode_submessage) {
        handler->decode_submessage(stream, field, arg);
    }

    return true;
}

int32_t rpc_srv(void* p) {
    RpcInstance* rpc = rpc_alloc();
    furi_record_create("rpc", rpc);

    pb_istream_t istream = {
        .callback = rpc_pb_stream_read,
        .state = rpc,
        .bytes_left = 0,
        .errmsg = "",
    };

    while(1) {
        if (pb_decode(&istream, &PB_Main_msg, &rpc->decoded_message)) {
            RpcHandler* handler = RpcHandlerDict_get(rpc->handlers, rpc->decoded_message->which_content);

            if (handler && handler->decode_submessage) {
                handler->message_handler(rpc->decoded_message, handler->context);
            } else if (!handler) {
                FURI_LOG_E(RPC_TAG, "Unhandled message, tag: %d\r\n", rpc->decoded_message->which_content);
            }
        } else {
            RpcSession* session = &rpc->session;
            if (session->terminate_session) {
                session->terminate_session = false;
                FURI_LOG_I(RPC_TAG, "Session terminated\r\n");
                for (int i = 0; i < COUNT_OF(rpc_systems); ++i) {
                    if(rpc_systems[i].free) {
                        rpc_systems[i].free(session->system_contexts[i]);
                    }
                }
                free(session->system_contexts);
                RpcHandlerDict_clear(rpc->handlers);
                rpc->busy = false;
            } else {
                FURI_LOG_E(RPC_TAG, "Decode failed\r\n");
            }
        }
    }
    return 0;
}

void rpc_add_handler(RpcInstance* rpc, pb_size_t message_tag, RpcHandler* handler) {
    furi_assert(RpcHandlerDict_get(rpc->handlers, message_tag) == NULL);
    furi_check(!rpc->busy);

    RpcHandlerDict_set_at(rpc->handlers, message_tag, *handler);
}


