import os

import pytest
from flippigator.case import BaseCase

os.system("color")


@pytest.mark.smoke
@pytest.mark.menu
class TestMain(BaseCase):
    """
    Here lies the test cases for the menu
    """

    def test_main_screen(self, nav):  # can have multiple fixtures and args
        """ "
        Negative test case for main screen
        """
        nav.go_to_main_screen()
        state = nav.get_current_state()
        assert "SDcardIcon" in state and "BTIcon" in state, "Can't reach main screen"

    def test_menu_option_list(self, nav):
        """
        Check that all menu options are present
        """
        nav.go_to_main_screen()
        nav.press_ok()
        ref_list = [
            "Sub-GHz",
            "125 kHz RFID",
            "NFC",
            "Infrared",
            "GPIO",
            "iButton",
            "Bad USB",
            "U2F",
            "Settings",
            "Apps",
        ]

        assert nav.get_menu_list(ref = nav.get_ref_from_list(ref_list, nav.font_helvB08, invert = 0)) == ref_list
