#ifndef SCENE_HEADERS
#define SCENE_HEADERS

#include "flipper.h"
#include "scene_main_menu.h"
#include "scene_add_code.h"
#include "scene_read_code.h"
#include "scene_save_code.h"
#include "scene_select_code.h"
#include "scene_send_code.h"
#include "scene_serial.h"

/** The current scene */
typedef enum { FcomMainMenuScene, 
               FcomReadCodeScene,
               FcomCodeSelectScene,
               FcomSendCodeScene,
               FcomAddCodeScene,
               FcomSerialScene,
               FcomSaveCodeScene,
               FcomSceneCount } FcomScene;

/** The current view */
typedef enum { FcomMainMenuView,
               FcomReadCodeView,
               FcomSendCodeView,
               FcomKeyboardView,
               FcomSerialView,
               FcomFileSelectView
                } FcomView;

extern void (*const fcom_scene_on_enter_handlers[])(void*);
extern bool (*const fcom_scene_on_event_handlers[])(void*, SceneManagerEvent);
extern void (*const fcom_scene_on_exit_handlers[])(void*);
extern const SceneManagerHandlers fcom_scene_manager_handlers;

bool fcom_custom_callback(void* context, uint32_t custom_event);
bool fcom_back_event_callback(void* context);

#endif