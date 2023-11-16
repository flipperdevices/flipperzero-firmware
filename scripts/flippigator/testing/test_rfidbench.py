import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from flippigator.flippigator import FlipperHEXKeyboard, FlipperTextKeyboard
from termcolor import colored

os.system("color")


@pytest.mark.bench_nfc_rfid
class TestRfidBench(BaseCase):
    def test_read_indala_card(self, nav, gator, reader_indala):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("RFID", "BR0")
        with allure.step("Go to RFID"):
            nav.rfid.go_into()
        with allure.step("Swim to RFID card"):
            gator.swim_to(-0.5, -865.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingRFID" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
        state = nav.get_current_state(ref = nav.get_ref_from_string("Indala26[Motorola]", nav.font_helvB08, 0, no_space = 1))
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(ref = nav.get_ref_from_string("C8 33 65 B0", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Write",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "RFID card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(ref = nav.get_ref_from_string("Indala26", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Emulation failed"
        nav.press_back()

        gator.swim_to(-0.5, -785.0, 15000)
        nav.go_to("Write")
        nav.press_ok()
        start_time = time.time()
        state = nav.get_current_state()
        while ("status_Writing Indala26" in state) or (
            "warn_Still trying to write" in state
        ):
            state = nav.get_current_state()
            if time.time() - start_time > 25:
                break
        state = nav.get_current_state()
        assert "status_Successfully writen" in state, "RFID Write fail"
        while "status_Successfully writen" in state:
            state = nav.get_current_state()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bR0\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_read_em_card(self, nav, gator, reader_em_hid):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("RFID", "BR1")
        with allure.step("Go to RFID"):
            nav.rfid.go_into()
        with allure.step("Swim to RFID card"):
            gator.swim_to(-100.0, -785.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingRFID" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
        state = nav.get_current_state()
        assert "card_EM4100_bench" in state, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Write",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "RFID card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(ref = nav.get_ref_from_string("EM4100", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Emulation failed"
        nav.press_back()

        gator.swim_to(-1.0, -785.0, 15000)
        nav.go_to("Write")
        nav.press_ok()
        start_time = time.time()
        state = nav.get_current_state()
        while ("status_Writing EM4100" in state) or (
            "warn_Still trying to write" in state
        ):
            state = nav.get_current_state()
            if time.time() - start_time > 25:
                break
        state = nav.get_current_state()
        assert "status_Successfully writen" in state, "RFID Write fail"
        while "status_Successfully writen" in state:
            state = nav.get_current_state()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bR1\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_read_hid_card(self, nav, gator, reader_em_hid):
        with allure.step("Delete previous card, if possible"):
            nav.delete_file("RFID", "BR2")
        with allure.step("Go to RFID"):
            nav.rfid.go_into()
        with allure.step("Swim to RFID card"):
            gator.swim_to(-100.0, -865.0, 15000)
        with allure.step("Read a card"):
            nav.go_to("Read")
            nav.press_ok()
            # key = cv.waitKey(550)
            nav.update_screen()
            state = nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingRFID" in state:
                nav.logger.debug("Reading")
                state = nav.get_current_state()
                if time.time() - start_time > 10:
                    break
        state = nav.get_current_state()
        assert "card_HID_bench" in state, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Write",
        ]
        assert nav.get_menu_list(ref = nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert = 1)) == menu_ref, "RFID card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(ref = nav.get_ref_from_string("Emulating", nav.font_helvB08, 0))
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(ref = nav.get_ref_from_string("H10301", nav.font_haxrcorp_4089, 0))
        assert len(state) > 0, "Emulation failed"
        nav.press_back()

        gator.swim_to(-1.0, -785.0, 15000)
        nav.go_to("Write")
        nav.press_ok()
        start_time = time.time()
        state = nav.get_current_state()
        while ("status_Writing H10301" in state) or (
            "warn_Still trying to write" in state
        ):
            state = nav.get_current_state()
            if time.time() - start_time > 25:
                break
        state = nav.get_current_state()
        assert "status_Successfully writen" in state, "RFID Write fail"
        while "status_Successfully writen" in state:
            state = nav.get_current_state()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bR2\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    def test_emulation_indala_card(self, nav, gator, reader_indala):
        nav.go_to_main_screen()
        reader_indala.go_to_place()
        reader_indala.clear()

        if nav.open_file("RFID", "BR0") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_indala.update():
                string = reader_indala.get()
                assert string == "W32W0316CE9E8", "Emulated RFID card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()

    def test_emulation_em_card(self, nav, gator, reader_em_hid):
        nav.go_to_main_screen()
        reader_em_hid.go_to_place()
        reader_em_hid.clear()

        if nav.open_file("RFID", "BR1") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_em_hid.update():
                string = reader_em_hid.get()
                assert string == "W26W0660F12", "Emulated RFID card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()

    def test_emulation_hid_card(self, nav, gator, reader_em_hid):
        nav.go_to_main_screen()
        reader_em_hid.go_to_place()
        reader_em_hid.clear()

        if nav.open_file("RFID", "BR2") == -1:
            assert 0, "File not found"
        nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if reader_em_hid.update():
                string = reader_em_hid.get()
                assert string == "W26W0F8C88A", "Emulated RFID card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        nav.go_to_main_screen()
