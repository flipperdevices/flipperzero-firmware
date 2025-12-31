from google.protobuf.internal.encoder import _VarintBytes
from async_protopy.flipperzero_protobuf_compiled import flipper_pb2, gui_pb2
from async_protopy.commands.base_command import BaseCommand


class SendInputEventRequestCommand(BaseCommand):
    def __init__(self, key: str, itype: str):
        super().__init__(
            method_name='gui_send_input_event_request',
            proto_class=gui_pb2.SendInputEventRequest,
        )

        self.key = key
        self.itype = itype

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.key = getattr(gui_pb2, self.key)
        cmd_data.type = getattr(gui_pb2, self.itype)

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StartVirtualDisplayRequestCommand(BaseCommand):
    def __init__(
        self,
        data: bytes,
    ):
        super().__init__(
            method_name='gui_start_virtual_display_request',
            proto_class=gui_pb2.StartVirtualDisplayRequest,
        )

        self.data = data

    def create_message(self, command_id):
        cmd_data = self.proto_class()
        cmd_data.first_frame.data = self.data

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class StopVirtualDisplayRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='gui_stop_virtual_display_request',
            proto_class=gui_pb2.StopVirtualDisplayRequest,
        )


class StartScreenStreamRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='gui_start_screen_stream_request',
            proto_class=gui_pb2.StartScreenStreamRequest,
        )


class StopScreenStreamRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='gui_stop_screen_stream_request',
            proto_class=gui_pb2.StopScreenStreamRequest,
        )
