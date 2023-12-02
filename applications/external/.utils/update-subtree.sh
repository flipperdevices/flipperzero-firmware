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

while read repo branch subdir; do
    if [ "${repo:0:1}" = "#" ]; then
        continue
    fi
    exec 69>&1
    if [ "${subdir}" = "/" ]; then
        result="$(git subtree pull -P "${path}" "${repo}" "${branch}" -m "Merge ${path} from ${repo}" 2>&1 | tee /proc/self/fd/69)"
    else
        result="$(bash .utils/subtree-subdir-helper.sh "${path}" "${repo}" "${branch}" "${subdir}" merge 2>&1 | tee /proc/self/fd/69)"
    fi
    if grep "Automatic merge failed; fix conflicts and then commit the result." <<< "$result" > /dev/null; then
        echo "MERGE_MSG: Merge ${path} from ${repo}"
        echo "Waiting for current index to be resolved..."
        notify-send -a Git -i git "Subtree merge failed" "Resolve current index to continue" &> /dev/null | true
        while ! git diff --quiet || ! git diff --cached --quiet || ! git merge HEAD &> /dev/null; do
            sleep 1
        done
    fi
done < "${path}/.gitsubtree"
