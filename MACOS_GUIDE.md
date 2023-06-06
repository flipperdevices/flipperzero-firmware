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

### Updating using qFlipper
1. Connect the Flipper Zero to your pc.

![image](https://github.com/EltonAlvess/flipperzero-firmware-wPlugins/assets/1638045/e2be7a6b-2ff2-46f5-9312-b928cf4cf560)

2. Click in ***Install from File***
3. Select the compiled firmware file created on the root folder i.e ***RM0606-0922-0.84.3-36d65f3.tgz***

![image](https://github.com/EltonAlvess/flipperzero-firmware-wPlugins/assets/1638045/4220f68a-b775-47a9-a2d6-2eae33fc8c70)

4. Click Install.

---

> If everything worked, your Flipper should be recognized and the firmware version should match the github commit shown here:
>
> [https://github.com/RogueMaster/flipperzero-firmware-wPlugins](https://github.com/RogueMaster/flipperzero-firmware-wPlugins)
