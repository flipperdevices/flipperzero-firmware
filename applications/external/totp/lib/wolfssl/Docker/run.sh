#!/bin/bash

echo "Running with \"${*}\"..."

# Assume we're in wolfssl/Docker
WOLFSSL_DIR=$(builtin cd ${BASH_SOURCE%/*}/..; pwd)

docker build -t wolfssl --build-arg UID=$(id -u) --build-arg GID=$(id -g) "${WOLFSSL_DIR}/Docker" && \
  docker run --rm -it -v ${HOME}/.gitconfig:/home/docker/.gitconfig:ro -v ${HOME}/.ssh:/home/docker/.ssh:ro -v "${WOLFSSL_DIR}:/tmp/wolfssl" -w /tmp/wolfssl wolfssl /bin/bash -c "./autogen.sh && ./configure ${*@Q} && make" && \
  docker run --rm -it -v ${HOME}/.gitconfig:/home/docker/.gitconfig:ro -v ${HOME}/.ssh:/home/docker/.ssh:ro -v "${WOLFSSL_DIR}:/tmp/wolfssl" -w /tmp/wolfssl wolfssl /bin/bash

exitval=$?
echo "Exited with error code $exitval"
exit $exitval
