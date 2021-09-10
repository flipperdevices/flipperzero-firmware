# Nanopb: New features in nanopb 0.4

## What's new in nanopb 0.4

Long in the making, nanopb 0.4 has seen some wide reaching improvements
in reaction to the development of the rest of the protobuf ecosystem.
This document showcases features that are not immediately visible, but
that you may want to take advantage of.

A lot of effort has been spent in retaining backwards and forwards
compatibility with previous nanopb versions. For a list of breaking
changes, see [migration document](migration.html)

### New field descriptor format

The basic design of nanopb has always been that the information about
messages is stored in a compact descriptor format, which is iterated in
runtime. Initially it was very tightly tied with encoder and decoder
logic.

In nanopb-0.3.0 the field iteration logic was separated to
`pb_common.c`. Already at that point it was clear that the old format
was getting too limited, but it wasn't extended at that time.

Now in 0.4, the descriptor format was completely decoupled from the
encoder and decoder logic, and redesigned to meet new demands.
Previously each field was stored as `pb_field_t` struct, which was
between 8 and 32 bytes in size, depending on compilation options and
platform. Now information about fields is stored as a variable length
sequence of `uint32_t` data words. There are 1, 2, 4 and 8 word formats,
with the 8 word format containing plenty of space for future
extensibility.

One benefit of the variable length format is that most messages now take
less storage space. Most fields use 2 words, while simple fields in
small messages require only 1 word. Benefit is larger if code previously
required `PB_FIELD_16BIT` or `PB_FIELD_32BIT` options. In
the `AllTypes` test case, 0.3 had data size of 1008 bytes in
8-bit configuration and 1408 bytes in 16-bit configuration. New format
in 0.4 takes 896 bytes for either of these.

In addition, the new decoupling has allowed moving most of the field
descriptor data into FLASH on Harvard architectures, such as AVR.
Previously nanopb was quite RAM-heavy on AVR, which cannot put normal
constants in flash like most other platforms do.

### Python packaging for generator

Nanopb generator is now available as a Python package, installable using
`pip` package manager. This will reduce the need for binary
packages, as if you have Python already installed you can just
`pip install nanopb` and have the generator available on path as
`nanopb_generator`.

The generator can also take advantage of the Python-based `protoc`
available in `grpcio-tools` Python package. If you also install that,
there is no longer a need to have binary `protoc` available.

### Generator now automatically calls protoc

Initially, nanopb generator was used in two steps: first calling
`protoc` to parse the `.proto` file into `.pb` binary
format, and then calling `nanopb_generator.py` to output the
`.pb.h` and `.pb.c` files.

Nanopb 0.2.3 added support for running as a `protoc` plugin, which
allowed single-step generation using `--nanopb_out` parameter. However,
the plugin mode has two complications: passing options to nanopb
generator itself becomes more difficult, and the generator does not know
the actual path of input files. The second limitation has been
particularly problematic for locating `.options` files.

Both of these older methods still work and will remain supported.
However, now `nanopb_generator` can also take `.proto` files
directly and it will transparently call `protoc` in the background.

### Callbacks bound by function name

Since its very beginnings, nanopb has supported field callbacks to allow
processing structures that are larger than what could fit in memory at
once. So far the callback functions have been stored in the message
structure in a `pb_callback_t` struct.

Storing pointers along with user data is somewhat risky from a security
point of view. In addition it has caused problems with `oneof` fields,
which reuse the same storage space for multiple submessages. Because
there is no separate area for each submessage, there is no space to
store the callback pointers either.

Nanopb-0.4.0 introduces callbacks that are referenced by the function
name instead of setting the pointers separately. This should work well
for most applications that have a single callback function for each
message type. For more complex needs, `pb_callback_t` will also remain
supported.

Function name callbacks also allow specifying custom data types for
inclusion in the message structure. For example, you could have
`MyObject*` pointer along with other message fields, and then process
that object in custom way in your callback.

This feature is demonstrated in
[tests/oneof_callback](https://github.com/nanopb/nanopb/tree/master/tests/oneof_callback) test case and
[examples/network_server](https://github.com/nanopb/nanopb/tree/master/examples/network_server) example.

### Message level callback for oneofs

As mentioned above, callbacks inside submessages inside oneofs have been
problematic to use. To make using `pb_callback_t`-style callbacks there
possible, a new generator option `submsg_callback` was added.

Setting this option to true will cause a new message level callback to
be added before the `which_field` of the oneof. This callback will be
called when the submessage tag number is known, but before the actual
message is decoded. The callback can either choose to set callback
pointers inside the submessage, or just completely decode the submessage
there and then. If any unread data remains after the callback returns,
normal submessage decoding will continue.

There is an example of this in [tests/oneof_callback](https://github.com/nanopb/nanopb/tree/master/tests/oneof_callback) test case.

### Binding message types to custom structures

It is often said that good C code is chock full of macros. Or maybe I
got it wrong. But since nanopb 0.2, the field descriptor generation has
heavily relied on macros. This allows it to automatically adapt to
differences in type alignment on different platforms, and to decouple
the Python generation logic from how the message descriptors are
implemented on the C side.

Now in 0.4.0, I've made the macros even more abstract. Time will tell
whether this was such a great idea that I think it is, but now the
complete list of fields in each message is available in `.pb.h` file.
This allows a kind of metaprogramming using [X-macros]()

One feature that this can be used for is binding the message descriptor
to a custom structure or C++ class type. You could have a bunch of other
fields in the structure and even the datatypes can be different to an
extent, and nanopb will automatically detect the size and position of
each field. The generated `.pb.c` files now just have calls of
`PB_BIND(msgname, structname, width)`. Adding a similar
call to your own code will bind the message to your own structure.

### UTF-8 validation

Protobuf format defines that strings should consist of valid UTF-8
codepoints. Previously nanopb has not enforced this, requiring extra
care in the user code. Now optional UTF-8 validation is available with
compilation option `PB_VALIDATE_UTF8`.

### Double to float conversion

Some platforms such as `AVR` do not support the `double`
datatype, instead making it an alias for `float`. This has resulted in
problems when trying to process message types containing `double` fields
generated on other machines. There has been an example on how to
manually perform the conversion between `double` and
`float`.

Now that example is integrated as an optional feature in nanopb core. By
defining `PB_CONVERT_DOUBLE_FLOAT`, the required conversion between 32-
and 64-bit floating point formats happens automatically on decoding and
encoding.

### Improved testing

Testing on embedded platforms has been integrated in the continuous
testing environment. Now all of the 80+ test cases are automatically run
on STM32 and AVR targets. Previously only a few specialized test cases
were manually tested on embedded systems.

Nanopb fuzzer has also been integrated in Google's [OSSFuzz](https://google.github.io/oss-fuzz/)
platform, giving a huge boost in the CPU power available for randomized
testing.
