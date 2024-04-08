import asyncio
import logging
from typing import Any, AsyncGenerator, Self

from async_protopy.flipperzero_protobuf_compiled import flipper_pb2
from async_protopy.connectors.base_connector import BaseConnector
from async_protopy.commands.base_command import BaseCommand


class FlipperProtoExecutor:
    """The general async commands executor.

    General command executor based on the command event loop (infinity loop) and event objects

    Attributes:
        _connector: BaseConnector
            A connector to the flipper.
        events: dict
            A dictionary of executor events.
            The key is a command id of an executed command.
            The value is list of two objects as executed command data and event object.
            {command_id: int: [result_data: Any, event_object: asyncio.Event]}
        command_id: int
            A command id of the executed command, increasing at a command execution.
        runner_task: None | asyncio.Task
            An asyncio task object of the executor running command event loop.
            Uses to manage task and tack it status.
    """
    def __init__(self, connector: BaseConnector):
        self._connector = connector
        # self.is_loop_running = False
        # self.command_queue = deque()
        self.events: {int: list[Any, asyncio.Event]} = {}
        self.command_id = 0

        self.runner_task = None

    async def execute_command(self, command: BaseCommand, command_id: int = None) -> AsyncGenerator:  # TODO command_id for writing without changing command_id
        """Executes a given command.

        Also adds a new event to the instance's events dictionary with no result data.
        Auto increases command id by 1.

        Args:
            command: BaseCommand
                A command to execute.

        Returns:
            An async generator of event_waiter.
        """
        if command_id is None:
            self.command_id += 1
        else:
            self.command_id = command_id

        event = asyncio.Event()
        self.events[self.command_id] = [None, event]
        waiter = self.event_waiter(self.command_id, event)

        await command.execute(command_id=self.command_id, connector=self._connector)
        return waiter

    async def event_waiter(self, event_name: int, event: asyncio.Event) -> Any:
        """Starts the event waiter async generator

        The infinity looped async generator based on the 3 steps:
        1. Wait for an event object to be set
        2. Clear an event object to be reset again later
        3. Yield event result data from the instance's events dictionary

        Args:
            event_name: int
                An key in the instance's events dictionary
            event: asyncio.Event

        Yields:
            An event result data
        """
        while True:
            await event.wait()
            event.clear()
            yield self.events[event_name][0]

    async def _run_command_event_loop(self) -> None:  # set cor as done
        """Runs command event loop

        The main event loop reads and parses data from the instance's connector,
        looks for the event with the returned command_id and set the result data if the event exists.

        Returns:
            None
        """
        while True:
            length = await self._connector.read_varint_32()
            data = flipper_pb2.Main()
            read_exactly = await self._connector.read_exactly(size=length)
            data.ParseFromString(read_exactly)

            if event := self.events.get(data.command_id):
                event[0] = data
                event[1].set()

    async def start(self) -> Self:
        """Starts the flipper executor

        Creates and starts a background task with the command event loop.

        Returns:
            Self
        """
        self.runner_task = asyncio.create_task(self._run_command_event_loop())
        await asyncio.sleep(0)

        return self

    async def stop(self) -> None:
        """Stops the flipper executor

        Cancels the command event loop if it is running.

        Returns:
            None
        """
        if not self.runner_task.cancelled():
            self.runner_task.cancel()

    async def __aenter__(self) -> Self:
        return await self.start()

    async def __aexit__(self, *args) -> None:
        return await self.stop()
