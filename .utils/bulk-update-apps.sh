#!/bin/bash
set -e

for file in */.gitsubtree; do
    app="$(dirname "${file}")"
    echo -e "\n\nUpdating ${app}..."
    pushd "${app}" > /dev/null
    git update-app
    popd > /dev/null
done
