#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/modules/validators.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <assets_icons.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/popup.h>
#include "scenes/nfc_maker_scene.h"
#include <lib/flipper_format/flipper_format.h>
#include <toolbox/name_generator.h>
#include <applications/main/nfc/nfc_app_i.h>
#include <furi_hal_bt.h>

#define MAC_INPUT_LEN GAP_MAC_ADDR_SIZE
#define MAIL_INPUT_LEN 128
#define PHONE_INPUT_LEN 17

#define BIG_INPUT_LEN 248
#define SMALL_INPUT_LEN 90

#define NTAG_DATA_AREA_UNIT_SIZE 2 * MF_ULTRALIGHT_PAGE_SIZE
typedef enum {
    Ntag203,
    Ntag213,
    Ntag215,
    Ntag216,
    NtagI2C1K,
    NtagI2C2K,
    NtagMAX,
} Ntag;
extern const NfcDataGeneratorType ntag_generators[NtagMAX];
extern const char* ntag_names[NtagMAX];
extern const size_t ntag_sizes[NtagMAX];
#define MAX_NDEF_LEN ntag_sizes[NtagI2C2K]

typedef enum {
    WifiAuthenticationOpen = 0x01,
    WifiAuthenticationWpa2Personal = 0x20,
    WifiAuthenticationWpa2Enterprise = 0x10,
    WifiAuthenticationWpaPersonal = 0x02,
    WifiAuthenticationWpaEnterprise = 0x08,
    WifiAuthenticationShared = 0x04,
} WifiAuthentication;

typedef enum {
    WifiEncryptionAes = 0x08,
    WifiEncryptionWep = 0x02,
    WifiEncryptionTkip = 0x04,
    WifiEncryptionNone = 0x01,
} WifiEncryption;

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    TextInput* text_input;
    ByteInput* byte_input;
    Popup* popup;

    NfcDevice* nfc_device;
    uint8_t* ndef_buffer;

    uint8_t mac_buf[MAC_INPUT_LEN];
    char mail_buf[MAIL_INPUT_LEN];
    char phone_buf[PHONE_INPUT_LEN];

    char big_buf[BIG_INPUT_LEN];
    char small_buf1[SMALL_INPUT_LEN];
    char small_buf2[SMALL_INPUT_LEN];
    char save_buf[BIG_INPUT_LEN];
} NfcMaker;

typedef enum {
    NfcMakerViewSubmenu,
    NfcMakerViewTextInput,
    NfcMakerViewByteInput,
    NfcMakerViewPopup,
} NfcMakerView;
