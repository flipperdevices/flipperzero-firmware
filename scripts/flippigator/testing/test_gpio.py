import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.smoke
@pytest.mark.gpio
class TestGpio(BaseCase):
    def test_gpio_menu(self, nav):
        nav.gpio.go_into()
        menu_ref = [
            "USB-UART Bridge",
            "GPIO Manual Control",
            "5V on GPIO",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "GPIO menu list is wrong"

    def test_manual_control(self, nav):
        nav.gpio.go_into()
        nav.go_to("GPIO Manual Control")
        nav.press_ok()
        state = nav.get_current_state()
        assert "GPIO Output Mode Test" in state, "GPIO Manual Control failed"
        nav.go_to_main_screen()
