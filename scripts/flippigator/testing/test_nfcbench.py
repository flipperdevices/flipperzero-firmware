import logging
import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from flippigator.flippigator import FlipperHEXKeyboard, FlipperTextKeyboard
from termcolor import colored

os.system("color")


@pytest.mark.bench_nfc_rfid
class TestNfcBench(BaseCase):
    def test_read_mifare_classic_1k_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("NFC", "BN0")
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-1.0, -10.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
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
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Classic 1K", nav.font_helvB08, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("UID: 04 4D 67 32 13 5C 80", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Detect Reader",
            "Info",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Classic 1K", nav.font_haxrcorp_4089, 0, no_space = 1))
        assert len(state) > 0, "Emulation failed"

        nav.press_back()

        nav.go_to("Info")
        nav.press_ok()
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bN0\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()


    def test_read_nfc_a_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("NFC", "BN1")
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-1.0, -90.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
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
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Classic 1K", nav.font_helvB08, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("UID: 00 00 00 00", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Detect Reader",
            "Info",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Classic 1K", nav.font_haxrcorp_4089, 0, no_space = 1))
        assert len(state) > 0, "Emulation failed"

        nav.press_back()

        nav.go_to("Info")
        nav.press_ok()
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bN1\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()


    def test_read_mifare_classic_4k_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("NFC", "BN7")
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-1.0, -170.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
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
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Classic 4K", nav.font_helvB08, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("UID: 7A D6 7C 9D", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Classic 4K", nav.font_haxrcorp_4089, 0, no_space = 1))
        assert len(state) > 0, "Emulation failed"

        nav.press_back()

        nav.go_to("Info")
        nav.press_ok()
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bN7\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()


    def test_read_troika_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("NFC", "BN2")
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-1.0, -250.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            time.sleep(0.5)
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = nav.get_current_state()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Troika", nav.font_helvB08, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Num: 41202868", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Balance: 1 RUR", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"

        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Classic 1K", nav.font_haxrcorp_4089, 0, no_space = 1))
        assert len(state) > 0, "Emulation failed"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bN2\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_read_ntag215_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("NFC", "BN3")
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-1.0, -330.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = nav.get_current_state()
        state = nav.get_current_state(ref = nav.get_ref_from_string("NTAG215", nav.font_helvB08, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("UID: 04 77 42 39 BA 07 CO", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Unlock",
            "Info",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"

        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(ref = nav.get_ref_from_string("NTAG215", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Emulation failed"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bN3\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_read_mifare_ultralight_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("NFC", "BN4")
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-1.0, -410.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = nav.get_current_state()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Ultralight", nav.font_helvB08, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("UID: 04 40 BD BA 5C 48 80", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"

        nav.go_to("Emulate")
        nav.press_ok()
        
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Ultralight", nav.font_haxrcorp_4089, 0, no_space = 1))
        assert len(state) > 0, "Emulation failed"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bN4\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()


    def test_read_mifare_desfire_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("NFC", "BN5")
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-110.0, -10.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = nav.get_current_state()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare DESFire", nav.font_helvB08, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("UID: 04 48 6A 32 33 58 80", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate UID",
            "Info",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"

        nav.go_to("Emulate UID")
        nav.press_ok()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating UID", nav.font_helvB08, 0, no_space = 1))
        assert len(state) > 0, "Emulation failed"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bN5\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()


    def test_read_bank_card(self, nav, gator):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("NFC", "BN8_1")
            nav.delete_file("NFC", "BN8_2")
            nav.delete_file("NFC", "BN8_3")
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-110.0, -90.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            state = nav.get_current_state()
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = nav.get_current_state()
            state = nav.get_current_state(ref = nav.get_ref_from_string("Multi-protocol card", nav.font_helvB08, 0))
            assert len(state) > 0, "Reading failed"
            with allure.step("Read as ISO14443-4A"):
                nav.go_to("Read as ISO14443-4A")
                nav.press_ok()
                nav.update_screen()
                state = nav.get_current_state()
                start_time = time.time()
                while "ReadingNFC" in state:
                    nav.logger.debug("Reading")
                    state = nav.get_current_state()
                    if time.time() - start_time > 10:
                        break
                while "ReadingCardNFC" in state:
                    nav.logger.debug("ReadingCardNFC")
                    state = nav.get_current_state()

                state = nav.get_current_state(ref = nav.get_ref_from_string("ISO14443-4A (Unknown)", nav.font_helvB08, 0, no_space = 1))
                assert len(state) > 0, "Result of reading reference card is fail"
                state = nav.get_current_state(ref = nav.get_ref_from_string("UID: 8E C5 C8 AF", nav.font_haxrcorp_4089, 0))
                assert len(state) > 0, "Result of reading reference card is fail"
                nav.press_right()
                menu_ref = [
                    "Save",
                    "Emulate UID",
                    "Info",
                ]
                assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"

                nav.go_to("Emulate UID")
                nav.press_ok()
                state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating UID", nav.font_helvB08, 0, no_space = 1))
                assert len(state) > 0, "Emulation failed"
                nav.press_back()

                nav.go_to("Save")
                nav.press_ok()
                key = FlipperTextKeyboard(nav)
                key.send("bN8_1\n")
                state = nav.get_current_state()
                while "Saved!" in state:
                    state = nav.get_current_state()
            nav.press_back()
            nav.press_ok()
            state = nav.get_current_state()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = nav.get_current_state()
            state = nav.get_current_state(ref = nav.get_ref_from_string("Multi-protocol card", nav.font_helvB08, 0))
            assert len(state) > 0, "Reading failed"
            with allure.step("Read as NTAG/Ultralight"):
                nav.go_to("Read as NTAG/Ultralight")
                nav.press_ok()
                nav.update_screen()
                state = nav.get_current_state()
                start_time = time.time()
                while "ReadingNFC" in state:
                    nav.logger.debug("Reading")
                    state = nav.get_current_state()
                    if time.time() - start_time > 10:
                        break
                while "ReadingCardNFC" in state:
                    nav.logger.debug("ReadingCardNFC")
                    state = nav.get_current_state()

                state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Ultralight", nav.font_helvB08, 0, no_space = 1))
                assert len(state) > 0, "Result of reading reference card is fail"
                state = nav.get_current_state(ref = nav.get_ref_from_string("UID: 8E C5 C8 AF", nav.font_haxrcorp_4089, 0))
                assert len(state) > 0, "Result of reading reference card is fail"
                state = nav.get_current_state(ref = nav.get_ref_from_string("Password-protected pages!", nav.font_haxrcorp_4089, 0))
                assert len(state) > 0, "Result of reading reference card is fail"
                nav.press_right()
                menu_ref = [
                    "Save",
                    "Emulate",
                    "Unlock",
                    "Info",
                ]
                assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"

                nav.go_to("Emulate")
                nav.press_ok()
                state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0, no_space = 1))
                assert len(state) > 0, "Emulation failed"
                state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Ultralight", nav.font_haxrcorp_4089, 0, no_space = 1))
                assert len(state) > 0, "Emulation failed"
                nav.press_back()

                nav.go_to("Save")
                nav.press_ok()
                key = FlipperTextKeyboard(nav)
                key.send("bN8_2\n")
                state = nav.get_current_state()
                while "Saved!" in state:
                    state = nav.get_current_state()
            nav.press_back()
            nav.press_ok()
            state = nav.get_current_state()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = nav.get_current_state()
            state = nav.get_current_state(ref = nav.get_ref_from_string("Multi-protocol card", nav.font_helvB08, 0))
            assert len(state) > 0, "Reading failed"
            with allure.step("Read as Mifare Classic"):
                nav.go_to("Read as Mifare Classic")
                nav.press_ok()
                nav.update_screen()
                state = nav.get_current_state()
                start_time = time.time()
                while "ReadingNFC" in state:
                    nav.logger.debug("Reading")
                    state = nav.get_current_state()
                    if time.time() - start_time > 10:
                        break
                while "ReadingCardNFC" in state:
                    nav.logger.debug("ReadingCardNFC")
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
                state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Classic 1K", nav.font_helvB08, 0))
                assert len(state) > 0, "Result of reading reference card is fail"
                state = nav.get_current_state(ref = nav.get_ref_from_string("UID: 8E C5 C8 AF", nav.font_haxrcorp_4089, 0))
                assert len(state) > 0, "Result of reading reference card is fail"
                nav.press_right()
                menu_ref = [
                    "Save",
                    "Emulate",
                    "Info",
                ]
                assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"
                nav.go_to("Emulate")
                nav.press_ok()
                state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
                assert len(state) > 0, "Emulation failed"
                state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Classic 1K", nav.font_haxrcorp_4089, 0, no_space = 1))
                assert len(state) > 0, "Emulation failed"

                nav.press_back()

                nav.go_to("Info")
                nav.press_ok()
                nav.press_back()

                nav.go_to("Save")
                nav.press_ok()
                key = FlipperTextKeyboard(nav)
                key.send("bN8_3\n")
                state = nav.get_current_state()
                while "Saved!" in state:
                    state = nav.get_current_state()
        nav.go_to_main_screen()


    def test_read_all_in_one_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("NFC", "BN6")
        with allure.step("Go to NFC"):
            nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            gator.swim_to(-110.0, -170.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = nav.get_current_state()
        state = nav.get_current_state(ref = nav.get_ref_from_string("All-In-One", nav.font_helvB08, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Number: 2105793721", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Rides left: 0", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "NFC card menu is wrong"

        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Mifare Ultralight", nav.font_haxrcorp_4089, 0, no_space = 1))
        assert len(state) > 0, "Emulation failed"
        nav.press_back()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bN6\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_emulation_mifare_classic_1k_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "BN0") == -1:
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

        if nav.open_file("NFC", "BN1") == -1:
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

    def test_emulation_mifare_classic_4k_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "BN7") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W9D7CD67A000000"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()

    def test_emulation_troika_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "BN2") == -1:
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

        if nav.open_file("NFC", "BN3") == -1:
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

        if nav.open_file("NFC", "BN4") == -1:
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

        if nav.open_file("NFC", "BN5") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W805833326A4804"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()

    def test_emulation_all_in_one_card(self, nav, gator, reader_nfc):
        nav.go_to_main_screen()
        reader_nfc.go_to_place()
        reader_nfc.clear()

        if nav.open_file("NFC", "BN6") == -1:
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
