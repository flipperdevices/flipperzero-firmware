#pragma once

#include <furi.h>

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>

#include "mfc_editor_app.h"
#include "scenes/mfc_editor_scene.h"

struct MfcEditorApp {
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Gui* gui;

    NfcDevice* nfc_device;
};
