#ifndef _TONE_GEN_H_

#define _TONE_GEN_H_

#define TAG "tone-gen"

#include <furi.h>
#include <music_worker/music_worker.h>

// ids for all scenes used by the app
typedef enum {
    ToneGenAppScene_Starting,
    ToneGenAppScene_Playback,
    ToneGenAppScene_Settings,
    ToneGenAppScene_count
} ToneGenAppScene;

// ids for the 2 types of view used by the app
typedef enum {
    ToneGenAppView_SharedMenu,
    ToneGenAppView_Submenu,
    ToneGenAppView_PlaybackView,
    ToneGenAppView_Popup,
    ToneGenAppView_count
} ToneGenAppView;

typedef enum { SQUARE, SINE } ToneWaveType;

struct ToneData_t {
    int animationOffset;
    int amplitude;
    int period;
    ToneWaveType waveType;
};

#endif
