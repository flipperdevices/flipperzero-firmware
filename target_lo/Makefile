TARGET = target_lo

######################################
# building variables
######################################
# debug build?
DEBUG = 1

#######################################
# paths
#######################################
PROJECT_DIR = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
# build path
BUILD_DIR = build

#######################################
# rust library
#######################################

RUST_LIB_SRC = $(realpath $(PROJECT_DIR)/../core-rs)
RUST_LIB_NAME = flipper_core
RUST_LIB_TARGET = x86_64-unknown-linux-gnu
RUST_LIB_FLAGS = --target=$(RUST_LIB_TARGET)

ifeq ($(DEBUG), 1)
    RUST_LIB_PATH = $(RUST_LIB_SRC)/target/$(RUST_LIB_TARGET)/debug
else
    RUST_LIB_FLAGS += --release
    RUST_LIB_PATH = $(RUST_LIB_SRC)/target/$(RUST_LIB_TARGET)/release
endif

#######################################
# build the application
#######################################

all: zephyr rust_lib
	west build code

zephyr:
	west update

rust_lib:
	cd $(RUST_LIB_SRC) && cargo build -p flipper-core $(RUST_LIB_FLAGS)

.PHONY: all rust_lib

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
	cd $(RUST_LIB_SRC) && cargo clean

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
