# Rolling Flaws

Rolling Flaws (version 1.5) by [@CodeAllNight](https://twitter.com/codeallnight).

[YouTube demo](https://youtu.be/gMnGuDC9EQo?si=4HLZpkC4XWhh97uQ) of using Rolling Flaws application.  The video shows how to use the application to simulate a receiver that has a Replay attack flaw, Pairing FZ to a receiver, Cloning sequence attack, Future attack, Rollback attack & KGB attack.  The Rolling Flaws application also supports things like "ENC00" attack & window-next attacks, which are described in scenarios below but was not in video.  Rolljam is discussed in document, but discouraged to test since it is [illegal to jam signals](https://www.fcc.gov/general/jammer-enforcement) in the US.  If you have additional ideas, please let me know!

Note: This application works with Official firmware & most of the other firmware versions as well.  Xtreme firmware needs to be on DEV channel to build.

- Discord invite: [https://discord.com/invite/NsjCvqwPAd](https://discord.com/invite/NsjCvqwPAd)
- YouTube: [https://youtube.com/@MrDerekJamison](https://youtube.com/@MrDerekJamison)
- GitHub: [https://github.com/jamisonderek/flipper-zero-tutorials/blob/main/subghz/apps/rolling-flaws](https://github.com/jamisonderek/flipper-zero-tutorials/blob/main/subghz/apps/rolling-flaws)
- Support my work: [ko-fi.com/codeallnight](ko-fi.com/codeallnight)

This application is intended to help you learn about rolling code flaws.  

- [Introduction](#introduction)
- [Helpful hints](#helpful-hints)
- [Installation](#installation)
- [Menu Options](#menu-options)
- [Settings](#settings)
- [Tutorial](#tutorial)
  - [Scenario 1: clone RAW signal, replay attack on](#scenario-1-clone-raw-signal-replay-attack-on)
  - [Scenario 2: clone RAW signal, replay attack off](#scenario-2-clone-raw-signal-replay-attack-off)
  - [Scenario 3: pair remote, send next code](#scenario-3-pair-remote-send-next-code)
  - [Scenario 4: clone remote, send next code](#scenario-4-clone-remote-send-next-code)
  - [Scenario 5: skip ahead, within window-next](#scenario-5-skip-ahead-within-window-next)
  - [Scenario 6: future attack](#scenario-6-future-attack)
  - [Scenario 7: rollback attack](#scenario-7-rollback-attack)
  - [Scenario 8: rolljam attack](#scenario-8-rolljam-attack)
  - [Scenario 9: KGB/Subaru MF attack](#scenario-9-kgbsubaru-mf-attack)
  - [Scenario 10: unknown MF attack](#scenario-10-unknown-mf-attack)
  - [Scenario 11: enc00 attack](#scenario-11-enc00-attack)
  - [Scenario 12: test transmitter](#scenario-12-test-transmitter)
- [Contact info](#contact-info)
- [Future features](#future-features)

## Introduction
**Educational use only.** This application is intended to be used for educational purposes only.  It is intended to help you learn about rolling code flaws.  IIf you use this information to attack devices, you are responsible for any damage you cause.

<img src="./docs/keeloq-codes.png" width="50%" />

The Keeloq protocol has a FIX (button + serial number) and a HOP (encrypted data that can be decrypted into a count + some validation information, such as the end of the serial number). The receiver has a current count for the serial number, like 0x1E00 in the diagram above.  There are a set of "Next" codes that will Open the device.  When one of those codes is received, the beginning of the Next block will start with the received code.  There are also a set of "Future" codes.  When two adjacent codes are received, then typically the Next block will start with the second received code.  The remaining set of codes are considered "Past" codes.  Different manufacturers handle past codes differently. As a new count is accepted, the door opens and the location of the Next, Future and Past codes change.  For more details, see [this video](https://youtu.be/x4ml1JAH1q0) along with the [rolling code playlist](https://www.youtube.com/playlist?list=PLM1cyTMe-PYJfnlDk3NjM85kU5VyCViNp).

Sending signals to a real receiver has the potential to desync the remote and can even cause the remote to no longer be valid.  The reason this application was built was so that you DO NOT mess with equipment, unless you are pen testing it with permission.  Even then, you can still mess things up & require service or replacement (for example, HCS300 overflow bits get cleared and you reach 0xFFFF count then bad things may happen).  Please use this application instead of an actual device.

This application is intended to simulate various KeeLoq receivers that you may encounter.  You can configure the receiver to simulate the device you want to practice on.  Use a second Flipper Zero or HackRF or whatever to try to get the "Opened!" message.

In the future, I hope to offload this application to an ESP32+CC1101 so that you can use a single Flipper to practice rolling codes.


## Helpful hints
You can rename the file ``SD Card\subghz\assets\keeloq_mfcodes``, so that a .sub file with KeeLoq protocol will be sent **without incrementing** counts.  This will also cause all signals to be decoded as "KL Unknown".  Be sure to rename it back when you are done.

Firmware other than Official does support encoding rolling code signals.  This makes it easier to save and transmit rolling codes.  This can be a good thing when you are pen testing, but a bad thing when you are just clicking around and don't understand the potential risks.  In official firmware, you can still decode signals, write the keys down and then use the "Add Manually" option to create a signal, and then edit the keys to match what was decoded.  It's more work, but then you won't accidently be sending signals that you don't understand.  Unofficial firmware may also unlock the TX frequency list, which may be illegal in your region.  Only broadcast on frequencies that are legal in your region.  If you choose to install Unofficial firmware, it is not supported by the Flipper Zero team, so you will need to get support from the community.  If you are using unofficial firmware, you should be aware of the risks.

This rest of this section assumes you are familiar with building your own firmware.  If you are not familiar, [this video](https://youtu.be/gqovwRkn2xw) will walk you through the process of being able to build and deplay the firmware.  Note: Due to recent firmware changes, you need to do "[Debug] Flash (USB, with Resources)" [instead of *without* Resources] since some of the subghz code has moved into Resources.

If you want a Bin_RAW file, you can build a custom firmware without knowledge of keeloq.  In ``.\lib\subghz\protocols\protocol_items.c`` replace the line ``&subghz_protocol_keeloq,`` with ``// &subghz_protocol_keeloq,``.  Then build the firmware.  Now you can use the "Read" option with Bin_RAW set to "On" to get a Bin_RAW file.  Be sure to edit the file back when you are done.

If you want to generate a custom SUB file for a specific key and count, you can replace the ``case SubmenuIndexDoorHan_433_92`` code in the ``.\applications\main\subghz\scenes\subghz_scene_set_type.c`` file.  You will need to build and deploy the firmware with resources.  This will replace the implmentation of "Add Manually/DoorHan_433".  For example, if you want to generate a SUB file for a DoorHan remote with key=0x084EE9D5, button=0x2, and count of 0xEC00, at 433.920MHz; you would use the following...
```c
        case SubmenuIndexDoorHan_433_92:
            generated_protocol = subghz_txrx_gen_keeloq_protocol(
                subghz->txrx, "AM650", 433920000, "DoorHan", 0x084EE9D5, 0x2, 0xEC00);
            if(generated_protocol != SubGhzProtocolStatusOk) {
                furi_string_set(
                    subghz->error_str, "Function requires\nan SD card with\nfresh databases.");
                scene_manager_next_scene(subghz->scene_manager, SubGhzSceneShowError);
            }
            break;
```

If you press the "OK" button when reading, it will flush the radio and set the current status to CLOSED.  This means you can attempt a replay attack without having to have custom firmware.  If you don't want to have to press the OK button try attempt a replay, then you need to make the following change:  If you want the Flipper Zero to be able to decode the same signal multiple times, in ``.\lib\subghz\protocols\protocol_items.c`` after the two occurances of ``instance->decoder.decode_count_bit = 0;`` add the line ``instance->generic.data = 0;``.  This will cause the Flipper Zero to forget the previous data, so it will decode the same signal multiple times.  Be sure to edit the file back when you are done.

To scan for more interesting sequences, make this breaking change to keeloq.c file that will keep incrementing the key until it finds a DoorHan code (but it leaves the FIX value the same).  This is one technique to search for ENC00 sequences.  Be sure to edit the file back when you are done.
```c
void subghz_protocol_decoder_keeloq_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderKeeloq* instance = context;

    // Uncomment the next line if you want to ALWAYS jump ahead.
    // instance->generic.data += 0x100000000L;
    while(true) {
        subghz_protocol_keeloq_check_remote_controller(
            &instance->generic, instance->keystore, &instance->manufacture_name);
        if(strcmp(instance->manufacture_name, "Unknown") == 0) {
            // UNKNOWN
        } else if(strcmp(instance->manufacture_name, "DoorHan") != 0) {
            FURI_LOG_E(
                TAG,
                "Wrong manufacturer name: %s  high-bytes:%08lX  cnt:%08lX",
                instance->manufacture_name,
                (uint32_t)(instance->generic.data >> 32),
                instance->generic.cnt);
        } else {
            FURI_LOG_I(
                TAG,
                "Found manufacturer name: %s %08lX",
                instance->manufacture_name,
                (uint32_t)(instance->generic.data >> 32));
            break;
        }
        instance->generic.data += 0x100000000L;
    }

    // Continue with the original code.
```

### HCS300
Two common hardware implementations of KeeLoq are the HCS300, which uses 10 bits in discriminator & the HCS200, which uses 8 bits.  The Flipper Zero software implementation decodes using 8 bits.  If you make a custom change to the ``.\lib\subghz\protocols\keeloq.c`` file you can return the encoded data, which will be used by the Rolling Flaws application.  For "SN00/cfw*" set to "No" to work properly, you will need these changes.  For "SN bits/cfw*" set to "10 (dec)", you will also need this changes.  These changes allow the application to see the encrypted data, which is needed for the "SN00/cfw*" and "SN bits/cfw*" features to work properly.

Step 1. Change the two occurances of ``decrypt & 0x0000FFFF`` to read ``decrypt``.
Step 2. Change the printf at the bottom of the file...
  - In particular, we use ``instance->generic.cnt & 0xFFFF`` instead of ``instance->generic.cnt``.
  - We added ``"Enc:%04lX\r\n"`` to the end of the printf string.
  - We added a final parameter ``instance->generic.cnt >> 16`` to the end of the printf.
```c
    furi_string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:%08lX%08lX\r\n"
        "Fix:0x%08lX    Cnt:%04lX\r\n"
        "Hop:0x%08lX    Btn:%01X\r\n"
        "MF:%s\r\n"
        "Sn:0x%07lX \r\n"
        "Enc:%04lX\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        code_found_hi,
        code_found_lo,
        code_found_reverse_hi,
        instance->generic.cnt & 0xFFFF,
        code_found_reverse_lo,
        instance->generic.btn,
        instance->manufacture_name,
        instance->generic.serial,
        instance->generic.cnt >> 16);
```

## Installation
- Connect your Flipper to your computer.
- Close any programs that may be using your Flipper (putty, lab.flipper.net, qFlipper, etc.)

Method 1: (easiest)
- Load [flipc.org](https://flipc.org) using a web browser such as Chrome or Edge; which will show an "Install" button.
- Select the channel and firmware that you are running on your Flipper Zero.
- Click the Install button.
- The application will appear under "Apps/subghz/rolling_flaws".

Method 2: (command-line + allows for "SN00/cfw*" and "SN bits/cfw*" features + allows for replay feature)
- Clone the firmware repository (make sure you use ``git clone --recursive``).
- Copy the ``rolling_flaws`` folder into your firmware's ``applications_user`` folder.
- Build the firmware using ``fbt updater_package``.
- Do the steps in the previous [HCS300](#hcs300) section and then reflash the device.
  - ``fbt update_package``
- In qFlipper choose "Install from file" and choose the .tgz file from the ``dist\f7-C`` folder.

Method 3: (VS Code + allows for "SN00/cfw*" and "SN bits/cfw*" features + allows for replay feature)
- Install [VS Code](https://code.visualstudio.com/Download)
- Clone the firmware repository (make sure you use **Git: Clone (Recursive)**).
  - The URL for firmware is at https://github.com/flipperdevices/flipperzero-firmware
- Make sure you have run ``fbt vscode_dist`` at least once, so VSCode works properly.
  - You can right click on "FBT" and then choose `Open in Integrated Terminal`.
  - Then type:  `./fbt vscode_dist`
  - It should populate your `.vscode` folder.
- Copy the ``rolling_flaws`` folder into your firmware's ``applications_user`` folder.
- Ctrl+Shift+B
- Select "[Debug] Flash (USB, with resources)"
- Ctrl+Shift+B
- Select "[Debug] Launch App on Flipper"
- Do the steps in the previous [HCS300](#hcs300) section (so the "Enc:" data is present) and then reflash the device
  - Select "[Debug] Flash (USB, with resources)"
  - Select "[Debug] Launch App on Flipper"

Here is a command-line method.  Before the last two commands, you should follow the steps in [HCS300](#hcs300) section (so the "Enc:" data is present).  Be sure your Flipper Zero is connected and that qFlipper is not running:
```c
mkdir \repos
cd \repos
git clone https://github.com/jamisonderek/flipper-zero-tutorials.git
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware
cd flipperzero-firmware
fbt vscode_dist
cd applications_user
md rolling-flaws
cd rolling-flaws
xcopy ..\..\..\flipper-zero-tutorials\subghz\apps\rolling-flaws\*.* . /e
cd ..
cd ..
fbt FORCE=1 flash_usb_full 
fbt launch APPSRC=applications_user\rolling-flaws 
```


## Menu Options
### Config
This is where you can configure the settings.  The settings are reset whenever the application restarts.

### Reset count to 0
This will reset the count to 0.  This is useful when you want to start over or want to test some rollback scenarios.

### Transmit Signal
This will transmit the signal.  This is useful to capture the next signal.

### Receive Signals
This will receive signals.  This is the primary purpose of the application.

### Sync Remote
This will sync the configuration using a remote signal.  This is useful when you want to pair the Flipper Zero to your remote.

### About
This will show information about the application.

## Settings
### Frequency
Frequency is the frequency that the receiver and transmitter will use.  This should typically be set to 433.92 unless prohibited in your region.

### Protocol
- "KL (DH)" is the KeeLoq protocol with the manufacturer key from DoorHan.
- "KL (All)" is the KeeLoq protocol with any manufacturer key.  So the Flipper will try all known keys.
- "KL(Custom)" is set when doing a 'Sync Remote' operation.  The manufacture used during the sync will be used for comparisons.

### Fix [SN+Btn]
This is the button and serial number to decode.
- 0x20000000 is considered a special test transmitter (right after a reset).
- 0x284EE9D5 is used by many of the sample files from this project.
- Custom is set when doing a 'Sync Remote' operation.

### Replay attack
If this is set to "yes" then it is possible to do a replay attack.  NOTE: The flipper has built in code that prevents it from receiving a duplicate signal, so you will need custom firmware if you want to receive the same code twice.

### Window [next]
This is how many counts forward from the existing count are considered acceptable.  For example, if the current count is 0x0001 and the window is 16, then the next count can be 0x0011.

### Window [future]
This is how many counts forward from the existing count are considered future.  For example, if the current count is 0x0001 and the window is 32768, then 0x5011 would be considered a future count, but 0xEC00 would be considered a past count.

### Window [gap]
This is how close two future counts need to be from each other for them to be considered within the gap.  When a second count is received and is within the gap, the next count will be advanced to the last count sent.

### SN00/cfw*
If this is set to "yes" then if the decoded data serial number bytes match 0x00, then any serial number will be considered a match.  If this is set to "no" then the serial number must match exactly.  For this feature to validate the serial number, you will need custom firmware.

### SN bits/cfw*
By default the firmware only checks 8 bits of the serial number.  If this is set to "10 (dec)" and you have custom firmware, then 10 bits from the decoded data will need to match the serial number.

### Count 0 opens
This will cause the receiver to open when it receives a count of 0.  This is a very bad idea, but I have multiple devices that implement this strategy.  They also implement "KL (All)", which means an Unknown MF with matching FIX can open the gate.  This is a very bad idea, but it is a real world example.

## Tutorial
This tutorial assumes you have two Flipper Zeros.  The first Flipper Zero will run this application and the second Flipper Zero will be used to send Sub-GHz signals.


### Setup
Flipper #1:
- [Install](#installation) the "Rolling Flaws" application.
- NOTE: If you want to use the "SN00/cfw*" or "SN bits/cfw*" features, or enable receiving the same code twice, you will need to make custom changes to your firmware so that the encrypted data can be accessed.  See the [Helpful hints](#helpful-hints) section above.

Flipper #2:
- By default, this project uses 433.92 MHz.  If 433.92 MHz is not supported in your country, you can use 315000000 or 390000000 instead.  In the .sub files replace "Frequency: 433920000" with the frequency you want to use.
- Copy all of the .SUB files from this project onto your second Flipper Zero (in the ``SD Card\subghz`` folder).
- NOTE: Official firmware will display "Error in protocol parameters description" when trying to send "Unknown" or "KGB/Subaru" signals.  For this Flipper Zero, it may be helpful to run an unofficial firmware that allows sending these signals.  Running unofficial firmware may not be legal in your region & may prevent you from getting support in the Official Flipper Zero forum.  I've also included Bin_RAW signals for these two MF, if you would like to stay on official firmware.


### Matching Signals
You need to make sure your Flipper Zero is using the same **frequency** as the remote.  You can use the "Frequency Analyzer" option in the "Sub-GHz" application to determine the frequency of the remote.  In some cases, you can also go to https://fccid.io and enter the FCC ID number and it will tell you the frequency being used.  In some cases you may need to add custom frequencies to the firmware. For this tutorial "433.92MHz" is the frequency we will be using (but you can change it to "315MHz" or "390MHz", as long as you also edit the .SUB files to have the matching frequency).

You need to make sure your Flipper Zero is using the same **modulation** as the remote.  In some cases, you can go to https://fccid.io and enter the FCC ID number and it will tell you the modulation being used.  Otherwise, you can try each of the modulations until you find the one that works.  For this tutorial "AM650" is the modulation we will be using.

### Scenario 1: clone RAW signal, replay attack on
<img src="./docs/replay-attack-diagram.png" width="50%" />

The first attack we will try is called a "Replay attack".  This is a very common attack to do with the Flipper Zero.  For static codes (codes that don't change every time they are sent) this approach works really well.  For dynamic codes, where the code changes each time it is sent, this attack will only work if the device has a replay attack flaw.

WARNING: With some receivers, when receiver detects a Replay attack (the count didn't increment) it can stop responding to the remote. It may be necessary to take the receiver to an authorized dealer to get it reset.  This is rare, but it is something to be aware of, since it could be an expensive mistake.

Flipper #1: **Enable Replay attack**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Set "Replay attack" to "Yes".
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Freq Analyzer**
- Launch "Sub-GHz" application.
- Select "Frequency Analyzer" option.

Flipper #1: **TX signal**
- Make sure the two Flipper Zeros are at least 6" apart.
- Select "Transmit Signal" option.
  - Flipper #1 will send the signal.
  - Flipper #1 should vibrate (but it will stay on same menu option).
  - Flipper #2 should show the frequency (if not: try again).

Flipper #2: **RX signal, determine frequency**
- Notice the frequency is the same as the one you set in the "Rolling Flaws" application.
- We use this technique to determine the frequency of the remote.

Flipper #2: **Read RAW record**
- Press BACK button to return to Sub-GHz menu.
- Select "Read RAW" option.
- Select "Config" option (LEFT button).
- Set "Frequency" to match Flipper #1.
- Set "Modulation" to "AM650".
- Set "RSSI Threshold" to "-75".
- Leave all the rest of the settings default & click the BACK button.
- Press OK button to start recording.

Flipper #1: **Reset count, TX signal once**
- Select "Reset count to 0".  This resets the rolling code.
- Select "Transmit Signal" option once.
  - Flipper #1 will increment the count (to 1).
  - Flipper #1 will send the signal.
  - Flipper #1 should vibrate (but stay on same menu option).
  - Flipper #2 should show some signal get received.

Flipper #2: **Stop recording**
- Press OK button to stop recording.
- You should see options for "Erase" (LEFT), "Send" (OK), and "Save" (RIGHT).

Flipper #1: **Receive signal**
- Select "Receive Signals" option.
- Flipper #1 has "Count: 0001".
- Flipper #1 has "Fix: 284EE9D5" (which means button=2 and serial number=0x84EE9D5).
- Flipper #1 has "CLOSED" message.

Flipper #2: **Send signal**
- Press OK button to replay the signal.

Flipper #1: **Opened!**
- Flipper #1 has "OPENED!" message.
- Flipper #1 has "Count: 0001".
- Flipper #1 (bottom right) has reason as "REPLAY".

Congratulations!  You have successfully cloned and replayed a signal.

### Scenario 2: clone RAW signal, replay attack off
<img src="./docs/replay-attack-failed-diagram.png" width="50%" />

In the previous attack we successfully performed a "Replay attack".  This is because our dynamic codes receiver had a replay attack flaw.  Let's try the above steps again, but without the flaw.

WARNING: With some receivers, when receiver detects a Replay attack (the count didn't increment) it can stop responding to the remote. It may be necessary to take the receiver to an authorized dealer to get it reset.  This is rare, but it is something to be aware of, since it could be an expensive mistake.

Flipper #1: **Disable Replay attack**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Confirm "Replay attack" is set to "No".
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Freq Analyzer**
- Launch "Sub-GHz" application.
- Select "Frequency Analyzer" option.

Flipper #1: **TX signal**
- Make sure the two Flipper Zeros are at least 6" apart.
- Select "Transmit Signal" option.
  - Flipper #1 will send the signal.
  - Flipper #1 should vibrate (but it will stay on same menu option).
  - Flipper #2 should show the frequency (if not: try again).

Flipper #2: **RX signal, determine frequency**
- Notice the frequency is the same as the one you set in the "Rolling Flaws" application.
- We use this technique to determine the frequency of the remote.

Flipper #2: **Read RAW record**
- Press BACK button to return to Sub-GHz menu.
- Select "Read RAW" option.
- Select "Config" option (LEFT button).
- Set "Frequency" to match Flipper #1.
- Set "Modulation" to "AM650".
- Set "RSSI Threshold" to "-75".
- Leave all the rest of the settings default & click the BACK button.
- Press OK button to start recording.

Flipper #1: **Reset count, TX signal once**
- Select "Reset count to 0".  This resets the rolling code.
- Select "Transmit Signal" option once.
  - Flipper #1 will increment the count (to 1).
  - Flipper #1 will send the signal.
  - Flipper #1 should vibrate (but stay on same menu option).
  - Flipper #2 should show some signal get received.

Flipper #2: **Stop recording**
- Press OK button to stop recording.
- You should see options for "Erase" (LEFT), "Send" (OK), and "Save" (RIGHT).

Flipper #1: **Receive signal**
- Select "Receive Signals" option.
- Flipper #1 has "Count: 0001".
- Flipper #1 has "Fix: 284EE9D5" (which means button=2 and serial number=0x84EE9D5).
- Flipper #1 has "CLOSED" message.

Flipper #2: **Send signal**
- Press OK button to replay the signal.

Sadly, this time the last step will be...
Flipper #1: **Closed**
- Flipper #1 has "CLOSED" message.
- Flipper #1 has "Count: 0001".
- Flipper #1 (bottom right) has reason as "PAST".

### Scenario 3: pair remote, send next code
<img src="./docs/pair-fz-remote.png" width="50%" />
Sometimes you aren't trying to attack a device, you just want to use your Flipper Zero as a Universal Remote.  For this you will need to know the protocol that the remote is using.  You can use the "Sub-GHz -> Read" application to determine the protocol.  In this example, we will use a "DoorHan" remote.  Since you will pair the Flipper Zero as a new remote to your receiver, it should not impact the existing remotes and you don't have to worry about getting things out of sync.  

NOTE: Some receivers have a limited number of remotes that can be paired, so you may want to check the manual to see if this is a concern for you.

Flipper #1: **Set Frequency**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Read**
- Launch "Sub-GHz" application.
- Select "Read" option.
- Select "Config" option (LEFT button).
- Set "Frequency" to match Flipper #1.
- Set "Modulation" to "AM650".
- Leave all the rest of the settings default & click the BACK button.

Flipper #1: **TX signal**
- Make sure the two Flipper Zeros are at least 6" apart.
- Select "Transmit Signal" option.
  - Flipper #1 will send the signal.
  - Flipper #1 should vibrate (but it will stay on same menu option).
  - Flipper #2 should show some signal get received.

Flipper #2: **Determine protocol**
- Notice the protocol is "Keeloq DoorHan".  We can click on this entry to see even more information.  This is how we determine what protocol the remote is using.  We can use this information to determine what protocol to use when we add the signal to our Flipper Zero.
- Press BACK button to return to Sub-GHz menu.

Flipper #2: **Add Manually**
- Launch "Sub-GHz" application.
- Select "Add Manually" option.
- Select "DoorHan_433" option (or 315 if you are using 315MHz).
- Enter a name for your signal (We will use "Dh433_man") and choose "Save".

Flipper #2: **Emulate signal**
(Same Flipper Zero as previous steps)
- Select the file you just created (in the future, you can access this list under "Saved" option).
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.

Flipper #1: **Sync Remote**
- Select "Sync Remote" option.
- Flipper #1 has "WAITING FOR SIGNAL"

Flipper #2: **Send signal**
- Press OK button to send the signal.

Flipper #1: **Receive signals**
- Select "Receive signals"
- Flipper #1 has "Fix" matching the remote.
- Notice the current "Count" from the remote.
- Flipper #1 (bottom right) has reason as "SYNCED"

Flipper #2: **Send signal**
- Press OK button to send the NEXT signal.

Flipper #1: **Opened!**
- Flipper #1 has "OPENED!" message.
- Flipper #1 has new "Count".
- Flipper #1 (bottom right) has reason as "NEXT".

Congratulations!  You have successfully opened a gate by pairing the receiver to your Flipper and sending the next code in the sequence.  You can continue to press OK on Flipper #2 and the gate will continue to open and the "Count" will continue to increase!  

### Scenario 4: clone remote, send next code
<img src="./docs/clone-fz-remote.png" width="50%" />

In this example, we will clone an existing remote.  This is a **"bad idea"** as you are likely to get the receiver out of sync with the original remote.  There is a high probability that the original remote will no longer work & using the original remote could cause the Flipper Zero remote to no longer work.  This is why it is better to use the "Pair Remote" option instead of the "Clone Remote" option.

This scenario WILL most likely cause problems for you.  **You should only do this if you are pen-testing a device and you are willing to take the risk of getting the receiver out of sync.**

WARNING: Your original remote will be considered as performing a "Replay attack" since it's codes will be in the past. With some receivers, when receiver detects a Replay attack (the count didn't increment on your original remote) it can stop responding to the remote. It may be necessary to take the receiver to an authorized dealer to get it reset.  This is rare, but it is something to be aware of, since it could be an expensive mistake.  More commonly, you will have to pair the originial remote one or more times (which may also require an authorized dealer).

**The "Rolling Flaws" application has no consequences for getting the receiver out of sync, it was made to help you learn. Let me know if you think the application should punish you for getting the receiver out of sync -- for example, we could store a list of banned FIX values & no longer allow them to work until you enter a special code.**

Flipper #1: **Set Frequency**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Read**
- Launch "Sub-GHz" application.
- Select "Read" option.
- Select "Config" option (LEFT button).
- Set "Frequency" to match Flipper #1.
- Set "Modulation" to "AM650".

Flipper #1: **TX signal**
- Make sure the two Flipper Zeros are at least 6" apart.
- Select "Transmit Signal" option.
  - Flipper #1 will send the signal.
  - Flipper #1 should vibrate (but it will stay on same menu option).
  - Flipper #2 should show some signal get received.

Flipper #2: **Determine protocol**
- Notice the protocol is "Keeloq DoorHan".  
- Click on the entry to see more information.
- Write down the KEY value (16-character code, put spaces every two characters).
  - For example "Key:AD045814AB977214" should be written as "AD 04 58 14 AB 97 72 14".

Flipper #2: **Create a SUB file & emulate it**
- If your firmware has a "Send" button, skip ahead to the next step.
- **WARNING:** There is a reason why your firmware does not have a "Send" button.  This WILL most likely cause problems for you.  You should only do this if you are pen-testing a device and you are willing to take the risk of getting the receiver out of sync.
- If your firmware does not have a "Send" button, you will need to create a .SUB file.
- Copy the "k-unknown-sn84EE9D5-hop6A2C4803.sub" file to a new file "dh433_clone.sub".
- Edit the "dh433_clone.sub" file and change the "Key" value to match the key you wrote down.
- Copy the "dh433_clone.sub" file to your Flipper Zero (in the ``SD Card\subghz`` folder).
- Press the BACK button as needed to return to the Sub-GHz menu.
- Select "Saved" option.
- Select the "dh433_clone.sub" file.
- Select "Emulate" option.

Flipper #2: **Add Manually**
- Launch "Sub-GHz" application.
- Select "Add Manually" option.
- Select "DoorHan_433" option (or 315 if you are using 315MHz).
- Enter a name for your signal (We will use "Dh433_man") and choose "Save".

Flipper #2: **Emulate signal**
(Same Flipper Zero as previous steps)
- Select the file you just created (in the future, you can access this list under "Saved" option).
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.

Flipper #1: **Sync Remote**
- Select "Sync Remote" option.
- Flipper #1 has "WAITING FOR SIGNAL"

Flipper #2: **Send signal**
- Press OK button to send the signal.

Flipper #1: **Receive signals**
- Select "Receive signals"
- Flipper #1 has "Fix" matching the remote.
- Notice the current "Count" from the remote.
- Flipper #1 (bottom right) has reason as "SYNCED"

Flipper #2: **Send signal**
- Press OK button to send the NEXT signal.

Flipper #1: **Opened!**
- Flipper #1 has "OPENED!" message.
- Flipper #1 has new "Count".
- Flipper #1 (bottom right) has reason as "NEXT".

Congratulations!  You have successfully opened a gate by pairing the receiver to your Flipper and sending the next code in the sequence.  You can continue to press OK on Flipper #2 and the gate will continue to open and the "Count" will continue to increase! This cloning worked because the protocol was known by the Flipper and it contained the manufacturer keys.  If the protocol was unknown, the Flipper would not have been able to clone the remote. 

### Scenario 5: skip ahead, within window-next
<img src="./docs/window-next-attack.png" width="50%" />

In this example, we somehow have a file with a matching FIX to our remote.  This file also happens to have a count that is only a little bit larger than the current count.  This is a very unlikely scenario, but it is possible.  This is why it is important to use a receiver that has a small "window-next" value.

Previous warnings still apply.

Flipper #1: **Set Frequency**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Notice the "Window [next]" setting is set to 16.  
  - This means the next code must be within 16 codes of the current one.
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Emulate signal**
- Launch "Sub-GHz" application.
- Select "Saved" option.
- Select the file "k-dh-sn84EE9D5-cnt000B"
  - This is KeeLoq with MF=DoorHan
  - The Fix matches our remote.
  - The current count should be 000B.
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.

Flipper #1: **Receive signals**
- Select "Receive signals"
- Notice the current "Count" from the remote, is "0000".

Flipper #2: **Send signal**
- Press OK button to send the NEXT signal.
  - This should be a count of "000C".

Flipper #1: **Opened!**
- Flipper #1 has "OPENED!" message.
- Flipper #1 has new "Count" ("000C").
- Flipper #1 (bottom right) has reason as "NEXT".

Congratulations!  You have successfully opened a gate by skipping ahead to another code that is in the expected range.  We jumped ahead and now the count is "000C".  You can continue to press OK on Flipper #2 and the gate will continue to open and the "Count" will continue to increase!

### Scenario 6: future attack
<img src="./docs/window-future-attack.png" width="50%" />

In this example, we somehow have a file with a matching FIX to our remote.  This file also happens to have a count that is quite a ways in the future (but less than the receiver's window-future value).  For this attack to work, we need to also have a signal containing a count right after it (or a small gap).  When we send those signals in sequence and if the count is less than the receivers window-future, when the receiver detects the second signal it will resyncronize the current count to the second signal.  In some cases it will open the door (and some will require a third signal to be sent, within the window-next range).  Most only require two signals to be sent, because to reduce the number of times the user has to press the button.

If your receiver becomes out of sync with your remote (because you were pressing the remote too many times, so your count is too far in the future) then pressing the button 2-3 times may resync things; depending on the receiver firmware.  If your reciever becomes out of sync with your remote (because a cloned remote sent a future signal) then you would want to press the button many times on the remote (out of range of the receiver, so as not to do a replay/past attack) until the remote was in the window-next or window-future range.

Previous warnings still apply.

Flipper #1: **Set Frequency**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Notice the "Window [future]" setting is set to 32768 (0x8000 in hex)  
  - This means the future code must be within 32768 codes of the current one.
- Notice the "Window [gap]" setting is set to 2.
  - This means after a future code, the next code must be within 2 codes of the future one.
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Emulate signal**
- Launch "Sub-GHz" application.
- Select "Saved" option.
- Select the file "k-dh-sn84EE9D5-cnt3E90"
  - This is KeeLoq with MF=DoorHan
  - The Fix matches our remote.
  - The current count should be 3E90.
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.

Flipper #1: **Receive signals**
- Select "Receive signals"
- Notice the current "Count" from the remote, is "0000".

Flipper #2: **Send future signal**
- Press OK button to send the NEXT signal.
  - This should be a count of "3E91".

Flipper #1: **Opened!**
- Flipper #1 has "Future" set to "3E91".
- Flipper #1 has "Count" still set to "0000".
- Flipper #1 has "CLOSED" message.
- Flipper #1 (bottom right) has reason as "FUTURE".

Flipper #2: **Send next future signal**
- Press OK button to send the NEXT signal (gap is 1 from previous).
  - This should be a count of "3E92".

Flipper #1: **Opened!**
- Flipper #1 has "OPENED!" message.
- Flipper #1 has new "Count" ("3E92").
- Flipper #1 (bottom right) has reason as "GAP".

Congratulations!  You have successfully opened a gate by skipping ahead to future code sequence.  You can continue to press OK on Flipper #2 and the gate will continue to open and the "Count" will continue to increase!  At this point, your old remote is way in the past and is no longer useful.

### Scenario 7: rollback attack
<img src="./docs/rollback-attack.png" width="50%" />

This is very similar to a future attack, but instead of using codes from the future, we record and use codes from the past.  Typically this attack will only work if the Window [future] is set to "all".  What is most likely happening is the previous codes are considered as part of the far future, and when you play back two sequencial codes, it resyncs the remote to the second code.  This is a very rare attack, but it is possible on some receivers.

Previous warnings still apply.

Flipper #1: **Set Frequency, future all**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Set the "Window [future]" setting to "All".
  - Effectively we are saying past codes are considered as part of the future.
- Notice the "Window [gap]" setting is set to 2.
  - This means after a future code, the next code must be within 2 codes of the future one.
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Read RAW record**
- Launch "Sub-GHz" application.
- Select "Read RAW" option.
- Select "Config" option (LEFT button).
- Set "Frequency" to match Flipper #1.
- Set "Modulation" to "AM650".
- Set "RSSI Threshold" to "-75".
- Leave all the rest of the settings default & click the BACK button.
- Press OK button to start recording.

Flipper #1: **TX signal**
- Make sure the two Flipper Zeros are at least 6" apart.
- Select "Transmit Signal" option.
  - Flipper #1 will send the signal.
  - Flipper #1 should vibrate (but it will stay on same menu option).
  - Flipper #2 should show some signal get received.

Flipper #2: **Stop & save recording**
- Press OK button to stop recording.
- You should see options for "Erase" (LEFT), "Send" (OK), and "Save" (RIGHT).
- Select "Save" option.
- Give signal a name (we will use "signal-1").

Flipper #2: **Read RAW record**
(Same Flipper Zero as previous step)
- Select "New" option (press LEFT button).
- Press OK button to start recording.

Flipper #1: **TX signal**
- Make sure the two Flipper Zeros are at least 6" apart.
- Select "Transmit Signal" option.
  - Flipper #1 will send the signal.
  - Flipper #1 should vibrate (but it will stay on same menu option).
  - Flipper #2 should show some signal get received.

Flipper #2: **Stop & save recording**
- Press OK button to stop recording.
- You should see options for "Erase" (LEFT), "Send" (OK), and "Save" (RIGHT).
- Select "Save" option.
- Give signal a name (we will use "signal-2").

Flipper #1: **TX signal**
- NOTE: This is an optional step, we are doing this just to increase the count further & show normal usage. 
- Select "Transmit Signal" option.
  - Flipper #1 will send the signal.
  - Flipper #1 should vibrate (but it will stay on same menu option).

Flipper #2: **Emulate signal**
- Press BACK button, until you are at Sub-GHz menu.
- Select "Saved" option.
- Select the file "signal-1"
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.

Flipper #1: **Receive signals**
- Select "Receive signals"
- Notice the current "Count" from the remote.

Flipper #2: **Send signal**
- Press OK button to send the NEXT signal.
- Notice: Flipper #1 should show "FUTURE" with some count in the past.

Flipper #2: **Emulate and send next signal**
- Press BACK button, until you are at Sub-GHz menu.
- Select "Saved" option.
- Select the file "signal-2"
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.
- Press OK button to send the NEXT signal.

Flipper #1: **Opened!**
- Flipper #1 has "OPENED!" message.
- Flipper #1 has new "Count".
- Flipper #1 (bottom right) has reason as "GAP".

Congratulations!  You have successfully opened a gate by replaying signals from back in time (that were considered future codes).  This attack worked using RAW signals, where we don't need to ability to decode the data, we just needed to record two open commands to play back later!

### Scenario 8: rolljam attack
<img src="./docs/rolljam-attack.png" width="50%" />

The concept is when the first signal is sent from the remote, you somehow record the signal (narrow bandwidth) while preventing the receiver from getting the signal (perhaps interference near the receiver).  The user of the remote then sends the second signal, which you again somehow record the signal (narrow bandwidth) while preventing the receiver from getting the signal.  You then send the first signal without interference and the device will open, so user doesn't realize anything strange.  You still have a second signal that you can use to open the device.  Depending on other flaws, you may need to use the second signal before the next open signal (or before any signal, including "close/lock" signals). Samy Kamkar released videos on this attack years ago.

https://www.fcc.gov/general/jammer-enforcement (illegal to jam signals in the US) so you should not practice this technique in the US.

### Scenario 9: KGB/Subaru MF attack
<img src="./docs/kgb-attack.png" width="50%" />

The concept here is that perhaps the receiver knows how to decode many manufacturers, instead of just one.  If we send a FIX value that matches the expected value, but encode the count using a different manufacturer key, then perhaps it will open (you will typically combine this attack with a future attack, because you don't know what the count should be).  For universal receivers, there is a chance they forget to actually store the manufacturer associated with the remote but instead loop through all of the keys they know about.  For this demo, we use KGB/Subaru as the manufacturer, but you can use any manufacturer that the receiver knows about.

Previous warnings still apply.

Flipper #1: **Set Frequency, KeeLoq all**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Set the "Protocol" setting to "KL (All)".
  - Effectively we are saying any MF Flipper Zero knows about is treated the same.
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Freq Analyzer**
- Launch "Sub-GHz" application.
- Select "Saved" option.
- Select the "k-dh-sn84EE9D5-cndEC01" file.
- Select "Emulate" option.

Flipper #1: **Sync Remote**
- Select "Sync Remote" option.
- Flipper #1 has "WAITING FOR SIGNAL"

Flipper #2: **Send signal**
- Press OK button to send the signal.
- Flipper #1 should go back to the menu.

Flipper #2: **Prepare to Read signal**
(same Flipper as previous step)
- Press BACK button to return to Sub-GHz menu.
- Select "Read" option.
- Select "Config" option (LEFT button).
- Set "Frequency" to match Flipper #1.
- Set "Modulation" to "AM650".
- Leave all the rest of the settings default & click the BACK button.

Flipper #1: **Send signal**
- Select "Transmit Signal" option.
- Flipper #1 will send the signal.
- Flipper #1 should vibrate (but it will stay on same menu option).
- Flipper #2 should show some signal get received.

Flipper #2: **View signal details**
- Press OK button to see details of the signal.
- Notice the "Fix" value is "284EE9D5".  
- Also the end of the Key is "AB977214" (Fix with bits from right to left)
- This technique works even when the receiver doesn't know the manufacturer.  We now know what data "AB 97 72 14" is required to be at the end of our key.  We can then encrypt a count using a different manufacturer key.  This is how we can use a KGB key to open a DoorHan gate.

Flipper #1: **Receive signals**
- Select "Receive signals"
- Notice the current "Count" from the remote, is "EC03".

Flipper #2: **Emulate and send KGB signal**
- Press BACK button, until you are at Sub-GHz menu.
- Select "Saved" option.
- Select the file "k-subaru-sn84E9D5-cntEC0D".  On some firmware, where non-DoorHan signals cannot be sent, you may be required to use "b-subaru-sn84E9D5-cntEC0D" instead; which is a Bin-RAW file.
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.
- Press OK button to send the NEXT signal.

Flipper #1: **Opened!**
- Flipper #1 has "OPENED!" message.
- Flipper #1 has new "Count".
- Flipper #1 (bottom right) has reason as "NEXT".

Congratulations!  You have successfully opened a gate by using a different manufacturer key to encrypt the count.  This attack worked because the receiver didn't save the manufacturer of the remote, so it tried all of the keys it knew about.

### Scenario 10: unknown MF attack
<img src="./docs/unknown-mf-attack.png" width="50%" />

The concept here is that when the receiver can't decode the manufacturer, it uses a count of 0.  For universal receivers, there is a chance they open when the count is 0.  For this demo, we use a "KeeLoq Unknown" as the manufacturer, but you can use any manufacturer that the receiver doesn't know about.

Previous warnings still apply.

Flipper #1: **Set Frequency, KeeLoq (All)**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Set the "Protocol" setting to "KL (All)".
  - Effectively we are saying any MF Flipper Zero knows about is treated the same.
  - This also will all the receiver to get a "KL Unknown" as the MF.
- Set the "Count 0 opens" to "Yes".
  - Effectively we are saying that a count of 0 will open the door.
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Emulate signal**
- Launch "Sub-GHz" application.
- Select "Saved" option.
- Select the "k-unknown-sn84EE9D5-hop6A2C4803" file. On some firmware, where non-DoorHan signals cannot be sent, you may be required to use "b-unknown-sn84EE9D5-hop6A2C4803" instead; which is a Bin-RAW file.
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.

Flipper #1: **Transmit signal**
- Select "Transmit Signal" option.
- Flipper #1 will send the signal.
- NOTE: We are just doing this step, so the count is not 0.

Flipper #1: **Receive signals**
(On the same Flipper Zero as the previous step.)
- Select "Receive signals"
- Notice the current "Count" from the remote, is "0001".

Flipper #2: **Emulate and send Unknown signal**
- Press OK button to send the NEXT signal.

Flipper #1: **Opened!**
- Flipper #1 has "OPENED!" message.
- Flipper #1 has new "Count".
- Flipper #1 (bottom right) has reason as "COUNT0".


### Scenario 11: enc00 attack
<img src="./docs/enc00-attack.png" width="50%" />

The software implementation of KeeLoq used by the Flipper Zero treats a decoded serial number of "00" as a special case that matches ANY serial number.  This is interesting, because once we find a HOP code for a manufacturer that decodes to "00" for the SN and a Button value we want, we can replace the FIX code with any other SN. The FIX code (Button+SN) is sent in the clear for KeeLoq.

For example, DoorHan with Key of "C0 00 0B D4 AB 97 72 14" decrypts to a count 3DC9 with a "00" SN.  The last digit of the key is 4 (0100) which written backwards is "0010" so button 2.  Now if someone transmits a button 2 DoorHan signal, say "AD 04 58 14 AB 97 72 14" we can replace their "AD 04 58 14" with "C0 00 0B D4", getting "C0 00 0B D4 AB 97 72 14" which will decrypt to a count of 3DC9 but using their serial number! We can then use Key "C0 02 8A 33" prefix to get a count of 3DCA with a "00" SN, which is the next code in the sequence.  Likely this would resync the receiver to our remote!  (NOTE: if "3DC9" was in the past, we would need to use a different KEY to jump to a different count, like count of A247 or FD75.)

At this point the original remote would no longer work, since it was in the PAST.  If we jumped even further into the future, at some point it _might_ work when we press the remote button twice?

**WARNING: Some receivers have a limit as to how many times you can cycle!**  The Overflow bits make it so that you can only cycle 0, 1 or 2 times.  "This can be done by programming OVR0 and OVR1 to 1s at the time of production. The encoder will automatically clear OVR0 the first time that the synchronization value wraps from 0xFFFF to 0x0000 and clear OVR1 the second time the counter wraps. Once cleared, OVR0 and OVR1 cannot be set again, thereby creating a permanent record of the counter overflow. This prevents fast cycling of 64K
counter. If the decoder system is programmed to track the overflow bits, then the effective number of unique synchronization values can be extended to 196,608." -- [HCS300 datasheet](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/21137G.pdf)

Previous warnings still apply.

Flipper #1: **Set Frequency, SN00/cfw**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Set "SN00/cfw*" to "Yes" to allow for 00 to match any.  (If you don't have custom FW this is the behavior regardless of the setting.)
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Read**
- Launch "Sub-GHz" application.
- Select "Read" option.
- Select "Config" option (LEFT button).
- Set "Frequency" to match Flipper #1.
- Set "Modulation" to "AM650".
- Leave all the rest of the settings default & click the BACK button.

Flipper #1: **TX signal**
- Make sure the two Flipper Zeros are at least 6" apart.
- Select "Transmit Signal" option.
  - Flipper #1 will send the signal.
  - Flipper #1 should vibrate (but it will stay on same menu option).
  - Flipper #2 should show some signal get received.

Flipper #2: **Determine key (FIX:BTN+SN)**
- Press OK to view the key.  In particular we are interested in the last 8 digits.
- Press BACK button to return to Sub-GHz menu.

Flipper #2: **Load ENC00 file**
- Launch "Sub-GHz" application.
- Select "Saved" option.
- If needed edit the "k-dh-enc00-cnt3DC9" on your PC, replacing the last 8 digits of the Key with the last 8 digits from the previous step.  (The provided file Key ends with "AB 97 72 14" which matches the tutorials "sn84EE9D5" and "button2".)
- Select "k-dh-enc00-cnt3DC9" file.
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.

Flipper #1: **Receive signals**
- Select "Receive signals"
- Flipper #1 has "Fix" matching the remote.
- Notice the current "Count" is some small number.

Flipper #2: **Send signal**
- Press OK button to send the 3DC9 signal with a "00SN".

Flipper #1: **Future**
- Flipper #1 has "Future" set to "3DCA".
- Flipper #1 has "Count" still set to the small number.

Flipper #2: **Load next ENC00 file**
- Press BACK to "Sub-GHz" menu.
- Select "Saved" option.
- If needed edit the "k-dh-enc00-cnt3DCA" on your PC, replacing the last 8 digits of the Key with the last 8 digits from the previous step.  (The provided file Key ends with "AB 97 72 14" which matches the tutorials "sn84EE9D5" and "button2".)
- Select "k-dh-enc00-cnt3DCA" file.
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.

Flipper #2: **Send signal**
- Press OK button to send the 3DCA signal with a "00SN".

Flipper #1: **Opened!**
- Flipper #1 has "OPENED!" message.
- Flipper #1 has new "Count".
- Flipper #1 (bottom right) has reason as "GAP".

Congratulations!  You have successfully opened a gate by using a different serial number.  This attack worked because the receiver didn't check the serial number ending of the remote when the decoded serial number was "00".

### Scenario 12: test transmitter
<img src="./docs/test-attack.png" width="50%" />

"The HCS512 decoder will automatically add a test transmitter each time an Erase All Function is done. A test transmitter is defined as a transmitter with a serial number of zero. After an Erase All, the test transmitter will always work without learning and will not check the
synchronization counter of the transmitter. Learning of any new transmitters will erase the test transmitter." -- [HCS512 datasheet](https://www.microchip.com/content/dam/mchp/documents/MCU08/ProductDocuments/DataSheets/40151E.pdf)

Flipper #1: **Set Frequency, SN0000000**
- Exit and relaunch the "Rolling Flaws" application (resets all settings).
- Select "Config" option.
- Set "Frequency" to the frequency you want to use.
- Set "Fix [Btn+SN]" to "0x20000000" to allow for test transmitter.
- Leave all the rest of the settings default & click the BACK button.

Flipper #2: **Load sn0000000 file**
- Launch "Sub-GHz" application.
- Select "Saved" option.
- Select "k-dh-sn0000000" file.
- Select "Emulate" option.
- Flipper #2 should show "Send" (OK) option.

Flipper #1: **Receive signals**
- Select "Receive signals"
- Notice the current "Count" is some small number.

Flipper #2: **Send signal**
- Press OK button to send the test signal with a SN of all zeros.

Flipper #1: **Opened!**
- Flipper #1 has "OPENED!" message.
- Flipper #1 has new "Count".
- Flipper #1 (bottom right) has reason as "TEST".

## Contact info
Rolling Flaws by [@CodeAllNight](https://twitter.com/codeallnight).
- Discord invite: [https://discord.com/invite/NsjCvqwPAd](https://discord.com/invite/NsjCvqwPAd)
- YouTube: [https://youtube.com/@MrDerekJamison](https://youtube.com/@MrDerekJamison)
- GitHub: [https://github.com/jamisonderek/flipper-zero-tutorials/blob/main/subghz/apps/rolling-flaws](https://github.com/jamisonderek/flipper-zero-tutorials/blob/main/subghz/apps/rolling-flaws)
- Support my work: [ko-fi.com/codeallnight](ko-fi.com/codeallnight)

## Future features
- Add GPIO feature to Flipper app so you can program an HCS301/HCS512 chip to have your own MF code and settings. This would help you secure your KeeLoq devices, since the MF code would only be known to you. 
- Toggle a GPIO pin when Open is displayed. 
- Send IR signal when Open is displayed. 
- Make it "painful" to reset the device when it gets out of sync (so people understand getting things out of sync can end up costing money or time).
- Additional support for more rolling-code protocols.
- Porting the application to ESP32+CC1101, so it doesn't require a second Flipper Zero to use.
