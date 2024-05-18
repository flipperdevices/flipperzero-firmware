#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <malveke_gb_photo_icons.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/dialog_ex.h>
#include <storage/storage.h>
#include "scenes/boilerplate_scene.h"
#include "views/boilerplate_startscreen.h"
#include "views/boilerplate_scene_1.h"
#include "views/boilerplate_scene_2.h"

#include <string.h>

#define TAG "GBPHOTO"

#define MALVEKE_APP_RAM_EXTENSION ".sav"

#define MALVEKE_APP_FOLDER_USER "apps_data/malveke"
#define MALVEKE_APP_FOLDER EXT_PATH(MALVEKE_APP_FOLDER_USER)
#define MALVEKE_APP_FOLDER_RAMS MALVEKE_APP_FOLDER "/rams"
#define MALVEKE_APP_FOLDER_PHOTOS MALVEKE_APP_FOLDER "/photos"

#define GB_FIRST_PHOTO_OFFSET 0x2000
#define GB_PHOTO_SIZE 0x1000

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    Storage* storage;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    BoilerplateStartscreen* boilerplate_startscreen;
    BoilerplateScene1* boilerplate_scene_1;
    BoilerplateScene2* boilerplate_scene_2;
    DialogsApp* dialogs; // File Browser
    FuriString* file_path; // File Browser
    uint32_t palette;
    uint32_t info;
    File* camera_ram_sav;
    int page;
    int pos_x;
    int pos_y;
    bool show_instructions;
    uint8_t tile_data[16];

    unsigned long palette_color_hex_a;
    unsigned long palette_color_hex_b;
    unsigned long palette_color_hex_c;
    unsigned long palette_color_hex_d;

} Boilerplate;

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

typedef enum {
    BoilerplateViewIdStartscreen,
    BoilerplateViewIdMenu,
    BoilerplateViewIdScene1,
    BoilerplateViewIdScene2,
    BoilerplateViewIdSettings,
} BoilerplateViewId;

typedef enum {
    BoilerplatePaletteBlackAndWhite,
    BoilerplatePaletteOriginal,
    BoilerplatePaletteSplashUp,
    BoilerplatePaletteGBLight,
    BoilerplatePalettePocket,
    BoilerplatePaletteAudiQuattroPikesPeak,
    BoilerplatePaletteAzureClouds,
    BoilerplatePaletteTheresalwaysmoney,
    BoilerplatePaletteBGBEmulator,
    BoilerplatePaletteGameBoyBlackZeropalette,
    BoilerplatePaletteCandyCottonTowerRaid,
    BoilerplatePaletteCaramelFudgeParanoia,
    BoilerplatePaletteCGAPaletteCrush1,
    BoilerplatePaletteCGAPaletteCrush2,
    BoilerplatePaletteChildhoodinGreenland,
    BoilerplatePaletteCMYKeystone,
    BoilerplatePaletteCyanideBlues,
    BoilerplatePaletteDune2000remastered,
    BoilerplatePaletteDrowningatnight,
    BoilerplatePaletteDeepHazeGreen,
    BoilerplatePaletteDiesistmeineWassermelone,
    BoilerplatePaletteFlowerfeldstrabe,
    BoilerplatePaletteFloydSteinberginLove,
    BoilerplatePaletteGameBoyColorSplashDown,
    BoilerplatePaletteGameBoyColorSplashDownA,
    BoilerplatePaletteGameBoyColorSplashDownB,
    BoilerplatePaletteGameBoyColorSplashRightAGameBoyCamera,
    BoilerplatePaletteGameBoyColorSplashLeft,
    BoilerplatePaletteGameBoyColorSplashLeftA,
    BoilerplatePaletteGameBoyColorSplashLeftB,
    BoilerplatePaletteGameBoyColorSplashRight,
    BoilerplatePaletteGameBoyColorSplashRightB,
    BoilerplatePaletteGameBoyColorSplashUpA,
    BoilerplatePaletteGameBoyColorSplashUpB,
    BoilerplatePaletteGoldenElephantCurry,
    BoilerplatePaletteGlowingMountains,
    BoilerplatePaletteGrafixkidGray,
    BoilerplatePaletteGrafixkidGreen,
    BoilerplatePaletteArtisticCaffeinatedLactose,
    BoilerplatePaletteKneeDeepintheWood,
    BoilerplatePaletteLinkslateAwakening,
    BoilerplatePaletteMetroidAranremixed,
    BoilerplatePaletteNortoriousComandante,
    BoilerplatePalettePurpleRain,
    BoilerplatePaletteRustedCitySign,
    BoilerplatePaletteRomerosGarden,
    BoilerplatePaletteSunflowerHolidays,
    BoilerplatePaletteSuperHyperMegaGameboy,
    BoilerplatePaletteSpaceHazeOverload,
    BoilerplatePaletteStarlitMemories,
    BoilerplatePaletteMyFriendfromBavaria,
    BoilerplatePaletteThedeathofYungColumbus,
    BoilerplatePaletteTramontoalParcodegliAcquedotti,
    BoilerplatePaletteThestarryknight,
    BoilerplatePaletteVirtualBoy1985,
    BoilerplatePaletteWaterfrontPlaza,
    BoilerplatePaletteYouthIkarusreloaded
} BoilerplatePaletteState;

typedef enum {
    BoilerplateInfoOff,
    BoilerplateInfoOn,
} BoilerplateInfoState;
