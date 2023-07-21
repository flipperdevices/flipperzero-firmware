#!/bin/bash

echo "Installing ESP8266 Arduino Core ..."
script_init_path="$PWD"
mkdir -p "$ARDUINO_USR_PATH/hardware/esp8266com"
cd "$ARDUINO_USR_PATH/hardware/esp8266com"

echo "Installing Python Serial ..."
pip install pyserial > /dev/null

if [ "$OS_IS_WINDOWS" == "1" ]; then
	echo "Installing Python Requests ..."
	pip install requests > /dev/null
fi

echo "Cloning Core Repository ..."
git clone https://github.com/esp8266/Arduino.git esp8266 > /dev/null 2>&1

echo "Updating submodules ..."
cd esp8266
git submodule update --init --recursive > /dev/null 2>&1

echo "Installing Platform Tools ..."
cd tools
python get.py > /dev/null
cd $script_init_path

echo "ESP8266 Arduino has been installed in '$ARDUINO_USR_PATH/hardware/esp8266com'"
echo ""
