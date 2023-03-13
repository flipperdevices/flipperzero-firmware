import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.badusb
class TestBadusb(BaseCase):
    def test_badusb_menu_negative(self, nav):
        nav.badusb.go_into()
        menu = nav.get_menu_list()
        menu_ref = [
            "Connection is active",
            "BTIcon",
        ]
        assert menu == menu_ref, "Bad USB menu list is wrong"
        nav.go_to_main_screen()
