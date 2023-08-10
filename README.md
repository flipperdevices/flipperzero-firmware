#secret_toggle

A game for the Flipper Zero device.

## Instructions

- Arrow buttons move around the board.
- Push center OK button to toggle a square. Some squares are connected.
- Turn all of the squares on (light colored) to pass the level.
- Levels get harder as you go.
- Beat level 10 and you win.

## Compiling

sudo usermod -a -G dialout username
...reboot machine to be added to dialout group for access to /dev/ttyACM0
python3 -m pip install --user --upgrade ufbt
cd secret_toggle
ufbt -h
ufbt update
ufbt
...connect flipper over usb
ufbt launch

## Validating Manifest

sudo apt-get install python3.10-venv
cd flipper-application-catalog
python3 -m venv venv
source venv/bin/activate
pip install -r tools/requirements.txt
python3 tools/bundle.py applications/Games/secret_toggle/manifest.yml bundle.zip
