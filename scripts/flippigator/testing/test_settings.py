import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.settings
class TestSettings(BaseCase):
    @pytest.mark.smoke
    def test_settings_menu(self, nav):
        nav.settings.go_into()
        menu_ref = [
            "Bluetooth",
            "LCD and Notifications",
            "Storage",
            "Power",
            "Desktop",
            "Passport",
            "System",
            "About",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "Settings menu list is wrong"
        nav.go_to_main_screen()
