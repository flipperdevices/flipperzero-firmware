from google.protobuf.internal.encoder import _VarintBytes
from async_protopy.flipperzero_protobuf_compiled import application_pb2, flipper_pb2
from async_protopy.commands.base_command import BaseCommand


class ApplicationLockStatusRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='app_lock_status_request',
            proto_class=application_pb2.LockStatusRequest,
        )


class ApplicationStartRequestCommand(BaseCommand):
    def __init__(self, name: str, args: str):
        super().__init__(
            method_name='app_start_request',
            proto_class=application_pb2.StartRequest,
        )

        self.name = name
        self.args = args

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.name = self.name
        cmd_data.args = self.args

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class ApplicationExitRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='app_exit_request',
            proto_class=application_pb2.AppExitRequest,
        )


class ApplicationLoadFileRequestCommand(BaseCommand):
    def __init__(
        self,
        path: str,
    ):
        super().__init__(
            method_name='app_load_file_request',
            proto_class=application_pb2.AppLoadFileRequest,
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


class ApplicationButtonPressRequestCommand(BaseCommand):
    def __init__(
        self,
        args: str,
    ):
        super().__init__(
            method_name='app_button_press_request',
            proto_class=application_pb2.AppButtonPressRequest,
        )

        self.args = args

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.args = self.args

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class ApplicationButtonReleaseRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='app_button_release_request',
            proto_class=application_pb2.AppButtonReleaseRequest,
        )


class ApplicationGetErrorRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='app_get_error_request',
            proto_class=application_pb2.GetErrorRequest,
        )


class ApplicationDataExchangeRequestCommand(BaseCommand):
    def __init__(
        self,
        data: bytes,
    ):
        super().__init__(
            method_name='app_data_exchange_request',
            proto_class=application_pb2.DataExchangeRequest,
        )

        self.data = data

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.data = self.data

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())
