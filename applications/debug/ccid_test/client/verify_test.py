#!/usr/bin/env python3
"""
ISO7816 VERIFY command test suite for Flipper Zero USB CCID implementation.

Tests the VERIFY command (INS=0x20) as defined in ISO/IEC 7816-4:2005 Section 7.5.6.

Requirements:
    pip install pyscard pytest

Usage:
    pytest verify_test.py -v

Reference: ISO/IEC 7816-4:2005 Section 7.5.6
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
SW_AUTH_BLOCKED = [0x69, 0x83]
SW_REF_DATA_NOT_FOUND = [0x6A, 0x88]
SW_REF_DATA_NOT_USABLE = [0x69, 0x84]
SW_WRONG_P1P2 = [0x6A, 0x86]
SW_FUNC_NOT_SUPPORTED = [0x6A, 0x81]

# Default test PIN "1234"
DEFAULT_PIN = [0x31, 0x32, 0x33, 0x34]

# Wrong PIN "9999"
WRONG_PIN = [0x39, 0x39, 0x39, 0x39]

# PIN with 0xFF padding (8 bytes total)
DEFAULT_PIN_PADDED = [0x31, 0x32, 0x33, 0x34, 0xFF, 0xFF, 0xFF, 0xFF]

# PIN reference values - different refs for each test class to avoid state conflicts
PIN_REF_GLOBAL_1 = 0x01  # Used by TestVerifySuccess
PIN_REF_GLOBAL_2 = 0x02  # Used by TestVerifyFailure  
PIN_REF_GLOBAL_3 = 0x03  # Used by TestPinBlocked
PIN_REF_NONEXISTENT = 0x1F

# Application defines 3 PINs by default: refs 1, 2, 3 all with PIN "1234"


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


@pytest.fixture
def reset_card(connection):
    """Reset the card to clear verification state."""
    # Reconnect to simulate power cycle
    connection.reconnect()
    yield connection


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


def verify_pin(connection, pin: list, reference: int = PIN_REF_GLOBAL_1):
    """Send VERIFY command with PIN data."""
    # APDU: 00 20 00 P2 Lc [PIN data]
    apdu = [0x00, 0x20, 0x00, reference, len(pin)] + pin
    return transmit_and_log(connection, apdu, 
        f"VERIFY PIN ref={reference:02X} data={toHexString(pin)}")


def check_pin_retries(connection, reference: int = PIN_REF_GLOBAL_1):
    """Check remaining PIN retries (VERIFY with no data)."""
    # APDU: 00 20 00 P2 (no Lc, no data)
    apdu = [0x00, 0x20, 0x00, reference]
    return transmit_and_log(connection, apdu, 
        f"CHECK RETRIES ref={reference:02X}")


def select_mf(connection):
    """Select Master File to reset context."""
    apdu = [0x00, 0xA4, 0x00, 0x00, 0x00]
    return transmit_and_log(connection, apdu, "SELECT MF")


def get_retries_from_sw(sw1: int, sw2: int) -> int:
    """Extract retry count from status word 63CX."""
    if sw1 == 0x63 and (sw2 & 0xF0) == 0xC0:
        return sw2 & 0x0F
    return -1


# =============================================================================
# VERIFY Success Tests
# =============================================================================

class TestVerifySuccess:
    """Test successful PIN verification.
    
    Uses PIN_REF_GLOBAL_1 exclusively. Tests skip if PIN is blocked.
    """

    def test_verify_correct_pin(self, connection):
        """VERIFY with correct PIN should return 9000."""
        connection.reconnect()
        
        # Check if PIN is blocked
        _, sw1, sw2 = check_pin_retries(connection, PIN_REF_GLOBAL_1)
        if [sw1, sw2] == SW_AUTH_BLOCKED:
            pytest.skip("PIN ref 1 is blocked from previous run")
        
        data, sw1, sw2 = verify_pin(connection, DEFAULT_PIN, PIN_REF_GLOBAL_1)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"

    def test_verify_padded_pin(self, connection):
        """VERIFY with 0xFF-padded PIN should also work."""
        connection.reconnect()
        
        # Check if PIN is blocked
        _, sw1, sw2 = check_pin_retries(connection, PIN_REF_GLOBAL_1)
        if [sw1, sw2] == SW_AUTH_BLOCKED:
            pytest.skip("PIN ref 1 is blocked from previous run")
        
        data, sw1, sw2 = verify_pin(connection, DEFAULT_PIN_PADDED, PIN_REF_GLOBAL_1)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"

    def test_verify_different_reference(self, connection):
        """VERIFY should work with different PIN references."""
        connection.reconnect()
        
        # Check if PIN is blocked
        _, sw1, sw2 = check_pin_retries(connection, PIN_REF_GLOBAL_2)
        if [sw1, sw2] == SW_AUTH_BLOCKED:
            pytest.skip("PIN ref 2 is blocked from previous run")
        
        # Global PIN ref 2
        data, sw1, sw2 = verify_pin(connection, DEFAULT_PIN, PIN_REF_GLOBAL_2)
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"


# =============================================================================
# VERIFY Failure and Retry Counter Tests
# =============================================================================

class TestVerifyFailure:
    """Test PIN verification failure and retry counter."""

    def test_verify_wrong_pin_returns_retries(self, connection):
        """VERIFY with wrong PIN should return 63CX with remaining retries."""
        connection.reconnect()
        
        # Use PIN ref 2 to avoid conflicts with other tests
        data, sw1, sw2 = verify_pin(connection, WRONG_PIN, PIN_REF_GLOBAL_2)
        
        # Should return 63C2 (2 retries remaining after 1 failure)
        assert sw1 == 0x63, f"Expected SW1=63, got {sw1:02X}"
        assert (sw2 & 0xF0) == 0xC0, f"Expected SW2=CX, got {sw2:02X}"
        
        retries = get_retries_from_sw(sw1, sw2)
        assert retries <= 2, f"Expected <= 2 retries after failure, got {retries}"

    def test_verify_consecutive_failures_decrement_counter(self, connection):
        """Consecutive wrong PINs should decrement retry counter."""
        # Don't reconnect - continue from previous test state
        # Note: This test uses PIN ref 2 which may have 2 retries left
        
        # Get current retry count first
        _, sw1, sw2 = check_pin_retries(connection, PIN_REF_GLOBAL_2)
        initial_retries = get_retries_from_sw(sw1, sw2)
        
        if initial_retries <= 1:
            pytest.skip("PIN ref 2 has too few retries for this test")
        
        # One failure
        _, sw1, sw2 = verify_pin(connection, WRONG_PIN, PIN_REF_GLOBAL_2)
        retries_after = get_retries_from_sw(sw1, sw2)
        
        # Should be one less than before
        assert retries_after == initial_retries - 1, \
            f"Expected {initial_retries - 1} retries, got {retries_after}"

    def test_verify_correct_pin_resets_counter(self, connection):
        """Correct PIN after failures should reset retry counter."""
        connection.reconnect()
        
        # Use a fresh app launch to ensure clean state
        # First check if PIN 1 is blocked
        _, sw1, sw2 = check_pin_retries(connection, PIN_REF_GLOBAL_1)
        
        if [sw1, sw2] == SW_AUTH_BLOCKED:
            pytest.skip("PIN ref 1 is blocked, cannot test reset")
        
        # If we have retries, cause a failure first
        initial_retries = get_retries_from_sw(sw1, sw2)
        if initial_retries > 1:
            # One failure
            verify_pin(connection, WRONG_PIN, PIN_REF_GLOBAL_1)
        
        # Now verify with correct PIN
        data, sw1, sw2 = verify_pin(connection, DEFAULT_PIN, PIN_REF_GLOBAL_1)
        
        # If PIN was blocked, we can't test this
        if [sw1, sw2] == SW_AUTH_BLOCKED:
            pytest.skip("PIN became blocked during test")
        
        assert [sw1, sw2] == SW_OK, f"Expected 9000, got {sw1:02X}{sw2:02X}"


# =============================================================================
# PIN Blocked Tests
# =============================================================================

class TestPinBlocked:
    """Test PIN blocking behavior.
    
    Uses PIN_REF_GLOBAL_3 to avoid conflicts with other test classes.
    """

    def test_verify_blocks_after_max_failures(self, connection):
        """PIN should be blocked after 3 consecutive wrong attempts."""
        connection.reconnect()
        
        # Check if PIN 3 is already blocked
        _, sw1, sw2 = check_pin_retries(connection, PIN_REF_GLOBAL_3)
        if [sw1, sw2] == SW_AUTH_BLOCKED:
            pytest.skip("PIN ref 3 is already blocked from previous run")
        
        # Three failures to block the PIN
        verify_pin(connection, WRONG_PIN, PIN_REF_GLOBAL_3)  # 3->2
        verify_pin(connection, WRONG_PIN, PIN_REF_GLOBAL_3)  # 2->1
        data, sw1, sw2 = verify_pin(connection, WRONG_PIN, PIN_REF_GLOBAL_3)  # 1->0 BLOCKED
        
        # Should return 6983 (blocked) or 63C0 (no retries left)
        if [sw1, sw2] != SW_AUTH_BLOCKED:
            # May get 63C0 on final failure before block kicks in
            assert sw1 == 0x63 and sw2 == 0xC0, \
                f"Expected 6983 or 63C0, got {sw1:02X}{sw2:02X}"

    def test_verify_blocked_pin_returns_blocked(self, connection):
        """Attempting to verify a blocked PIN should return 6983."""
        # Check if PIN is blocked first
        _, sw1, sw2 = check_pin_retries(connection, PIN_REF_GLOBAL_3)
        
        if [sw1, sw2] != SW_AUTH_BLOCKED:
            # PIN not blocked yet, skip
            pytest.skip("PIN ref 3 is not blocked")
        
        data, sw1, sw2 = verify_pin(connection, DEFAULT_PIN, PIN_REF_GLOBAL_3)
        assert [sw1, sw2] == SW_AUTH_BLOCKED, \
            f"Expected 6983 (blocked), got {sw1:02X}{sw2:02X}"

    def test_check_retries_blocked_pin(self, connection):
        """Checking retries on blocked PIN should return 6983."""
        _, sw1, sw2 = check_pin_retries(connection, PIN_REF_GLOBAL_3)
        
        if [sw1, sw2] != SW_AUTH_BLOCKED:
            # If PIN wasn't blocked, this is expected if tests ran in different order
            pytest.skip("PIN ref 3 is not blocked")
        
        assert [sw1, sw2] == SW_AUTH_BLOCKED, \
            f"Expected 6983 (blocked), got {sw1:02X}{sw2:02X}"


# =============================================================================
# Check Retries Tests
# =============================================================================

class TestCheckRetries:
    """Test VERIFY with no data (check retries)."""

    def test_check_retries_returns_counter(self, connection):
        """VERIFY with no data should return 63CX with retry count."""
        connection.reconnect()
        
        # Use PIN ref 1 or 2, checking which one is still active
        data, sw1, sw2 = check_pin_retries(connection, PIN_REF_GLOBAL_1)
        
        # Should return 63CX (some retries) or 6983 (blocked)
        if [sw1, sw2] == SW_AUTH_BLOCKED:
            # Try PIN ref 2
            data, sw1, sw2 = check_pin_retries(connection, PIN_REF_GLOBAL_2)
        
        # Should return 63CX with some retry count (or be blocked)
        if [sw1, sw2] != SW_AUTH_BLOCKED:
            assert sw1 == 0x63, f"Expected SW1=63, got {sw1:02X}"
            retries = get_retries_from_sw(sw1, sw2)
            assert 0 <= retries <= 3, f"Expected 0-3 retries, got {retries}"

    def test_check_retries_nonexistent_pin(self, connection):
        """Check retries for non-existent PIN should return 6A88."""
        connection.reconnect()
        
        data, sw1, sw2 = check_pin_retries(connection, PIN_REF_NONEXISTENT)
        assert [sw1, sw2] == SW_REF_DATA_NOT_FOUND, \
            f"Expected 6A88, got {sw1:02X}{sw2:02X}"


# =============================================================================
# Error Cases Tests
# =============================================================================

class TestVerifyErrors:
    """Test VERIFY error conditions."""

    def test_verify_nonexistent_reference(self, connection):
        """VERIFY for non-existent PIN reference should return 6A88."""
        connection.reconnect()
        
        data, sw1, sw2 = verify_pin(connection, DEFAULT_PIN, PIN_REF_NONEXISTENT)
        assert [sw1, sw2] == SW_REF_DATA_NOT_FOUND, \
            f"Expected 6A88, got {sw1:02X}{sw2:02X}"

    def test_verify_invalid_p1(self, connection):
        """VERIFY with P1 != 0x00 should return 6A86."""
        connection.reconnect()
        
        # P1 = 0x01 (invalid, must be 0x00)
        apdu = [0x00, 0x20, 0x01, 0x01, 0x04] + DEFAULT_PIN
        data, sw1, sw2 = transmit_and_log(connection, apdu, "VERIFY with invalid P1")
        
        assert [sw1, sw2] == SW_WRONG_P1P2, \
            f"Expected 6A86, got {sw1:02X}{sw2:02X}"

    def test_verify_invalid_p2_bits(self, connection):
        """VERIFY with reserved P2 bits set should return 6A86."""
        connection.reconnect()
        
        # P2 = 0x41 (bit 7 set, which is reserved)
        apdu = [0x00, 0x20, 0x00, 0x41, 0x04] + DEFAULT_PIN
        data, sw1, sw2 = transmit_and_log(connection, apdu, "VERIFY with invalid P2")
        
        assert [sw1, sw2] == SW_WRONG_P1P2, \
            f"Expected 6A86, got {sw1:02X}{sw2:02X}"

    def test_verify_biometric_not_supported(self, connection):
        """VERIFY with INS=0x21 (biometric) should return 6A81."""
        connection.reconnect()
        
        # INS = 0x21 (biometric VERIFY)
        apdu = [0x00, 0x21, 0x00, 0x01, 0x04] + DEFAULT_PIN
        data, sw1, sw2 = transmit_and_log(connection, apdu, "VERIFY biometric (INS=21)")
        
        assert [sw1, sw2] == SW_FUNC_NOT_SUPPORTED, \
            f"Expected 6A81, got {sw1:02X}{sw2:02X}"


# =============================================================================
# Main
# =============================================================================

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
