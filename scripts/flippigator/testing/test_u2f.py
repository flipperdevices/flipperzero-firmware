import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.xfail
@pytest.mark.u2f
class TestU2f(BaseCase):
    async def test_u2f_menu(self, nav):
        await nav.u2f.go_into()
        menu = await nav.get_menu_list()
        menu_ref = [
            "Connection is active",
            "BTIcon",
        ]
        assert menu == menu_ref, "U2F menu list is wrong"
        await nav.go_to_main_screen()
