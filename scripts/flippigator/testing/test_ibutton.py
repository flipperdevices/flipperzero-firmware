import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.smoke
@pytest.mark.ibutton
class TestIbutton(BaseCase):
    def test_ibutton_menu(self, nav):
        nav.ibutton.go_into()
        menu = nav.get_menu_list()
        menu_ref = [
            "Read",
            "Saved",
            "Add Manually",
        ]
        assert menu == menu_ref, "iButton menu list is wrong"
        nav.go_to_main_screen()

    def test_read(self, nav):
        nav.ibutton.go_into()
        nav.go_to("Read")
        nav.press_ok()
        time.sleep(0.1)
        state = nav.get_current_state()
        assert "ReadingiButton" in state, "iButton Reading failed"
        nav.go_to_main_screen()
