#pragma once

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

typedef enum {
    BasicScenesMainMenuSceneLottoNumbers,
    BasicScenesMainMenuSceneGreeting,
} BasicScenesMainMenuSceneIndex;

typedef enum {
    BasicScenesSubmenuView,
    BasicScenesWidgetView,
    BasicScenesTextInputView,
} BasicScenesView;

typedef enum {
    BasicScenesMainMenuScene,
    BasicScenesLotteryScene,
    BasicScenesGreetingInputScene,
    BasicScenesGreetingMessageScene,
    BasicScenesSceneCount,
} BasicScenesScene;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    TextInput* text_input;
    char* user_name;
    uint8_t user_name_size;
} App;