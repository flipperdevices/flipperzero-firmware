"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import descriptor as _descriptor

_sym_db = _symbol_database.Default()
DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(
    b'\n\rstorage.proto\x12\nPB_Storage"x\n\x04File\x12\'\n\x04type\x18\x01 \x01(\x0e2\x19.PB_Storage.File.FileType\x12\x0c\n\x04name\x18\x02 \x01(\t\x12\x0c\n\x04size\x18\x03 \x01(\r\x12\x0c\n\x04data\x18\x04 \x01(\x0c"\x1d\n\x08FileType\x12\x08\n\x04FILE\x10\x00\x12\x07\n\x03DIR\x10\x01"\x1b\n\x0bInfoRequest\x12\x0c\n\x04path\x18\x01 \x01(\t"7\n\x0cInfoResponse\x12\x13\n\x0btotal_space\x18\x01 \x01(\x04\x12\x12\n\nfree_space\x18\x02 \x01(\x04" \n\x10TimestampRequest\x12\x0c\n\x04path\x18\x01 \x01(\t"&\n\x11TimestampResponse\x12\x11\n\ttimestamp\x18\x01 \x01(\r"\x1b\n\x0bStatRequest\x12\x0c\n\x04path\x18\x01 \x01(\t".\n\x0cStatResponse\x12\x1e\n\x04file\x18\x01 \x01(\x0b2\x10.PB_Storage.File"\x1b\n\x0bListRequest\x12\x0c\n\x04path\x18\x01 \x01(\t".\n\x0cListResponse\x12\x1e\n\x04file\x18\x01 \x03(\x0b2\x10.PB_Storage.File"\x1b\n\x0bReadRequest\x12\x0c\n\x04path\x18\x01 \x01(\t".\n\x0cReadResponse\x12\x1e\n\x04file\x18\x01 \x01(\x0b2\x10.PB_Storage.File"<\n\x0cWriteRequest\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x1e\n\x04file\x18\x02 \x01(\x0b2\x10.PB_Storage.File"0\n\rDeleteRequest\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x11\n\trecursive\x18\x02 \x01(\x08"\x1c\n\x0cMkdirRequest\x12\x0c\n\x04path\x18\x01 \x01(\t"\x1d\n\rMd5sumRequest\x12\x0c\n\x04path\x18\x01 \x01(\t" \n\x0eMd5sumResponse\x12\x0e\n\x06md5sum\x18\x01 \x01(\t"3\n\rRenameRequest\x12\x10\n\x08old_path\x18\x01 \x01(\t\x12\x10\n\x08new_path\x18\x02 \x01(\t"+\n\x13BackupCreateRequest\x12\x14\n\x0carchive_path\x18\x01 \x01(\t",\n\x14BackupRestoreRequest\x12\x14\n\x0carchive_path\x18\x01 \x01(\tB%\n#com.flipperdevices.protobuf.storageb\x06proto3'
)
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'storage_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:
    DESCRIPTOR._options = None
    DESCRIPTOR._serialized_options = b'\n#com.flipperdevices.protobuf.storage'
    _FILE._serialized_start = 29
    _FILE._serialized_end = 149
    _FILE_FILETYPE._serialized_start = 120
    _FILE_FILETYPE._serialized_end = 149
    _INFOREQUEST._serialized_start = 151
    _INFOREQUEST._serialized_end = 178
    _INFORESPONSE._serialized_start = 180
    _INFORESPONSE._serialized_end = 235
    _TIMESTAMPREQUEST._serialized_start = 237
    _TIMESTAMPREQUEST._serialized_end = 269
    _TIMESTAMPRESPONSE._serialized_start = 271
    _TIMESTAMPRESPONSE._serialized_end = 309
    _STATREQUEST._serialized_start = 311
    _STATREQUEST._serialized_end = 338
    _STATRESPONSE._serialized_start = 340
    _STATRESPONSE._serialized_end = 386
    _LISTREQUEST._serialized_start = 388
    _LISTREQUEST._serialized_end = 415
    _LISTRESPONSE._serialized_start = 417
    _LISTRESPONSE._serialized_end = 463
    _READREQUEST._serialized_start = 465
    _READREQUEST._serialized_end = 492
    _READRESPONSE._serialized_start = 494
    _READRESPONSE._serialized_end = 540
    _WRITEREQUEST._serialized_start = 542
    _WRITEREQUEST._serialized_end = 602
    _DELETEREQUEST._serialized_start = 604
    _DELETEREQUEST._serialized_end = 652
    _MKDIRREQUEST._serialized_start = 654
    _MKDIRREQUEST._serialized_end = 682
    _MD5SUMREQUEST._serialized_start = 684
    _MD5SUMREQUEST._serialized_end = 713
    _MD5SUMRESPONSE._serialized_start = 715
    _MD5SUMRESPONSE._serialized_end = 747
    _RENAMEREQUEST._serialized_start = 749
    _RENAMEREQUEST._serialized_end = 800
    _BACKUPCREATEREQUEST._serialized_start = 802
    _BACKUPCREATEREQUEST._serialized_end = 845
    _BACKUPRESTOREREQUEST._serialized_start = 847
    _BACKUPRESTOREREQUEST._serialized_end = 891
