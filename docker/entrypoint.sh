#!/bin/sh

if [ -z "$1" ]; then
    sh
else
    echo "Running $1"
    set -ex
    sh -c "$1"
fi
