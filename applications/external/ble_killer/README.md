# Intro

The Bluetooth expansion board (ble ext) based on flipper zero realizes the scanning of surrounding low-power Bluetooth devices and the integrated Bluetooth device controller (currently comes with the control method of oklok ble Bluetooth padlock). The serial port is used between flipper zero and the expansion board. communication.

Flipper open source project address: https://oshwhub.com/h4lo/flipper-zero-ble-ext

## Compile & Install

Two programs need to be installed, one is the flipper zero app, which is in .fap format, and the other is the ble expansion board program (based on esp32).

### ble expansion board

Based on arduino development, create a new project in arduino IDE, copy the .ino source code in the esp32 directory, select the Board type as ESP32 DEV Module, install the BLEDevice library at the same time, find a usb to ttl tool, and connect tx, rx, gnd, then click compile and upload.

## USAGE

### Baud rate configuration

You can configure and select 115200, 38400, and 9600. By default, the communication baud rate between the FAP application and the ble expansion board is 9600.

![Screenshot-20240316-100444](.page_assets/Screenshot-20240316-100444.png)

### Scanning

Click the OK button to scan for surrounding BLE devices. Currently, only 5 are displayed on the default screen.

![Screenshot-20240316-101516](.page_assets/Screenshot-20240316-101516.png)

### Weapon

The left and right buttons switch the weapon payload, and the unlocking payload of the oklok Bluetooth padlock is configured by default.

![Screenshot-20240316-101528](.page_assets/Screenshot-20240316-101528.png)

### Console

Serial port information output by esp32.

![Screenshot-20240316-115228](.page_assets/Screenshot-20240316-115228.png)

## Video reference

blibili link： https://www.bilibili.com/video/BV1Dw4m197BL/?spm_id_from=333.999.0.0&vd_source=3959ff98419cda515c7f13dab0f2f564
