## **macOS compile guide**  by deafbed, 
* Updated by **@eltonalvess**

### Tested with M1 Macbook Air, macOS Monterey (12.4)
### :arrow_up: Update
 * Tested with M1 Macbook Pro, macOS Ventura (13.4, 22F66) [06-06-2023]

## **FROM THE BEGINNING**

### Install qFlipper
- qFlipper can be found on the App Store or you can download it from [https://flipperzero.one/update](https://flipperzero.one/update)

---

### **Install the Homebrew application**

`/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`

Open `Terminal.app` or `iTerm.app` and run `source ~/.bashrc` of if you are using zsh the command should be `source ~/.zshrc` (It will refresh enviroment variables that Homebrew creates).

To have sure that Brew was installed sucessfully then run

`brew help` it should output something like this:

![out](https://github.com/EltonAlvess/flipperzero-firmware-wPlugins/assets/1638045/a3f56314-c9ff-4e73-9811-81042f1e731a)

---

### **Download the source code of the firmware**
To clone the repository, just run the command below:

`git clone --recursive https://github.com/RogueMaster/flipperzero-firmware-wPlugins.git`

It will download the latest build files from the github repository (in your prefered folder)

Inside the source code folder (flipperzero-firmware-wPlugins) run the follow command:

`chmod u+x buildRelease.sh`

> This command will give permission to execute the Shell Script (.sh).

---

### **Install the necessary prerequisites**

Go to the downloaded firmware folder

`cd flipperzero-firmware-wPlugins`

Next, we run the following command to download the remaining requirements:

`brew bundle --verbose`

---

### Compile

To compile the source code just run the following command inside the root folder.

`./buildRelease.sh`

> :sleeping: It will start to compile, be patient it take a while! 
---

### The output files
If everything goes well it will appear some 3 files in the root folder like the image below:

![image](https://github.com/EltonAlvess/flipperzero-firmware-wPlugins/assets/1638045/eec62f52-8a6a-4f3a-a293-196cb83aeba3)

---

### Put your Flipper into Recovery/DFU mode
Put your flipper into DFU mode by:

1. holding BACK + LEFT until it turns off
2. once turned off, release BACK but keep holding LEFT until the LED flashes blue
3. once the LED flashes blue, release LEFT and your Flipper should be in recovery mode

---

### Install the firmware
Open qFlipper.app

Connect your Flipper via USB

In qFlipper, click "Install from file" (underneath the big INSTALL button)

Navigate to your DFU file (**flipperzero-firmware-wPlugins/dist/f7-C/flipper-z-f7-full-local.dfu**)

Wait for the process to finish. If your Flipper device is no longer in DFU mode (i.e. shows the normal Desktop), unplug your Flipper and close the qFlipper app

### Post-Install
Open qFlipper.app and connect your Flipper via USB

If everything worked, your Flipper should be recognized and the firmware version should match the github commit shown here:
[https://github.com/RogueMaster/flipperzero-firmware-wPlugins](https://github.com/RogueMaster/flipperzero-firmware-wPlugins)
