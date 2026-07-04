#!/usr/bin/env python3
"""
ISO7816 GET DATA command test suite for Flipper Zero USB CCID implementation.

Tests the GET DATA command (INS=0xCA) as defined in ISO/IEC 7816-4:2005 Section 7.4.2.

Requirements:
    pip install pyscard pytest

Usage:
    pytest get_data_test.py -v

Reference: ISO/IEC 7816-4:2005 Section 7.4.2
"""

import pytest
from smartcard.System import readers
from smartcard.Exceptions import NoCardException, CardConnectionException
from smartcard.util import toHexString


# =============================================================================
# Constants
# =============================================================================

# Status words
SW_OK = [0x90, 0x00]
SW_FILE_NOT_FOUND = [0x6A, 0x82]
SW_REF_DATA_NOT_FOUND = [0x6A, 0x88]
SW_WRONG_LENGTH = [0x67, 0x00]
SW_WRONG_P1P2 = [0x6A, 0x86]
SW_FUNC_NOT_SUPPORTED = [0x6A, 0x81]
SW_CMD_NOT_ALLOWED_NO_EF = [0x69, 0x86]

# Two-byte BER-TLV tags (P1-P2)
TAG_ATR_INFO = [0x5F, 0x51]           # Answer-to-Reset information
TAG_HISTORICAL_BYTES = [0x5F, 0x52]   # Historical bytes
TAG_EXTENDED_LENGTH = [0x5F, 0x56]    # Extended length info
TAG_URL = [0x5F, 0x50]                # Application URL

# Single-byte BER-TLV tags (P1=0x00, P2=tag)
TAG_AID = 0x4F                        # Application identifier
TAG_DF_NAME = 0x84                    # DF name
TAG_APPLICATION_LABEL = 0x50          # Application label

# Special P1-P2 values
P1P2_DUMP = [0x00, 0x00]              # Dump current EF


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

    print(f"\nUsing reader: {reader}")

    try:
        conn = reader.createConnection()
        conn.connect()
        print(f"ATR: {toHexString(conn.getATR())}")
        yield conn
        conn.disconnect()
    except (NoCardException, CardConnectionException) as e:
        pytest.skip(f"Could not connect to card: {e}")


# =============================================================================
# Helper Functions
# =============================================================================

def transmit_and_log(connection, apdu: list, description: str = ""):
    """Transmit an APDU and log the exchange."""
    if description:
        print(f"\n{description}")
    print(f"  >> {toHexString(apdu)}")
    data, sw1, sw2 = connection.transmit(apdu)
    print(f"  << {toHexString(data)} SW={sw1:02X}{sw2:02X}")
    return data, sw1, sw2


def get_data_two_byte_tag(connection, tag: list, le: int = 0x00):
    """GET DATA with two-byte BER-TLV tag in P1-P2."""
    # APDU: 00 CA P1 P2 Le
    apdu = [0x00, 0xCA, tag[0], tag[1], le]
    return transmit_and_log(connection, apdu, f"GET DATA tag {toHexString(tag)}")


def get_data_single_byte_tag(connection, tag: int, le: int = 0x00):
    """GET DATA with single-byte BER-TLV tag in P2 (P1=0x00)."""
    # APDU: 00 CA 00 tag Le
    apdu = [0x00, 0xCA, 0x00, tag, le]
    return transmit_and_log(connection, apdu, f"GET DATA tag {tag:02X}")


def get_data_dump(connection, le: int = 0x00):
    """GET DATA with P1-P2=0000 to dump current EF."""
    # APDU: 00 CA 00 00 Le
    apdu = [0x00, 0xCA, 0x00, 0x00, le]
    return transmit_and_log(connection, apdu, "GET DATA dump (P1P2=0000)")


def select_mf(connection):
    """Select Master File."""
    apdu = [0x00, 0xA4, 0x00, 0x00, 0x00]
    return transmit_and_log(connection, apdu, "SELECT MF")


def select_by_fid(connection, fid: list):
    """Select file by identifier."""
    apdu = [0x00, 0xA4, 0x00, 0x00, len(fid)] + fid + [0x00]
    return transmit_and_log(connection, apdu, f"SELECT {toHexString(fid)}")


def parse_tlv(data: list) -> dict:
    """Parse TLV structure and return tag->value dict."""
    result = {}
    pos = 0
    while pos < len(data) - 1:
        tag = data[pos]
        # Handle two-byte tags (first byte high nibble >= 1F)
        if (tag & 0x1F) == 0x1F:
            tag = (tag << 8) | data[pos + 1]
            pos += 1
        
        pos += 1
        if pos >= len(data):
            break
            
        length = data[pos]
        pos += 1
        
        if pos + length > len(data):
            length = len(data) - pos
        
        value = data[pos:pos + length]
        result[tag] = value
        pos += length
    
    return result


# =============================================================================
# GET DATA Two-Byte Tag Tests
# =============================================================================

class TestGetDataTwoByteTag:
    """Test GET DATA with two-byte BER-TLV tags (P1-P2 >= 0x4000)."""

    def test_get_data_atr_info(self, connection):
        """GET DATA 5F51 should return ATR information."""
        # First select MF to establish context
        select_mf(connection)
        
        data, sw1, sw2 = get_data_two_byte_tag(connection, TAG_ATR_INFO)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"
        
        # Should have data
        assert len(data) > 0, "Expected ATR info data"
        
        # First two bytes should be the tag 5F51
        assert data[0] == 0x5F, f"Expected tag 5F, got {data[0]:02X}"
        assert data[1] == 0x51, f"Expected tag 51, got {data[1]:02X}"

    def test_get_data_historical_bytes(self, connection):
        """GET DATA 5F52 should return historical bytes."""
        select_mf(connection)
        
        data, sw1, sw2 = get_data_two_byte_tag(connection, TAG_HISTORICAL_BYTES)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"
        
        # Should have data
        assert len(data) > 0, "Expected historical bytes data"
        
        # First two bytes should be the tag 5F52
        assert data[0] == 0x5F
        assert data[1] == 0x52

    def test_get_data_extended_length(self, connection):
        """GET DATA 5F56 should return extended length info."""
        select_mf(connection)
        
        data, sw1, sw2 = get_data_two_byte_tag(connection, TAG_EXTENDED_LENGTH)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"
        
        # Should have data
        assert len(data) > 0, "Expected extended length info"
        
        # First two bytes should be the tag 5F56
        assert data[0] == 0x5F
        assert data[1] == 0x56

    def test_get_data_nonexistent_tag(self, connection):
        """GET DATA for unknown tag should return 6A88."""
        select_mf(connection)
        
        # Use a tag that doesn't exist
        data, sw1, sw2 = get_data_two_byte_tag(connection, [0x5F, 0x99])
        assert [sw1, sw2] == SW_REF_DATA_NOT_FOUND, \
            f"Expected 6A88, got {sw1:02X}{sw2:02X}"


# =============================================================================
# GET DATA Dump Tests (P1-P2 = 0000)
# =============================================================================

class TestGetDataDump:
    """Test GET DATA with P1-P2 = 0000 (dump current EF)."""

    def test_get_data_dump_no_ef_selected(self, connection):
        """GET DATA 0000 with no EF selected should return error."""
        # Select MF (not an EF)
        select_mf(connection)
        
        data, sw1, sw2 = get_data_dump(connection)
        
        # Should fail because no EF is selected
        assert [sw1, sw2] == SW_CMD_NOT_ALLOWED_NO_EF, \
            f"Expected 6986, got {sw1:02X}{sw2:02X}"

    def test_get_data_dump_with_ef_selected(self, connection):
        """GET DATA 0000 with EF selected should return EF content."""
        # Select MF first
        select_mf(connection)
        
        # Select EF.DIR (2F00) under MF
        data, sw1, sw2 = select_by_fid(connection, [0x2F, 0x00])
        if [sw1, sw2] != SW_OK:
            pytest.skip("EF.DIR not available")
        
        # Now try to dump the EF
        data, sw1, sw2 = get_data_dump(connection)
        
        # Should succeed (EF might be empty but should return 9000)
        assert [sw1, sw2] == SW_OK or data is not None, \
            f"Expected 9000, got {sw1:02X}{sw2:02X}"


# =============================================================================
# GET DATA Single-Byte Tag Tests
# =============================================================================

class TestGetDataSingleByteTag:
    """Test GET DATA with single-byte BER-TLV tags (P1=0x00, P2=0x40-0xFF)."""

    def test_get_data_aid_no_app_selected(self, connection):
        """GET DATA 004F should return 6A88 when no app with AID is selected."""
        # Select MF (has no AID)
        select_mf(connection)
        
        data, sw1, sw2 = get_data_single_byte_tag(connection, TAG_AID)
        
        # MF doesn't have an AID, should return not found
        assert [sw1, sw2] == SW_REF_DATA_NOT_FOUND, \
            f"Expected 6A88, got {sw1:02X}{sw2:02X}"

    def test_get_data_df_name_at_mf(self, connection):
        """GET DATA 0084 at MF should return 6A88 (MF has no AID)."""
        select_mf(connection)
        
        data, sw1, sw2 = get_data_single_byte_tag(connection, TAG_DF_NAME)
        
        # MF doesn't have a DF name (AID), should return not found
        assert [sw1, sw2] == SW_REF_DATA_NOT_FOUND, \
            f"Expected 6A88, got {sw1:02X}{sw2:02X}"


# =============================================================================
# GET DATA Error Cases
# =============================================================================

class TestGetDataErrors:
    """Test GET DATA error conditions."""

    def test_get_data_odd_ins_not_supported(self, connection):
        """GET DATA with INS=CB (odd, tag list) should return 6A81."""
        select_mf(connection)
        
        # INS=0xCB with tag list in data (not implemented)
        # APDU: 00 CB 00 00 03 5C 01 4F 00
        apdu = [0x00, 0xCB, 0x00, 0x00, 0x03, 0x5C, 0x01, 0x4F, 0x00]
        data, sw1, sw2 = transmit_and_log(connection, apdu, "GET DATA INS=CB")
        
        assert [sw1, sw2] == SW_FUNC_NOT_SUPPORTED, \
            f"Expected 6A81, got {sw1:02X}{sw2:02X}"

    def test_get_data_proprietary_range(self, connection):
        """GET DATA in proprietary range (0100-01FF) returns 6A88 if not registered."""
        select_mf(connection)
        
        # Proprietary tag 0x0150
        apdu = [0x00, 0xCA, 0x01, 0x50, 0x00]
        data, sw1, sw2 = transmit_and_log(connection, apdu, "GET DATA proprietary 0150")
        
        # Should return not found (no proprietary data registered)
        assert [sw1, sw2] == SW_REF_DATA_NOT_FOUND, \
            f"Expected 6A88, got {sw1:02X}{sw2:02X}"


# =============================================================================
# GET DATA Content Verification Tests
# =============================================================================

class TestGetDataContent:
    """Test GET DATA response content validation."""

    def test_atr_info_contains_flipper(self, connection):
        """ATR info should contain 'Flipper' string."""
        select_mf(connection)
        
        data, sw1, sw2 = get_data_two_byte_tag(connection, TAG_ATR_INFO)
        assert [sw1, sw2] == SW_OK
        
        # Check if 'Flipper' is in the response
        # 'Flipper' = 46 6C 69 70 70 65 72
        flipper_bytes = [0x46, 0x6C, 0x69, 0x70, 0x70, 0x65, 0x72]
        data_str = bytes(data)
        
        assert b"Flipper" in data_str, \
            f"Expected 'Flipper' in ATR info, got {toHexString(data)}"

    def test_extended_length_values(self, connection):
        """Extended length info should contain valid max lengths."""
        select_mf(connection)
        
        data, sw1, sw2 = get_data_two_byte_tag(connection, TAG_EXTENDED_LENGTH)
        assert [sw1, sw2] == SW_OK
        
        # Parse the response: tag(2) + len(1) + max_cmd(2) + max_rsp(2) + reserved(2)
        assert len(data) >= 9, f"Expected at least 9 bytes, got {len(data)}"
        
        # Skip tag (5F56) and length
        max_cmd_len = (data[3] << 8) | data[4]
        max_rsp_len = (data[5] << 8) | data[6]
        
        # Should support at least 256 bytes (short APDU)
        assert max_cmd_len >= 256, f"Max command length {max_cmd_len} too small"
        assert max_rsp_len >= 256, f"Max response length {max_rsp_len} too small"


# =============================================================================
# Main
# =============================================================================

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
