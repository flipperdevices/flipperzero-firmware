import os
import time

import allure
import pytest
from termcolor import colored

from flippigator.case import BaseCase

os.system("color")


@pytest.mark.bench
class TestNfcRfidBench(BaseCase):
    def test_read_nfc_ref_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-700.0, -890.0, 5000)
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
        """
        if self.navi.getCurrentState()[0] == 'AllInOneNFCRef':
            print(colored("NFC Reading passed", 'green'))
        else:
            print(colored("NFC Reading failed", 'red'))
        """
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
            "MfClassic1KTestCardBench" in state
        ), "Result of reading reference card is fail"
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert menu == menu_ref, "NFC card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state()
        assert "Emulating" in state, "NFC Emulation fail"
        nav.press_back()

        nav.go_to("Info")
        nav.press_ok()
        state = nav.get_current_state()
        assert "MfClassic1KTestCardInfoBench" in state, "Card full info wrong"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        nav.press_ok()
        nav.press_ok()
        nav.press_down()
        state = nav.get_current_state()
        assert "FileBrowserLevelUp" in state, "Can't save read NFC card"
        nav.go_to_main_screen()

    def test_read_rfid_ref_card(self, nav, gator):
        nav.rfid.go_into()
        gator.swim_to(-300.0, -890.0, 5000)
        nav.go_to("Read")
        nav.press_ok()
        state = nav.get_current_state()
        start_time = time.time()
        while "ReadingRFID" in state:
            print(colored("Reading", "yellow"))
            state = nav.get_current_state()
            if time.time() - start_time > 10:
                break
        state = nav.get_current_state()
        assert "EM4100Ref" in state, "Result of reading reference card is fail"

        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save",
            "Emulate",
            "Write",
        ]
        assert menu == menu_ref, "RFID card menu is wrong"

        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state()
        assert "Emulating EM4100" in state, "RFID Emulation fail"

        nav.press_back()
        nav.go_to("Write")
        nav.press_ok()
        state = nav.get_current_state()
        start_time = time.time()
        while "Writing EM4100" in state:
            print(colored("Writing", "yellow"))
            state = nav.get_current_state()
            if time.time() - start_time > 10:
                break
        state = nav.get_current_state()
        assert "Successfully writen" in state, "RFID writing fail"
        while "Successfully writen" in state:
            print(colored("Waiting for return", "yellow"))
            state = nav.get_current_state()
            if time.time() - start_time > 10:
                break

        nav.go_to("Save")
        nav.press_ok()
        nav.press_ok()
        while "Save" in state:
            print(colored("Saving", "yellow"))
            state = nav.get_current_state()
            if time.time() - start_time > 10:
                break
        state = nav.get_current_state()
        assert "Saved!" in state, "Can't save read RFID card"
        nav.go_to_main_screen()
