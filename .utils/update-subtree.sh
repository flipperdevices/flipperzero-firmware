#!/bin/bash
set -e

path=${GIT_PREFIX%/}
read repo branch subdir < ${GIT_PREFIX}.gitsubtree

if [ "${subdir}" = "" ]; then
    git subtree pull -P ${path} ${repo} ${branch} -m "Merge ${path} from ${repo}"
else
    bash .utils/subtree-subdir-helper.sh ${path} ${repo} ${branch} ${subdir} merge
fi
