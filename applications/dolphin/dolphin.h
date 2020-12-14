#pragma once

typedef struct Dolphin Dolphin;

typedef enum {
    DolphinDeedIButtonRead,
    DolphinDeedIButtonWrite,
    DolphinDeedIButtonEmulate,

    DolphinDeedNfcRead,
    DolphinDeedNfcWrite,
    DolphinDeedNfcEmulate,

    DolphinDeedIrSend,
    DolphinDeedIrReceive,

    DolphinDeedRfidRead,
    DolphinDeedRfidWrite,
    DolphinDeedRfidEmulate,

} DolphinDeed;

void dolphin_deed(Dolphin* dolphin, DolphinDeed deed);

