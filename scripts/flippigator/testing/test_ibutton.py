import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.smoke
@pytest.mark.ibutton
class TestIbutton(BaseCase):
    async def test_ibutton_menu(self, nav):
        await nav.ibutton.go_into()
        menu_ref = [
            "Read",
            "Saved",
            "Add Manually",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "iButton menu list is wrong"

    async def test_read(self, nav):
        await nav.ibutton.go_into()
        await nav.go_to("Read")
        await nav.press_ok()
        time.sleep(0.1)
        state = await nav.get_current_state()
        assert "ReadingiButton" in state, "iButton Reading failed"
