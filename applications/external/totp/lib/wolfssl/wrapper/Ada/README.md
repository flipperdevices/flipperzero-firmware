# Ada Binding Example
The source code for the Ada/SPARK binding of the WolfSSL library
is the WolfSSL Ada package in the wolfssl.ads and wolfssl.adb files.

The source code here also demonstrates a TLS v1.3 server and client
using the WolfSSL Ada binding. The implementation is cross-platform
and compiles on Linux, Mac OS X and Windows.

Security: The WolfSSL Ada binding avoids usage of the
Secondary Stack. The GNAT compiler has a number of hardening
features for example Stack Scrubbing; the compiler can generate
code to zero-out stack frames used by subprograms.
Unfortunately this works well for the primary stack but not
for the secondary stack. The GNAT User's Guide recommends
avoiding the secondary stack using the restriction
No_Secondary_Stack (see the GNAT configuration file gnat.adc
which instructs compilation of the WolfSSL Ada binding under
this restriction).

Portability: The WolfSSL Ada binding makes no usage of controlled types
and has no dependency upon the Ada.Finalization package.
Lighter Ada run-times for embedded systems often have
the restriction No_Finalization. The WolfSSL Ada binding has
been developed with maximum portability in mind.

Not only can the WolfSSL Ada binding be used in Ada applications but
also SPARK applications (a subset of the Ada language suitable
formal verification). To formally verify the Ada code in this repository
open the client.gpr with GNAT Studio and then select
SPARK -> Prove All Sources and use Proof Level 2.

```
Summary of SPARK analysis
=========================

---------------------------------------------------------------------------------------------------------------
SPARK Analysis results        Total        Flow   CodePeer                       Provers   Justified   Unproved
---------------------------------------------------------------------------------------------------------------
Data Dependencies                 2           2          .                             .           .          .
Flow Dependencies                 .           .          .                             .           .          .
Initialization                   15          15          .                             .           .          .
Non-Aliasing                      .           .          .                             .           .          .
Run-time Checks                  58           .          .    58 (CVC4 85%, Trivial 15%)           .          .
Assertions                        6           .          .                      6 (CVC4)           .          .
Functional Contracts             91           .          .                     91 (CVC4)           .          .
LSP Verification                  .           .          .                             .           .          .
Termination                       .           .          .                             .           .          .
Concurrency                       .           .          .                             .           .          .
---------------------------------------------------------------------------------------------------------------
Total                           172    17 (10%)          .                     155 (90%)           .          .
```

## Compiler and Build System installation

### GNAT Community Edition 2021
Download and install the GNAT community Edition 2021 compiler and studio:
https://www.adacore.com/download

Linux Install:

```sh
chmod +x gnat-2021-20210519-x86_64-linux-bin
./gnat-2021-20210519-x86_64-linux-bin
```

```sh
export PATH="/opt/GNAT/2021/bin:$PATH"
cd wrapper/Ada
gprclean
gprbuild default.gpr
gprbuild client.gpr

cd obj/
./tls_server_main &
./tls_client_main 127.0.0.1
```

On Windows, build the executables with:
```sh
gprbuild -XOS=Windows default.gpr
gprbuild -XOS=Windows client.gpr
```


### GNAT FSF Compiler and GPRBuild manual installation
In May 2022 AdaCore announced the end of the GNAT Community releases.
Pre-built binaries for the GNAT FSF compiler and GPRBuild can be
downloaded and manually installed from here:
https://github.com/alire-project/GNAT-FSF-builds/releases
Make sure the executables for the compiler and GPRBuild are on the PATH
and use gprbuild to build the source code.

## Files
The TLS v1.3 client example in the Ada/SPARK programming language
using the WolfSSL library can be found in the files:
tls_client_main.adb
tls_client.ads
tls_client.adb

The TLS v1.3 server example in the Ada/SPARK programming language
using the WolfSSL library can be found in the files:
tls_server_main.adb
tls_server.ads
tls_server.adb

A feature of the Ada language that is not part of SPARK is exceptions.
Some packages of the Ada standard library and GNAT specific packages
provided by the GNAT compiler can therefore not be used directly but
need to be put into wrapper packages that does not raise exceptions.
The packages that provide access to sockets and command line arguments
to applications implemented in the SPARK programming language can be
found in the files:
spark_sockets.ads
spark_sockets.adb
spark_terminal.ads
spark_terminal.adb
