#pragma once

typedef enum { Q_Scene_Items, Q_Scene_Settings, Q_Scene_count } appScenes;

typedef enum {
    Q_ButtonMenu, // used on selected device, to show buttons/groups
    Q_Dialog, // shows errors
    Q_ActionMenu, // new UI,
    Q_Settings, // Variable Item List for settings
    Q_FileBrowser, // TODO: UNUSED!
    Q_TextInput // TODO: UNUSED
} appView;

typedef enum { Event_DeviceSelected, Event_ButtonPressed } AppCustomEvents;

extern void (*const app_on_enter_handlers[])(void*);
extern bool (*const app_on_event_handlers[])(void*, SceneManagerEvent);
extern void (*const app_on_exit_handlers[])(void*);
extern const SceneManagerHandlers app_scene_handlers;

extern bool app_scene_custom_callback(void* context, uint32_t custom_event_id);
extern bool app_back_event_callback(void* context);
