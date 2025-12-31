"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import descriptor as _descriptor

_sym_db = _symbol_database.Default()
DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(
    b'\n\rdesktop.proto\x12\nPB_Desktop"\x11\n\x0fIsLockedRequest"\x0f\n\rUnlockRequestB%\n#com.flipperdevices.protobuf.desktopb\x06proto3'
)
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'desktop_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:
    DESCRIPTOR._options = None
    DESCRIPTOR._serialized_options = b'\n#com.flipperdevices.protobuf.desktop'
    _ISLOCKEDREQUEST._serialized_start = 29
    _ISLOCKEDREQUEST._serialized_end = 46
    _UNLOCKREQUEST._serialized_start = 48
    _UNLOCKREQUEST._serialized_end = 63
