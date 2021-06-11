#pragma once
#include <furi.h>
#include <api-hal.h>
#include "generic-scene.hpp"
#include "scene-controller.hpp"

class ScenedApp {
public:
    enum class Event : uint8_t {
        GENERIC_EVENT_ENUM_VALUES,
    };

    enum class Scene : uint8_t {
        GENERIC_SCENE_ENUM_VALUES,
    };

    class EventType {
    public:
        union {
            int32_t menu_index;
            //DialogExResult dialog_ex_result;
        } payload;

        Event type;
    };

    SceneController<GenericScene<ScenedApp>, ScenedApp> scene_controller;

    ~ScenedApp();
    ScenedApp();

    void run();
};