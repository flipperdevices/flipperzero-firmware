default_target: all

all: clean linux windows

linux:
	mkdir -p build/linux
	cd build/linux && cmake ../../ && make

windows:
	mkdir -p build/windows
	cd build/windows && cmake --toolchain ./mingw.cmake ../../ && make

clean:
	rm -rf build
