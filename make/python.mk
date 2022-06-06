TOOLCHAIN_GDB_SYM := $(shell which arm-none-eabi-gdb)
ifneq ($(TOOLCHAIN_GDB_SYM),)
	TOOLCHAIN_GDB := $(shell readlink -f -- $(TOOLCHAIN_GDB_SYM))
	TOOLCHAIN_DIR := $(shell dirname -- $(TOOLCHAIN_GDB))
	FLIPPER_PY_REL_DIR := $(TOOLCHAIN_DIR)/../python/bin
	FLIPPER_PY_DIR := $(shell test -d $(FLIPPER_PY_REL_DIR) && cd $(FLIPPER_PY_REL_DIR) && pwd)
	FLIPPER_PROTO_REL_DIR := $(TOOLCHAIN_DIR)/../protobuf/bin
	FLIPPER_PROTO_DIR := $(shell test -d $(FLIPPER_PROTO_REL_DIR) && cd $(FLIPPER_PROTO_REL_DIR) && pwd)
	FLIPPER_IMAGEMAGICK_REL_DIR := $(TOOLCHAIN_DIR)/../image-magick/bin
	FLIPPER_IMAGEMAGICK_DIR := $(shell test -d $(FLIPPER_IMAGEMAGICK_REL_DIR) && cd $(FLIPPER_IMAGEMAGICK_REL_DIR) && pwd)
endif
ifneq ($(FLIPPER_PY_DIR),)
	PATH := $(FLIPPER_PY_DIR):$(PATH)
	export PATH
endif
ifneq ($(FLIPPER_PROTO_DIR),)
	PATH := $(FLIPPER_PROTO_DIR):$(PATH)
	export PATH
endif
ifneq ($(FLIPPER_IMAGEMAGICK_DIR),)
	PATH := $(FLIPPER_IMAGEMAGICK_DIR):$(PATH)
	export PATH
endif
