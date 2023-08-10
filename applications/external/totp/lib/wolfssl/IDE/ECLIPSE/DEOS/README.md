# Deos Port

## Overview

You can enable the wolfSSL support for Deos RTOS available
[here](https://www.ddci.com/products_deos_do_178c_arinc_653/) using
the `#define WOLFSSL_DEOS`.  Deos is a time & space partitioned,
multi-core enabled, DO-178C DAL A certifiable RTOS.

## Usage

You can start with your OpenArbor IDE-based example project for Deos
with the network stack (lwip) to integrate wolfSSL source code.

wolfSSL supports a compile-time user configurable options in the
`IDE/ECLIPSE/DEOS/user_settings.h` file.

### Importing the project

In this section you will import a pre-configured example project.

Note: To work wolfssl directory must not be under the workspace directory.

1. Launch the OpenArbor IDE as an administrator
2. In the Workspace Launcher dialog, in the Workspace field, enter
   your workspace
3. Right-click in the Project Explorer view and select Import
4. In the Import dialog, select General > Existing Projects into
   Workspace, then click Next.
5. In the Import Projects dialog, select `Select root directory` and
   browse to `IDE/ECLIPSE/DEOS/deos_wolfssl` and select the wolfssl
   project
6. In the Import Projects dialog, click Finish

#### Dependencies

* ansi
* gnu-language
* deos-time

### Setting up a Deos project with wolfSSL

The `tls_wolfssl.c` example application provides a simple function to
run the selected examples at compile time through the following four
`#defines` in `user_settings.h`. You can undefine any of these macro
options to run a test.

```
#undef NO_CRYPT_TEST
#undef NO_CRYPT_BENCHMARK
#undef NO_WOLFSSL_CLIENT
#undef NO_WOLFSSL_SERVER
```

1. Launch the OpenArbor IDE

2. Create a DDC-I Deos example project. In the main menu, go to File >
   DDC-I Deos example project > socket > udp-vs-tcp

3. Customize your config/udp-vs-tcp.pd.xml with the following changes:

```
<processTemplate
     mutexQuota = "5"
   >

   <logicalMemoryPools>
           pagesNeeded = "500"
      ></pool>
   </logicalMemoryPools>

   <threadTemplate
      stackSizeInPages = "20"
    ></threadTemplate>

   <mutexTemplates>
      <mutexTemplate
           name = "protectWolfSSLTemp"
           lockTimeInUsec = "40"
           priority = "fastest"
      ></mutexTemplate>
   </mutexTemplates>

</processTemplate>
```

Depending on your configuration, wolfSSL uses upto four mutexes. You
also need to configure enough memory for the stack of each threads and
the process logical memory pool.

4. Right click on the `udp-vs-tcp` project, select properties and add
   the following macros in the DDC-I Options > C Compile >
   Preprocessor

  * WOLFSSL_USER_SETTINGS

5.  Add the following directory paths in the DDC-I Options > C Compile >
    Directories and in the DDC-I Options > C++ Compile > Directories

  * $(PROJECT_DIR.wolfssl)/../../../..
  * $(PROJECT_DIR.wolfssl)/..
  * $(PROJECT_DIR.printx)/code

6.  Add the following library dependencies in the
    DDC-I Options > Deos > Dependencies

  * math
  * ansi
  * deos-time

  For benchmark and test code:

    * printx - You must add printx into your workspace, File >DDC-I
      Deos example project > training > printx

7.  Edit $(PROJECT_DIR)/wolfsslPort/IDE/ECLIPSE/DEOS/user_setting.h to
    customize your configuration. For example, you can undef or define
    these tests.

 * `#undef NO_CRYPT_TEST`
 * `#undef NO_CRYPT_BENCHMARK`
 * `#undef NO_WOLFSSL_CLIENT`
 * `#undef NO_WOLFSSL_SERVER`

8.  Edit your application source file where main() thread is defined
and add the following:

  * #include "printx.h"
  * #include "tls_wolfssl.h"
  * and a call to `wolfsslRunTests()`

Here's an example:

```
#include <deos.h>
#include <printx.h>
#include <tls_wolfssl.h>
#include <user_settings.h>

int main(void)
{
  initPrintx("");
  printf("TLS wolfssl example!\n");

  (void) waitUntilNextPeriod();
   wolfsslRunTests();

  deleteThread(currentThreadHandle());
}

```

9.  Review `$(PROJECT_DIR)/udp-vs-tcp/mailbox-transport.config`
    configuration.

```
transportConfigurationId
2                              # Client thread quota - for client and server TCP
2                              # Client connection quota - one for client and one for server
0                              # Server startup quota
0                              # Server connection quota
transportMemoryObject          # Name of memory object used for managing connections
/

connectionId1                  # TCP client connection
Network                        # Server process name
defaultMailbox                 # Server connection request mailbox name
0                              # Server connection mailbox queue size (unused by Network process)
userServiceThread              # Server thread template name
*                              # Error timeout
1                              # Client connection mailbox queue size
/

connectionId2                  # TCP connection
Network                        # Server process name
defaultMailbox                 # Server connection request mailbox name
0                              # Server connection mailbox queue size (unused by Network process)
userServiceThread              # Server thread template name
*                              # Error timeout
1                              # Client connection mailbox queue size
/
```

#### Building and Running

1.  Build your project, then load and run your image on a target
    platform. Review the test results on the console output.


### `wolfcrypt_test()`

`wolfcrypt_test()` prints a message on similar to the following:

```
error    test passed!
base64   test passed!
asn      test passed!
...
```

This example doesn't show the whole output.

### `benchmark_test()`

`benchmark_test()` prints a message on the similar to the following:

```
------------------------------------------------------------------------------
wolfSSL version 4.6.0
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                  2 MB took 1.000 seconds,    2.124 MB/s
AES-128-CBC-enc      5 MB took 1.000 seconds,    5.127 MB/s
AES-128-CBC-dec      5 MB took 1.000 seconds,    4.907 MB/s
AES-192-CBC-enc      5 MB took 1.000 seconds,    4.736 MB/s
AES-192-CBC-dec      5 MB took 1.000 seconds,    4.761 MB/s
...
```

This example doesn't show the whole output.

### `wolfssl_client_test()`

You can modify the `TCP_SERVER_IP_ADDR` and `TCP_SERVER_PORT` macros
in the `tls_wolfssl.c` file to configure the host address and
port. You will also need to define the server certificate. The example
client uses the GET request to get a web resource from the server at
https://google.com.

### `wolfssl_server_test()`

You can modify the `TLS_SERVER_PORT` in the `tls_wolfssl.c` file to
configure the port number to listen on a local-host.  Once you start
the TLS server and `Listening for client connection` displays on the
serial console, the server is ready to accept client connections.

You can connect to the server using the wolfssl TLS client example
from your Linux or Windows host as follows:

```
$ ./examples/client/client.exe -h TLS_SERVER_IP_ADDRESS

The client outputs messages similar to the following:

SSL version is TLSv1.2
SSL cipher suite is TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
SSL curve name is SECP256R1
I hear ya fa shizzle!
```

## References

The following software and tool chains were used for testing:

- OpenArbor 7.0.0
- wolfssl [latest version](https://github.com/wolfSSL/wolfssl)

For more information or questions, please email
[support@wolfssl.com](mailto:support@wolfssl.com)
