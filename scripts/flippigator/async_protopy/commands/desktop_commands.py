from async_protopy.flipperzero_protobuf_compiled import desktop_pb2
from async_protopy.commands.base_command import BaseCommand


class DesktopIsLockedRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='desktop_is_locked_request',
            proto_class=desktop_pb2.IsLockedRequest,
        )


class DesktopUnlockRequestCommand(BaseCommand):
    def __init__(self):
        super().__init__(
            method_name='desktop_unlock_request',
            proto_class=desktop_pb2.UnlockRequest,
        )
