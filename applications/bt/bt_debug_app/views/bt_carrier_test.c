#include "bt_carrier_test.h"
#include "bt_test.h"
#include "bt_test_types.h"
#include <m-string.h>

struct BtCarrierTest {
    BtTest* bt_test;
    BtTestMode mode;
    BtTestChannel channel;
    BtTestPower power;
    float rssi;
    string_t message;
};

static BtTestParamValue bt_param_mode[] = {
    {.value = BtTestModeRx, .str = "Rx"},
    {.value = BtTestModeTx, .str = "Tx"},
    {.value = BtTestModeTxHopping, .str = "Hopping Tx"},
};

static BtTestParamValue bt_param_channel[] = {
    {.value = BtTestChannel2402, .str = "2402 MHz"},
    {.value = BtTestChannel2440, .str = "2440 MHz"},
    {.value = BtTestChannel2480, .str = "2480 MHz"},
};

static BtTestParamValue bt_param_power[] = {
    {.value = BtPower0dB, .str = "0 dB"},
    {.value = BtPower2dB, .str = "2 dB"},
    {.value = BtPower4dB, .str = "4 dB"},
    {.value = BtPower6dB, .str = "6 dB"},
};

static uint32_t bt_carrier_test_param_changed(BtTestParam* param, BtTestParamValue* param_val) {
    furi_assert(param);
    uint8_t index = bt_test_get_current_value_index(param);
    bt_test_set_current_value_text(param, param_val[index].str);
    return param_val[index].value;
}

static void bt_carrier_test_mode_changed(BtTestParam* param) {
    BtCarrierTest* bt_carrier_test = bt_test_get_context(param);
    bt_carrier_test->mode = bt_carrier_test_param_changed(param, bt_param_mode);
}

static void bt_carrier_test_channel_changed(BtTestParam* param) {
    BtCarrierTest* bt_carrier_test = bt_test_get_context(param);
    bt_carrier_test->channel = bt_carrier_test_param_changed(param, bt_param_channel);
}

static void bt_carrier_test_param_channel(BtTestParam* param) {
    BtCarrierTest* bt_carrier_test = bt_test_get_context(param);
    bt_carrier_test->power = bt_carrier_test_param_changed(param, bt_param_power);
}

static void bt_carrier_test_change_state_callback(BtTestState state, void* context) {
    furi_assert(context);
    BtCarrierTest* bt_carrier_test = context;
    if(state == BtTestStateStart) {
        string_set_str(bt_carrier_test->message, "Ok - Stop");
    } else if(state == BtTestStateStop) {
        string_set_str(bt_carrier_test->message, "Ok - Start");
    }
    bt_test_set_message(bt_carrier_test->bt_test, string_get_cstr(bt_carrier_test->message));
}

BtCarrierTest* bt_carrier_test_alloc() {
    BtCarrierTest* bt_carrier_test = furi_alloc(sizeof(BtCarrierTest));
    bt_carrier_test->bt_test = bt_test_alloc();
    string_init(bt_carrier_test->message);
    bt_test_set_change_state_callback(
        bt_carrier_test->bt_test, bt_carrier_test_change_state_callback, bt_carrier_test);

    BtTestParam* param;
    param = bt_test_param_add(
        bt_carrier_test->bt_test,
        "Mode",
        SIZEOF_ARRAY(bt_param_mode),
        bt_carrier_test_mode_changed,
        bt_carrier_test);
    bt_test_set_current_value_index(param, 0);
    bt_test_set_current_value_text(param, bt_param_mode[0].str);

    param = bt_test_param_add(
        bt_carrier_test->bt_test,
        "Channel",
        SIZEOF_ARRAY(bt_param_channel),
        bt_carrier_test_channel_changed,
        bt_carrier_test);
    bt_test_set_current_value_index(param, 0);
    bt_test_set_current_value_text(param, bt_param_channel[0].str);

    param = bt_test_param_add(
        bt_carrier_test->bt_test,
        "Power",
        SIZEOF_ARRAY(bt_param_power),
        bt_carrier_test_param_channel,
        bt_carrier_test);
    bt_test_set_current_value_index(param, 0);
    bt_test_set_current_value_text(param, bt_param_power[0].str);

    return bt_carrier_test;
}

void bt_carrier_test_free(BtCarrierTest* bt_carrier_test) {
    furi_assert(bt_carrier_test);
    bt_test_free(bt_carrier_test->bt_test);
    string_clear(bt_carrier_test->message);
    free(bt_carrier_test);
}

View* bt_carrier_test_get_view(BtCarrierTest* bt_carrier_test) {
    furi_assert(bt_carrier_test);
    return bt_test_get_view(bt_carrier_test->bt_test);
}
