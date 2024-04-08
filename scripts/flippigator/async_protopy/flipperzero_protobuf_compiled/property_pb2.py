"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import descriptor as _descriptor

_sym_db = _symbol_database.Default()
DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(
    b'\n\x0eproperty.proto\x12\x0bPB_Property"\x19\n\nGetRequest\x12\x0b\n\x03key\x18\x01 \x01(\t")\n\x0bGetResponse\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\tB&\n$com.flipperdevices.protobuf.propertyb\x06proto3'
)
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'property_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:
    DESCRIPTOR._options = None
    DESCRIPTOR._serialized_options = b'\n$com.flipperdevices.protobuf.property'
    _GETREQUEST._serialized_start = 31
    _GETREQUEST._serialized_end = 56
    _GETRESPONSE._serialized_start = 58
    _GETRESPONSE._serialized_end = 99
