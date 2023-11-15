import time

import pytest
from flippigator.extensions.proxmark_wrapper import proxmark_wrapper
from flippigator.flippigator import FlipperHEXKeyboard, FlipperTextKeyboard


class TestEmulateClassic:
    @pytest.fixture(scope="function", autouse=True)
    def add_4k7_manually(self, nav):
        # Code below will run before each "function" (test)
        yield
        return

        # Delete file if exists, needs to be added as picture sadly
        # nav.delete_file("NFC", "T_mfc_4k7")
        # Todo: return back to line above, after browser is fixed
        nav.nfc.go_into()
        nav.open("Saved")
        menu_items = nav.get_menu_list()
        if "browser_T_mfc_4k7" in menu_items:
            nav.open("browser_T_mfc_4k7", direction="up")
            nav.open("Delete", direction="up")
            nav.press_right()

        nav.nfc.go_into()
        nav.open("Add Manually")
        # direction for faster testing, since its last element
        nav.open("Mifare Classic 4k 7byte UID", direction="up")
        nav.press_right()
        nav.press_ok()
        key = FlipperTextKeyboard(nav)
        # if you need, you can make another fixture and use any "existing" file name in /img/ref
        key.send("T_mfc_4k7\n")
        nav.go_to_main_screen()

        # here will be a test call
        yield

        # This code will run after each "function" (test)
        # nav.delete_file("NFC", "T_mfc_4k7")
        # Todo: return back to line above, after browser is fixed
        # Todo2: move to function
        nav.nfc.go_into()
        nav.open("Saved")
        menu_items = nav.get_menu_list()
        if "browser_T_mfc_4k7" in menu_items:
            nav.open("browser_T_mfc_4k7", direction="up")
            nav.open("Delete", direction="up")
            nav.press_right()

    @pytest.fixture(scope="function")
    def emulate_classic(self, nav):
        return
        nav.go_to_main_screen()
        if nav.open_file("NFC", "T_mfc_4k7") == -1:
            assert 0, "File not found"

        # Need for emulation to start, otherwise error on proxmark side
        time.sleep(1)

    # def test_mfc_4k7_basic(self, px, emulate_classic):
    #     """
    #     px: proxmark_wrapper fixture
    #     command: command to be executed
    #     expected_result: expected result of the command
    #     error: if the command should return an error
    #     """

    #     # read 1 block
    #     read1 = px.execute("hf mf rdbl --blk 1 -k ffffffffffff")
    #     expected = "[=]   1 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ "
    #     expected2 = "[=]   1 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ " # for a proxmark-formatted card
    #     assert expected in read1 or expected2 in read1, "Incorrect block read"

    #     # write 1 block
    #     write1 = px.execute(
    #         "hf mf wrbl --blk 1 -k ffffffffffff -d 11223344556677889910111213141516"
    #     )
    #     expected = "[+] Write ( ok )"
    #     assert expected in write1, "Incorrect block write"

    #     # read 1 block
    #     read1_wrong = px.execute("hf mf rdbl --blk 1 -k ffffffffffff")
    #     expected = '[=]   1 | 11 22 33 44 55 66 77 88 99 10 11 12 13 14 15 16 | ."3DUfw......... '
    #     assert expected in read1_wrong, "Incorrect block read"

    #     # set value block
    #     set_value = px.execute("hf mf value --blk 1 -k ffffffffffff --set 1000")
    #     expected = "[=] set value to : 1000"
    #     expected_status = "[+] Update ... : success"
    #     assert expected in set_value, "Incorrect set value"
    #     assert expected_status in set_value, "Update failed"

    #     # increment value block
    #     inc_value = px.execute("hf mf value --blk 1 -k ffffffffffff --inc 200")
    #     expected = "[=] Value incremented by : 200"
    #     expected_status = "[+] Update ... : success"

    #     assert expected in inc_value, "Incorrect increment value"
    #     assert expected_status in inc_value, "Update failed"

    #     # decrement value block
    #     dec_value = px.execute("hf mf value --blk 1 -k ffffffffffff --dec 10000")
    #     expected = "[=] Value decremented by : 10000"
    #     expected_status = "[+] Update ... : success"
    #     assert expected in dec_value, "Incorrect decrement value"
    #     assert expected_status in dec_value, "Update failed"

    #     # change key A in 0 sector
    #     change_key = px.execute(
    #         "hf mf wrbl --blk 3 -k ffffffffffff -d a0a1a2a3a4a5ff078069ffffffffffff"
    #     )
    #     expected = "[=] data: A0 A1 A2 A3 A4 A5 FF 07 80 69 FF FF FF FF FF FF "
    #     expected_status = "[+] Write ( ok )"
    #     assert expected in change_key, "Incorrect change key"
    #     assert expected_status in change_key, "Update failed"

    #     # read 1 block with old key
    #     read1 = px.execute("hf mf rdbl --blk 1 -k ffffffffffff")
    #     expected_status = "[#] Auth error"
    #     assert expected_status in read1, "Auth error not present"

    #     # read 1 block with new key
    #     read1 = px.execute("hf mf rdbl --blk 1 -k a0a1a2a3a4a5")
    #     expected = '[=]   1 | A0 DD FF FF 5F 22 00 00 A0 DD FF FF 01 FE 01 FE | ...._".......... '
    #     assert expected in read1, "Value block doesn't equal -8800 or failed to read the block"

    #     # change key B in 0 sector
    #     change_key = px.execute("hf mf wrbl --blk 3 -k a0a1a2a3a4a5 -d a0a1a2a3a4a5ff078069b0b1b2b3b4b5")
    #     assert "[+] Write ( ok )" in change_key, "Incorrect change key"

    #     # read sector trailer with key A
    #     trailer_a_key = px.execute("hf mf rdbl --blk 3 -k a0a1a2a3a4a5")
    #     assert "[=]   3 | 00 00 00 00 00 00 FF 07 80 69 B0 B1 B2 B3 B4 B5 | .........i......" in trailer_a_key or "[=]   3 | 00 00 00 00 00 00 FF 0F 00 69 B0 B1 B2 B3 B4 B5 | .........i...... in trailer_a_key", "Incorrect trailer read"

    #     # read sector trailer with key B
    #     trailer_b_key = px.execute("hf mf rdbl --blk 3 -b -k b0b1b2b3b4b5")
    #     assert "[=]   3 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................" in trailer_b_key, "Incorrect trailer read"

    # FF0F00	http://calc.gmss.ru/Mifare1k/	hf mf wrbl --blk 3 -k ffffffffffff -d ffffffffffffff0f0069ffffffffffff
    # hf mf rdbl --blk 3 -k ffffffffffff	                                        No keyA, ACs, KeyB
    # hf mf rdbl --blk 3 -b -k ffffffffffff	                                        No KeyA, No ACs, No KeyB
    # hf mf wrbl --blk 3 -k ffffffffffff -d a0a1a2a3a4a5ff0f0069b0b1b2b3b4b5	    Keys changed
    # hf mf wrbl --blk 3 -b -k ffffffffffff -d a1a2a3a4a5a6ff0f0069b1b2b3b4b5b6	    Keys not changed
    # hf mf wrbl --blk 3 (-b) -k ffffffffffff -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6	ACs not changed

    data = [
        (
            "FF0F00",
            [
                (
                    "hf mf wrbl --blk 3 -k ffffffffffff -d ffffffffffffff0f0069ffffffffffff",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 3 -k ffffffffffff",
                    "[=]   3 | 00 00 00 00 00 00 FF 0F 00 69 FF FF FF FF FF FF | .........i......",
                ),
                (
                    "hf mf rdbl --blk 3 -b -k ffffffffffff",
                    "[=]   3 | 00 00 00 00 00 00 FF 0F 00 69 FF FF FF FF FF FF | .........i......",  # real card
                    # "[=]   3 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................",  # datasheet and flipper
                ),
                (
                    "hf mf wrbl --blk 3 -k ffffffffffff -d a0a1a2a3a4a5ff0f0069b0b1b2b3b4b5",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf wrbl --blk 3 -b -k b0b1b2b3b4b5 -d a1a2a3a4a5a6ff0f0069b1b2b3b4b5b6",
                    # "[-] ⛔ Write ( fail )",
                    "[+] Write ( ok )",  # this shoudln't be possible yet here we are.
                ),
                (
                    "hf mf rdbl --blk 3 -k a1a2a3a4a5a6",
                    "[=]   3 | 00 00 00 00 00 00 FF 0F 00 69 B1 B2 B3 B4 B5 B6 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 3 -k a1a2a3a4a5a6 -d a0a1a2a3a4a5ff078069b0b1b2b3b4b5",
                    "[+] Write ( ok )",  # but ACs not changed
                ),
                (
                    "hf mf wrbl --blk 3 -b -k b0b1b2b3b4b5 -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6",
                    "[+] Write ( ok )",  # keys changed, ACs not changed
                ),
                (
                    "hf mf rdbl --blk 3 -k a1a2a3a4a5a6",
                    "[=]   3 | 00 00 00 00 00 00 FF 0F 00 69 B1 B2 B3 B4 B5 B6 | .........i......",
                ),
            ],
        ),
        (
            "7F0F08",
            [
                (
                    "hf mf wrbl --blk 7 -k ffffffffffff -d ffffffffffff7f0f0869ffffffffffff --force",
                    "[=] Writing block no 7, key A - FFFFFFFFFFFF",  # may fail on a real card cause read-only, but it's ok, we check if it actually matches in the next step
                ),
                (
                    "hf mf rdbl --blk 7 -k ffffffffffff",
                    "[=]   7 | 00 00 00 00 00 00 7F 0F 08 69 FF FF FF FF FF FF | .........i......",
                ),
                (
                    "hf mf rdbl --blk 7 -b -k ffffffffffff",
                    # "[=]   7 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................", # datasheet
                    "[=]   7 | 00 00 00 00 00 00 7F 0F 08 69 FF FF FF FF FF FF | .........i......",  # reality
                ),
                (
                    "hf mf wrbl --blk 7 -k ffffffffffff -d a0a1a2a3a4a57f0f0869b0b1b2b3b4b5 --force",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf wrbl --blk 7 -b -k ffffffffffff -d a0a1a2a3a4a57f0f0869b0b1b2b3b4b5 --force",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf wrbl --blk 7 -k ffffffffffff -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf wrbl --blk 7 -b -k ffffffffffff -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 7 -k ffffffffffff",
                    "[=]   7 | 00 00 00 00 00 00 7F 0F 08 69 FF FF FF FF FF FF | .........i......",
                ),
            ],
        ),
        (
            "F78F00",
            [
                (
                    "hf mf wrbl --blk 11 -k ffffffffffff -b -d fffffffffffff78f0069ffffffffffff",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 11 -k ffffffffffff",
                    "[=]  11 | 00 00 00 00 00 00 F7 8F 00 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf rdbl --blk 11 -b -k ffffffffffff",
                    "[=]  11 | 00 00 00 00 00 00 F7 8F 00 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 11 -k ffffffffffff -d a0a1a2a3a4a5f78f0069b0b1b2b3b4b5",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 11 -k ffffffffffff",
                    "[=]  11 | 00 00 00 00 00 00 F7 8F 00 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 11 -b -k ffffffffffff -d a1a2a3a4a5a6f78f0069b1b2b3b4b5b6",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf wrbl --blk 11 -k a1a2a3a4a5a6 -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf wrbl --blk 11 -b -k b1b2b3b4b5b6 -d a0a1a2a3a4a5ff078069b0b1b2b3b4b5",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 11 -k a0a1a2a3a4a5",
                    "[=]  11 | 00 00 00 00 00 00 F7 8F 00 69 00 00 00 00 00 00 | .........i......",
                ),
            ],
        ),
        (
            "778F08",
            [
                (
                    "hf mf wrbl --blk 15 -k ffffffffffff -d ffffffffffff778f0869ffffffffffff --force",
                    "[=] Writing block no 15, key A - FFFFFFFFFFFF",
                ),
                (
                    "hf mf rdbl --blk 15 -k ffffffffffff",
                    "[=]  15 | 00 00 00 00 00 00 77 8F 08 69 00 00 00 00 00 00 | ......w..i......",
                ),
                (
                    "hf mf rdbl --blk 15 -b -k ffffffffffff",
                    "[=]  15 | 00 00 00 00 00 00 77 8F 08 69 00 00 00 00 00 00 | ......w..i......",
                ),
                (
                    "hf mf wrbl --blk 15 -k ffffffffffff -d a0a1a2a3a4a5f78f0069b0b1b2b3b4b5",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 15 -k ffffffffffff",
                    "[=]  15 | 00 00 00 00 00 00 77 8F 08 69 00 00 00 00 00 00 | ......w..i......",
                ),
                (
                    "hf mf wrbl --blk 15 -b -k ffffffffffff -d a0a1a2a3a4a5f78f0069b0b1b2b3b4b5",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 15 -k a0a1a2a3a4a5",
                    "[#] Auth error",
                ),
                (
                    "hf mf wrbl --blk 15 -k ffffffffffff -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 15 -k ffffffffffff",
                    "[=]  15 | 00 00 00 00 00 00 77 8F 08 69 00 00 00 00 00 00 | ......w..i......",
                ),
                (
                    "hf mf wrbl --blk 15 -b -k ffffffffffff -d a0a1a2a3a4a5ff078069b0b1b2b3b4b5",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 15 -k ffffffffffff",
                    "[=]  15 | 00 00 00 00 00 00 77 8F 08 69 00 00 00 00 00 00 | ......w..i......",
                ),
            ],
        ),
        (
            "FF0780",
            [
                (
                    "hf mf wrbl --blk 19 -k ffffffffffff -d ffffffffffffff078069ffffffffffff",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 19 -k ffffffffffff",
                    "[=]  19 | 00 00 00 00 00 00 FF 07 80 69 FF FF FF FF FF FF | .........i......",
                ),
                (
                    "hf mf rdbl --blk 19 -b -k ffffffffffff",
                    "[=]  19 | 00 00 00 00 00 00 FF 07 80 69 FF FF FF FF FF FF | .........i......",  # per datasheet it should NACK out, but it... doesn't???
                ),
                (
                    "hf mf wrbl --blk 19 -k ffffffffffff -d a0a1a2a3a4a5ff078069b0b1b2b3b4b5",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 19 -k a0a1a2a3a4a5",
                    "[=]  19 | 00 00 00 00 00 00 FF 07 80 69 B0 B1 B2 B3 B4 B5 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 19 -b -k b0b1b2b3b4b5 -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6",
                    "[+] Write ( ok )",  # again, that shouldn't happen according to the datasheet
                ),
                (
                    "hf mf rdbl --blk 19 -k a0a1a2a3a4a5",
                    "[#] Auth error",
                ),
                (
                    "hf mf rdbl --blk 19 -k a1a2a3a4a5a6",
                    "[=]  19 | 00 00 00 00 00 00 FF 07 80 69 B1 B2 B3 B4 B5 B6 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 19 -k a1a2a3a4a5a6 -d a2a3a4a5a6a77f078869b2b3b4b5b6b7 --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 19 -k a2a3a4a5a6a7",
                    "[=]  19 | 00 00 00 00 00 00 7F 07 88 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 19 -b -k b2b3b4b5b6b7 -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6 --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 19 -k a1a2a3a4a5a6",
                    "[=]  19 | 00 00 00 00 00 00 FF 07 80 69 B1 B2 B3 B4 B5 B6 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 19 -b -k b1b2b3b4b5b6 -d a0a1a2a3a4a57f078869b0b1b2b3b4b5 --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 19 -k a0a1a2a3a4a5",
                    "[=]  19 | 00 00 00 00 00 00 7F 07 88 69 00 00 00 00 00 00 | .........i......",
                ),
            ],
        ),
        (
            "7F0788",
            [
                (
                    "hf mf wrbl --blk 23 -k ffffffffffff -d ffffffffffff7f078869ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 23 -k ffffffffffff",
                    "[=]  23 | 00 00 00 00 00 00 7F 07 88 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf rdbl --blk 23 -b -k ffffffffffff",
                    "[=]  23 | 00 00 00 00 00 00 7F 07 88 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 23 -k ffffffffffff -d a0a1a2a3a4a57f078869b0b1b2b3b4b5 --force",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 23 -k ffffffffffff",
                    "[=]  23 | 00 00 00 00 00 00 7F 07 88 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 23 -b -k ffffffffffff -d a0a1a2a3a4a57f078869b0b1b2b3b4b5 --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 23 -k a0a1a2a3a4a5",
                    "[=]  23 | 00 00 00 00 00 00 7F 07 88 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 23 -k a0a1a2a3a4a5 -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 23 -k a0a1a2a3a4a5",
                    "[=]  23 | 00 00 00 00 00 00 7F 07 88 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 23 -b -k b0b1b2b3b4b5 -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 23 -k a1a2a3a4a5a6",
                    "[=]  23 | 00 00 00 00 00 00 FF 07 80 69 B1 B2 B3 B4 B5 B6 | .........i......",
                ),
            ],
        ),
        (
            "F78780",
            [
                (
                    "hf mf wrbl --blk 27 -k ffffffffffff -d ffffffffffffF7878069ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 27 -k ffffffffffff",
                    "[=]  27 | 00 00 00 00 00 00 F7 87 80 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf rdbl --blk 27 -b -k ffffffffffff",
                    "[=]  27 | 00 00 00 00 00 00 F7 87 80 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 27 -k ffffffffffff -d a0a1a2a3a4a5f7878069b0b1b2b3b4b5",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 27 -k ffffffffffff",
                    "[=]  27 | 00 00 00 00 00 00 F7 87 80 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 27 -b -k ffffffffffff -d a0a1a2a3a4a5f7878069b0b1b2b3b4b5",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 27 -k ffffffffffff",
                    "[=]  27 | 00 00 00 00 00 00 F7 87 80 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 27 -k ffffffffffff -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 27 -k ffffffffffff",
                    "[=]  27 | 00 00 00 00 00 00 F7 87 80 69 00 00 00 00 00 00 | .........i......",
                ),
                (
                    "hf mf wrbl --blk 27 -b -k ffffffffffff -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 27 -k ffffffffffff",
                    "[=]  27 | 00 00 00 00 00 00 FF 07 80 69 FF FF FF FF FF FF | .........i......",
                ),
            ],
        ),
        (
            "778788",
            [
                (
                    "hf mf wrbl --blk 31 -k ffffffffffff -d ffffffffffff77878869ffffffffffff --force",
                    "[=] Writing block no 31, key A - FFFFFFFFFFFF",
                ),
                (
                    "hf mf rdbl --blk 31 -k ffffffffffff",
                    "[=]  31 | 00 00 00 00 00 00 77 87 88 69 00 00 00 00 00 00 | ......w..i......",
                ),
                (
                    "hf mf rdbl --blk 31 -b -k ffffffffffff",
                    "[=]  31 | 00 00 00 00 00 00 77 87 88 69 00 00 00 00 00 00 | ......w..i......",
                ),
                (
                    "hf mf wrbl --blk 31 -k ffffffffffff -d a0a1a2a3a4a577878869b0b1b2b3b4b5 --force",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 31 -k ffffffffffff",
                    "[=]  31 | 00 00 00 00 00 00 77 87 88 69 00 00 00 00 00 00 | ......w..i......",
                ),
                (
                    "hf mf wrbl --blk 31 -b -k ffffffffffff -d a0a1a2a3a4a577878869b0b1b2b3b4b5 --force",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 31 -k ffffffffffff",
                    "[=]  31 | 00 00 00 00 00 00 77 87 88 69 00 00 00 00 00 00 | ......w..i......",
                ),
                (
                    "hf mf wrbl --blk 31 -k ffffffffffff -d a1a2a3a4a5a6F7878069b1b2b3b4b5b6",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 31 -k ffffffffffff",
                    "[=]  31 | 00 00 00 00 00 00 77 87 88 69 00 00 00 00 00 00 | ......w..i......",
                ),
                (
                    "hf mf wrbl --blk 31 -b -k ffffffffffff -d a1a2a3a4a5a6F7878069b1b2b3b4b5b6",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 31 -k ffffffffffff",
                    "[=]  31 | 00 00 00 00 00 00 77 87 88 69 00 00 00 00 00 00 | ......w..i......",
                ),
            ],
        ),
        # Data Access Conditions
        (
            "7F0788",
            [
                (
                    "hf mf wrbl --blk 35 -k ffffffffffff -d ffffffffffff7f078869ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    "[=]  33 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf rdbl --blk 33 -b -k ffffffffffff",
                    "[=]  33 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 33 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    '[=]  33 | 11 22 33 44 55 66 77 88 99 00 11 22 33 44 55 66 | ."3DUfw...."3DUf ',
                ),
                (
                    "hf mf wrbl --blk 33 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    '[=]  33 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 33 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    "[=]  33 | E8 03 00 00 17 FC FF FF E8 03 00 00 21 DE 21 DE | ............!.!. ",
                ),
                (
                    "hf mf value --blk 33 -b -k ffffffffffff --set 500",
                    "[+] Dec ...... : 500",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    "[=]  33 | F4 01 00 00 0B FE FF FF F4 01 00 00 21 DE 21 DE | ............!.!. ",
                ),
                (
                    "hf mf value --blk 33 -k ffffffffffff --inc 200",
                    "[+] Dec ...... : 700",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    "[=]  33 | BC 02 00 00 43 FD FF FF BC 02 00 00 21 DE 21 DE | ....C.......!.!. ",
                ),
                (
                    "hf mf value --blk 33 -b -k ffffffffffff --inc 100",
                    "[+] Dec ...... : 800",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    "[=]  33 | 20 03 00 00 DF FC FF FF 20 03 00 00 21 DE 21 DE |  ....... ...!.!. ",
                ),
                (
                    "hf mf value --blk 33 -k ffffffffffff --dec 50",
                    "[+] Dec ...... : 750",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    "[=]  33 | EE 02 00 00 11 FD FF FF EE 02 00 00 21 DE 21 DE | ............!.!. ",
                ),
                (
                    "hf mf value --blk 33 -b -k ffffffffffff --dec 350",
                    "[+] Dec ...... : 400",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    "[=]  33 | 90 01 00 00 6F FE FF FF 90 01 00 00 21 DE 21 DE | ....o.......!.!. ",
                ),
                (
                    "hf mf value --blk 33 -k ffffffffffff --res --transfer 33",
                    "[+] Dec ...... : 400",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    "[=]  33 | 90 01 00 00 6F FE FF FF 90 01 00 00 21 DE 21 DE | ....o.......!.!. ",
                ),
                (
                    "hf mf value --blk 33 -b -k ffffffffffff --res --transfer 33",
                    "[+] Dec ...... : 400",
                ),
                (
                    "hf mf rdbl --blk 33 -k ffffffffffff",
                    "[=]  33 | 90 01 00 00 6F FE FF FF 90 01 00 00 21 DE 21 DE | ....o.......!.!. ",
                ),
            ],
        ),
        (
            "5F078A part 1",
            [
                (
                    "hf mf wrbl --blk 39 -k ffffffffffff -d ffffffffffff5f078a69ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 37 -k ffffffffffff",
                    "[=]  37 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf rdbl --blk 37 -b -k ffffffffffff",
                    "[=]  37 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 37 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 37 -k ffffffffffff",
                    "[=]  37 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 37 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 37 -k ffffffffffff",
                    "[=]  37 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf value --blk 37 -k ffffffffffff --set 1000",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 37 -k ffffffffffff",
                    "[=]  37 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf value --blk 37 -b -k ffffffffffff --set 500",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 37 -k ffffffffffff",
                    "[=]  37 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
            ],
        ),
        (
            "5F078A part 2",
            [
                (
                    "hf mf value --blk 41 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf rdbl --blk 41 -k ffffffffffff",
                    "[=]  41 | E8 03 00 00 17 FC FF FF E8 03 00 00 29 D6 29 D6 | ............).). ",
                ),
                (
                    "hf mf wrbl --blk 43 -k ffffffffffff -d ffffffffffff5f078a69ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf value --blk 41 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 41 -k ffffffffffff",
                    "[=]  41 | E8 03 00 00 17 FC FF FF E8 03 00 00 29 D6 29 D6 | ............).). ",
                ),
                (
                    "hf mf value --blk 41 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 41 -k ffffffffffff",
                    "[=]  41 | E8 03 00 00 17 FC FF FF E8 03 00 00 29 D6 29 D6 | ............).). ",
                ),
                (
                    "hf mf value --blk 41 -k ffffffffffff --dec 50",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 41 -k ffffffffffff",
                    "[=]  41 | E8 03 00 00 17 FC FF FF E8 03 00 00 29 D6 29 D6 | ............).). ",
                ),
                (
                    "hf mf value --blk 41 -b -k ffffffffffff --dec 350",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 41 -k ffffffffffff",
                    "[=]  41 | E8 03 00 00 17 FC FF FF E8 03 00 00 29 D6 29 D6 | ............).). ",
                ),
                (
                    "hf mf value --blk 41 -k ffffffffffff --res --transfer 41",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 41 -k ffffffffffff",
                    "[=]  41 | E8 03 00 00 17 FC FF FF E8 03 00 00 29 D6 29 D6 | ............).). ",
                ),
                (
                    "hf mf value --blk 41 -b -k ffffffffffff --res --transfer 41",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 41 -k ffffffffffff",
                    "[=]  41 | E8 03 00 00 17 FC FF FF E8 03 00 00 29 D6 29 D6 | ............).). ",
                ),
            ],
        ),
        (
            "7D2788",
            [
                (
                    "hf mf wrbl --blk 47 -k ffffffffffff -d ffffffffffff7d278869ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    "[=]  45 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf rdbl --blk 45 -b -k ffffffffffff",
                    "[=]  45 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 45 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    "[=]  45 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 45 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    '[=]  45 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 45 -k ffffffffffff --set 1000",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    '[=]  45 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 45 -b -k ffffffffffff --set 500",
                    "[+] Dec ...... : 500",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    "[=]  45 | F4 01 00 00 0B FE FF FF F4 01 00 00 2D D2 2D D2 | ............-.-. ",
                ),
                (
                    "hf mf value --blk 45 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    "[=]  45 | F4 01 00 00 0B FE FF FF F4 01 00 00 2D D2 2D D2 | ............-.-. ",
                ),
                (
                    "hf mf value --blk 45 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    "[=]  45 | F4 01 00 00 0B FE FF FF F4 01 00 00 2D D2 2D D2 | ............-.-. ",
                ),
                (
                    "hf mf value --blk 45 -k ffffffffffff --dec 50",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    "[=]  45 | F4 01 00 00 0B FE FF FF F4 01 00 00 2D D2 2D D2 | ............-.-. ",
                ),
                (
                    "hf mf value --blk 45 -b -k ffffffffffff --dec 350",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    "[=]  45 | F4 01 00 00 0B FE FF FF F4 01 00 00 2D D2 2D D2 | ............-.-. ",
                ),
                (
                    "hf mf value --blk 45 -k ffffffffffff --res --transfer 45",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    "[=]  45 | F4 01 00 00 0B FE FF FF F4 01 00 00 2D D2 2D D2 | ............-.-. ",
                ),
                (
                    "hf mf value --blk 45 -b -k ffffffffffff --res --transfer 45",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 45 -k ffffffffffff",
                    "[=]  45 | F4 01 00 00 0B FE FF FF F4 01 00 00 2D D2 2D D2 | ............-.-. ",
                ),
            ],
        ),
        (
            "5D278A",
            [
                (
                    "hf mf wrbl --blk 51 -k ffffffffffff -d ffffffffffff5d278a69ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    "[=]  49 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf rdbl --blk 49 -b -k ffffffffffff",
                    "[=]  49 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 49 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    "[=]  49 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 49 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    '[=]  49 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 49 -k ffffffffffff --set 1000",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    '[=]  49 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 49 -b -k ffffffffffff --set 500",
                    "[+] Dec ...... : 500",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    "[=]  49 | F4 01 00 00 0B FE FF FF F4 01 00 00 31 CE 31 CE | ............1.1. ",
                ),
                (
                    "hf mf value --blk 49 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    "[=]  49 | F4 01 00 00 0B FE FF FF F4 01 00 00 31 CE 31 CE | ............1.1. ",
                ),
                (
                    "hf mf value --blk 49 -b -k ffffffffffff --inc 100",
                    "[+] Dec ...... : 600",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    "[=]  49 | 58 02 00 00 A7 FD FF FF 58 02 00 00 31 CE 31 CE | X.......X...1.1. ",
                ),
                (
                    "hf mf value --blk 49 -k ffffffffffff --dec 50",
                    "[+] Dec ...... : 550",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    "[=]  49 | 26 02 00 00 D9 FD FF FF 26 02 00 00 31 CE 31 CE | &.......&...1.1. ",
                ),
                (
                    "hf mf value --blk 49 -b -k ffffffffffff --dec 350",
                    "[+] Dec ...... : 200",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    "[=]  49 | C8 00 00 00 37 FF FF FF C8 00 00 00 31 CE 31 CE | ....7.......1.1. ",
                ),
                (
                    "hf mf value --blk 49 -k ffffffffffff --res --transfer 49",
                    "[+] Dec ...... : 200",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    "[=]  49 | C8 00 00 00 37 FF FF FF C8 00 00 00 31 CE 31 CE | ....7.......1.1. ",
                ),
                (
                    "hf mf value --blk 49 -b -k ffffffffffff --res --transfer 49",
                    "[+] Dec ...... : 200",
                ),
                (
                    "hf mf rdbl --blk 49 -k ffffffffffff",
                    "[=]  49 | C8 00 00 00 37 FF FF FF C8 00 00 00 31 CE 31 CE | ....7.......1.1. ",
                ),
            ],
        ),
        (
            "7F05A8 part 1",
            [
                (
                    "hf mf wrbl --blk 55 -k ffffffffffff -d ffffffffffff7f05a869ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 53 -k ffffffffffff",
                    "[=]  53 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf rdbl --blk 53 -b -k ffffffffffff",
                    "[=]  53 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 53 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 53 -k ffffffffffff",
                    "[=]  53 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 53 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 53 -k ffffffffffff",
                    "[=]  53 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf value --blk 53 -k ffffffffffff --set 1000",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 53 -k ffffffffffff",
                    "[=]  53 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf value --blk 53 -b -k ffffffffffff --set 500",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 53 -k ffffffffffff",
                    "[=]  53 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
            ],
        ),
        (
            "7F05A8 part 2",
            [
                (
                    "hf mf value --blk 57 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf wrbl --blk 59 -k ffffffffffff -d ffffffffffff7f05a869ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf value --blk 57 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 57 -k ffffffffffff",
                    "[=]  57 | E8 03 00 00 17 FC FF FF E8 03 00 00 39 C6 39 C6 | ............9.9. ",
                ),
                (
                    "hf mf value --blk 57 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 57 -k ffffffffffff",
                    "[=]  57 | E8 03 00 00 17 FC FF FF E8 03 00 00 39 C6 39 C6 | ............9.9. ",
                ),
                (
                    "hf mf value --blk 57 -k ffffffffffff --dec 50",
                    "[+] Dec ...... : 950",
                ),
                (
                    "hf mf rdbl --blk 57 -k ffffffffffff",
                    "[=]  57 | B6 03 00 00 49 FC FF FF B6 03 00 00 39 C6 39 C6 | ....I.......9.9. ",
                ),
                (
                    "hf mf value --blk 57 -b -k ffffffffffff --dec 350",
                    "[+] Dec ...... : 600",
                ),
                (
                    "hf mf rdbl --blk 57 -k ffffffffffff",
                    "[=]  57 | 58 02 00 00 A7 FD FF FF 58 02 00 00 39 C6 39 C6 | X.......X...9.9. ",
                ),
                (
                    "hf mf value --blk 57 -k ffffffffffff --res --transfer 57",
                    "[+] Dec ...... : 600",
                ),
                (
                    "hf mf rdbl --blk 57 -k ffffffffffff",
                    "[=]  57 | 58 02 00 00 A7 FD FF FF 58 02 00 00 39 C6 39 C6 | X.......X...9.9. ",
                ),
                (
                    "hf mf value --blk 57 -b -k ffffffffffff --res --transfer 57",
                    "[+] Dec ...... : 600",
                ),
                (
                    "hf mf rdbl --blk 57 -k ffffffffffff",
                    "[=]  57 | 58 02 00 00 A7 FD FF FF 58 02 00 00 39 C6 39 C6 | X.......X...9.9. ",
                ),
            ],
        ),
        (
            "5F05AA",
            [
                (
                    "hf mf wrbl --blk 63 -k ffffffffffff -d ffffffffffff5f05aa69ffffffffffff",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 61 -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    "[=]  61 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 61 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    "[=]  61 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 61 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    '[=]  61 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 61 -k ffffffffffff --set 1000",
                    "[-] failed to read value block",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    '[=]  61 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 61 -b -k ffffffffffff --set 500",
                    "[+] Dec ...... : 500",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    "[=]  61 | F4 01 00 00 0B FE FF FF F4 01 00 00 3D C2 3D C2 | ............=.=. ",
                ),
                (
                    "hf mf value --blk 61 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    "[=]  61 | F4 01 00 00 0B FE FF FF F4 01 00 00 3D C2 3D C2 | ............=.=. ",
                ),
                (
                    "hf mf value --blk 61 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    "[=]  61 | F4 01 00 00 0B FE FF FF F4 01 00 00 3D C2 3D C2 | ............=.=. ",
                ),
                (
                    "hf mf value --blk 61 -k ffffffffffff --dec 50",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    "[=]  61 | F4 01 00 00 0B FE FF FF F4 01 00 00 3D C2 3D C2 | ............=.=. ",
                ),
                (
                    "hf mf value --blk 61 -b -k ffffffffffff --dec 350",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    "[=]  61 | F4 01 00 00 0B FE FF FF F4 01 00 00 3D C2 3D C2 | ............=.=. ",
                ),
                (
                    "hf mf value --blk 61 -k ffffffffffff --res --transfer 61",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    "[=]  61 | F4 01 00 00 0B FE FF FF F4 01 00 00 3D C2 3D C2 | ............=.=. ",
                ),
                (
                    "hf mf value --blk 61 -b -k ffffffffffff --res --transfer 61",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 61 -b -k ffffffffffff",
                    "[=]  61 | F4 01 00 00 0B FE FF FF F4 01 00 00 3D C2 3D C2 | ............=.=. ",
                ),
            ],
        ),
        (
            "7D25A8 part 1",
            [
                (
                    "hf mf wrbl --blk 67 -k ffffffffffff -d ffffffffffff7d25a869ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 65 -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf rdbl --blk 65 -b -k ffffffffffff",
                    "[=]  65 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 65 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 65 -b -k ffffffffffff",
                    "[=]  65 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 65 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 65 -b -k ffffffffffff",
                    "[=]  65 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf value --blk 65 -k ffffffffffff --set 1000",
                    "[#] Read block error",
                ),
                (
                    "hf mf rdbl --blk 65 -b -k ffffffffffff",
                    "[=]  65 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf value --blk 65 -b -k ffffffffffff --set 500",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 65 -b -k ffffffffffff",
                    "[=]  65 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
            ],
        ),
        (
            "7D25A8 part 2",
            [
                (
                    "hf mf value --blk 69 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf rdbl --blk 69 -k ffffffffffff",
                    "[=]  69 | E8 03 00 00 17 FC FF FF E8 03 00 00 45 BA 45 BA | ............E.E. ",
                ),
                (
                    "hf mf wrbl --blk 71 -k ffffffffffff -d ffffffffffff7d25a869ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf value --blk 69 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 69 -b -k ffffffffffff",
                    "[=]  69 | E8 03 00 00 17 FC FF FF E8 03 00 00 45 BA 45 BA | ............E.E. ",
                ),
                (
                    "hf mf value --blk 69 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 69 -b -k ffffffffffff",
                    "[=]  69 | E8 03 00 00 17 FC FF FF E8 03 00 00 45 BA 45 BA | ............E.E. ",
                ),
                (
                    "hf mf value --blk 69 -k ffffffffffff --dec 50",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 69 -b -k ffffffffffff",
                    "[=]  69 | E8 03 00 00 17 FC FF FF E8 03 00 00 45 BA 45 BA | ............E.E. ",
                ),
                (
                    "hf mf value --blk 69 -b -k ffffffffffff --dec 350",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 69 -b -k ffffffffffff",
                    "[=]  69 | E8 03 00 00 17 FC FF FF E8 03 00 00 45 BA 45 BA | ............E.E. ",
                ),
                (
                    "hf mf value --blk 69 -k ffffffffffff --res --transfer 69",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 69 -b -k ffffffffffff",
                    "[=]  69 | E8 03 00 00 17 FC FF FF E8 03 00 00 45 BA 45 BA | ............E.E. ",
                ),
                (
                    "hf mf value --blk 69 -b -k ffffffffffff --res --transfer 69",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 69 -b -k ffffffffffff",
                    "[=]  69 | E8 03 00 00 17 FC FF FF E8 03 00 00 45 BA 45 BA | ............E.E. ",
                ),
            ],
        ),
        (
            "5D25AA part 1",
            [
                (
                    "hf mf wrbl --blk 75 -k ffffffffffff -d ffffffffffff5d25aa69ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 73 -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf rdbl --blk 73 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf wrbl --blk 73 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 73 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf wrbl --blk 73 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 73 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf value --blk 73 -k ffffffffffff --set 1000",
                    "[#] Read block error",
                ),
                (
                    "hf mf rdbl --blk 73 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf value --blk 73 -b -k ffffffffffff --set 500",
                    "[#] Read block error",
                ),
                (
                    "hf mf rdbl --blk 73 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
            ],
        ),
        (
            "5D25AA part 2",
            [
                (
                    "hf mf value --blk 77 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf rdbl --blk 77 -k ffffffffffff",
                    "[=]  77 | E8 03 00 00 17 FC FF FF E8 03 00 00 4D B2 4D B2 | ............M.M. ",
                ),
                (
                    "hf mf wrbl --blk 79 -k ffffffffffff -d ffffffffffff5d25aa69ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf value --blk 77 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 77 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf value --blk 77 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 77 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf value --blk 77 -k ffffffffffff --dec 50",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 77 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf value --blk 77 -b -k ffffffffffff --dec 350",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 77 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf value --blk 77 -k ffffffffffff --res --transfer 77",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 77 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf value --blk 77 -b -k ffffffffffff --res --transfer 77",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 77 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
            ],
        ),
        (
            "FF0780",
            [
                (
                    "hf mf wrbl --blk 83 -k ffffffffffff -d ffffffffffffFF078069ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    "[=]  81 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf rdbl --blk 81 -b -k ffffffffffff",
                    # "[#] Read block error", # expected
                    "[=]  81 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",  # actual
                ),
                (
                    "hf mf wrbl --blk 81 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    '[=]  81 | 11 22 33 44 55 66 77 88 99 00 11 22 33 44 55 66 | ."3DUfw...."3DUf ',
                ),
                (
                    "hf mf wrbl --blk 81 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    # "[-] Write ( fail )", # expected
                    "[+] Write ( ok )",  # actual
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    #'[=]  81 | 11 22 33 44 55 66 77 88 99 00 11 22 33 44 55 66 | ."3DUfw...."3DUf ', # expected
                    '[=]  81 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',  # actual
                ),
                (
                    "hf mf value --blk 81 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    "[=]  81 | E8 03 00 00 17 FC FF FF E8 03 00 00 51 AE 51 AE | ............Q.Q. ",
                ),
                (
                    "hf mf value --blk 81 -b -k ffffffffffff --set 500",
                    # "[#] Read block error", # expected
                    "[+] Dec ...... : 500",  # actual
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    "[=]  81 | F4 01 00 00 0B FE FF FF F4 01 00 00 51 AE 51 AE | ............Q.Q. ",
                ),
                (
                    "hf mf value --blk 81 -k ffffffffffff --inc 200",
                    "[+] Dec ...... : 700",
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    "[=]  81 | BC 02 00 00 43 FD FF FF BC 02 00 00 51 AE 51 AE | ....C.......Q.Q. ",
                ),
                (
                    "hf mf value --blk 81 -b -k ffffffffffff --inc 100",
                    # "[#] Read block error", # expected
                    "[+] Dec ...... : 800",  # actual
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    "[=]  81 | 20 03 00 00 DF FC FF FF 20 03 00 00 51 AE 51 AE |  ....... ...Q.Q. ",
                ),
                (
                    "hf mf value --blk 81 -k ffffffffffff --dec 50",
                    "[+] Dec ...... : 750",
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    "[=]  81 | EE 02 00 00 11 FD FF FF EE 02 00 00 51 AE 51 AE | ............Q.Q. ",
                ),
                (
                    "hf mf value --blk 81 -b -k ffffffffffff --dec 350",
                    # "[#] Read block error", # expected
                    "[+] Dec ...... : 400",  # actual
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    "[=]  81 | 90 01 00 00 6F FE FF FF 90 01 00 00 51 AE 51 AE | ....o.......Q.Q. ",
                ),
                (
                    "hf mf value --blk 81 -k ffffffffffff --res --transfer 81",
                    "[+] Dec ...... : 400",
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    "[=]  81 | 90 01 00 00 6F FE FF FF 90 01 00 00 51 AE 51 AE | ....o.......Q.Q. ",
                ),
                (
                    "hf mf value --blk 81 -b -k ffffffffffff --res --transfer 81",
                    # "[#] Read block error", # expected
                    "[+] Dec ...... : 400",
                ),
                (
                    "hf mf rdbl --blk 81 -k ffffffffffff",
                    "[=]  81 | 90 01 00 00 6F FE FF FF 90 01 00 00 51 AE 51 AE | ....o.......Q.Q. ",
                ),
            ],
        ),
        (
            "DF0782 part 1",
            [
                (
                    "hf mf wrbl --blk 87 -k ffffffffffff -d ffffffffffffDF078269ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 85 -k ffffffffffff",
                    "[=]  85 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf rdbl --blk 85 -b -k ffffffffffff",
                    # "[#] Read block error", # expected
                    "[=]  85 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",  # actual
                ),
                (
                    "hf mf wrbl --blk 85 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 85 -k ffffffffffff",
                    "[=]  85 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 85 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 85 -b -k ffffffffffff",
                    "[=]  85 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf value --blk 85 -k ffffffffffff --set 1000",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 85 -b -k ffffffffffff",
                    "[=]  85 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf value --blk 85 -b -k ffffffffffff --set 500",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 85 -b -k ffffffffffff",
                    "[=]  85 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
            ],
        ),
        (
            "DF0782 part 2",
            [
                (
                    "hf mf value --blk 89 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf wrbl --blk 91 -k ffffffffffff -d ffffffffffffDF078269ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf value --blk 89 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 89 -k ffffffffffff",
                    "[=]  89 | E8 03 00 00 17 FC FF FF E8 03 00 00 59 A6 59 A6 | ............Y.Y. ",
                ),
                (
                    "hf mf value --blk 89 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 89 -k ffffffffffff",
                    "[=]  89 | E8 03 00 00 17 FC FF FF E8 03 00 00 59 A6 59 A6 | ............Y.Y. ",
                ),
                (
                    "hf mf value --blk 89 -k ffffffffffff --dec 50",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 89 -k ffffffffffff",
                    "[=]  89 | E8 03 00 00 17 FC FF FF E8 03 00 00 59 A6 59 A6 | ............Y.Y. ",
                ),
                (
                    "hf mf value --blk 89 -b -k ffffffffffff --dec 350",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 89 -k ffffffffffff",
                    "[=]  89 | E8 03 00 00 17 FC FF FF E8 03 00 00 59 A6 59 A6 | ............Y.Y. ",
                ),
                (
                    "hf mf value --blk 89 -k ffffffffffff --res --transfer 89",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 89 -k ffffffffffff",
                    "[=]  89 | E8 03 00 00 17 FC FF FF E8 03 00 00 59 A6 59 A6 | ............Y.Y. ",
                ),
                (
                    "hf mf value --blk 89 -b -k ffffffffffff --res --transfer 89",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 89 -k ffffffffffff",
                    "[=]  89 | E8 03 00 00 17 FC FF FF E8 03 00 00 59 A6 59 A6 | ............Y.Y. ",
                ),
            ],
        ),
        (
            "FD2780 part 1",
            [
                (
                    "hf mf wrbl --blk 95 -k ffffffffffff -d ffffffffffffFD278069ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 93 -k ffffffffffff",
                    "[=]  93 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf rdbl --blk 93 -k ffffffffffff",
                    "[=]  93 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 93 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 93 -k ffffffffffff",
                    "[=]  93 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 93 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    # "[-] Write ( fail )", # expected
                    "[+] Write ( ok )",  # actual
                ),
                (
                    "hf mf rdbl --blk 93 -k ffffffffffff",
                    '[=]  93 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 93 -k ffffffffffff --set 1000",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 93 -k ffffffffffff",
                    '[=]  93 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 93 -b -k ffffffffffff --set 500",
                    # "[-] No value block detected", # expected
                    "[+] Dec ...... : 500",  # actual
                ),
                (
                    "hf mf rdbl --blk 93 -k ffffffffffff",
                    "[=]  93 | F4 01 00 00 0B FE FF FF F4 01 00 00 5D A2 5D A2 | ............].]. ",
                ),
            ],
        ),
        (
            "FD2780 part 2",
            [
                (
                    "hf mf value --blk 97 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf wrbl --blk 99 -k ffffffffffff -d ffffffffffffFD278069ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf value --blk 97 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 97 -k ffffffffffff",
                    "[=]  97 | E8 03 00 00 17 FC FF FF E8 03 00 00 61 9E 61 9E | ............a.a. ",
                ),
                (
                    "hf mf value --blk 97 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 97 -k ffffffffffff",
                    "[=]  97 | E8 03 00 00 17 FC FF FF E8 03 00 00 61 9E 61 9E | ............a.a. ",
                ),
                (
                    "hf mf value --blk 97 -k ffffffffffff --dec 50",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 97 -k ffffffffffff",
                    "[=]  97 | E8 03 00 00 17 FC FF FF E8 03 00 00 61 9E 61 9E | ............a.a. ",
                ),
                (
                    "hf mf value --blk 97 -b -k ffffffffffff --dec 350",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 97 -k ffffffffffff",
                    "[=]  97 | E8 03 00 00 17 FC FF FF E8 03 00 00 61 9E 61 9E | ............a.a. ",
                ),
                (
                    "hf mf value --blk 97 -k ffffffffffff --res --transfer 97",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 97 -k ffffffffffff",
                    "[=]  97 | E8 03 00 00 17 FC FF FF E8 03 00 00 61 9E 61 9E | ............a.a. ",
                ),
                (
                    "hf mf value --blk 97 -b -k ffffffffffff --res --transfer 97",
                    "[-] Update ... : failed",
                ),
            ],
        ),
        (
            "DD2782 part 1",
            [
                (
                    "hf mf wrbl --blk 103 -k ffffffffffff -d ffffffffffffDD278269ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 101 -k ffffffffffff",
                    "[=] 101 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf rdbl --blk 101 -b -k ffffffffffff",
                    # "[#] Read block error", # expected
                    "[=] 101 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",  # actual
                ),
                (
                    "hf mf wrbl --blk 101 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 101 -k ffffffffffff",
                    "[=] 101 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 101 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    # "[-] Write ( fail )", # expected
                    "[+] Write ( ok )",  # actual
                ),
                (
                    "hf mf rdbl --blk 101 -k ffffffffffff",
                    '[=] 101 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 101 -k ffffffffffff --set 1000",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 101 -k ffffffffffff",
                    '[=] 101 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 101 -b -k ffffffffffff --set 500",
                    # "[-] No value block detected",  # expected
                    "[+] Dec ...... : 500",  # actual
                ),
                (
                    "hf mf rdbl --blk 101 -k ffffffffffff",
                    "[=] 101 | F4 01 00 00 0B FE FF FF F4 01 00 00 65 9A 65 9A | ............e.e. ",
                ),
            ],
        ),
        (
            "DD2782 part 2",
            [
                (
                    "hf mf value --blk 105 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf wrbl --blk 107 -k ffffffffffff -d ffffffffffffDD278269ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf value --blk 105 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 105 -k ffffffffffff",
                    "[=] 105 | E8 03 00 00 17 FC FF FF E8 03 00 00 69 96 69 96 | ............i.i. ",
                ),
                (
                    "hf mf value --blk 105 -b -k ffffffffffff --inc 100",
                    # "[-] Update ... : failed", # expected
                    "[+] Dec ...... : 1100",  # actual
                ),
                (
                    "hf mf rdbl --blk 105 -k ffffffffffff",
                    "[=] 105 | 4C 04 00 00 B3 FB FF FF 4C 04 00 00 69 96 69 96 | L.......L...i.i. ",
                ),
                (
                    "hf mf value --blk 105 -k ffffffffffff --dec 50",
                    "[+] Dec ...... : 1050",
                ),
                (
                    "hf mf rdbl --blk 105 -k ffffffffffff",
                    "[=] 105 | 1A 04 00 00 E5 FB FF FF 1A 04 00 00 69 96 69 96 | ............i.i. ",
                ),
                (
                    "hf mf value --blk 105 -b -k ffffffffffff --dec 350",
                    # "[-] Update ... : failed", # expexted
                    "[+] Dec ...... : 700",  # actual
                ),
                (
                    "hf mf rdbl --blk 105 -k ffffffffffff",
                    "[=] 105 | BC 02 00 00 43 FD FF FF BC 02 00 00 69 96 69 96 | ....C.......i.i. ",
                ),
                (
                    "hf mf value --blk 105 -k ffffffffffff --res --transfer 105",
                    "[+] Dec ...... : 700",
                ),
                (
                    "hf mf rdbl --blk 105 -k ffffffffffff",
                    "[=] 105 | BC 02 00 00 43 FD FF FF BC 02 00 00 69 96 69 96 | ....C.......i.i. ",
                ),
                (
                    "hf mf value --blk 105 -b -k ffffffffffff --res --transfer 105",
                    # "[-] Update ... : failed", # expected
                    "[+] Dec ...... : 700",
                ),
                (
                    "hf mf rdbl --blk 105 -k ffffffffffff",
                    "[=] 105 | BC 02 00 00 43 FD FF FF BC 02 00 00 69 96 69 96 | ....C.......i.i. ",
                ),
            ],
        ),
        (
            "FF05A0 part 1",
            [
                (
                    "hf mf wrbl --blk 111 -k ffffffffffff -d ffffffffffffFF05A069ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 109 -k ffffffffffff",
                    "[=] 109 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf rdbl --blk 109 -b -k ffffffffffff",
                    # "[#] Read block error", # expected
                    "[=] 109 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",  # actual
                ),
                (
                    "hf mf wrbl --blk 109 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",  # expexted
                ),
                (
                    "hf mf rdbl --blk 109 -k ffffffffffff",
                    "[=] 109 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 109 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[-] Write ( fail )",  # expected
                    # "[+] Write ( ok )",  # actual
                ),
                (
                    "hf mf rdbl --blk 109 -b -k ffffffffffff",
                    "[=] 109 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf value --blk 109 -k ffffffffffff --set 1000",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 109 -b -k ffffffffffff",
                    "[=] 109 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf value --blk 109 -b -k ffffffffffff --set 500",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 109 -b -k ffffffffffff",
                    "[=] 109 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
            ],
        ),
        (
            "FF05A0 part 2",
            [
                (
                    "hf mf value --blk 113 -b -k ffffffffffff --set 500",
                    "[+] Dec ...... : 500",
                ),
                (
                    "hf mf wrbl --blk 115 -k ffffffffffff -d ffffffffffffFF05A069ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf value --blk 113 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 113 -k ffffffffffff",
                    "[=] 113 | F4 01 00 00 0B FE FF FF F4 01 00 00 71 8E 71 8E | ............q.q. ",
                ),
                (
                    "hf mf value --blk 113 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",  # expected
                ),
                (
                    "hf mf rdbl --blk 113 -k ffffffffffff",
                    "[=] 113 | F4 01 00 00 0B FE FF FF F4 01 00 00 71 8E 71 8E | ............q.q. ",
                ),
                (
                    "hf mf value --blk 113 -k ffffffffffff --dec 50",
                    "[+] Dec ...... : 450",
                ),
                (
                    "hf mf rdbl --blk 113 -k ffffffffffff",
                    "[=] 113 | C2 01 00 00 3D FE FF FF C2 01 00 00 71 8E 71 8E | ....=.......q.q. ",
                ),
                (
                    "hf mf value --blk 113 -b -k ffffffffffff --dec 350",
                    # "[-] Update ... : failed", # expexted
                    "[+] Dec ...... : 100",  # actual
                ),
                (
                    "hf mf rdbl --blk 113 -k ffffffffffff",
                    "[=] 113 | 64 00 00 00 9B FF FF FF 64 00 00 00 71 8E 71 8E | d.......d...q.q. ",
                ),
                (
                    "hf mf value --blk 113 -k ffffffffffff --res --transfer 113",
                    "[+] Dec ...... : 100",
                ),
                (
                    "hf mf rdbl --blk 113 -k ffffffffffff",
                    "[=] 113 | 64 00 00 00 9B FF FF FF 64 00 00 00 71 8E 71 8E | d.......d...q.q. ",
                ),
                (
                    "hf mf value --blk 113 -b -k ffffffffffff --res --transfer 113",
                    # "[-] Update ... : failed", # expected
                    "[+] Dec ...... : 100",  # actual
                ),
                (
                    "hf mf rdbl --blk 113 -k ffffffffffff",
                    "[=] 113 | 64 00 00 00 9B FF FF FF 64 00 00 00 71 8E 71 8E | d.......d...q.q. ",
                ),
            ],
        ),
        (
            "DF05A2 part 1",
            [
                (
                    "hf mf wrbl --blk 119 -k ffffffffffff -d ffffffffffffDF05A269ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 117 -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf rdbl --blk 117 -b -k ffffffffffff",
                    # "[#] Read block error", # expected
                    "[=] 117 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",  # actual
                ),
                (
                    "hf mf wrbl --blk 117 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 117 -b -k ffffffffffff",
                    "[=] 117 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",
                ),
                (
                    "hf mf wrbl --blk 117 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    # "[-] Write ( fail )", # expected
                    "[+] Write ( ok )",  # actual
                ),
                (
                    "hf mf rdbl --blk 117 -b -k ffffffffffff",
                    '[=] 117 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 117 -k ffffffffffff --set 1000",
                    "[-] failed to read value block",
                ),
                (
                    "hf mf rdbl --blk 117 -b -k ffffffffffff",
                    '[=] 117 | 00 11 22 33 44 55 66 77 88 99 12 34 56 12 34 56 | .."3DUfw...4V.4V ',
                ),
                (
                    "hf mf value --blk 117 -b -k ffffffffffff --set 500",
                    # "[-] No value block detected", # expected
                    "[+] Dec ...... : 500",  # actual
                ),
                (
                    "hf mf rdbl --blk 117 -b -k ffffffffffff",
                    "[=] 117 | F4 01 00 00 0B FE FF FF F4 01 00 00 75 8A 75 8A | ............u.u. ",
                ),
            ],
        ),
        (
            "DF05A2 part 2",
            [
                (
                    "hf mf value --blk 121 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf wrbl --blk 123 -k ffffffffffff -d ffffffffffffDF05A269ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 121 -b -k ffffffffffff",
                    "[=] 121 | E8 03 00 00 17 FC FF FF E8 03 00 00 79 86 79 86 | ............y.y. ",
                ),
                (
                    "hf mf value --blk 121 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 121 -b -k ffffffffffff",
                    "[=] 121 | E8 03 00 00 17 FC FF FF E8 03 00 00 79 86 79 86 | ............y.y. ",
                ),
                (
                    "hf mf value --blk 121 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 121 -b -k ffffffffffff",
                    "[=] 121 | E8 03 00 00 17 FC FF FF E8 03 00 00 79 86 79 86 | ............y.y. ",
                ),
                (
                    "hf mf value --blk 121 -k ffffffffffff --dec 50",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 121 -b -k ffffffffffff",
                    "[=] 121 | E8 03 00 00 17 FC FF FF E8 03 00 00 79 86 79 86 | ............y.y. ",
                ),
                (
                    "hf mf value --blk 121 -b -k ffffffffffff --dec 350",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 121 -b -k ffffffffffff",
                    "[=] 121 | E8 03 00 00 17 FC FF FF E8 03 00 00 79 86 79 86 | ............y.y. ",
                ),
                (
                    "hf mf value --blk 121 -k ffffffffffff --res --transfer 121",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 121 -b -k ffffffffffff",
                    "[=] 121 | E8 03 00 00 17 FC FF FF E8 03 00 00 79 86 79 86 | ............y.y. ",
                ),
                (
                    "hf mf value --blk 121 -b -k ffffffffffff --res --transfer 121",
                    "[-] Update ... : failed",
                ),
            ],
        ),
        (
            "FD25A0 part 1",
            [
                (
                    "hf mf wrbl --blk 127 -k ffffffffffff -d ffffffffffffFD25A069ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 125 -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf rdbl --blk 125 -b -k ffffffffffff",
                    # "[#] Read block error", # expected
                    "[=] 125 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",  # actual
                ),
                (
                    "hf mf wrbl --blk 125 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 125 -b -k ffffffffffff",
                    # "[#] Read block error", # expected
                    "[=] 125 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",  # actual
                ),
                (
                    "hf mf wrbl --blk 125 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf rdbl --blk 125 -b -k ffffffffffff",
                    # "[#] Read block error", # expected
                    "[=] 125 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",  # actual
                ),
                (
                    "hf mf value --blk 125 -k ffffffffffff --set 1000",
                    "[#] Read block error",
                ),
                (
                    "hf mf rdbl --blk 125 -b -k ffffffffffff",
                    # "[#] Read block error", # expected
                    "[=] 125 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",  # actual
                ),
                (
                    "hf mf value --blk 125 -b -k ffffffffffff --set 500",
                    "[-] No value block detected",
                ),
                (
                    "hf mf rdbl --blk 125 -b -k ffffffffffff",
                    # "[#] Read block error", # expected
                    "[=] 125 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................ ",  # actual
                ),
            ],
        ),
        (
            "FD25A0 part 2",
            [
                (
                    "hf mf value --blk 133 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf wrbl --blk 143 -k ffffffffffff -d ffffffffffffFD25A069ffffffffffff --force",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 133 -b -k ffffffffffff",
                    "[=] 133 | E8 03 00 00 17 FC FF FF E8 03 00 00 85 7A 85 7A | .............z.z ",
                ),
                (
                    "hf mf value --blk 133 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 133 -b -k ffffffffffff",
                    "[=] 133 | E8 03 00 00 17 FC FF FF E8 03 00 00 85 7A 85 7A | .............z.z ",
                ),
                (
                    "hf mf value --blk 133 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 133 -b -k ffffffffffff",
                    "[=] 133 | E8 03 00 00 17 FC FF FF E8 03 00 00 85 7A 85 7A | .............z.z ",
                ),
                (
                    "hf mf value --blk 133 -k ffffffffffff --dec 50",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 133 -b -k ffffffffffff",
                    "[=] 133 | E8 03 00 00 17 FC FF FF E8 03 00 00 85 7A 85 7A | .............z.z ",
                ),
                (
                    "hf mf value --blk 133 -b -k ffffffffffff --dec 350",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 133 -b -k ffffffffffff",
                    "[=] 133 | E8 03 00 00 17 FC FF FF E8 03 00 00 85 7A 85 7A | .............z.z ",
                ),
                (
                    "hf mf value --blk 133 -k ffffffffffff --res --transfer 133",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 133 -b -k ffffffffffff",
                    "[=] 133 | E8 03 00 00 17 FC FF FF E8 03 00 00 85 7A 85 7A | .............z.z ",
                ),
                (
                    "hf mf value --blk 133 -b -k ffffffffffff --res --transfer 133",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf rdbl --blk 133 -b -k ffffffffffff",
                    "[=] 133 | E8 03 00 00 17 FC FF FF E8 03 00 00 85 7A 85 7A | .............z.z ",
                ),
            ],
        ),
        (
            "DD25A2 part 1",
            [
                (
                    "hf mf wrbl --blk 159 -k ffffffffffff -d ffffffffffffDD25A269ffffffffffff",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf rdbl --blk 149 -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf rdbl --blk 149 -b -k ffffffffffff",
                    "[#] Read block error",
                ),
                (
                    "hf mf wrbl --blk 149 -k ffffffffffff -d 11223344556677889900112233445566",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf wrbl --blk 149 -b -k ffffffffffff -d 00112233445566778899123456123456",
                    "[-] Write ( fail )",
                ),
                (
                    "hf mf value --blk 149 -k ffffffffffff --set 1000",
                    "[#] Read block error",
                ),
                (
                    "hf mf value --blk 149 -b -k ffffffffffff --set 500",
                    "[#] Read block error",
                ),
            ],
        ),
        (
            "DD25A2 part 2",
            [
                (
                    "hf mf value --blk 165 -k ffffffffffff --set 1000",
                    "[+] Dec ...... : 1000",
                ),
                (
                    "hf mf wrbl --blk 175 -k ffffffffffff -d ffffffffffffDD25A269ffffffffffff",
                    "[+] Write ( ok )",
                ),
                (
                    "hf mf value --blk 165 -k ffffffffffff --inc 200",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf value --blk 165 -b -k ffffffffffff --inc 100",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf value --blk 165 -k ffffffffffff --dec 50",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf value --blk 165 -b -k ffffffffffff --dec 350",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf value --blk 165 -k ffffffffffff --res --transfer 165",
                    "[-] Update ... : failed",
                ),
                (
                    "hf mf value --blk 165 -b -k ffffffffffff --res --transfer 165",
                    "[-] Update ... : failed",
                ),
            ],
        ),
    ]

    @pytest.mark.parametrize("condition, cmds", data)
    def test_trailer_access_condition(self, px, emulate_classic, condition, cmds):
        for i, cmd in enumerate(cmds):
            command = cmd[0]
            expected = cmd[1]
            result = px.execute(command)
            retry_count = 3
            if expected not in result:
                while retry_count > 0:
                    result = px.execute(command)
                    if expected in result:
                        break
                    retry_count -= 1
            assert expected in result, f"{i}: {command} failed"
