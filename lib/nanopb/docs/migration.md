# Nanopb: Migration from older versions

This document details all the breaking changes that have been made to
nanopb since its initial release. For each change, the rationale and
required modifications of user applications are explained. Also any
error indications are included, in order to make it easier to find this
document.

Nanopb-0.4.4 (2020-11-25)
-------------------------

### Remove outdated generator/nanopb/options.proto

**Changes:** Back in 2018, it was considered in pull request #241 to
move nanopb generator options to a separate namespace. For this reason,
a transitional file was added. It was later abandoned and is now removed
to avoid confusion.

**Required actions:** Most nanopb users probably never used that transitional
file at all. If your `.proto` files import it, change to using `generator/proto/nanopb.proto`.

**Error indications:** Errors about missing file `options.proto` when running
the generator.

Nanopb-0.4.3 (2020-09-21)
-------------------------

### pb_msgdesc_t struct has new fields

**Changes:** New fields `required_field_count` and
`largest_tag` were added to `pb_msgdesc_t`
and existing fields were reordered.

**Required actions:** All `.pb.c` files must be recompiled.
Regeneration is not needed.

**Error indications:** Messages may fail to encode or decode, or the
code can crash inside `load_descriptor_values()` in
`pb_common.c`.

Nanopb-0.4.2 (2020-06-23)
-------------------------

### Generator now uses Python 3 by default

**Rationale:** Previously `nanopb-generator.py` had hashbang
of `#!/usr/bin/env python`, which would execute with Python
2 on most systems. Python 2 is now deprecated and many libraries are
dropping support for it, which makes installing dependencies difficult.
While `nanopb_generator.py` has worked with Python 3 for
years now, and overriding the python version was possible with
virtualenv, that was an extra complication.

**Changes:** Hashbang now uses `#!/usr/bin/env python3`.
New file `nanopb_generator.py2` can be used to run with
Python 2, if necessary.

**Required actions:** If possible, just verify Python 3 is installed and
necessary dependencies are installed for it. For example `pip3 install protobuf grpcio-tools`
should take care of it. If this is not possible, call `nanopb_generator.py2` from your build
scripts instead.

**Error indications:** `python3: command not found` if
Python 3 is not installed.
`Could not import the Google protobuf Python libraries` if dependencies are only installed for Python 2.

Nanopb-0.4.0 (2019-12-20)
-------------------------

### New field descriptor format

**Rationale:** Previously information about struct fields was stored as
an array of `pb_field_t` structures. This was a
straightforward method, but required allocating space for e.g.
submessage type and array size for all fields, even though most fields
are not submessages nor arrays.

**Changes:** Now field information is encoded more efficiently in
`uint32_t` array in a variable-length format. Old
`pb_field_t` structure has been removed and it is now a
typedef for `pb_field_iter_t`. This retains compatibility
with most old callback definitions. The field definitions in
`.pb.h` files are now of type `pb_msgdesc_t`.

**Required actions:** If your own code accesses the low-level field
information in `pb_field_t`, it must be modified to do so
only through the functions declared in `pb_common.h`.

**Error indications:** `incompatible pointer type` errors
relating to `pb_field_t`

### Changes to generator default options

**Rationale:** Previously nanopb_generator added a timestamp header to
generated files and used only basename of files in
`#include` directives. This is different than what the
`protoc` C++ backend does.

**Changes:** Now default options are `--no-timestamp` and
`--no-strip-path`.

**Required actions:** If old behaviour is desired, add
`--timestamp` and `--strip-path` options to
`nanopb_generator.py` or on `protoc` command
line as `--nanopb_out=--timestamp,--strip-path:outdir`.

**Error indications:** Compiler error: cannot find include file
`mymessage.pb.h` when compiling
`mymessage.pb.c`.

### Removal of bundled plugin.proto

**Rationale:** Google's Python protobuf library, which is used in
nanopb generator, has included `plugin_pb2` with it since
version 3.1.0. It is not necessary to bundle it with nanopb anymore.

**Required actions:** Update `python-protobuf` to version
3.1.0 or newer.

**Error indications:** `ImportError: No module named compiler.plugin_pb2`

### .options file is now always case-sensitive

**Rationale:** Previously field names in `.options` file
were case-sensitive on Linux and case-insensitive on Windows. This was
by accident. Because `.proto` files are case-sensitive,
`.options` files should be too.

**Changes:** Now field names in `.options` are always
case-sensitive, and matched by `fnmatchcase()` instead of
`fnmatch()`.

**Required actions:** If field names in `.options` are not
capitalized the same as in `.proto`, they must be updated.

### `CHAR_BIT` define is now needed

**Rationale:** To check whether the platform has 8-bit or larger chars,
the C standard `CHAR_BIT` macro is needed.

**Changes:** `pb.h` now includes `limits.h` for this macro.

**Required actions:** If your platform doesn't have `limits.h`
available, you can define the macro in `pb_syshdr.h`. There is an
example in `extra` directory.

**Error indications:** `"Cannot find include file <limits.h>."` or
`"Undefined identifier: CHAR_BIT."`

### Strings must now always be null-terminated

**Rationale:** Previously `pb_encode()` would accept non-terminated
strings and assume that they are the full length of the defined array.
However, `pb_decode()` would reject such messages because null
terminator wouldn't fit in the array.

**Changes:** `pb_encode()` will now return an error if null terminator
is missing. Maximum encoded message size calculation is changed
accordingly so that at most `max_size-1` strings are assumed. New field
option `max_length` can be used to define the maximum string length,
instead of the array size.

**Required actions:** If your strings were previously filling the whole
allocated array, increase the size of the field by 1.

**Error indications:** `pb_encode()` returns error `unterminated string`.

### Removal of per-field default value constants

**Rationale:** Previously nanopb declared a
`fieldname_default` constant variable for each field with a
default value, and used these internally to initialize messages. This
however used unnecessarily large amount of storage for the values. The
variables were mostly for internal usage, but were available in the
header file.

**Changes:** Default values are now stored as an encoded protobuf
message.

**Required actions:** If your code previously used default constants, it
will have to be adapted to take the default value in some other way,
such as by defining
`static const MyMessage msg_default = MyMessage_init_default;` and accessing
`msg_default.fieldname`.

**Error indications:** Compiler error about `fieldname_default` being undeclared.

### Zero tag in message now raises error by default

**Rationale:** Previously nanopb has allowed messages to be terminated
by a null byte, which is read as zero tag value. Most other protobuf
implementations don't support this, so it is not very useful feature.
It has also been noted that this can complicate debugging issues with
corrupted messages.

**Changes:** `pb_decode()` now gives error when it
encounters zero tag value. A new function `pb_decode_ex()`
supports flag `PB_DECODE_NULLTERMINATED` that supports
decoding null terminated messages.

**Required actions:** If application uses null termination for messages,
switch it to use `pb_decode_ex()` and
`pb_encode_ex()`. If compatibility with 0.3.9.x is needed,
there are also `pb_decode_nullterminated()` and
`pb_encode_nullterminated()` macros, which work both in
0.4.0 and 0.3.9.

**Error indications:** Error message from `pb_decode()`: `zero_tag`.

### Submessages now have has_field in proto3 mode

**Rationale:** Previously nanopb considered proto3 submessages as
present only when their contents was non-zero. Most other protobuf
libraries allow explicit null state for submessages.

**Changes:** Submessages now have separate `has_field` in
proto3 mode also.

**Required actions:** When using submessages in proto3 mode, user code
must now set `mymsg.has_submsg = true` for each submessage
that is present. Alternatively, the field option
`proto3_singular_msgs` can be used to restore the old
behavior.

**Error indications:** Submessages do not get encoded.

### PB_OLD_CALLBACK_STYLE option has been removed

**Rationale:** Back in 2013, function signature for callbacks was
changed. The `PB_OLD_CALLBACK_STYLE` option allowed
compatibility with old code, but complicated code and testing because of
the different options.

**Changes:** `PB_OLD_CALLBACK_STYLE` option no-longer has
any effect.

**Required actions:** If `PB_OLD_CALLBACK_STYLE` option
was in use previously, function signatures must be updated to use double
pointers (`void**` and `void * const *`).

**Error indications:** Assignment from incompatible pointer type.

### protoc insertion points are no longer included by default

**Rationale:** Protoc allows including comments in form
`@@protoc_insertion_point` to identify locations for
other plugins to insert their own extra content. Previously these were
included by default, but they clutter the generated files and are rarely
used.

**Changes:** Insertion points are now included only when
`--protoc-insertion-points` option is passed to the
generator.

Nanopb-0.3.9.4, 0.4.0 (2019-10-13)
----------------------------------

### Fix generation of min/max defines for enum types

**Rationale:** Nanopb generator makes \#defines for enum minimum and
maximum value. Previously these defines incorrectly had the first and
last enum value, instead of the actual minimum and maximum. (issue
#405)

**Changes:** Minimum define now always has the smallest value, and
maximum define always has the largest value.

**Required actions:** If these defines are used and enum values in
.proto file are not defined in ascending order, user code behaviour may
change. Check that user code doesn\'t expect the old, incorrect
first/last behaviour.

### Fix undefined behavior related to bool fields

**Rationale:** In C99, `bool` variables are not allowed to
have other values than `true` and `false`.
Compilers use this fact in optimization, and constructs like
`int foo = msg.has_field ? 100 : 0;` will give unexpected results
otherwise. Previously nanopb didn\'t enforce that decoded bool fields
had valid values.

**Changes:** Bool fields are now handled separately as
`PB_LTYPE_BOOL`. The `LTYPE` descriptor
numbers for other field types were renumbered.

**Required actions:** Source code files must be recompiled, but
regenerating `.pb.h`/`.pb.c` files from
`.proto` is not required. If user code directly uses the
nanopb internal field representation (search for
`PB_LTYPE_VARINT` in source), it may need updating.

Nanopb-0.3.9.1, 0.4.0 (2018-04-14)
----------------------------------

### Fix handling of string and bytes default values

**Rationale:** Previously nanopb didn't properly decode special
character escapes like `\200` emitted by protoc. This caused these
escapes to end up verbatim in the default values in .pb.c file.

**Changes:** Escapes are now decoded, and e.g. `\200` or `\x80`
results in {0x80} for bytes field and `"\x80"` for string field.

**Required actions:** If code has previously relied on `\` in default
value being passed through verbatim, it must now be changed to `\\`.

Nanopb-0.3.8 (2017-03-05)
-------------------------

### Fully drain substreams before closing

**Rationale:** If the substream functions were called directly and the
caller did not completely empty the substring before closing it, the
parent stream would be put into an incorrect state.

**Changes:** `pb_close_string_substream` can now error and returns a
boolean.

**Required actions:** Add error checking onto any call to
`pb_close_string_substream`.

### Change oneof format in .pb.c files

**Rationale:** Previously two oneofs in a single message would be
erroneously handled as part of the same union.

**Changes:** Oneofs fields now use special `PB_DATAOFFSET_UNION`
offset type in generated .pb.c files to distinguish whether they are the
first or following field inside an union.

**Required actions:** Regenerate `.pb.c/.pb.h` files with new nanopb
version if oneofs are used.

Nanopb-0.3.5 (2016-02-13)
-------------------------

### Add support for platforms without uint8_t

**Rationale:** Some platforms cannot access 8-bit sized values directly,
and do not define `uint8_t`. Nanopb previously didn\'t support these
platforms.

**Changes:** References to `uint8_t` were replaced with several
alternatives, one of them being a new `pb_byte_t` typedef. This in
turn uses `uint_least8_t` which means the smallest available type.

**Required actions:** If your platform does not have a
standards-compliant `stdint.h`, it may lack the definition for
`[u]int_least8_t`. This must be added manually, example can be found
in `extra/pb_syshdr.h`.

**Error indications:** Compiler error: `"unknown type name 'uint_least8_t'"`.

Nanopb-0.3.2 (2015-01-24)
-------------------------

### Add support for OneOfs

**Rationale:** Previously nanopb did not support the `oneof` construct
in `.proto` files. Those fields were generated as regular `optional`
fields.

**Changes:** OneOfs are now generated as C unions. Callback fields are
not supported inside oneof and generator gives an error.

**Required actions:** The generator option `no_unions` can be used to
restore old behaviour and to allow callbacks to be used. To use unions,
one change is needed: use `which_xxxx` field to detect which field is
present, instead of `has_xxxx`. Compare the value against
`MyStruct_myfield_tag`.

**Error indications:** Generator error: `"Callback fields inside of
oneof are not supported"`. Compiler error: `"Message"` has no member
named `"has_xxxx"`.

Nanopb-0.3.0 (2014-08-26)
-------------------------

### Separate field iterator logic to pb_common.c

**Rationale:** Originally, the field iteration logic was simple enough
to be duplicated in `pb_decode.c` and `pb_encode.c`. New field types
have made the logic more complex, which required the creation of a new
file to contain the common functionality.

**Changes:** There is a new file, `pb_common.c`, which must be included
in builds.

**Required actions:** Add `pb_common.c` to build rules. This file is
always required. Either `pb_decode.c` or `pb_encode.c` can still be
left out if some functionality is not needed.

**Error indications:** Linker error: undefined reference to
`pb_field_iter_begin`, `pb_field_iter_next` or similar.

### Change data type of field counts to pb_size_t

**Rationale:** Often nanopb is used with small arrays, such as 255 items
or less. Using a full `size_t` field to store the array count wastes
memory if there are many arrays. There already exists parameters
`PB_FIELD_16BIT` and `PB_FIELD_32BIT` which tell nanopb what is the
maximum size of arrays in use.

**Changes:** Generator will now use `pb_size_t` for the array
`_count` fields. The size of the type will be controlled by the
`PB_FIELD_16BIT` and `PB_FIELD_32BIT` compilation time options.

**Required actions:** Regenerate all `.pb.h` files. In some cases casts
to the `pb_size_t` type may need to be added in the user code when
accessing the `_count` fields.

**Error indications:** Incorrect data at runtime, crashes. But note that
other changes in the same version already require regenerating the files
and have better indications of errors, so this is only an issue for
development versions.

### Renamed some macros and identifiers

**Rationale:** Some names in nanopb core were badly chosen and
conflicted with ISO C99 reserved names or lacked a prefix. While they
haven\'t caused trouble so far, it is reasonable to switch to
non-conflicting names as these are rarely used from user code.

**Changes:** The following identifier names have changed:

 -   Macros:
     -   STATIC_ASSERT(x) -> PB_STATIC_ASSERT(x)
     -   UNUSED(x) -> PB_UNUSED(x)
 -   Include guards:
     -   PB_filename -> PB_filename_INCLUDED
 -   Structure forward declaration tags:
     -   _pb_field_t -> pb_field_s
     -   _pb_bytes_array_t -> pb_bytes_array_s
     -   _pb_callback_t -> pb_callback_s
     -   _pb_extension_type_t -> pb_extension_type_s
     -   _pb_extension_t -> pb_extension_s
     -   _pb_istream_t -> pb_istream_s
     -   _pb_ostream_t -> pb_ostream_s

**Required actions:** Regenerate all `.pb.c` files. If you use any of
the above identifiers in your application code, perform search-replace
to the new name.

**Error indications:** Compiler errors on lines with the macro/type
names.

Nanopb-0.2.9 (2014-08-09)
-------------------------

### Change semantics of generator -e option

**Rationale:** Some compilers do not accept filenames with two dots
(like in default extension .pb.c). The `-e` option to the generator
allowed changing the extension, but not skipping the extra dot.

**Changes:** The `-e` option in generator will no longer add the
prepending dot. The default value has been adjusted accordingly to
`.pb.c` to keep the default behaviour the same as before.

**Required actions:** Only if using the generator -e option. Add dot
before the parameter value on the command line.

**Error indications:** File not found when trying to compile generated
files.

Nanopb-0.2.7 (2014-04-07)
-------------------------

### Changed pointer-type bytes field datatype

**Rationale:** In the initial pointer encoding support since
nanopb-0.2.5, the bytes type used a separate `pb_bytes_ptr_t` type to
represent `bytes` fields. This made it easy to encode data from a
separate, user-allocated buffer. However, it made the internal logic
more complex and was inconsistent with the other types.

**Changes:** Dynamically allocated bytes fields now have the
`pb_bytes_array_t` type, just like statically allocated ones.

**Required actions:** Only if using pointer-type fields with the bytes
datatype. Change any access to `msg->field.size` to
`msg->field->size`. Change any allocation to reserve space of amount
`PB_BYTES_ARRAY_T_ALLOCSIZE(n)`. If the data pointer was begin
assigned from external source, implement the field using a callback
function instead.

**Error indications:** Compiler error: unknown type name
`pb_bytes_ptr_t`.

Nanopb-0.2.4 (2013-11-07)
-------------------------

### Remove the NANOPB_INTERNALS compilation option

**Rationale:** Having the option in the headers required the functions
to be non-static, even if the option is not used. This caused errors on
some static analysis tools.

**Changes:** The `\#ifdef` and associated functions were removed from
the header.

**Required actions:** Only if the `NANOPB_INTERNALS` option was
previously used. Actions are as listed under nanopb-0.1.3 and
nanopb-0.1.6.

**Error indications:** Compiler warning: implicit declaration of
function `pb_dec_string`, `pb_enc_string`, or similar.

Nanopb-0.2.1 (2013-04-14)
-------------------------

### Callback function signature

**Rationale:** Previously the auxilary data to field callbacks was
passed as `void*`. This allowed passing of any data, but made it
unnecessarily complex to return a pointer from callback.

**Changes:** The callback function parameter was changed to `void**`.

**Required actions:** You can continue using the old callback style by
defining `PB_OLD_CALLBACK_STYLE`. Recommended action is to:

-   Change the callback signatures to contain `void**` for decoders and `void * const *` for encoders.
-   Change the callback function body to use **arg` instead of `arg`.

**Error indications:** Compiler warning: assignment from incompatible
pointer type, when initializing `funcs.encode` or `funcs.decode`.

Nanopb-0.2.0 (2013-03-02)
-------------------------

### Reformatted generated .pb.c file using macros

**Rationale:** Previously the generator made a list of C `pb_field_t`
initializers in the .pb.c file. This led to a need to regenerate all
.pb.c files after even small changes to the `pb_field_t` definition.

**Changes:** Macros were added to pb.h which allow for cleaner
definition of the .pb.c contents. By changing the macro definitions,
changes to the field structure are possible without breaking
compatibility with old .pb.c files.

**Required actions:** Regenerate all .pb.c files from the .proto
sources.

**Error indications:** Compiler warning: implicit declaration of
function `pb_delta_end`.

### Changed pb_type_t definitions

**Rationale:** The `pb_type_t` was previously an enumeration type.
This caused warnings on some compilers when using bitwise operations to
set flags inside the values.

**Changes:** The `pb_type_t` was changed to *typedef uint8_t*. The
values were changed to `#define`. Some value names were changed for
consistency.

**Required actions:** Only if you directly access the
`pb_field_t` contents in your own code, something which is
not usually done. Needed changes:

-   Change `PB_HTYPE_ARRAY` to `PB_HTYPE_REPEATED`.
-   Change `PB_HTYPE_CALLBACK` to `PB_ATYPE()` and `PB_ATYPE_CALLBACK`.

**Error indications:** Compiler error: `PB_HTYPE_ARRAY` or
`PB_HTYPE_CALLBACK` undeclared.

Nanopb-0.1.6 (2012-09-02)
-------------------------

### Refactored field decoder interface

**Rationale:** Similarly to field encoders in nanopb-0.1.3.

**Changes:** New functions with names `pb_decode_*` were added.

**Required actions:** By defining NANOPB_INTERNALS, you can still keep
using the old functions. Recommended action is to replace any calls with
the newer `pb_decode_*` equivalents.

**Error indications:** Compiler warning: implicit declaration of
function `pb_dec_string`, `pb_dec_varint`, `pb_dec_submessage` or
similar.

Nanopb-0.1.3 (2012-06-12)
-------------------------

### Refactored field encoder interface

**Rationale:** The old `pb_enc_*` functions were designed mostly for
the internal use by the core. Because they are internally accessed
through function pointers, their signatures had to be common. This led
to a confusing interface for external users.

**Changes:** New functions with names `pb_encode_*` were added. These
have easier to use interfaces. The old functions are now only thin
wrappers for the new interface.

**Required actions:** By defining NANOPB_INTERNALS, you can still keep
using the old functions. Recommended action is to replace any calls with
the newer `pb_encode_*` equivalents.

**Error indications:** Compiler warning: implicit declaration of
function `pb_enc_string`, *pb_enc_varint,`pb_enc_submessage\` or
similar.
