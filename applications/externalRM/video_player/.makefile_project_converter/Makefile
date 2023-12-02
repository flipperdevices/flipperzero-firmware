CC=gcc
CXX=g++
CFLAGS+=-O3 -Wall -Wextra -Wpedantic
CXXFLAGS=$(CFLAGS)
	
conv: flipper_video_converter.o BMPLoad.o
	gcc -o $@ $^ -lm
	
	@rm -rf flipper_video_converter.o
	@rm -rf BMPLoad.o
	
	strip conv.exe
	strip conv