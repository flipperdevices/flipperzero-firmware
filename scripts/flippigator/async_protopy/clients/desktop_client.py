from async_protopy.commands.desktop_commands import (
    DesktopIsLockedRequestCommand,
    DesktopUnlockRequestCommand,
)
from async_protopy.clients.base_client import FlipperBaseProtoClient
from async_protopy.flipperzero_protobuf_compiled import flipper_pb2


class FlipperDesktopProtoClient(FlipperBaseProtoClient):
    """The desktop flipper protobuf client.

    Inherits from FlipperBaseProtoClient.
    """
    async def is_locked_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the desktop is locked command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            DesktopIsLockedRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )

    async def unlock_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the desktop unlock command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(DesktopUnlockRequestCommand(), wait_for_response=wait_for_response, to_validate=True)
