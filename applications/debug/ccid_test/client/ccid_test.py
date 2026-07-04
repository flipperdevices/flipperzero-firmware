#!/usr/bin/env python3
"""
CCID protocol-level test suite for Flipper Zero USB CCID implementation.

This test suite tests the CCID protocol layer directly using raw USB communication,
as opposed to the APDU-level tests which use the PC/SC abstraction.

Requirements:
    pip install pyusb pytest

Usage:
    pytest ccid_test.py -v

Reference: USB CCID Specification v1.1
https://www.usb.org/sites/default/files/DWG_Smart-Card_CCID_Rev110.pdf
"""

import struct
from typing import Optional

import pytest

try:
    import usb.core
    import usb.util
except ImportError:
    pytest.skip("pyusb is required", allow_module_level=True)


# =============================================================================
# CCID Protocol Constants
# =============================================================================

USB_CLASS_CCID = 0x0B

# Bulk-OUT Messages (PC to Reader)
PC_TO_RDR_ICCPOWERON = 0x62
PC_TO_RDR_ICCPOWEROFF = 0x63
PC_TO_RDR_GETSLOTSTATUS = 0x65
PC_TO_RDR_XFRBLOCK = 0x6F
PC_TO_RDR_GETPARAMETERS = 0x6C
PC_TO_RDR_RESETPARAMETERS = 0x6D
PC_TO_RDR_SETPARAMETERS = 0x61
PC_TO_RDR_ESCAPE = 0x6B
PC_TO_RDR_ICCCLOCK = 0x6E
PC_TO_RDR_T0APDU = 0x6A
PC_TO_RDR_SECURE = 0x69
PC_TO_RDR_MECHANICAL = 0x71
PC_TO_RDR_ABORT = 0x72
PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY = 0x73

# Bulk-IN Messages (Reader to PC)
RDR_TO_PC_DATABLOCK = 0x80
RDR_TO_PC_SLOTSTATUS = 0x81
RDR_TO_PC_PARAMETERS = 0x82

# Slot Status - ICC Status
CCID_ICCSTATUS_PRESENTANDACTIVE = 0x00

# Slot Status - Command Status
CCID_COMMANDSTATUS_NOERROR = 0x00
CCID_COMMANDSTATUS_FAILED = 0x40

# Error Codes
CCID_ERROR_CMD_NOT_SUPPORTED = 0x01
CCID_ERROR_BAD_LENGTH = 0x02
CCID_ERROR_BAD_SLOT = 0x05
CCID_ERROR_SLOTNOTFOUND = 0x05
CCID_ERROR_BAD_ABRFU_3B = 0x0A
CCID_ERROR_BAD_ABRFU_2B = 0x0D

# Control Requests
CCID_GET_CLOCK_FREQUENCIES = 0x02
CCID_GET_DATA_RATES = 0x03

# VID/PID for Flipper CCID
FLIPPER_VID = 0x076B
FLIPPER_PID = 0x3A21


# =============================================================================
# Message Builders
# =============================================================================

def build_icc_power_on(slot: int, seq: int, power_select: int = 0,
                       rfu1: int = 0, rfu2: int = 0) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_ICCPOWERON, 0, slot, seq, 0)
    body = struct.pack('<BBB', power_select, rfu1, rfu2)
    return header + body


def build_icc_power_off(slot: int, seq: int,
                        rfu0: int = 0, rfu1: int = 0, rfu2: int = 0) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_ICCPOWEROFF, 0, slot, seq, 0)
    body = struct.pack('<BBB', rfu0, rfu1, rfu2)
    return header + body


def build_get_slot_status(slot: int, seq: int,
                          rfu0: int = 0, rfu1: int = 0, rfu2: int = 0) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_GETSLOTSTATUS, 0, slot, seq, 0)
    body = struct.pack('<BBB', rfu0, rfu1, rfu2)
    return header + body


def build_get_parameters(slot: int, seq: int,
                         rfu0: int = 0, rfu1: int = 0, rfu2: int = 0) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_GETPARAMETERS, 0, slot, seq, 0)
    body = struct.pack('<BBB', rfu0, rfu1, rfu2)
    return header + body


def build_reset_parameters(slot: int, seq: int,
                           rfu0: int = 0, rfu1: int = 0, rfu2: int = 0) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_RESETPARAMETERS, 0, slot, seq, 0)
    body = struct.pack('<BBB', rfu0, rfu1, rfu2)
    return header + body


def build_set_parameters_t0(slot: int, seq: int,
                            rfu1: int = 0, rfu2: int = 0,
                            fi_di: int = 0x11, guard_time: int = 0,
                            waiting_int: int = 0x0A, clock_stop: int = 0) -> bytes:
    param_len = 5
    header = struct.pack('<BIBBB', PC_TO_RDR_SETPARAMETERS, param_len, slot, seq, 0)
    body = struct.pack('<BBB', 0x00, rfu1, rfu2)
    params = struct.pack('<BBBBB', fi_di, guard_time, waiting_int, clock_stop, 0)
    return header + body + params


def build_xfr_block(slot: int, seq: int, data: bytes, bwi: int = 0) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_XFRBLOCK, len(data), slot, seq, 0)
    body = struct.pack('<BBB', bwi, 0, 0)
    return header + body + data


def build_xfr_block_raw(slot: int, seq: int, claimed_len: int, data: bytes) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_XFRBLOCK, claimed_len, slot, seq, 0)
    body = struct.pack('<BBB', 0, 0, 0)
    return header + body + data


def build_escape(slot: int, seq: int) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_ESCAPE, 0, slot, seq, 0)
    body = struct.pack('<BBB', 0, 0, 0)
    return header + body


def build_icc_clock(slot: int, seq: int) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_ICCCLOCK, 0, slot, seq, 0)
    body = struct.pack('<BBB', 0, 0, 0)
    return header + body


def build_t0_apdu(slot: int, seq: int) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_T0APDU, 0, slot, seq, 0)
    body = struct.pack('<BBB', 0, 0, 0)
    return header + body


def build_secure(slot: int, seq: int) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_SECURE, 0, slot, seq, 0)
    body = struct.pack('<BH', 0, 0)
    return header + body


def build_mechanical(slot: int, seq: int) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_MECHANICAL, 0, slot, seq, 0)
    body = struct.pack('<BBB', 0, 0, 0)
    return header + body


def build_abort(slot: int, seq: int) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_ABORT, 0, slot, seq, 0)
    body = struct.pack('<BBB', 0, 0, 0)
    return header + body


def build_set_data_rate_and_clock(slot: int, seq: int) -> bytes:
    header = struct.pack('<BIBBB', PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY, 8, slot, seq, 0)
    body = struct.pack('<BBB', 0, 0, 0)
    data = struct.pack('<II', 0, 0)
    return header + body + data


# =============================================================================
# Response Parser
# =============================================================================

def parse_response(data: bytes) -> dict:
    if len(data) < 10:
        return {"error": "Response too short"}

    msg_type = data[0]
    length = struct.unpack('<I', data[1:5])[0]
    result = {
        "msg_type": msg_type,
        "length": length,
        "slot": data[5],
        "seq": data[6],
        "status": data[7],
        "icc_status": data[7] & 0x03,
        "command_status": data[7] & 0xC0,
        "error": data[8],
        "chain_param": data[9],
    }
    if length > 0 and len(data) > 10:
        result["data"] = data[10:10+length]
    return result


# =============================================================================
# CCID Device Fixture
# =============================================================================

class CCIDDevice:
    def __init__(self, dev, ep_out, ep_in):
        self.dev = dev
        self.ep_out = ep_out
        self.ep_in = ep_in
        self._seq = 0

    def next_seq(self) -> int:
        seq = self._seq
        self._seq = (self._seq + 1) % 256
        return seq

    def send_receive(self, data: bytes, timeout: int = 5000) -> Optional[dict]:
        try:
            self.ep_out.write(data, timeout)
            response = self.ep_in.read(2048, timeout)
            return parse_response(bytes(response))
        except usb.core.USBError as e:
            pytest.fail(f"USB error: {e}")

    def control_request(self, request: int, length: int = 64) -> Optional[bytes]:
        try:
            data = self.dev.ctrl_transfer(0xA1, request, 0, 0, length)
            return bytes(data)
        except usb.core.USBError as e:
            pytest.fail(f"Control request error: {e}")


@pytest.fixture(scope="module")
def ccid():
    dev = usb.core.find(idVendor=FLIPPER_VID, idProduct=FLIPPER_PID)
    if dev is None:
        pytest.skip(f"Device {FLIPPER_VID:04X}:{FLIPPER_PID:04X} not found")

    try:
        if dev.is_kernel_driver_active(0):
            dev.detach_kernel_driver(0)
    except (usb.core.USBError, NotImplementedError):
        pass

    try:
        dev.set_configuration()
    except usb.core.USBError:
        pass

    cfg = dev.get_active_configuration()
    intf = None
    for i in cfg:
        if i.bInterfaceClass == USB_CLASS_CCID:
            intf = i
            break

    if intf is None:
        pytest.skip("No CCID interface found")

    ep_out = ep_in = None
    for ep in intf:
        if usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_OUT:
            ep_out = ep
        elif usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_IN:
            if ep.bmAttributes & 0x03 == usb.util.ENDPOINT_TYPE_BULK:
                ep_in = ep

    if ep_out is None or ep_in is None:
        pytest.skip("Could not find bulk endpoints")

    device = CCIDDevice(dev, ep_out, ep_in)
    yield device
    usb.util.dispose_resources(dev)


# =============================================================================
# Core Functionality Tests
# =============================================================================

class TestIccPowerOn:
    def test_response_type(self, ccid):
        resp = ccid.send_receive(build_icc_power_on(0, ccid.next_seq()))
        assert resp["msg_type"] == RDR_TO_PC_DATABLOCK

    def test_command_succeeds(self, ccid):
        resp = ccid.send_receive(build_icc_power_on(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_NOERROR

    def test_icc_active(self, ccid):
        resp = ccid.send_receive(build_icc_power_on(0, ccid.next_seq()))
        assert resp["icc_status"] == CCID_ICCSTATUS_PRESENTANDACTIVE

    def test_atr_returned(self, ccid):
        resp = ccid.send_receive(build_icc_power_on(0, ccid.next_seq()))
        assert "data" in resp and len(resp["data"]) >= 2

    def test_atr_format(self, ccid):
        resp = ccid.send_receive(build_icc_power_on(0, ccid.next_seq()))
        assert resp["data"][0] == 0x3B


class TestIccPowerOff:
    def test_response_type(self, ccid):
        resp = ccid.send_receive(build_icc_power_off(0, ccid.next_seq()))
        assert resp["msg_type"] == RDR_TO_PC_SLOTSTATUS

    def test_command_succeeds(self, ccid):
        resp = ccid.send_receive(build_icc_power_off(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_NOERROR


class TestGetSlotStatus:
    def test_response_type(self, ccid):
        resp = ccid.send_receive(build_get_slot_status(0, ccid.next_seq()))
        assert resp["msg_type"] == RDR_TO_PC_SLOTSTATUS

    def test_command_succeeds(self, ccid):
        resp = ccid.send_receive(build_get_slot_status(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_NOERROR

    def test_clock_status_valid(self, ccid):
        resp = ccid.send_receive(build_get_slot_status(0, ccid.next_seq()))
        assert resp["chain_param"] in [0, 1, 2]


class TestGetParameters:
    def test_response_type(self, ccid):
        ccid.send_receive(build_icc_power_on(0, ccid.next_seq()))
        resp = ccid.send_receive(build_get_parameters(0, ccid.next_seq()))
        assert resp["msg_type"] == RDR_TO_PC_PARAMETERS

    def test_command_succeeds(self, ccid):
        resp = ccid.send_receive(build_get_parameters(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_NOERROR

    def test_t0_params_length(self, ccid):
        resp = ccid.send_receive(build_get_parameters(0, ccid.next_seq()))
        if "data" in resp:
            assert len(resp["data"]) == 5


class TestResetParameters:
    def test_response_type(self, ccid):
        resp = ccid.send_receive(build_reset_parameters(0, ccid.next_seq()))
        assert resp["msg_type"] == RDR_TO_PC_PARAMETERS

    def test_command_succeeds(self, ccid):
        resp = ccid.send_receive(build_reset_parameters(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_NOERROR


class TestSetParameters:
    def test_response_type(self, ccid):
        resp = ccid.send_receive(build_set_parameters_t0(0, ccid.next_seq()))
        assert resp["msg_type"] == RDR_TO_PC_PARAMETERS


class TestSequenceNumbers:
    def test_sequence_echoed(self, ccid):
        resp = ccid.send_receive(build_get_slot_status(0, 0x42))
        assert resp["seq"] == 0x42

    def test_sequence_0xff_echoed(self, ccid):
        resp = ccid.send_receive(build_get_slot_status(0, 0xFF))
        assert resp["seq"] == 0xFF


class TestPowerCycle:
    def test_power_on_succeeds(self, ccid):
        resp = ccid.send_receive(build_icc_power_on(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_NOERROR
        assert resp["icc_status"] == CCID_ICCSTATUS_PRESENTANDACTIVE

    def test_power_off_succeeds(self, ccid):
        resp = ccid.send_receive(build_icc_power_off(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_NOERROR

    def test_icc_active_after_power_off(self, ccid):
        ccid.send_receive(build_icc_power_off(0, ccid.next_seq()))
        resp = ccid.send_receive(build_get_slot_status(0, ccid.next_seq()))
        assert resp["icc_status"] == CCID_ICCSTATUS_PRESENTANDACTIVE

    def test_re_power_on_succeeds(self, ccid):
        ccid.send_receive(build_icc_power_off(0, ccid.next_seq()))
        resp = ccid.send_receive(build_icc_power_on(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_NOERROR


class TestInvalidSlot:
    def test_command_fails(self, ccid):
        resp = ccid.send_receive(build_get_slot_status(99, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED

    def test_error_slotnotfound(self, ccid):
        resp = ccid.send_receive(build_get_slot_status(99, ccid.next_seq()))
        assert resp["error"] in [CCID_ERROR_SLOTNOTFOUND, CCID_ERROR_BAD_SLOT]


class TestUnsupportedCommands:
    def test_escape_not_supported(self, ccid):
        resp = ccid.send_receive(build_escape(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_CMD_NOT_SUPPORTED

    def test_icc_clock_not_supported(self, ccid):
        resp = ccid.send_receive(build_icc_clock(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_CMD_NOT_SUPPORTED

    def test_t0_apdu_not_supported(self, ccid):
        resp = ccid.send_receive(build_t0_apdu(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_CMD_NOT_SUPPORTED

    def test_secure_not_supported(self, ccid):
        resp = ccid.send_receive(build_secure(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_CMD_NOT_SUPPORTED

    def test_mechanical_not_supported(self, ccid):
        resp = ccid.send_receive(build_mechanical(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_CMD_NOT_SUPPORTED

    def test_abort_not_supported(self, ccid):
        resp = ccid.send_receive(build_abort(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_CMD_NOT_SUPPORTED

    def test_set_data_rate_not_supported(self, ccid):
        resp = ccid.send_receive(build_set_data_rate_and_clock(0, ccid.next_seq()))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_CMD_NOT_SUPPORTED


class TestControlRequests:
    def test_get_clock_frequencies(self, ccid):
        data = ccid.control_request(CCID_GET_CLOCK_FREQUENCIES, length=4)
        assert data is not None and len(data) >= 4
        clock = struct.unpack('<I', data[:4])[0]
        assert clock > 0

    def test_get_data_rates(self, ccid):
        data = ccid.control_request(CCID_GET_DATA_RATES, length=4)
        assert data is not None and len(data) >= 4
        rate = struct.unpack('<I', data[:4])[0]
        assert rate > 0


class TestRFUValidation:
    def test_icc_power_on_bad_rfu(self, ccid):
        resp = ccid.send_receive(build_icc_power_on(0, ccid.next_seq(), rfu1=0xFF, rfu2=0xFF))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_BAD_ABRFU_2B

    def test_get_slot_status_bad_rfu(self, ccid):
        resp = ccid.send_receive(build_get_slot_status(0, ccid.next_seq(), 0x01, 0x02, 0x03))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_BAD_ABRFU_3B

    def test_icc_power_off_bad_rfu(self, ccid):
        resp = ccid.send_receive(build_icc_power_off(0, ccid.next_seq(), 0xAA, 0xBB, 0xCC))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_BAD_ABRFU_3B

    def test_get_parameters_bad_rfu(self, ccid):
        resp = ccid.send_receive(build_get_parameters(0, ccid.next_seq(), 0x11, 0x22, 0x33))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_BAD_ABRFU_3B

    def test_reset_parameters_bad_rfu(self, ccid):
        resp = ccid.send_receive(build_reset_parameters(0, ccid.next_seq(), 0xDE, 0xAD, 0xBE))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_BAD_ABRFU_3B

    def test_set_parameters_bad_rfu(self, ccid):
        resp = ccid.send_receive(build_set_parameters_t0(0, ccid.next_seq(), rfu1=0xFF, rfu2=0xFF))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_BAD_ABRFU_2B


class TestLengthValidation:
    def test_xfr_block_length_overflow(self, ccid):
        resp = ccid.send_receive(build_xfr_block_raw(0, ccid.next_seq(), 0xFFFF, b''))
        assert resp["command_status"] == CCID_COMMANDSTATUS_FAILED
        assert resp["error"] == CCID_ERROR_BAD_LENGTH

    def test_xfr_block_large_valid_data(self, ccid):
        ccid.send_receive(build_icc_power_on(0, ccid.next_seq()))
        data_len = 200
        apdu_data = bytes(range(data_len))
        apdu = bytes([0x01, 0x04, 0x00, 0x00, data_len]) + apdu_data + bytes([data_len])
        resp = ccid.send_receive(build_xfr_block(0, ccid.next_seq(), apdu))
        assert resp["msg_type"] == RDR_TO_PC_DATABLOCK
        assert resp["command_status"] == CCID_COMMANDSTATUS_NOERROR


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
