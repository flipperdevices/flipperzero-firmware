#pragma once
#include <furi.h>
#include <furi_hal.h>

#include <generic_scene.hpp>
#include <scene_controller.hpp>
#include <view_controller.hpp>

#include <view_modules/submenu_vm.h>
#include "view_modules/rfid_view_lftune_vm.h"

#include "../lfrfid/helpers/rfid_worker.h"
#include <notification/notification.h>

class RfidDebugApp {
public:
    enum class EventType : uint8_t {
        GENERIC_EVENT_ENUM_VALUES,
        MenuSelected,
    };

    enum class SceneType : uint8_t {
        GENERIC_SCENE_ENUM_VALUES,
        LFTuneScene,
        HFFieldScene,
    };

    class Event {
    public:
        union {
            int32_t menu_index;
        } payload;

        EventType type;
    };

    bool HF_field_enabled;
    bool LF_field_enabled;

    RfidWorker worker;
    NotificationApp* notification;

    SceneController<GenericScene<RfidDebugApp>, RfidDebugApp> scene_controller;
    ViewController<RfidDebugApp, SubmenuVM, RfidViewLFTuneVM> view_controller;

    ~RfidDebugApp();
    RfidDebugApp();

    void run();
};
