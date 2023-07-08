# HID File Transfer

This is an application for the Flipper Zero that allows to exchange files using the HID protocol with around 260kB/s.  
This approach is especially useful when access to mass storage devices is blocked on a PC.

Transferring files with this application requires the Windows/Linux client to run an executable that communicates with the Flipper Zero for data exchange.
Therefore, multiple stages are required to be able to exchange data.

## Included Linux/Windows Clients
- HIDMinimalClient
    - Is sent to the target system via the Flipper Zero BadUSB application.
    - Used to send the full-featured HIDFullClient application via a single HID endpoint to the target system (@60kB/s).
    - Can only receive data from the HIDTransfer Flipper Zero application using the *Send Client* menu entry.
- HIDFullClient
    - Can send and receive data to/from the Flipper Zero.
    - Utilizes 6 HID endpoints to achieve ~260kB/s data transfer speeds.

## Prerequirements

- Windows
    - Run powershell scripts
    - Run arbitrary executables
    - ~5 minutes of time
    - Low privileged user account is sufficient
- Linux
    - Run arbitrary executables
    - User account with root privileges

## How To
1. Compile the application (see below)
1. Copy the contents from the `dist` folder to the Flipper Zero SD-Card  
    ```
    rsync -rt dist/ [SD-CARD]
    ```
1. Transfer the HIDMinimalClient to the target system
    1. Open a text editor on the target system, start the BadUSB Flipper application and select the correct `HIDMinimalClient` payload.
    1. Wait ~3 minutes until finished... 
    1. Save the text file, that is actually a powershell/bash script.
    1. Execute the script. This will create a `.zip` file on the system in the same folder.
    1. Extract the `.zip` on the target system.
    1. Start the extracted executable on the target system.
1. Transfer the HIDFullClient
    1. Start the HIDTransfer application on the Flipper
    1. Select *Send Client* and then select the `HIDFullClient.zip` (Linux)/`HIDFullClient.exe.zip` (Windows) from the `HIDTransfer` folder on the Flipper SD Card. This will transfer the FullClient at 60kB/s to the target system.
    1. Extract the `.zip` file.

### Receive Data on the target system
1. Launch `HIDFullClient receive` on the target system.
1. Start the HIDTransfer application on the Flipper and select `Send file`.
1. Select a file from the SD-Card.

### Send Data from the target system
1. Start the HIDTransfer application on the Flipper and select `Receive file`.
1. Launch `HIDFullClient send -path [file path]` on the target system. The file is stored on the Flipper SD Card inside the `HIDTransfer` folder.
1. The stored file is in a `raw` format that must be post processed. This can also be done by using `HIDFullClient decompress [.raw file]`.



## How to Compile
- Requirements
    - Docker
    - Internet connection

I only tested this with the [unleased-firmware](https://github.com/DarkFlippers/unleashed-firmware), but it should also work for other firmware forks.

- Compile for latest `unleased-firmware` release:
    ```
    ./build.sh https://github.com/DarkFlippers/unleashed-firmware
    ```
- Compile for a specific version (git tag):
    ```
    ./build.sh https://github.com/DarkFlippers/unleashed-firmware unlshd-055
    ```

The output is stored in the `dist` folder.
