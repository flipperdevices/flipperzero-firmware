# Mifare Nested Attacks for Flipper Zero

Ported nested attacks from proxmark3 (Iceman fork)

## Download

[![FAP Factory](https://flipc.org/api/v1/AloneLiberty/FlipperNested/badge)](https://flipc.org/AloneLiberty/FlipperNested)

## Currently supported attacks:

 - static nested attack
 - nested attack

## Warning

App is still in early development, so there may be bugs. Your Flipper Zero may randomly crash/froze. Please create issue if you find any bugs (one bug = one issue).

## Disclaimer

The app provided for personal use only. Developer does not take responsibility for any loss or damage caused by the misuse of this app. In addition, the app developer does not guarantee the performance or compatibility of the app with all tags, and cannot be held liable for any damage caused to your tags/Flipper Zero as a result of using the app. By using this app you confirm that the tag belongs to you, you have permission to preform the attack and you agree to hold the app developer harmless from any and all claims, damages, or losses that may arise from its use.

## I need **your** help!

To successfuly recover keys from nested attack we need to correctly predict PRNG value. But we have a problem with that. Due to lack of my knowlege of Flipper Zero NFC HAL, PRNG can jump by quite large values (not like Proxmark3). So app is trying to find a delay where PRNG can be predicted accurately enough. This is not the best option, because we have to try to recover a bunch of unnecessary keys, which takes a lot of time and RAM and also spend a lot of time on timings. I don't know how to fix it. 

UPD: Chameleon Ultra devs [faced same issue](https://youtu.be/_wfikmXNQzE?t=202). They seems to use same method: [nested.c](https://github.com/RfidResearchGroup/ChameleonUltra/blob/main/software/src/nested.c) (better know from the beginning of development...)

## How to use it?

This guide assumes that you have already installed the app.

To recover keys from card you first need to collect nonces.

That's what this app was created for. App can't recover keys on Flipper Zero (yet). You need to use external device (PC) to recover keys. You can't use Mfkey32 on your phone with this app.

1. Save tag after scanning (you must have found at least one key). This action will create key cache that app will use for authorization on tag.

2. Run "Nested attack" from app menu

3. Wait until calibration complete

If the calibration passed and nonce collection began, you are very lucky and the tag is vulnerable. If not, you cannot use this app to recover keys from this tag.

Calibration can take a lot of time. If the calibration takes longer than 10 minutes, it is better to see the logs. Some static encrypted nonce tags may lead to inifnity calibration loop.

4. Wait until "Nonces collected!"

5. Recover keys via [desktop script](https://github.com/AloneLiberty/FlipperNestedRecovery)

6. Run "Check found keys" in app menu

When all keys are checked, they will be added to your user key dictonary. You can rescan your tag now. 

If not all keys found - run Nested attack again.

## FAQ

For frequently asked questions, please refer to the [FAQ.md](./FAQ.md) file.

## Contacts

Telegram: [@libertydev](https://t.me/libertydev)
