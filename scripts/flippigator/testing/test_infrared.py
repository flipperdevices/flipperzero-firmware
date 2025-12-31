import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.smoke
@pytest.mark.infrared
class TestInfrared(BaseCase):
    async def test_infrared_menu(self, nav):
        await nav.infrared.go_into()
        menu_ref = [
            "Universal Remotes",
            "Learn New Remote",
            "Saved Remotes",
            "Debug",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        )
        await nav.go_to_main_screen()

    async def test_read(self, nav):
        await nav.infrared.go_into()
        await nav.go_to("Learn New Remote")
        await nav.press_ok()
        state = await nav.get_current_state()
        assert "ReadingIR" in state, "IR Reading failed"
        await nav.go_to_main_screen()
