AM_CFLAGS=-I m4

#add in wolfssl directory
AM_CPPFLAGS+=-I$(abs_srcdir)/../../ -I$(srcdir)/include/
lib_LTLIBRARIES = lib/libusercrypto.la
lib_libusercrypto_la_CPPFLAGS = $(AM_CPPFLAGS)
lib_libusercrypto_la_LDFLAGS = $(AM_LDFLAGS)
lib_libusercrypto_la_SOURCES = src/rsa.c
include_HEADERS = include/user_rsa.h
