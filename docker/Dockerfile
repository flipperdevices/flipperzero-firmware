FROM alpine:3.14 as builder

RUN apk add --no-cache \
    git \
    g++ \
    libtool \
    make \
    boost-dev \
    libgcrypt-dev \
    wget

RUN wget --progress=dot:giga http://srecord.sourceforge.net/srecord-1.64.tar.gz && \
    tar -xvzf srecord-1.64.tar.gz && cd srecord-1.64 && \
    ./configure && make -j"$(nproc)" all-bin && make install-bin && make install-libdir && \
    srec_cat -vers

RUN git clone https://github.com/rusdacent/hex2dfu.git && \
    cd hex2dfu && gcc hex2dfu.c ED25519/*.c -o hex2dfu && mv ./hex2dfu /usr/local/bin/hex2dfu  && \
    hex2dfu -h

FROM alpine:3.14

RUN apk add --no-cache \
        make \
        python3 \
        py3-lxml \
        git \
        clang-extra-tools && \
    apk add --no-cache -X http://dl-cdn.alpinelinux.org/alpine/v3.14/community \
        gcc-arm-none-eabi=10.2.0-r2 \
        newlib-arm-none-eabi=4.1.0-r0 \
        openocd=0.11.0-r0 && \
    apk add --no-cache -X http://dl-cdn.alpinelinux.org/alpine/edge/testing \
        dfu-util=0.10-r0

COPY --from=builder /usr/local/bin /usr/local/bin
COPY --from=builder /usr/local/lib /usr/local/lib

COPY entrypoint.sh syntax_check.sh /

RUN chmod +x /syntax_check.sh

ENTRYPOINT ["/entrypoint.sh"]
