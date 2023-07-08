#!/bin/bash

cd "$(dirname "$0")"
mkdir dist

CGO_LDFLAGS='-fstack-protector' GOOS=windows GOARCH=amd64 CGO_ENABLED=1 CXX=x86_64-w64-mingw32-g++ CC=x86_64-w64-mingw32-gcc \
go build -o dist -ldflags "-linkmode external -extldflags -static" $@

go build -o dist $@
