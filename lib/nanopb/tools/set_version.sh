#!/bin/bash

# Run this from the top directory of nanopb tree.
# e.g. user@localhost:~/nanopb$ tools/set_version.sh nanopb-0.1.9-dev
# It sets the version number in pb.h and generator/nanopb_generator.py.

sed -i -e 's/nanopb_version\s*=\s*"[^"]*"/nanopb_version = "'$1'"/' generator/nanopb_generator.py
sed -i -e 's/#define\s*NANOPB_VERSION\s*.*/#define NANOPB_VERSION '$1'/' pb.h
sed -i -e 's/set(\s*nanopb_VERSION_STRING\s*[^)]*)/set(nanopb_VERSION_STRING '$1')/' CMakeLists.txt

VERSION_ONLY=$(echo $1 | sed 's/nanopb-//')
if [[ $1 != *dev ]]
then sed -i -e 's/"version":\s*"[^"]*"/"version": "'$VERSION_ONLY'"/' library.json
fi

sed -i -e 's/version =.*/version = "'$VERSION_ONLY'"/' conanfile.py
sed -i -e 's/^version =.*/version = "'$VERSION_ONLY'"/' extra/poetry/pyproject.toml
