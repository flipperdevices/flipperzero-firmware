# FindMy Network - Location Reports with Generated AirTags

Query Apple's Find My network, allowing non-Apple devices to retrieve the location reports.

Video Tutorial: https://www.youtube.com/watch?v=XGwHmwvQoqo

This was made possible thanks to Chapoly1305 and his work bringing airtag tracking to non Apple devices:
1. https://github.com/Chapoly1305/FindMy
2. https://github.com/seemoo-lab/openhaystack/
3. https://github.com/biemster/FindMy
4. https://github.com/Dadoum/anisette-v3-server
5. https://github.com/mrmay-dev/owntags

Please give Chapoly1305 a star for his work, thanks!

## Prerequisites

Before you begin, ensure you have the following installed on your system:

- Docker Desktop
- Python (likely already installed)
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

Navigate to the AirTag generation directory, then create and activate a Python virtual environment.
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

Run the ```generate_keys.py``` script to generate the keys needed for AirTags, which will be saved in a new folder called 'keys'.


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

### On The Flipper: Configuration on the FlipperZero (if not completed yet)
- Upon launching the app, open the config menu and either click ```Import Tag From File``` or ```Register Tag Manually```. Put your generated .keys file onto the FlipperZero SD card inside the AppsData/FindMyFlipper folder to import from file. Or you can manually enter the tag information. When using the cloning method, you can export a .txt file from nrfConnect (click save button) amd place that in the same folder in order to import.

Customization

- Beacon Interval: Adjust how frequently your FlipperZero broadcasts its presence.
- Transmit Power: Increase or decrease the signal strength to balance between tracking range and battery life.

Background Use

The app is designed to have a negligible impact on battery life, even when running in the background. This allows for continuous tracking without the need for frequent recharging.
