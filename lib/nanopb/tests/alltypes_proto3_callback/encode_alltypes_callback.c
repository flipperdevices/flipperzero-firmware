/* Attempts to test all the datatypes supported by ProtoBuf when used as callback fields.
 * Note that normally there would be no reason to use callback fields for this,
 * because each encoder defined here only gives a single field.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pb_encode.h>
#include "alltypes.pb.h"
#include "test_helpers.h"

static bool write_varint(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_varint(stream, (intptr_t)*arg);
}

static bool write_svarint(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_svarint(stream, (intptr_t)*arg);
}

static bool write_fixed32(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_fixed32(stream, *arg);
}

static bool write_fixed64(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_fixed64(stream, *arg);
}

static bool write_double(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
#ifdef PB_CONVERT_DOUBLE_FLOAT
    if (sizeof(double) == sizeof(float))
        return pb_encode_tag_for_field(stream, field) &&
               pb_encode_float_as_double(stream, *(float*)*arg);
#endif

    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_fixed64(stream, *arg);
}

static bool write_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, *arg, strlen(*arg));
}

static bool write_submsg(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
   
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, SubMessage_fields, *arg);
}

static bool write_emptymsg(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    EmptyMessage emptymsg = {0};
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, EmptyMessage_fields, &emptymsg);
}

static bool write_repeated_varint(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_varint(stream, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_varint(stream, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_varint(stream, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_varint(stream, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_varint(stream, (intptr_t)*arg);
}

static bool write_repeated_svarint(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_svarint(stream, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_svarint(stream, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_svarint(stream, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_svarint(stream, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_svarint(stream, (intptr_t)*arg);
}

static bool write_repeated_fixed32(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    uint32_t dummy = 0;

    /* Make it a packed field */
    return pb_encode_tag(stream, PB_WT_STRING, field->tag) &&
           pb_encode_varint(stream, 5 * 4) && /* Number of bytes */
           pb_encode_fixed32(stream, &dummy) &&
           pb_encode_fixed32(stream, &dummy) &&
           pb_encode_fixed32(stream, &dummy) &&
           pb_encode_fixed32(stream, &dummy) &&
           pb_encode_fixed32(stream, *arg);
}

static bool write_repeated_fixed64(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    uint64_t dummy = 0;

    /* Make it a packed field */
    return pb_encode_tag(stream, PB_WT_STRING, field->tag) &&
           pb_encode_varint(stream, 5 * 8) && /* Number of bytes */
           pb_encode_fixed64(stream, &dummy) &&
           pb_encode_fixed64(stream, &dummy) &&
           pb_encode_fixed64(stream, &dummy) &&
           pb_encode_fixed64(stream, &dummy) &&
           pb_encode_fixed64(stream, *arg);
}

static bool write_repeated_double(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    uint64_t dummy = 0;

#ifdef PB_CONVERT_DOUBLE_FLOAT
    if (sizeof(double) == sizeof(float))
        return pb_encode_tag(stream, PB_WT_STRING, field->tag) &&
               pb_encode_varint(stream, 5 * 8) && /* Number of bytes */
               pb_encode_float_as_double(stream, 0.0f) &&
               pb_encode_float_as_double(stream, 0.0f) &&
               pb_encode_float_as_double(stream, 0.0f) &&
               pb_encode_float_as_double(stream, 0.0f) &&
               pb_encode_float_as_double(stream, *(float*)*arg);
#endif

    /* Make it a packed field */
    return pb_encode_tag(stream, PB_WT_STRING, field->tag) &&
           pb_encode_varint(stream, 5 * 8) && /* Number of bytes */
           pb_encode_fixed64(stream, &dummy) &&
           pb_encode_fixed64(stream, &dummy) &&
           pb_encode_fixed64(stream, &dummy) &&
           pb_encode_fixed64(stream, &dummy) &&
           pb_encode_fixed64(stream, *arg);
}

static bool write_repeated_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, 0, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, 0, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, 0, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, 0, 0) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_string(stream, *arg, strlen(*arg));
}

static bool write_repeated_submsg(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    SubMessage dummy = {""};

    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, SubMessage_fields, &dummy) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, SubMessage_fields, &dummy) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, SubMessage_fields, &dummy) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, SubMessage_fields, &dummy) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, SubMessage_fields, *arg);
}

static bool write_limits(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    Limits limits = {0};
    limits.int32_min  = INT32_MIN;
    limits.int32_max  = INT32_MAX;
    limits.uint32_min = 0;
    limits.uint32_max = UINT32_MAX;
    limits.int64_min  = INT64_MIN;
    limits.int64_max  = INT64_MAX;
    limits.uint64_min = 0;
    limits.uint64_max = UINT64_MAX;
    limits.enum_min   = HugeEnum_Negative;
    limits.enum_max   = HugeEnum_Positive;
   
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, Limits_fields, &limits);
}

static bool write_repeated_emptymsg(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    EmptyMessage emptymsg = {0};
    return pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, EmptyMessage_fields, &emptymsg) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, EmptyMessage_fields, &emptymsg) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, EmptyMessage_fields, &emptymsg) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, EmptyMessage_fields, &emptymsg) &&
           pb_encode_tag_for_field(stream, field) &&
           pb_encode_submessage(stream, EmptyMessage_fields, &emptymsg);
}

int main(int argc, char **argv)
{
    int mode = (argc > 1) ? atoi(argv[1]) : 0;

    /* Values for use from callbacks through pointers. */
    uint32_t    rep_fixed32     = 2008;
    int32_t     rep_sfixed32    = -2009;
    float       rep_float       = 2010.0f;
    uint64_t    rep_fixed64     = 2011;
    int64_t     rep_sfixed64    = -2012;
    double      rep_double      = 2013.0;
    SubMessage  rep_submsg      = {"2016", 2016, 2016};
    
    uint32_t    sng_fixed32     = 3048;
    int32_t     sng_sfixed32    = 3049;
    float       sng_float       = 3050.0f;
    uint64_t    sng_fixed64     = 3051;
    int64_t     sng_sfixed64    = 3052;
    double      sng_double      = 3053.0f;
    SubMessage  sng_submsg      = {"3056", 3056};
    
    SubMessage  oneof_msg1      = {"4059", 4059};

    AllTypes alltypes = AllTypes_init_zero;
    
    /* Bind callbacks for repeated fields */
    alltypes.rep_int32.funcs.encode = &write_repeated_varint;
    alltypes.rep_int32.arg = (void*)-2001;
    
    alltypes.rep_int64.funcs.encode = &write_repeated_varint;
    alltypes.rep_int64.arg = (void*)-2002;
    
    alltypes.rep_uint32.funcs.encode = &write_repeated_varint;
    alltypes.rep_uint32.arg = (void*)2003;
    
    alltypes.rep_uint64.funcs.encode = &write_repeated_varint;
    alltypes.rep_uint64.arg = (void*)2004;
    
    alltypes.rep_sint32.funcs.encode = &write_repeated_svarint;
    alltypes.rep_sint32.arg = (void*)-2005;
    
    alltypes.rep_sint64.funcs.encode = &write_repeated_svarint;
    alltypes.rep_sint64.arg = (void*)-2006;
    
    alltypes.rep_bool.funcs.encode = &write_repeated_varint;
    alltypes.rep_bool.arg = (void*)true;
    
    alltypes.rep_fixed32.funcs.encode = &write_repeated_fixed32;
    alltypes.rep_fixed32.arg = &rep_fixed32;

    alltypes.rep_sfixed32.funcs.encode = &write_repeated_fixed32;
    alltypes.rep_sfixed32.arg = &rep_sfixed32;
    
    alltypes.rep_float.funcs.encode = &write_repeated_fixed32;
    alltypes.rep_float.arg = &rep_float;
    
    alltypes.rep_fixed64.funcs.encode = &write_repeated_fixed64;
    alltypes.rep_fixed64.arg = &rep_fixed64;

    alltypes.rep_sfixed64.funcs.encode = &write_repeated_fixed64;
    alltypes.rep_sfixed64.arg = &rep_sfixed64;
    
    alltypes.rep_double.funcs.encode = &write_repeated_double;
    alltypes.rep_double.arg = &rep_double;
    
    alltypes.rep_string.funcs.encode = &write_repeated_string;
    alltypes.rep_string.arg = "2014";
    
    alltypes.rep_bytes.funcs.encode = &write_repeated_string;
    alltypes.rep_bytes.arg = "2015";
    
    alltypes.rep_submsg.funcs.encode = &write_repeated_submsg;
    alltypes.rep_submsg.arg = &rep_submsg;
    
    alltypes.rep_enum.funcs.encode = &write_repeated_varint;
    alltypes.rep_enum.arg = (void*)MyEnum_Truth;
    
    alltypes.rep_emptymsg.funcs.encode = &write_repeated_emptymsg;
    
    alltypes.rep_fbytes.funcs.encode = &write_repeated_string;
    alltypes.rep_fbytes.arg = "2019";
    
    alltypes.req_limits.funcs.encode = &write_limits;
    
    /* Bind callbacks for singular fields */
    if (mode != 0)
    {
        alltypes.sng_int32.funcs.encode = &write_varint;
        alltypes.sng_int32.arg = (void*)3041;
        
        alltypes.sng_int64.funcs.encode = &write_varint;
        alltypes.sng_int64.arg = (void*)3042;
        
        alltypes.sng_uint32.funcs.encode = &write_varint;
        alltypes.sng_uint32.arg = (void*)3043;
        
        alltypes.sng_uint64.funcs.encode = &write_varint;
        alltypes.sng_uint64.arg = (void*)3044;
        
        alltypes.sng_sint32.funcs.encode = &write_svarint;
        alltypes.sng_sint32.arg = (void*)3045;
        
        alltypes.sng_sint64.funcs.encode = &write_svarint;
        alltypes.sng_sint64.arg = (void*)3046;
        
        alltypes.sng_bool.funcs.encode = &write_varint;
        alltypes.sng_bool.arg = (void*)true;

        alltypes.sng_fixed32.funcs.encode = &write_fixed32;
        alltypes.sng_fixed32.arg = &sng_fixed32;
        
        alltypes.sng_sfixed32.funcs.encode = &write_fixed32;
        alltypes.sng_sfixed32.arg = &sng_sfixed32;
        
        alltypes.sng_float.funcs.encode = &write_fixed32;
        alltypes.sng_float.arg = &sng_float;
        
        alltypes.sng_fixed64.funcs.encode = &write_fixed64;
        alltypes.sng_fixed64.arg = &sng_fixed64;
        
        alltypes.sng_sfixed64.funcs.encode = &write_fixed64;
        alltypes.sng_sfixed64.arg = &sng_sfixed64;
        
        alltypes.sng_double.funcs.encode = &write_double;
        alltypes.sng_double.arg = &sng_double;
        
        alltypes.sng_string.funcs.encode = &write_string;
        alltypes.sng_string.arg = "3054";
        
        alltypes.sng_bytes.funcs.encode = &write_string;
        alltypes.sng_bytes.arg = "3055";
        
        alltypes.sng_submsg.funcs.encode = &write_submsg;
        alltypes.sng_submsg.arg = &sng_submsg;
        
        alltypes.sng_enum.funcs.encode = &write_varint;
        alltypes.sng_enum.arg = (void*)MyEnum_Truth;
        
        alltypes.sng_emptymsg.funcs.encode = &write_emptymsg;

        alltypes.sng_fbytes.funcs.encode = &write_string;
        alltypes.sng_fbytes.arg = "3059";

        alltypes.oneof_msg1.funcs.encode = &write_submsg;
        alltypes.oneof_msg1.arg = &oneof_msg1;
    }
    
    alltypes.end.funcs.encode = &write_varint;
    alltypes.end.arg = (void*)1099;
    
    {
        uint8_t buffer[2048];
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
