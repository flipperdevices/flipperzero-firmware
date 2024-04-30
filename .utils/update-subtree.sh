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
    if [ "${subdir}" = "/" ]; then
        exec {capture}>&1
        result="$(git subtree pull -P "${path}" "${repo}" "${branch}" -m "Merge ${path} from ${repo}" 2>&1 | tee /proc/self/fd/$capture)"
        bash .utils/.check-merge.sh "${path}" "${repo}" "${result}"
    else
        bash .utils/.subtree-subdir-helper.sh "${path}" "${repo}" "${branch}" "${subdir}" merge
    fi
done {remote}< "${path}/.gitsubtree"
