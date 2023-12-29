#ifndef _APP_CONTEXT_H_

#define _APP_CONTEXT_H_

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include "utils/linked_list.h"

typedef enum {
    MENU,
    SUBMENU,
    VIEW,
    VARIABLE_ITEM_LIST,
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
    APP_CONTEXT_NOT_ENOUGH_VIEWS = -3,
    APP_CONTEXT_UNKNOWN_ERROR = -4,
} AppContextStatus;

struct AppContext_t {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    struct View_t** activeViews;
    int activeViewsCount;
    void* additionalData;
};

/// @brief Creates an app context with the desired scene handlers.
/// @param context The app context to populate. Will be passed through to the supplied scene handlers.
/// @param viewsCount The number of views that to be added to this scene.
/// @param sceneManagerHandlers The scene handlers to use for each scene in your app.
/// @return Returns APP_CONTEXT_OK on success, APP_CONTEXT_CANT_ALLOCATE if there is an error.
AppContextStatus initializeAppContext(
    struct AppContext_t** context,
    int viewsCount,
    const SceneManagerHandlers* sceneManagerHandlers);

/// @brief Adds a view to the given app context.
/// @param context The app context to add the view to.
/// @param view The view to add to the app context.
/// @return Returns APP_CONTEXT_OK on success, APP_CONTEXT_NOT_ENOUGH_VIEWS if the ID of
//  the view exceeds the number of available views in the app context.
AppContextStatus addViewToAppContext(struct AppContext_t** context, struct View_t* view);

/// @brief Frees the app context entirely, cleaning it up from usage.
/// @param context The app context to clean up.
/// @return Returns APP_CONTEXT_OK on success. Should not error.
AppContextStatus freeAppContext(struct AppContext_t** context);

#endif
