Developer notes about files in this directory:

NOTE: These files are for use with the wolfCrypt FIPS module and are not
      intended or designed for use in a generic NETOS use-case. Adjustments
      to the user_settings.h will be necessary for use in non-FIPS scenarios.

Makefile.wolfcrypt.inc:
This was developed for a specific customer for inclusion by their recursive
Makefile solution. This file is to be included by the top level Makefile and is
used to preserve the FIPS module order when linked.

user_settings.h:
For use with cert 3389: https://csrc.nist.gov/projects/cryptographic-module-validation-program/certificate/3389

user_settings.h-cert2425:
DEPRECATED (preserved for posterity)
For use with expired cert 2425: https://csrc.nist.gov/projects/cryptographic-module-validation-program/certificate/2425

wolfssl_netos_custom.c:
Functions in this file may be modified without risk of effecting the FIPS
validation, these APIs' are external to the FIPS module boundary.
