# NFC Magic

This application allows you to check whether your NFC card is Magic, write data to Magic cards, and restore them to their original state.

## What's a "Magic" Card?

A Magic card is a card that allows you to change its UID. Generally, NFC cards have a UID pre-programmed at the factory, and it cannot be changed. However, some cards have a special backdoor feature that allows you to change the UID. These cards are called Magic cards.

## Supported Cards

Currently, not all Magic cards are supported. The following cards are supported:

* Mifare Classic 1K/4K Gen1A/Gen1B (including OTP versions)
* Ultimate Magic Card (Gen4 GTU)

For the Gen4 cards, both the Ultralight and the Classic modes are supported, with the ability to select a custom password.

The following cards are currently not supported:

* Mifare Classic 1K/4K Gen2 (DirectWrite/CUID/FUID/UFUID)
* Mifare Classic 1K/4K Gen3 (APDU)
* Mifare Classic Magic Gen4 (GDM)
* Supercard (Gen1 and Gen2)
* Mifare Ultralight Gen1A/Gen1B (including OTP versions)
* Mifare Ultralight DirectWrite
* Mifare Ultralight EV1 DirectWrite
* Mifare Ultralight C Gen1
* Mifare Ultralight C DirectWrite
* NTAG (all versions)
* DESFire (all versions)
* NFC-V/ISO15693 Magic Cards (all versions)
