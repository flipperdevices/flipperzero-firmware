import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.smoke
@pytest.mark.infrared
class TestInfrared(BaseCase):
    def test_infrared_menu(self, nav):
        nav.infrared.go_into()
        menu_ref = [
            "Universal Remotes",
            "Learn New Remote",
            "Saved Remotes",
            "Debug",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref
        nav.go_to_main_screen()

    def test_read(self, nav):
        nav.infrared.go_into()
        nav.go_to("Learn New Remote")
        nav.press_ok()
        state = nav.get_current_state()
        assert "ReadingIR" in state, "IR Reading failed"
        nav.go_to_main_screen()
