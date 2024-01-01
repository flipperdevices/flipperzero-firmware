#include <furi_hal.h>

#include "playback_scene.h"
#include "../app_context.h"
#include "../tone_gen.h"
#include "../system/audio_helper.h"

#define SINE_WAVE(x, toneModelData) \
    (sin((x + toneDataModel->animationOffset) * 50) * 20 + (64 / 2))

#define SQUARE_WAVE(x, toneModelData) \
    ((sin((x + toneDataModel->animationOffset) * 50) > 0 ? 1 : -1) * 20 + (64 / 2))

// Renders the waveform
static void playback_view_draw_callback(Canvas* canvas, void* model) {
    UNUSED(model);

    struct ToneData_t* toneDataModel = (struct ToneData_t*)model;
    for(int x = 1; x < 128; x++) {
        int x1 = x - 1;
        int x2 = x;
        int y1 = 0;
        int y2 = 0;
        switch(toneDataModel->waveType) {
        case SINE:
            y1 = SINE_WAVE(x1, toneDataModel);
            y2 = SINE_WAVE(x2, toneDataModel);
            break;
        case SQUARE:
            y1 = SQUARE_WAVE(x1, toneDataModel);
            y2 = SQUARE_WAVE(x2, toneDataModel);
            break;
        default:
            y1 = 64 / 2;
            y2 = 64 / 2;
            break;
        }
        // Draw lines to connect the pieces of the wave.
        canvas_draw_line(canvas, x1, y1, x2, y2);
    }
    if(toneDataModel->animationOffset < 128) {
        toneDataModel->animationOffset += 2;
    } else {
        toneDataModel->animationOffset = 0;
    }
}

// Sets up the waveform to be displayed
void scene_on_enter_playback_scene(void* context) {
    FURI_LOG_I(TAG, "scene_on_enter_playback_scene");
    struct AppContext_t* app = (struct AppContext_t*)context;
    struct View_t* playbackView = app->activeViews[ToneGenAppView_PlaybackView];

    // Configure the custom view
    view_set_draw_callback(playbackView->viewData, playback_view_draw_callback);
    view_set_context(playbackView->viewData, context);

    FURI_LOG_I(TAG, "setting view model");
    struct ToneData_t* toneDataModel = (struct ToneData_t*)view_get_model(playbackView->viewData);
    toneDataModel->waveType = ((struct ToneData_t*)app->additionalData)->waveType;
    toneDataModel->frequency = ((struct ToneData_t*)app->additionalData)->frequency;
    toneDataModel->volume = ((struct ToneData_t*)app->additionalData)->volume;

    // Set the currently active view
    FURI_LOG_I(TAG, "setting active view");
    view_dispatcher_switch_to_view(app->view_dispatcher, ToneGenAppView_PlaybackView);

    if(initializeSpeaker()) {
        FURI_LOG_I(TAG, "Starting sound");
        startSound(toneDataModel);
    }
}

// Not actively used in this instance.
bool scene_on_event_playback_scene(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "scene_on_event_playback_scene");
    UNUSED(context);
    UNUSED(event);
    return false;
}

// Not actively used in this instance.
void scene_on_exit_playback_scene(void* context) {
    FURI_LOG_I(TAG, "scene_on_exit_playback_scene");
    UNUSED(context);
    stopSound();
    deinitializeSpeaker();
}
