#!/bin/bash

cd "$(dirname "$0")"

FW_URL="$1"
FW_VERSION="${2:-latest}"

set -e

if [[ -z "$FW_URL" ]]; then
    echo "usage: [FW_GIT_URL] [GIT_TAG]";
    exit 1;
fi

if [[ "$FW_VERSION" == "latest" ]]; then
    repo="$(echo "$FW_URL" | sed -E 's,https?://github.com/([^/]+/[^/]+),\1,')"
    FW_VERSION="$(curl --silent "https://api.github.com/repos/$repo/releases/latest" | # Get latest release from GitHub api
        grep '"tag_name":' |                                            # Get tag line
        sed -E 's/.*"([^"]+)".*/\1/')"                                    # Pluck JSON value

    if [[ -z "$FW_VERSION" || -z "$repo" ]]; then
        echo "Unable to get latest github release tag"
        exit 1
    fi
fi

if [[ -z "$RUNS_IN_DOCKER" ]]; then
    # is set in the Dockerfile
    git submodule update --init
    docker build -t flipper_hidtransfer_build:latest \
        --build-arg UID=$(id -u ${USER}) \
        --build-arg GID=$(id -g ${USER}) \
        - < Dockerfile

    docker run --rm  \
        -v "$(pwd):/build" \
        flipper_hidtransfer_build:latest \
        /build/build.sh "$FW_URL" "$FW_VERSION"
    exit 0;
fi

rm -rf dist || true
mkdir dist || true

if [[ ! -d "firmware_${FW_VERSION}" ]]; then
    git clone --depth 1 --branch "$FW_VERSION" "$FW_URL" "firmware_${FW_VERSION}"
fi

cd "firmware_${FW_VERSION}"
# pwd = /build/firmware
ln -sf ../../flipperApp applications_user/
sed -i 's/"-Werror",//' site_scons/cc.scons

./fbt fap_hidtransfer
mkdir -p ../dist/apps/Tools
cp build/f7-firmware-D/.extapps/hidtransfer.fap ../dist/apps/Tools

cd ../full-client
# pwd = /build/full-client
mkdir -p ../dist/HIDTransfer
./compile.sh main2.go
cp dist/main2 ../dist/HIDTransfer/HIDFullClient
cp dist/main2.exe ../dist/HIDTransfer/HIDFullClient.exe

cd ../dist/HIDTransfer
# pwd = /build/dist/HIDTransfer
zip -9 HIDFullClient.exe.zip HIDFullClient.exe
zip -9 HIDFullClient.zip HIDFullClient

cd ../../minimal-client
# pwd = /build/minimal-client
make all

mkdir ../dist/badusb
python3 ../CreateBadUSBPayload.py -out ../dist/badusb/HIDMinimalClient.exe.txt -in build/windows/HIDMinimalClient.exe --format powershell
python3 ../CreateBadUSBPayload.py -out ../dist/badusb/HIDMinimalClient.txt -in build/linux/HIDMinimalClient --format bash


