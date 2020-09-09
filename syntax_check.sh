#!/usr/bin/env bash

echo "RUN SYNTAX CHECK INSIDE CONTAINER"
sudo docker exec -it flipperzero-firmware-community_dev_1 /syntax_check.sh
