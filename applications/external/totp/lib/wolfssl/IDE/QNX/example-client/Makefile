ARTIFACT = client-tls

#Build architecture/variant string, possible values: x86, armv7le, etc...
PLATFORM = armv7le
OUTPUT_DIR = build
TARGET = $(ARTIFACT)

CC ?= qcc -Vgcc_nto$(PLATFORM)
CXX = qcc -lang-c++ -Vgcc_nto$(PLATFORM)
LD = $(CC)

INCLUDES += -I../../../
LIBS += -L../../../src/.libs/ -lwolfssl
LIBS += -lsocket -lm
CCFLAGS += -O2 -Wall

SRCS = client-tls.c

OBJS = $(addprefix $(OUTPUT_DIR)/,$(addsuffix .o, $(basename $(SRCS))))

$(OUTPUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $(INCLUDES) $(CCFLAGS) $<

$(TARGET):$(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LIBS)

all: $(TARGET)

clean:
	rm -rf $(OUTPUT_DIR) $(ARTIFACT)

rebuild: clean all
