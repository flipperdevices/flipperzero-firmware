# This file ensures the client directory (Python test scripts) 
# is not included in the Flipper Zero application build.
# The build system will skip directories without proper .fam files
# when they don't contain C source files to compile.

# Note: This directory contains Python test scripts for testing
# the CCID implementation from a host computer.
#
# ---------------------------------------------------------------------------
# IMPORTANT: two mutually-exclusive test layers
# ---------------------------------------------------------------------------
# ccid_test.py talks to the device at the raw USB level (pyusb). It must have
# EXCLUSIVE access to the USB device, so pcscd must be STOPPED before running
# it, otherwise pyusb detaches the interface from pcscd, fails to claim it,
# and leaves the reader in a wedged state that only a hardware reset clears.
#
#   sudo systemctl stop pcscd pcscd.socket
#   python -m pytest ccid_test.py
#   sudo systemctl start pcscd.socket
#
# The PC/SC test suites (iso7816_test.py, select_test.py, get_data_test.py,
# verify_test.py) go through pcscd and REQUIRE it to be running. Do NOT run
# ccid_test.py in the same pytest session as the PC/SC suites.
