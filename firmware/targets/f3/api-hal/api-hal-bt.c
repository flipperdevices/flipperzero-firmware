#include <api-hal-bt.h>
#include <app_entry.h>
#include <ble.h>


static void Reset_IPCC( void )
{
    LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_IPCC);

    LL_C1_IPCC_ClearFlag_CHx(IPCC,
            LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 |
            LL_IPCC_CHANNEL_4 | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);

    LL_C2_IPCC_ClearFlag_CHx(IPCC,
            LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 |
            LL_IPCC_CHANNEL_4 | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);

    LL_C1_IPCC_DisableTransmitChannel(IPCC,
            LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 |
            LL_IPCC_CHANNEL_4 | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);

    LL_C2_IPCC_DisableTransmitChannel(IPCC,
            LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 |
            LL_IPCC_CHANNEL_4 | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);

    LL_C1_IPCC_DisableReceiveChannel(IPCC,
            LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 |
            LL_IPCC_CHANNEL_4 | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);

    LL_C2_IPCC_DisableReceiveChannel(IPCC,
            LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 |
            LL_IPCC_CHANNEL_4 | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);
}

void api_hal_bt_init() {
    Reset_IPCC();

    LL_PWR_SMPS_SetStartupCurrent(LL_PWR_SMPS_STARTUP_CURRENT_80MA);
    LL_PWR_SMPS_SetOutputVoltageLevel(LL_PWR_SMPS_OUTPUT_VOLTAGE_1V40);
    LL_PWR_SMPS_Enable();

    APPE_Init();
}

void api_hal_bt_dump_state(string_t buffer) {
    uint8_t HCI_Version;
    uint16_t HCI_Revision;
    uint8_t LMP_PAL_Version;
    uint16_t Manufacturer_Name;
    uint16_t LMP_PAL_Subversion;

    tBleStatus ret = hci_read_local_version_information(
        &HCI_Version, &HCI_Revision, &LMP_PAL_Version, &Manufacturer_Name, &LMP_PAL_Subversion
    );

    string_cat_printf(buffer,
        "Ret: %d, HCI_Version: %d, HCI_Revision: %d, LMP_PAL_Version: %d, Manufacturer_Name: %d, LMP_PAL_Subversion: %d",
        ret, HCI_Version, HCI_Revision, LMP_PAL_Version, Manufacturer_Name, LMP_PAL_Subversion
    );
}
