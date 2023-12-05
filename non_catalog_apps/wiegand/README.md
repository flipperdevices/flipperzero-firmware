# Wiegand

This application supports W4, W8, W24, W26, W32, W34, W37, W40 and W48 formats.

This application can be used to test Wiegand readers and keypads. It can save the data to a file, and can load and replay the data. Timings are measured and displayed; which can be used to help debug Wiegand readers.

## Wiring

The D0 and D1 wires of the Wiegand must be about 1K or higher for the Flipper to be able to effectively pull them down to ground. If the line has a lower resistance, like 100 ohms, then you need to add MOSFETs to help pull the lines low. The following [YouTube video](https://youtu.be/OVyd3ffnZ0M) is a demonstration of how to correctly wire the device.

In all configurations:

- Pin A7 goes to your Wiegand D1 (white) wire.
- Pin A4 goes to your Wiegand D0 (green) wire.
- Pin GND goes to your Wiegand GND (black) wire.
  This is sufficient for reading signals with both this application and the Debug Accessor application.

If the pull-up resistors on your card reader are less than 1K, you also need the following:

- Two additional MOSFETs are required. I used IRF540 (but perhaps IRL540 would be better?)
- Two additional Pull-down resitors are required. I used 5K, but 4.7K would be fine as well.
- I'm still learning, but perhaps a MOSFET driver / optocoupler would provide even more protection to the Flipper GPIO pins? Use this circuit at your own risk. :)

Here is how you wire up the MOSFET and the pull-down resistors:

- The source pin of each MOSFET connects to ground.
- The gate pin connects to one side of a resistor. The other side of the resistor goes to ground.
- For the MOSFET used for D1:
  - The gate pin of the MOSFET for D1 goes to pin A6 on the Flipper. (It should also already be connected to the pull-down resistor)
  - The drain pin of the MOSFET for D1 goes to pin A7 on the Flipper.
- For the MOSFET used for D0:
  - The gate pin of the MOSFET for D0 goes to pin B3 on the Flipper. (It should also already be connected to the pull-down resistor)
  - The drain pin of the MOSFET for D0 goes to pin A4 on the Flipper.

## W4: 4-bit Wiegand

This format is used by some keypads. Digits 0-9 are sent as 0-9. ESC is sent as 10 and ENTER as 11. There is no parity bit. The application will display
the button pressed (including ESC and ENTER).

## W8: 8-bit Wiegand

This format is used by some keypads. The last 4 bits are the actual data.Digits 0-9 are sent as 0-9. ESC is sent as 10 and ENTER as 11. The first 4 bits are the inverse of the last 4 bits. The application will display
the button pressed (including ESC and ENTER). If there are any bit errors, the application will show the incorrect bits.

## W26: 26-bit Wiegand

This is a 26-bit format used by many readers. The first bit is an even parity bit. The next 8 bits are the facility code. The next 16 bits are the card number. The last bit is an odd parity bit. The application will display the facility code and card number. If there are any bit errors, the application will show the incorrect bits (the even partity is the first 13 bits, odd parity is the last 13 bits).

## W24: 24-bit Wiegand

This is similar to W26, but without the leading and trailing parity bits. The first 8 bits are the facility code. The next 16 bits are the card number. The application will display the facility code and card number.

## W35: 35-bit Wiegand

This is HID 35 bit Corporate 1000 - C1k35s format. The first bit is odd parity 2 (based on bits 2-35). The next bit is even parity (based on 4-5,7-8,10-11,...,33-34). Then 12 bit company code. Then 20 bit card id. Then odd parity 1.

## W36: 36-bit Wiegand

This is decode HID 36 bit Keyscan - C15001 format. The first bit is an even parity bit. The next 10 bits are the OEM number. The next 8 bits are the facility code. The next 16 bits are the card number. The last bit is an odd parity bit.
Other 36 bit credentials may be decoded incorrectly.

## W48: 48-bit Wiegand

This is HID 48 bit Corporate 1000 - C1k48s format. The first bit is odd parity 2 (based on bits 2-48). The next bit is even parity (based on 4-5,7-8,10-11,...,46-47). Then 22 bit company code. Then 23 bit card id. Then odd parity 1 (based on 3-4,6-7,9-10,...,45-46).

## W32/W34/W37/W40: 32/34/37/40-bit Wiegand

These formats are not very standardized, so the application will not try to interpret the data. You can modify the wiegand_data.c file to add your own interpretation.

## Installation

### Step 1. Install Git and VS Code.

If you have not already installed Git and VS Code, you will need to do so. The following links will take you to the download pages for Git and VS Code.

- [Git](https://git-scm.com/downloads)
- [VS Code](https://code.visualstudio.com/download)

### Step 2. Clone the Flipper Zero firmware.

Clone the Flipper Zero firmware from GitHub. The following command will clone the firmware into a folder named official-firmware. (The below commands may wrap on your screen. You can copy and paste the entire command; there should only be two lines.)

```console
cd <your working directory>
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git official-firmware
```

Replace _&lt;your working directory&gt;_ with the directory where you want to clone the firmware.

### Step 3. Run FBT to build the firmware and configure VS Code.

Run the following commands from the root of the firmware folder to build the firmware and configure VS Code. Replace _&lt;your working directory&gt;_ with the directory where you cloned the firmware. In some environments you do not need the "./" at the beginning of the command.

```console
cd <your working directory>
cd official-firmware
./fbt vscode_dist
./fbt updater_package
```

\*\*\* **Please follow the steps at [FBT VSCode integration](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/fbt.md#vscode-integration) before proceeding.** \*\*\*

### Step 4. Open the applications_user folder in VS Code.

After following the previous step, navigate to the **applications_user** folder in VS Code.

### Step 5. Crete a new folder for the application.

Create a new folder for the application. The name of the folder will be the name of the application. For this example, we will use the name **wiegand**.

### Step 6. Copy the application files.

Copy the files from this project to the **wiegand** folder. Be sure to copy the scenes directory into a scenes directory in the **wiegand** folder.

### Step 7. Build the application.

Build the application by running the following command from the root of the firmware folder. Replace _&lt;your working directory&gt;_ with the directory where you cloned the firmware. In some environments you do not need the "./" at the beginning of the command.

```console
cd <your working directory>
cd official-firmware
./fbt launch_app APPSRC=applications_users/wiegand
```
