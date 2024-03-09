#pragma once

typedef enum { SR_Scene_Items, SR_Scene_count } appScenes;

typedef enum {
    SR_ButtonMenu, // used on selected device, to show buttons/groups
    SR_Dialog, // shows errors
    SR_FileBrowser, // TODO: UNUSED!
    SR_TextInput // TODO: UNUSED
} appView;

typedef enum { Event_DeviceSelected, Event_ButtonPressed } AppCustomEvents;

extern void (*const app_on_enter_handlers[])(void*);
extern bool (*const app_on_event_handlers[])(void*, SceneManagerEvent);
extern void (*const app_on_exit_handlers[])(void*);
extern const SceneManagerHandlers app_scene_handlers;

extern bool app_scene_custom_callback(void* context, uint32_t custom_event_id);
extern bool app_back_event_callback(void* context);
