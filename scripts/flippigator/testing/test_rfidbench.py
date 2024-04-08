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
    async def test_read_indala_card(self, nav, gator, reader_indala):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("RFID", "BR0")
        with allure.step("Go to RFID"):
            await nav.rfid.go_into()
        with allure.step("Swim to RFID card"):
            await gator.swim_to(-0.5, -865.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            # key = cv.waitKey(550)
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingRFID" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "Indala26[Motorola]", nav.font_helvB08, 0, no_space=1
            )
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("C8 33 65 B0", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Write",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "RFID card menu is wrong"
        await nav.go_to("Emulate")
        await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Indala26", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Emulation failed"
        await nav.press_back()

        await gator.swim_to(-0.5, -785.0, 15000)
        await nav.go_to("Write")
        await nav.press_ok()
        start_time = time.time()
        state = await nav.get_current_state()
        while ("status_Writing Indala26" in state) or (
            "warn_Still trying to write" in state
        ):
            state = await nav.get_current_state()
            if time.time() - start_time > 25:
                break
        state = await nav.get_current_state()
        assert "status_Successfully writen" in state, "RFID Write fail"
        while "status_Successfully writen" in state:
            state = await nav.get_current_state()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bR0\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    async def test_read_em_card(self, nav, gator, reader_em_hid):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("RFID", "BR1")
        with allure.step("Go to RFID"):
            await nav.rfid.go_into()
        with allure.step("Swim to RFID card"):
            await gator.swim_to(-100.0, -785.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            # key = cv.waitKey(550)
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingRFID" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
        state = await nav.get_current_state()
        assert "card_EM4100_bench" in state, "Result of reading reference card is fail"
        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Write",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "RFID card menu is wrong"
        await nav.go_to("Emulate")
        await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("EM4100", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Emulation failed"
        await nav.press_back()

        await gator.swim_to(-1.0, -785.0, 15000)
        await nav.go_to("Write")
        await nav.press_ok()
        start_time = time.time()
        state = await nav.get_current_state()
        while ("status_Writing EM4100" in state) or (
            "warn_Still trying to write" in state
        ):
            state = await nav.get_current_state()
            if time.time() - start_time > 25:
                break
        state = await nav.get_current_state()
        assert "status_Successfully writen" in state, "RFID Write fail"
        while "status_Successfully writen" in state:
            state = await nav.get_current_state()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bR1\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    async def test_read_hid_card(self, nav, gator, reader_em_hid):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("RFID", "BR2")
        with allure.step("Go to RFID"):
            await nav.rfid.go_into()
        with allure.step("Swim to RFID card"):
            await gator.swim_to(-100.0, -865.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            # key = cv.waitKey(550)
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingRFID" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
        state = await nav.get_current_state()
        assert "card_HID_bench" in state, "Result of reading reference card is fail"
        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Write",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "RFID card menu is wrong"
        await nav.go_to("Emulate")
        await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("H10301", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Emulation failed"
        await nav.press_back()

        await gator.swim_to(-1.0, -785.0, 15000)
        await nav.go_to("Write")
        await nav.press_ok()
        start_time = time.time()
        state = await nav.get_current_state()
        while ("status_Writing H10301" in state) or (
            "warn_Still trying to write" in state
        ):
            state = await nav.get_current_state()
            if time.time() - start_time > 25:
                break
        state = await nav.get_current_state()
        assert "status_Successfully writen" in state, "RFID Write fail"
        while "status_Successfully writen" in state:
            state = await nav.get_current_state()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bR2\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    async def test_emulation_indala_card(self, nav, gator, reader_indala):
        await nav.go_to_main_screen()
        await reader_indala.go_to_place()
        await reader_indala.clear()

        if await nav.open_file("RFID", "BR0") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if await reader_indala.update():
                string = reader_indala.get()
                assert string == "W32W0316CE9E8", "Emulated RFID card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()

    async def test_emulation_em_card(self, nav, gator, reader_em_hid):
        await nav.go_to_main_screen()
        await reader_em_hid.go_to_place()
        await reader_em_hid.clear()

        if await nav.open_file("RFID", "BR1") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if await reader_em_hid.update():
                string = reader_em_hid.get()
                assert string == "W26W0660F12", "Emulated RFID card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()

    async def test_emulation_hid_card(self, nav, gator, reader_em_hid):
        await nav.go_to_main_screen()
        await reader_em_hid.go_to_place()
        await reader_em_hid.clear()

        if await nav.open_file("RFID", "BR2") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if await reader_em_hid.update():
                string = reader_em_hid.get()
                assert string == "W26W0F8C88A", "Emulated RFID card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()
