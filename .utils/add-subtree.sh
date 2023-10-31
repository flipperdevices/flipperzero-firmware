#!/bin/bash
set -e

if [ "$1" = "" ] || [ "$2" = "" ] || [ "$3" = "" ]; then
    echo "Usage: git add-subtree <path> <repo> <branch> [subdir]"
    exit
fi
path=${1%/}
repo=${2%/}
branch=${3}
subdir=${4%/}

if [ "${subdir}" = "" ]; then
    git subtree add -P ${path} ${repo} ${branch} -m "Add ${path} from ${repo}"
else
    bash .utils/subtree-subdir-helper.sh ${path} ${repo} ${branch} ${subdir} add
fi

gitsubtree=${path}/.gitsubtree
echo ${repo} ${branch} ${subdir} > ${gitsubtree}
git add ${gitsubtree}
git commit --amend --no-edit
