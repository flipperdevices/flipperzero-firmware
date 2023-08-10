NAME    = benchmark
PLATFORM = MOD5441X
CSRCS := ../../../wolfcrypt/benchmark/benchmark.c
CXXSRCS := ./main.cpp
XTRALIB += $(NBROOT)/lib/wolfssl.a

INC = -I"./../../../" -I"./../"
EXTRACFLAGS = $(INC) -DWOLFSSL_USER_SETTINGS -DUSE_CERT_BUFFERS_2048 -DBENCH_EMBEDDED  -DNO_MAIN_DRIVER
EXTRACXXFLAGS = $(EXTRACFLAGS)

include $(NBROOT)/make/main.mak
