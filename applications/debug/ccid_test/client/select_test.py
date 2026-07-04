#!/usr/bin/env python3
"""
ISO7816 SELECT command test suite for Flipper Zero USB CCID implementation.

Tests the SELECT command (INS=0xA4) with various selection modes as defined
in ISO/IEC 7816-4:2005 Section 7.1.1.

Requirements:
    pip install pyscard pytest

Usage:
    pytest select_test.py -v

Reference: ISO/IEC 7816-4:2005 Section 7.1.1
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
SW_WRONG_LENGTH = [0x67, 0x00]
SW_WRONG_P1P2 = [0x6A, 0x86]
SW_FILE_DEACTIVATED = [0x62, 0x83]

# File identifiers
FID_MF = [0x3F, 0x00]
FID_EF_DIR = [0x2F, 0x00]
FID_EF_ATR = [0x2F, 0x01]

# P1 values for SELECT
SELECT_P1_BY_FID = 0x00
SELECT_P1_CHILD_DF = 0x01
SELECT_P1_EF_UNDER_DF = 0x02
SELECT_P1_PARENT_DF = 0x03
SELECT_P1_BY_AID = 0x04
SELECT_P1_PATH_FROM_MF = 0x08
SELECT_P1_PATH_FROM_CURRENT = 0x09

# P2 values for SELECT (FCI control)
SELECT_P2_FCI = 0x00
SELECT_P2_FCP = 0x04
SELECT_P2_FMD = 0x08
SELECT_P2_NONE = 0x0C


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


def select_by_fid(connection, fid: list, p2: int = SELECT_P2_FCI, le: int = 0x00):
    """Send SELECT by file identifier."""
    apdu = [0x00, 0xA4, SELECT_P1_BY_FID, p2] + [len(fid)] + fid + [le]
    return transmit_and_log(connection, apdu, f"SELECT by FID {toHexString(fid)}")


def select_mf(connection, p2: int = SELECT_P2_FCI):
    """Select Master File."""
    # SELECT with no data selects MF
    apdu = [0x00, 0xA4, SELECT_P1_BY_FID, p2, 0x00]
    return transmit_and_log(connection, apdu, "SELECT MF (no data)")


def select_by_aid(connection, aid: list, p2: int = SELECT_P2_FCI, le: int = 0x00):
    """Send SELECT by AID (P1=0x04)."""
    apdu = [0x00, 0xA4, SELECT_P1_BY_AID, p2, len(aid)] + aid + [le]
    return transmit_and_log(connection, apdu, f"SELECT by AID {toHexString(aid)}")


def select_parent_df(connection, p2: int = SELECT_P2_FCI):
    """Select parent DF."""
    apdu = [0x00, 0xA4, SELECT_P1_PARENT_DF, p2, 0x00]
    return transmit_and_log(connection, apdu, "SELECT parent DF")


def select_ef_under_df(connection, fid: list, p2: int = SELECT_P2_FCI, le: int = 0x00):
    """Select EF under current DF."""
    apdu = [0x00, 0xA4, SELECT_P1_EF_UNDER_DF, p2, len(fid)] + fid + [le]
    return transmit_and_log(connection, apdu, f"SELECT EF under DF {toHexString(fid)}")


def parse_fci(data: list) -> dict:
    """Parse FCI template (tag 6F) and extract common fields."""
    result = {}
    if len(data) < 2:
        return result
    
    if data[0] == 0x6F:  # FCI template
        fci_len = data[1]
        pos = 2
        while pos < len(data) - 1:
            tag = data[pos]
            length = data[pos + 1]
            value = data[pos + 2:pos + 2 + length]
            
            if tag == 0x83:  # File identifier
                result['fid'] = value
            elif tag == 0x84:  # DF name (AID)
                result['aid'] = value
            elif tag == 0x82:  # File descriptor
                result['descriptor'] = value
            elif tag == 0x8A:  # Life cycle status
                result['lcs'] = value[0] if value else None
            elif tag == 0x80:  # File size
                result['size'] = value
            
            pos += 2 + length
    
    return result


# =============================================================================
# SELECT MF Tests
# =============================================================================

class TestSelectMF:
    """Test SELECT MF (Master File) operations."""

    def test_select_mf_no_data(self, connection):
        """SELECT with no data should select MF."""
        data, sw1, sw2 = select_mf(connection)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"

    def test_select_mf_by_fid(self, connection):
        """SELECT with FID 3F00 should select MF."""
        data, sw1, sw2 = select_by_fid(connection, FID_MF)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"
        
        # Parse FCI and verify
        if len(data) > 0:
            fci = parse_fci(data)
            if 'fid' in fci:
                assert fci['fid'] == FID_MF, f"Expected FID 3F00, got {fci['fid']}"

    def test_select_mf_returns_fci(self, connection):
        """SELECT MF with P2=00 should return FCI template."""
        data, sw1, sw2 = select_mf(connection, p2=SELECT_P2_FCI)
        assert [sw1, sw2] == SW_OK
        
        if len(data) > 0:
            assert data[0] == 0x6F, f"Expected FCI template (6F), got {data[0]:02X}"

    def test_select_mf_fcp_template(self, connection):
        """SELECT MF with P2=04 should return FCP template."""
        # First select something else to have context
        select_mf(connection)
        
        apdu = [0x00, 0xA4, SELECT_P1_BY_FID, SELECT_P2_FCP, 0x02] + FID_MF + [0x00]
        data, sw1, sw2 = transmit_and_log(connection, apdu, "SELECT MF with FCP")
        assert [sw1, sw2] == SW_OK
        
        if len(data) > 0:
            assert data[0] == 0x62, f"Expected FCP template (62), got {data[0]:02X}"


# =============================================================================
# SELECT EF Tests
# =============================================================================

class TestSelectEF:
    """Test SELECT EF (Elementary File) operations."""

    def test_select_ef_dir(self, connection):
        """SELECT EF.DIR (2F00) under MF."""
        # First select MF
        select_mf(connection)
        
        # Then select EF.DIR
        data, sw1, sw2 = select_ef_under_df(connection, FID_EF_DIR)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"

    def test_select_ef_atr(self, connection):
        """SELECT EF.ATR (2F01) under MF."""
        # First select MF
        select_mf(connection)
        
        # Then select EF.ATR
        data, sw1, sw2 = select_ef_under_df(connection, FID_EF_ATR)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"

    def test_select_nonexistent_ef(self, connection):
        """SELECT non-existent EF should return 6A82."""
        select_mf(connection)
        
        data, sw1, sw2 = select_ef_under_df(connection, [0xFF, 0xFF])
        assert [sw1, sw2] == SW_FILE_NOT_FOUND, \
            f"Expected 6A82 (file not found), got {sw1:02X}{sw2:02X}"


# =============================================================================
# SELECT by AID Tests
# =============================================================================

class TestSelectByAID:
    """Test SELECT by DF name (AID) operations."""

    def test_select_nonexistent_aid(self, connection):
        """SELECT with unknown AID should return 6A82."""
        # Random AID that doesn't exist
        unknown_aid = [0xA0, 0x00, 0x00, 0x00, 0x99, 0x99, 0x99]
        data, sw1, sw2 = select_by_aid(connection, unknown_aid)
        assert [sw1, sw2] == SW_FILE_NOT_FOUND, \
            f"Expected 6A82 (file not found), got {sw1:02X}{sw2:02X}"

    def test_select_aid_wrong_length(self, connection):
        """SELECT with AID < 5 bytes should return error."""
        short_aid = [0xA0, 0x00, 0x00]  # Too short
        apdu = [0x00, 0xA4, SELECT_P1_BY_AID, SELECT_P2_FCI, len(short_aid)] + short_aid + [0x00]
        data, sw1, sw2 = transmit_and_log(connection, apdu, "SELECT with short AID")
        assert [sw1, sw2] == SW_WRONG_LENGTH, \
            f"Expected 6700 (wrong length), got {sw1:02X}{sw2:02X}"


# =============================================================================
# SELECT Parent DF Tests  
# =============================================================================

class TestSelectParentDF:
    """Test SELECT parent DF operations."""

    def test_select_parent_from_mf(self, connection):
        """SELECT parent from MF should stay at MF."""
        select_mf(connection)
        
        data, sw1, sw2 = select_parent_df(connection)
        # Should succeed (stays at MF)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"

    def test_select_parent_multiple_times(self, connection):
        """SELECT parent multiple times should stay at MF."""
        select_mf(connection)
        
        # Select parent several times
        for i in range(3):
            data, sw1, sw2 = select_parent_df(connection)
            assert [sw1, sw2] == SW_OK, f"Iteration {i}: Expected 9000, got {sw1:02X}{sw2:02X}"


# =============================================================================
# SELECT Error Cases
# =============================================================================

class TestSelectErrors:
    """Test SELECT error handling."""

    def test_select_invalid_p1(self, connection):
        """SELECT with invalid P1 should return 6A86."""
        apdu = [0x00, 0xA4, 0xFF, 0x00, 0x00]  # P1=0xFF is invalid
        data, sw1, sw2 = transmit_and_log(connection, apdu, "SELECT with invalid P1")
        assert [sw1, sw2] == SW_WRONG_P1P2, \
            f"Expected 6A86 (wrong P1-P2), got {sw1:02X}{sw2:02X}"

    def test_select_wrong_fid_length(self, connection):
        """SELECT by FID with wrong data length should return error."""
        # FID should be exactly 2 bytes
        apdu = [0x00, 0xA4, SELECT_P1_BY_FID, SELECT_P2_FCI, 0x03, 0x3F, 0x00, 0x00, 0x00]
        data, sw1, sw2 = transmit_and_log(connection, apdu, "SELECT with 3-byte FID")
        assert [sw1, sw2] == SW_WRONG_LENGTH, \
            f"Expected 6700 (wrong length), got {sw1:02X}{sw2:02X}"


# =============================================================================
# SELECT FCI Response Tests
# =============================================================================

class TestSelectFCIResponse:
    """Test SELECT FCI/FCP response content."""

    def test_fci_contains_file_identifier(self, connection):
        """FCI should contain file identifier tag (83)."""
        data, sw1, sw2 = select_by_fid(connection, FID_MF, le=0x00)
        assert [sw1, sw2] == SW_OK
        
        if len(data) > 0:
            fci = parse_fci(data)
            assert 'fid' in fci, "FCI should contain file identifier"
            print(f"  File ID: {toHexString(fci['fid'])}")

    def test_fci_contains_descriptor(self, connection):
        """FCI should contain file descriptor tag (82)."""
        data, sw1, sw2 = select_by_fid(connection, FID_MF, le=0x00)
        assert [sw1, sw2] == SW_OK
        
        if len(data) > 0:
            fci = parse_fci(data)
            assert 'descriptor' in fci, "FCI should contain file descriptor"
            print(f"  Descriptor: {toHexString(fci['descriptor'])}")

    def test_fci_contains_lcs(self, connection):
        """FCI should contain life cycle status tag (8A)."""
        data, sw1, sw2 = select_by_fid(connection, FID_MF, le=0x00)
        assert [sw1, sw2] == SW_OK
        
        if len(data) > 0:
            fci = parse_fci(data)
            assert 'lcs' in fci, "FCI should contain life cycle status"
            # 0x05 = activated
            assert fci['lcs'] == 0x05, f"Expected LCS=05 (activated), got {fci['lcs']:02X}"


# =============================================================================
# Main
# =============================================================================

if __name__ == "__main__":
    pytest.main([__file__, "-v", "-s"])
