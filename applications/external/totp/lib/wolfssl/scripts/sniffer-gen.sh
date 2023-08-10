#!/bin/bash
#set -x

# Run this script from the wolfSSL root
if [ ! -f wolfssl/ssl.h ]; then
    echo "Run from the wolfssl root"
    exit 1
fi

server_pid=0
tcpdump_pid=0

cleanup() {
    if [ "$server_pid" -ne 0 ]; then kill $server_pid; server_pid=0; fi
    if [ "$tcpdump_pid" -ne 0 ]; then sleep 1; kill -15 $tcpdump_pid; tcpdump_pid=0; fi
}
trap cleanup EXIT INT TERM HUP

set -o pipefail
prepend() { # Usage: cmd 2>&1 | prepend "sometext "
    while read line; do echo "${1}${line}"; done
}

run_test() { # Usage: run_test <cipher> [serverArgs [clientArgs]]
    echo "Running test $1"
    CIPHER=$1
    if [ "$CIPHER" != "" ]; then
        CIPHER="-l $CIPHER"
    fi
    stdbuf -oL -eL ./examples/server/server -i -x $CIPHER $2 2>&1 | prepend "[server] " &
    server_pid=$!
    ((server_pid--)) # Get the first PID in the pipe
    sleep 0.1
    stdbuf -oL -eL ./examples/client/client $CIPHER $3 2>&1 | prepend "[client] "
    RET=$?
    if [ "$RET" != 0 ]; then
        echo "Error in test: $RET"
        exit $RET
    fi
    kill $server_pid; server_pid=0
    echo "Test passed: $1"
}

run_sequence() {
    if [ "$1" == "tls13-dh" ] || [ "$1" == "tls13-ecc" ]; then # TLS v1.3
        run_test "TLS13-AES128-GCM-SHA256" "-v 4" "-v 4"
        run_test "TLS13-AES256-GCM-SHA384" "-v 4" "-v 4"
        run_test "TLS13-CHACHA20-POLY1305-SHA256" "-v 4" "-v 4"
    elif [ "$1" == "tls12" ]; then # TLS v1.2
        run_test "ECDHE-ECDSA-AES128-GCM-SHA256" "-v 3 -A ./certs/ca-ecc-cert.pem -k ./certs/ecc-key.pem -c ./certs/intermediate/server-chain-ecc.pem -V" "-v 3 -A ./certs/ca-ecc-cert.pem -k ./certs/ecc-client-key.pem -c ./certs/intermediate/client-chain-ecc.pem -C"
        run_test "ECDHE-ECDSA-AES256-GCM-SHA384" "-v 3 -A ./certs/ca-ecc-cert.pem -k ./certs/ecc-key.pem -c ./certs/intermediate/server-chain-ecc.pem -V" "-v 3 -A ./certs/ca-ecc-cert.pem -k ./certs/ecc-client-key.pem -c ./certs/intermediate/client-chain-ecc.pem -C"
    elif [ "$1" == "tls13-dh-resume" ] || [ "$1" == "tls13-ecc-resume" ]; then # TLS v1.3 Resumption
        run_test "TLS13-AES128-GCM-SHA256" "-v 4 -r" "-v 4 -r"
        run_test "TLS13-AES256-GCM-SHA384" "-v 4 -r" "-v 4 -r"
        run_test "TLS13-CHACHA20-POLY1305-SHA256" "-v 4 -r" "-v 4 -r"
    elif [ "$1" == "tls13-x25519" ]; then # TLS v1.3
        run_test "TLS13-AES128-GCM-SHA256" "-v 4 -c ./certs/ed25519/server-ed25519.pem -k ./certs/ed25519/server-ed25519-priv.pem -A ./certs/ed25519/client-ed25519.pem" "-v 4 -c ./certs/ed25519/client-ed25519.pem -k ./certs/ed25519/client-ed25519-priv.pem -A ./certs/ed25519/root-ed25519.pem"
        run_test "TLS13-AES256-GCM-SHA384" "-v 4 -c ./certs/ed25519/server-ed25519.pem -k ./certs/ed25519/server-ed25519-priv.pem -A ./certs/ed25519/client-ed25519.pem" "-v 4 -c ./certs/ed25519/client-ed25519.pem -k ./certs/ed25519/client-ed25519-priv.pem -A ./certs/ed25519/root-ed25519.pem"
        run_test "TLS13-CHACHA20-POLY1305-SHA256" "-v 4 -c ./certs/ed25519/server-ed25519.pem -k ./certs/ed25519/server-ed25519-priv.pem -A ./certs/ed25519/client-ed25519.pem" "-v 4 -c ./certs/ed25519/client-ed25519.pem -k ./certs/ed25519/client-ed25519-priv.pem -A ./certs/ed25519/root-ed25519.pem"
    elif [ "$1" == "tls13-x25519-resume" ]; then # TLS v1.3 x25519 Resumption
        run_test "TLS13-AES128-GCM-SHA256" "-v 4 -r -c ./certs/ed25519/server-ed25519.pem -k ./certs/ed25519/server-ed25519-priv.pem -A ./certs/ed25519/client-ed25519.pem" "-v 4 -r -c ./certs/ed25519/client-ed25519.pem -k ./certs/ed25519/client-ed25519-priv.pem -A ./certs/ed25519/root-ed25519.pem"
        run_test "TLS13-AES256-GCM-SHA384" "-v 4 -r -c ./certs/ed25519/server-ed25519.pem -k ./certs/ed25519/server-ed25519-priv.pem -A ./certs/ed25519/client-ed25519.pem" "-v 4 -r -c ./certs/ed25519/client-ed25519.pem -k ./certs/ed25519/client-ed25519-priv.pem -A ./certs/ed25519/root-ed25519.pem"
        run_test "TLS13-CHACHA20-POLY1305-SHA256" "-v 4 -r -c ./certs/ed25519/server-ed25519.pem -k ./certs/ed25519/server-ed25519-priv.pem -A ./certs/ed25519/client-ed25519.pem" "-v 4 -r -c ./certs/ed25519/client-ed25519.pem -k ./certs/ed25519/client-ed25519-priv.pem -A ./certs/ed25519/root-ed25519.pem"
    elif [ "$1" == "tls13-hrr" ]; then # TLS v1.3 Hello Retry Request
        run_test "" "-v 4 -g" "-v 4 -J"
    else
        echo "Invalid test"
        exit 1
    fi
}

run_capture(){
    echo -e "\nconfiguring and building wolfssl ($1)..."
    ./configure --enable-sniffer $2 1>/dev/null || exit $?
    make 1>/dev/null || exit $?
    echo "starting capture"
    tcpdump -i lo -n port 11111 -w ./scripts/sniffer-${1}.pcap -U &
    tcpdump_pid=$!
    run_sequence $1
    sleep 1
    kill -15 $tcpdump_pid; tcpdump_pid=0
}

run_capture "tls12"               ""
run_capture "tls13-ecc"           ""
run_capture "tls13-ecc-resume"    "--enable-session-ticket"
run_capture "tls13-dh"            "--disable-ecc"
run_capture "tls13-dh-resume"     "--disable-ecc --enable-session-ticket"
run_capture "tls13-x25519"        "--enable-curve25519 --disable-dh --disable-ecc"
run_capture "tls13-x25519-resume" "--enable-curve25519 --disable-dh --disable-ecc --enable-session-ticket"
run_capture "tls13-hrr"           "--disable-dh CFLAGS=-DWOLFSSL_SNIFFER_WATCH"

echo "Tests passed in $SECONDS seconds"
