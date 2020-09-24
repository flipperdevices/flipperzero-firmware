#!/bin/bash

dfu-util -D `dirname "$0"`/target_f2/build/target_prod.bin -a 0 -s 0x08000000
