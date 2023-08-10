#!/bin/bash

keyList=(
            chainA-ICA1-key.pem chainA-entity-key.pem
            chainB-ICA2-key.pem chainB-ICA1-key.pem chainB-entity-key.pem
            chainC-ICA1-key.pem chainC-entity-key.pem
            chainD-ICA1-key.pem chainD-entity-key.pem
            chainE-ICA1-key.pem chainE-entity-key.pem
            chainF-ICA2-key.pem chainF-ICA1-key.pem chainF-entity-key.pem
            chainG-ICA7-key.pem chainG-ICA6-key.pem chainG-ICA5-key.pem
            chainG-ICA4-key.pem chainG-ICA3-key.pem chainG-ICA2-key.pem
            chainG-ICA1-key.pem chainG-entity-key.pem
            chainH-ICA4-key.pem chainH-ICA3-key.pem chainH-ICA2-key.pem
            chainH-ICA1-key.pem chainH-entity-key.pem
            chainI-ICA3-key.pem chainI-ICA2-key.pem chainI-ICA1-key.pem
            chainI-entity-key.pem
            chainJ-ICA4-key.pem chainJ-ICA3-key.pem chainJ-ICA2-key.pem
            chainJ-ICA1-key.pem chainJ-entity-key.pem
        )

for TARGET_KEY in "${keyList[@]}"
do
    if [ -f "$TARGET_KEY" ]; then
        printf '%s\n' "Removing $TARGET_KEY"
        rm "$TARGET_KEY"
    fi
    printf '%s\n' "Creating new $TARGET_KEY"
    openssl genrsa -out "$TARGET_KEY" 2048
done
