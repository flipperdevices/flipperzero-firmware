#include "bas_app.h"
#include "app_common.h"
#include "ble.h"
#include <furi/log.h>

void BASAPP_Init(void) {
    uint8_t battery = 65;
    BAS_Update_Char(BATTERY_LEVEL_CHAR_UUID, 0, (uint8_t*)&battery);
}

void BAS_Notification(BAS_Notification_evt_t * pNotification) {
    FURI_LOG_I("BAS", "Update event: %d", pNotification->BAS_Evt_Opcode);
}