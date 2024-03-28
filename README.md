# FindMy Flipper - AirTag and SmartTag Emulator

This app extends the functionality of the FlipperZero's bluetooth capabilities, enabling it to act as an Apple AirTag, Samsung SmartTag, or a Tile Tracker. It utilizes the FlipperZero's BLE beacon to broadcast a SmartTag signal to be picked up by the FindMy Network. I made this to serve as a versatile tool for tracking purposes, offering the ability to clone existing tags, generate OpenHaystack key pairs for integration with Apple's FindMy network, and always know where your FlipperZero is.

## Features

1. Tag Emulation: Clone your existing Apple AirTag, Samsung SmartTag, or Tile Tracker to the FlipperZero, or generate a key pair for use with the FindMy network without owning an actual AirTag.
2. Customization: Users can adjust the interval between beacon broadcasts and modify the transmit power to suit their needs, optimizing for both visibility and battery life.
3. Efficient Background Operation: The app is optimized to run in the background, ensuring that your FlipperZero can still be tracked with minimal battery usage and without stopping normal use.

## Usage Guide

### Step 1: Installation
- **Option A:** Use the released/precompiled firmware appropriate (FAP) for your device.
- **Option B:** Build the firmware yourself using `fbt/ufbt`.
- Both Installation options require you to be running a dev build of firmware. When release gets access to the extra BLE beacon this will change, thank you!
- All firmware should now work with main branch, including icons
  
### Step 2: Obtaining SmartTag Data
###### There are 2 methods to get SmartTag data depending on the type of tag you wish to emulate. Option A allows you to use Apple, Samsung, and Tile tags through the use of cloning the MAC Address and Payload of an actual tag. This also allows you to use the native app for tracking (Apple FindMy, Samsung SmartThing, Tile App). Option B allows you to emulate an Apple AirTag without needing to own an Apple device or airtag. This is done through key generation and requires a computer to download the location data.

<details>
  <summary><b>Option A: Cloning Existing Tag (Preferred and allows you to track without additional setup)</b></summary>

1. **Pair a Tag:** First, pair an AirTag, Samsung SmartTag or Tile Tracker with your device.
2. **Enter 'Lost' Mode:** Keep the tag away from the device it's registered to for approximately 15 minutes.
3. **Download nrfConnect or use an ESP32** Install nrfConnect from the Google Play Store. (Apple version doesn't reveal the needed Raw data, looking for a workaround)
4. OR **Use an ESP32-WROOM / ESP32-S3** Don't have an android? No problem! You can get all the data you need from an ESP32: https://github.com/MatthewKuKanich/ESP32-AirTag-Scanner (Skip to step 7 if using an ESP32)
5. **Filter and Scan:**
   - Open the app, click on filters, and exclude all except for the brand of your tag (Apple/Samsung).
   - Adjust the RSSI to the lowest setting (-40 dBm).
   - Initiate a scan. Wait for your SmartTag to appear as a "FindMy" device.
6. **Capture Data:** Click **Raw** or **View Raw** to capture your **payload** and note your tag's **MAC Address**. Immediately remove the tag's battery to prevent key/MAC rotation.
   - The AirTag has now been cloned, if you put the battery back into the AirTag it will eventually rotate its keys with the Apple servers. This will invalidate the current keys on the Flipper. To prevent this you must keep the AirTag powered off / battery removed. You can always repeat these steps to return functionality to the Flipper. No need to get rid of the AirTag, you can always return to it if you desire, you just can't use both at the same time.
8. **Enter Data in FlipperZero App:** Input the captured **payload** and **MAC Address** into the FlipperZero app.
</details>

<details>
<summary><b>Option B: AirTag Key Generation</b></summary>
  <br>

Video Tutorial: https://youtu.be/XGwHmwvQoqo?si=CAsKWEqGP5VFi9p9

### Prerequisites

Before you begin, ensure you have the following installed on your system:

- Docker Desktop
- Python
- Git

## Step-by-Step Instructions

### 1. Clone the Repository

Navigate to Matthew KuKanich's GitHub repository, copy the repository URL, and clone it to your desired location using the terminal.
```
git clone https://github.com/MatthewKuKanich/FindMyFlipper.git
```
### 2. Set Up the AirTag Generation Folder

Inside the cloned repository, locate the 'air tag generation' folder which contains all necessary files for creating AirTags.

### 3. Start Docker Desktop

Ensure Docker Desktop is running on your computer, as it is required for the server setup.

### 4. Set Up a Server Using Docker

Run the following Docker command to set up the server. This server emulates an environment that tricks Apple's authentication servers.
```
docker run -d --restart always --name anisette-v3 -p 6969:6969 dadoum/anisette-v3-server:latest
```
### 5. Create a Python Virtual Environment

Navigate to the AirTag generation directory, then create and activate a Python virtual environment. You will run all scripts in this terminal.
```
cd AirTagGeneration
```
```
python3 -m venv venv
```
(or `python -m venv venv`)

Activate the environment:
 - Windows:
```
.\venv\Scripts\activate.bat
```
 - Mac/Linux:
```
source venv/bin/activate
```
### 6. Install the Required Python Packages
```
pip3 install -r requirements.txt
```
### 7. Generate Keys for AirTags

Run the ```generate_keys.py``` script in the current terminal to generate the keys needed for AirTags, which will be saved in a new folder called 'keys'.


### 8. Transfer the Generated Keys to Flipper Zero

Move the '.Keys' file to your Flipper device by connecting it to your computer and using the Flipper's file management system.
   - For ease of use, drag your `.keys` file onto your FlipperZero's SD card in the apps_data->findmy folder. You can import it directly from the app!
     1. Open the app and navigate to the config menu.
     2. Choose "register tag" and select the tag type.
     3. Either click import `.keys`, `.txt`, or enter Manually.
     4. If entering manually then a MAC and payload dialog will appear next. Enter your **MAC** then **Payload** here.
     5. Click save.

### 9. Request Location Reports

Use the ```request_reports.py``` script to request real-time location data, requiring your Apple ID and password for authentication. This will save your Apple login information to a auth file so you won't need to re-enter your Apple credentials. 

### 10. Generate an Advanced Location Map

Finally, run the ```RequestReport&Map.py``` script to generate an interactive map of all location data in the past 24 hours. This script automates the process by requesting the location report using the hashed adv key in your ```keys``` folder, then decrypting that data from your private key located in the same `.keys` file. After the data is decrypted it will be displayed in the terminal. It then launches a mapping script that maps all the coordinates, connects them to show movement, displays a plethora of location metadata, and saves to an html file named by the date of the report.

You're done!

 - If you want to use OpenHaystack or Macless instead, then you can follow the steps below. I don't recommend these methods due to reliability issues and setup complexity.
To use OpenHayStack for tracking, you must use MacOS lower than version 14 (Mail Plug-in Incompetiablity of MacOS 14+ seemoo-lab/openhaystack#224). If you do own a device, I believe a convertor script can be provided without much of effort. If you do not own a Mac device or the system has been upgraded to 14 and beyond. The alternative solutions includes,

    https://github.com/dchristl/macless-haystack
    
If using this solution, be sure to only use the `generate_keys.py` script from this repo in the AirTagGeneration folder. Not the ones included in that repo as the formatting of the key file changes. (Mine includes data that the FlipperZero needs for proper importing)
</details>

### On The Flipper: Configuration on the FlipperZero (if not completed yet)
- Upon launching the app, open the config menu and either click ```Import Tag From File``` or ```Register Tag Manually```. Put your generated .keys file onto the FlipperZero SD card inside the AppsData/FindMyFlipper folder to import from file. Or you can manually enter the tag information. When using the cloning method, you can export a .txt file from nrfConnect (click save button) amd place that in the same folder in order to import.

Customization

- Beacon Interval: Adjust how frequently your FlipperZero broadcasts its presence.
- Transmit Power: Increase or decrease the signal strength to balance between tracking range and battery life.

Background Use

The app is designed to have a negligible impact on battery life, even when running in the background. This allows for continuous tracking without the need for frequent recharging.

Compatibility

- Apple devices for AirTag tracking via the FindMy network.
- Any device that supports Samsung SmartTag tracking, including web browsers (previously FindMyMobile).
- Tile Trackers via the Tile App

Thanks

- Huge thanks to all the people that contributed to the OpenHaystack project, supporting projects, and guides on the subject. This wouldn't be a thing without any of you! Special thanks to Chapoly1305 for introducing me to the FindMy network and WillyJL for helping get the app input working and overall overhaul of the apps functions!

Legal and Privacy

This app is intended for personal and educational use. Users are responsible for complying with local privacy laws and regulations regarding tracking devices. The cloning and emulation of tracking tags should be done responsibly and with respect to the ownership of the original devices.

Disclaimer

This project is not affiliated with Apple Inc. or Samsung. All product names, logos, and brands are property of their respective owners. Use this app responsibly and ethically.
