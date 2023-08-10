#How to build with MQX
## Overview
This Makefile is for building wolfSSL library and sample programs running with MQX.
It has following targets.
 - wolfssllib: wolfSSL static library (libwolfssl.a)
 - test: crypt test
 - benchmark: cypher benchmark
 - client: TLS client example
 - server: TLS server example

## Prerequisites
- Installed GCC
  Download from GNU Arm Embedded Toolchain at https://developer.arm.com/
- Installed MQX
  Download Freescale MQX RTOS 4.1 or later at https://www.nxp.com/
  Follow Freescale_MQX_4_1/doc/MQX_Getting_Started.pdf

## Setup
- wolfSSL configuration parameters
  You can add or remove configuration options in <wolfSSL-root>/IDE/MQX/user_settings.h.

- Setup Makefile
  MQX_ROOT: MQX source code installed path
  MQXLIB:   MQX library path to like with
  CC:       compiler
  AR:       archiver
  WOLF_ROOT: change this if you move this Makefile location
