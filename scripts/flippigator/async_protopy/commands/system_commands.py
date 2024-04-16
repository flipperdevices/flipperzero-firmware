from datetime import datetime

from google.protobuf.internal.encoder import _VarintBytes
from async_protopy.flipperzero_protobuf_compiled import flipper_pb2, system_pb2
from async_protopy.exceptions.base_exceptions import InputTypeException, InputDataException
from async_protopy.commands.base_command import BaseCommand


class SystemFactoryResetRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='system_factory_reset_request',
            proto_class=system_pb2.FactoryResetRequest,
        )


class SystemUpdateRequestCommand(BaseCommand):
    def __init__(
        self,
        update_manifest: str,
    ):
        super().__init__(
            method_name='system_update_request',
            proto_class=system_pb2.UpdateRequest,
        )

        self.update_manifest = update_manifest

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.update_manifest = self.update_manifest

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class SystemRebootRequestCommand(BaseCommand):
    def __init__(
        self,
        mode: str | int,
    ):
        super().__init__(
            method_name='system_reboot_request',
            proto_class=system_pb2.RebootRequest,
        )

        self.mode = mode

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        if isinstance(self.mode, int):
            cmd_data.mode = self.mode
        else:
            cmd_data.mode = getattr(system_pb2.RebootRequest().RebootMode, self.mode, None)

            if cmd_data.mode is None:
                raise InputDataException(f'Invalid Reboot mode: {self.mode}')

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class SystemPowerInfoRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='system_power_info_request',
            proto_class=system_pb2.PowerInfoRequest,
        )


class SystemDeviceInfoRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='system_device_info_request',
            proto_class=system_pb2.DeviceInfoRequest,
        )


class SystemProtobufVersionRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='system_protobuf_version_request',
            proto_class=system_pb2.ProtobufVersionRequest,
        )


class SystemGetDatetimeRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='system_get_datetime_request',
            proto_class=system_pb2.GetDateTimeRequest,
        )


class SystemSetDatetimeRequestCommand(BaseCommand):
    def __init__(
        self,
        date: dict | datetime,
    ):
        super().__init__(
            method_name='system_set_datetime_request',
            proto_class=system_pb2.SetDateTimeRequest,
        )

        self.date = date

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        if isinstance(self.date, datetime):
            cmd_data.datetime.year = self.date.year
            cmd_data.datetime.month = self.date.month
            cmd_data.datetime.day = self.date.day
            cmd_data.datetime.hour = self.date.hour
            cmd_data.datetime.minute = self.date.minute
            cmd_data.datetime.second = self.date.second
            cmd_data.datetime.weekday = self.date.isoweekday()
        elif isinstance(self.date, dict):
            cmd_data.datetime.update(self.date)
        else:
            raise InputTypeException(f'Invalid datetime type: {type(self.date)}')

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class SystemPingRequestCommand(BaseCommand):
    def __init__(
        self,
        data: bytes,
    ):
        super().__init__(
            method_name='system_ping_request',
            proto_class=system_pb2.PingRequest,
        )

        self.data = data

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        if not isinstance(self.data, bytes):
            raise InputTypeException(f'Invalid Ping data type: {type(self.data)}')

        cmd_data.data = self.data

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class SystemPlayAudiovisualAlertRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='system_play_audiovisual_alert_request',
            proto_class=system_pb2.PlayAudiovisualAlertRequest,
        )


class StopSessionCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='stop_session',
            proto_class=flipper_pb2.StopSession,
        )
