#include "flipper.pb.h"
#include "furi-hal-delay.h"
#include "furi/check.h"
#include "furi/record.h"
#include "pb_decode.h"
#include "rpc/rpc.h"
#include "storage.pb.h"
#include "storage/storage.h"
#include <furi.h>
#include "../minunit.h"
#include <stdint.h>
#include <stream_buffer.h>
#include <pb.h>
#include <pb_encode.h>
#include <m-list.h>


/* TODO: check does it copy whole struct into List API, maybe should use M_PTR_OPLIST ?*/
LIST_DEF(MsgList, PB_Main, M_POD_OPLIST)
#define M_OPL_MsgList_t() LIST_OPLIST(MsgList)

/* MinUnit test framework doesn't allow passing context into tests,
 * so we have to use global variables
 */
static RpcInstance* rpc = NULL;
static RpcSession* session = NULL;
static StreamBufferHandle_t output_stream = NULL;
static uint64_t command_id = 0;

#define TEST_RPC_TAG                    "TEST_RPC"
#define MAX_RECEIVE_OUTPUT_TIMEOUT      3000
#define MAX_NAME_LENGTH                 255

#define DEBUG_PRINT    1

#define BYTES(x)        (x), sizeof(x)

static void test_setup(void) {
    furi_assert(!rpc);
    furi_assert(!session);
    furi_assert(!output_stream);

    rpc = furi_record_open("rpc");
    for (int i = 0; !session && (i < 10000); ++i) {
        session = rpc_open_session(rpc);
        delay(1);
    }

    output_stream = xStreamBufferCreate(256, 1);
    mu_assert(session, "failed to start session");
}

static void test_teardown(void) {
    rpc_close_session(session);
    furi_record_close("rpc");
    vStreamBufferDelete(output_stream);
    ++command_id;
    output_stream = NULL;
    rpc = NULL;
    session = NULL;
}

static void output_bytes_callback(void* ctx, uint8_t* got_bytes, size_t got_size) {
    StreamBufferHandle_t stream_buffer = ctx;
    size_t bytes_sent = xStreamBufferSend(stream_buffer, got_bytes, got_size, osWaitForever);
    furi_assert(bytes_sent == got_size);
}

static void compare_expected_bytes(uint8_t* expected_bytes, size_t expected_size, StreamBufferHandle_t stream_buffer) {
    furi_assert(expected_bytes);
    furi_assert(expected_size);
    uint8_t* got_bytes = furi_alloc(expected_size);

    size_t got_size = xStreamBufferReceive(stream_buffer, got_bytes, expected_size, MAX_RECEIVE_OUTPUT_TIMEOUT);
    mu_check(got_size == expected_size);

    for (size_t i = 0; i < got_size; ++i) {
        mu_check(got_bytes[i] == expected_bytes[i]);
    }

    free(got_bytes);
}

//static void run_single_message_session(uint8_t* send_bytes, size_t send_bytes_size, uint8_t* expected_bytes, size_t expected_bytes_size) {
//}

MU_TEST(test_ping_raw) {

// command_id: 0x123
// command_status: 4
// not_last: 0
// ping_request: {
// }
    static uint8_t input_ping_request[] = {0x07, 0x08, 0xA3, 0x02, 0x10, 0x04, 0x22, 0x00};

// command_id: 0x123
// command_status: 0
// not_last: 0
// ping_response: {
// }
    static uint8_t expected_ping_response[] = {0x05, 0x08, 0xA3, 0x02, 0x2A, 0x00};

    rpc_set_send_bytes_callback(session, output_bytes_callback, output_stream);
    size_t bytes_sent = rpc_feed_bytes(session, BYTES(input_ping_request), 0);
    furi_check(bytes_sent == sizeof(input_ping_request));

    compare_expected_bytes(BYTES(expected_ping_response), output_stream);
}

static void test_rpc_create_storage_list_request(PB_Main* request, const char* path, uint64_t command_id) {
    furi_assert(request);
    furi_assert(path);

    char* path_str = furi_alloc(strlen(path));
    strcpy(path_str, path);
    PB_Storage_ListRequest list = {
        .path = path_str,
    };
    request->command_id = command_id;
    request->command_status = 0;
    request->cb_content.funcs.encode = NULL;
    request->which_content = PB_Main_storage_list_response_tag;
    request->not_last = false;
    request->content.storage_list_request = list;
}

static void test_rpc_encode_and_feed(PB_Main* request) {
    furi_assert(request);

    pb_ostream_t ostream = PB_OSTREAM_SIZING;

    bool result = pb_encode_ex(&ostream, &PB_Main_msg, request, PB_ENCODE_DELIMITED);
    furi_check(result && ostream.bytes_written);

    uint8_t* buffer = furi_alloc(ostream.bytes_written);
    ostream = pb_ostream_from_buffer(buffer, ostream.bytes_written);

    pb_encode_ex(&ostream, &PB_Main_msg, request, PB_ENCODE_DELIMITED);

    size_t bytes_sent = rpc_feed_bytes(session, buffer, ostream.bytes_written, 0);
    furi_check(bytes_sent == ostream.bytes_written);
    free(buffer);
    pb_release(&PB_Main_msg, request);
}

static void test_rpc_compare_messages(PB_Main* result, PB_Main* expected) {
    mu_check(result->command_id == expected->command_id);
    mu_check(result->command_status == expected->command_status);
    mu_check(result->not_last == expected->not_last);
    mu_check(result->which_content == expected->which_content);
    switch(result->which_content) {
    case PB_Main_dummy_tag:
    case PB_Main_ping_response_tag:
        /* nothing to check */
        break;
    case PB_Main_ping_request_tag:
    case PB_Main_storage_list_request_tag:
    case PB_Main_storage_delete_request_tag:
    case PB_Main_storage_read_request_tag:
    case PB_Main_storage_write_request_tag:
        /* not implemented yet, and will be ever? */
        break;
    case PB_Main_storage_read_response_tag:
        /* not implemented yet */
        break;
    case PB_Main_storage_list_response_tag: {
        size_t expected_elements = expected->content.storage_list_response.storage_element_count;
        size_t result_elements = result->content.storage_list_response.storage_element_count;
        mu_check(result_elements == expected_elements);
        for (int i = 0; i < expected_elements; ++i) {
            PB_Storage_Element* result_element = &result->content.storage_list_response.storage_element[i];
            PB_Storage_Element* expected_element = &expected->content.storage_list_response.storage_element[i];
            mu_assert(!strcmp(result_element->name, expected_element->name), "storage list: name mismatch");
            mu_assert(result_element->size == expected_element->size, "storage list: size mismatch");
            mu_assert(result_element->type == expected_element->type, "storage list: type mismatch");
            // TODO: add data comparation
        }
        break;
    }
    }
}

#if DEBUG_PRINT
static void test_rpc_print_message(const PB_Main* message) {
    FURI_LOG_I(TEST_RPC_TAG, "PB_Main:");
    FURI_LOG_I(TEST_RPC_TAG, "\tresult: %d cmd_id: %lld (%s) {", 
            message->command_status,
            message->command_id,
            (!message->not_last) ? "last" : "not_last");
    switch(message->which_content) {
    case PB_Main_dummy_tag:
    case PB_Main_ping_response_tag:
    case PB_Main_ping_request_tag:
    case PB_Main_storage_list_request_tag:
    case PB_Main_storage_delete_request_tag:
    case PB_Main_storage_read_request_tag:
    case PB_Main_storage_write_request_tag:
    case PB_Main_storage_read_response_tag:
        /* not implemented yet */
        break;
    case PB_Main_storage_list_response_tag: {
        const PB_Storage_Element* element = message->content.storage_list_response.storage_element;
        size_t element_count = message->content.storage_list_response.storage_element_count;
        for (int j = 0; j < element_count; ++j) {
            FURI_LOG_I(TEST_RPC_TAG, "\t[%s] \'%s\' size: %d",
                    element->type == PB_Storage_Element_FileType_DIR ? "d" : "f",
                    element->name,
                    element->size);
            /* TODO: add print data */
            ++element;
        }
    }
    }
    FURI_LOG_I(TEST_RPC_TAG, "}");
}
#endif  // DEBUG_PRINT

static bool test_rpc_pb_stream_read(pb_istream_t *istream, pb_byte_t *buf, size_t count) {
    StreamBufferHandle_t stream_buffer = istream->state;
    size_t bytes_received = 0;

#if DEBUG_PRINT
    printf("TEST_ASK %d", count);
#endif
    bytes_received = xStreamBufferReceive(stream_buffer, buf, count, MAX_RECEIVE_OUTPUT_TIMEOUT);

#if DEBUG_PRINT
    printf("TEST_<== %d:", count);
    for (int i = 0; i < bytes_received; ++i) {
        printf(" [%X]", buf[i]);
    }
    printf("\r\n");
#endif

    return (count == bytes_received);
}

static void test_rpc_system_storage_list_create_list(MsgList_t msg_list, const char* path, uint64_t command_id) {

    Storage* fs_api = furi_record_open("storage");
    File* dir = storage_file_alloc(fs_api);

    FURI_LOG_I(TEST_RPC_TAG, "Storage list path: \'%.128s\', cmd_id: %lld", path, command_id);

    PB_Main response = {
        .command_id = command_id,
        .not_last = false,
        .which_content = PB_Main_storage_list_request_tag,
    };
    PB_Storage_ListResponse* list = &response.content.storage_list_response;
    response.which_content = PB_Main_storage_list_response_tag;

    bool finish = false;
    int i = 0;

    if(storage_dir_open(dir, path)) {
        response.command_status = PB_CommandStatus_OK;
    } else {
        response.command_status = PB_CommandStatus_ERROR_NO_FILE;
        finish = true;
    }

    while(!finish) {
        FileInfo fileinfo;
        char* name = furi_alloc(MAX_NAME_LENGTH + 1);
        if (storage_dir_read(dir, &fileinfo, name, MAX_NAME_LENGTH)) {
            if ((i + 1) == COUNT_OF(list->storage_element)) {
                list->storage_element_count = i;
                response.not_last = true;
#if DEBUG_PRINT
                test_rpc_print_message(&response);
#endif  // DEBUG_PRINT
                MsgList_push_back(msg_list, response);
                i = 0;
            }
            list->storage_element[i].type = (fileinfo.flags & FSF_DIRECTORY)
                                            ? PB_Storage_Element_FileType_DIR
                                            : PB_Storage_Element_FileType_FILE;
            list->storage_element[i].size = fileinfo.size;
            list->storage_element[i].data.funcs.encode = NULL;
            /* memory free inside rpc_encode_and_send() -> pb_release() */
            list->storage_element[i].name = name;
            ++i;
        } else {
            finish = true;
            free(name);
        }
    }

    list->storage_element_count = i;
    response.not_last = false;
#if DEBUG_PRINT
    test_rpc_print_message(&response);
#endif  // DEBUG_PRINT
    MsgList_push_back(msg_list, response);

    storage_dir_close(dir);
    storage_file_free(dir);

    furi_record_close("storage");
}


static void test_rpc_decode_and_compare(MsgList_t msg_list) {
    MsgList_it_ct list_it;
    MsgList_it(list_it, msg_list);

    while(1) {
        pb_istream_t istream = {
            .callback = test_rpc_pb_stream_read,
            .state = output_stream,
            .errmsg = NULL,
            .bytes_left = 0x7FFFFFFF,
        };
        PB_Main result;
        if (!pb_decode_ex(&istream, &PB_Main_msg, &result, PB_DECODE_DELIMITED)) {
            break;
        }

        if (MsgList_end_p(list_it)) {
            mu_assert(0, "got more messages than expected");
            break;
        }
        test_rpc_compare_messages(&result, MsgList_ref(list_it));
        MsgList_next(list_it);
    }

    mu_assert(MsgList_end_p(list_it), "not all expected messages decoded");
}

MU_TEST(test_storage_list) {
    PB_Main request;
    MsgList_t msg_list;
    MsgList_init(msg_list);

    test_rpc_system_storage_list_create_list(msg_list, "/", command_id);
    rpc_set_send_bytes_callback(session, output_bytes_callback, output_stream);
    test_rpc_create_storage_list_request(&request, "/", command_id);
    test_rpc_encode_and_feed(&request);
    test_rpc_decode_and_compare(msg_list);

    MsgList_clear(msg_list);
}




MU_TEST(test_storage_list_raw) {
// command_id: 0x123
// command_status: 0
// not_last: 0
// storage_list_request: {
//     path:"/E/music/kirkorov"
// }
    static uint8_t input_bytes_list[] = {24 /*0x18*/,
        0x08, 0xA3, 0x02, 0x32, 0x13, 0x0A, 0x11, 0x2F,
        0x45, 0x2F, 0x6D, 0x75, 0x73, 0x69, 0x63, 0x2F,
        0x6B, 0x69, 0x72, 0x6B, 0x6F, 0x72, 0x6F, 0x76};

    rpc_set_send_bytes_callback(session, output_bytes_callback, output_stream);
    size_t bytes_sent = rpc_feed_bytes(session, BYTES(input_bytes_list), 0);
    furi_check(bytes_sent == sizeof(input_bytes_list));

    delay(1000);        // tmp

    static uint8_t expected_ping_response[] = {7, 0x08, 0xA3, 0x02, 0x10, 0x03, 0x3A, 0x00};
    compare_expected_bytes(BYTES(expected_ping_response), output_stream);
}

MU_TEST_SUITE(test_irda_decoder_encoder) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);

    volatile int a = 0;
    if (a) {
        MU_RUN_TEST(test_ping_raw);
        MU_RUN_TEST(test_storage_list);
    } else {
        MU_RUN_TEST(test_storage_list_raw);
    }
}

int run_minunit_test_rpc() {
    MU_RUN_SUITE(test_irda_decoder_encoder);
    MU_REPORT();

    return MU_EXIT_CODE;
}
