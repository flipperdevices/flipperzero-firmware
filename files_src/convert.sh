#!/bin/sh

DIR=$(dirname $(readlink -f $0))
LC_ALL=C tr -d '\04\r\0' < "$DIR/russian.ucs2.dict" > "$DIR/../files/russian.dict"
LC_ALL=C tr -d '\04\r\0' < "$DIR/tatar.ucs2.dict" > "$DIR/../files/tatar.dict"
