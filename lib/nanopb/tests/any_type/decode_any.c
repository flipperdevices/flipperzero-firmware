#include <assert.h>
#include <pb_decode.h>
#include <string.h>
#include <stdio.h>
#include "test_helpers.h"
#include "anytest.pb.h"
#include "google/protobuf/duration.pb.h"

int main()
{
    BaseMessage msg = BaseMessage_init_zero;
    uint8_t buffer[256];
    pb_istream_t stream;
    size_t count;
    bool status;
    
    /* Read the data into buffer */
    SET_BINARY_MODE(stdin);
    count = fread(buffer, 1, sizeof(buffer), stdin);
    stream = pb_istream_from_buffer(buffer, count);
    
    /* Decode the base message */
    if (!pb_decode(&stream, BaseMessage_fields, &msg))
    {
        printf("Parsing failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    assert(msg.start == 1234);
    assert(msg.end == 5678);

    /* Decode the Any message if we know the type */
    if (strcmp(msg.details.type_url, "type.googleapis.com/google.protobuf.Duration") == 0)
    {
        google_protobuf_Duration duration = google_protobuf_Duration_init_zero;
        stream = pb_istream_from_buffer(msg.details.value.bytes, msg.details.value.size);
        status = pb_decode(&stream, google_protobuf_Duration_fields, &duration);
        assert(status);
        assert(duration.seconds == 99999);
        assert(duration.nanos == 100);
        return 0;
    }
    else
    {
        fprintf(stderr, "Unknown Any type\n");
        return 2;
    }
}

