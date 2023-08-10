==================================================================================================
                Static Library : wolfssl Project Overview
==================================================================================================

Requirements:
This code was created to use Intel's SGX hardware. It is expected that the user has gone through the
steps of both turning on the hardware in bios if needed and has installed the necessary software
from Intel to make use of the hardware. (https://software.intel.com/en-us/sgx) If these steps have
not been done then it is expected that the user is familure with simiulation software being used in
place of hardware.

Overview and Build:
This project creates a static library to then link with Enclaves. A simple example of an Enclave
linking to the created wolfSSL library can be found in wolfssl-examples on github. By default
the Platform Toolset is set to "Intel C++ Compiler 16.0", this may need adjusted depending on
available compilers on the system being built on.
    Testing and development was done with Visual Studio 2013. This project may build with other versions
of Visual Studio but has not been tested with them.

To link with the created library with Visual Studio first open the wolfSSL_SGX project in Visual Studio.
Select platform desired from build configuration ie Win32 or x64. (note the architecture of the library
should match that of the Enclave/application being linked to it) Select Debug or PreSales, if looking
to build in Release mode look at Intels documentation for farther steps on creating a Release build.
Next select Build->Build Solution. This will create a library named wolfssl.lib in the directory
wolfssl-root/IDE/WIN-SGX/<Configuration>/<Platform>. Move this library to the search path of Enclave
linking to. Next create an Enclave using the library.

Limitations:
    Single Threaded (multiple threaded applications have not been tested)
    Crypto Only (network communication from trusted Enclave has not been added yet)
    No ASN/Certificates (handling getting the system time has not yet been added)
    AES-NI use with SGX has not been added in yet

