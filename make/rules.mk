OBJ_DIR := $(OBJ_DIR)/$(TARGET)

# Include source folder paths to virtual paths
VPATH = $(sort $(dir $(C_SOURCES)) $(dir $(ASM_SOURCES)) $(dir $(CPP_SOURCES)))

# Gather object
OBJECTS = $(addprefix $(OBJ_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(OBJ_DIR)/, $(notdir $(ASM_SOURCES:.s=.o)))
OBJECTS += $(addprefix $(OBJ_DIR)/, $(notdir $(CPP_SOURCES:.cpp=.o)))

# Generate dependencies
DEPS = $(OBJECTS:.o=.d)

OUTFILES = $(OBJ_DIR)/$(PROJECT).elf $(OBJ_DIR)/$(PROJECT).hex $(OBJ_DIR)/$(PROJECT).bin

$(shell mkdir -p $(OBJ_DIR))

all: $(OUTFILES)

$(OBJ_DIR)/$(PROJECT).elf: $(OBJECTS)
	@echo "\tLD\t" $@
	@$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	@$(SZ) $@

$(OBJ_DIR)/$(PROJECT).hex: $(OBJ_DIR)/$(PROJECT).elf
	@echo "\tHEX\t" $@
	@$(HEX) $< $@
	
$(OBJ_DIR)/$(PROJECT).bin: $(OBJ_DIR)/$(PROJECT).elf
	@echo "\tBIN\t" $@
	@$(BIN) $< $@

$(OBJ_DIR)/%.o: %.c
	@echo "\tCC\t" $@
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.s
	@echo "\tASM\t" $@
	@$(AS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@echo "\tCPP\t" $@
	@$(CPP) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

flash: $(OBJ_DIR)/$(PROJECT).bin
	st-flash --reset write $(OBJ_DIR)/$(PROJECT).bin $(FLASH_ADDRESS)

upload:
	dfu-util -D $(OBJ_DIR)/$(PROJECT).bin -a 0 -s $(FLASH_ADDRESS)

debug:
	st-util & arm-none-eabi-gdb -ex "target extended-remote 127.0.0.1:4242" $(OBJ_DIR)/$(PROJECT).elf

clean:
	@echo "\tCLEAN\t"
	@$(RM) $(OBJ_DIR)/*

zz: | clean flash

zzz: | clean flash debug

-include $(DEPS)
