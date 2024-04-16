from google.protobuf.internal.encoder import _VarintBytes
from async_protopy.flipperzero_protobuf_compiled import property_pb2, flipper_pb2
from async_protopy.commands.base_command import BaseCommand


class PropertyGetRequestCommand(BaseCommand):
    def __init__(
        self,
        key: str,
    ):
        super().__init__(
            method_name='property_get_request',
            proto_class=property_pb2.GetRequest,
        )

        self.key = key

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        cmd_data.key = self.key

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())
