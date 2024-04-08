from async_protopy.commands.gui_commands import (
    StartVirtualDisplayRequestCommand,
    StopVirtualDisplayRequestCommand,
    StartScreenStreamRequestCommand,
    StopScreenStreamRequestCommand,
    SendInputEventRequestCommand,
)
from async_protopy.clients.base_client import FlipperBaseProtoClient
from async_protopy.flipperzero_protobuf_compiled import flipper_pb2


class FlipperGUIProtoClient(FlipperBaseProtoClient):
    """The GUI flipper protobuf client.

    Input Key:
        0 = 'UP'
        1 = 'DOWN'
        2 = 'RIGHT'
        3 = 'LEFT'
        4 = 'OK'

    Input Type:
        1 = 'RELEASE'
        2 = 'SHORT'
        3 = 'LONG'
        4 = 'REPEAT'

    Inherits from FlipperBaseProtoClient.
    """
    async def send_input_event_request(self, key: str, itype: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the GUI send input event command.

        Args:
            key: str
            itype: str
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            SendInputEventRequestCommand(key=key, itype=itype), wait_for_response=wait_for_response, to_validate=True
        )

    async def start_virtual_display_request(self, data: bytes, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the GUI start virtual display command.

        Args:
            data: bytes
                A first frame data.
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            StartVirtualDisplayRequestCommand(data=data), wait_for_response=wait_for_response, to_validate=True
        )

    async def stop_virtual_display_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the GUI stop virtual display command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            StopVirtualDisplayRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )

    async def start_screen_stream_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the GUI start screen stream command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            StartScreenStreamRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )

    async def stop_screen_stream_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the GUI stop screen stream command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            StopScreenStreamRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )

    async def snapshot_screen(self, wait_for_response: bool = True):
        raise NotImplementedError()  # TODO implement it
