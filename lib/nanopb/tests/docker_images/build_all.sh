#!/bin/bash -e

# Run all targets
for file in `ls */Dockerfile`
do echo -e "\n\n\n---------------------------------------- Building image for" $file " -------------------------------------------\n\n\n"
docker build $(dirname $file)
done

