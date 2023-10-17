import pytest
from flippigator.extensions.proxmark_wrapper import proxmark_wrapper
from flippigator.flippigator import FlipperHEXKeyboard, FlipperTextKeyboard
import time


class TestEmulateClassic:

    @pytest.fixture(scope="class", autouse=True)
    def add_4k7_manually(self, nav):
        # Code below will run before each "function" (test)

        # Delete file if exists, needs to be added as picture sadly
        # nav.delete_file("NFC", "T_mfc_4k7")
        # Todo: return back to line above, after browser is fixed
        nav.nfc.go_into()
        nav.go_to("Saved")
        nav.press_ok()
        menu_items = nav.get_menu_list()
        if "browser_T_mfc_4k7" in menu_items:
            nav.go_to("browser_T_mfc_4k7", direction="up")
            nav.press_ok()
            nav.go_to("Delete", direction="up")
            nav.press_ok()
            nav.press_right()

        nav.nfc.go_into()
        nav.go_to("Add Manually")
        nav.press_ok()
        # direction for faster testing, since its last element
        nav.go_to("Mifare Classic 4k 7byte UID", direction="up")
        nav.press_ok()
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
        nav.go_to("Saved")
        nav.press_ok()
        menu_items = nav.get_menu_list()
        if "browser_T_mfc_4k7" in menu_items:
            nav.go_to("browser_T_mfc_4k7", direction="up")
            nav.press_ok()
            nav.go_to("Delete", direction="up")
            nav.press_ok()
            nav.press_right()

    @pytest.fixture(scope="function")
    def emulate_classic(self, nav):
        nav.go_to_main_screen()
        if nav.open_file("NFC", "T_mfc_4k7") == -1:
            assert 0, "File not found"

        # Need for emulation to start, otherwise error on proxmark side
        time.sleep(1)

    def test_mfc_4k7_basic(self, px, emulate_classic):
        """
        px: proxmark_wrapper fixture
        command: command to be executed
        expected_result: expected result of the command
        error: if the command should return an error
        """

        # read 1 block
        read1 = px.execute("hf mf rdbl --blk 1 -k ffffffffffff")
        expected = "[=]   1 | FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF | ................ "
        assert expected in read1, "Incorrect block read"

        # write 1 block
        write1 = px.execute("hf mf wrbl --blk 1 -k ffffffffffff -d 11223344556677889910111213141516")
        expected = "[+] Write ( ok )"
        assert expected in write1, "Incorrect block write"

        # read 1 block
        read1_wrong = px.execute("hf mf rdbl --blk 1 -k ffffffffffff")
        expected = "[=]   1 | 11 22 33 44 55 66 77 88 99 10 11 12 13 14 15 16 | .\"3DUfw......... "
        assert expected in read1_wrong, "Incorrect block read"

        # set value block
        set_value = px.execute("hf mf value --blk 1 -k ffffffffffff --set 1000")
        expected = "[=] set value to : 1000"
        expected_status = "[+] Update ... : success"
        assert expected in set_value, "Incorrect set value"
        assert expected_status in set_value, "Update failed"

        # increment value block
        inc_value = px.execute("hf mf value --blk 1 -k ffffffffffff --inc 200")
        expected = "[=] Value incremented by : 200"
        expected_status = "[+] Update ... : success"

        assert expected in inc_value, "Incorrect increment value"
        assert expected_status in inc_value, "Update failed"

        # decrement value block
        dec_value = px.execute("hf mf value --blk 1 -k ffffffffffff --dec 10000")
        expected = "[=] Value decremented by : 10000"
        expected_status = "[+] Update ... : success"
        assert expected in dec_value, "Incorrect decrement value"
        assert expected_status in dec_value, "Update failed"

        # change key A in 0 sector
        change_key = px.execute("hf mf wrbl --blk 3 -k ffffffffffff -d a0a1a2a3a4a5ff078069ffffffffffff")
        expected = "[=] data: A0 A1 A2 A3 A4 A5 FF 07 80 69 FF FF FF FF FF FF"
        expected_status = "[+] Write ( ok )"
        assert expected in change_key, "Incorrect change key"
        assert expected_status in change_key, "Update failed"

        # read 1 block with old key
        read1 = px.execute("hf mf rdbl --blk 1 -k ffffffffffff")
        expected_status = "[#] Auth error"
        assert expected_status in read1, "Auth error not present"

        # read 1 block with new key
        read1 = px.execute("hf mf rdbl --blk 1 -k a0a1a2a3a4a5")
        expected = "[=]   1 | E8 03 00 00 17 FC FF FF E8 03 00 00 01 FE 01 FE | ................ "
        assert expected in read1, "Incorrect block read"

        # all tests below are non-functional, need help verifying them
        # change key B in 0 sector
        # change_key = px.execute("hf mf wrbl --blk 3 -k ffffffffffff -d ffffffffffff078069b0b1b2b3b4b5ff")
        # assert change_key[0] == "Block write OK", "Incorrect change key"

        # read sector trailer with key A
        # trailer_a_key = px.execute("hf mf rdbl --blk 3 -k a0a1a2a3a4a5")
        # assert trailer_a_key[0] == "Key A not present, Access Bits present, Key B present in the output", "Incorrect trailer read"

        # read sector trailer with key B
        # trailer_b_key = px.execute("hf mf rdbl --blk 3 -b -k b0b1b2b3b4b5")
        # assert trailer_b_key[0] == "Key A not present, Access Bits not present, Key B not present in the output", "Incorrect trailer read"

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
                ("hf mf wrbl --blk 3 -k ffffffffffff -d ffffffffffffff0f0069ffffffffffff", "[+] Write ( ok )"),
                ("hf mf rdbl --blk 3 -k ffffffffffff", "[=]   3 | 00 00 00 00 00 00 FF 0F 00 69 FF FF FF FF FF FF | .........i......"),
                ("hf mf rdbl --blk 3 -b -k ffffffffffff", "[#] Read block error"), # [#] Read block error
                ("hf mf wrbl --blk 3 -k ffffffffffff -d a0a1a2a3a4a5ff0f0069b0b1b2b3b4b5", "[+] Write ( ok )"),
                ("hf mf wrbl --blk 3 -b -k b0b1b2b3b4b5 -d a1a2a3a4a5a6ff0f0069b1b2b3b4b5b6", "[-] ⛔ Write ( fail )"),
                ("hf mf wrbl --blk 3 -k a0a1a2a3a4a5 -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6", "[+] Write ( ok )"),
                ("hf mf wrbl --blk 3 -b -k b1b2b3b4b5b6 -d a1a2a3a4a5a6ff078069b1b2b3b4b5b6", "[-] ⛔ Write ( fail )"),
            ]
        ),
    ]

    @pytest.mark.parametrize("condition, cmds", data)
    def test_trailer_access_condition(self, px, emulate_classic, condition, cmds):
        result = px.execute(cmds[0][0])
        assert result[0] == cmds[0][1], "1: Write failed"

        result = px.execute(cmds[1][0])
        assert result[0] == cmds[1][1], "2: No keyA, ACs, KeyB"

        result = px.execute(cmds[2][0])
        assert result[0] == cmds[2][1], "3: No KeyA, No ACs, No KeyB"
        
        result = px.execute(cmds[3][0])
        assert result[0] == cmds[3][1], "4 Incorrect trailer read"
        
        result = px.execute(cmds[4][0])
        assert result[0] == cmds[4][1], "5 Incorrect trailer read"
        
        result = px.execute(cmds[5][0])
        assert result[0] == cmds[5][1], "Incorrect trailer read"

