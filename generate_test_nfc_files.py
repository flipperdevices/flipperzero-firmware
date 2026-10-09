#!/usr/bin/env python3
"""
NFC Test File Generator for Mifare Classic Value Block Testing

This script generates test NFC files specifically designed to exercise
the value block write path in the Mifare Classic poller, which was the
source of the BusFault bug (Issue #4108).

Usage:
    python3 generate_test_nfc.py [output_dir]

This creates MFC test files with value blocks that can be used to verify
the fix works correctly.
"""

import os
import sys
from datetime import datetime

def generate_mf_classic_test_file(filename, card_type="MFC4k"):
    """
    Generate a test Mifare Classic NFC file
    
    The FlipperFormat for MFC is:
    ```
    Filetype: Flipper NFC device
    Version: 4
    Device type: Mifare Classic
    UID: [UID bytes]
    ATQA: [ATQA bytes]
    SAK: [SAK byte]
    Mifare Classic type: [1K|4K]
    Data format version: 2
    Block 0: [block data]
    Block 1: [block data]
    ... (all blocks)
    ```
    """
    
    # MFC test data: a simple 1K card with known data
    test_data = {
        "MFC1k": {
            "uid": "BA E2 7C 9D",
            "atqa": "00 02",
            "sak": "18",
            "type": "1K",
            "blocks": 64  # 1K = 64 blocks
        },
        "MFC4k": {
            "uid": "DE AD BE EF",
            "atqa": "00 02",
            "sak": "18",
            "type": "4K",
            "blocks": 256  # 4K = 256 blocks
        }
    }
    
    config = test_data.get(card_type, test_data["MFC4k"])
    
    lines = [
        "Filetype: Flipper NFC device",
        "Version: 4",
        "Device type: Mifare Classic",
        f"UID: {config['uid']}",
        f"ATQA: {config['atqa']}",
        f"SAK: {config['sak']}",
        f"Mifare Classic type: {config['type']}",
        "Data format version: 2",
    ]
    
    # Add block data
    # Block 0: UID and manufacturer data (read-only)
    lines.append("Block 0: BA E2 7C 9D B9 18 02 00 46 44 53 37 30 56 30 31")
    
    # Blocks 1-3: Sector 0 data blocks (writable)
    lines.append("Block 1: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00")
    lines.append("Block 2: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00")
    
    # Block 3: Sector trailer (access control)
    lines.append("Block 3: FF FF FF FF FF FF FF 07 80 69 FF FF FF FF FF FF")
    
    # Add remaining blocks with value block data in appropriate sectors
    for block_num in range(4, config['blocks']):
        sector = block_num // 4
        block_in_sector = block_num % 4
        
        if block_in_sector == 3:
            # Sector trailer (every 4th block starting at 3)
            lines.append(f"Block {block_num}: FF FF FF FF FF FF FF 07 80 69 FF FF FF FF FF FF")
        elif sector >= 16 and block_in_sector < 3:
            # Value block data (typically sectors 16-31)
            # Value blocks have special structure: value (4 bytes) + inverted value (4 bytes) + block number + inverted block number
            value_hex = "E8 03 00 00"  # Example: 1000 in little-endian
            value_inv = "17 FC FF FF"  # Bitwise NOT of value
            block_num_byte = f"{block_num:02X}"
            block_num_inv = f"{(~block_num) & 0xFF:02X}"
            value_line = f"{value_hex} {value_inv} {block_num_byte} {block_num_inv} {block_num_byte} {block_num_inv}"
            # Pad to 16 bytes
            value_line += " 00" * ((16 - len(value_line.split())) % 16)
            lines.append(f"Block {block_num}: {value_line[:47]}")
        else:
            # Regular data blocks
            lines.append(f"Block {block_num}: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00")
    
    return "\n".join(lines)

def main():
    output_dir = sys.argv[1] if len(sys.argv) > 1 else "."
    
    # Create output directory if it doesn't exist
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    print("Generating NFC test files for value block testing...")
    print(f"Output directory: {os.path.abspath(output_dir)}")
    print()
    
    # Generate test files
    test_files = [
        ("test_mfc_1k.nfc", "MFC1k", "MFC 1K test card"),
        ("test_mfc_4k.nfc", "MFC4k", "MFC 4K test card with value blocks"),
    ]
    
    for filename, card_type, description in test_files:
        filepath = os.path.join(output_dir, filename)
        content = generate_mf_classic_test_file(filename, card_type)
        
        with open(filepath, 'w') as f:
            f.write(content)
        
        print(f"✅ Generated: {filename}")
        print(f"   Type: {description}")
        print(f"   Path: {filepath}")
        print(f"   Size: {len(content)} bytes")
        print()
    
    print("=" * 60)
    print("Test files generated successfully!")
    print()
    print("To use these files on Flipper Zero:")
    print("1. Copy to Flipper SD card: /nfc/")
    print("2. Open NFC app")
    print("3. Select Saved NFC → choose test file")
    print("4. Try 'Write to Initial Card' with an actual card")
    print("5. Verify no BusFault crash occurs")
    print()
    print("Files to test with:")
    for filename, _, desc in test_files:
        print(f"  - {filename} ({desc})")

if __name__ == "__main__":
    main()
