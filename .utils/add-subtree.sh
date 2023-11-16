#!/bin/bash
set -e

if [ "$1" = "" ] || [ "$2" = "" ]; then
    echo "Usage 1: git add-subtree <path> <repo url> <branch> [subdir]"
    echo "Usage 2: git add-subtree <path> <repo url>/tree/<branch>[/subdir]"
    exit
fi
path="${1%/}"
repo="${2%/}"
if [ "$3" = "" ]; then
    read repo branch subdir <<< "$(sed -E "s|(https?://[^/]+)/([^/]+)/([^/]+)/tree/([^/]+)/(.*)|\1/\2/\3 \4 \5|" <<< "${repo}")"
else
    branch="${3}"
    subdir="${4%/}"
fi

if [ "${subdir}" = "" ]; then
    subdir="/"
    git subtree add -P "${path}" "${repo}" "${branch}" -m "Add ${path} from ${repo}"
else
    bash .utils/subtree-subdir-helper.sh "${path}" "${repo}" "${branch}" "${subdir}" add
fi

gitsubtree="${path}/.gitsubtree"
echo "${repo} ${branch} ${subdir}" > "${gitsubtree}"
git add "${gitsubtree}"
git commit --amend --no-edit
