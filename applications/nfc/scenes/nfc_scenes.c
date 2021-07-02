#include "nfc_scene.h"

#define ADD_SCENE(prefix, name) prefix##_scene_##name##_on_enter,
void (*const on_enter_handlers[NfcSceneNum])(void* context) = {
    #include "nfc_scene_config.h"
};
#undef ADD_SCENE

#define ADD_SCENE(prefix, name) prefix##_scene_##name##_on_event,
bool (*const on_event_handlers[NfcSceneNum])(void* context) = {
    #include "nfc_scene_config.h"
};
#undef ADD_SCENE

#define ADD_SCENE(prefix, name) prefix##_scene_##name##_on_exit,
void (*const on_exit_handlers[NfcSceneNum])(void* context) = {
    #include "nfc_scene_config.h"
};
#undef ADD_SCENE
