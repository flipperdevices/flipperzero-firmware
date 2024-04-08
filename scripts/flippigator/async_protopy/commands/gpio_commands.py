from google.protobuf.internal.encoder import _VarintBytes
from async_protopy.flipperzero_protobuf_compiled import flipper_pb2, gpio_pb2
from async_protopy.exceptions.base_exceptions import InputDataException
from async_protopy.commands.base_command import BaseCommand


class GPIOGetPinModeCommand(BaseCommand):
    def __init__(
        self,
        pin: str | int,
    ):
        super().__init__(
            method_name='gpio_get_pin_mode',
            proto_class=gpio_pb2.GetPinMode,
        )

        self.pin = pin

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        if isinstance(self.pin, int):
            cmd_data.pin = self.pin
        else:
            cmd_data.pin = getattr(gpio_pb2.GpioPin, self.pin, None)

            if cmd_data.pin is None:
                raise InputDataException(f'Invalid pin: {self.pin}')

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class GPIOSetPinModeCommand(BaseCommand):
    def __init__(
        self,
        pin: str | int,
        mode: str,
    ):
        super().__init__(
            method_name='gpio_set_pin_mode',
            proto_class=gpio_pb2.SetPinMode,
        )

        self.pin = pin
        self.mode = mode

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        if isinstance(self.pin, int):
            cmd_data.pin = self.pin
        else:
            cmd_data.pin = getattr(gpio_pb2.GpioPin, self.pin, None)

            if cmd_data.pin is None:
                raise InputDataException(f'Invalid pin: {self.pin}')

        cmd_data.mode = getattr(gpio_pb2.GpioPinMode, self.mode, None)

        if cmd_data.mode is None:
            raise InputDataException(f'Invalid mode: {self.mode}')

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class GPIOWritePinCommand(BaseCommand):
    def __init__(
        self,
        pin: str | int,
        value: int,
    ):
        super().__init__(
            method_name='gpio_write_pin',
            proto_class=gpio_pb2.WritePin,
        )

        self.pin = pin
        self.value = value

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        if isinstance(self.pin, int):
            cmd_data.pin = self.pin
        else:
            cmd_data.pin = getattr(gpio_pb2.GpioPin, self.pin, None)

            if cmd_data.pin is None:
                raise InputDataException(f'Invalid pin: {self.pin}')

        cmd_data.value = self.value

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class GPIOReadPinCommand(BaseCommand):
    def __init__(
        self,
        pin: str | int,
    ):
        super().__init__(
            method_name='gpio_read_pin',
            proto_class=gpio_pb2.ReadPin,
        )

        self.pin = pin

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        if isinstance(self.pin, int):
            cmd_data.pin = self.pin
        else:
            cmd_data.pin = getattr(gpio_pb2.GpioPin, self.pin, None)

            if cmd_data.pin is None:
                raise InputDataException(f'Invalid pin: {self.pin}')

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())


class GPIOSetInputPullCommand(BaseCommand):
    def __init__(
        self,
        pin: str | int,
        pull_mode: str,
    ):
        super().__init__(
            method_name='gpio_set_input_pull',
            proto_class=gpio_pb2.SetInputPull,
        )

        self.pin = pin
        self.pull_mode = pull_mode

    def create_message(self, command_id):
        cmd_data = self.proto_class()

        if isinstance(self.pin, int):
            cmd_data.pin = self.pin
        else:
            cmd_data.pin = getattr(gpio_pb2.GpioPin, self.pin, None)

            if cmd_data.pin is None:
                raise InputDataException(f'Invalid pin: {self.pin}')

        cmd_data.pull_mode = getattr(gpio_pb2.GpioInputPull, self.pull_mode, None)

        if cmd_data.pin is None:
            raise InputDataException(f'Invalid pull_mode: {self.pull_mode}')

        flipper_message = flipper_pb2.Main()
        flipper_message.command_id = command_id
        flipper_message.command_status = self.command_status
        flipper_message.has_next = self.has_next
        getattr(flipper_message, self.method_name).CopyFrom(cmd_data)
        return bytearray(_VarintBytes(flipper_message.ByteSize()) + flipper_message.SerializeToString())
