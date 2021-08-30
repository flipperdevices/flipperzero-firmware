#include "bt_packet_test.h"
#include "bt_test.h"

struct BtPacketTest {
    BtTest* bt_test;
};

BtPacketTest* bt_packet_test_alloc() {
    BtPacketTest* bt_packet_test = furi_alloc(sizeof(BtPacketTest));
    bt_packet_test->bt_test = bt_test_alloc();
    return bt_packet_test;
}

void bt_packet_test_free(BtPacketTest* bt_packet_test) {
    furi_assert(bt_packet_test);
    bt_test_free(bt_packet_test->bt_test);
    free(bt_packet_test);
}

View* bt_packet_test_get_view(BtPacketTest* bt_packet_test) {
    furi_assert(bt_packet_test);
    return bt_test_get_view(bt_packet_test->bt_test);
}