import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.bench
class TestNfcBench(BaseCase):
    def test_read_mifare_classic_1k_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -10.0, 9000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                print(colored("Reading", "yellow"))
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                print(colored("ReadingCardNFC", "yellow"))
                state = nav.get_current_state()
        state = nav.get_current_state()
        while "Mf Classic User Dict" in state:
            state = nav.get_current_state()
        state = nav.get_current_state()
        while "Mf Classic Flipper Dict" in state:
            nav.press_ok()
            state = nav.get_current_state()
        state = nav.get_current_state()
        while "SkipOk" in state:
            state = nav.get_current_state()
        state = nav.get_current_state()
        assert (
            "card_Mifare Classic 1K_bench" in state
        ), "Result of reading reference card is fail"
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save",
            "Emulate",
            "Detect r(down)eader",
            "Info",
        ]
        assert menu == menu_ref, "NFC card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state()
        assert "Emulating MIFARE Classic" in state, "NFC Emulation fail"
        nav.press_back()

        nav.go_to("Info")
        nav.press_ok()
        nav.press_back()

        """
        nav.go_to("Save")
        nav.press_ok()
        nav.press_ok()
        nav.press_ok()
        nav.press_down()
        state = nav.get_current_state()
        assert "FileBrowserLevelUp" in state, "Can't save read NFC card"
        """
        nav.go_to_main_screen()

    def test_read_nfc_a_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -90.0, 9000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                print(colored("Reading", "yellow"))
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                print(colored("ReadingCardNFC", "yellow"))
                state = nav.get_current_state()
        state = nav.get_current_state()
        assert "card_NFC-A_bench" in state, "Result of reading reference card is fail"
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save UID",
            "Emulate UID",
            "Info",
        ]
        assert menu == menu_ref, "NFC card menu is wrong"
        nav.go_to_main_screen()

    """
    Disabled because of bug with Mifare cards 
    def test_read_mifare_classic_4k_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -170.0, 9000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                print(colored("Reading", "yellow"))
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                print(colored("ReadingCardNFC", "yellow"))
                state = nav.get_current_state()
        state = nav.get_current_state()
        while "Mf Classic User Dict" in state:
            state = nav.get_current_state()
        state = nav.get_current_state()
        while "Mf Classic Flipper Dict" in state:
            nav.press_ok()
            state = nav.get_current_state()
        state = nav.get_current_state()
        while "SkipOk" in state:
            state = nav.get_current_state()
        state = nav.get_current_state()
        assert (
            "card_Mifare Classic 4K_bench" in state
        ), "Result of reading reference card is fail"
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save",
            "Emulate",
            "Detect r(down)eader",
            "Info",
        ]
        assert menu == menu_ref, "NFC card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state()
        assert "Emulating MIFARE Classic" in state, "NFC Emulation fail"
        nav.press_back()

        nav.go_to("Info")
        nav.press_ok()
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        nav.press_ok()
        nav.press_ok()
        nav.press_down()
        state = nav.get_current_state()
        assert "FileBrowserLevelUp" in state, "Can't save read NFC card"

        nav.go_to_main_screen()
    """

    def test_read_troika_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -250.0, 9000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                print(colored("Reading", "yellow"))
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                print(colored("ReadingCardNFC", "yellow"))
                state = nav.get_current_state()
        state = nav.get_current_state()
        assert "card_Troika_bench" in state, "Result of reading reference card is fail"
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert menu == menu_ref, "NFC card menu is wrong"
        nav.go_to_main_screen()

    def test_read_ntag215_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -330.0, 9000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                print(colored("Reading", "yellow"))
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                print(colored("ReadingCardNFC", "yellow"))
                state = nav.get_current_state()
        state = nav.get_current_state()
        assert "card_NTAG215_bench" in state, "Result of reading reference card is fail"
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Unlock",
            "Save",
            "Emulate",
            "Info",
        ]
        assert menu == menu_ref, "NFC card menu is wrong"
        nav.go_to_main_screen()

    def test_read_mifare_ultralight_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -410.0, 9000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                print(colored("Reading", "yellow"))
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                print(colored("ReadingCardNFC", "yellow"))
                state = nav.get_current_state()
        state = nav.get_current_state()
        assert (
            "card_Mifare Ultralight_bench" in state
        ), "Result of reading reference card is fail"
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert menu == menu_ref, "NFC card menu is wrong"
        nav.go_to_main_screen()

    def test_read_mifare_desfire_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-120.0, -10.0, 9000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                print(colored("Reading", "yellow"))
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                print(colored("ReadingCardNFC", "yellow"))
                state = nav.get_current_state()
        state = nav.get_current_state()
        assert (
            "card_MIFARE DESfire_bench" in state
        ), "Result of reading reference card is fail"
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save",
            "Emulate UID",
            "Info",
        ]
        assert menu == menu_ref, "NFC card menu is wrong"
        nav.go_to_main_screen()

    """
    This card removed because of bug
    def test_read_nfc_v_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-120.0, -90.0, 9000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                print(colored("Reading", "yellow"))
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                print(colored("ReadingCardNFC", "yellow"))
                state = nav.get_current_state()
        state = nav.get_current_state()
        assert (
            "card_NFC-V_bench" in state
        ), "Result of reading reference card is fail"
        nav.go_to_main_screen()
    """

    def test_read_all_in_one_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-120.0, -170.0, 9000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                print(colored("Reading", "yellow"))
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                print(colored("ReadingCardNFC", "yellow"))
                state = nav.get_current_state()
        state = nav.get_current_state()
        assert (
            "card_All-In-One_bench" in state
        ), "Result of reading reference card is fail"
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert menu == menu_ref, "NFC card menu is wrong"
        nav.go_to_main_screen()
