/* Decode a message using map field */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pb_decode.h>
#include "map.pb.h"
#include "test_helpers.h"
#include "unittests.h"

/* Helper function to find an entry in the list. Not as efficient as a real
 * hashmap or similar would be, but suitable for small arrays. */
MyMessage_NumbersEntry *find_entry(MyMessage *msg, const char *key)
{
    int i;
    for (i = 0; i < msg->numbers_count; i++)
    {
        if (strcmp(msg->numbers[i].key, key) == 0)
        {
            return &msg->numbers[i];
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    uint8_t buffer[MyMessage_size];
    size_t count;
    
    SET_BINARY_MODE(stdin);
    count = fread(buffer, 1, sizeof(buffer), stdin);

    if (!feof(stdin))
    {
        printf("Message does not fit in buffer\n");
        return 1;
    }

    {
        int status = 0;
        MyMessage msg = MyMessage_init_zero;
        MyMessage_NumbersEntry *e;
        pb_istream_t stream = pb_istream_from_buffer(buffer, count);
        
        if (!pb_decode(&stream, MyMessage_fields, &msg))
        {
            fprintf(stderr, "Decoding failed\n");     
            return 2;   
        }
        
        TEST((e = find_entry(&msg, "one")) && e->value == 1);
        TEST((e = find_entry(&msg, "two")) && e->value == 2);
        TEST((e = find_entry(&msg, "seven")) && e->value == 7);
        TEST(!find_entry(&msg, "zero"));
        
        return status;
    }
}

