from __future__ import annotations

from time import sleep
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from flippigator.flippigator import Navigator


class App:
    def __init__(self, navi: Navigator, name: str):
        self._navi = navi
        self._name = name

    async def go_into(self):
        await self._navi.go_to_main_screen()
        sleep(1)
        await self._navi.press_ok()
        await self._navi.go_to(self._name)
        await self._navi.press_ok()
