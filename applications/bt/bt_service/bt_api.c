#include "bt_i.h"

void bt_set_profile(Bt* bt, BtProfile profile) {
    furi_assert(bt);
    BtMessage message = {.type = BtMessageTypeSetProfile, .data.profile = profile};
    furi_check(osMessageQueuePut(bt->message_queue, &message, 0, osWaitForever) == osOK);
}
