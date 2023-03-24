import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from termcolor import colored
from flippigator.flippigator import FlipperTextKeyboard, FlipperHEXKeyboard

os.system("color")


@pytest.mark.bench
class TestNfcBench(BaseCase):
    def test_read_mifare_classic_1k_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("NFC", "Bench_nfc_0")
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -10.0, 15000)
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
            nav.press_ok()
        state = nav.get_current_state()
        assert (
            "card_Mifare Classic 1K_bench" in state
        ), "Result of reading reference card is fail"
        nav.press_right()
        menu = nav.get_menu_list()
        menu_ref = [
            "Save",
            "Emulate",
            "Detect R(up)eader",
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
        key = FlipperTextKeyboard(nav)
        key.send("bench_nfc_0\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_read_nfc_a_card(self, nav, gator, reader_nfc):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -90.0, 15000)
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

        nav.go_to("Emulate UID")
        nav.press_ok()
        state = nav.get_current_state()
        assert "Emulating UID" in state, "NFC Emulation fail"
        nav.press_back()

        nav.go_to("Save UID")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bench_nfc_1\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    """
    def test_read_mifare_classic_4k_card(self, nav, gator, reader_nfc):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -170.0, 15000)
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

        reader_nfc.clear()
        reader_nfc.go_to_place()
        reader_nfc.update()
        string = reader_nfc.get()
        print(string)
        assert string == "W58W9D7CD67A000000", "Emulated NFC card reading failed"

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

    def test_read_troika_card(self, nav, gator, reader_nfc):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -250.0, 15000)
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
            time.sleep(0.5)
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

        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state()
        assert "Emulating MIFARE Classic" in state, "NFC Emulation fail"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bench_nfc_2\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_read_ntag215_card(self, nav, gator, reader_nfc):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -330.0, 15000)
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

        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state()
        assert "Emulating NTAG" in state, "NFC Emulation fail"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bench_nfc_3\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_read_mifare_ultralight_card(self, nav, gator, reader_nfc):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-10.0, -410.0, 15000)
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

        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state()
        assert "Emulating MIFARE Ultralight" in state, "NFC Emulation fail"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bench_nfc_4\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_read_mifare_desfire_card(self, nav, gator, reader_nfc):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-120.0, -10.0, 15000)
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

        nav.go_to("Emulate UID")
        nav.press_ok()
        state = nav.get_current_state()
        assert "Emulating UID" in state, "NFC Emulation fail"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bench_nfc_5\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    """
    This card removed because of bug
    def test_read_nfc_v_card(self, nav, gator):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-120.0, -90.0, 15000)
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

    def test_read_all_in_one_card(self, nav, gator, reader_nfc):
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-120.0, -170.0, 15000)
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

        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state()
        assert "Emulating MIFARE Ultralight" in state, "NFC Emulation fail"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bench_nfc_6\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_emulation_mifare_classic_1k_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "Bench_nfc_0") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W805C1332674D04"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()

    def test_emulation_nfc_a_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "Bench_nfc_1") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_nfc.update():
                string = reader_nfc.get()
                assert string == "W58W00", "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()

    def test_emulation_troika_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "Bench_nfc_2") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_nfc.update():
                string = reader_nfc.get()
                assert string == "W58W60950399D1334", "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()

    def test_emulation_ntag215_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "Bench_nfc_3") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58WC007BA39427704"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()

    def test_emulation_mifare_ultralight_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "Bench_nfc_4") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W80485CBABD4004"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()

    def test_emulation_mifare_desfire_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "Bench_nfc_5") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W8055177A073904"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()

    def test_emulation_all_in_one_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "Bench_nfc_6") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W369CE7B10AC134"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()
