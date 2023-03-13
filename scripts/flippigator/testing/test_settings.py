import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.settings
class TestSettings(BaseCase):
    def test_settings_menu_negative(self, nav):
        nav.settings.go_into()
        menu = nav.get_menu_list()
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
        assert menu == menu_ref, "Settings menu list is wrong"
        nav.go_to_main_screen()
