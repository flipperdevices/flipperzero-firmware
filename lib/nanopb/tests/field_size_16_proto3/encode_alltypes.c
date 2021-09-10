/* Attempts to test all the datatypes supported by ProtoBuf3.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pb_encode.h>
#include "alltypes.pb.h"
#include "test_helpers.h"

int main(int argc, char **argv)
{
    int mode = (argc > 1) ? atoi(argv[1]) : 0;
    
    /* Initialize the structure with constants */
    AllTypes alltypes = AllTypes_init_zero;
    
    alltypes.rep_int32_count = 5; alltypes.rep_int32[4] = -2001;
    alltypes.rep_int64_count = 5; alltypes.rep_int64[4] = -2002;
    alltypes.rep_uint32_count = 5; alltypes.rep_uint32[4] = 2003;
    alltypes.rep_uint64_count = 5; alltypes.rep_uint64[4] = 2004;
    alltypes.rep_sint32_count = 5; alltypes.rep_sint32[4] = -2005;
    alltypes.rep_sint64_count = 5; alltypes.rep_sint64[4] = -2006;
    alltypes.rep_bool_count = 5; alltypes.rep_bool[4] = true;
    
    alltypes.rep_fixed32_count = 5; alltypes.rep_fixed32[4] = 2008;
    alltypes.rep_sfixed32_count = 5; alltypes.rep_sfixed32[4] = -2009;
    alltypes.rep_float_count = 5; alltypes.rep_float[4] = 2010.0f;
    
    alltypes.rep_fixed64_count = 5; alltypes.rep_fixed64[4] = 2011;
    alltypes.rep_sfixed64_count = 5; alltypes.rep_sfixed64[4] = -2012;
    alltypes.rep_double_count = 5; alltypes.rep_double[4] = 2013.0;
    
    alltypes.rep_string_count = 5; strcpy(alltypes.rep_string[4], "2014");
    alltypes.rep_bytes_count = 5; alltypes.rep_bytes[4].size = 4;
    memcpy(alltypes.rep_bytes[4].bytes, "2015", 4);

    alltypes.rep_submsg_count = 5;
    strcpy(alltypes.rep_submsg[4].substuff1, "2016");
    alltypes.rep_submsg[4].substuff2 = 2016;
    alltypes.rep_submsg[4].substuff3 = 2016;
    
    alltypes.rep_enum_count = 5; alltypes.rep_enum[4] = MyEnum_Truth;
    alltypes.rep_emptymsg_count = 5;
    
    alltypes.rep_fbytes_count = 5;
    memcpy(alltypes.rep_fbytes[4], "2019", 4);
    
    alltypes.req_limits.int32_min  = INT32_MIN;
    alltypes.req_limits.int32_max  = INT32_MAX;
    alltypes.req_limits.uint32_min = 0;
    alltypes.req_limits.uint32_max = UINT32_MAX;
    alltypes.req_limits.int64_min  = INT64_MIN;
    alltypes.req_limits.int64_max  = INT64_MAX;
    alltypes.req_limits.uint64_min = 0;
    alltypes.req_limits.uint64_max = UINT64_MAX;
    alltypes.req_limits.enum_min   = HugeEnum_Negative;
    alltypes.req_limits.enum_max   = HugeEnum_Positive;
    
    if (mode != 0)
    {
        /* Fill in values for singular fields */
        alltypes.sng_int32         = 3041;
        alltypes.sng_int64         = 3042;
        alltypes.sng_uint32        = 3043;
        alltypes.sng_uint64        = 3044;
        alltypes.sng_sint32        = 3045;
        alltypes.sng_sint64        = 3046;
        alltypes.sng_bool          = true;
        
        alltypes.sng_fixed32       = 3048;
        alltypes.sng_sfixed32      = 3049;
        alltypes.sng_float         = 3050.0f;
        
        alltypes.sng_fixed64       = 3051;
        alltypes.sng_sfixed64      = 3052;
        alltypes.sng_double        = 3053.0;
        
        strcpy(alltypes.sng_string, "3054");
        alltypes.sng_bytes.size = 4;
        memcpy(alltypes.sng_bytes.bytes, "3055", 4);
        alltypes.has_sng_submsg = true;
        strcpy(alltypes.sng_submsg.substuff1, "3056");
        alltypes.sng_submsg.substuff2 = 3056;
        alltypes.sng_enum = MyEnum_Truth;
        memcpy(alltypes.sng_fbytes, "3059", 4);

        alltypes.which_oneof = AllTypes_oneof_msg1_tag;
        strcpy(alltypes.oneof.oneof_msg1.substuff1, "4059");
        alltypes.oneof.oneof_msg1.substuff2 = 4059;
    }
    
    alltypes.end = 1099;
    
    {
        uint8_t buffer[AllTypes_size];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        
        /* Now encode it and check if we succeeded. */
        if (pb_encode(&stream, AllTypes_fields, &alltypes))
        {
            SET_BINARY_MODE(stdout);
            fwrite(buffer, 1, stream.bytes_written, stdout);
            return 0; /* Success */
        }
        else
        {
            fprintf(stderr, "Encoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1; /* Failure */
        }
    }
}
