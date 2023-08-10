# wolfSSL and wolfCrypt Xcode Projects for OS X and iOS

This directory contains the following files:

1. `wolfssl.xcworkspace` -- workspace with library and testsuite client
2. `wolfssl_testsuite.xcodeproj` -- project to run the testsuite.
3. `wolfssl.xcodeproj` -- project to build OS/x and iOS libraries for wolfSSL and/or wolfCrypt
4. `wolfssl-FIPS.xcodeproj` -- project to build wolfSSL and wolfCrypt-FIPS if available
5. `user_settings.h` -- custom library settings, which are shared across projects

The library will output as `libwolfssl_osx.a` or 'libwolfssl_ios.a` depending on 
the target. It will also copy the wolfSSL/wolfCrypt (and the CyaSSL/CtaoCrypt 
compatibility) headers into an `include` directory located in 
`Build/Products/Debug` or `Build/Products/Release`.

For the library and testsuite to link properly the build location needs to be 
configured as realitive to workspace.
1. File -> Workspace Settings (or Xcode -> Preferences -> Locations -> Locations)
2. Derived Data -> Advanced
3. Custom -> Relative to Workspace
4. Products -> Build/Products

These Xcode projects define the `WOLFSSL_USER_SETTINGS` preprocessor 
to enable the `user_settings.h` file for setting macros across 
multiple projects.

If needed the Xcode preprocessors can be modified with these steps:
1. Click on the Project in "Project Navigator".
2. Click on the "Build Settings" tab.
3. Scroll down to the "Apple LLVM 6.0 - Preprocessing" section.
4. Open the disclosure for "Preprocessor Macros" and use the "+" and 
"-" buttons to modify. Remember to do this for both Debug and Release.

## wolfSSL

This project should build wolfSSL and wolfCrypt using the default settings.

## wolfSSL-FIPS

To use the FIPS version, one must have the FIPS sources. The project won't
build without them. Please contact info@wolfssl.com for more information about
wolfCrypt with FIPS.

By default, this builds the wolfSSL and wolfCrypt with FIPS library. The default
configuration enables the settings required for FIPS. Others may be turned on.
The project also ensures the FIPS related objects are linked in the proper
order.


# Building libwolfssl.a

There are several options of builds. You can make a simulator build, or a
device build. Both are debug builds.

You can make an archive for a device, as well. That is a release build.

## Known issues:

When building for older i386 architectures and using tfm.c there are specific
CFLAGS required to expose the necessary registers for inline assembly in tfm.c.
An example script has been provided "build-for-i386.sh" that targets the watchos
by default. If using SDK iphonesimulator10.1 or older you can change the SDK
variable in that script however newer versions of the SDK no longer support
i386 for the iphones.

# Installing libwolfssl.a

Simply drag the file libwolfssl_XXX_.a and the directory `include` and drop it into
your project file list pane where it makes sense for you. Allow it to copy the
files over to the project directory. This should automatically add the library
to the list of libraries to link against.

Click on your project target, then the "Build Phases" tab. On the targets list
click your target. Click the disclosure triangle on the section "Link Binary
With Libraries" and verify libwolfssl.a is in the list. If not, click the "+",
and on the "Choose frameworks and libraries to add:" dialog, click the
button "Add other..." then find the file libwolfssl.a.

Click on the "Build Settings" tab. Scroll down to the section "Search Paths".
Add the path to the include directory to the list "Header Search Paths".


## When using FIPS

When using the FIPS version the following preprocessors need to be defined:

* `IPHONE`
* `HAVE_FIPS`
* `HAVE_HASHDRBG`
* `HAVE_AESGCM`
* `WOLFSSL_SHA512`
* `WOLFSSL_SHA384`
* `NO_MD4`
* `NO_DSA`
* `NO_PWDBASED`

The approved FIPS source files are from the CyaSSL project tag v3.4.8.fips. The
files fips.c and fips_test.c, and the wolfCAVP test app are from the FIPS
project tag v3.4.8a. The wolfSSL/wolfCrypt files are from tag v3.4.8.

# Using the FIPS library

The FIPS library contains a self-check verify hash. Normally, on the desktop or
server build, the library is built as a dynamic library. The library looks the
same to every application that builds against it, and can be verified. For
static libraries, when linking into your application, the addresses are all
fixed, and the verify checksum becomes unusable. iOS does not allow dynamic
libraries like this, so static builds are required. This creates a problem.
Every time the application is changed, the FIPS checksum will change, because
the FIPS library's position in the executable may change.

You need to add something to your application that will output the verifyCore
value to be used. The verifyCore in fips_test.c will need to be updated with
this value, the library rebuilt, and relinked into your application. The
application should not be changed during this process or the verifyCore check
will fail again.
