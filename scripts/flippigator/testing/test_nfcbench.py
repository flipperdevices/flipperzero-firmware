import logging
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
class TestNfcBench(BaseCase):
    async def compare_nfc_files(self, nav, name):
        with open("./ref_files/"+name) as ref_file:
            ref = ref_file.read().splitlines()
        try:
            downloaded = await nav.proto.rpc_read("/ext/nfc/"+name).decode("UTF-8").splitlines()
        except nav.proto.FlipperProtoException:
            assert 0, "No dump file for "+name
        d = df.Differ()
        diff = d.compare(ref, downloaded)
        compared = list()
        for i in diff:
            compared.append(i[2:])
        assert compared == ref, "Dump" + name + "is wrong!"

    async def test_read_mifare_classic_1k_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("NFC", "BN0")
        with allure.step("Go to NFC"):
            await nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            await gator.swim_to(-1.0, -10.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
        state = await nav.get_current_state()
        while "Mf Classic User Dict" in state:
            state = await nav.get_current_state()
        state = await nav.get_current_state()
        while "Mf Classic Flipper Dict" in state:
            await nav.press_ok()
            state = await nav.get_current_state()
        state = await nav.get_current_state()
        while "SkipOk" in state:
            state = await nav.get_current_state()
            await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("MIFARE Classic 1K", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "UID: 04 4D 67 32 13 5C 80", nav.font_haxrcorp_4089, 0
            )
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Detect Reader",
            "Unlock with Dictionary",
            "Info",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "NFC card menu is wrong"
        await nav.go_to("Emulate")
        await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "MIFARE Classic 1K", nav.font_haxrcorp_4089, 0, no_space=1
            )
        )
        assert len(state) > 0, "Emulation failed"

        await nav.press_back()

        await nav.go_to("Info")
        await nav.press_ok()
        await nav.press_back()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bN0\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_mifare_classic_1k_card')
    async def test_reference_file_mfc1k_card(self, nav):
         await self.compare_nfc_files(nav, "BN0.nfc")

    async def test_read_nfc_a_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("NFC", "BN1")
        with allure.step("Go to NFC"):
            await nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            await gator.swim_to(-1.0, -90.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
        state = await nav.get_current_state()
        while "Mf Classic User Dict" in state:
            state = await nav.get_current_state()
        state = await nav.get_current_state()
        while "Mf Classic Flipper Dict" in state:
            await nav.press_ok()
            state = await nav.get_current_state()
        state = await nav.get_current_state()
        while "SkipOk" in state:
            state = await nav.get_current_state()
            await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("MIFARE Classic 1K", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("UID: 00 00 00 00", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Detect Reader",
            "Unlock with Dictionary",
            "Info",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "NFC card menu is wrong"
        await nav.go_to("Emulate")
        await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "MIFARE Classic 1K", nav.font_haxrcorp_4089, 0, no_space=1
            )
        )
        assert len(state) > 0, "Emulation failed"

        await nav.press_back()

        await nav.go_to("Info")
        await nav.press_ok()
        await nav.press_back()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bN1\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_nfc_a_card')
    async def test_reference_file_nfc_a_card(self, nav):
        await self.compare_nfc_files(nav, "BN1.nfc")

    async def test_read_mifare_classic_4k_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("NFC", "BN7")
        with allure.step("Go to NFC"):
            await nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            await gator.swim_to(-1.0, -170.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
        state = await nav.get_current_state()
        while "Mf Classic User Dict" in state:
            state = await nav.get_current_state()
        state = await nav.get_current_state()
        while "Mf Classic Flipper Dict" in state:
            await nav.press_ok()
            state = await nav.get_current_state()
        state = await nav.get_current_state()
        while "SkipOk" in state:
            state = await nav.get_current_state()
            await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("MIFARE Classic 4K", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("UID: 7A D6 7C 9D", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "NFC card menu is wrong"
        await nav.go_to("Emulate")
        await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "MIFARE Classic 4K", nav.font_haxrcorp_4089, 0, no_space=1
            )
        )
        assert len(state) > 0, "Emulation failed"

        await nav.press_back()

        await nav.go_to("Info")
        await nav.press_ok()
        await nav.press_back()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bN7\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_mifare_classic_4k_card')
    async def test_reference_file_mfc4k_card(self, nav):
        await self.compare_nfc_files(nav, "BN7.nfc")

    async def test_read_troika_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("NFC", "BN2")
        with allure.step("Go to NFC"):
            await nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            await gator.swim_to(-1.0, -250.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            # key = cv.waitKey(550)
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            time.sleep(0.5)
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Troyka card", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail  (Name)"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Transport department:", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail (Data)"

        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "NFC card menu is wrong"

        await nav.go_to("Emulate")
        await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "MIFARE Classic 1K", nav.font_haxrcorp_4089, 0, no_space=1
            )
        )
        assert len(state) > 0, "Emulation failed"
        await nav.press_back()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bN2\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_troika_card')
    async def test_reference_file_troika_card(self, nav):
        await self.compare_nfc_files(nav, "BN2.nfc")

    async def test_read_ntag215_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("NFC", "BN3")
        with allure.step("Go to NFC"):
            await nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            await gator.swim_to(-1.0, -330.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            # key = cv.waitKey(550)
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("NTAG215", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "UID: 04 77 42 39 BA 07 CO", nav.font_haxrcorp_4089, 0
            )
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "Pages Read: 133/135", nav.font_haxrcorp_4089, 0
            )
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Unlock",
            "Info",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "NFC card menu is wrong"

        await nav.go_to("Emulate")
        await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("NTAG215", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Emulation failed"
        await nav.press_back()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bN3\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_ntag215_card')
    async def test_reference_file_ntag215_card(self, nav):
        await self.compare_nfc_files(nav, "BN3.nfc")

    async def test_read_mifare_ultralight_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("NFC", "BN4")
        with allure.step("Go to NFC"):
            await nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            await gator.swim_to(-1.0, -410.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            # key = cv.waitKey(550)
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("MIFARE Ultralight", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "UID: 04 40 BD BA 5C 48 80", nav.font_haxrcorp_4089, 0
            )
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "NFC card menu is wrong"

        await nav.go_to("Emulate")
        await nav.press_ok()

        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "MIFARE Ultralight", nav.font_haxrcorp_4089, 0, no_space=1
            )
        )
        assert len(state) > 0, "Emulation failed"
        await nav.press_back()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bN4\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_mifare_ultralight_card')
    async def test_reference_file_ultralight_card(self, nav):
        await self.compare_nfc_files(nav, "BN4.nfc")

    async def test_read_mifare_desfire_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("NFC", "BN5")
        with allure.step("Go to NFC"):
            await nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            await gator.swim_to(-110.0, -10.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            # key = cv.waitKey(550)
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("MIFARE DESFire", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "UID: 04 48 6A 32 33 58 80", nav.font_haxrcorp_4089, 0
            )
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate UID",
            "Info",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "NFC card menu is wrong"

        await nav.go_to("Emulate UID")
        await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "Emulating UID", nav.font_helvB08, 0, no_space=1
            )
        )
        assert len(state) > 0, "Emulation failed"
        await nav.press_back()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bN5\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_mifare_desfire_card')
    async def test_reference_file_desfire_card(self, nav):
        await self.compare_nfc_files(nav, "BN5.nfc")

    async def test_read_bank_card(self, nav, gator):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("NFC", "BN8_1")
            await nav.delete_file("NFC", "BN8_2")
            await nav.delete_file("NFC", "BN8_3")
        with allure.step("Go to NFC"):
            await nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            await gator.swim_to(-110.0, -90.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            state = await nav.get_current_state()
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
            state = await nav.get_current_state(
                ref=nav.get_ref_from_string("Multi-protocol card", nav.font_helvB08, 0)
            )
            assert len(state) > 0, "Reading failed"
            with allure.step("Read as ISO14443-4A"):
                await nav.go_to("Read as ISO14443-4A")
                await nav.press_ok()
                await nav.update_screen()
                state = await nav.get_current_state()
                start_time = time.time()
                while "ReadingNFC" in state:
                    nav.logger.debug("Reading")
                    state = await nav.get_current_state()
                    if time.time() - start_time > 10:
                        break
                while "ReadingCardNFC" in state:
                    nav.logger.debug("ReadingCardNFC")
                    state = await nav.get_current_state()

                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "ISO14443-4A (Unknown)", nav.font_helvB08, 0, no_space=1
                    )
                )
                assert len(state) > 0, "Result of reading reference card is fail"
                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "UID: 8E C5 C8 AF", nav.font_haxrcorp_4089, 0
                    )
                )
                assert len(state) > 0, "Result of reading reference card is fail"
                await nav.press_right()
                menu_ref = [
                    "Save",
                    "Emulate UID",
                    "Info",
                ]
                assert (
                    await nav.get_menu_list(
                        ref=nav.get_ref_from_list(
                            menu_ref, nav.font_haxrcorp_4089, invert=1
                        )
                    )
                    == menu_ref
                ), "NFC card menu is wrong"

                await nav.go_to("Emulate UID")
                await nav.press_ok()
                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "Emulating UID", nav.font_helvB08, 0, no_space=1
                    )
                )
                assert len(state) > 0, "Emulation failed"
                await nav.press_back()

                await nav.go_to("Save")
                await nav.press_ok()
                key = FlipperTextKeyboard(nav)
                await key.send("bN8_1\n")
                state = await nav.get_current_state()
                while "Saved!" in state:
                    state = await nav.get_current_state()
            await nav.press_back()
            await nav.press_ok()
            state = await nav.get_current_state()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
            state = await nav.get_current_state(
                ref=nav.get_ref_from_string("Multi-protocol card", nav.font_helvB08, 0)
            )
            assert len(state) > 0, "Reading failed"
            with allure.step("Read as NTAG/Ultralight"):
                await nav.go_to("Read as NTAG/Ultralight")
                await nav.press_ok()
                await nav.update_screen()
                state = await nav.get_current_state()
                start_time = time.time()
                while "ReadingNFC" in state:
                    nav.logger.debug("Reading")
                    state = await nav.get_current_state()
                    if time.time() - start_time > 10:
                        break
                while "ReadingCardNFC" in state:
                    nav.logger.debug("ReadingCardNFC")
                    state = await nav.get_current_state()

                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "MIFARE Ultralight", nav.font_helvB08, 0, no_space=1
                    )
                )
                assert len(state) > 0, "Result of reading reference card is fail"
                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "UID: 8E C5 C8 AF", nav.font_haxrcorp_4089, 0
                    )
                )
                assert len(state) > 0, "Result of reading reference card is fail"
                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "Password-protected pages!", nav.font_haxrcorp_4089, 0
                    )
                )
                assert len(state) > 0, "Result of reading reference card is fail"
                await nav.press_right()
                menu_ref = [
                    "Save",
                    "Emulate",
                    "Unlock",
                    "Info",
                ]
                assert (
                    await nav.get_menu_list(
                        ref=nav.get_ref_from_list(
                            menu_ref, nav.font_haxrcorp_4089, invert=1
                        )
                    )
                    == menu_ref
                ), "NFC card menu is wrong"

                await nav.go_to("Emulate")
                await nav.press_ok()
                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "Emulating", nav.font_helvB08, 0, no_space=1
                    )
                )
                assert len(state) > 0, "Emulation failed"
                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "MIFARE Ultralight", nav.font_haxrcorp_4089, 0, no_space=1
                    )
                )
                assert len(state) > 0, "Emulation failed"
                await nav.press_back()

                await nav.go_to("Save")
                await nav.press_ok()
                key = FlipperTextKeyboard(nav)
                await key.send("bN8_2\n")
                state = await nav.get_current_state()
                while "Saved!" in state:
                    state = await nav.get_current_state()
            await nav.press_back()
            await nav.press_ok()
            state = await nav.get_current_state()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
            state = await nav.get_current_state(
                ref=nav.get_ref_from_string("Multi-protocol card", nav.font_helvB08, 0)
            )
            assert len(state) > 0, "Reading failed"
            with allure.step("Read as MIFARE Classic"):
                await nav.go_to("Read as MIFARE Classic")
                await nav.press_ok()
                await nav.update_screen()
                state = await nav.get_current_state()
                start_time = time.time()
                while "ReadingNFC" in state:
                    nav.logger.debug("Reading")
                    state = await nav.get_current_state()
                    if time.time() - start_time > 10:
                        break
                while "ReadingCardNFC" in state:
                    nav.logger.debug("ReadingCardNFC")
                    state = await nav.get_current_state()

                state = await nav.get_current_state()
                while "Mf Classic User Dict" in state:
                    state = await nav.get_current_state()
                state = await nav.get_current_state()
                while "Mf Classic Flipper Dict" in state:
                    await nav.press_ok()
                    state =await  nav.get_current_state()
                state = await nav.get_current_state()
                while "SkipOk" in state:
                    state = await nav.get_current_state()
                    await nav.press_ok()
                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "MIFARE Classic 1K", nav.font_helvB08, 0
                    )
                )
                assert len(state) > 0, "Result of reading reference card is fail"
                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "UID: 8E C5 C8 AF", nav.font_haxrcorp_4089, 0
                    )
                )
                assert len(state) > 0, "Result of reading reference card is fail"
                await nav.press_right()
                menu_ref = [
                    "Save",
                    "Emulate",
                    "Info",
                ]
                assert (
                    await nav.get_menu_list(
                        ref=nav.get_ref_from_list(
                            menu_ref, nav.font_haxrcorp_4089, invert=1
                        )
                    )
                    == menu_ref
                ), "NFC card menu is wrong"
                await nav.go_to("Emulate")
                await nav.press_ok()
                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
                )
                assert len(state) > 0, "Emulation failed"
                state = await nav.get_current_state(
                    ref=nav.get_ref_from_string(
                        "MiIFARE Classic 1K", nav.font_haxrcorp_4089, 0, no_space=1
                    )
                )
                assert len(state) > 0, "Emulation failed"

                await nav.press_back()

                await nav.go_to("Info")
                await nav.press_ok()
                await nav.press_back()

                await nav.go_to("Save")
                await nav.press_ok()
                key = FlipperTextKeyboard(nav)
                await key.send("bN8_3\n")
                state = await nav.get_current_state()
                while "Saved!" in state:
                    state = await nav.get_current_state()
        await nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_bank_card')
    async def test_reference_file_bank1_card(self, nav):
        await self.compare_nfc_files(nav, "BN8_1.nfc")

    @pytest.mark.run(after='test_read_bank_card')
    async def test_reference_file_bank2_card(self, nav):
        await self.compare_nfc_files(nav, "BN8_2.nfc")

    @pytest.mark.run(after='test_read_bank_card')
    async def test_reference_file_bank3_card(self, nav):
        await self.compare_nfc_files(nav, "BN8_3.nfc")

    async def test_read_all_in_one_card(self, nav, gator, reader_nfc):
        with allure.step("Delete previous card, if possible"):
            await nav.delete_file("NFC", "BN6")
        with allure.step("Go to NFC"):
            await nav.nfc.go_into()
        with allure.step("Swim to NFC card"):
            await gator.swim_to(-110.0, -170.0, 15000)
        with allure.step("Read a card"):
            await nav.go_to("Read")
            await nav.press_ok()
            # key = cv.waitKey(550)
            await nav.update_screen()
            state = await nav.get_current_state()
            state = state[0]
            start_time = time.time()
            while "ReadingNFC" in state:
                nav.logger.debug("Reading")
                state = await nav.get_current_state()
                if time.time() - start_time > 10:
                    break
            while "ReadingCardNFC" in state:
                nav.logger.debug("ReadingCardNFC")
                state = await nav.get_current_state()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("All-In-One", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Number: 2105793721", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Rides left: 0", nav.font_haxrcorp_4089, 0)
        )
        assert len(state) > 0, "Result of reading reference card is fail"
        await nav.press_right()
        menu_ref = [
            "Save",
            "Emulate",
            "Info",
        ]
        assert (
            await nav.get_menu_list(
                ref=nav.get_ref_from_list(menu_ref, nav.font_haxrcorp_4089, invert=1)
            )
            == menu_ref
        ), "NFC card menu is wrong"

        await nav.go_to("Emulate")
        await nav.press_ok()
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string("Emulating", nav.font_helvB08, 0)
        )
        assert len(state) > 0, "Emulation failed"
        state = await nav.get_current_state(
            ref=nav.get_ref_from_string(
                "MIFARE Ultralight", nav.font_haxrcorp_4089, 0, no_space=1
            )
        )
        assert len(state) > 0, "Emulation failed"
        await nav.press_back()

        await nav.go_to("Save")
        await nav.press_ok()
        key = FlipperTextKeyboard(nav)
        await key.send("bN6\n")
        state = await nav.get_current_state()
        while "Saved!" in state:
            state = await nav.get_current_state()
        await nav.go_to_main_screen()

    @pytest.mark.run(after='test_read_all_in_one_card')
    async def test_reference_file_aio_card(self, nav):
        await self.compare_nfc_files(nav, "BN6.nfc")

    async def test_emulation_mifare_classic_1k_card(self, nav, gator, reader_nfc):
        await nav.go_to_main_screen()
        await reader_nfc.go_to_place()
        await reader_nfc.clear()

        if await nav.open_file("NFC", "BN0") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if await reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W805C1332674D04"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()

    async def test_emulation_nfc_a_card(self, nav, gator, reader_nfc):
        await nav.go_to_main_screen()
        await reader_nfc.go_to_place()
        await reader_nfc.clear()

        if await nav.open_file("NFC", "BN1") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if await reader_nfc.update():
                string = reader_nfc.get()
                assert string == "W58W00", "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()

    async def test_emulation_mifare_classic_4k_card(self, nav, gator, reader_nfc):
        await nav.go_to_main_screen()
        await reader_nfc.go_to_place()
        await reader_nfc.clear()

        if await nav.open_file("NFC", "BN7") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if await reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W9D7CD67A000000"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()

    async def test_emulation_troika_card(self, nav, gator, reader_nfc):
        await nav.go_to_main_screen()
        await reader_nfc.go_to_place()
        await reader_nfc.clear()

        if await nav.open_file("NFC", "BN2") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if await reader_nfc.update():
                string = reader_nfc.get()
                assert string == "W58W60950399D1334", "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()

    async def test_emulation_ntag215_card(self, nav, gator, reader_nfc):
        await nav.go_to_main_screen()
        await reader_nfc.go_to_place()
        await reader_nfc.clear()

        if await nav.open_file("NFC", "BN3") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if await reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58WC007BA39427704"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()

    async def test_emulation_mifare_ultralight_card(self, nav, gator, reader_nfc):
        await nav.go_to_main_screen()
        await reader_nfc.go_to_place()
        await reader_nfc.clear()

        if await nav.open_file("NFC", "BN4") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if await reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W80485CBABD4004"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()

    async def test_emulation_mifare_desfire_card(self, nav, gator, reader_nfc):
        await nav.go_to_main_screen()
        await reader_nfc.go_to_place()
        await reader_nfc.clear()

        if await nav.open_file("NFC", "BN5") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 25 > time.time():
            if await reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W805833326A4804"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 25 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()

    async def test_emulation_all_in_one_card(self, nav, gator, reader_nfc):
        await nav.go_to_main_screen()
        await reader_nfc.go_to_place()
        await reader_nfc.clear()

        if await nav.open_file("NFC", "BN6") == -1:
            assert 0, "File not found"
        await nav.get_current_state()

        start_time = time.time()
        while start_time + 5 > time.time():
            if await reader_nfc.update():
                string = reader_nfc.get()
                assert (
                    string == "W58W369CE7B10AC134"
                ), "Emulated NFC card reading failed"
                break

        if start_time + 5 < time.time():
            assert 0, "Timeout of emulation"

        await nav.go_to_main_screen()
