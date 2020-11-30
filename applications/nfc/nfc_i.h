#pragma once

#include "nfc.h"

#include <flipper_v2.h>

#include <rfal_analogConfig.h>
#include <rfal_rf.h>
#include <rfal_nfc.h>
#include <rfal_nfca.h>
#include <rfal_nfcb.h>
#include <rfal_nfcf.h>
#include <rfal_nfcv.h>
#include <st25r3916.h>
#include <st25r3916_irq.h>

#include <gui/gui.h>
#include <gui/widget.h>
#include <gui/canvas.h>
#include <assets_icons.h>

#include <menu/menu.h>
#include <menu/menu_item.h>

#include "dispatcher.h"

typedef enum {
    NfcDeviceTypeNfca,
    NfcDeviceTypeNfcb,
    NfcDeviceTypeNfcf,
    NfcDeviceTypeNfcv,
    NfcDeviceTypeNfcMifare
} NfcDeviceType;

typedef struct {
    NfcDeviceType type;
    union {
        rfalNfcaListenDevice nfca;
        rfalNfcbListenDevice nfcb;
        rfalNfcfListenDevice nfcf;
        rfalNfcvListenDevice nfcv;
    };
} NfcDevice;

typedef enum {
    MessageTypeBase,
} NfcMessageType;

typedef struct {
    Message base;
    void* data;
} NfcMessage;

struct Nfc {
    Dispatcher* dispatcher;
    Icon* icon;
    Widget* widget;
    ValueMutex* menu_vm;
    MenuItem* menu;

    osThreadAttr_t worker_attr;
    osThreadId_t worker;

    uint8_t screen;
    uint8_t ret;
    uint8_t devCnt;
    rfalNfcaSensRes first_atqa;
    rfalNfcaSelRes first_sak;

    char* current;
};
