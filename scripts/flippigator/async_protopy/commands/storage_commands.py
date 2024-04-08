from google.protobuf.internal.encoder import _VarintBytes
from async_protopy.flipperzero_protobuf_compiled import storage_pb2, flipper_pb2
from async_protopy.commands.base_command import BaseCommand


class StorageBackupCreateRequestCommand(BaseCommand):
    def __init__(self, archive_path: str):
        super().__init__(
            method_name='storage_backup_create_request',
            proto_class=storage_pb2.BackupCreateRequest,
        )
        self.archive_path = archive_path

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.archive_path = self.archive_path

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StorageBackupRestoreRequestCommand(BaseCommand):
    def __init__(self, archive_path: str):
        super().__init__(
            method_name='storage_backup_restore_request',
            proto_class=storage_pb2.BackupRestoreRequest,
        )
        self.archive_path = archive_path

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.archive_path = self.archive_path

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StorageReadRequestCommand(BaseCommand):
    def __init__(self, path: str):
        super().__init__(
            method_name='storage_read_request',
            proto_class=storage_pb2.ReadRequest,
        )
        self.path = path

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.path = self.path

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StorageInfoRequestCommand(BaseCommand):
    def __init__(self, path: str):
        super().__init__(
            method_name='storage_info_request',
            proto_class=storage_pb2.InfoRequest,
        )
        self.path = path

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.path = self.path

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StorageStatRequestCommand(BaseCommand):
    def __init__(self, path: str):
        super().__init__(
            method_name='storage_stat_request',
            proto_class=storage_pb2.StatRequest,
        )
        self.path = path

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.path = self.path

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StorageTimestampRequestCommand(BaseCommand):
    def __init__(self, path: str):
        super().__init__(
            method_name='storage_timestamp_request',
            proto_class=storage_pb2.TimestampRequest,
        )
        self.path = path

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.path = self.path

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StorageStatRequestCommand(BaseCommand):
    def __init__(self, path: str):
        super().__init__(
            method_name='storage_stat_request',
            proto_class=storage_pb2.StatRequest,
        )
        self.path = path

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.path = self.path

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StorageMd5sumRequestCommand(BaseCommand):
    def __init__(self, path: str):
        super().__init__(
            method_name='storage_md5sum_request',
            proto_class=storage_pb2.Md5sumRequest,
        )
        self.path = path

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.path = self.path

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StorageMkdirRequestCommand(BaseCommand):
    def __init__(self, path: str):
        super().__init__(
            method_name='storage_mkdir_request',
            proto_class=storage_pb2.MkdirRequest,
        )
        self.path = path

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.path = self.path

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StorageDeleteRequestCommand(BaseCommand):
    def __init__(self, path: str, recursive: bool):
        super().__init__(
            method_name='storage_delete_request',
            proto_class=storage_pb2.DeleteRequest,
        )
        self.path = path
        self.recursive = recursive

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        cmd_data.path = self.path
        cmd_data.recursive = self.recursive

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StorageRenameRequestCommand(BaseCommand):
    def __init__(self, old_path: str, new_path: str):
        super().__init__(
            method_name='storage_rename_request',
            proto_class=storage_pb2.RenameRequest,
        )
        self.old_path = old_path
        self.new_path = new_path

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        cmd_data.old_path = self.old_path
        cmd_data.new_path = self.new_path

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())
