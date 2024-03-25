#pragma once
#include <furi.h>
#include <furi_hal_light.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <power/power_service/power.h>
#include "lib/sdq/sdq_device.c"

typedef enum { EventTypeKey } EventType;

typedef enum {
    YuriCableProMaxMainMenuScene,
    YuriCableProMaxDCSDScene,
    YuriCableProMaxResetScene,
    YuriCableProMaxDFUScene,
    YuriCableProMaxCharginScene,
    YuriCableProMaxSceneCount
} YuriCableProMaxScene;

typedef enum {
    YuriCableProMaxSubmenuView,
    YuriCableProMaxWidgetView,
} YuriCableProMaxView;

typedef enum {
    YuriCableProMaxMainMenuSceneDCSD,
    YuriCableProMaxMainMenuSceneReset,
    YuriCableProMaxMainMenuSceneDFU,
    YuriCableProMaxMainMenuSceneCharging,
} YuriCableProMaxMainMenuSceneIndex;

typedef enum {
    YuriCableProMaxMainMenuTitle,
    YuriCableProMaxDCSDSubmenuTitle,
    YuriCableProMaxResetSubmenuTitle,
    YuriCableProMaxDFUSubmenuTitle,
    YuriCableProMaxChargingSubmenuTitle,
    YuriCableProMaxSubmenuTitlesCount
} YuriCableProMaxSubmenuTitles;

const char* YuriCableProMaxSubmenuTitlesStrings[] = {
    "YuriCable Pro Max",
    "DCSD - Bootlog",
    "Force Reset",
    "Force DFU",
    "5V Charging",
};
typedef struct {
    SDQDevice* sdq;
    IconAnimation* listeningAnimation;
    YuriCableProMaxSubmenuTitles selectedSubmenu;
    bool ledMainMenu;
    bool ledSequenceCommandExecutedPlayed;
} YuriCableData;

typedef struct App {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    FuriMessageQueue* queue;
    FuriMutex* mutex;
    YuriCableData* data;
    FuriThread* led_thread;
    FuriThread* battery_info_update_thread;
    Power* power;
    PowerInfo info;
} App;

typedef enum {
    YuriCableProMaxMainMenuSceneDCSDModeEvent,
    YuriCableProMaxMainMenuSceneResetModeEvent,
    YuriCableProMaxMainMenuSceneDFUModeEvent,
    YuriCableProMaxMainMenuSceneChargingModeEvent
} YuriCableProMaxMainMenuSceneEvent;

typedef struct {
    EventType type;
    InputEvent input;
} Event;

typedef enum {
    LedEvtStop = (1 << 0),
    LedEvtStart = (1 << 1),
} LedEvtFlags;
