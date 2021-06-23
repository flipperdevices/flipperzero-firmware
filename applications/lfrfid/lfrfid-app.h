#pragma once
#include <furi.h>
#include <api-hal.h>

#include <generic-scene.hpp>
#include <scene-controller.hpp>
#include <view-controller.hpp>

#include <view-modules/submenu-vm.h>

class LfRfidApp {
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

    SceneController<GenericScene<LfRfidApp>, LfRfidApp> scene_controller;
    ViewController<LfRfidApp, SubmenuVM> view_controller;

    ~LfRfidApp();
    LfRfidApp();

    void run();
};