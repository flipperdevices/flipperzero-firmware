#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <lib/nfc/nfc_device.h>
#include <lib/nfc/protocols/nfc_protocol.h>
#include "lib/nfc/protocols/mf_classic/mf_classic.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>

#include <dialogs/dialogs.h>

#include <input/input.h>

#include <toolbox/stream/stream.h>
//#include <toolbox/stream/string_stream.h>
//#include <toolbox/stream/file_stream.h>
#include <toolbox/stream/buffered_file_stream.h>

#include "mifare_fuzzer.h"

#include "scenes/mifare_fuzzer_scene.h"
#include "views/mifare_fuzzer_emulator.h"

#include "mifare_fuzzer_worker.h"

#define TAG "MifareFuzzerApp"

#define MIFARE_FUZZER_APP_FOLDER EXT_PATH("mifare_fuzzer")
#define MIFARE_FUZZER_UID_FILE_EXT ".txt"
#define MIFARE_FUZZER_CARD_FILE_EXT ".nfc"

#define MIFARE_FUZZER_TICK_PERIOD 200
#define MIFARE_FUZZER_DEFAULT_TICKS_BETWEEN_CARDS 10
#define MIFARE_FUZZER_MIN_TICKS_BETWEEN_CARDS 5
#define MIFARE_FUZZER_MAX_TICKS_BETWEEN_CARDS 50

typedef enum MifareFuzzerSceneState {
    MifareFuzzerSceneStateClassic1k,
    MifareFuzzerSceneStateClassic4k,
    MifareFuzzerSceneStateUltralight,
} MifareFuzzerSceneState;

typedef enum {
    MifareFuzzerViewSelectCard,
    MifareFuzzerViewSelectAttack,
    MifareFuzzerViewEmulator,
} MifareFuzzerView;

struct MifareFuzzerApp {
    Gui* gui;
    NotificationApp* notifications;

    ViewDispatcher* view_dispatcher;

    SceneManager* scene_manager;

    DialogsApp* dialogs;

    Storage* storage;

    // Common Views
    Submenu* submenu_card;
    Submenu* submenu_attack;

    MifareFuzzerEmulator* emulator_view;

    MifareFuzzerWorker* worker;

    MifareCard card;
    MifareFuzzerAttack attack;
    FuriString* app_folder;
    FuriString* card_file_path;
    FuriString* uid_file_path;
    FuriString* uid_str;
    Stream* uids_stream;
};
