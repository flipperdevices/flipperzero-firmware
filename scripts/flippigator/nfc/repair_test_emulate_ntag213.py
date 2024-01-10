import time

import pytest


class TestEmulateNtag213:
    @pytest.fixture(scope="class", autouse=False)
    def emulate_ntag213(self, nav):
        nav.go_to_main_screen()
        if nav.open_file("NFC", "T_Ntag_213") == -1:
            assert 0, "File not found"

    @pytest.fixture(scope="class", autouse=True)
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

    class TestAsciiMirror:
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

        @pytest.mark.parametrize(
            "command, expected_result", test_data_ascii_counter_0_pwd
        )
        def test_ascii_counter_pwd_0(self, px, command, expected_result):
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

    class TestPwdProtection:
        data = [
            ("hf 14a raw -sc A229040000FF", "0A"),
            ("hf 14a raw -sc A22BDEADBEAF", "0A"),
            (
                "hf 14a raw -sc 302B",
                "00 00 00 00 00 00 00 00 04 BA FF C9 CA 4D 5D 80 [ 29 F8 ]",
            ),
            ("hf 14a raw -sc A22BFFFFFFFF", "0A"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_set_then_read(self, px, command, expected_result):
            """Set password then read it, then reset it"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc A22BDEADBEAF", "0A"),
            ("hf 14a raw -sc A2290400000A", "0A"),
            ("hf 14a raw -sck 1BDEAD0000", "00"),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            ("hf 14a raw -ack A229040000FF", "0A"),
            ("hf 14a raw -ac A22BFFFFFFFF", "0A"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_auth_with_wrong_pwd(self, px, command, expected_result):
            """Set password then try to auth with wrong one"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc A20A00000000", "0A"),
            ("hf 14a raw -sc A22BDEADBEAF", "0A"),
            ("hf 14a raw -sc A2290400000A", "0A"),
            (
                "hf 14a raw -sc 3009",
                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 [ 37 49 ]",
            ),
            (
                "hf 14a raw -sc 300A",
                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 [ 37 49 ]",
            ),
            (
                "hf 14a raw -sc 3A090F",
                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 [ CE 98 ]",
            ),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            ("hf 14a raw -ack A229040000FF", "0A"),
            ("hf 14a raw -ac A22BFFFFFFFF", "0A"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_auth_with_correct_pwd(self, px, command, expected_result):
            """Set password and AUTHLIM, then try to read without protection enabled"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc A20A00000000", "0A"),
            ("hf 14a raw -sc A22A80050000", "0A"),
            ("hf 14a raw -sc A22BDEADBEAF", "0A"),
            ("hf 14a raw -sc A2290400000A", "0A"),
            (
                "hf 14a raw -sc 3009",
                "00 00 00 00 04 BA FF C9 CA 4D 5D 80 5A 48 00 00 [ 46 3D ]",
            ),
            ("hf 14a raw -sc 300A", "00"),
            ("hf 14a raw -sc 3A090F", "00"),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            ("hf 14a raw -ack A229040000FF", "0A"),
            ("hf 14a raw -ack A22BFFFFFFFF", "0A"),
            ("hf 14a raw -ac A22A00050000", "0A"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_auth_with_correct_pwd(self, px, command, expected_result):
            """Set password and AUTHLIM, then try to read with protection enabled but no password applied"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc A20911223344", "0A"),
            ("hf 14a raw -sc A20AAABBCCDD", "0A"),
            ("hf 14a raw -sc A22A80050000", "0A"),
            ("hf 14a raw -sc A22BDEADBEAF", "0A"),
            ("hf 14a raw -sc A2290400000A", "0A"),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            (
                "hf 14a raw -ack 3009",
                "11 22 33 44 AA BB CC DD 00 00 00 00 00 00 00 00 [ B1 0A ]",
            ),
            (
                "hf 14a raw -ack 300A",
                "AA BB CC DD 00 00 00 00 00 00 00 00 00 00 00 00 [ AB 23 ]",
            ),
            (
                "hf 14a raw -ac 3A090F",
                "11 22 33 44 AA BB CC DD 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 [ 75 02 ]",
            ),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            ("hf 14a raw -ack A229040000FF", "0A"),
            ("hf 14a raw -ack A22BFFFFFFFF", "0A"),
            ("hf 14a raw -ac A22A00050000", "0A"),
            ("hf 14a raw -sc A20900000000", "0A"),
            ("hf 14a raw -sc A20A00000000", "0A"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_auth_with_correct_pwd(self, px, command, expected_result):
            """Set password and AUTHLIM = 0x0A, then try to read with protection enabled and password applied"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc A20A00000000", "0A"),
            ("hf 14a raw -sc A22BDEADBEAF", "0A"),
            ("hf 14a raw -sc A2290400000A", "0A"),
            ("hf 14a raw -sc A20A01020304", "00"),
            ("hf 14a raw -sck A00A", "00"),
            ("hf 14a raw -ac DEADBEAF00112233445566778899AABB", ""),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            ("hf 14a raw -ack A229040000FF", "0A"),
            ("hf 14a raw -ac A22BFFFFFFFF", "0A"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_write_without_pwd(self, px, command, expected_result):
            """Set password and AUTHLIM, then try to write without applying password"""
            px_result = px.execute(command)

            if expected_result == "":
                result = ""
            else:
                result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc A20A00000000", "0A"),
            ("hf 14a raw -sc A22BDEADBEAF", "0A"),
            ("hf 14a raw -sc A2290400000A", "0A"),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            ("hf 14a raw -ac A20A01020304", "0A"),
            (
                "hf 14a raw -sc 300A",
                "01 02 03 04 00 00 00 00 00 00 00 00 00 00 00 00 [ F9 C2 ]",
            ),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            ("hf 14a raw -ack A00A", "0A"),
            ("hf 14a raw -ac DEADBEAF00112233445566778899AABB", "0A"),
            (
                "hf 14a raw -sc 300A",
                "DE AD BE AF 00 00 00 00 00 00 00 00 00 00 00 00 [ E3 27 ]",
            ),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            ("hf 14a raw -ack A229040000FF", "0A"),
            ("hf 14a raw -ac A22BFFFFFFFF", "0A"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_write_with_pwd(self, px, command, expected_result):
            """Set password and AUTHLIM, then apply password and try to write"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

    class TestPwdAndMirror:
        data = [
            ("hf 14a raw -sc A22A80050000", "0A"),
            ("hf 14a raw -sc A22BDEADBEAF", "0A"),
            ("hf 14a raw -sc A2294400240A", "0A"),
            ("hf 14a raw -sc 3024", "00"),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            (
                "hf 14a raw -ack 3024",
                "30 34 42 41 46 46 43 41 34 44 35 44 38 30 00 00 [ E9 19 ]",
            ),
            (
                "hf 14a raw -ack 3023",
                "00 00 00 00 30 34 42 41 46 46 43 41 34 44 35 44 [ 1C AC ]",
            ),
            (
                "hf 14a raw -ack 3025",
                "46 46 43 41 34 44 35 44 38 30 00 00 00 00 00 BD [ 10 8D ]",
            ),
            ("hf 14a raw -ack A229040000FF", "0A"),
            ("hf 14a raw -ack A22BFFFFFFFF", "0A"),
            ("hf 14a raw -ac A22A00050000", "0A"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_set_then_read_36(self, px, command, expected_result):
            """Set protection and password whith ascii mirror page = 36, byte = 0, mode = UID"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc A22A80050000", "0A"),
            ("hf 14a raw -sc A22BDEADBEAF", "0A"),
            ("hf 14a raw -sc A22944000404", "0A"),
            (
                "hf 14a raw -sc 3003",
                "E1 10 12 00 04 BA FF C9 CA 4D 5D 80 5A 48 00 00 [ A7 C3 ]",
            ),
            ("hf 14a raw -sck 1BDEADBEAF", "00 00 [ A0 1E ]"),
            (
                "hf 14a raw -ack 3003",
                "E1 10 12 00 30 34 42 41 46 46 43 41 34 44 35 44 [ FD 52 ]",
            ),
            (
                "hf 14a raw -ack 3004",
                "30 34 42 41 46 46 43 41 34 44 35 44 38 30 00 00 [ E9 19 ]",
            ),
            (
                "hf 14a raw -ack 3005",
                "46 46 43 41 34 44 35 44 38 30 00 00 00 00 00 00 [ 7E E3 ]",
            ),
            ("hf 14a raw -ack A229040000FF", "0A"),
            ("hf 14a raw -ack A22BFFFFFFFF", "0A"),
            ("hf 14a raw -ac A22A00050000", "0A"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_set_then_read_04(self, px, command, expected_result):
            """Set protection and password whith ascii mirror page = 04, byte = 0, mode = UID"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result
