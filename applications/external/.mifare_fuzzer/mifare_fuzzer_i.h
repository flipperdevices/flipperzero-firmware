#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

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

#define MIFARE_FUZZER_APP_FOLDER EXT_PATH("apps_data/mifare_fuzzer")
#define MIFARE_FUZZER_FILE_EXT ".txt"

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
    FuriHalNfcDevData nfc_dev_data;
    FuriString* app_folder;
    FuriString* file_path;
    FuriString* uid_str;
    Stream* uids_stream;
};
