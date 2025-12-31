from async_protopy.flipperzero_protobuf_compiled import flipper_pb2, gpio_pb2
from async_protopy.commands.gpio_commands import (
    GPIOSetInputPullCommand,
    GPIOSetPinModeCommand,
    GPIOGetPinModeCommand,
    GPIOWritePinCommand,
    GPIOReadPinCommand,
)
from async_protopy.clients.base_client import FlipperBaseProtoClient


class FlipperGPIOProtoClient(FlipperBaseProtoClient):
    """The GPIO flipper protobuf client.

    Gpio Pin ID:
        0 = 'PC0'
        1 = 'PC1'
        2 = 'PC3'
        3 = 'PB2'
        4 = 'PB3'
        5 = 'PA4'
        6 = 'PA6'
        7 = 'PA7'

    Gpio Input Pull Mode:
        0 = 'NO'
        1 = 'UP'
        2 = 'DOWN'

    Gpio Pin Mode:
        0 = 'OUTPUT'
        1 = 'INPUT'

    Inherits from FlipperBaseProtoClient.
    """
    async def get_pin_mode(self, pin: str | int, wait_for_response: bool = True) -> str:
        """Requests the get GPIO pin mode command.

        Args:
            pin: str | int
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A GPIO pin mode.
        """
        response = await self.request(
            GPIOGetPinModeCommand(pin=pin), wait_for_response=wait_for_response, to_validate=True
        )
        return (
            gpio_pb2.DESCRIPTOR.enum_types_by_name["GpioPinMode"]
            .values_by_number[response.gpio_get_pin_mode_response.mode]
            .name
        )

    async def set_pin_mode(self, pin: str | int, mode: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the set GPIO pin mode command.

        Args:
            pin: str | int
            mode: str
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            GPIOSetPinModeCommand(pin=pin, mode=mode), wait_for_response=wait_for_response, to_validate=True
        )

    async def write_pin(self, pin: str | int, value: int, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the write GPIO pin command.

        Args:
            pin: str | int
            value: int
                A value to write
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            GPIOWritePinCommand(pin=pin, value=value), wait_for_response=wait_for_response, to_validate=True
        )

    async def read_pin(self, pin: str | int, wait_for_response: bool = True) -> int:
        """Requests the read GPIO pin command.

        Args:
            pin: str | int
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A pin value.
        """
        response = await self.request(
            GPIOReadPinCommand(pin=pin), wait_for_response=wait_for_response, to_validate=True
        )
        return response.read_pin_response.value

    async def set_input_pull(self, pin: str | int, pull_mode: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the set GPIO input pull command.

        Args:
            pin: str | int
            pull_mode: str
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            A request response message.
        """
        return await self.request(
            GPIOSetInputPullCommand(pin=pin, pull_mode=pull_mode), wait_for_response=wait_for_response, to_validate=True
        )
