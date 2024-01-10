import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.subghz
class TestSubGhz(BaseCase):
    @pytest.mark.smoke
    def test_subghz_menu(self, nav):
        nav.subghz.go_into()
        menu_ref = [
            "Read",
            "Read RAW",
            "Saved",
            "Add Manually",
            "Frequency Analyzer",
            "Region Information",
            "Radio Settings",
        ]

        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "Some of Sub-GHz menu items are missing"

        nav.go_to_main_screen()

    @pytest.mark.smoke
    def test_subghz_read(self, nav):
        nav.subghz.go_into()
        nav.go_to("Read")
        nav.press_ok()
        nav.update_screen()
        state = nav.get_current_state()
        assert "Sub-GHz Scanning" in state, "Sub-GHz scanning fail"
        nav.press_back()
        nav.go_to_main_screen()
