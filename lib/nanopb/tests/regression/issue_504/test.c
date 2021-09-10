#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pb_encode.h>
#include "test.pb.h"
#include "unittests.h"

const char STR[] = "test str";
#define ALIGN 0x100

int main(int argc, char **argv)
{
    int status = 0;
    uint8_t buffer[512] = {0};
    int i;

    {
        pb_ostream_t ostream;
        MyMessage msg = MyMessage_init_zero;
        char *pStr, *pStrAligned;

        COMMENT("Test for false negatives with pointer value low byte 0x00")
        ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        /* copy STR to a malloced 0x100 aligned address */
        pStr = malloc(sizeof(STR) + ALIGN);
        pStrAligned = (char*)((uintptr_t)(pStr + ALIGN) & ~(ALIGN - 1));
        memcpy(pStrAligned, STR, sizeof(STR));

        msg.submessage.somestring = pStrAligned;
        printf("%p: '%s'\n", msg.submessage.somestring, msg.submessage.somestring);

        if (!pb_encode(&ostream, MyMessage_fields, &msg)) {
            fprintf(stderr, "Encode failed: %s\n", PB_GET_ERROR(&ostream));
            return 1;
        }

        free(pStr);
        msg.submessage.somestring = NULL;

        printf("response payload (%d):", (int)ostream.bytes_written);
        for (i = 0; i < ostream.bytes_written; i++) {
            printf("%02X", buffer[i]);
        }
        printf("\n");

        TEST(ostream.bytes_written != 0);
    }

    {
        pb_ostream_t ostream;
        struct {
            MyMessage msg;
            uint32_t bar;
         } msg = {MyMessage_init_zero, 0};

        COMMENT("Test for false positives with data after end of struct")
        ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        msg.bar = 0xFFFFFFFF;

        if (!pb_encode(&ostream, MyMessage_fields, &msg)) {
            fprintf(stderr, "Encode failed: %s\n", PB_GET_ERROR(&ostream));
            return 1;
        }

        printf("response payload (%d):", (int)ostream.bytes_written);
        for (i = 0; i < ostream.bytes_written; i++) {
            printf("%02X", buffer[i]);
        }
        printf("\n");

        TEST(ostream.bytes_written == 0);
    }

    return status;
}

