TOOLCHAIN_GDB_SYM := $(shell which arm-none-eabi-gdb)
ifneq ($(TOOLCHAIN_GDB_SYM),)
	TOOLCHAIN_GDB := $(shell readlink -f -- $(TOOLCHAIN_GDB_SYM))
	TOOLCHAIN_DIR := $(shell dirname -- $(TOOLCHAIN_GDB))
	FLIPPER_PY_REL_DIR := $(TOOLCHAIN_DIR)/../python/bin
	FLIPPER_PY_DIR := $(shell test -d $(FLIPPER_PY_REL_DIR) && cd $(FLIPPER_PY_REL_DIR) && pwd)
endif
ifneq ($(FLIPPER_PY_DIR),)
	PATH := $(FLIPPER_PY_DIR):$(PATH)
	export PATH
endif
