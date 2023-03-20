# syntax=docker/dockerfile-upstream:master-labs
FROM python:3.10 as builder

# temporary, there will be obviously cp into docker container
# COPY . /build
ADD https://github.com/flipperdevices/flipperzero-firmware.git#doom/2991-e2e-runner /build

WORKDIR /build/scripts/flippigator
RUN pip3 install -r requirements-headless.txt
CMD pytest --gui=0
