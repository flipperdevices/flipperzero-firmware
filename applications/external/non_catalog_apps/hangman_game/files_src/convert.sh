#!/bin/sh

DIR=$(dirname $(readlink -f $0))
tr -d '\04\r\0' < "$DIR/russian.ucs2.dict" > "$DIR/../files/russian.dict"
