Nanopb - Protocol Buffers for Embedded Systems
==============================================

[![Build Status](https://travis-ci.com/nanopb/nanopb.svg?branch=master)](https://travis-ci.com/nanopb/nanopb)

Nanopb is a small code-size Protocol Buffers implementation in ansi C. It is
especially suitable for use in microcontrollers, but fits any memory
restricted system.

* **Homepage:** https://jpa.kapsi.fi/nanopb/
* **Documentation:** https://jpa.kapsi.fi/nanopb/docs/
* **Downloads:** https://jpa.kapsi.fi/nanopb/download/
* **Forum:** https://groups.google.com/forum/#!forum/nanopb
* **Nightly builds:** https://jpa.kapsi.fi/jenkins/job/nanopb/


Using the nanopb library
------------------------
To use the nanopb library, you need to do two things:

1. Compile your .proto files for nanopb, using `protoc`.
2. Include *pb_encode.c*, *pb_decode.c* and *pb_common.c* in your project.

The easiest way to get started is to study the project in "examples/simple".
It contains a Makefile, which should work directly under most Linux systems.
However, for any other kind of build system, see the manual steps in
README.txt in that folder.


Generating the headers
----------------------
Protocol Buffers messages are defined in a `.proto` file, which follows a standard
format that is compatible with all Protocol Buffers libraries. To use it with nanopb,
you need to generate `.pb.c` and `.pb.h` files from it:

    python generator/nanopb_generator.py myprotocol.proto  # For source checkout
    generator-bin/nanopb_generator myprotocol.proto        # For binary package

(Note: For instructions for nanopb-0.3.9.x and older, see the documentation
of that particular version [here](https://github.com/nanopb/nanopb/blob/maintenance_0.3/README.md))

The binary packages for Windows, Linux and Mac OS X should contain all necessary
dependencies, including Python, python-protobuf library and protoc. If you are
using a git checkout or a plain source distribution, you will need to install
Python separately. Once you have Python, you can install the other dependencies
with `pip install protobuf grpcio-tools`.

You can further customize the header generation by creating an `.options` file.
See [documentation](https://jpa.kapsi.fi/nanopb/docs/concepts.html#modifying-generator-behaviour) for details.


Running the tests
-----------------
If you want to perform further development of the nanopb core, or to verify
its functionality using your compiler and platform, you'll want to run the
test suite. The build rules for the test suite are implemented using Scons,
so you need to have that installed (ex: `sudo apt install scons` or `pip install scons`).
To run the tests:

    cd tests
    scons

This will show the progress of various test cases. If the output does not
end in an error, the test cases were successful.

Note: Mac OS X by default aliases 'clang' as 'gcc', while not actually
supporting the same command line options as gcc does. To run tests on
Mac OS X, use: `scons CC=clang CXX=clang`. Same way can be used to run
tests with different compilers on any platform.

For embedded platforms, there is currently support for running the tests
on STM32 discovery board and [simavr](https://github.com/buserror/simavr)
AVR simulator. Use `scons PLATFORM=STM32` and `scons PLATFORM=AVR` to run
these tests.


Build systems and integration
-----------------------------
Nanopb C code itself is designed to be portable and easy to build
on any platform. Often the bigger hurdle is running the generator which
takes in the `.proto` files and outputs `.pb.c` definitions.

There exist build rules for several systems:

* **Makefiles**: `extra/nanopb.mk`, see `examples/simple`
* **CMake**: `extra/FindNanopb.cmake`, see `examples/cmake`
* **SCons**: `tests/site_scons` (generator only)
* **Bazel**: `BUILD` in source root
* **Conan**: `conanfile.py` in source root
* **PlatformIO**: https://platformio.org/lib/show/431/Nanopb
* **PyPI/pip**: https://pypi.org/project/nanopb/

And also integration to platform interfaces:

* **Arduino**: http://platformio.org/lib/show/1385/nanopb-arduino

Building nanopb - Using vcpkg
-----------------------------

You can download and install nanopb using the [vcpkg](https://github.com/Microsoft/vcpkg) dependency manager:

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ./vcpkg integrate install
    ./vcpkg install nanopb

The nanopb port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.
