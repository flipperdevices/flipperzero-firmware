import time

import pytest
from flippigator.extensions.proxmark_wrapper import proxmark_wrapper


class TestEmulateNtag213:
    @pytest.fixture(scope="class", autouse=True)
    def emulate_ntag213(self, nav):
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

    Test_Data = [
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
            "46 46 43 41 34 44 35 44 38 30 00 00 00 AA AA BD [ 32 A3 ]",
            "Error: Incorrect data on NFC card",
        ),
    ]

    @pytest.mark.parametrize("command, expected_result, error", Test_Data)
    def test_ascii_mirror_uid(self, px, command, expected_result, error):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result == result, error

    test_data_byte_1 = [
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
            "46 46 43 41 34 44 35 44 38 30 00 00 00 AA AA BD [ 32 A3 ]",
            "Error: Incorrect data on NFC card",
        ),
    ]

    @pytest.mark.parametrize("command, expected_result, error", test_data_byte_1)
    def test_ascii_mirror_uid_byte_1(self, px, command, expected_result, error):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result == result, error

    test_data_byte_2 = [
        ("hf 14a raw -sc A229540024FF", "0A", "Error: NFC card not found"),
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
            "41 46 46 43 41 34 44 35 44 38 30 00 00 AA AA BD [ 36 2A ]",
            "Error: Incorrect data on NFC card",
        ),
    ]

    @pytest.mark.parametrize("command, expected_result, error", test_data_byte_2)
    def test_ascii_mirror_uid_byte_2(self, px, command, expected_result, error):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result == result, error

    test_data_ascii_counter_0 = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229840026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 AA AA BD [ 7B 09 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 AA AA BD 84 00 26 FF [ DB E9 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 AA AA BD 84 00 26 FF 00 05 00 00 [ E3 5C ]",
        ),
        ("hf 14a raw -sc A22A10050000", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 30 30 30 30 30 30 00 00 00 AA AA BD [ 54 E9 ]",
        ),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 30 30 30 30 30 31 00 00 00 AA AA BD [ 81 76 ]",
        ),
        (
            "hf 14a raw -sck 3025",
            "00 00 00 00 30 30 30 30 30 32 00 00 00 AA AA BD [ EF DE ]",
        ),
        (
            "hf 14a raw -ac 3025",
            "00 00 00 00 30 30 30 30 30 33 00 00 00 AA AA BD [ 3A 41 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "30 30 30 30 30 33 00 00 00 AA AA BD 84 00 26 FF [ 63 23 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "30 34 00 00 00 AA AA BD 84 00 26 FF 10 05 00 00 [ 4F B2 ]",
        ),
        ("hf 14a raw -sc A22A00050000", "0A"),
    ]

    ASCII_COUNTER_1 = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229940026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 AA AA BD [ 7B 09 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 AA AA BD 84 00 26 FF [ DB E9 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 AA AA BD 84 00 26 FF 00 05 00 00 [ E3 5C ]",
        ),
        ("hf 14a raw -sc A22A10050000", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 30 30 30 30 35 39 00 00 00 AA AA BD [ 0C 3F ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "30 30 30 30 35 41 00 00 00 AA AA BD 84 00 26 FF [ A8 56 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "35 42 00 00 00 AA AA BD 84 00 26 FF 10 05 00 00 [ 5F D8 ]",
        ),
        ("hf 14a raw -sc A22A00050000", "0A"),
    ]

    test_data_ascii_counter_1 = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229840026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 AA AA BD [ 7B 09 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 AA AA BD 84 00 26 FF [ DB E9 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 AA AA BD 84 00 26 FF 00 05 00 00 [ E3 5C ]",
        ),
        ("hf 14a raw -sc A22A10050000", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 30 30 30 30 35 39 00 00 00 AA AA BD [ 0C 3F ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "30 30 30 30 35 41 00 00 00 AA AA BD 84 00 26 FF [ A8 56 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "35 42 00 00 00 AA AA BD 84 00 26 FF 10 05 00 00 [ 5F D8 ]",
        ),
        ("hf 14a raw -sc A22A00050000", "0A"),
    ]

    test_data_ascii_counter_2 = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229A40026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 AA AA BD [ 7B 09 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 AA AA BD 84 00 26 FF [ DB E9 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 AA AA BD 84 00 26 FF 00 05 00 00 [ E3 5C ]",
        ),
        ("hf 14a raw -sc A22A10050000", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 30 30 30 30 35 39 00 00 00 AA AA BD [ 0C 3F ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "30 30 30 30 35 41 00 00 00 AA AA BD 84 00 26 FF [ A8 56 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "35 42 00 00 00 AA AA BD 84 00 26 FF 10 05 00 00 [ 5F D8 ]",
        ),
        ("hf 14a raw -sc A22A00050000", "0A"),
    ]

    test_data_ascii_counter_3 = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229B40026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 AA AA BD [ 7B 09 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 AA AA BD B4 00 26 FF [ 29 A5 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 AA AA BD B4 00 26 FF 00 05 00 00 [ 6B B1 ]",
        ),
        ("hf 14a raw -sc A22A10050000", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 AA AA BD [ 7B 09 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 AA AA BD B4 00 26 FF [ 29 A5 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 AA AA BD B4 00 26 FF 10 05 00 00 [ CA 72 ]",
        ),
        ("hf 14a raw -sc A22A00050000", "0A"),
    ]

    @pytest.mark.parametrize("command, expected_result", test_data_ascii_counter_0)
    def test_ascii_counter_0(self, px, command, expected_result):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result == result

        result = px_result[0]

        assert expected_result == result

    @pytest.mark.parametrize("command, expected_result", test_data_ascii_counter_1)
    def test_ascii_counter_1(self, px, command, expected_result):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result == result

    @pytest.mark.parametrize("command, expected_result", test_data_ascii_counter_2)
    def test_ascii_counter_2(self, px, command, expected_result):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result == result

    @pytest.mark.parametrize("command, expected_result", test_data_ascii_counter_3)
    def test_ascii_counter_3(self, px, command, expected_result):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result == result

    test_data_ascii_counter_0_pwd = [
        ("hf 14a raw -sc A22A00050000", "0A"),
        ("hf 14a raw -sc A229840026FF", "0A"),
        (
            "hf 14a raw -sc 3025",
            "00 00 00 00 00 00 00 00 00 00 00 00 00 AA AA BD [ 7B 09 ]",
        ),
        (
            "hf 14a raw -sc 3026",
            "00 00 00 00 00 00 00 00 00 AA AA BD 84 00 26 FF [ DB E9 ]",
        ),
        (
            "hf 14a raw -sc 3027",
            "00 00 00 00 00 AA AA BD 84 00 26 FF 00 05 00 00 [ E3 5C ]",
        ),
        ("hf 14a raw -sc A22A18050000", "0A"),
        (
            "hf 14a raw -sck 1BFFFFFFFF",
            "00 00 00 00 30 30 30 30 35 39 00 00 00 AA AA BD [ 0C 3F ]",
        ),
        (
            "hf 14a raw -ack 3025",
            "00 00 00 00 30 30 30 30 35 39 00 00 00 AA AA BD [ 0C 3F ]",
        ),
        (
            "hf 14a raw -ack 3026",
            "30 30 30 30 35 41 00 00 00 AA AA BD 84 00 26 FF [ A8 56 ]",
        ),
        (
            "hf 14a raw -ack 3027",
            "35 42 00 00 00 AA AA BD 84 00 26 FF 10 05 00 00 [ 5F D8 ]",
        ),
        ("hf 14a raw -ac A22A00050000", "0A"),
    ]

    @pytest.mark.parametrize("command, expected_result", test_data_ascii_counter_0_pwd)
    def test_ascii_counter_0_pwd(self, px, command, expected_result):
        px_result = px.execute(command)

        result = px_result[0]

        assert expected_result == result
