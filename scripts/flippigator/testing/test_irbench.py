import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from flippigator.flippigator import FlipperHEXKeyboard, FlipperTextKeyboard
from termcolor import colored

os.system("color")


@pytest.mark.bench_ibutton_ir
class TestInfraredBench(BaseCase):
    def test_new_remote_and_universal_remotes(self, nav_reader, nav_key):
        with allure.step("Delete learned remote 'Remote' if possible"):
            nav_reader.delete_file("Infrared", "Remote")
        with allure.step("Learn new remote on reader_flipper"):
            nav_key.infrared.go_into()
            nav_key.go_to("Universal Remotes")
            nav_key.press_ok()
            nav_key.go_to("TVs")
            nav_key.press_ok()
            nav_reader.infrared.go_into()
            nav_reader.go_to("Learn New Remote")
            nav_reader.press_ok()
            nav_key.press_ok()
            assert (
                nav_reader.wait_for_state("SaveRight") == 0
            ), "Infrared reading error while adding new remote"
            nav_reader.press_right()
            nav_reader.press_ok()
            assert (
                nav_reader.wait_for_state("ir_SIRC_15_button") == 0
            ), "Error while saving first button in IR remote"
            if "status_ir_sending" in nav_key.get_current_state():
                nav_key.press_back()
        with allure.step("Adding another button in remote on reader_flipper"):
            nav_reader.press_down()
            assert (
                "ir_add_button" in nav_reader.get_current_state()
            ), "Error while starting adding new button to remote"
            nav_reader.press_ok()
            nav_key.press_back()
            nav_key.go_to("Audio Players")
            nav_key.press_ok()
            time.sleep(2)
            nav_key.press_ok()
            assert (
                nav_reader.wait_for_state("SaveRight") == 0
            ), "Infrared reading error while adding new remote"
            nav_reader.press_right()
            nav_reader.press_ok()
            nav_reader.wait_for_state("ir_add_button")
            nav_reader.press_up()
            assert (
                nav_reader.wait_for_state("ir_NECe_ED12_button") == 0
            ), "Error while saving second button in IR remote"
            if "status_ir_sending" in nav_key.get_current_state():
                nav_key.press_back()
        with allure.step("Adding another button in remote on reader_flipper"):
            nav_reader.press_down()
            assert (
                "ir_add_button" in nav_reader.get_current_state()
            ), "Error while starting adding new button to remote"
            nav_reader.press_ok()
            nav_key.press_back()
            nav_key.go_to("Projectors")
            nav_key.press_ok()
            time.sleep(2)
            nav_key.press_ok()
            assert (
                nav_reader.wait_for_state("SaveRight") == 0
            ), "Infrared reading error while adding new remote"
            nav_reader.press_right()
            nav_reader.press_ok()
            nav_reader.wait_for_state("ir_add_button")
            nav_reader.press_up()
            assert (
                nav_reader.wait_for_state("ir_NEC_8A_button") == 0
            ), "Error while adding third button in IR remote"
            if "status_ir_sending" in nav_key.get_current_state():
                nav_key.press_back()
        with allure.step("Adding another button in remote on reader_flipper"):
            nav_reader.press_down()
            assert (
                "ir_add_button" in nav_reader.get_current_state()
            ), "Error while starting adding new button to remote"
            nav_reader.press_ok()
            nav_key.press_back()
            nav_key.go_to("Air Conditioners")
            nav_key.press_ok()
            time.sleep(5)
            nav_key.press_ok()
            assert (
                nav_reader.wait_for_state("SaveRight") == 0
            ), "Infrared reading error while adding new remote"
            nav_reader.press_right()
            nav_reader.press_ok()
            nav_reader.wait_for_state("ir_add_button")
            nav_reader.press_up()
            assert (
                nav_reader.wait_for_state("ir_RAW_109_button") == 0
            ), "Error while adding last button in IR remote"
            if "status_ir_sending" in nav_key.get_current_state():
                nav_key.press_back()
        with allure.step("Check remote menu"):
            nav_reader.go_to("ir_Edit_button")
            nav_reader.press_ok()
            menu = nav_reader.get_menu_list()
            menu_ref = [
                "Add Button",
                "Rename Button",
                "Delete Button",
                "Rename Remote",
                "Delete Remote",
            ]
            for i in menu:
                if i in menu_ref:
                    menu_ref.remove(i)
            assert len(menu_ref) == 0, "IR remote action menu is wrong"

    def test_debug(self, nav_reader, nav_key):
        with allure.step("Emulating TV remote on key_flipper"):
            nav_reader.open_file("Infrared", "Remote")
        with allure.step("Starting debug on key_flipper"):
            nav_key.infrared.go_into()
            nav_key.go_to("Debug")
            nav_key.press_ok()
        with allure.step("Check transaction"):
            nav_reader.go_to("ir_NEC_8A_button")
            nav_reader.press_ok()
            assert (
                "ir_NEC_8A_bench" in nav_key.get_current_state()
            ), "Error while recieving test signal"
