/*
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


/*
 Created to use intel's IPP see their license for linking to intel's IPP library
 */


##BUILDING ON 64BIT MAC OSX
Tested and developed on MAC OSX linking to IPP v9.0

for me exporting the IPP library was needed. As an example it was
export DYLD_LIBRARY_PATH="/opt/intel/ipp/lib"

first go to the root wolfssl dir and run ./autogen.sh && ./configure it with desired settings then make. This is to set up the define options and wolfssl library for the user crypto to link to.

Then go to the wolfssl/user-crypto directory and run ./autogen.sh && ./configure then make make install this creates a usercrypto library to use

Finally go back to the root wolfssl directory and follow these build instructions

building wolfSSL add CPPFLAGS=-I/opt/intel/ipp/include for finding the IPP include files
An example build would be
./configure --with-user-crypto CPPFLAGS=-I/opt/intel/ipp/include --enable-lighty


##BUILDING IN 32BIT UBUNTU
Tested on UBUNTU 32 bit linking to IPP v9.0

for me exporting the IPP library. As an example it was
export LD_LIBRARY_PATH="/opt/intel/ipp/lib/ia32_lin/:$LD_LIBRARY_PATH"

first go to the root wolfssl dir and configure it with desired settings and make install. This is to set up the define options and wolfssl library for the user crypto to link to.

For me on Ubuntu the IPP libraries had been installed into /opt/intel/ipp/lib/ia32_lin/ so the ./configure LDFLAGS=-L/opt/intel/ipp/lib/ia32_lin was needed to be looking at that directory.
Run make && make install from the directory wolfssl_root/wolfssl/user-crypto/ this creates a usercrypto library to use

Finally go back to the root wolfssl directory and follow these build instructions

building wolfSSL add CPPFLAGS=-I/opt/intel/ipp/include for finding the IPP include files

./configure --with-user-crypto=root_wolfssl/wolfssl/user-crypto CPPFLAGS=-I/opt/intel/ipp/include (plus any desired additional flags)


##THINGS TO CHECK FOR IF NOT ABLE TO LINK WITH USERCRYPTO LIB
Check that the path has been exported for the IPP library. If usercrypto is unable to use the  function to init an RSA key then the link to it will fail in configure. Check for this by $DYLD_LIBRARY_PATH on mac or $LD_LIBRARY_PATH on ubuntu. If the directory for the Intel IPP libraries are not displayed than use "export DYLD_LIBRARY_PATH=path_to_ipp_libraries:$DYLD_LIBRARY_PATH".


##CREATING OWN RSA CRYPTO PLUGIN

It is required to have a header file named user_rsa.h. This is what is looked for by wolfssl/wolfcrypt/rsa.h and should contain the user defined rsa key struct.

It is required to have a library called usercrypto. This is linked to when configuring wolfSSL with the option --with-user-crypto

It is required when compiled with RSA cert generation to have key struct elements named n and e containing the corresponding big numbers. And the three helper functions to work with the big numbers. These functions are called by wolfcrypt/src/asn.c when working with certificates.
To view the needed functions look at wolfssl/wolfcrypt/rsa.h they will be extern functions surrounded by HAVE_USER_RSA define.
Cert Generation for other sign and verify such as ECC are not yet supported.

When building with openssl compatibility layer extra developent needs to be done, having the two functions SetRsaExernal and SetRsaInternal

wolfSSL does not take responsibility for the strength of security of third party cryptography libraries plugged in by the user.
