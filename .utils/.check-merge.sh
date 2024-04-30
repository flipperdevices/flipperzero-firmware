#!/bin/bash
set -e

if [ "$(git rev-parse --show-prefix)" != "" ]; then
    echo "Must be in root of git repo!"
    exit
fi

if [ "${1}" = "" ] || [ "${2}" = "" ]; then
    echo "Usage: <path> <repo url> <merge output>"
    exit
fi
path="${1}"
repo="${2}"
result="${3}"

if grep "Automatic merge failed; fix conflicts and then commit the result." <<< "$result" > /dev/null; then
    echo "MERGE_MSG: Merge ${path} from ${repo}"
    notify-send -a Git -i git "Subtree merge failed" "Resolve current index to continue" &> /dev/null | true
    while true; do
        echo "Resolve current index then press Enter..."
        read
        if git diff --quiet && git diff --cached --quiet && git merge HEAD &> /dev/null; then
            break
        fi
        sleep 1
    done
fi
if grep "Please commit your changes or stash them before you switch branches." <<< "$result" > /dev/null; then
    exit 1
fi
if grep "^fatal: " <<< "$result" > /dev/null; then
    exit 1
fi
