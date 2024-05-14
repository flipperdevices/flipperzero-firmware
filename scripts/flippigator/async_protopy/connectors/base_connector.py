from typing import Any, Self
from abc import abstractmethod, ABC

from async_protopy.exceptions.base_exceptions import Varint32Exception


class BaseConnector(ABC):
    """Abstract base flipper connector"""

    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs

        self.is_open = False

    @abstractmethod
    async def open_connection(self) -> Self:
        """Opens a connection

        Returns:
            Self
        """

    @abstractmethod
    async def close_connection(self) -> None:
        """Closes a connection

        Returns:
            None
        """

    @abstractmethod
    async def is_closing(self) -> bool:
        """Returns True if the transport is closing or closed

        Returns:
            Is connection is closing or closed
        """

    @abstractmethod
    async def write(self, data: Any) -> None:
        """Writes data to the transport

        Args:
            data: Any

        Returns:
            None
        """

    @abstractmethod
    async def drain(self) -> None:
        """Flushes the write buffer.

        Returns:
            None
        """

    @abstractmethod
    async def write_and_drain(self, data: Any) -> None:
        """Writes data to the transport and flush the write buffer

        Args:
            data: Any

        Returns:
            None
        """

    @abstractmethod
    async def read(self, size: int) -> Any:
        """Reads data from the transport up to 'size' bytes

        Args:
            size: int
            Length of the data to read up

        Returns:
            Any
        """

    @abstractmethod
    async def readline(self) -> Any:
        """Reads line from the transport

        Returns:
            Any
        """
    @abstractmethod
    async def read_exactly(self, size: int) -> Any:
        """Reads exactly 'size' bytes from the transport

        Args:
            size: int
            Length of the data to read

        Returns:
            Any
        """

    @abstractmethod
    async def read_until(self, separator: str) -> Any:
        """Reads until the separator is found

        Args:
            separator: str

        Returns:
            Any
        """
    async def read_varint_32(self) -> int:
        """Reads varint from serial port

        Returns:
            Exactly read data.

        Raises:
            Varint32Exception: An error occurred getting too many bytes when decoding varint.
        """
        MASK = (1 << 32) - 1

        result = 0
        shift = 0
        while 1:
            b = int.from_bytes(await self.read_exactly(size=1), byteorder="little", signed=False)
            result |= (b & 0x7F) << shift

            if not b & 0x80:
                result &= MASK
                result = int(result)
                return result
            shift += 7
            if shift >= 64:
                raise Varint32Exception("Too many bytes when decoding varint.")
