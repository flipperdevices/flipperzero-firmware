import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from flippigator.flippigator import FlipperHEXKeyboard, FlipperTextKeyboard
from termcolor import colored
import difflib as df

os.system("color")


@pytest.mark.bench_nfc_rfid
class TestRfidBench(BaseCase):
    def compare_rfid_files(self, nav, name):
        with open("./ref_files/"+name) as ref_file:
            ref = ref_file.read().splitlines()
        try:
            downloaded = nav.proto.rpc_read("/ext/lfrfid/"+name).decode("UTF-8").splitlines()
        except FlipperProtoException:
            assert 0, "No dump file for "+name
        d = df.Differ()
        diff = d.compare(ref, downloaded)
        compared = list()
        for i in diff:
            compared.append(i[2:])
        assert compared == ref, "Dump" + name + "is wrong!"

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
                if time.time() - start_time > 20:
                    break
        state = nav.get_current_state(
            ref=nav.get_ref_from_string(
                "Motorola Indala26", nav.font_helvB08, 0, no_space=1
            )
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("Hex: C8 33 65 B0", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("FC: 184", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("Card: 8574", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Write",
        ]
        assert (
            nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "RFID card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("Indala26", nav.font_haxrcorp_4089, 0)
        )
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
        assert "status_Success" in state, "RFID Write fail"
        while "status_Success" in state:
            state = nav.get_current_state()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bR0\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_indala_card')
    def test_reference_file_indala_card(self, nav):
        self.compare_rfid_files(nav, "BR0.rfid")

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
        state = nav.get_current_state(
            ref=nav.get_ref_from_string(
                "EM-Micro EM4100", nav.font_helvB08, 0, no_space=1
            )
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("Hex: DC 69 66 0F 12", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("FC: 102", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("Card: 03858 (RF/64)", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Write",
        ]
        assert (
            nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "RFID card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("EM4100", nav.font_haxrcorp_4089, 0)
        )
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
        assert "status_Success" in state, "RFID Write fail"
        while "status_Success" in state:
            state = nav.get_current_state()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bR1\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_em_card')
    def test_reference_file_em_card(self, nav):
        self.compare_rfid_files(nav, "BR1.rfid")

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
        state = nav.get_current_state(
            ref=nav.get_ref_from_string(
                "HID H10301", nav.font_helvB08, 0, no_space=1
            )
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("Hex: F8 C8 8A", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("FC: 248", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("Card: 51338", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Write",
        ]
        assert (
            nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "RFID card menu is wrong"
        nav.go_to("Emulate")
        nav.press_ok()
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = nav.get_current_state(
            ref=nav.get_ref_from_string("H10301", nav.font_haxrcorp_4089, 0)
        )
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
        assert "status_Success" in state, "RFID Write fail"
        while "status_Success" in state:
            state = nav.get_current_state()

        nav.go_to("Save")
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        key.send("bR2\n")
        state = nav.get_current_state()
        while "Saved!" in state:
            state = nav.get_current_state()
        nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_hid_card')
    def test_reference_file_hid_card(self, nav):
        self.compare_rfid_files(nav, "BR2.rfid")

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
