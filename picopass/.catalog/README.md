# Picopass


This application allows you to read, write, save, and emulate legacy HID iClass cards and fobs (based on the picopass chipset).  Also supports saving the credential to the Flipper Zero LFRFID data format, changing the keys on the card, performing dictionary attack, and performing the 'online' part of the loclass attack.

# Loclass

The loclass attack emulates specific CSN and collects responses from the reader which can be used to calculate the elite or (some) custom key configured for that reader.  This key is then used to read data on the cards used with that reader.

## Online part

1. Run _loclass_ from the picopass main menu
2. Present the flipper to the reader.  Holding flipper directly to reader may not work, vary distance by a few inches.
3. Collect responses until the progress bar is full.

NOTE: If the screen says “Got std key” AND stays on 0/18, then loclass isn't needed.

## Offline part

1. Download the loclass log (_sdcard/apps_data/picopass/.loclass.log_) from your Flipper Zero.
2. Use [loclass.ericbetts.dev](https://loclass.ericbetts.dev/) or a tool of your choice to calculate the key
3. Copy the key to _iclass_elite_dict_user.txt_ and place in _sdcard/apps_data/picopass/assets/_
4. Run _Elite Dict. Attack_ from the picopass main menu
5. Present card to the back of the Flipper Zero.

## Failure

There are some situations when the offline loclass may not find a key, such as:
 * iClass SE
 * Readers configured with Standard-2 keyset
 * Custom keyed readers using Standard KDF
 * Custom keyed readers using SE KDF

# NR-MAC read

Due to the nature of how secure picopass works, it is possible to emulate some public fields from a card and capture the reader's response, which can be used to authenticate.  Two of the pieces involved in this are the NR and MAC.

These instructions are intended to be performed all at the same time.  If you use the card with the reader between Card Part 1 and Card Part 2, then Card Part 2 will fail.

## Card Part 1

1. Place card against Flipper Zero
2. Run _Read_ from the picopass main menu
3. Get a "Read Failed" message
4. Select the "Menu" option
5. Select "Save Partial"
6. Name file something you'll remember

## Reader Part

1. Select _Saved_ from the picopass main menu
2. Select the file name you saved in last step of Card Part 1
3. Select _Emulate_
4. Expose Flipper Zero to reader (It may work better a few inches from the reader, as opposed to physically touching)
5. Flipper will buzz and screen will say "NR-MAC Saved!"

## Card Part 2

1. Place card against Flipper Zero
2. Run _Read_ from the picopass main menu
3. Card will authenticate and read

