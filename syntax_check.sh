#!/usr/bin/env bash

echo "BUILD CONTAINER"
sudo docker build ./docker --tag flipper-syntax-check

echo "RUN CONTAINER"
sudo docker run --rm --name c-syntax-check --entrypoint /syntax_check.sh -v $(pwd):/project flipper-syntax-check \
