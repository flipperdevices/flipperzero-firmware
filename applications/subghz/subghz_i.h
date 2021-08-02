#pragma once

#include "subghz.h"
#include "views/subghz_capture.h"
#include "views/subghz_static.h"

#include "views/subghz_test_carrier.h"
#include "views/subghz_test_packet.h"

#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>

#include <subghz/scenes/subghz_scene.h>

extern const uint32_t subghz_frequencies[];
extern const uint32_t subghz_frequencies_count;
extern const uint32_t subghz_frequencies_433_92;

struct SubGhz {
    Gui* gui;

    SceneManager* scene_manager;

    ViewDispatcher* view_dispatcher;

    Submenu* submenu;

    SubghzCapture* subghz_capture;
    SubghzStatic* subghz_static;

    SubghzTestCarrier* subghz_test_carrier;
    SubghzTestPacket* subghz_test_packet;
};

typedef enum {
    SubGhzViewMenu,

    SubGhzViewCapture,
    SubGhzViewStatic,

    SubGhzViewTestCarrier,
    SubGhzViewTestPacket,
} SubGhzView;
