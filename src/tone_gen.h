#ifndef _TONE_GEN_H_

#define _TONE_GEN_H_

#define TAG "tone-gen"

#include <furi.h>
#include <music_worker/music_worker.h>

// ids for all scenes used by the app
typedef enum { ToneGenAppScene_MainMenu, ToneGenAppScene_count } ToneGenAppScene;

// ids for the 2 types of view used by the app
typedef enum { ToneGenAppView_Menu, ToneGenAppView_Popup } ToneGenAppView;

#endif
