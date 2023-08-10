# libwolfssl Linux kernel module Makefile (wraps Kbuild-native makefile)
#
# Copyright (C) 2006-2023 wolfSSL Inc.
#
# This file is part of wolfSSL.
#
# wolfSSL is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# wolfSSL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA

SHELL=/bin/bash

all: libwolfssl.ko libwolfssl.ko.signed

.PHONY: libwolfssl.ko

ifndef MODULE_TOP
    MODULE_TOP=$(CURDIR)
endif

ifndef SRC_TOP
    SRC_TOP=$(shell dirname $(MODULE_TOP))
endif

WOLFSSL_CFLAGS=-DHAVE_CONFIG_H -I$(SRC_TOP) -DBUILDING_WOLFSSL $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -Wno-declaration-after-statement -Wno-redundant-decls -DLIBWOLFSSL_GLOBAL_EXTRA_CFLAGS="\" $(KERNEL_EXTRA_CFLAGS)\""
ifdef KERNEL_EXTRA_CFLAGS
    WOLFSSL_CFLAGS += $(KERNEL_EXTRA_CFLAGS)
endif

WOLFSSL_ASFLAGS=-DHAVE_CONFIG_H -I$(SRC_TOP) -DBUILDING_WOLFSSL $(AM_CCASFLAGS) $(CCASFLAGS)

WOLFSSL_OBJ_FILES=$(patsubst %.lo, %.o, $(patsubst src/src_libwolfssl_la-%, src/%, $(patsubst src/libwolfssl_la-%, src/%, $(patsubst wolfcrypt/src/src_libwolfssl_la-%, wolfcrypt/src/%, $(src_libwolfssl_la_OBJECTS)))))

ifeq "$(ENABLED_CRYPT_TESTS)" "yes"
    WOLFSSL_OBJ_FILES+=wolfcrypt/test/test.o
else
    WOLFSSL_CFLAGS+=-DNO_CRYPT_TEST
endif

ifeq "$(ENABLED_LINUXKM_PIE)" "yes"
    WOLFCRYPT_PIE_FILES := linuxkm/pie_first.o $(filter wolfcrypt/src/%,$(WOLFSSL_OBJ_FILES)) linuxkm/pie_redirect_table.o linuxkm/pie_last.o
    WOLFSSL_OBJ_FILES := $(WOLFCRYPT_PIE_FILES) $(filter-out $(WOLFCRYPT_PIE_FILES),$(WOLFSSL_OBJ_FILES))
endif

export WOLFSSL_CFLAGS WOLFSSL_ASFLAGS WOLFSSL_OBJ_FILES WOLFCRYPT_PIE_FILES

libwolfssl.ko:
	@if test -z "$(KERNEL_ROOT)"; then echo '$$KERNEL_ROOT is unset' >&2; exit 1; fi
	@if test -z "$(AM_CFLAGS)$(CFLAGS)"; then echo '$$AM_CFLAGS and $$CFLAGS are both unset.' >&2; exit 1; fi
	@if test -z "$(src_libwolfssl_la_OBJECTS)"; then echo '$$src_libwolfssl_la_OBJECTS is unset.' >&2; exit 1; fi
	@mkdir -p linuxkm src wolfcrypt/src wolfcrypt/test
	@if test ! -h $(SRC_TOP)/Kbuild; then ln -s $(MODULE_TOP)/Kbuild $(SRC_TOP)/Kbuild; fi
ifeq "$(ENABLED_LINUXKM_PIE)" "yes"
	+$(MAKE) -C $(KERNEL_ROOT) M=$(MODULE_TOP) src=$(SRC_TOP) $(KBUILD_EXTRA_FLAGS) CC_FLAGS_FTRACE=
else
	+$(MAKE) -C $(KERNEL_ROOT) M=$(MODULE_TOP) src=$(SRC_TOP) $(KBUILD_EXTRA_FLAGS)
endif

libwolfssl.ko.signed: libwolfssl.ko
	@cd '$(KERNEL_ROOT)' || exit $$?;							\
	while read configline; do								\
		case "$$configline" in								\
		CONFIG_MODULE_SIG*=*)								\
			declare "$${configline%=*}"="$${configline#*=}"				\
			;;									\
		esac;										\
	done < .config || exit $$?;								\
	if [[ "$${CONFIG_MODULE_SIG}" = "y" && -n "$${CONFIG_MODULE_SIG_KEY}" && 		\
		-n "$${CONFIG_MODULE_SIG_HASH}" && ( ! -f '$(MODULE_TOP)/$@' ||			\
			'$(MODULE_TOP)/$<' -nt '$(MODULE_TOP)/$@' ) ]]; then			\
		CONFIG_MODULE_SIG_KEY="$${CONFIG_MODULE_SIG_KEY#\"}";				\
		CONFIG_MODULE_SIG_KEY="$${CONFIG_MODULE_SIG_KEY%\"}";				\
		CONFIG_MODULE_SIG_HASH="$${CONFIG_MODULE_SIG_HASH#\"}";				\
		CONFIG_MODULE_SIG_HASH="$${CONFIG_MODULE_SIG_HASH%\"}";				\
		cp -p '$(MODULE_TOP)/$<' '$(MODULE_TOP)/$@' || exit $$?;			\
		./scripts/sign-file "$${CONFIG_MODULE_SIG_HASH}"				\
				    "$${CONFIG_MODULE_SIG_KEY}"					\
				    "$${CONFIG_MODULE_SIG_KEY/%.pem/.x509}"			\
				    '$(MODULE_TOP)/$@';						\
		sign_file_exitval=$$?;								\
		if [[ $$sign_file_exitval != 0 ]]; then						\
			$(RM) -f '$(MODULE_TOP)/$@';						\
			exit $$sign_file_exitval;						\
		fi;										\
		if [[ "$(quiet)" != "silent_" ]]; then						\
			echo "  Module $@ signed by $${CONFIG_MODULE_SIG_KEY}.";		\
		fi										\
	fi


.PHONY: install modules_install
install modules_install:
	+$(MAKE) -C $(KERNEL_ROOT) M=$(MODULE_TOP) src=$(SRC_TOP) INSTALL_MOD_DIR=wolfssl modules_install

.PHONY: clean
# note, must supply $(MODULE_TOP) as the src value for clean so that Kbuild is included, else
# the top Makefile (which is not for the kernel build) would be included here.
clean:
	+$(MAKE) -C $(KERNEL_ROOT) M=$(MODULE_TOP) src=$(MODULE_TOP) clean

.PHONY: check
check:

.PHONY: distclean
distclean: clean

.PHONY: dist
dist:

.PHONY: distdir
distdir:
