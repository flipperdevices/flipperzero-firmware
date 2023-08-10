CFLAGS := -g -ggdb -Wall -Wextra -std=c11
CINCS  := -I. -I/usr/local/wolfssl
CLIBS  := -L/usr/local/lib
CDEPS  := -lc -lg -lm -lwolfssl

WOLFSSL_BUILD=build
MKDIR_P = mkdir -p

ENABLE_DEBUG_UART_IO       := off
ENABLE_DEBUG_UART_IO_EXTRA := off
ENABLE_SECRET_CALLBACK     := off

ifeq ($(ENABLE_DEBUG_UART_IO), on)
	CFLAGS+=-DDEBUG_UART_IO
endif

ifeq ($(ENABLE_DEBUG_UART_IO_EXTRA), on)
	CFLAGS+=-DDEBUG_UART_IO -DDEBUG_UART_IO_EXTRA_VERBOSE
endif

ifeq ($(ENABLE_SECRET_CALLBACK), on)
	CFLAGS+=-DUSE_SECRET_CALLBACK
endif

OBJS:=$(WOLFSSL_BUILD)/main.o $(WOLFSSL_BUILD)/client-tls13.o

default: all
	.PHONY: all clean help

define run-help =
echo "Run 'make TARGET* OPTION*'"
echo ""
echo "TARGET (specify 0 or more):"
echo "  all (default target)"
echo "    Build main.bin executable"
echo "  main.bin"
echo "    Build main.bin executable"
echo "  main.o"
echo "    Build main.o"
echo "  client-tls13.o"
echo "    Build client-tls13.o"
echo "  clean"
echo "    Clean *.o and *.bin"
echo "  help"
echo "    This help"
echo ""
echo "OPTION (specify 0 or more):"
echo "  ENABLE_DEBUG_UART_IO=on|off (default off)"
echo "    Enable printing ASCII characters sent and received by the UART"
echo "  ENABLE_DEBUG_UART_IO_EXTRA=on|off (default off)"
echo "    Enable more printing hex characters sent and received by the UART"
echo "    Setting this on implicitly sets ENABLE_DEBUG_UART_IO=on"
echo "  ENABLE_SECRET_CALLBACK=on|off (default off)"
echo "    Enable secret callback for TLS 1.3 handshaking, which can be useful for wireshark sniffing"
echo ""
echo "Example:"
echo "  make all ENABLE_DEBUG_UART_IO_EXTRA=on"
echo ""
endef

help:
	@$(run-help)

all: directories main.bin

directories: $(WOLFSSL_BUILD)

$(WOLFSSL_BUILD):
	${MKDIR_P} $(WOLFSSL_BUILD)

main.bin: $(OBJS)
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) -o $@ $^ $(CDEPS)

$(WOLFSSL_BUILD)/main.o: main.c
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) -c -o $@ $^ $(CDEPS)

$(WOLFSSL_BUILD)/client-tls13.o: client-tls13.c
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) -c -o $@ $^ $(CDEPS)

clean:
	rm -f *.o *.bin
	rm -rf build
