#include "findmy_state.h"
#include <furi_hal.h>
#include <bt/bt_service/bt.h>

void findmy_startup() {
    if(!furi_hal_is_normal_boot()) return;

    // Wait for BT init and check core2
    furi_record_open(RECORD_BT);
    furi_record_close(RECORD_BT);
    if(!furi_hal_bt_is_gatt_gap_supported()) return;

    FindMyState state;
    if(findmy_state_load(&state)) {
        findmy_state_apply(&state);
    }
}
