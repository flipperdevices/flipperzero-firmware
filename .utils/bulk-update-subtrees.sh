#!/bin/bash
set -e

for file in */.gitsubtree; do
    subtree="$(dirname "${file}")"
    echo -e "\n\nUpdating ${subtree}..."
    pushd "${subtree}" > /dev/null
    git update-subtree
    popd > /dev/null
done
