#define PB_VALIDATE_UTF8
#include "pb_common.c"

#include <stdio.h>
#include <string.h>
#include "unittests.h"
#include "../alltypes/alltypes.pb.h"

int main()
{
    int status = 0;

    {
        AllTypes msg;
        pb_field_iter_t iter;

        COMMENT("Test field iterator logic on AllTypes");

        TEST(pb_field_iter_begin(&iter, AllTypes_fields, &msg))

        TEST(iter.tag == 1  && iter.pData == &msg.req_int32 && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 2  && iter.pData == &msg.req_int64    && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 3  && iter.pData == &msg.req_uint32   && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 4  && iter.pData == &msg.req_uint64   && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 5  && iter.pData == &msg.req_sint32   && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 6  && iter.pData == &msg.req_sint64   && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 7  && iter.pData == &msg.req_bool     && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 8  && iter.pData == &msg.req_fixed32  && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 9  && iter.pData == &msg.req_sfixed32 && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 10 && iter.pData == &msg.req_float    && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 11 && iter.pData == &msg.req_fixed64  && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 12 && iter.pData == &msg.req_sfixed64 && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 13 && iter.pData == &msg.req_double   && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 14 && iter.pData == &msg.req_string   && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 15 && iter.pData == &msg.req_bytes    && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 16 && iter.pData == &msg.req_submsg   && !iter.pSize
             && iter.submsg_desc == SubMessage_fields)
        TEST(pb_field_iter_next(&iter) && iter.tag == 17 && iter.pData == &msg.req_enum     && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 18 && iter.pData == &msg.req_emptymsg && !iter.pSize)
        TEST(pb_field_iter_next(&iter) && iter.tag == 19 && iter.pData == &msg.req_fbytes   && !iter.pSize)
        TEST(iter.required_field_index == 18)
        TEST(iter.submessage_index == 2)

        TEST(pb_field_iter_next(&iter) && iter.tag == 21 && iter.pData == &msg.rep_int32    && iter.pSize == &msg.rep_int32_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 22 && iter.pData == &msg.rep_int64    && iter.pSize == &msg.rep_int64_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 23 && iter.pData == &msg.rep_uint32   && iter.pSize == &msg.rep_uint32_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 24 && iter.pData == &msg.rep_uint64   && iter.pSize == &msg.rep_uint64_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 25 && iter.pData == &msg.rep_sint32   && iter.pSize == &msg.rep_sint32_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 26 && iter.pData == &msg.rep_sint64   && iter.pSize == &msg.rep_sint64_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 27 && iter.pData == &msg.rep_bool     && iter.pSize == &msg.rep_bool_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 28 && iter.pData == &msg.rep_fixed32  && iter.pSize == &msg.rep_fixed32_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 29 && iter.pData == &msg.rep_sfixed32 && iter.pSize == &msg.rep_sfixed32_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 30 && iter.pData == &msg.rep_float    && iter.pSize == &msg.rep_float_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 31 && iter.pData == &msg.rep_fixed64  && iter.pSize == &msg.rep_fixed64_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 32 && iter.pData == &msg.rep_sfixed64 && iter.pSize == &msg.rep_sfixed64_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 33 && iter.pData == &msg.rep_double   && iter.pSize == &msg.rep_double_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 34 && iter.pData == &msg.rep_string   && iter.pSize == &msg.rep_string_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 35 && iter.pData == &msg.rep_bytes    && iter.pSize == &msg.rep_bytes_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 36 && iter.pData == &msg.rep_submsg   && iter.pSize == &msg.rep_submsg_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 37 && iter.pData == &msg.rep_enum     && iter.pSize == &msg.rep_enum_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 38 && iter.pData == &msg.rep_emptymsg && iter.pSize == &msg.rep_emptymsg_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 39 && iter.pData == &msg.rep_fbytes   && iter.pSize == &msg.rep_fbytes_count)
        TEST(pb_field_iter_next(&iter) && iter.tag == 40 && iter.pData == &msg.rep_farray   && iter.pSize == &iter.array_size && iter.array_size == 5)
        TEST(iter.required_field_index == 19)
        TEST(iter.submessage_index == 4)

        TEST(pb_field_iter_next(&iter) && iter.tag == 41 && iter.pData == &msg.opt_int32    && iter.pSize == &msg.has_opt_int32   )
        TEST(pb_field_iter_next(&iter) && iter.tag == 42 && iter.pData == &msg.opt_int64    && iter.pSize == &msg.has_opt_int64   )
        TEST(pb_field_iter_next(&iter) && iter.tag == 43 && iter.pData == &msg.opt_uint32   && iter.pSize == &msg.has_opt_uint32  )
        TEST(pb_field_iter_next(&iter) && iter.tag == 44 && iter.pData == &msg.opt_uint64   && iter.pSize == &msg.has_opt_uint64  )
        TEST(pb_field_iter_next(&iter) && iter.tag == 45 && iter.pData == &msg.opt_sint32   && iter.pSize == &msg.has_opt_sint32  )
        TEST(pb_field_iter_next(&iter) && iter.tag == 46 && iter.pData == &msg.opt_sint64   && iter.pSize == &msg.has_opt_sint64  )
        TEST(pb_field_iter_next(&iter) && iter.tag == 47 && iter.pData == &msg.opt_bool     && iter.pSize == &msg.has_opt_bool    )
        TEST(pb_field_iter_next(&iter) && iter.tag == 48 && iter.pData == &msg.opt_fixed32  && iter.pSize == &msg.has_opt_fixed32 )
        TEST(pb_field_iter_next(&iter) && iter.tag == 49 && iter.pData == &msg.opt_sfixed32 && iter.pSize == &msg.has_opt_sfixed32)
        TEST(pb_field_iter_next(&iter) && iter.tag == 50 && iter.pData == &msg.opt_float    && iter.pSize == &msg.has_opt_float   )
        TEST(pb_field_iter_next(&iter) && iter.tag == 51 && iter.pData == &msg.opt_fixed64  && iter.pSize == &msg.has_opt_fixed64 )
        TEST(pb_field_iter_next(&iter) && iter.tag == 52 && iter.pData == &msg.opt_sfixed64 && iter.pSize == &msg.has_opt_sfixed64)
        TEST(pb_field_iter_next(&iter) && iter.tag == 53 && iter.pData == &msg.opt_double   && iter.pSize == &msg.has_opt_double  )
        TEST(pb_field_iter_next(&iter) && iter.tag == 54 && iter.pData == &msg.opt_string   && iter.pSize == &msg.has_opt_string  )
        TEST(pb_field_iter_next(&iter) && iter.tag == 55 && iter.pData == &msg.opt_bytes    && iter.pSize == &msg.has_opt_bytes   )
        TEST(pb_field_iter_next(&iter) && iter.tag == 56 && iter.pData == &msg.opt_submsg   && iter.pSize == &msg.has_opt_submsg  )
        TEST(pb_field_iter_next(&iter) && iter.tag == 57 && iter.pData == &msg.opt_enum     && iter.pSize == &msg.has_opt_enum    )
        TEST(pb_field_iter_next(&iter) && iter.tag == 58 && iter.pData == &msg.opt_emptymsg && iter.pSize == &msg.has_opt_emptymsg)
        TEST(pb_field_iter_next(&iter) && iter.tag == 59 && iter.pData == &msg.opt_fbytes   && iter.pSize == &msg.has_opt_fbytes)
        TEST(iter.required_field_index == 19)
        TEST(iter.submessage_index == 6)

        TEST(pb_field_iter_next(&iter) && iter.tag == 60 && iter.pData == &msg.oneof.oneof_msg1  && iter.pSize == &msg.which_oneof   )
        TEST(pb_field_iter_next(&iter) && iter.tag == 61 && iter.pData == &msg.oneof.oneof_msg2  && iter.pSize == &msg.which_oneof   )
        TEST(pb_field_iter_next(&iter) && iter.tag == 62 && iter.pData == &msg.opt_non_zero_based_enum && iter.pSize == &msg.has_opt_non_zero_based_enum)
        TEST(pb_field_iter_next(&iter) && iter.tag == 63 && iter.pData == &msg.oneof.static_msg  && iter.pSize == &msg.which_oneof   )
        TEST(iter.required_field_index == 19)
        TEST(iter.submessage_index == 8)

        TEST(pb_field_iter_next(&iter) && iter.tag == 95 && iter.pData == &msg.rep_farray2 && iter.pSize == &iter.array_size && iter.array_size == 3)
        TEST(iter.required_field_index == 19)
        TEST(iter.submessage_index == 9)

        TEST(pb_field_iter_next(&iter) && iter.tag == 96 && iter.pData == &msg.req_intsizes && !iter.pSize)
        TEST(iter.required_field_index == 19)
        TEST(iter.submessage_index == 9)

        TEST(pb_field_iter_next(&iter) && iter.tag == 97 && iter.pData == &msg.req_ds8 && !iter.pSize)
        TEST(iter.required_field_index == 20)
        TEST(iter.submessage_index == 10)

        TEST(pb_field_iter_next(&iter) && iter.tag == 98 && iter.pData == &msg.req_limits && !iter.pSize)
        TEST(iter.required_field_index == 21)
        TEST(iter.submessage_index == 11)

        TEST(pb_field_iter_next(&iter) && iter.tag == 200 && iter.pData == &msg.extensions && !iter.pSize)
        TEST(iter.required_field_index == 22)
        TEST(iter.submessage_index == 12)

        TEST(pb_field_iter_next(&iter) && iter.tag == 999 && iter.pData == &msg.end && !iter.pSize)
        TEST(iter.required_field_index == 22)
        TEST(iter.submessage_index == 12)

        TEST(!pb_field_iter_next(&iter))

        TEST(iter.tag == 1  && iter.pData == &msg.req_int32 && !iter.pSize)
        TEST(iter.required_field_index == 0)
        TEST(iter.submessage_index == 0)
    }

    {
        COMMENT("Test pb_validate_utf8()");

        TEST(pb_validate_utf8("abcdefg"));
        TEST(pb_validate_utf8("\xc3\xa4\xc3\xa4\x6b\x6b\xc3\xb6\x6e\x65\x6e\x0a"));
        TEST(!pb_validate_utf8("\xc3\xa4\xc3\xa4\x6b\x6b\xb6\xc3\x6e\x65\x6e\x0a"));
        TEST(pb_validate_utf8("\xed\x9f\xbf"));
        TEST(pb_validate_utf8("\xee\x80\x80"));
        TEST(pb_validate_utf8("\xef\xbf\xbd"));
        TEST(pb_validate_utf8("\xf4\x8f\xbf\xbf"));
        TEST(!pb_validate_utf8("a\x80z"));
        TEST(!pb_validate_utf8("a\xbfz"));
        TEST(!pb_validate_utf8("a\xfez"));
        TEST(!pb_validate_utf8("a\xffz"));
        TEST(!pb_validate_utf8("a\xc0\xafz"));
        TEST(!pb_validate_utf8("a\xef\xbf\xbez"));
    }

    if (status != 0)
        fprintf(stdout, "\n\nSome tests FAILED!\n");

    return status;
}
