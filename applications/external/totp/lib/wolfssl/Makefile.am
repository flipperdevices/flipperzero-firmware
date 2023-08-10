# includes append to these:
SUFFIXES =
TESTS =
CLEANFILES =
DISTCLEANFILES =
MAINTAINERCLEANFILES =
bin_PROGRAMS =
noinst_HEADERS =
lib_LTLIBRARIES =
man_MANS =
noinst_LTLIBRARIES =
noinst_PROGRAMS =
include_HEADERS =
nobase_include_HEADERS =
check_PROGRAMS =
EXTRA_HEADERS =
BUILT_SOURCES=
EXTRA_DIST=
dist_doc_DATA=
dist_noinst_SCRIPTS =
noinst_SCRIPTS =
check_SCRIPTS =
noinst_DATA =
ignore_files =
SUBDIRS_OPT =
DIST_SUBDIRS_OPT =

# allow supplementary or override flags to be passed at make time:
AM_CPPFLAGS += $(EXTRA_CPPFLAGS)
AM_CFLAGS += $(EXTRA_CFLAGS)
AM_CCASFLAGS += $(EXTRA_CCASFLAGS)
AM_LDFLAGS += $(EXTRA_LDFLAGS)

#includes additional rules from aminclude.am
@INC_AMINCLUDE@
DISTCLEANFILES+= aminclude.am \
             .build_params

CLEANFILES+= ecc-key.der \
             ecc-public-key.der \
             cert.der \
             cert.pem \
             certecc.der \
             certecc.pem \
             certreq.der \
             certreq.pem \
             key.der \
             key.pem \
             ecc-key-pkcs8.der \
             othercert.der \
             othercert.pem \
             pkcs7cert.der \
             pkcs7authEnvelopedDataAES128GCM.der \
             pkcs7authEnvelopedDataAES128GCM_ECDH_SHA1KDF.der \
             pkcs7authEnvelopedDataAES128GCM_KEKRI.der \
             pkcs7authEnvelopedDataAES128GCM_ORI.der \
             pkcs7authEnvelopedDataAES128GCM_PWRI.der \
             pkcs7authEnvelopedDataAES192GCM.der \
             pkcs7authEnvelopedDataAES256GCM.der \
             pkcs7authEnvelopedDataAES256GCM_ECDH_SHA256KDF.der \
             pkcs7authEnvelopedDataAES256GCM_ECDH_SHA256KDF_authAttribs.der \
             pkcs7authEnvelopedDataAES256GCM_ECDH_SHA256KDF_bothAttribs.der \
             pkcs7authEnvelopedDataAES256GCM_ECDH_SHA256KDF_fw_bothAttribs.der \
             pkcs7authEnvelopedDataAES256GCM_ECDH_SHA256KDF_unauthAttribs.der \
             pkcs7authEnvelopedDataAES256GCM_ECDH_SHA512KDF.der \
             pkcs7authEnvelopedDataAES256GCM_ECDH_SHA512KDF_ukm.der \
             pkcs7authEnvelopedDataAES256GCM_firmwarePkgData.der \
             pkcs7authEnvelopedDataAES256GCM_IANDS.der \
             pkcs7authEnvelopedDataAES256GCM_SKID.der \
             pkcs7compressedData_data_zlib.der \
             pkcs7compressedData_firmwarePkgData_zlib.der \
             pkcs7encryptedDataAES128CBC.der \
             pkcs7encryptedDataAES192CBC.der \
             pkcs7encryptedDataAES256CBC_attribs.der \
             pkcs7encryptedDataAES256CBC.der \
             pkcs7encryptedDataAES256CBC_firmwarePkgData.der \
             pkcs7encryptedDataAES256CBC_multi_attribs.der \
             pkcs7encryptedDataDES3.der \
             pkcs7encryptedDataDES.der \
             pkcs7envelopedDataAES256CBC_ECDH.der \
             pkcs7envelopedDataAES128CBC_ECDH_SHA1KDF.der \
             pkcs7envelopedDataAES256CBC_ECDH_SHA256KDF.der \
             pkcs7envelopedDataAES256CBC_ECDH_SHA512KDF.der \
             pkcs7envelopedDataAES256CBC_ECDH_SHA512KDF_ukm.der \
             pkcs7envelopedDataDES3.der \
             pkcs7envelopedDataAES128CBC.der \
             pkcs7envelopedDataAES128CBC_KEKRI.der \
             pkcs7envelopedDataAES128CBC_PWRI.der \
             pkcs7envelopedDataAES128CBC_ORI.der \
             pkcs7envelopedDataAES192CBC.der \
             pkcs7envelopedDataAES256CBC.der \
             pkcs7envelopedDataAES256CBC_IANDS.der \
             pkcs7envelopedDataAES256CBC_SKID.der \
             pkcs7signedData_RSA_SHA.der \
             pkcs7signedData_RSA_SHA_noattr.der \
             pkcs7signedData_RSA_SHA224.der \
             pkcs7signedData_RSA_SHA256.der \
             pkcs7signedData_RSA_SHA256_firmwarePkgData.der \
             pkcs7signedData_RSA_SHA256_custom_contentType.der \
             pkcs7signedData_RSA_SHA256_with_ca_cert.der \
             pkcs7signedData_RSA_SHA256_SKID.der \
             pkcs7signedData_RSA_SHA256_detachedSig.der \
             pkcs7signedData_RSA_SHA384.der \
             pkcs7signedData_RSA_SHA512.der \
             pkcs7signedData_ECDSA_SHA.der \
             pkcs7signedData_ECDSA_SHA_noattr.der \
             pkcs7signedData_ECDSA_SHA224.der \
             pkcs7signedData_ECDSA_SHA256.der \
             pkcs7signedData_ECDSA_SHA256_firmwarePkgData.der \
             pkcs7signedData_ECDSA_SHA256_custom_contentType.der \
             pkcs7signedData_ECDSA_SHA256_SKID.der \
             pkcs7signedData_ECDSA_SHA384.der \
             pkcs7signedData_ECDSA_SHA512.der \
             pkcs7signedFirmwarePkgData_ECDSA_SHA256.der \
             pkcs7signedFirmwarePkgData_ECDSA_SHA256_SKID.der \
             pkcs7signedFirmwarePkgData_ECDSA_SHA256_noattr.der \
             pkcs7signedFirmwarePkgData_RSA_SHA256.der \
             pkcs7signedFirmwarePkgData_RSA_SHA256_SKID.der \
             pkcs7signedFirmwarePkgData_RSA_SHA256_noattr.der \
             pkcs7signedFirmwarePkgData_RSA_SHA256_with_ca_cert.der \
             pkcs7signedCompressedFirmwarePkgData_ECDSA_SHA256.der \
             pkcs7signedCompressedFirmwarePkgData_ECDSA_SHA256_noattr.der \
             pkcs7signedCompressedFirmwarePkgData_RSA_SHA256.der \
             pkcs7signedCompressedFirmwarePkgData_RSA_SHA256_noattr.der \
             pkcs7signedEncryptedFirmwarePkgData_RSA_SHA256.der \
             pkcs7signedEncryptedFirmwarePkgData_RSA_SHA256_noattr.der \
             pkcs7signedEncryptedFirmwarePkgData_ECDSA_SHA256.der \
             pkcs7signedEncryptedFirmwarePkgData_ECDSA_SHA256_noattr.der \
             pkcs7signedEncryptedCompressedFirmwarePkgData_ECDSA_SHA256.der \
             pkcs7signedEncryptedCompressedFirmwarePkgData_ECDSA_SHA256_noattr.der \
             pkcs7signedEncryptedCompressedFirmwarePkgData_RSA_SHA256.der \
             pkcs7signedEncryptedCompressedFirmwarePkgData_RSA_SHA256_noattr.der \
             tests/test-log-dump-to-file.txt \
             MyKeyLog.txt

exampledir = $(docdir)/example
dist_example_DATA=

ACLOCAL_AMFLAGS= -I m4

EXTRA_DIST+= lib/dummy

EXTRA_DIST+= wolfssl.vcproj
EXTRA_DIST+= wolfssl.vcxproj
EXTRA_DIST+= wolfssl64.sln
EXTRA_DIST+= valgrind-error.sh
EXTRA_DIST+= valgrind-bash.supp
EXTRA_DIST+= fips-hash.sh
EXTRA_DIST+= gencertbuf.pl
EXTRA_DIST+= README.md
EXTRA_DIST+= README
EXTRA_DIST+= ChangeLog.md
EXTRA_DIST+= LICENSING
EXTRA_DIST+= INSTALL
EXTRA_DIST+= IPP
EXTRA_DIST+= LPCExpresso.cproject
EXTRA_DIST+= LPCExpresso.project
EXTRA_DIST+= resource.h wolfssl.rc
EXTRA_DIST+= CMakeLists.txt
EXTRA_DIST+= m4/ax_atomic.m4

include cmake/include.am
include wrapper/include.am
include wolfssl/include.am
include certs/include.am
include doc/include.am
include Docker/include.am

include src/include.am
include support/include.am
include wolfcrypt/user-crypto/include.am
include wolfcrypt/benchmark/include.am
include wolfcrypt/src/include.am
include wolfcrypt/test/include.am
include examples/include.am
include testsuite/include.am
include tests/include.am
include sslSniffer/sslSnifferTest/include.am
include rpm/include.am
include linuxkm/include.am
include zephyr/include.am
include RTOS/nuttx/include.am

# Exclude references to non-DFSG sources from build files
if !BUILD_DISTRO
include mqx/util_lib/Sources/include.am
include mqx/wolfcrypt_benchmark/Sources/include.am
include mqx/wolfcrypt_test/Sources/include.am
include mqx/wolfssl/include.am
include mqx/wolfssl_client/Sources/include.am
include mplabx/include.am
include mplabx/wolfcrypt_benchmark.X/nbproject/include.am
include mplabx/wolfcrypt_test.X/nbproject/include.am
include mplabx/wolfssl.X/nbproject/include.am
include mcapi/include.am
include mcapi/wolfcrypt_mcapi.X/nbproject/include.am
include mcapi/wolfcrypt_test.X/nbproject/include.am
include mcapi/wolfssl.X/nbproject/include.am
include mcapi/zlib.X/nbproject/include.am
include tirtos/include.am
include IDE/include.am
endif
include scripts/include.am

if BUILD_LINUXKM
    # rather than setting $SUBDIRS here directly, we set an auxiliary variable.
    # autotools sees the SUBDIRS assignment here even if BUILD_LINUXKM is false,
    # at least for purposes of recursing for "make distdir", which we don't want to happen.
    SUBDIRS_OPT += linuxkm
    DIST_SUBDIRS_OPT += linuxkm

    export KERNEL_ROOT KERNEL_ARCH KERNEL_EXTRA_CFLAGS \
        EXTRA_CFLAGS EXTRA_CPPFLAGS EXTRA_CCASFLAGS EXTRA_LDFLAGS \
	AM_CPPFLAGS CPPFLAGS AM_CFLAGS CFLAGS \
        AM_CCASFLAGS CCASFLAGS \
        src_libwolfssl_la_OBJECTS ENABLED_CRYPT_TESTS ENABLED_LINUXKM_PIE ENABLED_ASM \
        CFLAGS_FPU_DISABLE CFLAGS_FPU_ENABLE CFLAGS_SIMD_DISABLE CFLAGS_SIMD_ENABLE \
        CFLAGS_AUTO_VECTORIZE_DISABLE CFLAGS_AUTO_VECTORIZE_ENABLE \
        ASFLAGS_FPU_DISABLE_SIMD_ENABLE ASFLAGS_FPU_ENABLE_SIMD_DISABLE \
        ASFLAGS_FPUSIMD_DISABLE ASFLAGS_FPUSIMD_ENABLE ENABLED_LINUXKM_BENCHMARKS

module:
	+$(MAKE) -C linuxkm libwolfssl.ko

clean_module:
	+$(MAKE) -C linuxkm clean

install_module modules_install:
	+$(MAKE) -C linuxkm modules_install

endif

if USE_VALGRIND
TESTS_ENVIRONMENT=./valgrind-error.sh
endif

TEST_EXTENSIONS=.test
TESTS += $(check_PROGRAMS)

check_SCRIPTS+= $(dist_noinst_SCRIPTS)
TESTS += $(check_SCRIPTS)

test: check
tests/unit.log: testsuite/testsuite.log
scripts/unit.log: testsuite/testsuite.log

DISTCLEANFILES+= wolfssl-config

MAINTAINERCLEANFILES+= wolfssl/wolfcrypt/async.h
MAINTAINERCLEANFILES+= wolfssl/wolfcrypt/fips.h
MAINTAINERCLEANFILES+= wolfssl/wolfcrypt/port/cavium/cavium_nitrox.h
MAINTAINERCLEANFILES+= wolfssl/wolfcrypt/port/intel/quickassist.h
MAINTAINERCLEANFILES+= wolfssl/wolfcrypt/port/intel/quickassist_mem.h

SUBDIRS=$(SUBDIRS_OPT)
DIST_SUBDIRS=$(DIST_SUBDIRS_OPT)

maintainer-clean-local:
	-rm Makefile.in
	-rm aclocal.m4
	-rm build-aux/compile
	-rm build-aux/config.guess
	-rm build-aux/config.sub
	-rm build-aux/depcomp
	-rm build-aux/install-sh
	-rm build-aux/ltmain.sh
	-rm build-aux/missing
	-rmdir build-aux
	-rm configure
	-rm config.log
	-rm config.status
	-rm config.in
	-rm m4/libtool.m4
	-rm m4/ltoptions.m4
	-rm m4/ltsugar.m4
	-rm m4/ltversion.m4
	-rm m4/lt~obsolete.m4
	-rm support/wolfssl.pc
	find . -type f -name '*~' -exec rm -f '{}' \;
	-rm -f @PACKAGE@-*.tar.gz
	-rm -f @PACKAGE@-*.rpm

# !!!! first line of rule has to start with a hard (real) tab, not spaces
egs:
	$(MAKE) examples/client/client; \
	$(MAKE) examples/echoclient/echoclient;\
	$(MAKE) examples/server/server; \
	$(MAKE) examples/echoserver/echoserver;

wc_egs:
	$(MAKE) wolfcrypt/test/testwolfcrypt; \
	$(MAKE) wolfcrypt/benchmark/benchmark;

install-exec-local:	install-generic-config

install-generic-config:
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_SCRIPT) @GENERIC_CONFIG@ $(DESTDIR)$(bindir)

uninstall-local:
	-rm -f $(DESTDIR)$(bindir)/@GENERIC_CONFIG@

merge-clean:
	@find ./ | $(GREP) \.gcda | xargs rm -f
	@find ./ | $(GREP) \.gcno | xargs rm -f
	@find ./ | $(GREP) \.gz | xargs rm -f
	@find ./ | $(GREP) \.orig | xargs rm -f
	@find ./ | $(GREP) \.rej | xargs rm -f
	@find ./ | $(GREP) \.rpm | xargs rm -f
	@find ./ | $(GREP) \.THIS | xargs rm -f
	@find ./ | $(GREP) \.OTHER | xargs rm -f
	@find ./ | $(GREP) \.BASE | xargs rm -f
	@find ./ | $(GREP) \~$$ | xargs rm -f

