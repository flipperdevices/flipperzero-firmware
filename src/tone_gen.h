#ifndef _TONE_GEN_H_

#define _TONE_GEN_H_

#define TAG "tone-gen"

#include <furi.h>

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
    ToneGenAppView_VariableItemList,
    ToneGenAppView_PlaybackView,
    ToneGenAppView_Popup,
    ToneGenAppView_count
} ToneGenAppView;

typedef enum { SINE, SQUARE } ToneWaveType;

struct ToneData_t {
    int animationOffset;
    ToneWaveType waveType;
    uint16_t frequency;
    float volume;
};

#endif
