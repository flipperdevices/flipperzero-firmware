BUILD_DIR = ./Build

all: WolfSSLStaticLib WolfCryptTest WolfCryptBench WolfSSLClient WolfSSLServer

WolfCryptTest:
	$(MAKE) -f Makefile.test
WolfCryptBench:
	$(MAKE) -f Makefile.bench
WolfSSLClient:
	$(MAKE) -f Makefile.client
WolfSSLServer:
	$(MAKE) -f Makefile.server
WolfSSLStaticLib:
	$(MAKE) -f Makefile.static

clean:
	rm -f $(BUILD_DIR)/*.elf $(BUILD_DIR)/*.hex $(BUILD_DIR)/*.map
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.a $(BUILD_DIR)/*.sym $(BUILD_DIR)/*.disasm
