import pytest
from flippigator.extensions.proxmark_wrapper import proxmark_wrapper
from flippigator.flippigator import FlipperHEXKeyboard, FlipperTextKeyboard
import time


class TestEmulateClassic:

    @pytest.fixture(scope="function", autouse=True)
    def add_4k7_manually(self, nav):
        # Code below will run before each "function" (test)
        nav.go_to_main_screen()

        # Delete file if exists, needs to be added as picture sadly
        nav.delete_file("NFC", "T_mfc_4k7")
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
        nav.go_to_main_screen()
        nav.delete_file("NFC", "T_mfc_4k7")

    @pytest.fixture(scope="function")
    def emulate_classic(self, nav):
        nav.go_to_main_screen()
        if nav.open_file("NFC", "T_mfc_4k7") == -1:
            assert 0, "File not found"

        # Need for emulation to start, otherwise error on proxmark side
        time.sleep(2)

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
        print("///////////////////////////////////")
        print(expected in read1)
        print("///////////////////////////////////")
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
        expected = "[+] Update ... : success"
        assert expected in set_value, "Incorrect set value"

        # increment value block
        inc_value = px.execute("hf mf value --blk 1 -k ffffffffffff --inc 200")
        expected = "[+] Update ... : success"

        # tests work up to this point
        assert expected in inc_value, "Incorrect increment value"

        # decrement value block
        dec_value = px.execute("hf mf value --blk 1 -k ffffffffffff --dec 10000")
        assert dec_value[0] == "Decrement success, new value is -8800", "Incorrect decrement value"

        # change key A in 0 sector
        change_key = px.execute("hf mf wrbl --blk 3 -k ffffffffffff -d a0a1a2a3a4a5ff078069ffffffffffff")
        assert change_key[0] == "Block write OK", "Incorrect change key"

        # read 1 block with old key
        read1 = px.execute("hf mf rdbl --blk 1 -k ffffffffffff")
        assert read1[0] == "Auth error", "Incorrect block read"

        # read 1 block with new key
        read1 = px.execute("hf mf rdbl --blk 1 -k a0a1a2a3a4a5")
        assert read1[0] == "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", "Incorrect block read"

        # change key B in 0 sector
        change_key = px.execute("hf mf wrbl --blk 3 -k ffffffffffff -d b0b1b2b3b4b5ff078069ffffffffffff")
        assert change_key[0] == "Block write OK", "Incorrect change key"

        # read sector trailer with key A
        trailer_a_key = px.execute("hf mf rdbl --blk 3 -k a0a1a2a3a4a5")
        assert trailer_a_key[0] == "Key A not present, Access Bits present, Key B present in the output", "Incorrect trailer read"

        # read sector trailer with key B
        trailer_b_key = px.execute("hf mf rdbl --blk 3 -b -k b0b1b2b3b4b5")
        assert trailer_b_key[0] == "Key A not present, Access Bits not present, Key B not present in the output", "Incorrect trailer read"



        px_result = px.execute(command)
        #
        # result = px_result[0]

        assert 1
