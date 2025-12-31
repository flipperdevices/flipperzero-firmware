from google._upb._message import MessageMeta
from google.protobuf.internal.encoder import _VarintBytes
from async_protopy.flipperzero_protobuf_compiled import flipper_pb2
from async_protopy.connectors.base_connector import BaseConnector


class BaseCommand:
    """Base command

    Implements base command methods

    Attributes:
        method_name: str
            RPC method name
        proto_class: MessageMeta
            A message class
        command_status: int
            A message command status
        has_next: bool:
            Is there the next message
    """
    def __init__(
        self,
        method_name: str,
        proto_class: MessageMeta,
        command_status: int = flipper_pb2.CommandStatus.Value("OK"),
        has_next: bool = False,
        callback=None,
    ):
        self.method_name = method_name
        self.proto_class = proto_class
        self.has_next = has_next
        self.command_status = command_status

    async def execute(self, command_id: int, connector: BaseConnector) -> None:
        """Executes the command with the given command_id.

        Creates a new message with the given command_id, writes it and flush the write buffer.

        Args:
            command_id: int
            connector: BaseConnector

        Returns:
            None
        """
        message = self.create_message(command_id=command_id)
        await connector.write_and_drain(data=message)

    def create_message(self, command_id: int) -> bytearray:
        """Creates message with the given command_id

        Args:
            command_id: int

        Returns:
            A byte array message
        """
        cmd_data = self.proto_class()

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())
