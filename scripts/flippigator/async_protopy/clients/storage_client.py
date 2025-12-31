from google.protobuf.json_format import MessageToDict
from async_protopy.commands.storage_commands import (
    StorageBackupRestoreRequestCommand,
    StorageBackupCreateRequestCommand,
    StorageReadRequestCommand, StorageTimestampRequestCommand,
    StorageRenameRequestCommand,
    StorageMd5sumRequestCommand,
    StorageDeleteRequestCommand,
    StorageStatRequestCommand,
    StorageMkdirRequestCommand,
    StorageStatRequestCommand,
    StorageInfoRequestCommand,
)
from async_protopy.clients.base_client import FlipperBaseProtoClient
from async_protopy.exceptions.base_exceptions import FlipperValidateException
from async_protopy.flipperzero_protobuf_compiled import flipper_pb2


class FlipperStorageProtoClient(FlipperBaseProtoClient):
    """The Storage flipper protobuf client.

    Inherits from FlipperBaseProtoClient.
    """
    async def backup_create_request(self, archive_path: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the Storage backup create command.

        Args:
            archive_path: str
                A path to the archive_path
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            StorageBackupCreateRequestCommand(archive_path=archive_path),
            wait_for_response=wait_for_response,
            to_validate=True,
        )

    async def backup_restore_request(self, archive_path: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the Storage backup restore command.

        Args:
            archive_path: str
                A path to the archive_path
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            StorageBackupRestoreRequestCommand(archive_path=archive_path),
            wait_for_response=wait_for_response,
            to_validate=True,
        )

    async def read_request(self, path: str, wait_for_response: bool = True) -> bytes:
        """Requests the Storage read command. Reads file from the flipper.

        Args:
            path: str
                A path to filesystem; must be full path; mut not has trailing '/';
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           Bytes of a file.
        """

        stream = await self.stream(StorageReadRequestCommand(path=path))

        if not wait_for_response:
            return

        response = await stream.__anext__()

        if response.command_status != 0:
            raise FlipperValidateException(
                f'Command status is not equal to 0, given command: {response.command_status}'
            )

        result = [response.storage_read_response.file.data]

        while response.has_next:
            response = await stream.__anext__()
            result.append(response.storage_read_response.file.data)

        return b"".join(result)

    async def write_request(self):
        raise NotImplementedError()  # TODO: implement

    async def info_request(self, path: str, wait_for_response: bool = True) -> dict:
        """Requests the Storage info command.

        Args:
            path: str
                A path to filesystem; must be full path; mut not has trailing '/';
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A dict of storage info response.
        """
        response = await self.request(
            StorageInfoRequestCommand(path=path), wait_for_response=wait_for_response, to_validate=True
        )
        return MessageToDict(message=response.storage_info_response)

    async def stat_request(self, path: str, wait_for_response: bool = True) -> dict:
        """Requests the Storage stat command.

        Args:
            path: str
                A path to filesystem; must be full path; mut not has trailing '/';
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A dict of storage stat response file.
        """
        response = await self.request(
            StorageStatRequestCommand(path=path), wait_for_response=wait_for_response, to_validate=True
        )
        return MessageToDict(
            message=response.storage_stat_response.file,
            including_default_value_fields=True,
        )

    async def timestamp_request(self, path: str, wait_for_response: bool = True) -> int:
        """Requests the Storage timestamp command.

        Args:
            path: str
                A path to filesystem; must be full path; mut not has trailing '/';
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           An integer timestamp.
        """
        response = await self.request(
            StorageTimestampRequestCommand(path=path), wait_for_response=wait_for_response, to_validate=True
        )
        return response.storage_timestamp_response.timestamp

    async def md5sum_request(self, path: str, wait_for_response: bool = True) -> str:
        """Requests the Storage md5sum command.

        Args:
            path: str
                A path to filesystem; must be full path; mut not has trailing '/';
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A string value containing the m5dsum of file.
        """
        response = await self.request(
            StorageMd5sumRequestCommand(path=path), wait_for_response=wait_for_response, to_validate=True
        )
        return response.storage_md5sum_response.md5sum

    async def mkdir_request(self, path: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the Storage mkdir command. Creates a new directory.

        Args:
            path: str
                A path to filesystem; must be full path; mut not has trailing '/';
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            StorageMkdirRequestCommand(path=path), wait_for_response=wait_for_response, to_validate=True
        )

    async def delete_request(self, path: str, recursive: bool = False, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the Storage delete command. Deletes a file or a directory.

        Args:
            path: str
                A path to filesystem; must be full path; mut not has trailing '/';
            recursive: bool (default is True)
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            StorageDeleteRequestCommand(path=path, recursive=recursive),
            wait_for_response=wait_for_response,
            to_validate=True,
        )

    async def rename_request(self, old_path: str, new_path: str, wait_for_response: bool = True) -> flipper_pb2.Main:
        """Requests the Storage rename command. Renames a file or a directory.

        Args:
            old_path: str
                A path to filesystem; must be full path; mut not has trailing '/';
            new_path: str
                 A path to filesystem; must be full path; mut not has trailing '/';
            wait_for_response: bool
                A flag used to wait for the response data or return after the command is sent (default is True)

        Returns:
           A request response message.
        """
        return await self.request(
            StorageRenameRequestCommand(old_path=old_path, new_path=new_path),
            wait_for_response=wait_for_response,
            to_validate=True,
        )

    async def list_request(self, old_path: str, new_path: str, wait_for_response: bool = True):
        raise NotImplementedError()  # TODO: implement
