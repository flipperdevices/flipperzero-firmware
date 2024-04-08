import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.subghz
class TestSubGhz(BaseCase):
    @pytest.mark.smoke
    async def test_subghz_menu(self, nav):
        await nav.subghz.go_into()
        menu_ref = [
            "Read",
            "Read RAW",
            "Saved",
            "Add Manually",
            "Frequency Analyzer",
            "Region Information",
            "Radio Settings",
        ]

        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "Some of Sub-GHz menu items are missing"

        await nav.go_to_main_screen()

    @pytest.mark.smoke
    async def test_subghz_read(self, nav):
        await nav.subghz.go_into()
        await nav.go_to("Read")
        await nav.press_ok()
        await nav.update_screen()
        state = await nav.get_current_state()
        assert "Sub-GHz Scanning" in state, "Sub-GHz scanning fail"
        await nav.press_back()
        await nav.go_to_main_screen()
