/* Attempts to test all the datatypes supported by ProtoBuf when used as callback fields.
 * Note that normally there would be no reason to use callback fields for this,
 * because each encoder defined here only gives a single field.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pb_decode.h>
#include "alltypes.pb.h"
#include "test_helpers.h"

#define TEST(x) if (!(x)) { \
    printf("Test %s failed (in field %d).\n", #x, field->tag); \
    return false; \
    }

static bool read_varint(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint64_t value;
    if (!pb_decode_varint(stream, &value))
        return false;
    
    TEST((int64_t)value == (intptr_t)*arg);
    return true;
}

static bool read_svarint(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    int64_t value;
    if (!pb_decode_svarint(stream, &value))
        return false;
    
    TEST(value == (intptr_t)*arg);
    return true;
}

static bool read_fixed32(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint32_t value;
    if (!pb_decode_fixed32(stream, &value))
        return false;
    
    TEST(value == *(uint32_t*)*arg);
    return true;
}

static bool read_fixed64(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint64_t value;
    if (!pb_decode_fixed64(stream, &value))
        return false;
    
    TEST(value == *(uint64_t*)*arg);
    return true;
}

static bool read_double(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
#ifdef PB_CONVERT_DOUBLE_FLOAT
    if (sizeof(double) == sizeof(float))
    {
        float value;
        if (!pb_decode_double_as_float(stream, &value))
            return false;
        
        TEST(memcmp(&value, *arg, sizeof(float)) == 0);
        return true;
    }
#endif

    uint64_t value;
    if (!pb_decode_fixed64(stream, &value))
        return false;
    
    TEST(value == *(uint64_t*)*arg);
    return true;
}

static bool read_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint8_t buf[16] = {0};
    size_t len = stream->bytes_left;
    
    if (len > sizeof(buf) - 1 || !pb_read(stream, buf, len))
        return false;
    
    TEST(strcmp((char*)buf, *arg) == 0);
    return true;
}

static bool read_submsg(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    SubMessage submsg = {""};
    SubMessage *ref = *arg;
    
    if (!pb_decode(stream, SubMessage_fields, &submsg))
        return false;
    
    TEST(strcmp(submsg.substuff1, ref->substuff1) == 0);
    TEST(submsg.substuff2 == ref->substuff2);
    TEST(submsg.substuff3 == ref->substuff3); 
    return true;
}

static bool read_emptymsg(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    EmptyMessage emptymsg = {0};
    return pb_decode(stream, EmptyMessage_fields, &emptymsg);
}

static bool read_repeated_varint(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    int32_t** expected = (int32_t**)arg;
    uint64_t value;
    if (!pb_decode_varint(stream, &value))
        return false;

    TEST(*(*expected)++ == value);
    return true;
}

static bool read_repeated_svarint(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    int32_t** expected = (int32_t**)arg;
    int64_t value;
    if (!pb_decode_svarint(stream, &value))
        return false;

    TEST(*(*expected)++ == value);
    return true;
}

static bool read_repeated_fixed32(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint32_t** expected = (uint32_t**)arg;
    uint32_t value;
    if (!pb_decode_fixed32(stream, &value))
        return false;

    TEST(*(*expected)++ == value);
    return true;
}

static bool read_repeated_fixed64(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint64_t** expected = (uint64_t**)arg;
    uint64_t value;
    if (!pb_decode_fixed64(stream, &value))
        return false;

    TEST(*(*expected)++ == value);
    return true;
}

static bool read_repeated_double(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
#ifdef PB_CONVERT_DOUBLE_FLOAT
    if (sizeof(double) == sizeof(float))
    {
        float** expectedf = (float**)arg;
        float value;
        if (!pb_decode_double_as_float(stream, &value))
            return false;
        
        TEST(memcmp(&value, (*expectedf)++, sizeof(float)) == 0);
        return true;
    }
#endif

    uint64_t** expected = (uint64_t**)arg;
    uint64_t value;
    if (!pb_decode_fixed64(stream, &value))
        return false;

    TEST(*(*expected)++ == value);
    return true;
}

static bool read_repeated_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint8_t*** expected = (uint8_t***)arg;
    uint8_t buf[16] = {0};
    size_t len = stream->bytes_left;
    
    if (len > sizeof(buf) - 1 || !pb_read(stream, buf, len))
        return false;
    
    TEST(strcmp((char*)*(*expected)++, (char*)buf) == 0);
    return true;
}

static bool read_repeated_submsg(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    SubMessage** expected = (SubMessage**)arg;
    SubMessage submsg = {""};
    if (!pb_decode(stream, SubMessage_fields, &submsg))
        return false;

    TEST(strcmp(submsg.substuff1, (*expected)->substuff1) == 0);
    TEST(submsg.substuff2 == (*expected)->substuff2);
    TEST(submsg.substuff3 == (*expected)->substuff3);
    (*expected)++;

    return true;
}

static bool read_limits(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    Limits decoded = {0};
    if (!pb_decode(stream, Limits_fields, &decoded))
        return false;

    TEST(decoded.int32_min  == INT32_MIN);
    TEST(decoded.int32_max  == INT32_MAX);
    TEST(decoded.uint32_min == 0);
    TEST(decoded.uint32_max == UINT32_MAX);
    TEST(decoded.int64_min  == INT64_MIN);
    TEST(decoded.int64_max  == INT64_MAX);
    TEST(decoded.uint64_min == 0);
    TEST(decoded.uint64_max == UINT64_MAX);
    TEST(decoded.enum_min   == HugeEnum_Negative);
    TEST(decoded.enum_max   == HugeEnum_Positive);
    
    return true;
}

/* This function is called once from main(), it handles
   the decoding and checks the fields. */
bool check_alltypes(pb_istream_t *stream, int mode)
{
    /* Values for use from callbacks through pointers. */
    bool status;
    
    int32_t     rep_int32[5]    = {0, 0, 0, 0, -2001};
    int32_t     rep_int64[5]    = {0, 0, 0, 0, -2002};
    int32_t     rep_uint32[5]   = {0, 0, 0, 0,  2003};
    int32_t     rep_uint64[5]   = {0, 0, 0, 0,  2004};
    int32_t     rep_sint32[5]   = {0, 0, 0, 0, -2005};
    int32_t     rep_sint64[5]   = {0, 0, 0, 0, -2006};
    int32_t     rep_bool[5]     = {false, false, false, false, true};
    uint32_t    rep_fixed32[5]  = {0, 0, 0, 0,  2008};
    int32_t     rep_sfixed32[5] = {0, 0, 0, 0, -2009};
    float       rep_float[5]    = {0, 0, 0, 0,  2010.0f};
    uint64_t    rep_fixed64[5]  = {0, 0, 0, 0,  2011};
    int64_t     rep_sfixed64[5] = {0, 0, 0, 0, -2012};
    double      rep_double[5]   = {0, 0, 0, 0,  2013.0};
    char*       rep_string[5]   = {"", "", "", "", "2014"};
    char*       rep_bytes[5]    = {"", "", "", "", "2015"};
    SubMessage  rep_submsg[5]   = {{"", 0, 0},
                                   {"", 0, 0},
                                   {"", 0, 0},
                                   {"", 0, 0},
                                   {"2016", 2016, 2016}};
    int32_t     rep_enum[5]     = {0, 0, 0, 0, MyEnum_Truth};
    
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
    alltypes.rep_int32.funcs.decode = &read_repeated_varint;
    alltypes.rep_int32.arg = rep_int32;
    
    alltypes.rep_int64.funcs.decode = &read_repeated_varint;
    alltypes.rep_int64.arg = rep_int64;
    
    alltypes.rep_uint32.funcs.decode = &read_repeated_varint;
    alltypes.rep_uint32.arg = rep_uint32;
    
    alltypes.rep_uint64.funcs.decode = &read_repeated_varint;
    alltypes.rep_uint64.arg = rep_uint64;
    
    alltypes.rep_sint32.funcs.decode = &read_repeated_svarint;
    alltypes.rep_sint32.arg = rep_sint32;
    
    alltypes.rep_sint64.funcs.decode = &read_repeated_svarint;
    alltypes.rep_sint64.arg = rep_sint64;
    
    alltypes.rep_bool.funcs.decode = &read_repeated_varint;
    alltypes.rep_bool.arg = rep_bool;
    
    alltypes.rep_fixed32.funcs.decode = &read_repeated_fixed32;
    alltypes.rep_fixed32.arg = rep_fixed32;
    
    alltypes.rep_sfixed32.funcs.decode = &read_repeated_fixed32;
    alltypes.rep_sfixed32.arg = rep_sfixed32;
    
    alltypes.rep_float.funcs.decode = &read_repeated_fixed32;
    alltypes.rep_float.arg = rep_float;
    
    alltypes.rep_fixed64.funcs.decode = &read_repeated_fixed64;
    alltypes.rep_fixed64.arg = rep_fixed64;
    
    alltypes.rep_sfixed64.funcs.decode = &read_repeated_fixed64;
    alltypes.rep_sfixed64.arg = rep_sfixed64;
    
    alltypes.rep_double.funcs.decode = &read_repeated_double;
    alltypes.rep_double.arg = rep_double;
    
    alltypes.rep_string.funcs.decode = &read_repeated_string;
    alltypes.rep_string.arg = rep_string;
    
    alltypes.rep_bytes.funcs.decode = &read_repeated_string;
    alltypes.rep_bytes.arg = rep_bytes;
    
    alltypes.rep_submsg.funcs.decode = &read_repeated_submsg;
    alltypes.rep_submsg.arg = rep_submsg;
    
    alltypes.rep_enum.funcs.decode = &read_repeated_varint;
    alltypes.rep_enum.arg = rep_enum;
    
    alltypes.rep_emptymsg.funcs.decode = &read_emptymsg;
    
    alltypes.req_limits.funcs.decode = &read_limits;
    
    alltypes.end.funcs.decode = &read_varint;
    alltypes.end.arg = (void*)1099;
    
    /* Bind callbacks for optional fields */
    if (mode == 1)
    {
        alltypes.sng_int32.funcs.decode = &read_varint;
        alltypes.sng_int32.arg = (void*)3041;
        
        alltypes.sng_int64.funcs.decode = &read_varint;
        alltypes.sng_int64.arg = (void*)3042;
        
        alltypes.sng_uint32.funcs.decode = &read_varint;
        alltypes.sng_uint32.arg = (void*)3043;
        
        alltypes.sng_uint64.funcs.decode = &read_varint;
        alltypes.sng_uint64.arg = (void*)3044;
        
        alltypes.sng_sint32.funcs.decode = &read_svarint;
        alltypes.sng_sint32.arg = (void*)3045;
        
        alltypes.sng_sint64.funcs.decode = &read_svarint;
        alltypes.sng_sint64.arg = (void*)3046;
        
        alltypes.sng_bool.funcs.decode = &read_varint;
        alltypes.sng_bool.arg = (void*)true;

        alltypes.sng_fixed32.funcs.decode = &read_fixed32;
        alltypes.sng_fixed32.arg = &sng_fixed32;
        
        alltypes.sng_sfixed32.funcs.decode = &read_fixed32;
        alltypes.sng_sfixed32.arg = &sng_sfixed32;
        
        alltypes.sng_float.funcs.decode = &read_fixed32;
        alltypes.sng_float.arg = &sng_float;
        
        alltypes.sng_fixed64.funcs.decode = &read_fixed64;
        alltypes.sng_fixed64.arg = &sng_fixed64;
        
        alltypes.sng_sfixed64.funcs.decode = &read_fixed64;
        alltypes.sng_sfixed64.arg = &sng_sfixed64;
        
        alltypes.sng_double.funcs.decode = &read_double;
        alltypes.sng_double.arg = &sng_double;
        
        alltypes.sng_string.funcs.decode = &read_string;
        alltypes.sng_string.arg = "3054";
        
        alltypes.sng_bytes.funcs.decode = &read_string;
        alltypes.sng_bytes.arg = "3055";
        
        alltypes.sng_submsg.funcs.decode = &read_submsg;
        alltypes.sng_submsg.arg = &sng_submsg;
        
        alltypes.sng_enum.funcs.decode = &read_varint;
        alltypes.sng_enum.arg = (void*)MyEnum_Truth;
        
        alltypes.sng_emptymsg.funcs.decode = &read_emptymsg;

        alltypes.oneof_msg1.funcs.decode = &read_submsg;
        alltypes.oneof_msg1.arg = &oneof_msg1;
    }
    
    status = pb_decode(stream, AllTypes_fields, &alltypes);
    
#ifdef PB_ENABLE_MALLOC
    /* Just to check for any interference between pb_release() and callback fields */
    pb_release(AllTypes_fields, &alltypes);
#endif

    return status;
}

int main(int argc, char **argv)
{
    uint8_t buffer[1024];
    size_t count;
    pb_istream_t stream;

    /* Whether to expect the optional values or the default values. */
    int mode = (argc > 1) ? atoi(argv[1]) : 0;
    
    /* Read the data into buffer */
    SET_BINARY_MODE(stdin);
    count = fread(buffer, 1, sizeof(buffer), stdin);
    
    /* Construct a pb_istream_t for reading from the buffer */
    stream = pb_istream_from_buffer(buffer, count);
    
    /* Decode and print out the stuff */
    if (!check_alltypes(&stream, mode))
    {
        printf("Parsing failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    } else {
        return 0;
    }
}
