#!/bin/bash

sed -ir '/^\s*$/d' AUTHORS.txt # Remove empty lines

echo "New authors:"
git log --format='%aN <%aE>' | grep -vFf AUTHORS.txt | sort -u

