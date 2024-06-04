# flipper-enigma

[![Build](https://github.com/xtruan/flipper-enigma/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/xtruan/flipper-enigma/actions/workflows/build.yml)

![ENIGMA](https://github.com/xtruan/flipper-enigma/blob/main/icons/ENIGMA_128x56.png)

## Enigma machine emulator for Flipper Zero
- Built against `0.101.2` Flipper Zero firmware release
- Uses [enigma-machine](https://github.com/LeonardoE95/enigma-machine) lib

The Enigma machine was an encryption device used primarily by Germany during World War II to secure military communications. It consisted of a series of rotors and plugboards that scrambled plaintext messages into ciphertext, which could then be transmitted securely. The machine's complexity and the constantly changing settings of its components made it incredibly difficult to decipher without knowing the exact configuration used at the time of encryption. Despite its initial success in keeping communications secret, the Allies eventually broke the Enigma code, significantly contributing to their victory in the war. Today, the Enigma machine is a symbol of cryptographic history and technological innovation.

This repository provides an emulator for the M3 Enigma. The M3 Enigma, also known as the Naval Enigma, was a specific version of the Enigma encryption machine used by the German Navy (Kriegsmarine) during World War II. The M3 featured three rotors and a plugboard (steckerbrett) to scramble messages into ciphertext.

Key features of the M3 Enigma included:

1. Three Rotors: Each rotor had 26 positions, and their arrangement and initial settings were crucial for encryption.
2. Reflector: A fixed component that redirected the electrical current back through the rotors in a unique pattern.
3. Plugboard (Steckerbrett): Allowed for additional scrambling by swapping pairs of letters.

### Installation

- Download [last release fap file](https://github.com/xtruan/flipper-enigma/releases/latest)
- Copy fap file to the apps folder of your Flipper SD card

### Usage

- Start "Enigma" plugin

### Build

- Recursively clone your base firmware (official or not)
- Clone this repository in `applications_user`
- Build with `./fbt fap_dist APPSRC=applications_user/flipper-enigma`
- Retrieve build fap in dist subfolders

(More info about build tool [here](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/fbt.md))

### 


