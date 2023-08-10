# This Dockerfile is used in conjunction with the docker-OpenWrt.yml GitHub Action.
ARG DOCKER_BASE_CONTAINER=openwrt/rootfs:x86-64-snapshot
FROM $DOCKER_BASE_CONTAINER

RUN mkdir -p /var/lock # Fix for parent container
COPY libwolfssl.so /tmp/libwolfssl.so
RUN export LIBWOLFSSL=$(ls /usr/lib/libwolfssl.so.* -1); \
                      rm ${LIBWOLFSSL} && ln -s /tmp/libwolfssl.so ${LIBWOLFSSL}
# for debugging purposes to make sure the correct library is tested
RUN ls -Ll /usr/lib/libwolfssl* && ldd /lib/libustream-ssl.so | grep wolfssl
COPY runTests.sh /tmp/.
RUN /tmp/runTests.sh
