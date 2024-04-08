from typing import Self

from async_protopy.clients.application_client import FlipperApplicationProtoClient
from async_protopy.connectors.base_connector import BaseConnector
from async_protopy.clients.property_client import FlipperPropertyProtoClient
from async_protopy.clients.storage_client import FlipperStorageProtoClient
from async_protopy.clients.desktop_client import FlipperDesktopProtoClient
from async_protopy.clients.system_client import FlipperSystemProtoClient
from async_protopy.clients.gpio_client import FlipperGPIOProtoClient
from async_protopy.clients.base_client import FlipperBaseProtoClient
from async_protopy.clients.gui_client import FlipperGUIProtoClient
from async_protopy.executor import FlipperProtoExecutor


class FlipperProtobufClient(FlipperBaseProtoClient):
    """The general async flipper protobuf client.

    General flipper protobuf client includes all needed clients with rpc methods.
    At start opens the connector connection, starts rpc session, reads all start data and start the executor.
    At exit closes the connector connection and stops the executor.

    Attributes:
        connector: BaseConnector
            A connector to the flipper.
        executor: FlipperProtoExecutor
        application: FlipperApplicationProtoClient
        desktop: FlipperDesktopProtoClient
        gpio: FlipperGPIOProtoClient
        gui: FlipperGUIProtoClient
        property: FlipperPropertyProtoClient
        storage: FlipperStorageProtoClient
        system: FlipperSystemProtoClient
    """
    def __init__(self, connector: BaseConnector):
        self.connector = connector

        self.executor = FlipperProtoExecutor(connector=connector)

        self.application = FlipperApplicationProtoClient(executor=self.executor)
        self.desktop = FlipperDesktopProtoClient(executor=self.executor)
        self.gpio = FlipperGPIOProtoClient(executor=self.executor)
        self.gui = FlipperGUIProtoClient(executor=self.executor)
        self.property = FlipperPropertyProtoClient(executor=self.executor)
        self.storage = FlipperStorageProtoClient(executor=self.executor)
        self.system = FlipperSystemProtoClient(executor=self.executor)

        super().__init__(
            executor=self.executor
        )

    async def start(self) -> None:
        await self.connector.open_connection()

        await self.connector.read_until(b">: ")
        await self.connector.write(b"start_rpc_session\r")
        await self.connector.read_until(b"\n")

        await self.executor.start()

    async def stop(self) -> None:
        await self.system.stop_session()
        await self.connector.close_connection()
        await self.executor.stop()

    async def __aenter__(self) -> Self:
        await self.start()
        return self

    async def __aexit__(self, *args) -> None:
        await self.stop()
