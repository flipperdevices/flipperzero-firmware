ICON_SOURCE := $(shell find icons -type f -iname '*.bmp')
OUTPUT_DIR	:= headers

VPATH = $(sort $(dir $(ICON_SOURCE)))

OBJECTS = $(ICON_SOURCE:%.bmp=$(OUTPUT_DIR)/%.xbm)
OBJECTS_PATHS = $(VPATH:%=$(OUTPUT_DIR)/%)

all: $(OUTPUT_DIR)/assets_icons.h

$(OUTPUT_DIR)/assets_icons.h: $(OBJECTS)
	@echo "\tGEN\t" $@
	@./header_generate.py

$(OUTPUT_DIR)/%.xbm: %.bmp $(OUTPUT_DIR)
	@echo "\tICON\t" $@
	@convert $< $@

$(OUTPUT_DIR):
	@echo "\tMKDIR\t" $@
	@mkdir -p $(OBJECTS_PATHS) $@

clean:
	@echo "\tCLEAN"
	@rm -rf $(OUTPUT_DIR)
