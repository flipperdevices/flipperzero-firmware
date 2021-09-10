# Nanopb: API reference

## Compilation options

The following options can be specified in one of two ways:

1.  Using the -D switch on the C compiler command line.
2.  Using a `#define` at the top of pb.h.

> **NOTE:** You must have the same settings for the nanopb library and all code that
includes nanopb headers.

* `PB_ENABLE_MALLOC`: Enable dynamic allocation support in the decoder.
* `PB_MAX_REQUIRED_FIELDS`: Maximum number of proto2 `required` fields to check for presence. Default value is 64. Compiler warning will tell if you need this.
* `PB_FIELD_32BIT`: Add support for field tag numbers over 65535, fields larger than 64 kiB and arrays larger than 65535 entries. Compiler warning will tell if you need this.
* `PB_NO_ERRMSG`: Disable error message support to save code size. Only error information is the `true`/`false` return value.
* `PB_BUFFER_ONLY`: Disable support for custom streams. Only supports encoding and decoding with memory buffers. Speeds up execution and slightly decreases code size.
* `PB_SYSTEM_HEADER`: Replace the standards header files with a single system-specific header file. Value must include quotes, for example `#define PB_SYSTEM_HEADER "foo.h"`. See [extra/pb_syshdr.h](https://github.com/nanopb/nanopb/blob/master/extra/pb_syshdr.h) for an example.
* `PB_WITHOUT_64BIT`: Disable support of 64-bit integer fields, for old compilers or for a slight speedup on 8-bit platforms.
* `PB_ENCODE_ARRAYS_UNPACKED`: Encode scalar arrays in the unpacked format, which takes up more space. Only to be used when the decoder on the receiving side cannot process packed arrays, such as [protobuf.js versions before 2020](https://github.com/protocolbuffers/protobuf/issues/1701).
* `PB_CONVERT_DOBULE_FLOAT`: Convert doubles to floats for platforms that do not support 64-bit `double` datatype. Mainly `AVR` processors.
* `PB_VALIDATE_UTF8`: Check whether incoming strings are valid UTF-8 sequences. Adds a small performance and code size penalty.

The `PB_MAX_REQUIRED_FIELDS` and `PB_FIELD_32BIT` settings allow
raising some datatype limits to suit larger messages. Their need is
recognized automatically by C-preprocessor `#if`-directives in the
generated `.pb.c` files. The default setting is to use the smallest
datatypes (least resources used).

## Proto file options

The generator behaviour can be adjusted using several options, defined
in the [nanopb.proto](https://github.com/nanopb/nanopb/blob/master/generator/proto/nanopb.proto) file in the generator folder. Here is a list of the most common options, but see the file for a full list:

* `max_size`: Allocated maximum size for `bytes` and `string` fields. For strings, this includes the terminating zero.
* `max_length`: Maximum length for `string` fields. Setting this is equivalent to setting `max_size` to a value of length + 1.
* `max_count`: Allocated maximum number of entries in arrays (`repeated` fields).
* `type`: Select how memory is allocated for the generated field. Default value is `FT_DEFAULT`, which defaults to `FT_STATIC` when possible and `FT_CALLBACK` if not possible. You can use `FT_CALLBACK`, `FT_POINTER`, `FT_STATIC` or `FT_IGNORE` to select a callback field, a dynamically allocate dfield, a statically allocated field or to completely ignore the field.
* `long_names`: Prefix the enum name to the enum value in definitions, i.e. `EnumName_EnumValue`. Enabled by default.
* `packed_struct`: Make the generated structures packed, which saves some RAM space but slows down execution. This can only be used if the CPU supports unaligned access to variables.
* `skip_message`: Skip a whole message from generation. Can be used to remove message types that are not needed in an application.
* `no_unions`: Generate `oneof` fields as multiple optional fields instead of a C `union {}`.
* `anonymous_oneof`: Generate `oneof` fields as an anonymous union.
* `msgid`: Specifies a unique id for this message type. Can be used by user code as an identifier.
* `fixed_length`: Generate `bytes` fields with a constant length defined by `max_size`. A separate `.size` field will then not be generated.
* `fixed_count`: Generate arrays with constant length defined by `max_count`.
* `package`: Package name that applies only for nanopb generator. Defaults to name defined by `package` keyword in .proto file, which applies for all languages.
* `int_size`: Override the integer type of a field. For example, specify `int_size = IS_8` to convert `int32` from protocol definition into `int8_t` in the structure.

These options can be defined for the .proto files before they are
converted using the nanopb-generatory.py. There are three ways to define
the options:

1.  Using a separate .options file. This allows using wildcards for
    applying same options to multiple fields.
2.  Defining the options on the command line of nanopb_generator.py.
    This only makes sense for settings that apply to a whole file.
3.  Defining the options in the .proto file using the nanopb extensions.
    This keeps the options close to the fields they apply to, but can be
    problematic if the same .proto file is shared with many projects.

The effect of the options is the same no matter how they are given. The
most common purpose is to define maximum size for string fields in order
to statically allocate them.

### Defining the options in a .options file

The preferred way to define options is to have a separate file
'myproto.options' in the same directory as the 'myproto.proto'. :

    # myproto.proto
    message MyMessage {
        required string name = 1;
        repeated int32 ids = 4;
    }

    # myproto.options
    MyMessage.name         max_size:40
    MyMessage.ids          max_count:5

The generator will automatically search for this file and read the
options from it. The file format is as follows:

-   Lines starting with `#` or `//` are regarded as comments.
-   Blank lines are ignored.
-   All other lines should start with a field name pattern, followed by
    one or more options. For example: `MyMessage.myfield max_size:5 max_count:10`.
-   The field name pattern is matched against a string of form
    `Message.field`. For nested messages, the string is
    `Message.SubMessage.field`. A whole file can be matched by its
    filename `dir/file.proto`.
-   The field name pattern may use the notation recognized by Python
    fnmatch():
    -   `*` matches any part of string, like `Message.*` for all
        fields
    -   `?` matches any single character
    -   `[seq]` matches any of characters `s`, `e` and `q`
    -   `[!seq]` matches any other character
-   The options are written as `option_name:option_value` and
    several options can be defined on same line, separated by
    whitespace.
-   Options defined later in the file override the ones specified
    earlier, so it makes sense to define wildcard options first in the
    file and more specific ones later.

To debug problems in applying the options, you can use the `-v` option
for the nanopb generator. With protoc, plugin options are specified with
`--nanopb_opt`:

    nanopb_generator -v message.proto           # When invoked directly
    protoc ... --nanopb_opt=-v --nanopb_out=. message.proto  # When invoked through protoc

Protoc doesn't currently pass include path into plugins. Therefore if
your `.proto` is in a subdirectory, nanopb may have trouble finding the
associated `.options` file. A workaround is to specify include path
separately to the nanopb plugin, like:

    protoc -Isubdir --nanopb_opt=-Isubdir --nanopb_out=. message.proto

If preferred, the name of the options file can be set using generator
argument `-f`.

### Defining the options on command line

The nanopb_generator.py has a simple command line option `-s OPTION:VALUE`.
The setting applies to the whole file that is being processed.

### Defining the options in the .proto file

The .proto file format allows defining custom options for the fields.
The nanopb library comes with *nanopb.proto* which does exactly that,
allowing you do define the options directly in the .proto file:

~~~~ protobuf
import "nanopb.proto";

message MyMessage {
    required string name = 1 [(nanopb).max_size = 40];
    repeated int32 ids = 4   [(nanopb).max_count = 5];
}
~~~~

A small complication is that you have to set the include path of protoc
so that nanopb.proto can be found. Therefore, to compile a .proto file
which uses options, use a protoc command similar to:

    protoc -Inanopb/generator/proto -I. --nanopb_out=. message.proto

The options can be defined in file, message and field scopes:

~~~~ protobuf
option (nanopb_fileopt).max_size = 20; // File scope
message Message
{
    option (nanopb_msgopt).max_size = 30; // Message scope
    required string fieldsize = 1 [(nanopb).max_size = 40]; // Field scope
}
~~~~

## pb.h

### pb_byte_t

Type used for storing byte-sized data, such as raw binary input and
bytes-type fields.

    typedef uint_least8_t pb_byte_t;

For most platforms this is equivalent to `uint8_t`. Some platforms
however do not support 8-bit variables, and on those platforms 16 or 32
bits need to be used for each byte.

### pb_size_t

Type used for storing tag numbers and sizes of message fields. By
default the type is 16-bit:

    typedef uint_least16_t pb_size_t;

If tag numbers or fields larger than 65535 are needed, `PB_FIELD_32BIT`
option can be used to change the type to 32-bit value.

### pb_type_t

Type used to store the type of each field, to control the
encoder/decoder behaviour.

    typedef uint_least8_t pb_type_t;

The low-order nibble of the enumeration values defines the function that
can be used for encoding and decoding the field data:

| LTYPE identifier                 |Value  |Storage format
| ---------------------------------|-------|------------------------------------------------
| `PB_LTYPE_BOOL`                  |0x00   |Boolean.
| `PB_LTYPE_VARINT`                |0x01   |Integer.
| `PB_LTYPE_UVARINT`               |0x02   |Unsigned integer.
| `PB_LTYPE_SVARINT`               |0x03   |Integer, zigzag encoded.
| `PB_LTYPE_FIXED32`               |0x04   |32-bit integer or floating point.
| `PB_LTYPE_FIXED64`               |0x05   |64-bit integer or floating point.
| `PB_LTYPE_BYTES`                 |0x06   |Structure with `size_t` field and byte array.
| `PB_LTYPE_STRING`                |0x07   |Null-terminated string.
| `PB_LTYPE_SUBMESSAGE`            |0x08   |Submessage structure.
| `PB_LTYPE_SUBMSG_W_CB`           |0x09   |Submessage with pre-decoding callback.
| `PB_LTYPE_EXTENSION`             |0x0A   |Pointer to `pb_extension_t`.
| `PB_LTYPE_FIXED_LENGTH_BYTES`    |0x0B   |Inline `pb_byte_t` array of fixed size.

The bits 4-5 define whether the field is required, optional or repeated.
There are separate definitions for semantically different modes, even
though some of them share values and are distinguished based on values
of other fields:

 |HTYPE identifier     |Value  |Field handling
 |---------------------|-------|--------------------------------------------------------------------------------------------
 |`PB_HTYPE_REQUIRED`  |0x00   |Verify that field exists in decoded message.
 |`PB_HTYPE_OPTIONAL`  |0x10   |Use separate `has_<field>` boolean to specify whether the field is present.
 |`PB_HTYPE_SINGULAR`  |0x10   |Proto3 field, which is present when its value is non-zero.
 |`PB_HTYPE_REPEATED`  |0x20   |A repeated field with preallocated array. Separate `<field>_count` for number of items.
 |`PB_HTYPE_FIXARRAY`  |0x20   |A repeated field that has constant length.
 |`PB_HTYPE_ONEOF`     |0x30   |Oneof-field, only one of each group can be present.

The bits 6-7 define the how the storage for the field is allocated:

|ATYPE identifier     |Value  |Allocation method
|---------------------|-------|--------------------------------------------------------------------------------------------
|`PB_ATYPE_STATIC`    |0x00   |Statically allocated storage in the structure.
|`PB_ATYPE_POINTER`   |0x80   |Dynamically allocated storage. Struct field contains a pointer to the storage.
|`PB_ATYPE_CALLBACK`  |0x40   |A field with dynamic storage size. Struct field contains a pointer to a callback function.

### pb_msgdesc_t

Autogenerated structure that contains information about a message and
pointers to the field descriptors. Use functions defined in
`pb_common.h` to process the field information.

    typedef struct pb_msgdesc_s pb_msgdesc_t;
    struct pb_msgdesc_s {
        pb_size_t field_count;
        const uint32_t *field_info;
        const pb_msgdesc_t * const * submsg_info;
        const pb_byte_t *default_value;

        bool (*field_callback)(pb_istream_t *istream, pb_ostream_t *ostream, const pb_field_iter_t *field);
    };

|                 |                                                        |
|-----------------|--------------------------------------------------------|
|`field_count`    | Total number of fields in the message.
|`field_info`     | Pointer to compact representation of the field information.
|`submsg_info`    | Pointer to array of pointers to descriptors for submessages.
|`default_value`  | Default values for this message as an encoded protobuf message.
|`field_callback` | Function used to handle all callback fields in this message. By default `pb_default_field_callback()`  which loads per-field callbacks from a `pb_callback_t` structure.

### pb_field_iter_t

Describes a single structure field with memory position in relation to
others. The field information is stored in a compact format and loaded
into `pb_field_iter_t` by the functions defined in `pb_common.h`.

    typedef struct pb_field_iter_s pb_field_iter_t;
    struct pb_field_iter_s {
        const pb_msgdesc_t *descriptor;
        void *message;

        pb_size_t index;
        pb_size_t field_info_index;
        pb_size_t required_field_index;
        pb_size_t submessage_index;

        pb_size_t tag;
        pb_size_t data_size;
        pb_size_t array_size;
        pb_type_t type;

        void *pField;
        void *pData;
        void *pSize;

        const pb_msgdesc_t *submsg_desc;
    };

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| descriptor           | Pointer to `pb_msgdesc_t` for the message that contains this field.
| message              | Pointer to the start of the message structure.
| index                | Index of the field inside the message
| field_info_index     | Index to the internal `field_info` array
| required_field_index | Index that counts only the required fields
| submessage_index     | Index that counts only submessages
| tag                  | Tag number defined in `.proto` file for this field.
| data_size            | `sizeof()` of the field in the structure. For repeated fields this is for a single array entry.
| array_size           | Maximum number of items in a statically allocated array.
| type                 | Type ([pb_type_t](#pb_type_t)) of the field.
| pField               | Pointer to the field storage in the structure.
| pData                | Pointer to data contents. For arrays and pointers this can be different than `pField`.
| pSize                | Pointer to count or has field, or NULL if this field doesn't have such.
| submsg_desc          | For submessage fields, points to the descriptor for the submessage.

By default [pb_size_t](#pb_size_t) is 16-bit, limiting the sizes and
tags to 65535. The limit can be raised by defining `PB_FIELD_32BIT`.

### pb_bytes_array_t

An byte array with a field for storing the length:

    typedef struct {
        pb_size_t size;
        pb_byte_t bytes[1];
    } pb_bytes_array_t;

In an actual array, the length of `bytes` may be different. The macros
`PB_BYTES_ARRAY_T()` and `PB_BYTES_ARRAY_T_ALLOCSIZE()`
are used to allocate variable length storage for bytes fields.

### pb_callback_t

Part of a message structure, for fields with type PB_HTYPE_CALLBACK:

    typedef struct _pb_callback_t pb_callback_t;
    struct _pb_callback_t {
        union {
            bool (*decode)(pb_istream_t *stream, const pb_field_iter_t *field, void **arg);
            bool (*encode)(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg);
        } funcs;

        void *arg;
    };

A pointer to the *arg* is passed to the callback when calling. It can be
used to store any information that the callback might need. Note that
this is a double pointer. If you set `field.arg` to point to
`&data` in your main code, in the callback you can access it like this:

    myfunction(*arg);           /* Gives pointer to data as argument */
    myfunction(*(data_t*)*arg); /* Gives value of data as argument */
    *arg = newdata;             /* Alters value of field.arg in structure */

When calling [pb_encode](#pb_encode), `funcs.encode` is used, and
similarly when calling [pb_decode](#pb_decode), `funcs.decode` is used.
The function pointers are stored in the same memory location but are of
incompatible types. You can set the function pointer to NULL to skip the
field.

### pb_wire_type_t

Protocol Buffers wire types. These are used with
[pb_encode_tag](#pb_encode_tag). :

    typedef enum {
        PB_WT_VARINT = 0,
        PB_WT_64BIT  = 1,
        PB_WT_STRING = 2,
        PB_WT_32BIT  = 5
    } pb_wire_type_t;

### pb_extension_type_t

Defines the handler functions and auxiliary data for a field that
extends another message. Usually autogenerated by
`nanopb_generator.py`.

    typedef struct {
        bool (*decode)(pb_istream_t *stream, pb_extension_t *extension,
                   uint32_t tag, pb_wire_type_t wire_type);
        bool (*encode)(pb_ostream_t *stream, const pb_extension_t *extension);
        const void *arg;
    } pb_extension_type_t;

In the normal case, the function pointers are `NULL` and the decoder and
encoder use their internal implementations. The internal implementations
assume that `arg` points to a [pb_field_iter_t](#pb_field_iter_t)
that describes the field in question.

To implement custom processing of unknown fields, you can provide
pointers to your own functions. Their functionality is mostly the same
as for normal callback fields, except that they get called for any
unknown field when decoding.

### pb_extension_t

Ties together the extension field type and the storage for the field
value. For message structs that have extensions, the generator will
add a `pb_extension_t*` field. It should point to a linked list of
extensions.

    typedef struct {
        const pb_extension_type_t *type;
        void *dest;
        pb_extension_t *next;
        bool found;
    } pb_extension_t;

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| type                 | Pointer to the structure that defines the callback functions.
| dest                 | Pointer to the variable that stores the field value (as used by the default extension callback functions.)
| next                 | Pointer to the next extension handler, or `NULL` for last handler.
| found                | Decoder sets this to true if the extension was found.

### PB_GET_ERROR

Get the current error message from a stream, or a placeholder string if
there is no error message:

    #define PB_GET_ERROR(stream) (string expression)

This should be used for printing errors, for example:

    if (!pb_decode(...))
    {
        printf("Decode failed: %s\n", PB_GET_ERROR(stream));
    }

The macro only returns pointers to constant strings (in code memory), so
that there is no need to release the returned pointer.

### PB_RETURN_ERROR

Set the error message and return false:

    #define PB_RETURN_ERROR(stream,msg) (sets error and returns false)

This should be used to handle error conditions inside nanopb functions
and user callback functions:

    if (error_condition)
    {
        PB_RETURN_ERROR(stream, "something went wrong");
    }

The *msg* parameter must be a constant string.

### PB_BIND

This macro generates the [pb_msgdesc_t](#pb_msgdesc_t) and associated
arrays, based on a list of fields in [X-macro](https://en.wikipedia.org/wiki/X_Macro) format. :

    #define PB_BIND(msgname, structname, width) ...

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| msgname              | Name of the message type. Expects `msgname_FIELDLIST` macro to exist.
| structname           | Name of the C structure to bind to.
| width                | Number of words per field descriptor, or `AUTO` to use minimum size possible.

This macro is automatically invoked inside the autogenerated `.pb.c`
files. User code can also call it to bind message types with custom
structures or class types.

## pb_encode.h

### pb_ostream_from_buffer

Constructs an output stream for writing into a memory buffer. It uses an internal callback that
stores the pointer in stream `state` field. :

    pb_ostream_t pb_ostream_from_buffer(pb_byte_t *buf, size_t bufsize);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| buf                  | Memory buffer to write into.
| bufsize              | Maximum number of bytes to write.
| returns              | An output stream.

After writing, you can check `stream.bytes_written` to find out how
much valid data there is in the buffer. This should be passed as the
message length on decoding side.

### pb_write

Writes data to an output stream. Always use this function, instead of
trying to call stream callback manually. :

    bool pb_write(pb_ostream_t *stream, const pb_byte_t *buf, size_t count);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to.
| buf                  | Pointer to buffer with the data to be written.
| count                | Number of bytes to write.
| returns              | True on success, false if maximum length is exceeded or an IO error happens.

> **NOTE:** If an error happens, *bytes_written* is not incremented. Depending on
the callback used, calling pb_write again after it has failed once may
cause undefined behavior. Nanopb itself never does this, instead it
returns the error to user application. The builtin
`pb_ostream_from_buffer` is safe to call again after failed write.

### pb_encode

Encodes the contents of a structure as a protocol buffers message and
writes it to output stream. :

    bool pb_encode(pb_ostream_t *stream, const pb_msgdesc_t *fields, const void *src_struct);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to.
| fields               | Message descriptor, usually autogenerated.
| src_struct           | Pointer to the message structure. Must match `fields` descriptor.
| returns              | True on success, false on any error condition. Error message is set to `stream->errmsg`.

Normally pb_encode simply walks through the fields description array
and serializes each field in turn. However, submessages must be
serialized twice: first to calculate their size and then to actually
write them to output. This causes some constraints for callback fields,
which must return the same data on every call.

### pb_encode_ex

Encodes the message, with extended behavior set by flags:

    bool pb_encode_ex(pb_ostream_t *stream, const pb_msgdesc_t *fields, const void *src_struct, unsigned int flags);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to.
| fields               | Message descriptor, usually autogenerated.
| src_struct           | Pointer to the message structure. Must match `fields` descriptor.
| flags                | Extended options, see below.
| returns              | True on success, false on any error condition. Error message is set to `stream->errmsg`.

The options that can be defined are:

* `PB_ENCODE_DELIMITED`: Indicate the length of the message by prefixing with a varint-encoded length. Compatible with `parseDelimitedFrom` in Google's protobuf library.
* `PB_ENCODE_NULLTERMINATED`: Indicate the length of the message by appending a zero tag value after it. Supported by nanopb decoder, but not by most other protobuf libraries.

### pb_get_encoded_size

Calculates the length of the encoded message.

    bool pb_get_encoded_size(size_t *size, const pb_msgdesc_t *fields, const void *src_struct);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| size                 | Calculated size of the encoded message.
| fields               | Message descriptor, usually autogenerated.
| src_struct           | Pointer to the data that will be serialized.
| returns              | True on success, false on detectable errors in field description or if a field encoder returns false.

### Callback field encoders
The functions with names `pb_encode_<datatype>` are used when dealing with
callback fields. The typical reason for using callbacks is to have an
array of unlimited size. In that case, [pb_encode](#pb_encode) will
call your callback function, which in turn will call `pb_encode_<datatype>`
functions repeatedly to write out values.

The tag of a field must be encoded first with
[pb_encode_tag_for_field](#pb_encode_tag_for_field). After that, you
can call exactly one of the content-writing functions to encode the
payload of the field. For repeated fields, you can repeat this process
multiple times.

Writing packed arrays is a little bit more involved: you need to use
`pb_encode_tag` and specify `PB_WT_STRING` as the wire
type. Then you need to know exactly how much data you are going to
write, and use [pb_encode_varint](#pb_encode_varint) to write out the
number of bytes before writing the actual data. Substreams can be used
to determine the number of bytes beforehand; see
[pb_encode_submessage](#pb_encode_submessage) source code for an
example.

See [Google Protobuf Encoding Format Documentation](https://developers.google.com/protocol-buffers/docs/encoding)
for background information on the Protobuf wire format.

#### pb_encode_tag

Starts a field in the Protocol Buffers binary format: encodes the field
number and the wire type of the data.

    bool pb_encode_tag(pb_ostream_t *stream, pb_wire_type_t wiretype, uint32_t field_number);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to. 1-5 bytes will be written.
| wiretype             | `PB_WT_VARINT`, `PB_WT_64BIT`, `PB_WT_STRING` or `PB_WT_32BIT`
| field_number         | Identifier for the field, defined in the .proto file. You can get it from `field->tag`.
| returns              | True on success, false on IO error.

#### pb_encode_tag_for_field

Same as [pb_encode_tag](#pb_encode_tag), except takes the parameters
from a `pb_field_iter_t` structure.

    bool pb_encode_tag_for_field(pb_ostream_t *stream, const pb_field_iter_t *field);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to. 1-5 bytes will be written.
| field                | Field iterator for this field.
| returns              | True on success, false on IO error or unknown field type.

This function only considers the `PB_LTYPE` of the field. You can use it from
your field callbacks, because the source generator writes correct `LTYPE`
also for callback type fields.

Wire type mapping is as follows:

| LTYPEs                                           | Wire type
|--------------------------------------------------|-----------------
| BOOL, VARINT, UVARINT, SVARINT                   | PB_WT_VARINT
| FIXED64                                          | PB_WT_64BIT
| STRING, BYTES, SUBMESSAGE, FIXED_LENGTH_BYTES    | PB_WT_STRING
| FIXED32                                          | PB_WT_32BIT

#### pb_encode_varint

Encodes a signed or unsigned integer in the
[varint](http://code.google.com/apis/protocolbuffers/docs/encoding.html#varints)
format. Works for fields of type `bool`, `enum`, `int32`, `int64`, `uint32` and `uint64`:

    bool pb_encode_varint(pb_ostream_t *stream, uint64_t value);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to. 1-10 bytes will be written.
| value                | Value to encode, cast to `uint64_t`.
| returns              | True on success, false on IO error.

> **NOTE:** Value will be converted to `uint64_t` in the argument.
> To encode signed values, the argument should be cast to `int64_t` first for correct sign extension.

#### pb_encode_svarint

Encodes a signed integer in the [zig-zagged](https://developers.google.com/protocol-buffers/docs/encoding#signed_integers) format.
Works for fields of type `sint32` and `sint64`:

    bool pb_encode_svarint(pb_ostream_t *stream, int64_t value);

(parameters are the same as for [pb_encode_varint](#pb_encode_varint)

#### pb_encode_string

Writes the length of a string as varint and then contents of the string.
Works for fields of type `bytes` and `string`:

    bool pb_encode_string(pb_ostream_t *stream, const pb_byte_t *buffer, size_t size);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to.
| buffer               | Pointer to string data.
| size                 | Number of bytes in the string. Pass `strlen(s)` for strings.
| returns              | True on success, false on IO error.

#### pb_encode_fixed32

Writes 4 bytes to stream and swaps bytes on big-endian architectures.
Works for fields of type `fixed32`, `sfixed32` and `float`:

    bool pb_encode_fixed32(pb_ostream_t *stream, const void *value);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to. 4 bytes will be written.
| value                | Pointer to a 4-bytes large C variable, for example `uint32_t foo;`.
| returns              | True on success, false on IO error.

#### pb_encode_fixed64

Writes 8 bytes to stream and swaps bytes on big-endian architecture.
Works for fields of type `fixed64`, `sfixed64` and `double`:

    bool pb_encode_fixed64(pb_ostream_t *stream, const void *value);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to. 8 bytes will be written.
| value                | Pointer to a 8-bytes large C variable, for example `uint64_t foo;`.
| returns              | True on success, false on IO error.

#### pb_encode_float_as_double

Encodes a 32-bit `float` value so that it appears like a 64-bit `double` in the encoded message.
This is sometimes needed when platforms like AVR that do not support 64-bit `double` need to communicate using a
message type that contains `double` fields.

    bool pb_encode_float_as_double(pb_ostream_t *stream, float value);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to. 8 bytes will be written.
| value                | Float value to encode.
| returns              | True on success, false on IO error.

#### pb_encode_submessage

Encodes a submessage field, including the size header for it. Works for
fields of any message type.

    bool pb_encode_submessage(pb_ostream_t *stream, const pb_msgdesc_t *fields, const void *src_struct);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Output stream to write to.
| fields               | Pointer to the autogenerated message descriptor for the submessage type, e.g. `MyMessage_fields`.
| src                  | Pointer to the structure where submessage data is.
| returns              | True on success, false on IO errors, pb_encode errors or if submessage size changes between calls.

In Protocol Buffers format, the submessage size must be written before
the submessage contents. Therefore, this function has to encode the
submessage twice in order to know the size beforehand.

If the submessage contains callback fields, the callback function might
misbehave and write out a different amount of data on the second call.
This situation is recognized and `false` is returned, but garbage will
be written to the output before the problem is detected.

## pb_decode.h

### pb_istream_from_buffer

Helper function for creating an input stream that reads data from a
memory buffer.

    pb_istream_t pb_istream_from_buffer(const pb_byte_t *buf, size_t bufsize);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| buf                  | Pointer to byte array to read from.
| bufsize              | Size of the byte array.
| returns              | An input stream ready to use.

### pb_read

Read data from input stream. Always use this function, don't try to
call the stream callback directly.

    bool pb_read(pb_istream_t *stream, pb_byte_t *buf, size_t count);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Input stream to read from.
| buf                  | Buffer to store the data to, or `NULL` to just read data without storing it anywhere.
| count                | Number of bytes to read.
| returns              | True on success, false if `stream->bytes_left` is less than `count` or if an IO error occurs.

End of file is signalled by `stream->bytes_left` being zero after pb_read returns false.

### pb_decode

Read and decode all fields of a structure. Reads until EOF on input
stream.

    bool pb_decode(pb_istream_t *stream, const pb_msgdesc_t *fields, void *dest_struct);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Input stream to read from.
| fields               | Message descriptor, usually autogenerated.
| dest_struct          | Pointer to message structure where data will be stored.
| returns              | True on success, false on any error condition. Error message will be in `stream->errmsg`.

In Protocol Buffers binary format, end-of-file is only allowed between fields.
If it happens anywhere else, pb_decode will return `false`. If
pb_decode returns `false`, you cannot trust any of the data in the
structure.

For optional fields, this function applies the default value and sets
`has_<field>` to false if the field is not present.

If `PB_ENABLE_MALLOC` is defined, this function may allocate storage
for any pointer type fields. In this case, you have to call
[pb_release](#pb_release) to release the memory after you are done with
the message. On error return `pb_decode` will release the memory itself.

### pb_decode_ex

Same as [pb_decode](#pb_decode), but allows extended options.

    bool pb_decode_ex(pb_istream_t *stream, const pb_msgdesc_t *fields, void *dest_struct, unsigned int flags);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Input stream to read from.
| fields               | Message descriptor, usually autogenerated.
| dest_struct          | Pointer to message structure where data will be stored.
| flags                | Extended options, see below
| returns              | True on success, false on any error condition. Error message will be in `stream->errmsg`.

The following options can be defined and combined with bitwise `|` operator:

* `PB_DECODE_NOINIT`: Do not initialize structure before decoding. This can be used to combine multiple messages, or if you have already initialized the message structure yourself.

* `PB_DECODE_DELIMITED`: Expect a length prefix in varint format before message. The counterpart of `PB_ENCODE_DELIMITED`.

* `PB_DECODE_NULLTERMINATED`: Expect the message to be terminated with zero tag. The counterpart of `PB_ENCODE_NULLTERMINATED`.

If `PB_ENABLE_MALLOC` is defined, this function may allocate storage
for any pointer type fields. In this case, you have to call
[pb_release](#pb_release) to release the memory after you are done with
the message. On error return `pb_decode_ex` will release the memory
itself.

### pb_release

Releases any dynamically allocated fields:

    void pb_release(const pb_msgdesc_t *fields, void *dest_struct);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| fields               | Message descriptor, usually autogenerated.
| dest_struct          | Pointer to structure where data is stored. If `NULL`, function does nothing.

This function is only available if `PB_ENABLE_MALLOC` is defined. It
will release any pointer type fields in the structure and set the
pointers to `NULL`.

This function is safe to call multiple times, calling it again does nothing.

### pb_decode_tag

Decode the tag that comes before field in the protobuf encoding:

    bool pb_decode_tag(pb_istream_t *stream, pb_wire_type_t *wire_type, uint32_t *tag, bool *eof);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Input stream to read from.
| wire_type            | Pointer to variable where to store the wire type of the field.
| tag                  | Pointer to variable where to store the tag of the field.
| eof                  | Pointer to variable where to store end-of-file status.
| returns              | True on success, false on error or EOF.

When the message (stream) ends, this function will return `false` and set
`eof` to true. On other errors, `eof` will be set to false.

### pb_skip_field

Remove the data for a field from the stream, without actually decoding it:

    bool pb_skip_field(pb_istream_t *stream, pb_wire_type_t wire_type);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Input stream to read from.
| wire_type            | Type of field to skip.
| returns              | True on success, false on IO error.

This function determines the amount of bytes to read based on the wire type.
For `PB_WT_STRING`, it will read the length prefix of a string or submessage
to determine its length.

### Callback field decoders
The functions with names `pb_decode_<datatype>` are used when dealing with callback fields.
The typical reason for using callbacks is to have an array of unlimited size.
In that case, [pb_decode](#pb_decode) will call your callback function repeatedly,
which can then store the values into e.g. filesystem in the order received in.

For decoding numeric (including enumerated and boolean) values, use
[pb_decode_varint](#pb_decode_varint), [pb_decode_svarint](#pb_decode_svarint),
[pb_decode_fixed32](#pb_decode_fixed32) and [pb_decode_fixed64](#pb_decode_fixed64).
They take a pointer to a 32- or 64-bit C variable, which you may then cast to smaller datatype for storage.

For decoding strings and bytes fields, the length has already been decoded and the callback function is given a length-limited substream.
You can therefore check the total length in `stream->bytes_left` and read the data using [pb_read](#pb_read).

Finally, for decoding submessages in a callback, use [pb_decode](#pb_decode) and pass it the `SubMessage_fields` descriptor array.

#### pb_decode_varint

Read and decode a [varint](http://code.google.com/apis/protocolbuffers/docs/encoding.html#varints)
encoded integer.

    bool pb_decode_varint(pb_istream_t *stream, uint64_t *dest);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Input stream to read from. 1-10 bytes will be read.
| dest                 | Storage for the decoded integer. Value is undefined on error.
| returns              | True on success, false if value exceeds uint64_t range or an IO error happens.

#### pb_decode_varint32

Same as `pb_decode_varint`, but limits the value to 32 bits:

    bool pb_decode_varint32(pb_istream_t *stream, uint32_t *dest);

Parameters are the same as `pb_decode_varint`. This function can be used
for decoding lengths and other commonly occurring elements that you know
shouldn't be larger than 32 bit. It will return an error if the value
exceeds the `uint32_t` datatype.

#### pb_decode_svarint

Similar to [pb_decode_varint](#pb_decode_varint), except that it
performs zigzag-decoding on the value. This corresponds to the Protocol
Buffers `sint32` and `sint64` datatypes. :

    bool pb_decode_svarint(pb_istream_t *stream, int64_t *dest);

(parameters are the same as [pb_decode_varint](#pb_decode_varint))

#### pb_decode_fixed32

Decode a `fixed32`, `sfixed32` or `float` value.

    bool pb_decode_fixed32(pb_istream_t *stream, void *dest);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Input stream to read from. 4 bytes will be read.
| dest                 | Pointer to destination `int32_t`, `uint32_t` or `float`.
| returns              | True on success, false on IO errors.

This function reads 4 bytes from the input stream. On big endian
architectures, it then reverses the order of the bytes. Finally, it
writes the bytes to `dest`.

#### pb_decode_fixed64

Decode a `fixed64`, `sfixed64` or `double` value. :

    bool pb_decode_fixed64(pb_istream_t *stream, void *dest);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Input stream to read from. 8 bytes will be read.
| dest                 | Pointer to destination `int64_t`, `uint64_t` or `double`.
| returns              | True on success, false on IO errors.

Same as [pb_decode_fixed32](#pb_decode_fixed32), except this reads 8
bytes.

#### pb_decode_double_as_float

Decodes a 64-bit `double` value into a 32-bit `float`
variable. Counterpart of [pb_encode_float_as_double](#pb_encode_float_as_double). :

    bool pb_decode_double_as_float(pb_istream_t *stream, float *dest);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Input stream to read from. 8 bytes will be read.
| dest                 | Pointer to destination *float*.
| returns              | True on success, false on IO errors.

#### pb_make_string_substream

Decode the length for a field with wire type `PB_WT_STRING` and create
a substream for reading the data.

    bool pb_make_string_substream(pb_istream_t *stream, pb_istream_t *substream);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Original input stream to read the length and data from.
| substream            | Storage for a new substream that has limited length. Filled in by the function.
| returns              | True on success, false if reading the length fails.

This function uses `pb_decode_varint` to read an integer from the stream.
This is interpreted as a number of bytes, and the substream is set up so that its `bytes_left` is initially the same as the
length, and its callback function and state the same as the parent stream.

#### pb_close_string_substream

Close the substream created with
[pb_make_string_substream](#pb_make_string_substream).

    void pb_close_string_substream(pb_istream_t *stream, pb_istream_t *substream);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| stream               | Original input stream to read data from.
| substream            | Substream to close

This function copies back the state from the substream to the parent stream,
and throws away any unread data from the substream.
It must be called after done with the substream.

## pb_common.h

### pb_field_iter_begin

Begins iterating over the fields in a message type:

    bool pb_field_iter_begin(pb_field_iter_t *iter, const pb_msgdesc_t *desc, void *message);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| iter                 | Pointer to destination [pb_field_iter_t](#pb_field_iter_t) variable.
| desc                 | Autogenerated message descriptor.
| message              | Pointer to message structure.
| returns              | True on success, false if the message type has no fields.

### pb_field_iter_next

Advance to the next field in the message:

    bool pb_field_iter_next(pb_field_iter_t *iter);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| iter                 | Pointer to `pb_field_iter_t` previously initialized by [pb_field_iter_begin](#pb_field_iter_begin).
| returns              | True on success, false after last field in the message.

When the last field in the message has been processed, this function
will return false and initialize `iter` back to the first field in the
message.

### pb_field_iter_find

Find a field specified by tag number in the message:

    bool pb_field_iter_find(pb_field_iter_t *iter, uint32_t tag);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| iter                 | Pointer to `pb_field_iter_t` previously initialized by [pb_field_iter_begin](#pb_field_iter_begin).
| tag                  | Tag number to search for.
| returns              | True if field was found, false otherwise.

This function is functionally identical to calling `pb_field_iter_next()` until `iter.tag` equals the searched value.
Internally this function avoids fully processing the descriptor for intermediate fields.

### pb_validate_utf8

Validates an UTF8 encoded string:

    bool pb_validate_utf8(const char *s);

|                      |                                                        |
|----------------------|--------------------------------------------------------|
| s                    | Pointer to beginning of a string.
| returns              | True, if string is valid UTF-8, false otherwise.

The protobuf standard requires that `string` fields only contain valid
UTF-8 encoded text, while `bytes` fields can contain arbitrary data.
When the compilation option `PB_VALIDATE_UTF8` is defined, nanopb will
automatically validate strings on both encoding and decoding.

User code can call this function to validate strings in e.g. custom
callbacks.
