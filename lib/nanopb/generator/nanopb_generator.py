#!/usr/bin/env python3
# kate: replace-tabs on; indent-width 4;

from __future__ import unicode_literals

'''Generate header file for nanopb from a ProtoBuf FileDescriptorSet.'''
nanopb_version = "nanopb-0.4.5"

import sys
import re
import codecs
import copy
import itertools
import tempfile
import shutil
import os
from functools import reduce

try:
    # Add some dummy imports to keep packaging tools happy.
    import google, distutils.util # bbfreeze seems to need these
    import pkg_resources # pyinstaller / protobuf 2.5 seem to need these
    import proto.nanopb_pb2 as nanopb_pb2 # pyinstaller seems to need this
    import pkg_resources.py2_warn
except:
    # Don't care, we will error out later if it is actually important.
    pass

try:
    # Make sure grpc_tools gets included in binary package if it is available
    import grpc_tools.protoc
except:
    pass

try:
    import google.protobuf.text_format as text_format
    import google.protobuf.descriptor_pb2 as descriptor
    import google.protobuf.compiler.plugin_pb2 as plugin_pb2
    import google.protobuf.reflection as reflection
    import google.protobuf.descriptor
except:
    sys.stderr.write('''
         *************************************************************
         *** Could not import the Google protobuf Python libraries ***
         *** Try installing package 'python3-protobuf' or similar.  ***
         *************************************************************
    ''' + '\n')
    raise

try:
    from .proto import nanopb_pb2
    from .proto._utils import invoke_protoc
except TypeError:
    sys.stderr.write('''
         ****************************************************************************
         *** Got TypeError when importing the protocol definitions for generator. ***
         *** This usually means that the protoc in your path doesn't match the    ***
         *** Python protobuf library version.                                     ***
         ***                                                                      ***
         *** Please check the output of the following commands:                   ***
         *** which protoc                                                         ***
         *** protoc --version                                                     ***
         *** python3 -c 'import google.protobuf; print(google.protobuf.__file__)'  ***
         *** If you are not able to find the python protobuf version using the    ***
         *** above command, use this command.                                     ***
         *** pip freeze | grep -i protobuf                                        ***
         ****************************************************************************
    ''' + '\n')
    raise
except (ValueError, SystemError, ImportError):
    # Probably invoked directly instead of via installed scripts.
    import proto.nanopb_pb2 as nanopb_pb2
    from proto._utils import invoke_protoc
except:
    sys.stderr.write('''
         ********************************************************************
         *** Failed to import the protocol definitions for generator.     ***
         *** You have to run 'make' in the nanopb/generator/proto folder. ***
         ********************************************************************
    ''' + '\n')
    raise

try:
    from tempfile import TemporaryDirectory
except ImportError:
    class TemporaryDirectory:
        '''TemporaryDirectory fallback for Python 2'''
        def __enter__(self):
            self.dir = tempfile.mkdtemp()
            return self.dir

        def __exit__(self, *args):
            shutil.rmtree(self.dir)

# ---------------------------------------------------------------------------
#                     Generation of single fields
# ---------------------------------------------------------------------------

import time
import os.path

# Values are tuple (c type, pb type, encoded size, data_size)
FieldD = descriptor.FieldDescriptorProto
datatypes = {
    FieldD.TYPE_BOOL:       ('bool',     'BOOL',        1,  4),
    FieldD.TYPE_DOUBLE:     ('double',   'DOUBLE',      8,  8),
    FieldD.TYPE_FIXED32:    ('uint32_t', 'FIXED32',     4,  4),
    FieldD.TYPE_FIXED64:    ('uint64_t', 'FIXED64',     8,  8),
    FieldD.TYPE_FLOAT:      ('float',    'FLOAT',       4,  4),
    FieldD.TYPE_INT32:      ('int32_t',  'INT32',      10,  4),
    FieldD.TYPE_INT64:      ('int64_t',  'INT64',      10,  8),
    FieldD.TYPE_SFIXED32:   ('int32_t',  'SFIXED32',    4,  4),
    FieldD.TYPE_SFIXED64:   ('int64_t',  'SFIXED64',    8,  8),
    FieldD.TYPE_SINT32:     ('int32_t',  'SINT32',      5,  4),
    FieldD.TYPE_SINT64:     ('int64_t',  'SINT64',     10,  8),
    FieldD.TYPE_UINT32:     ('uint32_t', 'UINT32',      5,  4),
    FieldD.TYPE_UINT64:     ('uint64_t', 'UINT64',     10,  8),

    # Integer size override options
    (FieldD.TYPE_INT32,   nanopb_pb2.IS_8):   ('int8_t',   'INT32', 10,  1),
    (FieldD.TYPE_INT32,  nanopb_pb2.IS_16):   ('int16_t',  'INT32', 10,  2),
    (FieldD.TYPE_INT32,  nanopb_pb2.IS_32):   ('int32_t',  'INT32', 10,  4),
    (FieldD.TYPE_INT32,  nanopb_pb2.IS_64):   ('int64_t',  'INT32', 10,  8),
    (FieldD.TYPE_SINT32,  nanopb_pb2.IS_8):   ('int8_t',  'SINT32',  2,  1),
    (FieldD.TYPE_SINT32, nanopb_pb2.IS_16):   ('int16_t', 'SINT32',  3,  2),
    (FieldD.TYPE_SINT32, nanopb_pb2.IS_32):   ('int32_t', 'SINT32',  5,  4),
    (FieldD.TYPE_SINT32, nanopb_pb2.IS_64):   ('int64_t', 'SINT32', 10,  8),
    (FieldD.TYPE_UINT32,  nanopb_pb2.IS_8):   ('uint8_t', 'UINT32',  2,  1),
    (FieldD.TYPE_UINT32, nanopb_pb2.IS_16):   ('uint16_t','UINT32',  3,  2),
    (FieldD.TYPE_UINT32, nanopb_pb2.IS_32):   ('uint32_t','UINT32',  5,  4),
    (FieldD.TYPE_UINT32, nanopb_pb2.IS_64):   ('uint64_t','UINT32', 10,  8),
    (FieldD.TYPE_INT64,   nanopb_pb2.IS_8):   ('int8_t',   'INT64', 10,  1),
    (FieldD.TYPE_INT64,  nanopb_pb2.IS_16):   ('int16_t',  'INT64', 10,  2),
    (FieldD.TYPE_INT64,  nanopb_pb2.IS_32):   ('int32_t',  'INT64', 10,  4),
    (FieldD.TYPE_INT64,  nanopb_pb2.IS_64):   ('int64_t',  'INT64', 10,  8),
    (FieldD.TYPE_SINT64,  nanopb_pb2.IS_8):   ('int8_t',  'SINT64',  2,  1),
    (FieldD.TYPE_SINT64, nanopb_pb2.IS_16):   ('int16_t', 'SINT64',  3,  2),
    (FieldD.TYPE_SINT64, nanopb_pb2.IS_32):   ('int32_t', 'SINT64',  5,  4),
    (FieldD.TYPE_SINT64, nanopb_pb2.IS_64):   ('int64_t', 'SINT64', 10,  8),
    (FieldD.TYPE_UINT64,  nanopb_pb2.IS_8):   ('uint8_t', 'UINT64',  2,  1),
    (FieldD.TYPE_UINT64, nanopb_pb2.IS_16):   ('uint16_t','UINT64',  3,  2),
    (FieldD.TYPE_UINT64, nanopb_pb2.IS_32):   ('uint32_t','UINT64',  5,  4),
    (FieldD.TYPE_UINT64, nanopb_pb2.IS_64):   ('uint64_t','UINT64', 10,  8),
}

class Globals:
    '''Ugly global variables, should find a good way to pass these.'''
    verbose_options = False
    separate_options = []
    matched_namemasks = set()
    protoc_insertion_points = False

# String types (for python 2 / python 3 compatibility)
try:
    strtypes = (unicode, str)
    openmode_unicode = 'rU'
except NameError:
    strtypes = (str, )
    openmode_unicode = 'r'


class Names:
    '''Keeps a set of nested names and formats them to C identifier.'''
    def __init__(self, parts = ()):
        if isinstance(parts, Names):
            parts = parts.parts
        elif isinstance(parts, strtypes):
            parts = (parts,)
        self.parts = tuple(parts)

    def __str__(self):
        return '_'.join(self.parts)

    def __add__(self, other):
        if isinstance(other, strtypes):
            return Names(self.parts + (other,))
        elif isinstance(other, Names):
            return Names(self.parts + other.parts)
        elif isinstance(other, tuple):
            return Names(self.parts + other)
        else:
            raise ValueError("Name parts should be of type str")

    def __eq__(self, other):
        return isinstance(other, Names) and self.parts == other.parts

    def __lt__(self, other):
        if not isinstance(other, Names):
            return NotImplemented
        return str(self) < str(other)

def names_from_type_name(type_name):
    '''Parse Names() from FieldDescriptorProto type_name'''
    if type_name[0] != '.':
        raise NotImplementedError("Lookup of non-absolute type names is not supported")
    return Names(type_name[1:].split('.'))

def varint_max_size(max_value):
    '''Returns the maximum number of bytes a varint can take when encoded.'''
    if max_value < 0:
        max_value = 2**64 - max_value
    for i in range(1, 11):
        if (max_value >> (i * 7)) == 0:
            return i
    raise ValueError("Value too large for varint: " + str(max_value))

assert varint_max_size(-1) == 10
assert varint_max_size(0) == 1
assert varint_max_size(127) == 1
assert varint_max_size(128) == 2

class EncodedSize:
    '''Class used to represent the encoded size of a field or a message.
    Consists of a combination of symbolic sizes and integer sizes.'''
    def __init__(self, value = 0, symbols = [], declarations = [], required_defines = []):
        if isinstance(value, EncodedSize):
            self.value = value.value
            self.symbols = value.symbols
            self.declarations = value.declarations
            self.required_defines = value.required_defines
        elif isinstance(value, strtypes + (Names,)):
            self.symbols = [str(value)]
            self.value = 0
            self.declarations = []
            self.required_defines = [str(value)]
        else:
            self.value = value
            self.symbols = symbols
            self.declarations = declarations
            self.required_defines = required_defines

    def __add__(self, other):
        if isinstance(other, int):
            return EncodedSize(self.value + other, self.symbols, self.declarations, self.required_defines)
        elif isinstance(other, strtypes + (Names,)):
            return EncodedSize(self.value, self.symbols + [str(other)], self.declarations, self.required_defines + [str(other)])
        elif isinstance(other, EncodedSize):
            return EncodedSize(self.value + other.value, self.symbols + other.symbols,
                               self.declarations + other.declarations, self.required_defines + other.required_defines)
        else:
            raise ValueError("Cannot add size: " + repr(other))

    def __mul__(self, other):
        if isinstance(other, int):
            return EncodedSize(self.value * other, [str(other) + '*' + s for s in self.symbols],
                               self.declarations, self.required_defines)
        else:
            raise ValueError("Cannot multiply size: " + repr(other))

    def __str__(self):
        if not self.symbols:
            return str(self.value)
        else:
            return '(' + str(self.value) + ' + ' + ' + '.join(self.symbols) + ')'

    def get_declarations(self):
        '''Get any declarations that must appear alongside this encoded size definition,
        such as helper union {} types.'''
        return '\n'.join(self.declarations)

    def get_cpp_guard(self, local_defines):
        '''Get an #if preprocessor statement listing all defines that are required for this definition.'''
        needed = [x for x in self.required_defines if x not in local_defines]
        if needed:
            return '#if ' + ' && '.join(['defined(%s)' % x for x in needed]) + "\n"
        else:
            return ''

    def upperlimit(self):
        if not self.symbols:
            return self.value
        else:
            return 2**32 - 1


'''
Constants regarding path of proto elements in file descriptor.
They are used to connect proto elements with source code information (comments)
These values come from:
    https://github.com/google/protobuf/blob/master/src/google/protobuf/descriptor.proto
'''
MESSAGE_PATH = 4
ENUM_PATH = 5
FIELD_PATH = 2


class ProtoElement(object):
    def __init__(self, path, index, comments):
        '''
        path is a predefined value for each element type in proto file.
            For example, message == 4, enum == 5, service == 6
        index is the N-th occurance of the `path` in the proto file.
            For example, 4-th message in the proto file or 2-nd enum etc ...
        comments is a dictionary mapping between element path & SourceCodeInfo.Location
            (contains information about source comments).
        '''
        self.path = path
        self.index = index
        self.comments = comments

    def element_path(self):
        '''Get path to proto element.'''
        return [self.path, self.index]

    def member_path(self, member_index):
        '''Get path to member of proto element.
        Example paths:
        [4, m] - message comments, m: msgIdx in proto from 0
        [4, m, 2, f] - field comments in message, f: fieldIdx in message from 0
        [6, s] - service comments, s: svcIdx in proto from 0
        [6, s, 2, r] - rpc comments in service, r: rpc method def in service from 0
        '''
        return self.element_path() + [FIELD_PATH, member_index]

    def get_comments(self, path, leading_indent=True):
        '''Get leading & trailing comments for enum member based on path.

        path is the proto path of an element or member (ex. [5 0] or [4 1 2 0])
        leading_indent is a flag that indicates if leading comments should be indented
        '''

        # Obtain SourceCodeInfo.Location object containing comment
        # information (based on the member path)
        comment = self.comments.get(str(path))

        leading_comment = ""
        trailing_comment = ""

        if not comment:
            return leading_comment, trailing_comment

        if comment.leading_comments:
            leading_comment = "    " if leading_indent else ""
            leading_comment += "/* %s */" % comment.leading_comments.strip()

        if comment.trailing_comments:
            trailing_comment = "/* %s */" % comment.trailing_comments.strip()

        return leading_comment, trailing_comment


class Enum(ProtoElement):
    def __init__(self, names, desc, enum_options, index, comments):
        '''
        desc is EnumDescriptorProto
        index is the index of this enum element inside the file
        comments is a dictionary mapping between element path & SourceCodeInfo.Location
            (contains information about source comments)
        '''
        super(Enum, self).__init__(ENUM_PATH, index, comments)

        self.options = enum_options
        self.names = names

        # by definition, `names` include this enum's name
        base_name = Names(names.parts[:-1])

        if enum_options.long_names:
            self.values = [(names + x.name, x.number) for x in desc.value]
        else:
            self.values = [(base_name + x.name, x.number) for x in desc.value]

        self.value_longnames = [self.names + x.name for x in desc.value]
        self.packed = enum_options.packed_enum

    def has_negative(self):
        for n, v in self.values:
            if v < 0:
                return True
        return False

    def encoded_size(self):
        return max([varint_max_size(v) for n,v in self.values])

    def __str__(self):
        enum_path = self.element_path()
        leading_comment, trailing_comment = self.get_comments(enum_path, leading_indent=False)

        result = ''
        if leading_comment:
            result = '%s\n' % leading_comment

        result += 'typedef enum _%s { %s\n' % (self.names, trailing_comment)

        enum_length = len(self.values)
        enum_values = []
        for index, (name, value) in enumerate(self.values):
            member_path = self.member_path(index)
            leading_comment, trailing_comment = self.get_comments(member_path)

            if leading_comment:
                enum_values.append(leading_comment)

            comma = ","
            if index == enum_length - 1:
                # last enum member should not end with a comma
                comma = ""

            enum_values.append("    %s = %d%s %s" % (name, value, comma, trailing_comment))

        result += '\n'.join(enum_values)
        result += '\n}'

        if self.packed:
            result += ' pb_packed'

        result += ' %s;' % self.names
        return result

    def auxiliary_defines(self):
        # sort the enum by value
        sorted_values = sorted(self.values, key = lambda x: (x[1], x[0]))
        result  = '#define _%s_MIN %s\n' % (self.names, sorted_values[0][0])
        result += '#define _%s_MAX %s\n' % (self.names, sorted_values[-1][0])
        result += '#define _%s_ARRAYSIZE ((%s)(%s+1))\n' % (self.names, self.names, sorted_values[-1][0])

        if not self.options.long_names:
            # Define the long names always so that enum value references
            # from other files work properly.
            for i, x in enumerate(self.values):
                result += '#define %s %s\n' % (self.value_longnames[i], x[0])

        if self.options.enum_to_string:
            result += 'const char *%s_name(%s v);\n' % (self.names, self.names)

        return result

    def enum_to_string_definition(self):
        if not self.options.enum_to_string:
            return ""

        result = 'const char *%s_name(%s v) {\n' % (self.names, self.names)
        result += '    switch (v) {\n'

        for ((enumname, _), strname) in zip(self.values, self.value_longnames):
            # Strip off the leading type name from the string value.
            strval = str(strname)[len(str(self.names)) + 1:]
            result += '        case %s: return "%s";\n' % (enumname, strval)

        result += '    }\n'
        result += '    return "unknown";\n'
        result += '}\n'

        return result

class FieldMaxSize:
    def __init__(self, worst = 0, checks = [], field_name = 'undefined'):
        if isinstance(worst, list):
            self.worst = max(i for i in worst if i is not None)
        else:
            self.worst = worst

        self.worst_field = field_name
        self.checks = list(checks)

    def extend(self, extend, field_name = None):
        self.worst = max(self.worst, extend.worst)

        if self.worst == extend.worst:
            self.worst_field = extend.worst_field

        self.checks.extend(extend.checks)

class Field:
    macro_x_param = 'X'
    macro_a_param = 'a'

    def __init__(self, struct_name, desc, field_options):
        '''desc is FieldDescriptorProto'''
        self.tag = desc.number
        self.struct_name = struct_name
        self.union_name = None
        self.name = desc.name
        self.default = None
        self.max_size = None
        self.max_count = None
        self.array_decl = ""
        self.enc_size = None
        self.data_item_size = None
        self.ctype = None
        self.fixed_count = False
        self.callback_datatype = field_options.callback_datatype
        self.math_include_required = False
        self.sort_by_tag = field_options.sort_by_tag

        if field_options.type == nanopb_pb2.FT_INLINE:
            # Before nanopb-0.3.8, fixed length bytes arrays were specified
            # by setting type to FT_INLINE. But to handle pointer typed fields,
            # it makes sense to have it as a separate option.
            field_options.type = nanopb_pb2.FT_STATIC
            field_options.fixed_length = True

        # Parse field options
        if field_options.HasField("max_size"):
            self.max_size = field_options.max_size

        self.default_has = field_options.default_has

        if desc.type == FieldD.TYPE_STRING and field_options.HasField("max_length"):
            # max_length overrides max_size for strings
            self.max_size = field_options.max_length + 1

        if field_options.HasField("max_count"):
            self.max_count = field_options.max_count

        if desc.HasField('default_value'):
            self.default = desc.default_value

        # Check field rules, i.e. required/optional/repeated.
        can_be_static = True
        if desc.label == FieldD.LABEL_REPEATED:
            self.rules = 'REPEATED'
            if self.max_count is None:
                can_be_static = False
            else:
                self.array_decl = '[%d]' % self.max_count
                if field_options.fixed_count:
                  self.rules = 'FIXARRAY'

        elif field_options.proto3:
            if desc.type == FieldD.TYPE_MESSAGE and not field_options.proto3_singular_msgs:
                # In most other protobuf libraries proto3 submessages have
                # "null" status. For nanopb, that is implemented as has_ field.
                self.rules = 'OPTIONAL'
            elif hasattr(desc, "proto3_optional") and desc.proto3_optional:
                # Protobuf 3.12 introduced optional fields for proto3 syntax
                self.rules = 'OPTIONAL'
            else:
                # Proto3 singular fields (without has_field)
                self.rules = 'SINGULAR'
        elif desc.label == FieldD.LABEL_REQUIRED:
            self.rules = 'REQUIRED'
        elif desc.label == FieldD.LABEL_OPTIONAL:
            self.rules = 'OPTIONAL'
        else:
            raise NotImplementedError(desc.label)

        # Check if the field can be implemented with static allocation
        # i.e. whether the data size is known.
        if desc.type == FieldD.TYPE_STRING and self.max_size is None:
            can_be_static = False

        if desc.type == FieldD.TYPE_BYTES and self.max_size is None:
            can_be_static = False

        # Decide how the field data will be allocated
        if field_options.type == nanopb_pb2.FT_DEFAULT:
            if can_be_static:
                field_options.type = nanopb_pb2.FT_STATIC
            else:
                field_options.type = nanopb_pb2.FT_CALLBACK

        if field_options.type == nanopb_pb2.FT_STATIC and not can_be_static:
            raise Exception("Field '%s' is defined as static, but max_size or "
                            "max_count is not given." % self.name)

        if field_options.fixed_count and self.max_count is None:
            raise Exception("Field '%s' is defined as fixed count, "
                            "but max_count is not given." % self.name)

        if field_options.type == nanopb_pb2.FT_STATIC:
            self.allocation = 'STATIC'
        elif field_options.type == nanopb_pb2.FT_POINTER:
            self.allocation = 'POINTER'
        elif field_options.type == nanopb_pb2.FT_CALLBACK:
            self.allocation = 'CALLBACK'
        else:
            raise NotImplementedError(field_options.type)

        if field_options.HasField("type_override"):
            desc.type = field_options.type_override

        # Decide the C data type to use in the struct.
        if desc.type in datatypes:
            self.ctype, self.pbtype, self.enc_size, self.data_item_size = datatypes[desc.type]

            # Override the field size if user wants to use smaller integers
            if (desc.type, field_options.int_size) in datatypes:
                self.ctype, self.pbtype, self.enc_size, self.data_item_size = datatypes[(desc.type, field_options.int_size)]
        elif desc.type == FieldD.TYPE_ENUM:
            self.pbtype = 'ENUM'
            self.data_item_size = 4
            self.ctype = names_from_type_name(desc.type_name)
            if self.default is not None:
                self.default = self.ctype + self.default
            self.enc_size = None # Needs to be filled in when enum values are known
        elif desc.type == FieldD.TYPE_STRING:
            self.pbtype = 'STRING'
            self.ctype = 'char'
            if self.allocation == 'STATIC':
                self.ctype = 'char'
                self.array_decl += '[%d]' % self.max_size
                # -1 because of null terminator. Both pb_encode and pb_decode
                # check the presence of it.
                self.enc_size = varint_max_size(self.max_size) + self.max_size - 1
        elif desc.type == FieldD.TYPE_BYTES:
            if field_options.fixed_length:
                self.pbtype = 'FIXED_LENGTH_BYTES'

                if self.max_size is None:
                    raise Exception("Field '%s' is defined as fixed length, "
                                    "but max_size is not given." % self.name)

                self.enc_size = varint_max_size(self.max_size) + self.max_size
                self.ctype = 'pb_byte_t'
                self.array_decl += '[%d]' % self.max_size
            else:
                self.pbtype = 'BYTES'
                self.ctype = 'pb_bytes_array_t'
                if self.allocation == 'STATIC':
                    self.ctype = self.struct_name + self.name + 't'
                    self.enc_size = varint_max_size(self.max_size) + self.max_size
        elif desc.type == FieldD.TYPE_MESSAGE:
            self.pbtype = 'MESSAGE'
            self.ctype = self.submsgname = names_from_type_name(desc.type_name)
            self.enc_size = None # Needs to be filled in after the message type is available
            if field_options.submsg_callback and self.allocation == 'STATIC':
                self.pbtype = 'MSG_W_CB'
        else:
            raise NotImplementedError(desc.type)

        if self.default and self.pbtype in ['FLOAT', 'DOUBLE']:
            if 'inf' in self.default or 'nan' in self.default:
                self.math_include_required = True

    def __lt__(self, other):
        return self.tag < other.tag

    def __str__(self):
        result = ''
        if self.allocation == 'POINTER':
            if self.rules == 'REPEATED':
                if self.pbtype == 'MSG_W_CB':
                    result += '    pb_callback_t cb_' + self.name + ';\n'
                result += '    pb_size_t ' + self.name + '_count;\n'

            if self.pbtype in ['MESSAGE', 'MSG_W_CB']:
                # Use struct definition, so recursive submessages are possible
                result += '    struct _%s *%s;' % (self.ctype, self.name)
            elif self.pbtype == 'FIXED_LENGTH_BYTES' or self.rules == 'FIXARRAY':
                # Pointer to fixed size array
                result += '    %s (*%s)%s;' % (self.ctype, self.name, self.array_decl)
            elif self.rules in ['REPEATED', 'FIXARRAY'] and self.pbtype in ['STRING', 'BYTES']:
                # String/bytes arrays need to be defined as pointers to pointers
                result += '    %s **%s;' % (self.ctype, self.name)
            else:
                result += '    %s *%s;' % (self.ctype, self.name)
        elif self.allocation == 'CALLBACK':
            result += '    %s %s;' % (self.callback_datatype, self.name)
        else:
            if self.pbtype == 'MSG_W_CB' and self.rules in ['OPTIONAL', 'REPEATED']:
                result += '    pb_callback_t cb_' + self.name + ';\n'

            if self.rules == 'OPTIONAL':
                result += '    bool has_' + self.name + ';\n'
            elif self.rules == 'REPEATED':
                result += '    pb_size_t ' + self.name + '_count;\n'
            result += '    %s %s%s;' % (self.ctype, self.name, self.array_decl)
        return result

    def types(self):
        '''Return definitions for any special types this field might need.'''
        if self.pbtype == 'BYTES' and self.allocation == 'STATIC':
            result = 'typedef PB_BYTES_ARRAY_T(%d) %s;\n' % (self.max_size, self.ctype)
        else:
            result = ''
        return result

    def get_dependencies(self):
        '''Get list of type names used by this field.'''
        if self.allocation == 'STATIC':
            return [str(self.ctype)]
        else:
            return []

    def get_initializer(self, null_init, inner_init_only = False):
        '''Return literal expression for this field's default value.
        null_init: If True, initialize to a 0 value instead of default from .proto
        inner_init_only: If True, exclude initialization for any count/has fields
        '''

        inner_init = None
        if self.pbtype in ['MESSAGE', 'MSG_W_CB']:
            if null_init:
                inner_init = '%s_init_zero' % self.ctype
            else:
                inner_init = '%s_init_default' % self.ctype
        elif self.default is None or null_init:
            if self.pbtype == 'STRING':
                inner_init = '""'
            elif self.pbtype == 'BYTES':
                inner_init = '{0, {0}}'
            elif self.pbtype == 'FIXED_LENGTH_BYTES':
                inner_init = '{0}'
            elif self.pbtype in ('ENUM', 'UENUM'):
                inner_init = '_%s_MIN' % self.ctype
            else:
                inner_init = '0'
        else:
            if self.pbtype == 'STRING':
                data = codecs.escape_encode(self.default.encode('utf-8'))[0]
                inner_init = '"' + data.decode('ascii') + '"'
            elif self.pbtype == 'BYTES':
                data = codecs.escape_decode(self.default)[0]
                data = ["0x%02x" % c for c in bytearray(data)]
                if len(data) == 0:
                    inner_init = '{0, {0}}'
                else:
                    inner_init = '{%d, {%s}}' % (len(data), ','.join(data))
            elif self.pbtype == 'FIXED_LENGTH_BYTES':
                data = codecs.escape_decode(self.default)[0]
                data = ["0x%02x" % c for c in bytearray(data)]
                if len(data) == 0:
                    inner_init = '{0}'
                else:
                    inner_init = '{%s}' % ','.join(data)
            elif self.pbtype in ['FIXED32', 'UINT32']:
                inner_init = str(self.default) + 'u'
            elif self.pbtype in ['FIXED64', 'UINT64']:
                inner_init = str(self.default) + 'ull'
            elif self.pbtype in ['SFIXED64', 'INT64']:
                inner_init = str(self.default) + 'll'
            elif self.pbtype in ['FLOAT', 'DOUBLE']:
                inner_init = str(self.default)
                if 'inf' in inner_init:
                    inner_init = inner_init.replace('inf', 'INFINITY')
                elif 'nan' in inner_init:
                    inner_init = inner_init.replace('nan', 'NAN')
                elif (not '.' in inner_init) and self.pbtype == 'FLOAT':
                    inner_init += '.0f'
                elif self.pbtype == 'FLOAT':
                    inner_init += 'f'
            else:
                inner_init = str(self.default)

        if inner_init_only:
            return inner_init

        outer_init = None
        if self.allocation == 'STATIC':
            if self.rules == 'REPEATED':
                outer_init = '0, {' + ', '.join([inner_init] * self.max_count) + '}'
            elif self.rules == 'FIXARRAY':
                outer_init = '{' + ', '.join([inner_init] * self.max_count) + '}'
            elif self.rules == 'OPTIONAL':
                if null_init or not self.default_has:
                    outer_init = 'false, ' + inner_init
                else:
                    outer_init = 'true, ' + inner_init
            else:
                outer_init = inner_init
        elif self.allocation == 'POINTER':
            if self.rules == 'REPEATED':
                outer_init = '0, NULL'
            else:
                outer_init = 'NULL'
        elif self.allocation == 'CALLBACK':
            if self.pbtype == 'EXTENSION':
                outer_init = 'NULL'
            else:
                outer_init = '{{NULL}, NULL}'

        if self.pbtype == 'MSG_W_CB' and self.rules in ['REPEATED', 'OPTIONAL']:
            outer_init = '{{NULL}, NULL}, ' + outer_init

        return outer_init

    def tags(self):
        '''Return the #define for the tag number of this field.'''
        identifier = '%s_%s_tag' % (self.struct_name, self.name)
        return '#define %-40s %d\n' % (identifier, self.tag)

    def fieldlist(self):
        '''Return the FIELDLIST macro entry for this field.
        Format is: X(a, ATYPE, HTYPE, LTYPE, field_name, tag)
        '''
        name = self.name

        if self.rules == "ONEOF":
          # For oneofs, make a tuple of the union name, union member name,
          # and the name inside the parent struct.
          if not self.anonymous:
            name = '(%s,%s,%s)' % (self.union_name, self.name, self.union_name + '.' + self.name)
          else:
            name = '(%s,%s,%s)' % (self.union_name, self.name, self.name)

        return '%s(%s, %-9s %-9s %-9s %-16s %3d)' % (self.macro_x_param,
                                                     self.macro_a_param,
                                                     self.allocation + ',',
                                                     self.rules + ',',
                                                     self.pbtype + ',',
                                                     name + ',',
                                                     self.tag)

    def data_size(self, dependencies):
        '''Return estimated size of this field in the C struct.
        This is used to try to automatically pick right descriptor size.
        If the estimate is wrong, it will result in compile time error and
        user having to specify descriptor_width option.
        '''
        if self.allocation == 'POINTER' or self.pbtype == 'EXTENSION':
            size = 8
            alignment = 8
        elif self.allocation == 'CALLBACK':
            size = 16
            alignment = 8
        elif self.pbtype in ['MESSAGE', 'MSG_W_CB']:
            alignment = 8
            if str(self.submsgname) in dependencies:
                other_dependencies = dict(x for x in dependencies.items() if x[0] != str(self.struct_name))
                size = dependencies[str(self.submsgname)].data_size(other_dependencies)
            else:
                size = 256 # Message is in other file, this is reasonable guess for most cases

            if self.pbtype == 'MSG_W_CB':
                size += 16
        elif self.pbtype in ['STRING', 'FIXED_LENGTH_BYTES']:
            size = self.max_size
            alignment = 4
        elif self.pbtype == 'BYTES':
            size = self.max_size + 4
            alignment = 4
        elif self.data_item_size is not None:
            size = self.data_item_size
            alignment = 4
            if self.data_item_size >= 8:
                alignment = 8
        else:
            raise Exception("Unhandled field type: %s" % self.pbtype)

        if self.rules in ['REPEATED', 'FIXARRAY'] and self.allocation == 'STATIC':
            size *= self.max_count

        if self.rules not in ('REQUIRED', 'SINGULAR'):
            size += 4

        if size % alignment != 0:
            # Estimate how much alignment requirements will increase the size.
            size += alignment - (size % alignment)

        return size

    def encoded_size(self, dependencies):
        '''Return the maximum size that this field can take when encoded,
        including the field tag. If the size cannot be determined, returns
        None.'''

        if self.allocation != 'STATIC':
            return None

        if self.pbtype in ['MESSAGE', 'MSG_W_CB']:
            encsize = None
            if str(self.submsgname) in dependencies:
                submsg = dependencies[str(self.submsgname)]
                other_dependencies = dict(x for x in dependencies.items() if x[0] != str(self.struct_name))
                encsize = submsg.encoded_size(other_dependencies)

                my_msg = dependencies.get(str(self.struct_name))
                external = (not my_msg or submsg.protofile != my_msg.protofile)

                if encsize and encsize.symbols and external:
                    # Couldn't fully resolve the size of a dependency from
                    # another file. Instead of including the symbols directly,
                    # just use the #define SubMessage_size from the header.
                    encsize = None

                if encsize is not None:
                    # Include submessage length prefix
                    encsize += varint_max_size(encsize.upperlimit())
                elif not external:
                    # The dependency is from the same file and size cannot be
                    # determined for it, thus we know it will not be possible
                    # in runtime either.
                    return None

            if encsize is None:
                # Submessage or its size cannot be found.
                # This can occur if submessage is defined in different
                # file, and it or its .options could not be found.
                # Instead of direct numeric value, reference the size that
                # has been #defined in the other file.
                encsize = EncodedSize(self.submsgname + 'size')

                # We will have to make a conservative assumption on the length
                # prefix size, though.
                encsize += 5

        elif self.pbtype in ['ENUM', 'UENUM']:
            if str(self.ctype) in dependencies:
                enumtype = dependencies[str(self.ctype)]
                encsize = enumtype.encoded_size()
            else:
                # Conservative assumption
                encsize = 10

        elif self.enc_size is None:
            raise RuntimeError("Could not determine encoded size for %s.%s"
                               % (self.struct_name, self.name))
        else:
            encsize = EncodedSize(self.enc_size)

        encsize += varint_max_size(self.tag << 3) # Tag + wire type

        if self.rules in ['REPEATED', 'FIXARRAY']:
            # Decoders must be always able to handle unpacked arrays.
            # Therefore we have to reserve space for it, even though
            # we emit packed arrays ourselves. For length of 1, packed
            # arrays are larger however so we need to add allowance
            # for the length byte.
            encsize *= self.max_count

            if self.max_count == 1:
                encsize += 1

        return encsize

    def has_callbacks(self):
        return self.allocation == 'CALLBACK'

    def requires_custom_field_callback(self):
        return self.allocation == 'CALLBACK' and self.callback_datatype != 'pb_callback_t'

class ExtensionRange(Field):
    def __init__(self, struct_name, range_start, field_options):
        '''Implements a special pb_extension_t* field in an extensible message
        structure. The range_start signifies the index at which the extensions
        start. Not necessarily all tags above this are extensions, it is merely
        a speed optimization.
        '''
        self.tag = range_start
        self.struct_name = struct_name
        self.name = 'extensions'
        self.pbtype = 'EXTENSION'
        self.rules = 'OPTIONAL'
        self.allocation = 'CALLBACK'
        self.ctype = 'pb_extension_t'
        self.array_decl = ''
        self.default = None
        self.max_size = 0
        self.max_count = 0
        self.data_item_size = 0
        self.fixed_count = False
        self.callback_datatype = 'pb_extension_t*'

    def requires_custom_field_callback(self):
        return False

    def __str__(self):
        return '    pb_extension_t *extensions;'

    def types(self):
        return ''

    def tags(self):
        return ''

    def encoded_size(self, dependencies):
        # We exclude extensions from the count, because they cannot be known
        # until runtime. Other option would be to return None here, but this
        # way the value remains useful if extensions are not used.
        return EncodedSize(0)

class ExtensionField(Field):
    def __init__(self, fullname, desc, field_options):
        self.fullname = fullname
        self.extendee_name = names_from_type_name(desc.extendee)
        Field.__init__(self, self.fullname + "extmsg", desc, field_options)

        if self.rules != 'OPTIONAL':
            self.skip = True
        else:
            self.skip = False
            self.rules = 'REQUIRED' # We don't really want the has_field for extensions
            # currently no support for comments for extension fields => provide 0, {}
            self.msg = Message(self.fullname + "extmsg", None, field_options, 0, {})
            self.msg.fields.append(self)

    def tags(self):
        '''Return the #define for the tag number of this field.'''
        identifier = '%s_tag' % self.fullname
        return '#define %-40s %d\n' % (identifier, self.tag)

    def extension_decl(self):
        '''Declaration of the extension type in the .pb.h file'''
        if self.skip:
            msg = '/* Extension field %s was skipped because only "optional"\n' % self.fullname
            msg +='   type of extension fields is currently supported. */\n'
            return msg

        return ('extern const pb_extension_type_t %s; /* field type: %s */\n' %
            (self.fullname, str(self).strip()))

    def extension_def(self, dependencies):
        '''Definition of the extension type in the .pb.c file'''

        if self.skip:
            return ''

        result = "/* Definition for extension field %s */\n" % self.fullname
        result += str(self.msg)
        result += self.msg.fields_declaration(dependencies)
        result += 'pb_byte_t %s_default[] = {0x00};\n' % self.msg.name
        result += self.msg.fields_definition(dependencies)
        result += 'const pb_extension_type_t %s = {\n' % self.fullname
        result += '    NULL,\n'
        result += '    NULL,\n'
        result += '    &%s_msg\n' % self.msg.name
        result += '};\n'
        return result


# ---------------------------------------------------------------------------
#                   Generation of oneofs (unions)
# ---------------------------------------------------------------------------

class OneOf(Field):
    def __init__(self, struct_name, oneof_desc, oneof_options):
        self.struct_name = struct_name
        self.name = oneof_desc.name
        self.ctype = 'union'
        self.pbtype = 'oneof'
        self.fields = []
        self.allocation = 'ONEOF'
        self.default = None
        self.rules = 'ONEOF'
        self.anonymous = oneof_options.anonymous_oneof
        self.sort_by_tag = oneof_options.sort_by_tag
        self.has_msg_cb = False

    def add_field(self, field):
        field.union_name = self.name
        field.rules = 'ONEOF'
        field.anonymous = self.anonymous
        self.fields.append(field)

        if self.sort_by_tag:
            self.fields.sort()

        if field.pbtype == 'MSG_W_CB':
            self.has_msg_cb = True

        # Sort by the lowest tag number inside union
        self.tag = min([f.tag for f in self.fields])

    def __str__(self):
        result = ''
        if self.fields:
            if self.has_msg_cb:
                result += '    pb_callback_t cb_' + self.name + ';\n'

            result += '    pb_size_t which_' + self.name + ";\n"
            result += '    union {\n'
            for f in self.fields:
                result += '    ' + str(f).replace('\n', '\n    ') + '\n'
            if self.anonymous:
                result += '    };'
            else:
                result += '    } ' + self.name + ';'
        return result

    def types(self):
        return ''.join([f.types() for f in self.fields])

    def get_dependencies(self):
        deps = []
        for f in self.fields:
            deps += f.get_dependencies()
        return deps

    def get_initializer(self, null_init):
        if self.has_msg_cb:
            return '{{NULL}, NULL}, 0, {' + self.fields[0].get_initializer(null_init) + '}'
        else:
            return '0, {' + self.fields[0].get_initializer(null_init) + '}'

    def tags(self):
        return ''.join([f.tags() for f in self.fields])

    def data_size(self, dependencies):
        return max(f.data_size(dependencies) for f in self.fields)

    def encoded_size(self, dependencies):
        '''Returns the size of the largest oneof field.'''
        largest = 0
        dynamic_sizes = {}
        for f in self.fields:
            size = EncodedSize(f.encoded_size(dependencies))
            if size is None or size.value is None:
                return None
            elif size.symbols:
                dynamic_sizes[f.tag] = size
            elif size.value > largest:
                largest = size.value

        if not dynamic_sizes:
            # Simple case, all sizes were known at generator time
            return EncodedSize(largest)

        if largest > 0:
            # Some sizes were known, some were not
            dynamic_sizes[0] = EncodedSize(largest)

        # Couldn't find size for submessage at generation time,
        # have to rely on macro resolution at compile time.
        if len(dynamic_sizes) == 1:
            # Only one symbol was needed
            return list(dynamic_sizes.values())[0]
        else:
            # Use sizeof(union{}) construct to find the maximum size of
            # submessages.
            union_name = "%s_%s_size_union" % (self.struct_name, self.name)
            union_def = 'union %s {%s};\n' % (union_name, ' '.join('char f%d[%s];' % (k, s) for k,s in dynamic_sizes.items()))
            required_defs = list(itertools.chain.from_iterable(s.required_defines for k,s in dynamic_sizes.items()))
            return EncodedSize(0, ['sizeof(union %s)' % union_name], [union_def], required_defs)

    def has_callbacks(self):
        return bool([f for f in self.fields if f.has_callbacks()])

    def requires_custom_field_callback(self):
        return bool([f for f in self.fields if f.requires_custom_field_callback()])

# ---------------------------------------------------------------------------
#                   Generation of messages (structures)
# ---------------------------------------------------------------------------


class Message(ProtoElement):
    def __init__(self, names, desc, message_options, index, comments):
        super(Message, self).__init__(MESSAGE_PATH, index, comments)
        self.name = names
        self.fields = []
        self.oneofs = {}
        self.desc = desc
        self.math_include_required = False
        self.packed = message_options.packed_struct
        self.descriptorsize = message_options.descriptorsize

        if message_options.msgid:
            self.msgid = message_options.msgid

        if desc is not None:
            self.load_fields(desc, message_options)

        self.callback_function = message_options.callback_function
        if not message_options.HasField('callback_function'):
            # Automatically assign a per-message callback if any field has
            # a special callback_datatype.
            for field in self.fields:
                if field.requires_custom_field_callback():
                    self.callback_function = "%s_callback" % self.name
                    break

    def load_fields(self, desc, message_options):
        '''Load field list from DescriptorProto'''

        no_unions = []

        if hasattr(desc, 'oneof_decl'):
            for i, f in enumerate(desc.oneof_decl):
                oneof_options = get_nanopb_suboptions(desc, message_options, self.name + f.name)
                if oneof_options.no_unions:
                    no_unions.append(i) # No union, but add fields normally
                elif oneof_options.type == nanopb_pb2.FT_IGNORE:
                    pass # No union and skip fields also
                else:
                    oneof = OneOf(self.name, f, oneof_options)
                    self.oneofs[i] = oneof
        else:
            sys.stderr.write('Note: This Python protobuf library has no OneOf support\n')

        for f in desc.field:
            field_options = get_nanopb_suboptions(f, message_options, self.name + f.name)
            if field_options.type == nanopb_pb2.FT_IGNORE:
                continue

            if field_options.descriptorsize > self.descriptorsize:
                self.descriptorsize = field_options.descriptorsize

            field = Field(self.name, f, field_options)
            if hasattr(f, 'oneof_index') and f.HasField('oneof_index'):
                if hasattr(f, 'proto3_optional') and f.proto3_optional:
                    no_unions.append(f.oneof_index)

                if f.oneof_index in no_unions:
                    self.fields.append(field)
                elif f.oneof_index in self.oneofs:
                    self.oneofs[f.oneof_index].add_field(field)

                    if self.oneofs[f.oneof_index] not in self.fields:
                        self.fields.append(self.oneofs[f.oneof_index])
            else:
                self.fields.append(field)

            if field.math_include_required:
                self.math_include_required = True

        if len(desc.extension_range) > 0:
            field_options = get_nanopb_suboptions(desc, message_options, self.name + 'extensions')
            range_start = min([r.start for r in desc.extension_range])
            if field_options.type != nanopb_pb2.FT_IGNORE:
                self.fields.append(ExtensionRange(self.name, range_start, field_options))

        if message_options.sort_by_tag:
            self.fields.sort()

    def get_dependencies(self):
        '''Get list of type names that this structure refers to.'''
        deps = []
        for f in self.fields:
            deps += f.get_dependencies()
        return deps

    def __str__(self):
        message_path = self.element_path()
        leading_comment, trailing_comment = self.get_comments(message_path, leading_indent=False)

        result = ''
        if leading_comment:
            result = '%s\n' % leading_comment

        result += 'typedef struct _%s { %s\n' % (self.name, trailing_comment)

        if not self.fields:
            # Empty structs are not allowed in C standard.
            # Therefore add a dummy field if an empty message occurs.
            result += '    char dummy_field;'

        msg_fields = []
        for index, field in enumerate(self.fields):
            member_path = self.member_path(index)
            leading_comment, trailing_comment = self.get_comments(member_path)

            if leading_comment:
                msg_fields.append(leading_comment)

            msg_fields.append("%s %s" % (str(field), trailing_comment))

        result += '\n'.join(msg_fields)

        if Globals.protoc_insertion_points:
            result += '\n/* @@protoc_insertion_point(struct:%s) */' % self.name

        result += '\n}'

        if self.packed:
            result += ' pb_packed'

        result += ' %s;' % self.name

        if self.packed:
            result = 'PB_PACKED_STRUCT_START\n' + result
            result += '\nPB_PACKED_STRUCT_END'

        return result + '\n'

    def types(self):
        return ''.join([f.types() for f in self.fields])

    def get_initializer(self, null_init):
        if not self.fields:
            return '{0}'

        parts = []
        for field in self.fields:
            parts.append(field.get_initializer(null_init))
        return '{' + ', '.join(parts) + '}'

    def count_required_fields(self):
        '''Returns number of required fields inside this message'''
        count = 0
        for f in self.fields:
            if not isinstance(f, OneOf):
                if f.rules == 'REQUIRED':
                    count += 1
        return count

    def all_fields(self):
        '''Iterate over all fields in this message, including nested OneOfs.'''
        for f in self.fields:
            if isinstance(f, OneOf):
                for f2 in f.fields:
                    yield f2
            else:
                yield f


    def field_for_tag(self, tag):
        '''Given a tag number, return the Field instance.'''
        for field in self.all_fields():
            if field.tag == tag:
                return field
        return None

    def count_all_fields(self):
        '''Count the total number of fields in this message.'''
        count = 0
        for f in self.fields:
            if isinstance(f, OneOf):
                count += len(f.fields)
            else:
                count += 1
        return count

    def fields_declaration(self, dependencies):
        '''Return X-macro declaration of all fields in this message.'''
        Field.macro_x_param = 'X'
        Field.macro_a_param = 'a'
        while any(field.name == Field.macro_x_param for field in self.all_fields()):
            Field.macro_x_param += '_'
        while any(field.name == Field.macro_a_param for field in self.all_fields()):
            Field.macro_a_param += '_'

        # Field descriptor array must be sorted by tag number, pb_common.c relies on it.
        sorted_fields = list(self.all_fields())
        sorted_fields.sort(key = lambda x: x.tag)

        result = '#define %s_FIELDLIST(%s, %s) \\\n' % (self.name,
                                                        Field.macro_x_param,
                                                        Field.macro_a_param)
        result += ' \\\n'.join(x.fieldlist() for x in sorted_fields)
        result += '\n'

        has_callbacks = bool([f for f in self.fields if f.has_callbacks()])
        if has_callbacks:
            if self.callback_function != 'pb_default_field_callback':
                result += "extern bool %s(pb_istream_t *istream, pb_ostream_t *ostream, const pb_field_t *field);\n" % self.callback_function
            result += "#define %s_CALLBACK %s\n" % (self.name, self.callback_function)
        else:
            result += "#define %s_CALLBACK NULL\n" % self.name

        defval = self.default_value(dependencies)
        if defval:
            hexcoded = ''.join("\\x%02x" % ord(defval[i:i+1]) for i in range(len(defval)))
            result += '#define %s_DEFAULT (const pb_byte_t*)"%s\\x00"\n' % (self.name, hexcoded)
        else:
            result += '#define %s_DEFAULT NULL\n' % self.name

        for field in sorted_fields:
            if field.pbtype in ['MESSAGE', 'MSG_W_CB']:
                if field.rules == 'ONEOF':
                    result += "#define %s_%s_%s_MSGTYPE %s\n" % (self.name, field.union_name, field.name, field.ctype)
                else:
                    result += "#define %s_%s_MSGTYPE %s\n" % (self.name, field.name, field.ctype)

        return result

    def fields_declaration_cpp_lookup(self):
        result = 'template <>\n'
        result += 'struct MessageDescriptor<%s> {\n' % (self.name)
        result += '    static PB_INLINE_CONSTEXPR const pb_size_t fields_array_length = %d;\n' % (self.count_all_fields())
        result += '    static inline const pb_msgdesc_t* fields() {\n'
        result += '        return &%s_msg;\n' % (self.name)
        result += '    }\n'
        result += '};'
        return result

    def fields_definition(self, dependencies):
        '''Return the field descriptor definition that goes in .pb.c file.'''
        width = self.required_descriptor_width(dependencies)
        if width == 1:
          width = 'AUTO'

        result = 'PB_BIND(%s, %s, %s)\n' % (self.name, self.name, width)
        return result

    def required_descriptor_width(self, dependencies):
        '''Estimate how many words are necessary for each field descriptor.'''
        if self.descriptorsize != nanopb_pb2.DS_AUTO:
            return int(self.descriptorsize)

        if not self.fields:
          return 1

        max_tag = max(field.tag for field in self.all_fields())
        max_offset = self.data_size(dependencies)
        max_arraysize = max((field.max_count or 0) for field in self.all_fields())
        max_datasize = max(field.data_size(dependencies) for field in self.all_fields())

        if max_arraysize > 0xFFFF:
            return 8
        elif (max_tag > 0x3FF or max_offset > 0xFFFF or
              max_arraysize > 0x0FFF or max_datasize > 0x0FFF):
            return 4
        elif max_tag > 0x3F or max_offset > 0xFF:
            return 2
        else:
            # NOTE: Macro logic in pb.h ensures that width 1 will
            # be raised to 2 automatically for string/submsg fields
            # and repeated fields. Thus only tag and offset need to
            # be checked.
            return 1

    def data_size(self, dependencies):
        '''Return approximate sizeof(struct) in the compiled code.'''
        return sum(f.data_size(dependencies) for f in self.fields)

    def encoded_size(self, dependencies):
        '''Return the maximum size that this message can take when encoded.
        If the size cannot be determined, returns None.
        '''
        size = EncodedSize(0)
        for field in self.fields:
            fsize = field.encoded_size(dependencies)
            if fsize is None:
                return None
            size += fsize

        return size

    def default_value(self, dependencies):
        '''Generate serialized protobuf message that contains the
        default values for optional fields.'''

        if not self.desc:
            return b''

        if self.desc.options.map_entry:
            return b''

        optional_only = copy.deepcopy(self.desc)

        # Remove fields without default values
        # The iteration is done in reverse order to avoid remove() messing up iteration.
        for field in reversed(list(optional_only.field)):
            field.ClearField(str('extendee'))
            parsed_field = self.field_for_tag(field.number)
            if parsed_field is None or parsed_field.allocation != 'STATIC':
                optional_only.field.remove(field)
            elif (field.label == FieldD.LABEL_REPEATED or
                  field.type == FieldD.TYPE_MESSAGE):
                optional_only.field.remove(field)
            elif hasattr(field, 'oneof_index') and field.HasField('oneof_index'):
                optional_only.field.remove(field)
            elif field.type == FieldD.TYPE_ENUM:
                # The partial descriptor doesn't include the enum type
                # so we fake it with int64.
                enumname = names_from_type_name(field.type_name)
                try:
                    enumtype = dependencies[str(enumname)]
                except KeyError:
                    raise Exception("Could not find enum type %s while generating default values for %s.\n" % (enumname, self.name)
                                    + "Try passing all source files to generator at once, or use -I option.")

                if field.HasField('default_value'):
                    defvals = [v for n,v in enumtype.values if n.parts[-1] == field.default_value]
                else:
                    # If no default is specified, the default is the first value.
                    defvals = [v for n,v in enumtype.values]
                if defvals and defvals[0] != 0:
                    field.type = FieldD.TYPE_INT64
                    field.default_value = str(defvals[0])
                    field.ClearField(str('type_name'))
                else:
                    optional_only.field.remove(field)
            elif not field.HasField('default_value'):
                optional_only.field.remove(field)

        if len(optional_only.field) == 0:
            return b''

        optional_only.ClearField(str('oneof_decl'))
        optional_only.ClearField(str('nested_type'))
        optional_only.ClearField(str('extension'))
        optional_only.ClearField(str('enum_type'))
        desc = google.protobuf.descriptor.MakeDescriptor(optional_only)
        msg = reflection.MakeClass(desc)()

        for field in optional_only.field:
            if field.type == FieldD.TYPE_STRING:
                setattr(msg, field.name, field.default_value)
            elif field.type == FieldD.TYPE_BYTES:
                setattr(msg, field.name, codecs.escape_decode(field.default_value)[0])
            elif field.type in [FieldD.TYPE_FLOAT, FieldD.TYPE_DOUBLE]:
                setattr(msg, field.name, float(field.default_value))
            elif field.type == FieldD.TYPE_BOOL:
                setattr(msg, field.name, field.default_value == 'true')
            else:
                setattr(msg, field.name, int(field.default_value))

        return msg.SerializeToString()


# ---------------------------------------------------------------------------
#                    Processing of entire .proto files
# ---------------------------------------------------------------------------

def iterate_messages(desc, flatten = False, names = Names()):
    '''Recursively find all messages. For each, yield name, DescriptorProto.'''
    if hasattr(desc, 'message_type'):
        submsgs = desc.message_type
    else:
        submsgs = desc.nested_type

    for submsg in submsgs:
        sub_names = names + submsg.name
        if flatten:
            yield Names(submsg.name), submsg
        else:
            yield sub_names, submsg

        for x in iterate_messages(submsg, flatten, sub_names):
            yield x

def iterate_extensions(desc, flatten = False, names = Names()):
    '''Recursively find all extensions.
    For each, yield name, FieldDescriptorProto.
    '''
    for extension in desc.extension:
        yield names, extension

    for subname, subdesc in iterate_messages(desc, flatten, names):
        for extension in subdesc.extension:
            yield subname, extension

def toposort2(data):
    '''Topological sort.
    From http://code.activestate.com/recipes/577413-topological-sort/
    This function is under the MIT license.
    '''
    for k, v in list(data.items()):
        v.discard(k) # Ignore self dependencies
    extra_items_in_deps = reduce(set.union, list(data.values()), set()) - set(data.keys())
    data.update(dict([(item, set()) for item in extra_items_in_deps]))
    while True:
        ordered = set(item for item,dep in list(data.items()) if not dep)
        if not ordered:
            break
        for item in sorted(ordered):
            yield item
        data = dict([(item, (dep - ordered)) for item,dep in list(data.items())
                if item not in ordered])
    assert not data, "A cyclic dependency exists amongst %r" % data

def sort_dependencies(messages):
    '''Sort a list of Messages based on dependencies.'''
    dependencies = {}
    message_by_name = {}
    for message in messages:
        dependencies[str(message.name)] = set(message.get_dependencies())
        message_by_name[str(message.name)] = message

    for msgname in toposort2(dependencies):
        if msgname in message_by_name:
            yield message_by_name[msgname]

def make_identifier(headername):
    '''Make #ifndef identifier that contains uppercase A-Z and digits 0-9'''
    result = ""
    for c in headername.upper():
        if c.isalnum():
            result += c
        else:
            result += '_'
    return result

class ProtoFile:
    def __init__(self, fdesc, file_options):
        '''Takes a FileDescriptorProto and parses it.'''
        self.fdesc = fdesc
        self.file_options = file_options
        self.dependencies = {}
        self.math_include_required = False
        self.parse()
        for message in self.messages:
            if message.math_include_required:
                self.math_include_required = True
                break

        # Some of types used in this file probably come from the file itself.
        # Thus it has implicit dependency on itself.
        self.add_dependency(self)

    def parse(self):
        self.enums = []
        self.messages = []
        self.extensions = []

        mangle_names = self.file_options.mangle_names
        flatten = mangle_names == nanopb_pb2.M_FLATTEN
        strip_prefix = None
        replacement_prefix = None
        if mangle_names == nanopb_pb2.M_STRIP_PACKAGE:
            strip_prefix = "." + self.fdesc.package
        elif mangle_names == nanopb_pb2.M_PACKAGE_INITIALS:
            strip_prefix = "." + self.fdesc.package
            replacement_prefix = ""
            for part in self.fdesc.package.split("."):
                replacement_prefix += part[0]
        elif self.file_options.package:
            strip_prefix = "." + self.fdesc.package
            replacement_prefix = self.file_options.package


        def create_name(names):
            if mangle_names in (nanopb_pb2.M_NONE, nanopb_pb2.M_PACKAGE_INITIALS):
                return base_name + names
            if mangle_names == nanopb_pb2.M_STRIP_PACKAGE:
                return Names(names)
            single_name = names
            if isinstance(names, Names):
                single_name = names.parts[-1]
            return Names(single_name)

        def mangle_field_typename(typename):
            if mangle_names == nanopb_pb2.M_FLATTEN:
                return "." + typename.split(".")[-1]
            if strip_prefix is not None and typename.startswith(strip_prefix):
                if replacement_prefix is not None:
                    return "." + replacement_prefix + typename[len(strip_prefix):]
                else:
                    return typename[len(strip_prefix):]
            if self.file_options.package:
                return "." + replacement_prefix + typename
            return typename

        if replacement_prefix is not None:
            base_name = Names(replacement_prefix.split('.'))
        elif self.fdesc.package:
            base_name = Names(self.fdesc.package.split('.'))
        else:
            base_name = Names()

        # process source code comment locations
        # ignores any locations that do not contain any comment information
        self.comment_locations = {
            str(list(location.path)): location
            for location in self.fdesc.source_code_info.location
            if location.leading_comments or location.leading_detached_comments or location.trailing_comments
        }

        for index, enum in enumerate(self.fdesc.enum_type):
            name = create_name(enum.name)
            enum_options = get_nanopb_suboptions(enum, self.file_options, name)
            self.enums.append(Enum(name, enum, enum_options, index, self.comment_locations))

        for index, (names, message) in enumerate(iterate_messages(self.fdesc, flatten)):
            name = create_name(names)
            message_options = get_nanopb_suboptions(message, self.file_options, name)

            if message_options.skip_message:
                continue

            message = copy.deepcopy(message)
            for field in message.field:
                if field.type in (FieldD.TYPE_MESSAGE, FieldD.TYPE_ENUM):
                    field.type_name = mangle_field_typename(field.type_name)

            self.messages.append(Message(name, message, message_options, index, self.comment_locations))
            for index, enum in enumerate(message.enum_type):
                name = create_name(names + enum.name)
                enum_options = get_nanopb_suboptions(enum, message_options, name)
                self.enums.append(Enum(name, enum, enum_options, index, self.comment_locations))

        for names, extension in iterate_extensions(self.fdesc, flatten):
            name = create_name(names + extension.name)
            field_options = get_nanopb_suboptions(extension, self.file_options, name)

            extension = copy.deepcopy(extension)
            if extension.type in (FieldD.TYPE_MESSAGE, FieldD.TYPE_ENUM):
                extension.type_name = mangle_field_typename(extension.type_name)

            if field_options.type != nanopb_pb2.FT_IGNORE:
                self.extensions.append(ExtensionField(name, extension, field_options))

    def add_dependency(self, other):
        for enum in other.enums:
            self.dependencies[str(enum.names)] = enum
            enum.protofile = other

        for msg in other.messages:
            self.dependencies[str(msg.name)] = msg
            msg.protofile = other

        # Fix field default values where enum short names are used.
        for enum in other.enums:
            if not enum.options.long_names:
                for message in self.messages:
                    for field in message.all_fields():
                        if field.default in enum.value_longnames:
                            idx = enum.value_longnames.index(field.default)
                            field.default = enum.values[idx][0]

        # Fix field data types where enums have negative values.
        for enum in other.enums:
            if not enum.has_negative():
                for message in self.messages:
                    for field in message.all_fields():
                        if field.pbtype == 'ENUM' and field.ctype == enum.names:
                            field.pbtype = 'UENUM'

    def generate_header(self, includes, headername, options):
        '''Generate content for a header file.
        Generates strings, which should be concatenated and stored to file.
        '''

        yield '/* Automatically generated nanopb header */\n'
        if options.notimestamp:
            yield '/* Generated by %s */\n\n' % (nanopb_version)
        else:
            yield '/* Generated by %s at %s. */\n\n' % (nanopb_version, time.asctime())

        if self.fdesc.package:
            symbol = make_identifier(self.fdesc.package + '_' + headername)
        else:
            symbol = make_identifier(headername)
        yield '#ifndef PB_%s_INCLUDED\n' % symbol
        yield '#define PB_%s_INCLUDED\n' % symbol
        if self.math_include_required:
            yield '#include <math.h>\n'
        try:
            yield options.libformat % ('pb.h')
        except TypeError:
            # no %s specified - use whatever was passed in as options.libformat
            yield options.libformat
        yield '\n'

        for incfile in self.file_options.include:
            # allow including system headers
            if (incfile.startswith('<')):
                yield '#include %s\n' % incfile
            else:
                yield options.genformat % incfile
                yield '\n'

        for incfile in includes:
            noext = os.path.splitext(incfile)[0]
            yield options.genformat % (noext + options.extension + options.header_extension)
            yield '\n'

        if Globals.protoc_insertion_points:
            yield '/* @@protoc_insertion_point(includes) */\n'

        yield '\n'

        yield '#if PB_PROTO_HEADER_VERSION != 40\n'
        yield '#error Regenerate this file with the current version of nanopb generator.\n'
        yield '#endif\n'
        yield '\n'

        if self.enums:
            yield '/* Enum definitions */\n'
            for enum in self.enums:
                yield str(enum) + '\n\n'

        if self.messages:
            yield '/* Struct definitions */\n'
            for msg in sort_dependencies(self.messages):
                yield msg.types()
                yield str(msg) + '\n'
            yield '\n'

        if self.extensions:
            yield '/* Extensions */\n'
            for extension in self.extensions:
                yield extension.extension_decl()
            yield '\n'

        if self.enums:
                yield '/* Helper constants for enums */\n'
                for enum in self.enums:
                    yield enum.auxiliary_defines() + '\n'
                yield '\n'

        yield '#ifdef __cplusplus\n'
        yield 'extern "C" {\n'
        yield '#endif\n\n'

        if self.messages:
            yield '/* Initializer values for message structs */\n'
            for msg in self.messages:
                identifier = '%s_init_default' % msg.name
                yield '#define %-40s %s\n' % (identifier, msg.get_initializer(False))
            for msg in self.messages:
                identifier = '%s_init_zero' % msg.name
                yield '#define %-40s %s\n' % (identifier, msg.get_initializer(True))
            yield '\n'

            yield '/* Field tags (for use in manual encoding/decoding) */\n'
            for msg in sort_dependencies(self.messages):
                for field in msg.fields:
                    yield field.tags()
            for extension in self.extensions:
                yield extension.tags()
            yield '\n'

            yield '/* Struct field encoding specification for nanopb */\n'
            for msg in self.messages:
                yield msg.fields_declaration(self.dependencies) + '\n'
            for msg in self.messages:
                yield 'extern const pb_msgdesc_t %s_msg;\n' % msg.name
            yield '\n'

            yield '/* Defines for backwards compatibility with code written before nanopb-0.4.0 */\n'
            for msg in self.messages:
              yield '#define %s_fields &%s_msg\n' % (msg.name, msg.name)
            yield '\n'

            yield '/* Maximum encoded size of messages (where known) */\n'
            messagesizes = []
            for msg in self.messages:
                identifier = '%s_size' % msg.name
                messagesizes.append((identifier, msg.encoded_size(self.dependencies)))

            # If we require a symbol from another file, put a preprocessor if statement
            # around it to prevent compilation errors if the symbol is not actually available.
            local_defines = [identifier for identifier, msize in messagesizes if msize is not None]
            guards = {}
            for identifier, msize in messagesizes:
                if msize is not None:
                    cpp_guard = msize.get_cpp_guard(local_defines)
                    if cpp_guard not in guards:
                        guards[cpp_guard] = set()
                    for decl in msize.get_declarations().splitlines():
                        guards[cpp_guard].add(decl)
                    guards[cpp_guard].add('#define %-40s %s' % (identifier, msize))
                else:
                    yield '/* %s depends on runtime parameters */\n' % identifier
            for guard, values in guards.items():
                if guard:
                    yield guard
                for v in sorted(values):
                    yield v
                    yield '\n'
                if guard:
                    yield '#endif\n'
            yield '\n'

            if [msg for msg in self.messages if hasattr(msg,'msgid')]:
              yield '/* Message IDs (where set with "msgid" option) */\n'
              for msg in self.messages:
                  if hasattr(msg,'msgid'):
                      yield '#define PB_MSG_%d %s\n' % (msg.msgid, msg.name)
              yield '\n'

              symbol = make_identifier(headername.split('.')[0])
              yield '#define %s_MESSAGES \\\n' % symbol

              for msg in self.messages:
                  m = "-1"
                  msize = msg.encoded_size(self.dependencies)
                  if msize is not None:
                      m = msize
                  if hasattr(msg,'msgid'):
                      yield '\tPB_MSG(%d,%s,%s) \\\n' % (msg.msgid, m, msg.name)
              yield '\n'

              for msg in self.messages:
                  if hasattr(msg,'msgid'):
                      yield '#define %s_msgid %d\n' % (msg.name, msg.msgid)
              yield '\n'

        yield '#ifdef __cplusplus\n'
        yield '} /* extern "C" */\n'
        yield '#endif\n'

        if options.cpp_descriptors:
            yield '\n'
            yield '#ifdef __cplusplus\n'
            yield '/* Message descriptors for nanopb */\n'
            yield 'namespace nanopb {\n'
            for msg in self.messages:
                yield msg.fields_declaration_cpp_lookup() + '\n'
            yield '}  // namespace nanopb\n'
            yield '\n'
            yield '#endif  /* __cplusplus */\n'
            yield '\n'

        if Globals.protoc_insertion_points:
            yield '/* @@protoc_insertion_point(eof) */\n'

        # End of header
        yield '\n#endif\n'

    def generate_source(self, headername, options):
        '''Generate content for a source file.'''

        yield '/* Automatically generated nanopb constant definitions */\n'
        if options.notimestamp:
            yield '/* Generated by %s */\n\n' % (nanopb_version)
        else:
            yield '/* Generated by %s at %s. */\n\n' % (nanopb_version, time.asctime())
        yield options.genformat % (headername)
        yield '\n'

        if Globals.protoc_insertion_points:
            yield '/* @@protoc_insertion_point(includes) */\n'

        yield '#if PB_PROTO_HEADER_VERSION != 40\n'
        yield '#error Regenerate this file with the current version of nanopb generator.\n'
        yield '#endif\n'
        yield '\n'

        for msg in self.messages:
            yield msg.fields_definition(self.dependencies) + '\n\n'

        for ext in self.extensions:
            yield ext.extension_def(self.dependencies) + '\n'

        for enum in self.enums:
            yield enum.enum_to_string_definition() + '\n'

        # Add checks for numeric limits
        if self.messages:
            largest_msg = max(self.messages, key = lambda m: m.count_required_fields())
            largest_count = largest_msg.count_required_fields()
            if largest_count > 64:
                yield '\n/* Check that missing required fields will be properly detected */\n'
                yield '#if PB_MAX_REQUIRED_FIELDS < %d\n' % largest_count
                yield '#error Properly detecting missing required fields in %s requires \\\n' % largest_msg.name
                yield '       setting PB_MAX_REQUIRED_FIELDS to %d or more.\n' % largest_count
                yield '#endif\n'

        # Add check for sizeof(double)
        has_double = False
        for msg in self.messages:
            for field in msg.all_fields():
                if field.ctype == 'double':
                    has_double = True

        if has_double:
            yield '\n'
            yield '#ifndef PB_CONVERT_DOUBLE_FLOAT\n'
            yield '/* On some platforms (such as AVR), double is really float.\n'
            yield ' * To be able to encode/decode double on these platforms, you need.\n'
            yield ' * to define PB_CONVERT_DOUBLE_FLOAT in pb.h or compiler command line.\n'
            yield ' */\n'
            yield 'PB_STATIC_ASSERT(sizeof(double) == 8, DOUBLE_MUST_BE_8_BYTES)\n'
            yield '#endif\n'

        yield '\n'

        if Globals.protoc_insertion_points:
            yield '/* @@protoc_insertion_point(eof) */\n'

# ---------------------------------------------------------------------------
#                    Options parsing for the .proto files
# ---------------------------------------------------------------------------

from fnmatch import fnmatchcase

def read_options_file(infile):
    '''Parse a separate options file to list:
        [(namemask, options), ...]
    '''
    results = []
    data = infile.read()
    data = re.sub(r'/\*.*?\*/', '', data, flags = re.MULTILINE)
    data = re.sub(r'//.*?$', '', data, flags = re.MULTILINE)
    data = re.sub(r'#.*?$', '', data, flags = re.MULTILINE)
    for i, line in enumerate(data.split('\n')):
        line = line.strip()
        if not line:
            continue

        parts = line.split(None, 1)

        if len(parts) < 2:
            sys.stderr.write("%s:%d: " % (infile.name, i + 1) +
                             "Option lines should have space between field name and options. " +
                             "Skipping line: '%s'\n" % line)
            sys.exit(1)

        opts = nanopb_pb2.NanoPBOptions()

        try:
            text_format.Merge(parts[1], opts)
        except Exception as e:
            sys.stderr.write("%s:%d: " % (infile.name, i + 1) +
                             "Unparseable option line: '%s'. " % line +
                             "Error: %s\n" % str(e))
            sys.exit(1)
        results.append((parts[0], opts))

    return results

def get_nanopb_suboptions(subdesc, options, name):
    '''Get copy of options, and merge information from subdesc.'''
    new_options = nanopb_pb2.NanoPBOptions()
    new_options.CopyFrom(options)

    if hasattr(subdesc, 'syntax') and subdesc.syntax == "proto3":
        new_options.proto3 = True

    # Handle options defined in a separate file
    dotname = '.'.join(name.parts)
    for namemask, options in Globals.separate_options:
        if fnmatchcase(dotname, namemask):
            Globals.matched_namemasks.add(namemask)
            new_options.MergeFrom(options)

    # Handle options defined in .proto
    if isinstance(subdesc.options, descriptor.FieldOptions):
        ext_type = nanopb_pb2.nanopb
    elif isinstance(subdesc.options, descriptor.FileOptions):
        ext_type = nanopb_pb2.nanopb_fileopt
    elif isinstance(subdesc.options, descriptor.MessageOptions):
        ext_type = nanopb_pb2.nanopb_msgopt
    elif isinstance(subdesc.options, descriptor.EnumOptions):
        ext_type = nanopb_pb2.nanopb_enumopt
    else:
        raise Exception("Unknown options type")

    if subdesc.options.HasExtension(ext_type):
        ext = subdesc.options.Extensions[ext_type]
        new_options.MergeFrom(ext)

    if Globals.verbose_options:
        sys.stderr.write("Options for " + dotname + ": ")
        sys.stderr.write(text_format.MessageToString(new_options) + "\n")

    return new_options


# ---------------------------------------------------------------------------
#                         Command line interface
# ---------------------------------------------------------------------------

import sys
import os.path
from optparse import OptionParser

optparser = OptionParser(
    usage = "Usage: nanopb_generator.py [options] file.pb ...",
    epilog = "Compile file.pb from file.proto by: 'protoc -ofile.pb file.proto'. " +
             "Output will be written to file.pb.h and file.pb.c.")
optparser.add_option("--version", dest="version", action="store_true",
    help="Show version info and exit")
optparser.add_option("-x", dest="exclude", metavar="FILE", action="append", default=[],
    help="Exclude file from generated #include list.")
optparser.add_option("-e", "--extension", dest="extension", metavar="EXTENSION", default=".pb",
    help="Set extension to use instead of '.pb' for generated files. [default: %default]")
optparser.add_option("-H", "--header-extension", dest="header_extension", metavar="EXTENSION", default=".h",
    help="Set extension to use for generated header files. [default: %default]")
optparser.add_option("-S", "--source-extension", dest="source_extension", metavar="EXTENSION", default=".c",
    help="Set extension to use for generated source files. [default: %default]")
optparser.add_option("-f", "--options-file", dest="options_file", metavar="FILE", default="%s.options",
    help="Set name of a separate generator options file.")
optparser.add_option("-I", "--options-path", dest="options_path", metavar="DIR",
    action="append", default = [],
    help="Search for .options files additionally in this path")
optparser.add_option("--error-on-unmatched", dest="error_on_unmatched", action="store_true", default=False,
                     help ="Stop generation if there are unmatched fields in options file")
optparser.add_option("--no-error-on-unmatched", dest="error_on_unmatched", action="store_false", default=False,
                     help ="Continue generation if there are unmatched fields in options file (default)")
optparser.add_option("-D", "--output-dir", dest="output_dir",
                     metavar="OUTPUTDIR", default=None,
                     help="Output directory of .pb.h and .pb.c files")
optparser.add_option("-Q", "--generated-include-format", dest="genformat",
    metavar="FORMAT", default='#include "%s"',
    help="Set format string to use for including other .pb.h files. [default: %default]")
optparser.add_option("-L", "--library-include-format", dest="libformat",
    metavar="FORMAT", default='#include <%s>',
    help="Set format string to use for including the nanopb pb.h header. [default: %default]")
optparser.add_option("--strip-path", dest="strip_path", action="store_true", default=False,
    help="Strip directory path from #included .pb.h file name")
optparser.add_option("--no-strip-path", dest="strip_path", action="store_false",
    help="Opposite of --strip-path (default since 0.4.0)")
optparser.add_option("--cpp-descriptors", action="store_true",
    help="Generate C++ descriptors to lookup by type (e.g. pb_field_t for a message)")
optparser.add_option("-T", "--no-timestamp", dest="notimestamp", action="store_true", default=True,
    help="Don't add timestamp to .pb.h and .pb.c preambles (default since 0.4.0)")
optparser.add_option("-t", "--timestamp", dest="notimestamp", action="store_false", default=True,
    help="Add timestamp to .pb.h and .pb.c preambles")
optparser.add_option("-q", "--quiet", dest="quiet", action="store_true", default=False,
    help="Don't print anything except errors.")
optparser.add_option("-v", "--verbose", dest="verbose", action="store_true", default=False,
    help="Print more information.")
optparser.add_option("-s", dest="settings", metavar="OPTION:VALUE", action="append", default=[],
    help="Set generator option (max_size, max_count etc.).")
optparser.add_option("--protoc-insertion-points", dest="protoc_insertion_points", action="store_true", default=False,
                     help="Include insertion point comments in output for use by custom protoc plugins")

def parse_file(filename, fdesc, options):
    '''Parse a single file. Returns a ProtoFile instance.'''
    toplevel_options = nanopb_pb2.NanoPBOptions()
    for s in options.settings:
        text_format.Merge(s, toplevel_options)

    if not fdesc:
        data = open(filename, 'rb').read()
        fdesc = descriptor.FileDescriptorSet.FromString(data).file[0]

    # Check if there is a separate .options file
    had_abspath = False
    try:
        optfilename = options.options_file % os.path.splitext(filename)[0]
    except TypeError:
        # No %s specified, use the filename as-is
        optfilename = options.options_file
        had_abspath = True

    paths = ['.'] + options.options_path
    for p in paths:
        if os.path.isfile(os.path.join(p, optfilename)):
            optfilename = os.path.join(p, optfilename)
            if options.verbose:
                sys.stderr.write('Reading options from ' + optfilename + '\n')
            Globals.separate_options = read_options_file(open(optfilename, openmode_unicode))
            break
    else:
        # If we are given a full filename and it does not exist, give an error.
        # However, don't give error when we automatically look for .options file
        # with the same name as .proto.
        if options.verbose or had_abspath:
            sys.stderr.write('Options file not found: ' + optfilename + '\n')
        Globals.separate_options = []

    Globals.matched_namemasks = set()
    Globals.protoc_insertion_points = options.protoc_insertion_points

    # Parse the file
    file_options = get_nanopb_suboptions(fdesc, toplevel_options, Names([filename]))
    f = ProtoFile(fdesc, file_options)
    f.optfilename = optfilename

    return f

def process_file(filename, fdesc, options, other_files = {}):
    '''Process a single file.
    filename: The full path to the .proto or .pb source file, as string.
    fdesc: The loaded FileDescriptorSet, or None to read from the input file.
    options: Command line options as they come from OptionsParser.

    Returns a dict:
        {'headername': Name of header file,
         'headerdata': Data for the .h header file,
         'sourcename': Name of the source code file,
         'sourcedata': Data for the .c source code file
        }
    '''
    f = parse_file(filename, fdesc, options)

    # Check the list of dependencies, and if they are available in other_files,
    # add them to be considered for import resolving. Recursively add any files
    # imported by the dependencies.
    deps = list(f.fdesc.dependency)
    while deps:
        dep = deps.pop(0)
        if dep in other_files:
            f.add_dependency(other_files[dep])
            deps += list(other_files[dep].fdesc.dependency)

    # Decide the file names
    noext = os.path.splitext(filename)[0]
    headername = noext + options.extension + options.header_extension
    sourcename = noext + options.extension + options.source_extension

    if options.strip_path:
        headerbasename = os.path.basename(headername)
    else:
        headerbasename = headername

    # List of .proto files that should not be included in the C header file
    # even if they are mentioned in the source .proto.
    excludes = ['nanopb.proto', 'google/protobuf/descriptor.proto'] + options.exclude + list(f.file_options.exclude)
    includes = [d for d in f.fdesc.dependency if d not in excludes]

    headerdata = ''.join(f.generate_header(includes, headerbasename, options))
    sourcedata = ''.join(f.generate_source(headerbasename, options))

    # Check if there were any lines in .options that did not match a member
    unmatched = [n for n,o in Globals.separate_options if n not in Globals.matched_namemasks]
    if unmatched:
        if options.error_on_unmatched:
            raise Exception("Following patterns in " + f.optfilename + " did not match any fields: "
                            + ', '.join(unmatched));
        elif not options.quiet:
            sys.stderr.write("Following patterns in " + f.optfilename + " did not match any fields: "
                            + ', '.join(unmatched) + "\n")

        if not Globals.verbose_options:
            sys.stderr.write("Use  protoc --nanopb-out=-v:.   to see a list of the field names.\n")

    return {'headername': headername, 'headerdata': headerdata,
            'sourcename': sourcename, 'sourcedata': sourcedata}

def main_cli():
    '''Main function when invoked directly from the command line.'''

    options, filenames = optparser.parse_args()

    if options.version:
        print(nanopb_version)
        sys.exit(0)

    if not filenames:
        optparser.print_help()
        sys.exit(1)

    if options.quiet:
        options.verbose = False

    if options.output_dir and not os.path.exists(options.output_dir):
        optparser.print_help()
        sys.stderr.write("\noutput_dir does not exist: %s\n" % options.output_dir)
        sys.exit(1)

    if options.verbose:
        sys.stderr.write("Nanopb version %s\n" % nanopb_version)
        sys.stderr.write('Google Python protobuf library imported from %s, version %s\n'
                         % (google.protobuf.__file__, google.protobuf.__version__))

    # Load .pb files into memory and compile any .proto files.
    fdescs = {}
    include_path = ['-I%s' % p for p in options.options_path]
    for filename in filenames:
        if filename.endswith(".proto"):
            with TemporaryDirectory() as tmpdir:
                tmpname = os.path.join(tmpdir, os.path.basename(filename) + ".pb")
                status = invoke_protoc(["protoc"] + include_path + ['--include_imports', '--include_source_info', '-o' + tmpname, filename])
                if status != 0: sys.exit(status)
                data = open(tmpname, 'rb').read()
        else:
            data = open(filename, 'rb').read()

        fdesc = descriptor.FileDescriptorSet.FromString(data).file[-1]
        fdescs[fdesc.name] = fdesc

    # Process any include files first, in order to have them
    # available as dependencies
    other_files = {}
    for fdesc in fdescs.values():
        other_files[fdesc.name] = parse_file(fdesc.name, fdesc, options)

    # Then generate the headers / sources
    Globals.verbose_options = options.verbose
    for fdesc in fdescs.values():
        results = process_file(fdesc.name, fdesc, options, other_files)

        base_dir = options.output_dir or ''
        to_write = [
            (os.path.join(base_dir, results['headername']), results['headerdata']),
            (os.path.join(base_dir, results['sourcename']), results['sourcedata']),
        ]

        if not options.quiet:
            paths = " and ".join([x[0] for x in to_write])
            sys.stderr.write("Writing to %s\n" % paths)

        for path, data in to_write:
            dirname = os.path.dirname(path)
            if dirname and not os.path.exists(dirname):
                os.makedirs(dirname)

            with open(path, 'w') as f:
                f.write(data)

def main_plugin():
    '''Main function when invoked as a protoc plugin.'''

    import io, sys
    if sys.platform == "win32":
        import os, msvcrt
        # Set stdin and stdout to binary mode
        msvcrt.setmode(sys.stdin.fileno(), os.O_BINARY)
        msvcrt.setmode(sys.stdout.fileno(), os.O_BINARY)

    data = io.open(sys.stdin.fileno(), "rb").read()

    request = plugin_pb2.CodeGeneratorRequest.FromString(data)

    try:
        # Versions of Python prior to 2.7.3 do not support unicode
        # input to shlex.split(). Try to convert to str if possible.
        params = str(request.parameter)
    except UnicodeEncodeError:
        params = request.parameter

    import shlex
    args = shlex.split(params)

    if len(args) == 1 and ',' in args[0]:
        # For compatibility with other protoc plugins, support options
        # separated by comma.
        lex = shlex.shlex(params)
        lex.whitespace_split = True
        lex.whitespace = ','
        lex.commenters = ''
        args = list(lex)

    optparser.usage = "Usage: protoc --nanopb_out=[options][,more_options]:outdir file.proto"
    optparser.epilog = "Output will be written to file.pb.h and file.pb.c."

    if '-h' in args or '--help' in args:
        # By default optparser prints help to stdout, which doesn't work for
        # protoc plugins.
        optparser.print_help(sys.stderr)
        sys.exit(1)

    options, dummy = optparser.parse_args(args)

    if options.version:
        sys.stderr.write('%s\n' % (nanopb_version))
        sys.exit(0)

    Globals.verbose_options = options.verbose

    if options.verbose:
        sys.stderr.write("Nanopb version %s\n" % nanopb_version)
        sys.stderr.write('Google Python protobuf library imported from %s, version %s\n'
                         % (google.protobuf.__file__, google.protobuf.__version__))

    response = plugin_pb2.CodeGeneratorResponse()

    # Google's protoc does not currently indicate the full path of proto files.
    # Instead always add the main file path to the search dirs, that works for
    # the common case.
    import os.path
    options.options_path.append(os.path.dirname(request.file_to_generate[0]))

    # Process any include files first, in order to have them
    # available as dependencies
    other_files = {}
    for fdesc in request.proto_file:
        other_files[fdesc.name] = parse_file(fdesc.name, fdesc, options)

    for filename in request.file_to_generate:
        for fdesc in request.proto_file:
            if fdesc.name == filename:
                results = process_file(filename, fdesc, options, other_files)

                f = response.file.add()
                f.name = results['headername']
                f.content = results['headerdata']

                f = response.file.add()
                f.name = results['sourcename']
                f.content = results['sourcedata']

    if hasattr(plugin_pb2.CodeGeneratorResponse, "FEATURE_PROTO3_OPTIONAL"):
        response.supported_features = plugin_pb2.CodeGeneratorResponse.FEATURE_PROTO3_OPTIONAL

    io.open(sys.stdout.fileno(), "wb").write(response.SerializeToString())

if __name__ == '__main__':
    # Check if we are running as a plugin under protoc
    if 'protoc-gen-' in sys.argv[0] or '--protoc-plugin' in sys.argv:
        main_plugin()
    else:
        main_cli()
