#pragma once
#include <gui/view.h>

typedef enum {
    BtTestStateStart,
    BtTestStateStop,
} BtTestState;

typedef struct BtTest BtTest;
typedef void (*BtTestChangeStateCallback)(BtTestState state, void* context);
typedef struct BtTestParam BtTestParam;
typedef void (*BtTestParamChangeCallback)(BtTestParam* param);

BtTest* bt_test_alloc();

void bt_test_free(BtTest* bt_test);

View* bt_test_get_view(BtTest* bt_test);

BtTestParam* bt_test_param_add(
    BtTest* bt_test,
    const char* label,
    uint8_t values_count,
    BtTestParamChangeCallback change_callback,
    void* context);

void bt_test_set_change_state_callback(
    BtTest* bt_test,
    BtTestChangeStateCallback callback,
    void* context);

void bt_test_set_message(BtTest* bt_test, const char* message);

void bt_test_set_current_value_index(BtTestParam* param, uint8_t current_value_index);

void bt_test_set_current_value_text(BtTestParam* param, const char* current_value_text);

uint8_t bt_test_get_current_value_index(BtTestParam* param);

void* bt_test_get_context(BtTestParam* param);
