wolfSSL for Microsoft Azure Sphere Devices
==========================================

## Description
This directory contains the Visual Studio projects targeted for Azure Sphere.
The example projects include a client, server and wolfCrypt Library Test.
Each of these projects relies on the wolfSSL static library project.
Each project uses `user_settings.h` for to enable and disable features.


### Set Up Steps
0. Setup your Azure Sphere device.
   [Install Azure Sphere](https://docs.microsoft.com/en-us/azure-sphere/install/install)
   [Set up an account](https://docs.microsoft.com/en-us/azure-sphere/install/azure-directory-account)
   [Claim your device](https://docs.microsoft.com/en-us/azure-sphere/install/claim-device)
   [Configure networking](https://docs.microsoft.com/en-us/azure-sphere/install/configure-wifi)

1. Open the wolfssl Microsoft Visual Studio Solution

2. Build All the Projects
    + Right Click: `Solution 'wolfssl' (4 of 4 projects)`
    + Click: `Build Solution`

3. Connect your Azure Sphere MT3620 Development Board using USB.

4. Run the wolfCrypt Library Test
    + Right Click: `wolfcrypt_test (Azure Sphere)`
    + Click: `Debug->'Start new instance'.`

5. Wait for the wolfCrypt Library Test to finish.

6. Test the client.
    + Run client(Azure Sphere) using: `Debug->'Start new instance'`

It's OK if the HTTP GET request returns an error.
The TLS connection was successful.

7. Test the server.
    + Run server(Azure Sphere) using: `Debug->'Start new instance'`
    + Run the following wolfSSL example client command inside wolfssl directory.

```
./examples/client/client -h "Server IP Address" -p 11111 -A ./certs/ca-cert.pem
```

### Client
The client project has defines in `user_settings.h` for:
`SERVER_IP`, `CERT`, `SIZEOF_CERT`, `DEFAULT_PORT` and `msg`.
These are set by default to connect to `www.wolfssl.com`.

If `CUSTOM_SERVER_CONNECTION` is defined then the client would be ready to connect
to a example server at an IP address of `192.168.1.200`.
The example server could be started with the following command:

```
./examples/server/server -b -d -p 11111 -c ./certs/server-cert.pem -k ./certs/server-key.pem
```

Server Options Explanation:
`    -b          Bind to any interface instead of localhost only`
`    -c <file>   Certificate file, default ./certs/server-cert.pem`
`    -d          Disable client cert check`
`    -k <file>   Key file, default ./certs/server-key.pem`
`    -p <num>    Port to listen on, not 0, default 11111`
`    -? <num>    Help, print this usage`


This command assumes that you're in the base directory of 'wolfssl' and it has
been configured and compiled on a computer with an IP address of `192.168.1.200`.
Change `SERVER_IP` under `CUSTOM_SERVER_CONNECTION` in `user_settings.h`
accordingly.

If you would like to connect to a website on the internet other then
`www.wolfssl.com` then you would need to put it's corresponding CA certificate
in `client.h` similarly to `wolfssl_website_root_ca`.

The `CERT` and `SIZEOF_CERT` array could be created using the `dertoc.pl`
script under `wolfssl/scripts/dertoc.pl`.

Usage Example:

```
./scripts/dertoc.pl ./certs/server-cert.der server_cert_der_2048 dertoc.c
```

You would then copy the generated output from `dertoc.c` into `client.h` and set
CERT and `SIZEOF_CERT` accordingly inside `user_settings.h`.
The IP address of the server to connect to also needs to be added to the client's
`app_manifest.json` under 'AllowedConnections'.  There are IP addresses in the
default `app_manifest.json` for testing purposes and can be removed if not needed.


### Server
The Server application will wait for any incoming client connections once built
and uploaded to the MT3620 Development board.

The following wolfSSL example client can connect to a server on the MT3620 board:

```
./examples/client/client -h "Server IP Address" -p 11111 -A ./certs/ca-cert.pem
```

Client Options Explanation:
`    -A <file>   Certificate Authority file, default ./certs/ca-cert.pem`
`    -h <host>   Host to connect to, default 127.0.0.1`
`    -p <num>    Port to listen on, not 0, default 11111`
`    -? <num>    Help, print this usage`


### wolfCrypt Test
This tests the wolfCrypt Library.
This is a good test to run if you change the options in `user_settings.h`.


### Troubleshooting
* Ensure your Azure Sphere MT3620 Development Board was set up using the
  instructions using the Azure Sphere Documentation (See Link Below).
  This includes claiming your device, updating device, setting up networking,
  and prepping for debug.

* The commands for the example client/server assumes it is being run from the
  base directory of wolfssl.

[Azure Sphere Documentation](https://docs.microsoft.com/en-us/azure-sphere/)

[Support Forum](https://www.wolfssl.com/forums/)

[Support Email](support@wolfssl.com)


### Resources

[wolfSSL Website](https://www.wolfssl.com/)

[wolfSSL Wiki](https://github.com/wolfSSL/wolfssl/wiki)

[wolfSSL Manual](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-toc.html)

[wolfSSL API Reference]
(https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-17-wolfssl-api-reference.html)

[wolfCrypt API Reference]
(https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-18-wolfcrypt-api-reference.html)

[TLS 1.3](https://www.wolfssl.com/docs/tls13/)
