from typing import Any, Self

from async_protopy.connectors.base_connector import BaseConnector
from serial_asyncio import open_serial_connection


class SerialConnector(BaseConnector):
    """The async serial connector

    Attributes:
        url: str
        baud_rate: int
        _reader: None | asyncio.StreamReader
        _writer: None | asyncio.StreamWriter
    """
    def __init__(self, url: str, baud_rate: int, **kwargs):
        super().__init__(**kwargs)

        self.url = url
        self.baud_rate = baud_rate

        self._reader = None
        self._writer = None

    async def open_connection(self) -> Self:
        reader, writer = await open_serial_connection(url=self.url, baudrate=self.baud_rate, **self.kwargs)
        self._reader = reader
        self._writer = writer

        return self

    async def close_connection(self) -> None:
        self._writer.close()
        await self._writer.wait_closed()

    async def __aenter__(self) -> Self:
        await self.open_connection()
        return self

    async def __aexit__(self, *args) -> None:
        await self.close_connection()

    async def is_closing(self) -> bool:
        return self._writer.is_closing()

    async def read(self, size: int = -1) -> Any:
        return await self._reader.read(n=size)

    async def readline(self) -> Any:
        return await self._reader.readline()

    async def read_exactly(self, size: int) -> Any:
        return await self._reader.readexactly(n=size)

    async def read_until(self, separator: str) -> Any:
        return await self._reader.readuntil(separator=separator)

    async def write(self, data: Any) -> None:
        self._writer.write(data)

    async def drain(self) -> None:
        await self._writer.drain()

    async def write_and_drain(self, data: Any) -> None:
        await self.write(data=data)
        await self.drain()

    async def clear_read_buffer(self) -> None:
        self._reader._buffer.clear()

    # TODO flush input and output
