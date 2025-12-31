from async_protopy.exceptions.base_exceptions import FlipperValidateException
from async_protopy.commands.property_commands import PropertyGetRequestCommand
from async_protopy.clients.base_client import FlipperBaseProtoClient


class FlipperPropertyProtoClient(FlipperBaseProtoClient):
    """The Property flipper protobuf client.

    Inherits from FlipperBaseProtoClient.
    """
    async def get_request(self, key: str, wait_for_response: bool = True) -> list[tuple[str, str]]:
        """Requests the Property get command.

        Args:
            key: str
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           List of tuples of Flipper properties.
           list[tuple[key, value : str]]
        """
        stream = await self.stream(PropertyGetRequestCommand(key=key))

        if not wait_for_response:
            return

        response = await stream.__anext__()

        if response.command_status != 0:
            raise FlipperValidateException(
                f'Command status is not equal to 0, given command: {response.command_status}'
            )

        result = [(response.property_get_response.key, response.property_get_response.value)]

        while response.has_next:
            response = await stream.__anext__()
            result.append((response.property_get_response.key, response.property_get_response.value))

        return result
