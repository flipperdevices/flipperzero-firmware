#!/bin/bash
set -e

if [ "${1}" = "" ] || [ "${2}" = "" ] || [ "${3}" = "" ] || [ "${4}" = "" ] || [ "${5}" = "" ]; then
    echo "Usage: <path> <repo> <branch> <subdir> <action>"
    exit
fi
path=${1}
repo=${2}
branch=${3}
subdir=${4}
action=${5}

prev=$(git branch --show-current)
temp=$(echo ${repo%/} | rev | cut -d/ -f1,2 | rev | tr / -)-${branch}
fetch=_fetch-${temp}
split=_split-${temp}-$(echo ${subdir} | tr / -)
git fetch --no-tags ${repo} ${branch}:${fetch}
git submodule deinit --all -f
git checkout ${fetch}
git subtree split -P ${subdir} -b ${split}
git checkout ${prev}
git subtree ${action} -P ${path} ${split} -m "${action^} ${path} from ${repo}"
git submodule update --init
