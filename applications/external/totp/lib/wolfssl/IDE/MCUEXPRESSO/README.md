
- Open MCUEXPRESSO and set the workspace to wolfssl/IDE/MCUEXPRESSO or to wolfssl/IDE/MCUEXPRESSO/RT1170 (for RT1170)
- File -> Open Projects From File System... -> Directory : and set the browse to wolfssl/IDE/MCUEXPRESSO directory then click "select directory"
	- For RT1170 use the directory wolfssl/IDE/MCUEXPRESSO/RT1170
- Select MCUEXPRESSO\wolfssl, MCUEXPRESSO\benchmark and MCUEXPRESSO\wolfcrypt_test then click "Finish"
	- For RT1170 select wolfssl_cm7, wolfcrypt_test_cm7
- Right click the projects -> SDK Management -> Refresh SDK Components and click "yes"
- MCUEXPRESSO fails to generate the files for wolfssl/MIMXRT685S with RT685 build, just copy the files from either benchmark or wolfcrypt_test into the directory
- increase the size of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h to be 200000 for wolfcrypt_test and benchmark projects
- (note board files need to be recreated .... this can be done by creating a new project that has the same settings and copying over the generated board/* files)
- Build the projects


### Expanding RT1170 CAAM Driver
The files RT1170/fsl_caam_h.patch and RT1170/fsl_caam_c.patch include changes to
the existing NXP CAAM driver for use with creating/opening Blobs and generating
and using ECC black keys.

To apply the patches first create a project that has the caam driver. This will
generate the base fsl_caam.c and fsl_caam.h in the drivers directory. (i.e wolfcrypt_test_cm7/drivers/fls_caam.{c,h})
. Once the base files are generated then 'cd' to the drivers directory and apply
the patch. The following is an example of applying the patch to the PKCS7 example
using Cygwin (cd wolfcrypt_test_cm7/drivers/ && /bin/patch < ../../fsl_caam_c.patch && /bin/patch < ../../fsl_caam_h.patch).

In the patch for fsl_caam.h there are macros defined for both the ECC and Blob
expansion (CAAM_ECC_EXPANSION and CAAM_BLOB_EXPANSION). When wolfSSL code finds
that these macros are defined (the patch has been applied) then it tries to
compile in use of the expanded driver.

### CMake example

See the cmake directory in https://github.com/wolfssl/wolfssl-examples for an example of
building with CMake and CSR/PKCS7 examples.
