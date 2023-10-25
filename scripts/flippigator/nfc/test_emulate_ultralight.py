import time

import pytest


class TestEmulateUltralight:
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

    class TestMFU11:

        data = [
            ("hf 14a raw -sc 4B0000000000000000000000000000000000000000", "05 [ 53 06 ]"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_vcsl(self, px, command, expected_result):
            """Test of VSCL command"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc 3900", "00 00 00 [ 14 A5 ]"),
            ("hf 14a raw -sc A50000000100", "0A"),
            ("hf 14a raw -sc 3900", "00 00 01 [ 9D B4 ]"),
            ("hf 14a raw -sc A50000000500", "0A"),
            ("hf 14a raw -sc 3900", "00 00 06 [ 22 C0 ]"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_counter_0(self, px, command, expected_result):
            """Test of counter 0"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc 3901", "00 00 00 [ 14 A5 ]"),
            ("hf 14a raw -sc A50100000100", "0A"),
            ("hf 14a raw -sc 3901", "00 00 01 [ 9D B4 ]"),
            ("hf 14a raw -sc A50100000500", "0A"),
            ("hf 14a raw -sc 3901", "00 00 06 [ 22 C0 ]"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_counter_1(self, px, command, expected_result):
            """Test of counter 1"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc 3902", "00 00 00 [ 14 A5 ]"),
            ("hf 14a raw -sc A50200000100", "0A"),
            ("hf 14a raw -sc 3902", "00 00 01 [ 9D B4 ]"),
            ("hf 14a raw -sc A50200000500", "0A"),
            ("hf 14a raw -sc 3902", "00 00 06 [ 22 C0 ]"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_counter_2(self, px, command, expected_result):
            """Test of counter 2"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc 3E00", "BD [ 90 3F ]"),
            ("hf 14a raw -sc 3E01", "BD [ 90 3F ]"),
            ("hf 14a raw -sc 3E02", "BD [ 90 3F ]"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_check_tearing(self, px, command, expected_result):
            """Tearing test"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

    class TestMFU21:

        data = [
            ("hf 14a raw -sc 4B0000000000000000000000000000000000000000", "05 [ 53 06 ]"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_vcsl(self, px, command, expected_result):
            """Test of VSCL command"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc 3900", "00 00 00 [ 14 A5 ]"),
            ("hf 14a raw -sc A50000000100", "0A"),
            ("hf 14a raw -sc 3900", "00 00 01 [ 9D B4 ]"),
            ("hf 14a raw -sc A50000000500", "0A"),
            ("hf 14a raw -sc 3900", "00 00 06 [ 22 C0 ]"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_counter_0(self, px, command, expected_result):
            """Test of counter 0"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc 3901", "00 00 00 [ 14 A5 ]"),
            ("hf 14a raw -sc A50100000100", "0A"),
            ("hf 14a raw -sc 3901", "00 00 01 [ 9D B4 ]"),
            ("hf 14a raw -sc A50100000500", "0A"),
            ("hf 14a raw -sc 3901", "00 00 06 [ 22 C0 ]"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_counter_1(self, px, command, expected_result):
            """Test of counter 1"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc 3902", "00 00 00 [ 14 A5 ]"),
            ("hf 14a raw -sc A50200000100", "0A"),
            ("hf 14a raw -sc 3902", "00 00 01 [ 9D B4 ]"),
            ("hf 14a raw -sc A50200000500", "0A"),
            ("hf 14a raw -sc 3902", "00 00 06 [ 22 C0 ]"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_counter_2(self, px, command, expected_result):
            """Test of counter 2"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

        data = [
            ("hf 14a raw -sc 3E00", "BD [ 90 3F ]"),
            ("hf 14a raw -sc 3E01", "BD [ 90 3F ]"),
            ("hf 14a raw -sc 3E02", "BD [ 90 3F ]"),
        ]

        @pytest.mark.parametrize("command, expected_result", data)
        def test_check_tearing(self, px, command, expected_result):
            """Tearing test"""
            px_result = px.execute(command)

            result = px_result[0]

            assert expected_result in result

