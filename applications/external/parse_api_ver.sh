#!/bin/bash
# Parse API Version from api_symbols.csv
FILE=unleashed-firmware/targets/f7/api_symbols.csv
OLDIFS=$IFS
IFS=','
[ ! -f $FILE ] && { echo "$FILE not found"; exit 99; }
while read entry status name type params
do
	if [ "$entry" == "Version" ]; then
		echo "API: $name"
		echo '' >> CHANGELOG.md
		echo "Apps build for Unleashed FW with API version: $name" >> CHANGELOG.md
		echo '' >> CHANGELOG.md
		break;
	fi
done < $FILE
IFS=$OLDIFS
