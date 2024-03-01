#!/bin/bash
set -e

if [ "$(git rev-parse --show-prefix)" != "" ]; then
    echo "Must be in root of git repo!"
    exit
fi

for file in **/.gitsubtree; do
    subtree="$(dirname "${file}")"
    echo -e "\n\nUpdating ${subtree}..."
    bash .utils/update-subtree.sh "${subtree}"
done

notify-send -a Git -i git "Subtree bulk update finished" "Double check merge commits" &> /dev/null | true
