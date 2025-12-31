"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import descriptor as _descriptor

_sym_db = _symbol_database.Default()
DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(
    b'\n\x0csystem.proto\x12\tPB_System"\x1b\n\x0bPingRequest\x12\x0c\n\x04data\x18\x01 \x01(\x0c"\x1c\n\x0cPingResponse\x12\x0c\n\x04data\x18\x01 \x01(\x0c"m\n\rRebootRequest\x121\n\x04mode\x18\x01 \x01(\x0e2#.PB_System.RebootRequest.RebootMode")\n\nRebootMode\x12\x06\n\x02OS\x10\x00\x12\x07\n\x03DFU\x10\x01\x12\n\n\x06UPDATE\x10\x02"\x13\n\x11DeviceInfoRequest"0\n\x12DeviceInfoResponse\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\t"\x15\n\x13FactoryResetRequest"\x14\n\x12GetDateTimeRequest"<\n\x13GetDateTimeResponse\x12%\n\x08datetime\x18\x01 \x01(\x0b2\x13.PB_System.DateTime";\n\x12SetDateTimeRequest\x12%\n\x08datetime\x18\x01 \x01(\x0b2\x13.PB_System.DateTime"s\n\x08DateTime\x12\x0c\n\x04hour\x18\x01 \x01(\r\x12\x0e\n\x06minute\x18\x02 \x01(\r\x12\x0e\n\x06second\x18\x03 \x01(\r\x12\x0b\n\x03day\x18\x04 \x01(\r\x12\r\n\x05month\x18\x05 \x01(\r\x12\x0c\n\x04year\x18\x06 \x01(\r\x12\x0f\n\x07weekday\x18\x07 \x01(\r"\x1d\n\x1bPlayAudiovisualAlertRequest"\x18\n\x16ProtobufVersionRequest"7\n\x17ProtobufVersionResponse\x12\r\n\x05major\x18\x01 \x01(\r\x12\r\n\x05minor\x18\x02 \x01(\r"(\n\rUpdateRequest\x12\x17\n\x0fupdate_manifest\x18\x01 \x01(\t"\xca\x02\n\x0eUpdateResponse\x128\n\x04code\x18\x01 \x01(\x0e2*.PB_System.UpdateResponse.UpdateResultCode"\xfd\x01\n\x10UpdateResultCode\x12\x06\n\x02OK\x10\x00\x12\x17\n\x13ManifestPathInvalid\x10\x01\x12\x1a\n\x16ManifestFolderNotFound\x10\x02\x12\x13\n\x0fManifestInvalid\x10\x03\x12\x10\n\x0cStageMissing\x10\x04\x12\x17\n\x13StageIntegrityError\x10\x05\x12\x18\n\x14ManifestPointerError\x10\x06\x12\x12\n\x0eTargetMismatch\x10\x07\x12\x1b\n\x17OutdatedManifestVersion\x10\x08\x12\x0b\n\x07IntFull\x10\t\x12\x14\n\x10UnspecifiedError\x10\n"\x12\n\x10PowerInfoRequest"/\n\x11PowerInfoResponse\x12\x0b\n\x03key\x18\x01 \x01(\t\x12\r\n\x05value\x18\x02 \x01(\tB$\n"com.flipperdevices.protobuf.systemb\x06proto3'
)
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'system_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:
    DESCRIPTOR._options = None
    DESCRIPTOR._serialized_options = b'\n"com.flipperdevices.protobuf.system'
    _PINGREQUEST._serialized_start = 27
    _PINGREQUEST._serialized_end = 54
    _PINGRESPONSE._serialized_start = 56
    _PINGRESPONSE._serialized_end = 84
    _REBOOTREQUEST._serialized_start = 86
    _REBOOTREQUEST._serialized_end = 195
    _REBOOTREQUEST_REBOOTMODE._serialized_start = 154
    _REBOOTREQUEST_REBOOTMODE._serialized_end = 195
    _DEVICEINFOREQUEST._serialized_start = 197
    _DEVICEINFOREQUEST._serialized_end = 216
    _DEVICEINFORESPONSE._serialized_start = 218
    _DEVICEINFORESPONSE._serialized_end = 266
    _FACTORYRESETREQUEST._serialized_start = 268
    _FACTORYRESETREQUEST._serialized_end = 289
    _GETDATETIMEREQUEST._serialized_start = 291
    _GETDATETIMEREQUEST._serialized_end = 311
    _GETDATETIMERESPONSE._serialized_start = 313
    _GETDATETIMERESPONSE._serialized_end = 373
    _SETDATETIMEREQUEST._serialized_start = 375
    _SETDATETIMEREQUEST._serialized_end = 434
    _DATETIME._serialized_start = 436
    _DATETIME._serialized_end = 551
    _PLAYAUDIOVISUALALERTREQUEST._serialized_start = 553
    _PLAYAUDIOVISUALALERTREQUEST._serialized_end = 582
    _PROTOBUFVERSIONREQUEST._serialized_start = 584
    _PROTOBUFVERSIONREQUEST._serialized_end = 608
    _PROTOBUFVERSIONRESPONSE._serialized_start = 610
    _PROTOBUFVERSIONRESPONSE._serialized_end = 665
    _UPDATEREQUEST._serialized_start = 667
    _UPDATEREQUEST._serialized_end = 707
    _UPDATERESPONSE._serialized_start = 710
    _UPDATERESPONSE._serialized_end = 1040
    _UPDATERESPONSE_UPDATERESULTCODE._serialized_start = 787
    _UPDATERESPONSE_UPDATERESULTCODE._serialized_end = 1040
    _POWERINFOREQUEST._serialized_start = 1042
    _POWERINFOREQUEST._serialized_end = 1060
    _POWERINFORESPONSE._serialized_start = 1062
    _POWERINFORESPONSE._serialized_end = 1109
