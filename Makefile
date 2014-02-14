CFLAGS = -g -Wall -Wextra -Werror
LDFLAGS = -lcheck

all: scan-build test example
	@echo "+++ All good."""

test: tests
	@echo "+++ Running Check test suite..."
	./tests

scan-build: clean
	@echo "+++ Running Clang Static Analyzer..."
	scan-build $(MAKE) tests

clean:
	$(RM) tests *.o

tests: tests.o minmea.o
example: example.o minmea.o
tests.o: tests.c minmea.h
minmea.o: minmea.c minmea.h

.PHONY: all test scan-build clean
