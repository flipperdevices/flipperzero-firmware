from datetime import datetime

from google.protobuf.json_format import MessageToDict
from async_protopy.exceptions.base_exceptions import FlipperValidateException
from async_protopy.commands.system_commands import (
    SystemPlayAudiovisualAlertRequestCommand,
    SystemProtobufVersionRequestCommand,
    SystemFactoryResetRequestCommand,
    SystemSetDatetimeRequestCommand,
    SystemGetDatetimeRequestCommand,
    SystemDeviceInfoRequestCommand,
    SystemPowerInfoRequestCommand,
    SystemUpdateRequestCommand,
    SystemRebootRequestCommand,
    SystemPingRequestCommand,
    StopSessionCommand,
)
from async_protopy.clients.base_client import FlipperBaseProtoClient
from async_protopy.flipperzero_protobuf_compiled import flipper_pb2


class FlipperSystemProtoClient(FlipperBaseProtoClient):
    """The System flipper protobuf client.

    Inherits from FlipperBaseProtoClient.
    """
    async def factory_reset_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the System factory reset command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            SystemFactoryResetRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )

    async def update_request(self, update_manifest: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the System update command.

        Args:
            update_manifest: str
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            SystemUpdateRequestCommand(update_manifest=update_manifest),
            wait_for_response=wait_for_response,
            to_validate=True,
        )

    async def reboot_request(self, mode: str | int, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the System reboot command. No response validation.

        Args:
            mode: str | int
                0 = 'OS'
                1 = 'DFU'
                2 = 'UPDATE'
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            SystemRebootRequestCommand(mode=mode),
            wait_for_response=wait_for_response,
            to_validate=False,  # don't validate
        )

    async def power_info_request(self, wait_for_response: bool = True) -> list[tuple[str, str]]:
        """Requests the System power info command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            List of tuples of Flipper power info or charging status.
            list[tuple[key, value : str]]
        """
        stream = await self.stream(SystemPowerInfoRequestCommand())

        if not wait_for_response:
            return

        response = await stream.__anext__()

        if response.command_status != 0:
            raise FlipperValidateException(
                f'Command status is not equal to 0, given command: {response.command_status}'
            )

        result = [(response.system_power_info_response.key, response.system_power_info_response.value)]

        while response.has_next:
            response = await stream.__anext__()
            result.append((response.system_power_info_response.key, response.system_power_info_response.value))

        return result

    async def device_info_request(self, wait_for_response: bool = True) -> list[tuple[str, str]]:
        """Requests the System device info command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            List of tuples of Flipper device info.
            list[tuple[key, value : str]]
        """
        stream = await self.stream(SystemDeviceInfoRequestCommand())

        if not wait_for_response:
            return

        response = await stream.__anext__()

        if response.command_status != 0:
            raise FlipperValidateException(
                f'Command status is not equal to 0, given command: {response.command_status}'
            )

        result = [(response.system_device_info_response.key, response.system_device_info_response.value)]

        while response.has_next:
            response = await stream.__anext__()
            result.append((response.system_device_info_response.key, response.system_device_info_response.value))

        return result

    async def protobuf_version_request(self, wait_for_response: bool = True) -> tuple[int, int]:
        """Requests the System protobuf version command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            Tuple of Flipper protobuf version.
            tuple[major, minor: int]
        """
        response = await self.request(
            SystemProtobufVersionRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )
        return (
            response.system_protobuf_version_response.major,
            response.system_protobuf_version_response.minor,
        )

    async def get_datetime_request(self, wait_for_response: bool = True) -> dict:
        """Requests the System get datetime command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
            Dict of Flipper system date and time.
            keys: 'year', 'month', 'day', 'hour', 'minute', 'second', 'weekday'
        """
        response = await self.request(
            SystemGetDatetimeRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )
        return MessageToDict(response.system_get_datetime_response)["datetime"]

    async def set_datetime_request(self, date: dict | datetime = None, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the System set datetime command.

        Args:
            date: dict | datetime
                A date to be set (default is None).
                If dictionary, use keys: 'year', 'month', 'day', 'hour', 'minute', 'second', 'weekday'
                If None, sets datetime.now().
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        if date is None:
            date = datetime.now()
        return await self.request(
            SystemSetDatetimeRequestCommand(date=date), wait_for_response=wait_for_response, to_validate=True
        )

    async def ping_request(self, data: bytes = bytes([0xDE, 0xAD, 0xBE, 0xEF]), wait_for_response: bool = True) -> list:
        """Requests the System ping command.

        Args:
            data: bytes (default is bytes([0xDE, 0xAD, 0xBE, 0xEF]))
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           List of system ping response message data.
        """
        response = await self.request(
            SystemPingRequestCommand(data=data), wait_for_response=wait_for_response, to_validate=True
        )
        return response.system_ping_response.data

    async def play_audiovisual_alert_request(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the System play audiovisual alert command.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            SystemPlayAudiovisualAlertRequestCommand(), wait_for_response=wait_for_response, to_validate=True
        )

    async def stop_session(self, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the stop session command. Stops RPC session.

        Args:
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(StopSessionCommand(), wait_for_response=wait_for_response, to_validate=True)
