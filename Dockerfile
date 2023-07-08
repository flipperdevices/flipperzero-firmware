FROM python:3
ENV RUNS_IN_DOCKER=1

ARG UID
ARG GID

RUN apt update && \ 
    apt-get install -y cmake libudev-dev libusb-1.0-0-dev gcc-mingw-w64 g++-mingw-w64 zip

RUN wget -O go.tar.gz https://go.dev/dl/go1.20.5.linux-amd64.tar.gz && tar -C /usr/local -xzf go.tar.gz && rm go.tar.gz
RUN ln -s /usr/local/go/bin/go /usr/local/bin
RUN groupadd -g ${GID} user && \
    useradd -m -l -u ${UID} -g user user

USER user