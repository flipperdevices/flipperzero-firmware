lib_LTLIBRARIES=libsomething.la
libsomething_la_SOURCES=$(ASN_MODULE_SOURCES) $(ASN_MODULE_HEADERS)

ASN_CONVERTER_SOURCES+=test/parse.c
ASN_MODULE_SOURCES=$(wildcard lib/asn1/*.c)
ASN_MODULE_HEADERS=$(wildcard lib/asn1/*.h)

TARGET = parse
CFLAGS += -I. -Ilib/asn1
OBJS=${ASN_MODULE_SOURCES:.c=.o} ${ASN_CONVERTER_SOURCES:.c=.o}

all: regen

test: $(TARGET)

$(TARGET): regen ${OBJS}
	$(CC) $(CFLAGS) -o $(TARGET) ${OBJS} $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

regen: regenerate-from-asn1-source

regenerate-from-asn1-source:
	@asn1c -D lib/asn1 -no-gen-example -pdu=all seader.asn1

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
