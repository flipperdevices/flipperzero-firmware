import os
import time

import pytest
from flippigator.case import BaseCase
from termcolor import colored

os.system("color")


@pytest.mark.rfid
class TestRfid(BaseCase):
    @pytest.mark.smoke
    def test_rfid_menu(self, nav):
        nav.rfid.go_into()
        menu_ref = [
            "Read",
            "Saved",
            "Add Manually",
            "Extra Actions",
        ]

        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "RFID menu list is wrong"

        nav.go_to_main_screen()

    @pytest.mark.smoke
    def test_read(self, nav):
        nav.rfid.go_into()
        nav.go_to("Read")
        nav.press_ok()
        state = nav.get_current_state()

        assert "ReadingRFID" in state, "RFID Reading failed"

        nav.go_to_main_screen()

    """
    I think that it's time to del all handy test from this repo
    Let's make only smoke test without bench?
    def test_read_ref_card(self, nav):
        nav.rfid.go_into()
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
    """

    @pytest.mark.smoke
    def test_saved(self, nav):
        nav.rfid.go_into()
        nav.go_to("Saved")
        nav.press_ok()
        state = nav.get_current_state()

        assert "FileBrowserLevelUp" in state, "File browser in 'Saved' was not opened"

        nav.go_to_main_screen()

    def test_add_manually(self, nav):
        nav.rfid.go_into()
        nav.go_to("Add Manually")
        nav.press_ok()
        menu_ref = [
            "EM-Micro EM4100",
            "HID H10301",
            "IDTECK Idteck",
            "Motorola Indala26",
            "Kantech IoProxXSF",
            "AWID",
            "FECAVA FDX-A",
            "ISO FDX-B",
            "Generic HIDProx",
            "Generic HIDExt",
            "Farpointe Pyramid",
            "Viking",
            "Jablotron",
            "Paradox",
            "N/A PAC/Stanley",
            "Keri",
            "Gallagher",
            "Honeywell Nexwatch",
        ]

        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "RFID Add manually option list is wrong"

        nav.go_to_main_screen()

    @pytest.mark.smoke
    def test_add_manually_smoke(self, nav):
        nav.rfid.go_into()
        nav.go_to("Add Manually")
        nav.press_ok()
        menu_ref = [
            "EM-Micro EM4100",
            "HID H10301",
            "IDTECK Idteck",
            "Motorola Indala26",
            "Kantech IoProxXSF",
            "AWID",
            "FECAVA FDX-A",
            "ISO FDX-B",
            "Generic HIDProx",
            "Generic HIDExt",
            "Farpointe Pyramid",
            "Viking",
            "Jablotron",
            "Paradox",
            "N/A PAC/Stanley",
            "Keri",
            "Gallagher",
            "Honeywell Nexwatch",
        ]

        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "RFID Add manually option list is wrong"

        nav.go_to_main_screen()

    @pytest.mark.smoke
    def test_extra_options(self, nav):
        nav.rfid.go_into()
        nav.go_to("Extra Actions")
        nav.press_ok()
        menu_ref = [
            "Read ASK (Animal, Ordina...",
            "Read PSK (Indala)",
            "Read RAW RFID data",
        ]

        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "RFID Add manually option list is wrong"

        nav.go_to_main_screen()
