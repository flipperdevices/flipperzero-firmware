# MIFARE Classic Editor

An application for viewing and editing MIFARE Classic .nfc files without access to an external device. Supports all card types supported by the Flipper: 0.3K (Mini), 1K, and 4K - with both 4 and 7 byte UIDs.

## Features

- Separation of card data by sector then by block
- Editing the raw block contents of a card
- Separate editing for special parts of the card (such as the UID and keys)
- A block-by-block access condition decoder and editor
- A check for the validity of the stored BCC byte and an option to correct it if it does not match the expected value (applies to 4 byte UID cards only)

## Notes

- If a card has been modified during emulation, thereby giving it a corresponding .shd file, the application will give you an option of which to edit. While a .shd file exists, that is what will be read by the main NFC application.
- If the access condition check bits for a block are invalid, the application will display as such when viewing them, and correct them when they are edited.
  - The values in brackets in the access bit viewer correspond to the expected value of each bit according to the check bits. They should be equal to the bit value that they follow.
  - Keep in mind that if the check bits for a block are incorrect, it will disable the **entire sector** on a real card. This is not fully reflected by the decoded view, as it treats each block individually.
- If Key B is readable in the sector trailer (this is the case in the default configuration), then it cannot be used to authenticate anything in the sector no matter what the access bits of each block are. This is not reflected in the decoded view.
