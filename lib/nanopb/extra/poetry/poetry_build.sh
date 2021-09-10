#!/bin/bash

set -ex

rm -rf build
mkdir build
mkdir -p dist

(cd "$(git rev-parse --show-toplevel)"; git archive HEAD) > build/tmp.tar
cd build
ln -s ../dist .

mkdir nanopb
tar xf tmp.tar README.md generator
mv generator nanopb/
touch nanopb/__init__.py nanopb/generator/__init__.py
make -C nanopb/generator/proto
cp ../pyproject.toml .
sed -i -e 's/\(version =.*\)-dev.*/\1-dev'$(git rev-list HEAD --count)'"/' pyproject.toml
poetry build
