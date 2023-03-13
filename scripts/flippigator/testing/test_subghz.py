import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.subghz
class TestSubGhz(BaseCase):
    def test_subghz_menu_negative(self, nav):
        nav.subghz.go_into()
        menu = nav.get_menu_list()
        menu_ref = [
            "Read",
            "Read RAW",
            "Saved",
            "Add Manually",
            "Frequency Analyzer",
            "Region Information",
            "Test",
        ]
        assert menu == menu_ref, "Sub-GHz menu list is wrong"
        nav.go_to_main_screen()

    def test_subghz_read(self, nav):
        nav.subghz.go_into()
        nav.go_to("Read")
        nav.press_ok()
        nav.update_screen()
        state = nav.get_current_state()
        assert "Sub-GHz Scanning" in state, "Sub-GHz scanning fail"
        nav.press_back()
        nav.go_to_main_screen()
