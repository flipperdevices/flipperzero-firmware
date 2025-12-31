import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.xfail
@pytest.mark.badusb
class TestBadusb(BaseCase):
    async def test_badusb_menu(self, nav):
        await nav.badusb.go_into()
        menu = await nav.get_menu_list()
        menu_ref = [
            "Connection is active",
            "BTIcon",
        ]
        assert menu == menu_ref, "Bad USB menu list is wrong"
        await nav.go_to_main_screen()
