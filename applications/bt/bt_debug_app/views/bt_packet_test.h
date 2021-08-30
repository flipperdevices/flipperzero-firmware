#pragma once
#include <gui/view.h>

typedef struct BtPacketTest BtPacketTest;

BtPacketTest* bt_packet_test_alloc();

void bt_packet_test_free(BtPacketTest* bt_packet_test);

View* bt_packet_test_get_view(BtPacketTest* bt_packet_test);

void bt_packet_test_set_previous_view(BtPacketTest* bt_carrier_test, uint32_t view_id);
