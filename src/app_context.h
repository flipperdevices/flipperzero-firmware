#ifndef _APP_CONTEXT_H_

#define _APP_CONTEXT_H_

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include "utils/linked_list.h"

typedef enum {
    MENU,
    POPUP,
} ViewType;

struct View_t {
    ViewType type;
    int viewId;
    void* viewData;
};

typedef enum {
    APP_CONTEXT_OK = 0,
    APP_CONTEXT_CANT_ALLOCATE = -1,
    APP_CONTEXT_CANT_FREE_VIEWS = -2,
    APP_CONTEXT_UNKNOWN_ERROR = -3,
} AppContextStatus;

struct AppContext_t {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    struct ListNode_t* activeViews;
};

/// @brief Creates an app context with the desired scene handlers.
/// @param context The app context to populate. Will be passed through to the supplied scene handlers.
/// @param sceneManagerHandlers The scene handlers to use for each scene in your app.
/// @return Returns APP_CONTEXT_OK on success, APP_CONTEXT_CANT_ALLOCATE if there is an error.
AppContextStatus initializeAppContext(
    struct AppContext_t** context,
    const SceneManagerHandlers* sceneManagerHandlers);

/// @brief Frees and removes all views attached to the app context.
/// @param context The app context to remove all of the views from.
/// @return Returns APP_CONTEXT_OK on success. Should not error.
AppContextStatus freeAppContextViews(struct AppContext_t** context);

/// @brief Frees the app context entirely, cleaning it up from usage.
/// @param context The app context to clean up.
/// @return Returns APP_CONTEXT_OK on success. Should not error.
AppContextStatus freeAppContext(struct AppContext_t** context);

#endif
