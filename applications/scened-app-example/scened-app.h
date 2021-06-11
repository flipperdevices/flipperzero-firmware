#pragma once
#include <furi.h>
#include <api-hal.h>
#include "generic-scene.hpp"
#include "scene-controller.hpp"

class ScenedApp {
public:
    enum class EventType : uint8_t {
        GENERIC_EVENT_ENUM_VALUES,
    };

    enum class SceneType : uint8_t {
        GENERIC_SCENE_ENUM_VALUES,
    };

    class Event {
    public:
        union {
            int32_t menu_index;
            //DialogExResult dialog_ex_result;
        } payload;

        EventType type;
    };

    SceneController<GenericScene<ScenedApp>, ScenedApp> scene_controller;

    ~ScenedApp();
    ScenedApp();

    void run();
};