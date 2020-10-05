APP_DIR			= $(PROJECT_ROOT)/applications

CFLAGS			+= -I$(APP_DIR)
C_SOURCES		+= $(APP_DIR)/display-u8g2/display-u8g2.c
C_SOURCES		+= $(APP_DIR)/examples/u8g2_example.c

ifeq ($(TEST), 1)
C_SOURCES += ../applications/tests/furiac_test.c
C_SOURCES += ../applications/tests/furi_record_test.c
C_SOURCES += ../applications/tests/test_index.c
CFLAGS += -DTEST
endif
