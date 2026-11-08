## 1. Build and flash this branch
On your machine (with the repo and this branch checked out):
```bash
cd /Users/jclaus/dev/flipperzero-firmware
git checkout ISSUE-4106/fix-nfc-mifare-ultralight-card   # if not already
./fbt
Connect the Flipper over USB (powered on, normal firmware running), then:

./fbt flash_usb
Wait until the Flipper reboots. You're now running the fixed firmware.

2. Test with NFC app (normal read)
Open NFC — Main menu → NFC.
Start a read — Read (or Scan / Detect, depending on menu wording).
Hold the card — Put the Mifare Ultralight or NTAG213 flat on the back of the Flipper and keep it still.
Expected with the fix — After a short "Don't move" phase, you get a success (and optionally "Save"). No endless "Don't move" even if the card is one that used to get stuck (e.g. signature read failing).
If you have a card that used to get stuck on official/unfixed firmware, that's the main case to retest: it should now complete instead of hanging.

3. What to check
Behavior — Read finishes and shows success (and you can save the dump). No infinite "Don't move".
Saved dump — Open the saved file: UID, type (e.g. NTAG213), and page data should look correct. Signature may be all zeros if that read failed (that's expected with the fix).
Optional – debug log — If you use ./fbt debug or a serial/debug console and enable debug logs, you may see:
Read signature failed, continuing without signature when the fix path is used.
Then the read continues and completes instead of going to "Read Failed".
4. Optional: compare with unfixed firmware
To see the old bug and confirm the fix:

Flash stock/unfixed firmware (e.g. dev or latest release):

git stash
git checkout dev
./fbt flash_usb
Try the same card that used to get stuck: run NFC → Read, hold card. If it still reproduces, you'll see it hang on "Don't move".

Flash your branch again:

git checkout ISSUE-4106/fix-nfc-mifare-ultralight-card
git stash pop   # if you stashed
./fbt flash_usb
Retest the same card — read should now complete.

5. Testing multiple devices/cards
Several Flippers — Repeat "build once → ./fbt flash_usb" for each device (plug one at a time, or use ./fbt flash_usb -p /dev/cu.usbmodem* to pick a port if needed).
Several cards — Use at least:
One Mifare Ultralight (or compatible).
One NTAG213 (or NTAG215/216).
If you have one that used to get stuck (e.g. ATQA 0x0044, SAK 0x00), that's the most important to test.
6. Quick checklist
Step	Action
1	./fbt then ./fbt flash_usb on branch ISSUE-4106/fix-nfc-mifare-ultralight-card
2	NFC → Read, hold Ultralight/NTAG213 until read finishes
3	Confirm success (no endless "Don't move"), save and open dump
4	(Optional) Flash dev, reproduce hang, then re-flash branch and confirm fix