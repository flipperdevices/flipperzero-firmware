#include <assert.h>
#include <pb_encode.h>
#include <string.h>
#include <stdio.h>
#include "test_helpers.h"
#include "anytest.pb.h"
#include "google/protobuf/duration.pb.h"

int main()
{
    BaseMessage msg = BaseMessage_init_zero;
    google_protobuf_Duration duration = google_protobuf_Duration_init_zero;
    uint8_t buffer[256];
    pb_ostream_t stream;
    bool status;
    
    msg.start = 1234;
    msg.end = 5678;

    /* Fill in the Any message header */
    msg.has_details = true;
    strncpy(msg.details.type_url, "type.googleapis.com/google.protobuf.Duration", sizeof(msg.details.type_url));
    
    /* Encode a Duration message inside the Any message. */
    duration.seconds = 99999;
    duration.nanos = 100;
    stream = pb_ostream_from_buffer(msg.details.value.bytes, sizeof(msg.details.value.bytes));
    status = pb_encode(&stream, google_protobuf_Duration_fields, &duration);
    assert(status);
    msg.details.value.size = stream.bytes_written;
    
    /* Then encode the outer message */
    stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    if (pb_encode(&stream, BaseMessage_fields, &msg))
    {    
        /* Write the result data to stdout */
        SET_BINARY_MODE(stdout);
        fwrite(buffer, 1, stream.bytes_written, stdout);
        return 0;
    }
    else
    {
        fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }
}
