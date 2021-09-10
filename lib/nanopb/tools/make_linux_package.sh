#!/bin/bash

# Run this script in the top nanopb directory to create a binary package
# for Linux users.

# Requires: protobuf, python-protobuf, pyinstaller

set -e
set -x

VERSION=`git describe --always`-linux-x86
DEST=dist/$VERSION

rm -rf $DEST
mkdir -p $DEST

# Export the files from newest commit
git archive HEAD | tar x -C $DEST

# Rebuild the Python .proto files and .pyc
( cd $DEST/generator; python3 nanopb_generator.py ||: )

# Package the Python libraries
( cd $DEST/generator; python3 -m PyInstaller nanopb_generator.py )
( cd $DEST/generator; python3 -m PyInstaller protoc  )
mv $DEST/generator/dist/nanopb_generator $DEST/generator-bin
cp $DEST/generator/dist/protoc/protoc $DEST/generator-bin

# Include Google's descriptor.proto and nanopb.proto
cp -pr $(python3 -c 'import grpc_tools, os.path; print(os.path.dirname(grpc_tools.__file__))')/_proto $DEST/generator-bin/grpc_tools/
cp -pr $DEST/generator/proto $DEST/generator-bin/proto

# Remove temp files
rm -rf $DEST/generator/dist $DEST/generator/build $DEST/generator/nanopb_generator.spec

# Make the nanopb generator available as a protoc plugin
cp $DEST/generator-bin/nanopb_generator $DEST/generator-bin/protoc-gen-nanopb

# Remove debugging symbols to reduce size of package
( cd $DEST/generator-bin; strip *.so *.so.* )

# Tar it all up
( cd dist; tar -czf $VERSION.tar.gz $VERSION )

