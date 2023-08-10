#
#  ======== wolfssl.mak ========
#
include ./products.mak

# Enable older TI-RTOS 2.14-based variables
ifeq ($(BIOS_INSTALL_DIR),)
    BIOS_INSTALL_DIR=$(SYSBIOS_INSTALL_DIR)
endif
ifeq ($(TIVAWARE_INSTALL_DIR),)
    TIVAWARE_INSTALL_DIR=$(TIVAWARE)
endif

WOLFSSL_INSTALL_DIR=$(CURDIR)/../

#
# Set XDCARGS to some of the variables above.  XDCARGS are passed
# to the XDC build engine... which will load wolfssl.bld... which will
# extract these variables and use them to determine what to build and which
# toolchains to use.
#
# Note that not all of these variables need to be set to something valid.
# Unfortunately, since these vars are unconditionally assigned, your build line
# will be longer and more noisy than necessary.
#
# Some background is here:
#     http://rtsc.eclipse.org/docs-tip/Command_-_xdc#Environment_Variables
#
XDCARGS= \
    ti.targets.arm.elf.M4F=\"$(ti.targets.arm.elf.M4F)\" \
    gnu.targets.arm.M4F=\"$(gnu.targets.arm.M4F)\" \
    iar.targets.arm.M4F=\"$(iar.targets.arm.M4F)\" \
    NDK=\"$(NDK_INSTALL_DIR)\"

#
# Set XDCPATH to contain necessary repositories.
#
XDCPATH = $(BIOS_INSTALL_DIR)/packages;$(NDK_INSTALL_DIR)/packages;$(WOLFSSL_INSTALL_DIR);$(TIVAWARE_INSTALL_DIR)
export XDCPATH

#
# Set XDCOPTIONS.  Use -v for a verbose build.
#
#XDCOPTIONS=v
export XDCOPTIONS

#
# Set XDC executable command
# Note that XDCBUILDCFG points to the wolfssl.bld file which uses
# the arguments specified by XDCARGS
#
XDC = $(XDC_INSTALL_DIR)/xdc XDCARGS="$(XDCARGS)" XDCBUILDCFG=./wolfssl.bld

#######################################################
## Shouldn't have to modify anything below this line ##
#######################################################

all:
	@ echo building wolfssl packages ...
	@ $(XDC) -Pr ./packages

clean:
	@ echo cleaning wolfssl packages ...
	@ $(XDC) clean -Pr ./packages
