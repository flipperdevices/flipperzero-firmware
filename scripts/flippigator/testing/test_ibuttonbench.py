import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from termcolor import colored
from flippigator.flippigator import FlipperTextKeyboard, FlipperHEXKeyboard

os.system("color")


@pytest.mark.bench_ibutton_ir
class TestIbuttonBench(BaseCase):
    def test_read_cyfral(self, nav_reader, nav_key, relay):
        relay.reset()
        with allure.step("Choosing Cyfral key"):
            relay.set_key(9)
        with allure.step("Choosing flipper reader"):
            relay.set_reader(3)
        with allure.step("Delete previous key, if possible"):
            nav_key.delete_file("iButton", "BI0")
        with allure.step("Adding key to key_flipper"):
            nav_key.ibutton.go_into()
            nav_key.go_to("Add Manually")
            nav_key.press_ok()
            nav_key.go_to("Cyfral Cyfral")
            nav_key.press_ok()
            key = FlipperHEXKeyboard(nav_key)
            key.send("D109")
            time.sleep(0.1)
            key = FlipperTextKeyboard(nav_key)
            key.send("bI0")
            state = nav_key.get_current_state()
            while "Saved!" in state:
                state = nav_key.get_current_state()
            nav_key.go_to_main_screen()
        with allure.step("Emulating key on key_flipper"):
            if nav_key.open_file("iButton", "BI0") == -1:
                assert 0, "Error while emulating key: file not found"
        with allure.step("Reading original key by reader_flipper"):
            nav_reader.ibutton.go_into()
            nav_reader.go_to("Read")
            nav_reader.press_ok()
            assert nav_reader.wait_for_state("key_Cyfral_bench", timeout = 2) == 0, "Reading reference key failed"
        with allure.step("Reading emulated key by reader_flipper"):
            relay.set_key(8)
            nav_reader.press_left()
            nav_reader.press_left()
            assert nav_reader.wait_for_state("key_Cyfral_bench", timeout = 2) == 0, "Reading emulated key failed"
        with allure.step("Cheking key menu"):
            nav_reader.press_right()
            menu = nav_reader.get_menu_list()
            menu_ref = [
                "Save",
                "Emulate",
            ]
            assert menu == menu_ref, "Key actions menu is wrong"
        relay.reset()

