#!/bin/bash
set -e

path="${GIT_PREFIX%/}"
while read repo branch subdir; do
    if [ "${subdir}" = "/" ]; then
        git subtree pull -P "${path}" "${repo}" "${branch}" -m "Merge ${path} from ${repo}"
    else
        bash .utils/subtree-subdir-helper.sh "${path}" "${repo}" "${branch}" "${subdir}" merge
    fi
done < "${GIT_PREFIX}.gitsubtree"
