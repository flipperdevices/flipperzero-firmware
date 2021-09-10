#!/bin/bash

# Run this script in the top nanopb directory to create a binary package
# for Windows users. This script is designed to run under MingW/MSYS bash
# and requires the following tools: git, make, zip, unix2dos

set -e
set -x

VERSION=`git describe --always`-windows-x86
DEST=dist/$VERSION

rm -rf $DEST
mkdir -p $DEST

# Export the files from newest commit
git archive HEAD | tar x -C $DEST

# Rebuild the Python .proto files and .pyc
( cd $DEST/generator; py -3 nanopb_generator.py ||: )

# Package the Python libraries
( cd $DEST/generator; py -3 -m PyInstaller nanopb_generator.py  )
( cd $DEST/generator; py -3 -m PyInstaller protoc  )
mv $DEST/generator/dist/nanopb_generator $DEST/generator-bin
cp $DEST/generator/dist/protoc/protoc.exe $DEST/generator-bin

# Include Google's descriptor.proto and nanopb.proto
cp -pr $(py -3 -c 'import grpc_tools, os.path; print(os.path.dirname(grpc_tools.__file__))')/_proto $DEST/generator-bin/grpc_tools/
cp -pr $DEST/generator/proto $DEST/generator-bin/proto

# Remove temp files
rm -rf $DEST/generator/dist $DEST/generator/build $DEST/generator/*.spec

# Make the nanopb generator available as a protoc plugin
cp $DEST/generator-bin/nanopb_generator.exe $DEST/generator-bin/protoc-gen-nanopb.exe

# Convert line breaks for convenience
find $DEST -name '*.c' -o -name '*.h' -o -name '*.txt' \
    -o -name '*.proto' -o -name '*.py' -o -name '*.options' \
    -exec sed -i 's/$/\r/' '{}' \;

# Zip it all up
( cd dist; rm -f $VERSION.zip; powershell "Compress-Archive $VERSION $VERSION.zip" )
