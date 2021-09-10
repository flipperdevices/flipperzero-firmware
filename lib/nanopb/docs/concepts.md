# Nanopb: Basic concepts

The things outlined here are the underlying concepts of the nanopb
design.

## Proto files

All Protocol Buffers implementations use .proto files to describe the
message format. The point of these files is to be a portable interface
description language.

### Compiling .proto files for nanopb

Nanopb comes with a Python script to generate `.pb.c` and
`.pb.h` files from the `.proto` definition:

    user@host:~$ nanopb/generator/nanopb_generator.py message.proto
    Writing to message.pb.h and message.pb.c

Internally this script uses Google `protoc` to parse the
input file. If you do not have it available, you may receive an error
message. You can install either `grpcio-tools` Python
package using `pip`, or the `protoc` compiler
itself from `protobuf-compiler` distribution package.
Generally the Python package is recommended, because nanopb requires
protoc version 3.6 or newer to support all features, and some distributions come with an older
version.

### Modifying generator behaviour

Using generator options, you can set maximum sizes for fields in order
to allocate them statically. The preferred way to do this is to create
an .options file with the same name as your .proto file:

    # Foo.proto
    message Foo {
       required string name = 1;
    }

    # Foo.options
    Foo.name max_size:16

For more information on this, see the [Proto file
options](reference.html#proto-file-options) section in the reference
manual.

## Streams

Nanopb uses streams for accessing the data in encoded format. The stream
abstraction is very lightweight, and consists of a structure
(`pb_ostream_t` or `pb_istream_t`) which contains a pointer to a
callback function.

There are a few generic rules for callback functions:

1)  Return false on IO errors. The encoding or decoding process will
    abort immediately.
2)  Use state to store your own data, such as a file descriptor.
3)  `bytes_written` and `bytes_left` are updated by pb_write and
    pb_read.
4)  Your callback may be used with substreams. In this case
    `bytes_left`, `bytes_written` and `max_size` have smaller values
    than the original stream. Don't use these values to calculate
    pointers.
5)  Always read or write the full requested length of data. For example,
    POSIX `recv()` needs the `MSG_WAITALL` parameter to accomplish
    this.

### Output streams

    struct _pb_ostream_t
    {
       bool (*callback)(pb_ostream_t *stream, const uint8_t *buf, size_t count);
       void *state;
       size_t max_size;
       size_t bytes_written;
    };

The `callback` for output stream may be NULL, in which case the stream
simply counts the number of bytes written. In this case, `max_size` is
ignored.

Otherwise, if `bytes_written` + bytes_to_be_written is larger than
`max_size`, pb_write returns false before doing anything else. If you
don\'t want to limit the size of the stream, pass SIZE_MAX.

**Example 1:**

This is the way to get the size of the message without storing it
anywhere:

    Person myperson = ...;
    pb_ostream_t sizestream = {0};
    pb_encode(&sizestream, Person_fields, &myperson);
    printf("Encoded size is %d\n", sizestream.bytes_written);

**Example 2:**

Writing to stdout:

    bool callback(pb_ostream_t `stream, const uint8_t `buf, size_t count)
    {
       FILE *file = (FILE*) stream->state;
       return fwrite(buf, 1, count, file) == count;
    }

    pb_ostream_t stdoutstream = {&callback, stdout, SIZE_MAX, 0};

### Input streams

For input streams, there is one extra rule:

6)  You don't need to know the length of the message in advance. After
    getting EOF error when reading, set `bytes_left` to 0 and return
    `false`. `pb_decode()` will detect this and if the EOF was in a proper
    position, it will return true.

Here is the structure:

    struct _pb_istream_t
    {
       bool (*callback)(pb_istream_t *stream, uint8_t *buf, size_t count);
       void *state;
       size_t bytes_left;
    };

The `callback` must always be a function pointer. `Bytes_left` is an
upper limit on the number of bytes that will be read. You can use
SIZE_MAX if your callback handles EOF as described above.

**Example:**

This function binds an input stream to stdin:

    bool callback(pb_istream_t *stream, uint8_t *buf, size_t count)
    {
       FILE *file = (FILE*)stream->state;
       bool status;

       if (buf == NULL)
       {
           while (count-- && fgetc(file) != EOF);
           return count == 0;
       }

       status = (fread(buf, 1, count, file) == count);

       if (feof(file))
           stream->bytes_left = 0;

       return status;
    }

    pb_istream_t stdinstream = {&callback, stdin, SIZE_MAX};

## Data types

Most Protocol Buffers datatypes have directly corresponding C datatypes,
such as `int32` is `int32_t`, `float` is `float` and `bool` is `bool`. However, the
variable-length datatypes are more complex:

1)  Strings, bytes and repeated fields of any type map to callback
    functions by default.
2)  If there is a special option `(nanopb).max_size` specified in the
    .proto file, string maps to null-terminated char array and bytes map
    to a structure containing a char array and a size field.
3)  If `(nanopb).fixed_length` is set to `true` and
    `(nanopb).max_size` is also set, then bytes map to an inline byte
    array of fixed size.
4)  If there is a special option `(nanopb).max_count` specified on a
    repeated field, it maps to an array of whatever type is being
    repeated. Another field will be created for the actual number of
    entries stored.
5)  If `(nanopb).fixed_count` is set to `true` and
    `(nanopb).max_count` is also set, the field for the actual number
    of entries will not by created as the count is always assumed to be
    max count.

### Examples of .proto specifications vs. generated structure

**Simple integer field:**\
.proto: `int32 age = 1;`\
.pb.h: `int32_t age;`

**String with unknown length:**\
.proto: `string name = 1;`\
.pb.h: `pb_callback_t name;`

**String with known maximum length:**\
.proto: `string name = 1 [(nanopb).max_length = 40];`\
.pb.h: `char name[41];`

**Repeated string with unknown count:**\
.proto: `repeated string names = 1;`\
.pb.h: `pb_callback_t names;`

**Repeated string with known maximum count and size:**\
.proto: `repeated string names = 1 [(nanopb).max_length = 40, (nanopb).max_count = 5];`\
.pb.h: `size_t names_count;` `char names[5][41];`

**Bytes field with known maximum size:**\
.proto: `bytes data = 1 [(nanopb).max_size = 16];`\
.pb.h: `PB_BYTES_ARRAY_T(16) data;`, where the struct contains `{pb_size_t size; pb_byte_t bytes[n];}`

**Bytes field with fixed length:**\
.proto: `bytes data = 1 [(nanopb).max_size = 16, (nanopb).fixed_length = true];`\
.pb.h: `pb_byte_t data[16];`

**Repeated integer array with known maximum size:**\
.proto: `repeated int32 numbers = 1 [(nanopb).max_count = 5];`\
.pb.h: `pb_size_t numbers_count;` `int32_t numbers[5];`

**Repeated integer array with fixed count:**\
.proto: `repeated int32 numbers = 1 [(nanopb).max_count = 5, (nanopb).fixed_count = true];`\
.pb.h: `int32_t numbers[5];`

The maximum lengths are checked in runtime. If string/bytes/array
exceeds the allocated length, `pb_decode()` will return false.

> **Note:** For the `bytes` datatype, the field length checking may not be
exact. The compiler may add some padding to the `pb_bytes_t`
structure, and the nanopb runtime doesn't know how much of the
structure size is padding. Therefore it uses the whole length of the
structure for storing data, which is not very smart but shouldn't cause
problems. In practise, this means that if you specify
`(nanopb).max_size=5` on a `bytes` field, you may be able to store 6
bytes there. For the `string` field type, the length limit is exact.

> **Note:** The decoder only keeps track of one `fixed_count` repeated field at a time. Usually this it not an issue because all elements of a repeated field occur end-to-end. Interleaved array elements of several `fixed_count` repeated fields would be a valid protobuf message, but would get rejected by nanopb decoder with error `"wrong size for fixed count field"`.

## Field callbacks

When a field has dynamic length, nanopb cannot statically allocate
storage for it. Instead, it allows you to handle the field in whatever
way you want, using a callback function.

The [pb_callback_t](reference.html#pb-callback-t) structure contains a
function pointer and a `void` pointer called `arg` you can use for
passing data to the callback. If the function pointer is NULL, the field
will be skipped. A pointer to the `arg` is passed to the function, so
that it can modify it and retrieve the value.

The actual behavior of the callback function is different in encoding
and decoding modes. In encoding mode, the callback is called once and
should write out everything, including field tags. In decoding mode, the
callback is called repeatedly for every data item.

To write more complex field callbacks, it is recommended to read the
[Google Protobuf Encoding Specification](https://developers.google.com/protocol-buffers/docs/encoding).

### Encoding callbacks

    bool (*encode)(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg);

|            |                                                                    |
| ---------- | ------------------------------------------------------------------ |
| `stream`   | Output stream to write to                                          |
| `field`    | Iterator for the field currently being encoded or decoded.         |
| `arg`      | Pointer to the `arg` field in the `pb_callback_t` structure.       |

When encoding, the callback should write out complete fields, including
the wire type and field number tag. It can write as many or as few
fields as it likes. For example, if you want to write out an array as
`repeated` field, you should do it all in a single call.

Usually you can use [pb_encode_tag_for_field](reference.html#pb-encode-tag-for-field) to
encode the wire type and tag number of the field. However, if you want
to encode a repeated field as a packed array, you must call
[pb_encode_tag](reference.html#pb-encode-tag) instead to specify a
wire type of `PB_WT_STRING`.

If the callback is used in a submessage, it will be called multiple
times during a single call to [pb_encode](reference.html#pb-encode). In
this case, it must produce the same amount of data every time. If the
callback is directly in the main message, it is called only once.

This callback writes out a dynamically sized string:

    bool write_string(pb_ostream_t *stream, const pb_field_iter_t *field, void * const *arg)
    {
        char *str = get_string_from_somewhere();
        if (!pb_encode_tag_for_field(stream, field))
            return false;

        return pb_encode_string(stream, (uint8_t*)str, strlen(str));
    }

### Decoding callbacks

    bool (*decode)(pb_istream_t *stream, const pb_field_iter_t *field, void **arg);

|            |                                                                    |
| ---------- | ------------------------------------------------------------------ |
| `stream`   | Input stream to read from                                          |
| `field`    | Iterator for the field currently being encoded or decoded.         |
| `arg`      | Pointer to the `arg` field in the `pb_callback_t` structure.       |

When decoding, the callback receives a length-limited substring that
reads the contents of a single field. The field tag has already been
read. For `string` and `bytes`, the length value has already been
parsed, and is available at `stream->bytes_left`.

The callback will be called multiple times for repeated fields. For
packed fields, you can either read multiple values until the stream
ends, or leave it to [pb_decode](reference.html#pb-decode) to call your
function over and over until all values have been read.

This callback reads multiple integers and prints them:

    bool read_ints(pb_istream_t *stream, const pb_field_iter_t *field, void **arg)
    {
        while (stream->bytes_left)
        {
            uint64_t value;
            if (!pb_decode_varint(stream, &value))
                return false;
            printf("%lld\n", value);
        }
        return true;
    }

### Function name bound callbacks

    bool MyMessage_callback(pb_istream_t *istream, pb_ostream_t *ostream, const pb_field_iter_t *field);

|            |                                                                    |
| ---------- | ------------------------------------------------------------------ |
| `istream`  | Input stream to read from, or NULL if called in encoding context.  |
| `ostream`  | Output stream to write to, or NULL if called in decoding context.  |
| `field`    | Iterator for the field currently being encoded or decoded.         |

Storing function pointer in `pb_callback_t` fields inside
the message requires extra storage space and is often cumbersome. As an
alternative, the generator options `callback_function` and
`callback_datatype` can be used to bind a callback function
based on its name.

Typically this feature is used by setting
`callback_datatype` to e.g. `void\*` or other
data type used for callback state. Then the generator will automatically
set `callback_function` to
`MessageName_callback` and produce a prototype for it in
generated `.pb.h`. By implementing this function in your own
code, you will receive callbacks for fields without having to separately
set function pointers.

If you want to use function name bound callbacks for some fields and
`pb_callback_t` for other fields, you can call
`pb_default_field_callback` from the message-level
callback. It will then read a function pointer from
`pb_callback_t` and call it.

## Message descriptor

For using the `pb_encode()` and `pb_decode()` functions, you need a
description of all the fields contained in a message. This description
is usually autogenerated from .proto file.

For example this submessage in the Person.proto file:

~~~~ protobuf
message Person {
    message PhoneNumber {
        required string number = 1 [(nanopb).max_size = 40];
        optional PhoneType type = 2 [default = HOME];
    }
}
~~~~

This in turn generates a macro list in the `.pb.h` file:

    #define Person_PhoneNumber_FIELDLIST(X, a) \
    X(a, STATIC,   REQUIRED, STRING,   number,            1) \
    X(a, STATIC,   OPTIONAL, UENUM,    type,              2)

Inside the `.pb.c` file there is a macro call to
`PB_BIND`:

    PB_BIND(Person_PhoneNumber, Person_PhoneNumber, AUTO)

These macros will in combination generate `pb_msgdesc_t`
structure and associated lists:

    const uint32_t Person_PhoneNumber_field_info[] = { ... };
    const pb_msgdesc_t * const Person_PhoneNumber_submsg_info[] = { ... };
    const pb_msgdesc_t Person_PhoneNumber_msg = {
      2,
      Person_PhoneNumber_field_info,
      Person_PhoneNumber_submsg_info,
      Person_PhoneNumber_DEFAULT,
      NULL,
    };

The encoding and decoding functions take a pointer to this structure and
use it to process each field in the message.

## Oneof

Protocol Buffers supports
[oneof](https://developers.google.com/protocol-buffers/docs/reference/proto2-spec#oneof_and_oneof_field)
sections, where only one of the fields contained within can be present. Here is an example of `oneof` usage:

~~~~ protobuf
message MsgType1 {
    required int32 value = 1;
}

message MsgType2 {
    required bool value = 1;
}

message MsgType3 {
    required int32 value1 = 1;
    required int32 value2 = 2;
} 

message MyMessage {
    required uint32 uid = 1;
    required uint32 pid = 2;
    required uint32 utime = 3;

    oneof payload {
        MsgType1 msg1 = 4;
        MsgType2 msg2 = 5;
        MsgType3 msg3 = 6;
    }
}
~~~~

Nanopb will generate `payload` as a C union and add an additional field
`which_payload`:

    typedef struct _MyMessage {
      uint32_t uid;
      uint32_t pid;
      uint32_t utime;
      pb_size_t which_payload;
      union {
          MsgType1 msg1;
          MsgType2 msg2;
          MsgType3 msg3;
      } payload;
    } MyMessage;

`which_payload` indicates which of the `oneof` fields is actually set.
The user is expected to set the field manually using the correct field
tag:

    MyMessage msg = MyMessage_init_zero;
    msg.payload.msg2.value = true;
    msg.which_payload = MyMessage_msg2_tag;

Notice that neither `which_payload` field nor the unused fields in
`payload` will consume any space in the resulting encoded message.

When a field inside `oneof` contains `pb_callback_t`
fields, the callback values cannot be set before decoding. This is
because the different fields share the same storage space in C
`union`. Instead either function name bound callbacks or a
separate message level callback can be used. See
[tests/oneof_callback](https://github.com/nanopb/nanopb/tree/master/tests/oneof_callback)
for an example on this.

## Extension fields

Protocol Buffers supports a concept of [extension
fields](https://developers.google.com/protocol-buffers/docs/proto#extensions),
which are additional fields to a message, but defined outside the actual
message. The definition can even be in a completely separate .proto
file.

The base message is declared as extensible by keyword `extensions` in
the .proto file:

~~~~ protobuf
message MyMessage {
    .. fields ..
    extensions 100 to 199;
}
~~~~

For each extensible message, `nanopb_generator.py` declares an
additional callback field called `extensions`. The field and associated
datatype `pb_extension_t` forms a linked list of handlers. When an
unknown field is encountered, the decoder calls each handler in turn
until either one of them handles the field, or the list is exhausted.

The actual extensions are declared using the `extend` keyword in the
.proto, and are in the global namespace:

~~~~ protobuf
extend MyMessage {
    optional int32 myextension = 100;
}
~~~~

For each extension, `nanopb_generator.py` creates a constant of type
`pb_extension_type_t`. To link together the base message and the
extension, you have to:

1.  Allocate storage for your field, matching the datatype in the
    .proto. For example, for a `int32` field, you need a `int32_t`
    variable to store the value.
2.  Create a `pb_extension_t` constant, with pointers to your variable
    and to the generated `pb_extension_type_t`.
3.  Set the `message.extensions` pointer to point to the
    `pb_extension_t`.

An example of this is available in `tests/test_encode_extensions.c`
and `tests/test_decode_extensions.c`.

## Default values

Protobuf has two syntax variants, proto2 and proto3. Of these proto2 has
user definable default values that can be given in .proto file:

~~~~ protobuf
message MyMessage {
    optional bytes foo = 1 [default = "ABC\x01\x02\x03"];
    optional string bar = 2 [default = "åäö"];
}
~~~~

Nanopb will generate both static and runtime initialization for the
default values. In `myproto.pb.h` there will be a
`#define MyMessage_init_default {...}` that can be used to initialize
whole message into default values:

    MyMessage msg = MyMessage_init_default;

In addition to this, `pb_decode()` will initialize message
fields to defaults at runtime. If this is not desired,
`pb_decode_ex()` can be used instead.

## Message framing

Protocol Buffers does not specify a method of framing the messages for
transmission. This is something that must be provided by the library
user, as there is no one-size-fits-all solution. Typical needs for a
framing format are to:

1.  Encode the message length.
2.  Encode the message type.
3.  Perform any synchronization and error checking that may be needed
    depending on application.

For example UDP packets already fullfill all the requirements, and TCP
streams typically only need a way to identify the message length and
type. Lower level interfaces such as serial ports may need a more robust
frame format, such as HDLC (high-level data link control).

Nanopb provides a few helpers to facilitate implementing framing
formats:

1.  Functions `pb_encode_ex` and `pb_decode_ex` prefix the message
    data with a varint-encoded length.
2.  Union messages and oneofs are supported in order to implement
    top-level container messages.
3.  Message IDs can be specified using the `(nanopb_msgopt).msgid`
    option and can then be accessed from the header.

## Return values and error handling

Most functions in nanopb return bool: `true` means success, `false`
means failure. There is also support for error messages for
debugging purposes: the error messages go in `stream->errmsg`.

The error messages help in guessing what is the underlying cause of the
error. The most common error conditions are:

1)  Invalid protocol buffers binary message.
2)  Mismatch between binary message and .proto message type.
3)  Unterminated message (incorrect message length).
4) Exceeding the max_size or bytes_left of a stream.
5) Exceeding the max_size/max_count of a string or array field
6) IO errors in your own stream callbacks.
7) Errors that happen in your callback functions.
8) Running out of memory, i.e. stack overflow.
9) Invalid field descriptors (would usually mean a bug in the generator).
