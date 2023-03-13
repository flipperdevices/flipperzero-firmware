import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.infrared
class TestInfrared(BaseCase):
    def test_infrared_menu_negative(self, nav):
        nav.infrared.go_into()
        menu = nav.get_menu_list()
        menu_ref = [
            "Universal Remotes",
            "Learn New Remote",
            "Saved Remotes",
            "Debug",
        ]
        assert menu == menu_ref, "Infrared menu list is wrong"
        nav.go_to_main_screen()

    def test_read(self, nav):
        nav.infrared.go_into()
        nav.go_to("Learn New Remote")
        nav.press_ok()
        state = nav.get_current_state()
        assert "ReadingIR" in state, "IR Reading failed"
        nav.go_to_main_screen()
