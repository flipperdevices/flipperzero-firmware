NAME    = testwolfcyprt
PLATFORM = MOD5441X
CSRCS := ../../../wolfcrypt/test/test.c
CXXSRCS := ./main.cpp
XTRALIB += $(NBROOT)/lib/wolfssl.a

INC = -I"./../../../" -I"./../"
EXTRACFLAGS = $(INC) -DWOLFSSL_USER_SETTINGS -DUSE_CERT_BUFFERS_1024 -DNO_MAIN_DRIVER
EXTRACXXFLAGS = $(EXTRACFLAGS)

include $(NBROOT)/make/main.mak

