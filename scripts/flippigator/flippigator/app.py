from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from flippigator.flippigator import Navigator


class App:
    def __init__(self, navi: Navigator, name: str):
        self._navi = navi
        self._name = name

    def go_into(self):
        self._navi.go_to_main_screen()
        self._navi.press_ok()
        self._navi.go_to(self._name)
        self._navi.press_ok()
