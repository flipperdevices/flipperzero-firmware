#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "flipper.pb.h"
#include "furi-hal-delay.h"
#include "furi/check.h"
#include "furi/log.h"
#include "pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "portmacro.h"
#include "status.pb.h"
#include "storage.pb.h"
#include <stdint.h>
#include <stdio.h>
#include <furi.h>
#include <stream_buffer.h>
#include <m-dict.h>
#include "rpc_i.h"

#define RPC_TAG "RPC"

#define RPC_EVENT_NEW_DATA (1 << 0)
#define RPC_EVENT_DISCONNECT (1 << 1)
#define RPC_EVENTS_ALL (RPC_EVENT_DISCONNECT | RPC_EVENT_NEW_DATA)

#define DEBUG_PRINT 0

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
    {
        .alloc = rpc_system_storage_alloc,
        .free = rpc_system_storage_free,
    },
};

struct RpcSession {
    RpcSendBytesCallback send_bytes_callback;
    void* send_bytes_context;
    osMutexId_t send_bytes_mutex;
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

static bool content_callback(pb_istream_t* stream, const pb_field_t* field, void** arg);

static size_t rpc_sprint_element(char* str, size_t str_size, const char* prefix, const PB_Storage_Element* element, size_t elements_size) {
    size_t cnt = 0;

    for (int i = 0; i < elements_size; ++i, ++element) {
        cnt += snprintf(str + cnt, str_size - cnt, "%s[%c] size: %5ld",
                prefix,
                element->type == PB_Storage_Element_FileType_DIR ? 'd' : 'f',
                element->size);

        if (element->name) {
            cnt += snprintf(str + cnt, str_size - cnt, " \'%s\'", element->name);
        }

        if (element->data && element->data->size) {
            cnt += snprintf(str + cnt, str_size - cnt, " (%d):\'%.*s%s\'",
                    element->data->size,
                    MIN(element->data->size, 30),
                    element->data->bytes,
                    element->data->size > 30 ? "..." : "");
        }

        cnt += snprintf(str + cnt, str_size - cnt, "\r\n");
    }

    return cnt;
}

#define ADD_STR(s, c, ...)                      \
    snprintf(s + c, sizeof(s) - c, ##__VA_ARGS__);

#define ADD_STR_ELEMENT(s, c, ...)              \
    rpc_sprint_element(s + c, sizeof(s) - c, ##__VA_ARGS__);

void rpc_print_message(const PB_Main* message) {
    char str[500];
    size_t cnt = 0;

    cnt += snprintf(str + cnt, sizeof(str) - cnt,
            "PB_Main: {\r\n\tresult: %d cmd_id: %ld (%s)\r\n",
        message->command_status,
        message->command_id,
        message->not_last ? "not_last" : "last");
    switch(message->which_content) {
    default:
        /* not implemented yet */
        cnt += ADD_STR(str, cnt, "\tNOT_IMPLEMENTED (%d) {\r\n", message->which_content);
        break;
    case PB_Main_ping_request_tag:
        cnt += ADD_STR(str, cnt, "\tping_request {\r\n");
        break;
    case PB_Main_ping_response_tag:
        cnt += ADD_STR(str, cnt, "\tping_response {\r\n");
        break;
    case PB_Main_storage_mkdir_request_tag:
        cnt += ADD_STR(str, cnt, "\tmkdir {\r\n");
        break;
    case PB_Main_storage_delete_request_tag: {
        cnt += ADD_STR(str, cnt, "\tdelete {\r\n");
        const char* path = message->content.storage_delete_request.path;
        if (path) {
            cnt += ADD_STR(str, cnt, "\t\tpath: %s\r\n", path);
        }
        break;
    }
    case PB_Main_empty_tag:
        cnt += ADD_STR(str, cnt, "\tempty {\r\n");
        break;
    case PB_Main_storage_list_request_tag: {
        cnt += ADD_STR(str, cnt, "\tlist_request {\r\n");
        const char* path = message->content.storage_list_request.path;
        if (path) {
            cnt += ADD_STR(str, cnt, "\t\tpath: %s\r\n", path);
        }
        break;
    }
    case PB_Main_storage_read_request_tag: {
        cnt += ADD_STR(str, cnt, "\tread_request {\r\n");
        const char* path = message->content.storage_read_request.path;
        if (path) {
            cnt += ADD_STR(str, cnt, "\t\tpath: %s\r\n", path);
        }
        break;
    }
    case PB_Main_storage_write_request_tag: {
        cnt += ADD_STR(str, cnt, "\twrite_request {\r\n");
        const char* path = message->content.storage_write_request.path;
        if (path) {
            cnt += ADD_STR(str, cnt, "\t\tpath: %s\r\n", path);
        }
        if (message->content.storage_write_request.has_storage_element) {
            const PB_Storage_Element* element = &message->content.storage_write_request.storage_element;
            cnt += ADD_STR_ELEMENT(str, cnt, "\t\t\t", element, 1);
        }
        break;
    }
    case PB_Main_storage_read_response_tag:
        cnt += ADD_STR(str, cnt, "\tread_response {\r\n");
        if (message->content.storage_read_response.has_storage_element) {
            const PB_Storage_Element* element = &message->content.storage_read_response.storage_element;
            cnt += ADD_STR_ELEMENT(str, cnt, "\t\t\t", element, 1);
        }
        break;
    case PB_Main_storage_list_response_tag: {
        const PB_Storage_Element* element = message->content.storage_list_response.storage_element;
        size_t element_count = message->content.storage_list_response.storage_element_count;
        cnt += ADD_STR(str, cnt, "\tlist_response {\r\n");
        cnt += ADD_STR_ELEMENT(str, cnt, "\t\t", element, element_count);
    }
    }
    cnt += ADD_STR(str, cnt, "\t}\r\n}\r\n");
    printf("%s", str);
}

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
    if(rpc->busy) {
        result = false;
    } else {
        rpc->busy = true;
        result = true;
    }
    furi_check(osMutexRelease(rpc->busy_mutex) == osOK);

    if(result) {
        RpcSession* session = &rpc->session;
        session->send_bytes_mutex = osMutexNew(NULL);
        session->rpc = rpc;
        session->terminate_session = false;
        session->system_contexts = furi_alloc(COUNT_OF(rpc_systems) * sizeof(void*));
        for(int i = 0; i < COUNT_OF(rpc_systems); ++i) {
            session->system_contexts[i] = rpc_systems[i].alloc(rpc);
        }
        FURI_LOG_D(RPC_TAG, "Session started\r\n");
    }

    return result ? &rpc->session : NULL; /* support 1 open session for now */
}

void rpc_close_session(RpcSession* session) {
    furi_assert(session);
    furi_assert(session->rpc);
    furi_assert(session->rpc->busy);

    osMutexDelete(session->send_bytes_mutex);
    rpc_set_send_bytes_callback(session, NULL, NULL);
    osEventFlagsSet(session->rpc->events, RPC_EVENT_DISCONNECT);
}

void rpc_set_send_bytes_callback(RpcSession* session, RpcSendBytesCallback callback, void* context) {
    furi_assert(session);
    furi_assert(session->rpc);
    furi_assert(session->rpc->busy);

    osMutexAcquire(session->send_bytes_mutex, osWaitForever);
    session->send_bytes_callback = callback;
    session->send_bytes_context = context;
    osMutexRelease(session->send_bytes_mutex);
}

size_t
    rpc_feed_bytes(RpcSession* session, uint8_t* encoded_bytes, size_t size, TickType_t timeout) {
    furi_assert(session);
    RpcInstance* rpc = session->rpc;
    furi_assert(rpc->busy);

    size_t bytes_sent = xStreamBufferSend(rpc->stream, encoded_bytes, size, timeout);
    osEventFlagsSet(rpc->events, RPC_EVENT_NEW_DATA);

    return bytes_sent;
}

bool rpc_pb_stream_read(pb_istream_t* istream, pb_byte_t* buf, size_t count) {
    RpcInstance* rpc = istream->state;
    uint32_t flags = 0;
    size_t bytes_received = 0;

    while(1) {
        bytes_received +=
            xStreamBufferReceive(rpc->stream, buf + bytes_received, count - bytes_received, 0);
        if(count == bytes_received) {
            break;
        } else {
            flags = osEventFlagsWait(rpc->events, RPC_EVENTS_ALL, 0, osWaitForever);
            if(flags & RPC_EVENT_DISCONNECT) {
                if(xStreamBufferIsEmpty(rpc->stream)) {
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

void rpc_encode_and_send(RpcInstance* rpc, PB_Main* main_message) {
    furi_assert(rpc);
    furi_assert(main_message);
    RpcSession* session = &rpc->session;
    pb_ostream_t ostream = PB_OSTREAM_SIZING;

#if DEBUG_PRINT
    FURI_LOG_I(RPC_TAG, "OUTPUT:");
    rpc_print_message(main_message);
#endif

    bool result = pb_encode_ex(&ostream, &PB_Main_msg, main_message, PB_ENCODE_DELIMITED);
    furi_check(result && ostream.bytes_written);

    uint8_t* buffer = furi_alloc(ostream.bytes_written);
    ostream = pb_ostream_from_buffer(buffer, ostream.bytes_written);

    pb_encode_ex(&ostream, &PB_Main_msg, main_message, PB_ENCODE_DELIMITED);

    {
        osMutexAcquire(session->send_bytes_mutex, osWaitForever);

#if DEBUG_PRINT
        printf("\r\nREPONSE DEC(%d): {", ostream.bytes_written);
        for (int i = 0; i < ostream.bytes_written; ++i) {
            printf("%d, ", buffer[i]);
        }
        printf("}\r\n");

        printf("REPONSE HEX(%d): {", ostream.bytes_written);
        for (int i = 0; i < ostream.bytes_written; ++i) {
            printf("%02X" , buffer[i]);
        }
        printf("}\r\n\r\n");
#endif  // DEBUG_PRINT

        if(session->send_bytes_callback) {
            session->send_bytes_callback(session->send_bytes_context, buffer, ostream.bytes_written);
        }
        osMutexRelease(session->send_bytes_mutex);
    }
    free(buffer);
}

static bool content_callback(pb_istream_t* stream, const pb_field_t* field, void** arg) {
    furi_assert(stream);
    RpcInstance* rpc = *arg;

    RpcHandler* handler = RpcHandlerDict_get(rpc->handlers, field->tag);

    if(handler && handler->decode_submessage) {
        handler->decode_submessage(stream, field, arg);
    }

    return true;
}

int32_t rpc_srv(void* p) {
    RpcInstance* rpc = rpc_alloc();
    furi_record_create("rpc", rpc);

    while(1) {
        pb_istream_t istream = {
            .callback = rpc_pb_stream_read,
            .state = rpc,
            .errmsg = NULL,
            .bytes_left = 0x7FFFFFFF,
        };

        if(pb_decode_ex(&istream, &PB_Main_msg, rpc->decoded_message, PB_DECODE_DELIMITED)) {
#if DEBUG_PRINT
            FURI_LOG_I(RPC_TAG, "INPUT:");
            rpc_print_message(rpc->decoded_message);
#endif
            RpcHandler* handler =
                RpcHandlerDict_get(rpc->handlers, rpc->decoded_message->which_content);

            if(handler && handler->message_handler) {
                handler->message_handler(rpc->decoded_message, handler->context);
            } else if(!handler) {
                FURI_LOG_E(
                    RPC_TAG,
                    "Unhandled message, tag: %d\r\n",
                    rpc->decoded_message->which_content);
            }
            pb_release(&PB_Main_msg, rpc->decoded_message);
        } else {
            pb_release(&PB_Main_msg, rpc->decoded_message);
            RpcSession* session = &rpc->session;
            if(session->terminate_session) {
                session->terminate_session = false;
                osEventFlagsClear(rpc->events, RPC_EVENTS_ALL);
                FURI_LOG_D(RPC_TAG, "Session terminated\r\n");
                for(int i = 0; i < COUNT_OF(rpc_systems); ++i) {
                    if(rpc_systems[i].free) {
                        rpc_systems[i].free(session->system_contexts[i]);
                    }
                }
                free(session->system_contexts);
                RpcHandlerDict_clean(rpc->handlers);
                rpc->busy = false;
            } else {
                xStreamBufferReset(rpc->stream);
                FURI_LOG_E(
                    RPC_TAG, "Decode failed, error: \'%.128s\'\r\n", PB_GET_ERROR(&istream));
            }
        }
    }
    return 0;
}

void rpc_add_handler(RpcInstance* rpc, pb_size_t message_tag, RpcHandler* handler) {
    furi_assert(RpcHandlerDict_get(rpc->handlers, message_tag) == NULL);

    RpcHandlerDict_set_at(rpc->handlers, message_tag, *handler);
}

void rpc_encode_and_send_empty(RpcInstance* rpc, uint32_t command_id, PB_CommandStatus status) {
    PB_Main message = {
        .command_id = command_id,
        .command_status = status,
        .not_last = false,
        .which_content = PB_Main_empty_tag,
    };
    rpc_encode_and_send(rpc, &message);
    pb_release(&PB_Main_msg, &message);
}

