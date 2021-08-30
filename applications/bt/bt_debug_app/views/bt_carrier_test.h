#pragma once
#include <gui/view.h>

typedef struct BtCarrierTest BtCarrierTest;

BtCarrierTest* bt_carrier_test_alloc();

void bt_carrier_test_free(BtCarrierTest* bt_carrier_test);

View* bt_carrier_test_get_view(BtCarrierTest* bt_carrier_test);

void bt_carrier_test_set_previous_view(BtCarrierTest* bt_carrier_test, uint32_t view_id);
