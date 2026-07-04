#!/usr/bin/env python3
"""
ISO7816 APDU-level test suite for Flipper Zero USB CCID implementation.

This test suite tests the ISO7816 APDU layer using the PC/SC abstraction,
as opposed to the CCID-level tests which use raw USB communication.

Requirements:
    pip install pyscard pytest

Usage:
    pytest iso7816_test.py -v

Reference: ISO/IEC 7816-4
"""

import pytest
from smartcard.System import readers
from smartcard.Exceptions import NoCardException, CardConnectionException


# =============================================================================
# Fixtures
# =============================================================================

@pytest.fixture(scope="module")
def connection():
    """Create a connection to the smart card reader."""
    r = readers()
    if not r:
        pytest.skip("No smart card readers found")

    # Find the Flipper CCID reader
    reader = None
    for sc in r:
        if "USB Smart Card" in str(sc) or "Flipper" in str(sc):
            reader = sc
            break

    if reader is None:
        reader = r[0]  # Use first available reader

    try:
        conn = reader.createConnection()
        conn.connect()
        yield conn
        conn.disconnect()
    except (NoCardException, CardConnectionException) as e:
        pytest.skip(f"Could not connect to card: {e}")


# =============================================================================
# Helper Functions
# =============================================================================

def build_extended_apdu(cla: int, ins: int, p1: int, p2: int,
                        data: list = None, le: int = None) -> list:
    """Build an extended APDU (Case 4E format if data and le provided)."""
    apdu = [cla, ins, p1, p2]

    if data is not None:
        lc = len(data)
        lc_hi = (lc >> 8) & 0xFF
        lc_lo = lc & 0xFF
        apdu.extend([0x00, lc_hi, lc_lo])  # Extended Lc
        apdu.extend(data)

    if le is not None:
        le_hi = (le >> 8) & 0xFF
        le_lo = le & 0xFF
        if data is None:
            apdu.append(0x00)  # Extended Le marker for Case 2E
        apdu.extend([le_hi, le_lo])

    return apdu


# =============================================================================
# Basic APDU Tests
# =============================================================================

class TestBasicAPDU:
    """Test basic APDU command cases."""

    def test_case1_no_data(self, connection):
        """Case 1: No data, no response (4 bytes: CLA INS P1 P2)."""
        data, sw1, sw2 = connection.transmit([0x01, 0x01, 0x00, 0x00])
        assert sw1 == 0x90, f"Expected SW1=90, got {sw1:02X}"
        assert sw2 == 0x00, f"Expected SW2=00, got {sw2:02X}"
        assert len(data) == 0, f"Expected no data, got {len(data)} bytes"

    def test_case2_response_only(self, connection):
        """Case 2S: No data, response expected (5 bytes: CLA INS P1 P2 Le)."""
        data, sw1, sw2 = connection.transmit([0x01, 0x02, 0x00, 0x00, 0x02])
        assert sw1 == 0x90, f"Expected SW1=90, got {sw1:02X}"
        assert sw2 == 0x00, f"Expected SW2=00, got {sw2:02X}"
        assert data == [0x62, 0x63], f"Expected [0x62, 0x63], got {data}"

    def test_case3_command_only(self, connection):
        """Case 3S: Data, no response (5+Lc bytes: CLA INS P1 P2 Lc Data)."""
        data, sw1, sw2 = connection.transmit(
            [0x01, 0x03, 0x00, 0x00, 0x02, 0xCA, 0xFE]
        )
        assert sw1 == 0x90, f"Expected SW1=90, got {sw1:02X}"
        assert sw2 == 0x00, f"Expected SW2=00, got {sw2:02X}"
        assert len(data) == 0, f"Expected no data, got {len(data)} bytes"

    def test_case4_echo(self, connection):
        """Case 4S: Data with response (6+Lc bytes: CLA INS P1 P2 Lc Data Le)."""
        data, sw1, sw2 = connection.transmit(
            [0x01, 0x04, 0x00, 0x00, 0x02, 0xCA, 0xFE, 0x02]
        )
        assert sw1 == 0x90, f"Expected SW1=90, got {sw1:02X}"
        assert sw2 == 0x00, f"Expected SW2=00, got {sw2:02X}"
        assert data == [0xCA, 0xFE], f"Expected [0xCA, 0xFE], got {data}"


# =============================================================================
# Buffer Size Tests
# =============================================================================

class TestBufferSizes:
    """Test various buffer sizes for short APDUs."""

    def test_small_buffer_15_bytes(self, connection):
        """Echo 15 bytes of data."""
        test_data = list(range(0x0F))
        apdu = [0x01, 0x04, 0x00, 0x00, 0x0F] + test_data + [0x0F]
        data, sw1, sw2 = connection.transmit(apdu)

        assert sw1 == 0x90, f"Expected SW1=90, got {sw1:02X}"
        assert sw2 == 0x00, f"Expected SW2=00, got {sw2:02X}"
        assert data == test_data, "Echo data mismatch"

    def test_large_buffer_240_bytes(self, connection):
        """Echo 240 bytes of data (near short APDU limit)."""
        test_data = list(range(0xF0))
        apdu = [0x01, 0x04, 0x00, 0x00, 0xF0] + test_data + [0xF0]
        data, sw1, sw2 = connection.transmit(apdu)

        assert sw1 == 0x90, f"Expected SW1=90, got {sw1:02X}"
        assert sw2 == 0x00, f"Expected SW2=00, got {sw2:02X}"
        assert data == test_data, "Echo data mismatch"


# =============================================================================
# Extended APDU Tests
# =============================================================================

class TestExtendedAPDU:
    """Test extended APDU support (>255 bytes)."""

    def test_case2e_extended_le(self, connection):
        """Case 2E: Extended Le only (7 bytes: CLA INS P1 P2 00 Le1 Le2)."""
        # Request 256 bytes but INS 0x02 only returns 2 bytes
        apdu = [0x01, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00]  # Le = 0x0100 = 256
        data, sw1, sw2 = connection.transmit(apdu)

        assert sw1 == 0x90, f"Expected SW1=90, got {sw1:02X}"
        assert sw2 == 0x00, f"Expected SW2=00, got {sw2:02X}"
        assert data == [0x62, 0x63], f"Expected [0x62, 0x63], got {data}"

    def test_case4e_echo_300_bytes(self, connection):
        """Case 4E: Extended Lc and Le with 300 bytes."""
        test_data = [(i % 256) for i in range(300)]
        apdu = build_extended_apdu(0x01, 0x04, 0x00, 0x00, test_data, 300)
        data, sw1, sw2 = connection.transmit(apdu)

        assert sw1 == 0x90, f"Expected SW1=90, got {sw1:02X}"
        assert sw2 == 0x00, f"Expected SW2=00, got {sw2:02X}"
        assert data == test_data, "Echo data mismatch for 300 bytes"

    def test_case4e_echo_512_bytes(self, connection):
        """Case 4E: Extended Lc and Le with 512 bytes."""
        test_data = [(i % 256) for i in range(512)]
        apdu = build_extended_apdu(0x01, 0x04, 0x00, 0x00, test_data, 512)
        data, sw1, sw2 = connection.transmit(apdu)

        assert sw1 == 0x90, f"Expected SW1=90, got {sw1:02X}"
        assert sw2 == 0x00, f"Expected SW2=00, got {sw2:02X}"
        assert data == test_data, "Echo data mismatch for 512 bytes"

    def test_case4e_echo_1000_bytes(self, connection):
        """Case 4E: Extended Lc and Le with 1000 bytes."""
        test_data = [(i % 256) for i in range(1000)]
        apdu = build_extended_apdu(0x01, 0x04, 0x00, 0x00, test_data, 1000)
        data, sw1, sw2 = connection.transmit(apdu)

        assert sw1 == 0x90, f"Expected SW1=90, got {sw1:02X}"
        assert sw2 == 0x00, f"Expected SW2=00, got {sw2:02X}"
        assert data == test_data, "Echo data mismatch for 1000 bytes"


# =============================================================================
# Main (for direct execution)
# =============================================================================

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
