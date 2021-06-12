#pragma once
#include <furi.h>
#include <api-hal.h>
#include "generic-scene.hpp"
#include "scene-controller.hpp"
#include "view-controller.hpp"
#include "text-store.h"

#include "view-modules/submenu-m.h"

class ScenedApp {
public:
    enum class EventType : uint8_t {
        GENERIC_EVENT_ENUM_VALUES,
        MenuSelected,
    };

    enum class SceneType : uint8_t {
        GENERIC_SCENE_ENUM_VALUES,
    };

    class Event {
    public:
        union {
            int32_t menu_index;
        } payload;

        EventType type;
    };

    SceneController<GenericScene<ScenedApp>, ScenedApp> scene_controller;
    TextStore text_store;
    ViewController<ScenedApp, SubmenuM> view_controller;

    ~ScenedApp();
    ScenedApp();

    void run();
};