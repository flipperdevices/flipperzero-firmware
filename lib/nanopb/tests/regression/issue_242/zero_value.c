#include <unittests.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <string.h>
#include "zero_value.pb.h"

int main()
{
    int status = 0;
    
    COMMENT("Test extension fields with zero values");
    {
        uint8_t buffer[256] = {0};
        pb_ostream_t ostream;
        int32_t value = 0;
        Extendable source = {0};

        pb_extension_t source_ext = {0};
        source_ext.type = &opt_int32;
        source_ext.dest = &value;
        source.extensions = &source_ext;

        ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        TEST(pb_encode(&ostream, Extendable_fields, &source));

        TEST(ostream.bytes_written == 2);
        TEST(memcmp(buffer, "\x58\x00", 2) == 0);
    }
    
    /* Note: There never was a bug here, but this check is included
     * in the regression test because the logic is closely related.
     */
    COMMENT("Test pointer fields with zero values");
    {
        uint8_t buffer[256] = {0};
        pb_ostream_t ostream;
        int32_t value = 0;
        PointerMessage source = {0};

        source.opt_int32 = &value;

        ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        TEST(pb_encode(&ostream, PointerMessage_fields, &source));

        TEST(ostream.bytes_written == 2);
        TEST(memcmp(buffer, "\x58\x00", 2) == 0);
    }    
    
    return status;
}

