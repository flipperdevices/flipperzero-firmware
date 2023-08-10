# Build snifftest on Windows

- To build the snifftest executable on Windows, first follow the steps to build wolfSSL on Visual Studio found [here](https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#building-on-windows).

- Next, download the WinPcap development pack found [here](https://www.winpcap.org/devel.htm).

- Extract the `WpdPack` folder from the downloaded zip and place adjacently to the wolfSSL directory as shown below.

        Projects\
            wolfssl\
            WpdPack\

- Then on Visual Studio, open the configuration manager for the wolfssl solution. This can be done by right-clicking on the solution viewer then selecting `properties`. The button for the configuration manager should be on the top-right of the window.

- On the configuration manager, tick the box to build `sslSniffTest` on your desired configuration. Make sure to configure the project for either `Release` or `Debug` and not `DLL Release` or `DLL Debug`. The snifftest project requires `sslSniffer.lib`, which is not built in the cases of `DLL Release` and `DLL Debug`.

- Add the following to your `user_settings.h`.

```
#define WOLFSSL_STATIC_EPHEMERAL
#define WOLFSSL_DH_EXTRA
#define HAVE_ECC
#define HAVE_ECC_SECPR2
```

- You can now build the solution and see `snifftest.exe` built.



For details on usage, see [sniffer README.md](../README.md#command-line-options) for more details. 
