macOS compile by deafbed

(NB: do NOT include the "$ "if copying/pasting any of the below commands) 

INSTALL BREW
$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
IMPORTANT: once installed, run the two commands given to you ("Next steps")
(first should begin $ echo 'eval...', second should begin $ eval "$(/opt/...))
reboot Terminal, run
$ brew help
to check installation was successful

INSTALL DOCKER DESKTOP
https://docs.docker.com/desktop/mac/install/
download appropriate version (intel/M1)
(if downloaded file has no extension/won't open, simply add .dmg to the filename)
open .dmg, drag Docker to Applications
eject .dmg, open Docker app, wait for setup to complete
restart Terminal, run
$ docker help
to check if successful

MAKE SURE PIP IS UP TO DATE
$ python3 -m pip install --upgrade pip

DOWNLOAD FW
$ git clone --recursive https://github.com/RogueMaster/flipperzero-firmware-wPlugins.git

INSTALL NECESSARY PREREQUISITES
$ cd flipperzero-firmware-wPlugins
$ brew bundle --verbose
$ pip3 install -r scripts/requirements.txt

COMPILE
make sure Docker app is STILL OPEN
in Terminal, double check you are in the flipperzero-firmware-wPlugins directory and run
$ docker-compose up -d
$ docker-compose exec dev ./fbt
the compiled firmware (DFU) file should now be in flipperzero-firmware-wPlugins/dist/f7-C/flipper-z-f7-full-local.dfu

RECOVERY MODE
put your flipper into DFU mode by:
1. holding BACK + LEFT until it turns off
2. once turned off, release BACK but keep holding LEFT until the LED flashes blue
3. once the LED flashes blue, release LEFT and your Flipper should be in recovery mode

INSTALL
open qFlipper
connect your Flipper via USB
in the qFlipper app, "Install from file" (underneath the big INSTALL button)
navigate to your DFU file (flipperzero-firmware-wPlugins/dist/f7-C/flipper-z-f7-full-local.dfu)
wait for the process to finish. if your Flipper device is no longer in DFU mode (i.e. shows the normal Desktop), unplug your Flipper and close the qFlipper app

POST-INSTALL
open the qFlipper app and connect your Flipper
if everything worked, your Flipper should be recognized by the app
the firmware in the box on the left should match the github commit here:
https://github.com/RogueMaster/flipperzero-firmware-wPlugins
