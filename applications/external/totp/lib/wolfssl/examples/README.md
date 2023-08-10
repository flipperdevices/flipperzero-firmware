# wolfSSL examples directory
## client and server

These directories contain a client (`client.c`) and server (`server.c`) that utilize a variety of the wolfSSL library's capabilities. The manner in which both programs operate can depend on the configure or can be specified at run-time depending on the end goal. Both applications contain testing as well as benchmarking code.

### Compile
```sh
./configure
make
```

### Usage
```sh
./examples/server/server

./examples/client/client
```

Run `./examples/server/server -h` and `./examples/client/client -h`  for usage details.

For simpler wolfSSL TLS server/client examples, visit https://github.com/wolfSSL/wolfssl-examples/tree/master/tls

## echoclient and echoserver

These directories contain a client (`echoclient.c`) and server (`echoserver.c`) that establish a connection encrypted by wolfSSL. Like the names indicate, once the connection has been established any messages entered into echoclient are sent to and displayed on the echoserver and are then echoed back to echoclient. The nature of the encryption, as well as additional behavior of the two programs, depends on how wolfSSL was configured ( DTLS enabled/disabled, Filesystem enabled/disabled, etc ... ).

### Compile
```sh
./configure
make
```

### Usage
```sh
./examples/echoserver/echoserver

./examples/echoclient/echoclient
```

## benchmark

The benchmark directory offers an application that can help you grasp just how well wolfSSL's TLS functionality is performing on your local machine.

### Compile
```sh
./configure
make
```

### Usage
```sh
./examples/benchmark/tls_bench
```

The `tls_bench` executable can also be compiled separately with `gcc -lwolfssl -lpthread -o tls_bench tls_bench.c`.

Run `./examples/benchmark/tls_bench -?` for usage details.

## sctp
This directory contains servers and clients that demonstrate wolfSSL's DTLS-SCTP support.

### Compile
```sh
./configure --enable-sctp
make
```

### Usage
```sh
./examples/sctp/sctp-server

./examples/sctp/sctp-client
```
and

```sh
./examples/sctp/sctp-server-dtls

./examples/sctp/sctp-client-dtls
```

## configs

This directory contains example wolfSSL configuration file templates for use when autoconf is not available, such as building with a custom IDE.

See [configs/README.md](configs/README.md) for more details.

## asn1
This directory contains an example that prints the ASN.1 data of a BER/DER or PEM encoded file. Configure wolfSSL with `--enable-asn-print`.

## pem
This directory contains an example of converting to/from PEM and DER. Configure wolfSSL with `--enable-coding`
