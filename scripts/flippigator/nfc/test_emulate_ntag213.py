import time

import pytest
from flippigator.extensions.proxmark_wrapper import proxmark_wrapper


class TestEmulateNtag213:
    @pytest.fixture(scope="class", autouse=False)
    def emulate_ntag213(self, nav):
        nav.go_to_main_screen()
        if nav.open_file("NFC", "T_Ntag_213") == -1:
            assert 0, "File not found"

    @pytest.fixture(scope="session", autouse=True)
    def reset_card_and_emulate(self, nav):
        nav.nfc.go_into()
        nav.open("Saved")
        nav.open("browser_T_Ntag_213", direction="up")
        menu_items = nav.get_menu_list()
        if "Restore Data Changes" in menu_items:
            nav.open("Restore Data Changes", direction="up")
            nav.press_right()
            time.sleep(1)

        nav.go_to_main_screen()
        if nav.open_file("NFC", "T_Ntag_213") == -1:
            assert 0, "File not found"

    @pytest.mark.skip(reason="not implemented")
    def test_ntag213_basic(self, px):
        """
        Todo: Rewrite proper example (not up to date)
        """
        pm = proxmark_wrapper("/home/vk/work/proxmark3/pm3")
        # pm.execute_single("hf 14a raw -sc 3000")

        result = pm.execute(
            ["hf 14a raw -sc 3000\n", "hf 14a raw -sc 3000\n", "hf 14a raw -sc 3001\n"]
        )
        print(result)
        print("-------------------------------------")

        assert (
            result == "04 63 5D B2 9A 5A 12 90 42 48 00 00 E1 10 6D 00 [ 65 9C ]"
        ), "Incorrect data on NFC card"

    test_data_byte_0 = [
        ("hf 14a raw -sc A229440024FF", "0A", "Error: NFC card not found"),
        (
            "hf 14a raw -sc 3024",
            "30 34 42 41 46 46 43 41 34 44 35 44 38 30 00 00 [ E9 19 ]",
            "Error: Incorrect data on NFC card",
        ),
        (
            "hf 14a raw -sc 3023",
            "00 00 00 00 30 34 42 41 46 46 43 41 34 44 35 44 [ 1C AC ]",
            "Error: Incorrect data on NFC card",
        ),
        (
            "hf 14a raw -sc 3025",
            "46 46 43 41 34 44 35 44 38 30 00 00 00 00 00 BD [ 10 8D ]",
            "Error: Incorrect data on NFC card",
        ),
    ]

    @pytest.mark.parametrize("command, expected_result, error", test_data_byte_0)
    def test_ascii_mirror_uid(self, px, command, expected_result, error):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result in result, error

    test_data_byte_1 = [
        ("hf 14a raw -sc A229440024FF", "0A", "Error: NFC card not found"),
        (
            "hf 14a raw -sc 3024",
            "00 30 34 42 41 46 46 43 41 34 44 35 44 38 30 00 [ 45 02 ]",
            "Error: Incorrect data on NFC card",
        ),
        (
            "hf 14a raw -sc 3023",
            "00 00 00 00 00 30 34 42 41 46 46 43 41 34 44 35 [ B1 7C ]",
            "Error: Incorrect data on NFC card",
        ),
        (
            "hf 14a raw -sc 3025",
            "41 46 46 43 41 34 44 35 44 38 30 00 00 00 00 BD [ 14 04 ]",
            "Error: Incorrect data on NFC card",
        ),
    ]

    @pytest.mark.parametrize("command, expected_result, error", test_data_byte_1)
    def test_ascii_mirror_uid_byte_1(self, px, command, expected_result, error):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result in result, error

    test_data_byte_2 = [
        ("hf 14a raw -sc A229540024FF", "0A", "Error: NFC card not found"),
        (
            "hf 14a raw -sc 3024",
            "00 00 30 34 42 41 46 46 43 41 34 44 35 44 38 30 [ EE 77 ]",
            "Error: Incorrect data on NFC card",
        ),
        (
            "hf 14a raw -sc 3023",
            "00 00 00 00 00 00 30 34 42 41 46 46 43 41 34 44 [ 76 6C ]",
            "Error: Incorrect data on NFC card",
        ),
        (
            "hf 14a raw -sc 3025",
            "42 41 46 46 43 41 34 44 35 44 38 30 00 00 00 BD [ 68 FA ]",
            "Error: Incorrect data on NFC card",
        ),
    ]

    @pytest.mark.parametrize("command, expected_result, error", test_data_byte_2)
    def test_ascii_mirror_uid_byte_2(self, px, command, expected_result, error):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result in result, error

    test_data_byte_3 = [
        ("hf 14a raw -sc A229540024FF", "0A", "Error: NFC card not found"),
        (
            "hf 14a raw -sc 3024",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 [ 37 49 ]",
            "Error: Incorrect data on NFC card",
        ),
        (
            "hf 14a raw -sc 3023",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 [ 37 49 ]",
            "Error: Incorrect data on NFC card",
        ),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 BD [ 59 27 ]",
            "Error: Incorrect data on NFC card",
        ),
    ]

    @pytest.mark.parametrize("command, expected_result, error", test_data_byte_3)
    def test_ascii_mirror_uid_byte_3(self, px, command, expected_result, error):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result in result, error

    test_data_ascii_counter_0 = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229840026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 BD [ 59 27 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 00 00 BD 84 00 26 FF [ DE 4C ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 00 00 BD 84 00 26 FF 00 05 00 00 [ DE 04 ]",
        ),
        ("hf 14a raw -sc A22A10050000", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 30 30 30 30 30 30 00 00 00 00 00 BD [ 76 C7 ]",
        ),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 30 30 30 30 30 31 00 00 00 00 00 BD [ A3 58 ]",
        ),
        (
            "hf 14a raw -sck 3025",
            "00 00 00 00 30 30 30 30 30 32 00 00 00 00 00 BD [ CD F0 ]",
        ),
        (
            "hf 14a raw -ac 3025",
            "00 00 00 00 30 30 30 30 30 33 00 00 00 00 00 BD [ 18 6F ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "30 30 30 30 30 33 00 00 00 00 00 BD 84 00 26 FF [ 66 86 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "30 34 00 00 00 00 00 BD 84 00 26 FF 10 05 00 00 [ 72 EA ]",
        ),
        ("hf 14a raw -sc A22A00050000", "0A"),
    ]

    test_data_ascii_counter_1 = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229840026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 BD [ 59 27 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 00 00 BD 94 00 26 FF [ 7F 8F ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 00 00 BD 94 00 26 FF 00 05 00 00 [ A6 5F ]",
        ),
        ("hf 14a raw -sc A22A10050000", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 30 30 30 30 30 35 00 00 00 00 BD [ 49 2A ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 30 30 30 30 30 36 00 00 00 00 BD 94 00 26 FF [ C8 C7 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "30 30 37 00 00 00 00 BD 94 00 26 FF 10 05 00 00 [ 5C 9A ]",
        ),
        ("hf 14a raw -sc A22A00050000", "0A"),
    ]

    test_data_ascii_counter_2 = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229A40026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 BD [ 59 27 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 00 00 BD A4 00 26 FF [ 8D C3 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 00 00 BD A4 00 26 FF 00 05 00 00 [ 2E B2 ]",
        ),
        ("hf 14a raw -sc A22A10050000", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 30 30 30 30 30 38 00 00 00 BD [ F0 23 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 30 30 30 30 30 39 00 00 00 BD A4 00 26 FF [ 4F B6 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "30 30 30 41 00 00 00 BD A4 00 26 FF 10 05 00 00 [ AF 61 ]",
        ),
        ("hf 14a raw -sc A22A00050000", "0A"),
    ]

    test_data_ascii_counter_3 = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229B40026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 BD [ 59 27 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 00 00 BD B4 00 26 FF [ 2C 00 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 00 00 BD B4 00 26 FF 00 05 00 00 [ 56 E9 ]",
        ),
        ("hf 14a raw -sc A22A10050000", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 BD [ 59 27 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 00 00 BD B4 00 26 FF [ 2C 00 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 00 00 BD B4 00 26 FF 10 05 00 00 [ F7 2A ]",
        ),
        ("hf 14a raw -sc A22A00050000", "0A"),
    ]

    @pytest.mark.parametrize("command, expected_result", test_data_ascii_counter_0)
    def test_ascii_counter_0(self, px, command, expected_result):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result in result

    @pytest.mark.parametrize("command, expected_result", test_data_ascii_counter_1)
    def test_ascii_counter_1(self, px, command, expected_result):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result in result

    @pytest.mark.parametrize("command, expected_result", test_data_ascii_counter_2)
    def test_ascii_counter_2(self, px, command, expected_result):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result in result

    @pytest.mark.parametrize("command, expected_result", test_data_ascii_counter_3)
    def test_ascii_counter_3(self, px, command, expected_result):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result in result

    test_data_ascii_counter_0_pwd = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229840026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 BD [ 59 27 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 00 00 BD 84 00 26 FF [ DE 4C ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 00 00 BD 84 00 26 FF 00 05 00 00 [ DE 04 ]",
        ),
        ("hf 14a raw -sc A22A18050000", "0A"),
        (
            "hf 14a raw -sck 1BFFFFFFFF",
            "00 00 [ A0 1E ]",
        ),
        (
            "hf 14a raw -ack 3025",
            "00 00 00 00 30 30 30 30 30 45 00 00 00 00 00 BD [ 99 26 ]",
        ),
        (
            "hf 14a raw -ack 3026",
            "30 30 30 30 30 46 00 00 00 00 00 BD 84 00 26 FF [ 68 D3 ]",
        ),
        (
            "hf 14a raw -ack 3027",
            "30 46 00 00 00 00 00 BD 84 00 26 FF 18 05 00 00 [ 71 EB ]",
        ),
        ("hf 14a raw -ac A22A00050000", "0A"),
    ]

    @pytest.mark.parametrize("command, expected_result", test_data_ascii_counter_0_pwd)
    def test_ascii_counter_pwd_0(self, px, command, expected_result):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result in result
