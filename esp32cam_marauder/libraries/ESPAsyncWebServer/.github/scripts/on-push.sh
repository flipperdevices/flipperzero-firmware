#!/bin/bash

set -e

if [ ! -z "$TRAVIS_BUILD_DIR" ]; then
	export GITHUB_WORKSPACE="$TRAVIS_BUILD_DIR"
	export GITHUB_REPOSITORY="$TRAVIS_REPO_SLUG"
elif [ -z "$GITHUB_WORKSPACE" ]; then
	export GITHUB_WORKSPACE="$PWD"
	export GITHUB_REPOSITORY="me-no-dev/ESPAsyncWebServer"
fi

TARGET_PLATFORM="$1"
CHUNK_INDEX=$2
CHUNKS_CNT=$3
BUILD_PIO=0
if [ "$#" -lt 1 ]; then
	TARGET_PLATFORM="esp32"
fi
if [ "$#" -lt 3 ] || [ "$CHUNKS_CNT" -le 0 ]; then
	CHUNK_INDEX=0
	CHUNKS_CNT=1
elif [ "$CHUNK_INDEX" -gt "$CHUNKS_CNT" ]; then
	CHUNK_INDEX=$CHUNKS_CNT
elif [ "$CHUNK_INDEX" -eq "$CHUNKS_CNT" ]; then
	BUILD_PIO=1
fi

if [ "$BUILD_PIO" -eq 0 ]; then
	# ArduinoIDE Test
	source ./.github/scripts/install-arduino-ide.sh

	echo "Installing ESPAsyncWebServer ..."
	cp -rf "$GITHUB_WORKSPACE" "$ARDUINO_USR_PATH/libraries/ESPAsyncWebServer"
	echo "Installing ArduinoJson ..."
	git clone https://github.com/bblanchon/ArduinoJson "$ARDUINO_USR_PATH/libraries/ArduinoJson" > /dev/null 2>&1

	if [[ "$TARGET_PLATFORM" == "esp32" ]]; then
		echo "Installing AsyncTCP ..."
		git clone https://github.com/me-no-dev/AsyncTCP "$ARDUINO_USR_PATH/libraries/AsyncTCP" > /dev/null 2>&1
		FQBN="espressif:esp32:esp32:PSRAM=enabled,PartitionScheme=huge_app"
		source ./.github/scripts/install-arduino-core-esp32.sh
		echo "BUILDING ESP32 EXAMPLES"
	else
		echo "Installing ESPAsyncTCP ..."
		git clone https://github.com/me-no-dev/ESPAsyncTCP "$ARDUINO_USR_PATH/libraries/ESPAsyncTCP" > /dev/null 2>&1
		FQBN="esp8266com:esp8266:generic:eesz=4M1M,ip=lm2f"
		source ./.github/scripts/install-arduino-core-esp8266.sh
		echo "BUILDING ESP8266 EXAMPLES"
	fi
	build_sketches "$FQBN" "$GITHUB_WORKSPACE/examples" "$CHUNK_INDEX" "$CHUNKS_CNT"
else
	# PlatformIO Test
	source ./.github/scripts/install-platformio.sh

	python -m platformio lib --storage-dir "$GITHUB_WORKSPACE" install
	echo "Installing ArduinoJson ..."
	python -m platformio lib -g install https://github.com/bblanchon/ArduinoJson.git > /dev/null 2>&1
	if [[ "$TARGET_PLATFORM" == "esp32" ]]; then
		BOARD="esp32dev"
		echo "Installing AsyncTCP ..."
		python -m platformio lib -g install https://github.com/me-no-dev/AsyncTCP.git > /dev/null 2>&1
		echo "BUILDING ESP32 EXAMPLES"
	else
		BOARD="esp12e"
		echo "Installing ESPAsyncTCP ..."
		python -m platformio lib -g install https://github.com/me-no-dev/ESPAsyncTCP.git > /dev/null 2>&1
		echo "BUILDING ESP8266 EXAMPLES"
	fi
	build_pio_sketches "$BOARD" "$GITHUB_WORKSPACE/examples"
fi
