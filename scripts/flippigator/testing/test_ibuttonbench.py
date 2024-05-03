import os
import time

import allure
import pytest
from flippigator.case import BaseCase
from flippigator.flippigator import FlipperHEXKeyboard, FlipperTextKeyboard

os.system("color")


@pytest.mark.bench_ibutton_ir
class TestIbuttonBench(BaseCase):
    async def test_cyfral(self, nav_reader, nav_key, relay):
        relay.reset()
        with allure.step("Choosing Cyfral key"):
            relay.set_key(9)
        with allure.step("Choosing flipper reader"):
            relay.set_reader(3)
        with allure.step("Emulating key on key_flipper"):
            if await nav_key.open_file("iButton", "BI0") == -1:
                await nav_key.ibutton.go_into()
                await nav_key.go_to("Add Manually")
                await nav_key.press_ok()
                await nav_key.go_to("Cyfral Cyfral")
                await nav_key.press_ok()
                key = FlipperHEXKeyboard(nav_key)
                await key.send("D109")
                time.sleep(0.1)
                key = FlipperTextKeyboard(nav_key)
                await key.send("bI0")
                state = await nav_key.get_current_state()
                while "Saved!" in state:
                    state = await nav_key.get_current_state()
                await nav_key.go_to_main_screen()
                await nav_key.open_file("iButton", "BI0")
        with allure.step("Reading original key by reader_flipper"):
            relay.set_reader(0)
            await nav_reader.ibutton.go_into()
            await nav_reader.go_to("Read")
            relay.set_reader(3)
            await nav_reader.press_ok()

            assert (
                await nav_reader.wait_for_state("key_Cyfral_bench", timeout=5) == 0
            ), "Reading reference key failed"

        with allure.step("Reading emulated key by reader_flipper"):
            relay.set_key(8)
            time.sleep(0.1)
            await nav_reader.press_left()
            await nav_reader.press_left()

            assert (
                await nav_reader.wait_for_state("key_Cyfral_bench", timeout=5) == 0
            ), "Reading emulated key failed"

        with allure.step("Cheking key menu"):
            await nav_reader.press_right()
            menu_ref = [
                "Save",
                "Emulate",
            ]
            assert (
                await nav_reader.get_menu_list(
                    ref=nav_reader.get_ref_from_list(
                        menu_ref, nav_reader.font_haxrcorp_4089, invert=1
                    )
                )
                == menu_ref
            ), "Key actions menu is wrong"
        relay.reset()

    async def test_metakom(self, nav_reader, nav_key, relay):
        relay.reset()
        with allure.step("Choosing Metakom key"):
            relay.set_key(10)
        with allure.step("Choosing flipper reader"):
            relay.set_reader(3)
        with allure.step("Emulating key on key_flipper"):
            if await nav_key.open_file("iButton", "BI1") == -1:
                await nav_key.ibutton.go_into()
                await nav_key.go_to("Read")
                relay.set_reader(2)
                await nav_key.press_ok()
                assert (
                    await nav_key.wait_for_state("key_Metakom_bench", timeout=5) == 0
                ), "No reference key"
                time.sleep(0.1)
                await nav_key.press_right()
                await nav_key.go_to("Save")
                await nav_key.press_ok()
                key = FlipperTextKeyboard(nav_key)
                await key.send("bI1")
                state = await nav_key.get_current_state()
                while "Saved!" in state:
                    state = await nav_key.get_current_state()
                await nav_key.go_to_main_screen()
                relay.set_reader(3)
                assert await nav_key.open_file("iButton", "BI1") == 0, "File not found"
        with allure.step("Reading original key by reader_flipper"):
            relay.set_reader(0)
            await nav_reader.ibutton.go_into()
            await nav_reader.go_to("Read")
            relay.set_reader(3)
            await nav_reader.press_ok()
            assert (
                await nav_reader.wait_for_state("key_Metakom_bench", timeout=5) == 0
            ), "Reading reference key failed"
        with allure.step("Reading emulated key by reader_flipper"):
            relay.set_key(8)
            time.sleep(0.1)
            await nav_reader.press_left()
            await nav_reader.press_left()
            assert (
                await nav_reader.wait_for_state("key_Metakom_bench", timeout=5) == 0
            ), "Reading emulated key failed"
        with allure.step("Cheking key menu"):
            await nav_reader.press_right()
            menu_ref = [
                "Save",
                "Emulate",
            ]
            assert (
                await nav_reader.get_menu_list(
                    ref=nav_reader.get_ref_from_list(
                        menu_ref, nav_reader.font_haxrcorp_4089, invert=1
                    )
                )
                == menu_ref
            ), "Key actions menu is wrong"
        relay.reset()

    async def test_ds1992(self, nav_reader, nav_key, relay):
        relay.reset()
        with allure.step("Choosing Metakom key"):
            relay.set_key(12)
        with allure.step("Choosing flipper reader"):
            relay.set_reader(3)
        with allure.step("Emulating key on key_flipper"):
            if await nav_key.open_file("iButton", "BI2") == -1:
                await nav_key.ibutton.go_into()
                await nav_key.go_to("Read")
                relay.set_reader(2)
                await nav_key.press_ok()
                assert (
                    await nav_key.wait_for_state("key_DS1992_bench", timeout=5) == 0
                ), "No reference key"
                time.sleep(0.1)
                await nav_key.press_right()
                await nav_key.go_to("Save")
                await nav_key.press_ok()
                key = FlipperTextKeyboard(nav_key)
                key.send("bI2")
                state = await nav_key.get_current_state()
                while "Saved!" in state:
                    state = await nav_key.get_current_state()
                await nav_key.go_to_main_screen()
                relay.set_reader(3)
                assert await nav_key.open_file("iButton", "BI2") == 0, "File not found"
        with allure.step("Reading original key by reader_flipper"):
            relay.set_reader(0)
            await nav_reader.ibutton.go_into()
            await nav_reader.go_to("Read")
            relay.set_reader(3)
            await nav_reader.press_ok()
            assert (
                await nav_reader.wait_for_state("key_DS1992_bench", timeout=5) == 0
            ), "Reading reference key failed"
        with allure.step("Reading emulated key by reader_flipper"):
            relay.set_key(8)
            time.sleep(0.1)
            await nav_reader.press_left()
            await nav_reader.press_left()
            assert (
                await nav_reader.wait_for_state("key_DS1992_bench", timeout=5) == 0
            ), "Reading emulated key failed"
        with allure.step("Cheking key menu"):
            await nav_reader.press_right()
            menu_ref = [
                "Save",
                "Emulate",
                "Write ID",
                "Full Write on Same Type",
                "Data info",
            ]
            assert (
                await nav_reader.get_menu_list(
                    ref=nav_reader.get_ref_from_list(
                        menu_ref, nav_reader.font_haxrcorp_4089, invert=1
                    )
                )
                == menu_ref
            ), "Key actions menu is wrong"
        relay.reset()

    async def test_ds1996(self, nav_reader, nav_key, relay):
        relay.reset()
        with allure.step("Choosing Metakom key"):
            relay.set_key(13)
        with allure.step("Choosing flipper reader"):
            relay.set_reader(3)
        with allure.step("Emulating key on key_flipper"):
            if await nav_key.open_file("iButton", "BI3") == -1:
                await nav_key.ibutton.go_into()
                await nav_key.go_to("Read")
                relay.set_reader(2)
                await nav_key.press_ok()
                assert (
                    await nav_key.wait_for_state("key_DS1996_bench", timeout=5) == 0
                ), "No reference key"
                time.sleep(0.1)
                await nav_key.press_right()
                await nav_key.go_to("Save")
                await nav_key.press_ok()
                key = FlipperTextKeyboard(nav_key)
                await key.send("bI3")
                state = await nav_key.get_current_state()
                while "Saved!" in state:
                    state = await nav_key.get_current_state()
                await nav_key.go_to_main_screen()
                relay.set_reader(3)
                assert await nav_key.open_file("iButton", "BI3") == 0, "File not found"
        with allure.step("Reading original key by reader_flipper"):
            relay.set_reader(0)
            await nav_reader.ibutton.go_into()
            await nav_reader.go_to("Read")
            relay.set_reader(3)
            await nav_reader.press_ok()
            assert (
                await nav_reader.wait_for_state("key_DS1996_bench", timeout=5) == 0
            ), "Reading reference key failed"
        with allure.step("Reading emulated key by reader_flipper"):
            relay.set_key(8)
            time.sleep(0.1)
            await nav_reader.press_left()
            await nav_reader.press_left()
            assert (
                await nav_reader.wait_for_state("key_DS1996_bench", timeout=5) == 0
            ), "Reading emulated key failed"
        with allure.step("Cheking key menu"):
            await nav_reader.press_right()
            menu_ref = [
                "Save",
                "Emulate",
                "Full Write on Same Type",
                "Data Info",
            ]
            assert (
                await nav_reader.get_menu_list(
                    ref=nav_reader.get_ref_from_list(
                        menu_ref, nav_reader.font_haxrcorp_4089, invert=1
                    )
                )
                == menu_ref
            ), "Key actions menu is wrong"
        relay.reset()

    async def test_ds1971(self, nav_reader, nav_key, relay):
        relay.reset()
        with allure.step("Choosing Metakom key"):
            relay.set_key(14)
        with allure.step("Choosing flipper reader"):
            relay.set_reader(3)
        with allure.step("Emulating key on key_flipper"):
            if await nav_key.open_file("iButton", "BI4") == -1:
                await nav_key.ibutton.go_into()
                await nav_key.go_to("Read")
                relay.set_reader(2)
                await nav_key.press_ok()
                assert (
                    await nav_key.wait_for_state("key_DS1971_bench", timeout=5) == 0
                ), "No reference key"
                time.sleep(0.1)
                await nav_key.press_right()
                await nav_key.go_to("Save")
                await nav_key.press_ok()
                key = FlipperTextKeyboard(nav_key)
                key.send("bI4")
                state = await nav_key.get_current_state()
                while "Saved!" in state:
                    state = await nav_key.get_current_state()
                await nav_key.go_to_main_screen()
                relay.set_reader(3)
                assert await nav_key.open_file("iButton", "BI4") == 0, "File not found"
        with allure.step("Reading original key by reader_flipper"):
            relay.set_reader(0)
            await nav_reader.ibutton.go_into()
            await nav_reader.go_to("Read")
            relay.set_reader(3)
            await nav_reader.press_ok()
            assert (
                await nav_reader.wait_for_state("key_DS1971_bench", timeout=5) == 0
            ), "Reading reference key failed"
        with allure.step("Reading emulated key by reader_flipper"):
            relay.set_key(8)
            time.sleep(0.1)
            await nav_reader.press_left()
            await nav_reader.press_left()
            assert (
                await nav_reader.wait_for_state("key_DS1971_bench", timeout=5) == 0
            ), "Reading emulated key failed"
        with allure.step("Cheking key menu"):
            await nav_reader.press_right()
            menu_ref = [
                "Save",
                "Emulate",
                "Full Write on Same Type",
                "Data Info",
            ]
            assert (
                await nav_reader.get_menu_list(
                    ref=nav_reader.get_ref_from_list(
                        menu_ref, nav_reader.font_haxrcorp_4089, invert=1
                    )
                )
                == menu_ref
            ), "Key actions menu is wrong"
        relay.reset()
