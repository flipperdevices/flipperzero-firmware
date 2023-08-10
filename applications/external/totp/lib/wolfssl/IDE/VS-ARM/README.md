VisualStudio for ARM build
==========================

VS2017 now offers solution options for Android and iOS applications. It includes
the GCC cross-compiler suite and property sheets for configuring a project
for those platforms and libraries for cross-platform mobile application
development.

The provided solution file will build wolfSSL for linking into an ARM based
application, *libwolfssl.a*. The *user_settings.h* file enables some of the
hardened settings and disables most uncommon ciphers.
