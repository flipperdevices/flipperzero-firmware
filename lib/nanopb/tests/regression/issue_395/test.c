#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "test.pb.h"
#include "unittests.h"

int main(int argc, char **argv)
{
    int status = 0;
    uint8_t buffer[512] = {0};
    int i;
    pb_ostream_t ostream;
    
    Reply reply = Reply_init_zero;
    Reply_Result request_result = Reply_Result_OK;

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    reply.result = request_result;
    if (!pb_encode(&ostream, Reply_fields, &reply)) {
        fprintf(stderr, "Encode failed: %s\n", PB_GET_ERROR(&ostream));
        return 1;
    }

    printf("response payload (%d):", (int)ostream.bytes_written);
    for (i = 0; i < ostream.bytes_written; i++) {
        printf("%02X", buffer[i]);
    }
    printf("\n");

    TEST(ostream.bytes_written == 2);
    TEST(buffer[0] == 0x08);
    TEST(buffer[1] == 0x01);
    
    return status;
}

