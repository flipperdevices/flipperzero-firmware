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

    async def test_main_screen(self, nav):  # can have multiple fixtures and args
        """ "
        Negative test case for main screen
        """
        await nav.go_to_main_screen()
        state = await nav.get_current_state()
        assert "SDcardIcon" in state and "BTIcon" in state, "Can't reach main screen"

    async def test_menu_option_list(self, nav):
        """
        Check that all menu options are present
        """
        # nav = await nav
        await nav.go_to_main_screen()
        await nav.press_ok()
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

        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(ref_list, nav.font_helvB08, invert=0)
            )
            == ref_list
        )
        # assert nav.get_menu_list() == ref_list
