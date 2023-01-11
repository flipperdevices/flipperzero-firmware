# firmware
The Flipper Zero makes updating the firmware simple and fairly safe.  It is easy to build your own firmware as well using the fbt command.

The flipper blog has a great [entry](https://blog.flipperzero.one/new-firmware-update-system/) all about the underlying firmware system, which explains why it is unlikely that you will brick your device.


## Updating firmware via an application
[https://flipperzero.one/update](https://flipperzero.one/update) has links to qFlipper (the PC application) and also to Flipper Mobile App.  These applications can be used to easily update the firmware on your flipper.

- Install an SD card.
- If needed, format the SD card
  - If your flipper is not at the Flipper Desktop press the BACK button until you are at the Desktop.
  - Press the OK button on the flipper to pull up the main menu.
  - Choose "Settings" from the menu.
  - Choose "Storage" fromthe sub-menu.
  - Choose "Format SD card".
- Download & run the application
- Connect to Flipper via bluetooth (or for the PC connect the USB cable)
- On the PC click on the wrench icon & choose "Release" as the dropdown
- Click the "Install" button.
- Click the "Update" button.
- Wait about 3 minutes.
- Your Flipper should say "Firmware update - Success. OK>"
- Click right arrow button on flipper.
- Unplug Flipper from USB port.

Your Flipper is now running the latest Release firmware.


## Building a custom firmware on Windows
You can build a custom firmware for the flipper, which allows you to add additional features to your flipper.  Many people have [special firmwares](https://github.com/UberGuidoZ/Flipper/tree/main/Firmware_Options) available on github, which enable additional functionality.  Most of these firmwares started with the [official firmware](https://github.com/flipperdevices/flipperzero-firmware) 
and create their own [fork](https://github.com/flipperdevices/flipperzero-firmware/network/members) with customizations.

For instructional purposes, we will just use the code from the [official firmware](https://github.com/flipperdevices/flipperzero-firmware). 


- Step 1. You need to have GIT tools installed.
  NOTE: If you already have GIT installed, you can skip this step.
  - Open https://git-scm.com/download/win
  - Download the setup program... https://github.com/git-for-windows/git/releases/download/v2.39.0.windows.2/Git-2.39.0.2-64-bit.exe
  - Install Git-2.39.0.2-64-bit.exe  (you can just accept all the defaults)

- Step 2. You need an command prompt running as administrator.
  - In windows search bar type "cmd" (it should find "Command Prompt". click on run as administrator (right click if needed))

- Step 3. Clone the repo.
  NOTE: If you have already cloned the repo, you can skip this step.
  - In the command prompt type the following commands to recursively clone the official firmware repo:
  ```
  md \repo
  cd \repo
  git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git
  ```

- Step 4. Install the tools.
  NOTE: You should only have to do this step once per repo.
  - In the command prompt type the following commands to install the firmware toolkit:
  ```
  cd \repo\flipperzero-firmware
  fbt.cmd
  ```

- Step 5. Update your code to the latest version.
  NOTE: If you don't want to update your files with what is latest on github, you can skip this step.
  - In the command prompt type the following commands to update your local files:
  ```
  cd \repo\flipperzero-firmware
  git pull
  ```

- Step 6. Build the updater package firmware.
  - In the command prompt type the following commands to build your firmware:
  ```
  cd \repo\flipperzero-firmware
  fbt.cmd updater_package
  ```

- Hopefully the list line of output should be simiar to the following:
  ```
  	2023-01-10 15:09:56,355 [INFO] Use this directory to self-update your Flipper:
          dist\f7-D\f7-update-local
  ```

- Your new firmware files should be at the c:\repo\flipperzero-firmware\dist\f7-D folder.


## Using qFlipper to update with custom firmware
Once you have built your firmware, you should have a file named flipper-z-f7-update-local.tgz

### Method 1:
- Step 1. Launch the qFlipper application.
- Step 2. Connect your flipper via USB cable.
- Step 3. Click "Install from file".
- Step 4. Choose c:\repo\flipperzero-firmware\dist\f7-D\flipper-z-f7-update-local.tgz and click Open.
- Step 5. Click "Install".
- Wait about 3 minutes for the installer.
- Step 6. Your flipper should show "Firmware updated. Success. OK>"
- Step 7. Click "OK"

### Method 2:
This method seems to always work for me.
- Step 1. Launch the qFlipper application.
- Step 2. Connect your flipper via USB cable.
- Step 3. Click on the folder icon in qFlipper.
- Step 4. Open the "SD Card" folder.
- Step 5. Select the "Manifest" file and right click "Delete" then click Delete.
- Step 6. Select the "Update" folder (if it exists) and right click "Delete" then click Delete.
- Step 7. Right click in a blank area and choose "New Folder" and name it "update".
- Step 8. Double click on the "update" folder.
- Step 9. In Windows File Explore open the c:\repo\flipperzero-firmware\dist\f7-D\f7-update-local folder.
- Step 10. Drag the firmware.dfu file from Windows File Explorer into the qFlipper file window.
- Step 11. Drag the remaining files from the c:\repo\flipperzero-firmware\dist\f7-D\f7-update-local folder into the qFlipper file window.
- Step 12. Disconnect the USB cable.
- Step 13. If your flipper is not at the Flipper Desktop press the BACK button until you are at the Desktop.
- Step 14. Press the down arrow on the flipper.
- Step 15. Press the right arrow on the flipper until it shows "Browser".
- Step 16. Select the update folder.
- Step 17. Select the update app.
- Step 18. Choose "Run in app"
- Step 19. Click the right arrow to do the "Install>"
- Wait about 3 minutes for the installer.
- Step 20. Your flipper should show "Firmware updated. Success. OK>"
- Step 21. Click "OK"


## Using SD card to update with custom firmware
These are directions if you prefer to do the update via SD card instead of USB cable.  (I typically do USB cable.)

- Step 1. If your flipper is not at the Flipper Desktop press the BACK button until you are at the Desktop.
- Step 2. Press the OK button on the flipper.
- Step 3. Choose "Settings"
- Step 4. Choose "Storage"
- Step 5. Choose "Unmount SD card"
- Step 6. Click the right arrow to do the "Unmount>"
- Step 7. Click the OK button on the flipper.
- Step 8. Click the back button. (You should be back in the "Settings" menu)
- Step 9. Choose "Power"
- Step 10. Choose "Power OFF"
- Step 11. Click the right arrow to do the "OFF>"
- Step 12. Eject the SD card.
- Step 13. Insert the SD card into your PC.
- Step 14. Delete all of the files from the SD card (or at least the manifest file and update folder if it exists)
- Step 15. Create a folder called "update" in the root of the SD card.
- Step 16. Copy all of the files from c:\repo\flipperzero-firmware\dist\f7-D\f7-update-local folder into the "update" folder.
- Step 17. Safely eject the SD card.
- Step 18. Insert the SD card into the flipper.
- Step 19. Hold the back button on the flipper until it powers on.
- Step 20. Press the down arrow on the flipper.
- Step 21. Press the right arrow on the flipper until it shows "Browser".
- Step 22. Select the update folder.
- Step 23. Select the update app.
- Step 24. Choose "Run in app"
- Step 25. Click the right arrow to do the "Install>"
- Wait about 3 minutes for the installer.
- Step 26. Your flipper should show "Firmware updated. Success. OK>"
- Step 27. Click "OK"
