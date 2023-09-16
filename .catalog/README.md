# Picopass


This application allows you to read, write, save, and emulate legacy HID iClass cards and fobs (based on the picopass chipset).  Also supports saving the credential to the Flipper Zero LFRFID data format, changing the keys on the card, performing dictionary attack, and performing the 'online' part of the loclass attack.

NOTE: Does not support iClass SE

# Loclass

The loclass attack emulates specific CSN and collects responses from the reader which can be used to calculate the elite or custom key configured for that reader.  This key is then used to read data on the cards used with that reader.

## Online part

1. Run _loclass_ from the picopass main menu
2. Present the flipper to the reader.
3. Collect responses until the progress bar is full.

NOTE: If the screen says “Got std key” AND stays on 0/18, the reader is not elite or custom keyed.

## Offline part

1. Download the loclass log (_sdcard/apps_data/picopass/.loclass.log_) from your Flipper Zero.
2. Use [loclass.ericbetts.dev](https://loclass.ericbetts.dev/) or a tool of your choice to calculate the key
3. Copy the key to _iclass_elite_dict_user.txt_ and place in _sdcard/apps_data/picopass/_
4. Run _Elite Dict. Attack_ from the picopass main menu
5. Present card to the back of the Flipper Zero.
