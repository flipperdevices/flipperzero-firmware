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
        menu_ref = [
            "Read",
            "Saved",
            "Add Manually",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "iButton menu list is wrong"

    def test_read(self, nav):
        nav.ibutton.go_into()
        nav.go_to("Read")
        nav.press_ok()
        time.sleep(0.1)
        state = nav.get_current_state()
        assert "ReadingiButton" in state, "iButton Reading failed"
