#!/bin/bash
set -e

if [ "${1}" = "" ] || [ "${2}" = "" ] || [ "${3}" = "" ] || [ "${4}" = "" ] || [ "${5}" = "" ]; then
    echo "Usage: <path> <repo> <branch> <subdir> <action>"
    exit
fi
path="${1}"
repo="${2}"
branch="${3}"
subdir="${4}"
action="${5}"

prev="$(git branch --show-current)"
temp="$(rev <<< "${repo%/}" | cut -d/ -f1,2 | rev | tr / -)-${branch}"
fetch="_fetch-${temp}"
split="_split-${temp}-$(tr / - <<< "${subdir}")"
git fetch --no-tags "${repo}" "${branch}:${fetch}"
git checkout "${fetch}"
git subtree split -P "${subdir}" -b "${split}"
git checkout "${prev}"

exec 69>&1
result="$(git subtree "${action}" -P "${path}" "${split}" -m "${action^} ${path} from ${repo}" 2>&1 | tee /proc/self/fd/69)"
if grep "refusing to merge unrelated histories" <<< "$result" > /dev/null; then
    echo "History at ${repo} is unrelated, using squash..."
    git subtree "${action}" -P "${path}" "${split}" -m "${action^} ${path} from ${repo}" --squash
fi
