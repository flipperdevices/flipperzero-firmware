#!/bin/bash
# Parse API Version from api_symbols.csv
FILE=unleashed-firmware/firmware/targets/f7/api_symbols.csv
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
		echo "Extra pack = all apps / Base pack = apps that was in UL by default" >> CHANGELOG.md
		break;
	fi
done < $FILE
IFS=$OLDIFS
