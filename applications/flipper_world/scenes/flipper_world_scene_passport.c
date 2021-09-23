#include <furi-hal-version.h>
#include <math.h>

#include "../flipper_world_i.h"
#include "../views/flipper_world_passport.h"

#include "dolphin/dolphin.h"
#include "dolphin/helpers/dolphin_state.h"

#define MAIN_VIEW_DEFAULT (0UL)

void flipper_world_scene_passport_callback(FlipperPassportEvent event, void* context) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    view_dispatcher_send_custom_event(flipper_world->view_dispatcher, event);
}

const void flipper_world_scene_passport_on_enter(void* context) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    FlipperPassportView* passport = flipper_world->passport;

    Dolphin* dolphin = furi_record_open("dolphin");
    DolphinState* state = dolphin_get_state(dolphin);

    with_view_model(
        passport->view, (FlipperPassportViewModel * model) {
            model->butthurt = CLAMP(dolphin_state_get_butthurt(state), BUTTHURT_MAX - 1, 0);
            model->current_level = dolphin_state_get_level(state);
            uint32_t prev_cap =
                dolphin_state_xp_to_levelup(state, model->current_level - 1, false);
            model->exp =
                (dolphin_state_xp_to_levelup(state, model->current_level, true) * 63) /
                (dolphin_state_xp_to_levelup(state, model->current_level, false) - prev_cap);
            model->portrait_level = CLAMP(floor(model->current_level / 14), MOODS_TOTAL - 1, 0);
            model->name = furi_hal_version_get_name_ptr();
            return true;
        });

    furi_record_close("dolphin");

    flipper_world_passport_set_callback(
        passport, flipper_world_scene_passport_callback, flipper_world);
    view_dispatcher_switch_to_view(flipper_world->view_dispatcher, FlipperWorldPassport);
}

const bool flipper_world_scene_passport_on_event(void* context, SceneManagerEvent event) {
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case FlipperPassportEventExit:
            scene_manager_previous_scene(flipper_world->scene_manager);
            consumed = true;
            break;

        default:
            break;
        }
    }

    return consumed;
}

const void flipper_world_scene_passport_on_exit(void* context) {
    // FlipperWorld* flipper_world = (FlipperWorld*)context;
}
