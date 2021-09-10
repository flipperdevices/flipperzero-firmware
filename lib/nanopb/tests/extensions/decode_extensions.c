/* Test decoding of extension fields. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pb_decode.h>
#include "alltypes.pb.h"
#include "extensions.pb.h"
#include "test_helpers.h"
#include "unittests.h"

int main(int argc, char **argv)
{
    uint8_t buffer[1024];
    size_t count;
    pb_istream_t stream;
    int status = 0;
    
    AllTypes alltypes = AllTypes_init_zero;
    int32_t extensionfield1;
    pb_extension_t ext1 = pb_extension_init_zero;
    ExtensionMessage extensionfield2 = ExtensionMessage_init_zero;
    pb_extension_t ext2 = pb_extension_init_zero;
    
    /* Read the message data */
    SET_BINARY_MODE(stdin);
    count = fread(buffer, 1, sizeof(buffer), stdin);
    stream = pb_istream_from_buffer(buffer, count);
    
    /* Add the extensions */
    alltypes.extensions = &ext1;        

    ext1.type = &AllTypes_extensionfield1;
    ext1.dest = &extensionfield1;
    ext1.next = &ext2;
    
    ext2.type = &ExtensionMessage_AllTypes_extensionfield2;
    ext2.dest = &extensionfield2;
    ext2.next = NULL;
    
    /* Decode the message */
    if (!pb_decode(&stream, AllTypes_fields, &alltypes))
    {
        printf("Parsing failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    /* Check that the extensions decoded properly */
    TEST(ext1.found)
    TEST(extensionfield1 == 12345)
    TEST(ext2.found)
    TEST(strcmp(extensionfield2.test1, "test") == 0)
    TEST(extensionfield2.test2 == 54321)
    
    return status;
}

