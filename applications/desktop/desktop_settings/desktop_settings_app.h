#pragma once

#include <gui/gui.h>
#include <gui/modules/popup.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>

#include "desktop_settings.h"
#include "desktop/views/pin_input.h"
#include "views/pin_setup_howto.h"
#include "views/pin_setup_howto2.h"

typedef enum {
    CodeEventsSetPin,
    CodeEventsChangePin,
    CodeEventsDisablePin,
} CodeEventsEnum;

typedef enum {
    DesktopSettingsAppViewMenu,
    DesktopSettingsAppViewIdPopup,
    DesktopSettingsAppViewIdPinInput,
    DesktopSettingsAppViewIdPinSetupHowto,
    DesktopSettingsAppViewIdPinSetupHowto2,
} DesktopSettingsAppView;

typedef struct {
    DesktopSettings settings;

    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Popup* popup;

    PinCode pincode_buffer;
    bool pincode_buffer_filled;
    DesktopViewPinInput* pin_input_view;
    DesktopSettingsViewPinSetupHowto* pin_setup_howto_view;
    DesktopSettingsViewPinSetupHowto2* pin_setup_howto2_view;

    uint8_t menu_idx;

} DesktopSettingsApp;
