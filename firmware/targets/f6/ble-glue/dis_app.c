#include "app_common.h"
#include "ble.h"
#include "dis_app.h"
#include <furi-hal-version.h>

void DISAPP_Init(void) {
    DIS_Data_t dis_information_data;

    // Update MANUFACTURER NAME Information
    dis_information_data.pPayload = (uint8_t*)DISAPP_MANUFACTURER_NAME;
    dis_information_data.Length = sizeof(DISAPP_MANUFACTURER_NAME);
    DIS_UpdateChar(MANUFACTURER_NAME_UUID, &dis_information_data);

    // Update MODEL NUMBERInformation
    const char* name = furi_hal_version_get_device_name_ptr();
    dis_information_data.pPayload = (uint8_t*)name;
    dis_information_data.Length = strlen(name) + 1;
    DIS_UpdateChar(MODEL_NUMBER_UUID, &dis_information_data);

    // Update FIRMWARE REVISION NUMBERInformation
    dis_information_data.pPayload = (uint8_t*)DISAPP_FIRMWARE_REVISION_NUMBER;
    dis_information_data.Length = sizeof(DISAPP_FIRMWARE_REVISION_NUMBER);
    DIS_UpdateChar(FIRMWARE_REVISION_UUID, &dis_information_data);

    // Update SOFTWARE REVISION NUMBERInformation
    dis_information_data.pPayload = (uint8_t*)DISAPP_SOFTWARE_REVISION_NUMBER;
    dis_information_data.Length = sizeof(DISAPP_SOFTWARE_REVISION_NUMBER);
    DIS_UpdateChar(SOFTWARE_REVISION_UUID, &dis_information_data);
}
