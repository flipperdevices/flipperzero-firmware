#include "dolphin_deed.h"
#include <furi.h>

static const DolphinDeedWeight dolphin_deed_weights[] = {
    {5, DolphinAppSubGhz}, // DolphinDeedSubGhzReceiverInfo
    {15, DolphinAppSubGhz}, // DolphinDeedSubGhzSave
    {5, DolphinAppSubGhz}, // DolphinDeedSubGhzRawRec
    {10, DolphinAppSubGhz}, // DolphinDeedSubGhzAddManually
    {10, DolphinAppSubGhz}, // DolphinDeedSubGhzSend
    {5, DolphinAppSubGhz}, // DolphinDeedSubGhzFrequencyAnalyzer

    {5, DolphinAppRfid}, // DolphinDeedRfidRead
    {15, DolphinAppRfid}, // DolphinDeedRfidReadSuccess
    {15, DolphinAppRfid}, // DolphinDeedRfidSave
    {10, DolphinAppRfid}, // DolphinDeedRfidEmulate
    {10, DolphinAppRfid}, // DolphinDeedRfidAdd

    {5, DolphinAppNfc}, // DolphinDeedNfcRead
    {15, DolphinAppNfc}, // DolphinDeedNfcReadSuccess
    {5, DolphinAppNfc}, // DolphinDeedNfcSave
    {5, DolphinAppNfc}, // DolphinDeedNfcDetectReader
    {10, DolphinAppNfc}, // DolphinDeedNfcEmulate
    {10, DolphinAppNfc}, // DolphinDeedNfcMfcAdd
    {5, DolphinAppNfc}, // DolphinDeedNfcAddSave
    {5, DolphinAppNfc}, // DolphinDeedNfcAddEmulate

    {5, DolphinAppIr}, // DolphinDeedIrSend
    {15, DolphinAppIr}, // DolphinDeedIrLearnSuccess
    {15, DolphinAppIr}, // DolphinDeedIrSave

    {5, DolphinAppIbutton}, // DolphinDeedIbuttonRead
    {15, DolphinAppIbutton}, // DolphinDeedIbuttonReadSuccess
    {15, DolphinAppIbutton}, // DolphinDeedIbuttonSave
    {10, DolphinAppIbutton}, // DolphinDeedIbuttonEmulate
    {10, DolphinAppIbutton}, // DolphinDeedIbuttonAdd

    {15, DolphinAppBadusb}, // DolphinDeedBadUsbPlayScript
    {15, DolphinAppPlugin}, // DolphinDeedU2fAuthorized

    {5, DolphinAppPlugin}, // DolphinDeedGpioUartBridge

    {5, DolphinAppPlugin}, // DolphinDeedPluginStart
    {5, DolphinAppPlugin}, // DolphinDeedPluginGameStart
    {20, DolphinAppPlugin}, // DolphinDeedPluginGameWin
};

static uint8_t dolphin_deed_limits[] = {
    100, // DolphinAppSubGhz
    100, // DolphinAppRfid
    100, // DolphinAppNfc
    100, // DolphinAppIr
    100, // DolphinAppIbutton
    100, // DolphinAppBadusb
    // 100, // DolphinAppU2f
    // 100, // DolphinAppGpio
    200, // DolphinAppPlugin
};

_Static_assert(COUNT_OF(dolphin_deed_weights) == DolphinDeedMAX, "dolphin_deed_weights size error");
_Static_assert(COUNT_OF(dolphin_deed_limits) == DolphinAppMAX, "dolphin_deed_limits size error");

uint8_t dolphin_deed_get_weight(DolphinDeed deed) {
    furi_check(deed < DolphinDeedMAX);
    return dolphin_deed_weights[deed].icounter;
}

DolphinApp dolphin_deed_get_app(DolphinDeed deed) {
    furi_check(deed < DolphinDeedMAX);
    return dolphin_deed_weights[deed].app;
}

uint8_t dolphin_deed_get_app_limit(DolphinApp app) {
    furi_check(app < DolphinAppMAX);
    return dolphin_deed_limits[app];
}
