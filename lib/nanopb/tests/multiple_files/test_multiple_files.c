/*
 * Tests if this still compiles when multiple .proto files are involved.
 */

#include <stdio.h>
#include <pb_encode.h>
#include <pb_common.h>
#include "unittests.h"
#include "multifile2.pb.h"
#include "subdir/multifile2.pb.h"

int main()
{
    int status = 0;

    /* Test that included file options are properly loaded */
    TEST(OneofMessage_size == 27);

    /* Check that enum signedness is detected properly */
    {
        pb_field_iter_t iter;
        Enums msg;
        TEST(pb_field_iter_begin(&iter, Enums_fields, &msg));
        TEST(PB_LTYPE(iter.type) == PB_LTYPE_VARINT);
        TEST(pb_field_iter_next(&iter));
        TEST(PB_LTYPE(iter.type) == PB_LTYPE_UVARINT);
    }

    /* Test that subdir file is correctly included */
    {
        subdir_SubdirMessage foo = subdir_SubdirMessage_init_default;
        TEST(foo.foo == 15);
        TEST(subdir_OneofMessage_size >= 27); /* Note: not perfectly accurate due to issue 172 */
        TEST(subdir_OneofMessage_size <= 30);
    }

    return status;
}
