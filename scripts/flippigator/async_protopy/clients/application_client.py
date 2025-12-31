from async_protopy.commands.application_commands import (
    ApplicationButtonReleaseRequestCommand,
    ApplicationDataExchangeRequestCommand,
    ApplicationButtonPressRequestCommand,
    ApplicationLockStatusRequestCommand,
    ApplicationLoadFileRequestCommand,
    ApplicationGetErrorRequestCommand,
    ApplicationStartRequestCommand,
    ApplicationExitRequestCommand,
)
from async_protopy.clients.base_client import FlipperBaseProtoClient
from async_protopy.flipperzero_protobuf_compiled import flipper_pb2


class FlipperApplicationProtoClient(FlipperBaseProtoClient):
    """The application flipper protobuf client.

    Inherits from FlipperBaseProtoClient.
    """
    async def lock_status_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the application lock status.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            ApplicationLockStatusRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )

    async def start_request(self, name: str, args: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the application start/run command.

        Args:
            name: str
                A name of the application.
            args:
                A path to the application.  # TODO: check it out
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            ApplicationStartRequestCommand(name=name, args=args), wait_for_response=wait_for_response, to_validate=True
        )

    async def exit_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the exit command to the application.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            ApplicationExitRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )

    async def load_file_request(self, path: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the load file command to the application.

        Args:
            path: str  # TODO: get to know
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            ApplicationLoadFileRequestCommand(path=path), wait_for_response=wait_for_response, to_validate=True
        )

    async def button_press_request(self, args: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the button press command to the application.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            ApplicationButtonPressRequestCommand(args=args), wait_for_response=wait_for_response, to_validate=True
        )

    async def button_release_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the button release command to the application.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            ApplicationButtonReleaseRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )

    async def get_error_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the get error command to the application.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            ApplicationGetErrorRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )

    async def data_exchange_request(self, data: bytes, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the data exchange command to the application.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            ApplicationDataExchangeRequestCommand(data=data), wait_for_response=wait_for_response, to_validate=True
        )

    async def data_exchange_receive(self) -> flipper_pb2.Main:
        raise NotImplementedError()  # TODO: implement it
