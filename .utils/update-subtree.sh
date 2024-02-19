#!/bin/bash
set -e

if [ "$(git rev-parse --show-prefix)" != "" ]; then
    echo "Must be in root of git repo!"
    exit
fi

if [ "${1}" = "" ]; then
    echo "Usage: <path>"
    exit
fi
path="${1}"

while read -u $remote repo branch subdir; do
    if [ "${repo:0:1}" = "#" ]; then
        continue
    fi
    exec {capture}>&1
    if [ "${subdir}" = "/" ]; then
        result="$(git subtree pull -P "${path}" "${repo}" "${branch}" -m "Merge ${path} from ${repo}" 2>&1 | tee /proc/self/fd/$capture)"
    else
        result="$(bash .utils/subtree-subdir-helper.sh "${path}" "${repo}" "${branch}" "${subdir}" merge 2>&1 | tee /proc/self/fd/$capture)"
    fi
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
    if grep "fatal: working tree has modifications." <<< "$result" > /dev/null; then
        exit 1
    fi
done {remote}< "${path}/.gitsubtree"
