## v1.2

Firmware switching! The app now supports flashing two different firmware apps to slot A and slot B and uses the otadata partition (initialized by boot_app0) to switch between them. Make sure your firmwares are compiled to use the same partition table to avoid issues!


## v1.1

Auto-reset and notifications! For supported boards (RTS and DTR connected), flasher will now automatically enter the bootloader and reset the board. Also added notifications for user feedback (blue LED for duration of flash, vibrate when done).


## v1.0

Initial ESP flasher release! Supports hardcoded addresses for bootloader, partition table, NVS, boot_app0, and firmware, with a toggle for ESP32-S3 (different bootloader address).
