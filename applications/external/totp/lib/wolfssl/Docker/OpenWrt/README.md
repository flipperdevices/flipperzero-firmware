This container is really only useful in conjunction with the GitHub Workflow
found in .github/workflows/docker-OpenWrt.yml. The idea is that we will
compile a new libwolfssl that gets placed in official OpenWrt containers to
run some tests ensuring the library is still compatible with existing
binaries.

To run the test locally, build libwolfssl.so (or download from the GitHub Action)
and put it in Docker/OpenWrt. Then switch to that folder and run:
docker build -t openwrt --build-args DOCKER_BASE_CONTAINER=<openwrtContainer> .
where 'openwrtContainer' => "openwrt/rootfs:x86-64-22.03-SNAPSHOT" or similar

This should run some sample tests. The resulting container then can be used to
evaluate OpenWrt with the latest wolfSSL library.
