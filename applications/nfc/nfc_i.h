#pragma once

#include "nfc.h"
#include "nfc_types.h"
#include "nfc_worker.h"

#include <furi.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <cli/cli.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>

#include "views/nfc_detect.h"
#include "views/nfc_emulate.h"
#include "views/nfc_emv.h"
#include "views/nfc_mifare_ul.h"

#include "scenes/nfc_scene_start.h"
#include "scenes/nfc_scene_read_card.h"
#include "scenes/nfc_scene_read_card_success.h"
#include "scenes/nfc_scene_card_menu.h"
#include "scenes/nfc_scene_not_implemented.h"

struct Nfc {
    NfcCommon nfc_common;
    Gui* gui;

    // Nfc Views
    NfcDetect* nfc_detect;
    NfcEmulate* nfc_emulate;
    NfcEmv* nfc_emv;
    NfcMifareUl* nfc_mifare_ul;

    // Common Views
    Submenu* submenu;
    DialogEx* dialog_ex;
    Popup* popup;
    TextInput* text_input;
    ByteInput* byte_input;

    // Scenes
    AppScene* scene_start;
    AppScene* scene_read_card;
    AppScene* scene_read_card_success;
    AppScene* scene_card_menu;
    AppScene* scene_not_implemented;
};

typedef enum {
    NfcViewMenu,
    NfcViewDialogEx,
    NfcViewPopup,
    NfcViewTextInput,
    NfcViewByteInput,
    NfcViewDetect,
    NfcViewEmulate,
    NfcViewEmv,
    NfcViewMifareUl,
} NfcView;

typedef enum {
    NfcSceneStart,
    NfcSceneReadCard,
    NfcSceneReadCardSuccess,
    NfcSceneCardMenu,
    NfcSceneNotImplemented,
} NfcScene;

Nfc* nfc_alloc();

int32_t nfc_task(void* p);
