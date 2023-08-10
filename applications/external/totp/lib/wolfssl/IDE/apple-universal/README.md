# Overview
This example shows how to build a wolfSSL static library for Apple targets on all architectures using GNU autotools/`configure` and demonstrates how to create a [universal binary framework]() suitable for use in an Xcode project. It also provides a demo Xcode project using the wolfSSL framework in a simple multiplatform app.

The example was created using Xcode version 14.3.1.

# Why?
Configuring and building wolfSSL through the `configure` interface can be simpler and more user friendly than manually adding the wolfSSL source files to your project and customizing through `user_settings.h`. Building via `configure` also streamlines integration with other open-source projects that expect an installation directory, such as `cURL`'s `--with-wolfssl` option. Finally, some developer teams might prefer to build wolfSSL once with the desired settings and then distribute it as a library framework for app developers to use. Packaging wolfSSL as a framework makes it highly portable and allows for drag-and-drop integration into Xcode projects without needing to worry about compiling the library every time they build their app.

However, if you do want to compile wolfSSL from source manually in your Xcode project using `user_settings.h`, see the example in [IDE/XCODE](https://github.com/wolfSSL/wolfssl/tree/master/IDE/XCODE).

# Example overview
This example consists of a build script and an Xcode example project. The build script generates a static library framework for all Apple targets. The Example project shows how to incorporate the framework into an Xcode project and wolfSSL framework in a simple application.

## The build script
`build-wolfssl-framework.sh` compiles wolfSSL as static library for all modern Apple platforms and simulators. This includes MacOS (`arm64`,`x86_64`), iPhone (`arm64`), iPhoneSimulator (`arm64`,`x86_64`), appleTV (`arm64`), appleTVSimulator (`arm64`,`x86_64`), appleWatch (`arm64`), and appleWatchSimulator (`arm64`,`x86_64`). The script compiles wolfSSL for each platform, creates universal binaries for platforms that support multiple architectures (macOS and simulators) using [lipo](https://developer.apple.com/documentation/apple-silicon/building-a-universal-macos-binary), then combines all the static libraries together into an `xcframework` that can be imported into Xcode. It is meant to be used as an example rather than a build tool, and chooses simplicity and readability over flexibility (no command line options). For an explanation of how the script cross compiles wolfSSL, see the [Technical Details](technical-details) section.

To use the build script, you can run it without arguments to build a default configuration, or you can use the `-c` option to pass in a quoted string containing any additional flags to `configure` that you need. Note that `--enable-static --disable-shared` is always passed to `configure` by default. Consider the following usage example, with descriptions in the comments:

```
# default configuration
./build-wolfssl-framework.sh

# hardened configuration with curl support and FIPS-ready crypto
./build-wolfssl-framework.sh -c "--enable-harden --enable-curl --enable-fips=ready"

```

## Example project
`wolfssl-multiplatform` is an xcode project containing a simple swiftUI "hello world" app that has been modified to run the wolfCrypt tests and establish a TLS connection to `www.wolfssl.com` on startup. It also provides an example for basic Swift/C interoperability using a "bridging header". When the app launches, the swiftUI initialization handler calls a C test driver function, which is responsible for running the wolfSSL examples. An overview of the additional files is as follows:

```
.
└── wolfssl-multiplatform
    ├── wolfssl-multiplatform
    │   ├── ContentView.swift # <-- boilerplate swiftUI modified to call wolfSSL test driver on UI init
    │   ├── wolfssl_multiplatformApp.swift  # <-- basic swift hello world

    │   ├── simple_client_example.c # <-- Simple TLS example that connects to wolfssl.com
    │   ├── simple_client_example.h

    │   ├── wolfssl-multiplatform-Bridging-Header.h # <-- "bridging header" that exposes wolfssl_test_driver app to swift
    │   ├── wolfssl_test_driver.c # <-- test driver function that runs wolfCrypt tests then calls simple_client_example
    │   └── wolfssl_test_driver.h
```

For a basic overview on how to call C code from Swift in an Xcode project, see this excellent blog post tutorial:
- [https://rlaguilar.com/posts/integrate-c-library-ios-project-swift](https://rlaguilar.com/posts/integrate-c-library-ios-project-swift)

More detailed information on swift/C interoperability can be found in the Apple swift language guide, as well as in the official swift documentation:
- [https://developer.apple.com/documentation/swift/c-interoperability](https://developer.apple.com/documentation/swift/c-interoperability)
- [https://www.swift.org/documentation/cxx-interop](https://www.swift.org/documentation/cxx-interop)

## Adding the framework to an Xcode project
In order to add the framework to any Xcode project, you can simply drag-and-drop the `artifacts/xcframework/libwolfssl.xcframework` directory into Xcode's project source navigator pane. This should automatically add it to the linked libraries for your application.

# Technical Details

## Cross compilation
If you are developing on a macOS machine and want to compile wolfSSL to run on macOS, then you can simply use `configure` without further customisation. However, if you wish to build wolfSSL to run on a different Apple device, then you need to cross-compile wolfSSL. Thankfully, `configure` makes cross compilation relatively straightforward by using the `--host` argument to pass the "[target triple](https://wiki.osdev.org/Target_Triplet)" describing the platform of the system on which you wish the binary to run, as well as a few other options which will are described below. For more details on cross-compilation, please see the [GNU cross-compilation documentation](https://www.gnu.org/software/automake/manual/html_node/Cross_002dCompilation.html) and the [wolfSSL manual page on cross-compiling with configure](https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#building-with-configure-with-cross-compile). Note that `clang` is the default compiler on macOS (symlinked to `/usr/bin/gcc`) and natively supports cross compilation for all Apple devices without requiring you to download a separate compiler. This means you do not need to override the system `CC`/`AR`/`RANLIB` etc. when using configure.

The generic `configure` invocation required to cross compile a static library for an Apple device is as follows:

```
./configure --disable-shared --enable-static \
            --prefix=${INSTALL_DIR} \
            --host=${HOST} \
            CFLAGS="-arch ${ARCH} -isysroot ${SDK_ROOT}"

```
where the
- `${INSTALL_DIR}` holds the path to the output directory for the wolfSSL install (which we will later include in the framework)
- `--host=${HOST}` is the triple describing the platform. It should be set to `${ARCH}-apple-darwin` for all targets
- `-arch ${ARCH}` is the CPU architecture of the platform. It should be `x86_64` for intel Macs and `arm64` for iPhone, appleTV, appleWatch, and Apple silicon Macs.
- `-isysroot ${SDK_ROOT}` is the path to the new sysroot for the target platform of cross compilation, which is where the compiler should look for system headers and libraries (which are usually different for the target system than for the host system when cross compiling). You can use the Xcode command line tools to query the SDK root path for a given target by running `xcrun --sdk <target> --show-sdk-path`. To get a list of installed targets, run `xcodebuild -showsdks`.


## Universal binaries
Apple intoduced two technologies to facilitate packaging portable libraries: "universal binaries" and "frameworks".

Universal binaries (a.k.a "fat" binaries) allow `elf` files targeting multiple CPU architectures to be combined into a single file (e.g. `x86_64` and `arm64`). These binaries are created using a tool called `lipo`. For more information on lipo and universal binaries, see [Creating Universal Binaries](https://developer.apple.com/documentation/apple-silicon/building-a-universal-macos-binary).

## Frameworks
In order to facilitate distribution binaries and dependencies, Apple introduced the concept of an `xcframework` bundle, which is a distribution format that allows developers to bundle binaries targeting multiple architectures together with their headers and other metadata. All builds of a library under all target platforms and architectures complete with their dependencies now can be packed ino one single bundle under the `.xcframework` extension.

## Issues with the process
Low-level programming in the Apple ecosystem is sparsely documented, and certain things that you think "should just work" don't. Here are a few issues we had with the process that need to be documented.

1. Apps meant to run on a simulator require building for/linking against universal binaries containing architecture slices for both `x86_64` and `arm64`. Even if you have the correct architecture (e.g. compiling on `arm64` and targeting an `arm64` simulator host) Xcode will complain that you have compiled the binary for the wrong host if the elf file does not include an `x86_64` architecture slice. Therefore, `build-wolfssl-framework.sh` builds all libraries for simulator targets for both `x86_64` and `arm64` architectures and links them as universal binaries with `lipo`. Again, it DOES NOT MATTER if you are targeting the correct architecture with your cross-compilation, Xcode will not recognize the binary as targeting the correct architecture unless it contains both.

2. Cross compiling for the **iOS simulator** with a min version specifier present (`-miphoneos-version-min`) requires the `-target ${ARCH}-apple-ios-simulator` compiler flag in order to build . It is unclear why this is required, as The GNU documentation claims that the `target` option is only required if cross-compiling a compiler to run on architecture X but emit code for architecture Y (known as a canadian cross-compilation scenario). Regardless, if you do not include a `-target` option, the build will generate a large number of warnings when linking against system libraries with messages like: `ld: warning: building for iOS, but linking in .tbd file (/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator16.4.sdk/usr/lib/libnetwork.tbd) built for iOS Simulator`. It was thought that perhaps the host option should instead be `--host=${ARCH}-apple-ios-simulator` but this is not a valid option, and `configure` will fail with a different error: `checking host system type... Invalid configuration 'arm64-apple-ios-simulator': Kernel 'ios' not known to work with OS 'simulator`. If you do not specify a min iOS version, this is not required. Mysteriously, the other simulators (tvOS, watchOS) do not have this issue....


