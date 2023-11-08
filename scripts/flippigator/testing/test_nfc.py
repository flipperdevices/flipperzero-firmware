import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from flippigator.flippigator import FlipperHEXKeyboard, FlipperTextKeyboard
from termcolor import colored

os.system("color")


@pytest.mark.nfc
class TestNfc(BaseCase):
    @pytest.mark.smoke
    def test_nfc_menu(self, nav):
        """
        TODO: needs refactor
        """
        nav.nfc.go_into()
        menu_ref = [
            "Read",
            "Detect Reader",
            "Saved",
            "Extra Actions",
            "Add Manually",
            "Debug",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC menu list differs from reference"

    @pytest.mark.smoke
    def test_read(self, nav):
        nav.nfc.go_into()
        nav.go_to("Read")
        nav.press_ok()
        state = nav.get_current_state()
        assert "ReadingNFC" in state, "NFC Reading failed"
        nav.go_to_main_screen()

    '''
    def test_read_ref_card(self, nav):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
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
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bench_nfc_0\n")
        state = nav.get_current_state()
        while not ("browser_Bench_nfc_0" in state):
            state = nav.get_current_state()
        nav.go_to_main_screen()
        nav.open_file("NFC", "Bench_nfc_0")
        nav.get_current_state()
        time.sleep(5)
        assert 0, "DELETE"
    '''

    @pytest.mark.smoke
    def test_detect_reader(self, nav):
        nav.nfc.go_into()
        nav.go_to("Detect Reader")
        nav.press_ok()
        state = nav.get_current_state()
        assert "EmulatingDetectReader" in state, "Reader detection error"
        nav.go_to_main_screen()

    @pytest.mark.smoke
    def test_saved(self, nav):
        nav.nfc.go_into()
        nav.go_to("Saved")
        nav.press_ok()
        state = nav.get_current_state()
        assert "FileBrowserLevelUp" in state, "File browser in 'Saved' was not opened"

    @pytest.mark.smoke
    def test_extra_actions(self, nav):
        nav.nfc.go_into()

        with allure.step("Check Extra Actions"):
            nav.go_to("Extra Actions")
            nav.press_ok()
            menu_ref = [
                "Read Specific Card Type",
                "Mifare Classic Keys",
                "Unlock NTAG/Ultralight",
            ]
            assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC Extra Actions list is wrong"

        with allure.step("Mifare Classic Keys"):
            nav.go_to("Mifare Classic Keys")
            nav.press_ok()
            state = nav.get_current_state()
            assert (
                "Mifare Classic Keys Pict" in state
            ), "Can't find Mifare Classic Keys dict"
            nav.press_back()

        with allure.step("Unlock NTAG/Ultralight"):
            nav.go_to("Unlock NTAG/Ultralight")
            nav.press_ok()
            menu_ref = [
                "Auth As Ameebo",
                "Auth As Xiaomi Air Purifier",
                "Enter Password Manually",
            ]
            assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC Extra Actions list is wrong"
            nav.press_back()

    def test_add_manually(self, nav):
        nav.nfc.go_into()
        nav.go_to("Add Manually")
        nav.press_ok()
        menu_ref = [
            "NFC-A 7-bytes UID",
            "NFC-A 4-bytes UID",
            "Mifare Ultralight",
            "Mifare Ultralight EV1 11",
            "Mifare Ultralight EV1 H11",
            "Mifare Ultralight EV1 21",
            "Mifare Ultralight EV1 H21",
            "NTAG203",
            "NTAG213",
            "NTAG215",
            "NTAG216",
            "NTAG I2C 1k",
            "NTAG I2C 2k",
            "NTAG I2C Plus 1k",
            "NTAG I2C Plus 2k",
            "Mifare Mini",
            "Mifare Classic 1k 4byte UID",
            "Mifare Classic 1k 7byte UID",
            "Mifare Classic 4k 4byte UI...",
            "Mifare Classic 4k 7byte UI...",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC Add manually option list is wrong"

    @pytest.mark.smoke
    def test_add_manually_smoke(self, nav):
        nav.nfc.go_into()
        nav.go_to("Add Manually")
        nav.press_ok()
        menu_ref = [
            "NFC-A 7-bytes UID",
            "NFC-A 4-bytes UID",
            "Mifare Ultralight",
            "Mifare Ultralight EV1 11",
            "Mifare Ultralight EV1 H11",
            "Mifare Ultralight EV1 21",
            "Mifare Ultralight EV1 H21",
            "NTAG203",
            "NTAG213",
            "NTAG215",
            "NTAG216",
            "NTAG I2C 1k",
            "NTAG I2C 2k",
            "NTAG I2C Plus 1k",
            "NTAG I2C Plus 2k",
            "Mifare Mini",
            "Mifare Classic 1k 4byte UID",
            "Mifare Classic 1k 7byte UID",
            "Mifare Classic 4k 4byte UI...",
            "Mifare Classic 4k 7byte UI...",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC Add manually option list is wrong"


    def test_debug(self, nav):
        nav.nfc.go_into()
        nav.go_to("Debug")
        nav.press_ok()

        menu = nav.get_menu_list()
        menu_ref = [
            "Field"
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC Debug options list is wrong"
