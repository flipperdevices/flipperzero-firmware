#include "genie_submenu.h"

#include <gui/view_dispatcher.h>
#include "genie_app.h"

#include "genie_config.h"

typedef struct GenieApp GenieApp;

struct GenieSubmenu {
    Submenu* submenu;
};

typedef enum {
    GenieSubmenuIndexSend,
    GenieSubmenuIndexConfig,
    GenieSubmenuIndexLearn,
    GenieSubmenuIndexAbout,
} GenieSubmenuIndex;

typedef enum {
    GenieViewSubmenu,
    GenieViewSend,
    GenieViewConfig,
    GenieViewLearn,
    GenieViewAbout,
} GenieView;

/**
 * @brief      Callback for navigation events
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context
 * @return     next view id
*/
uint32_t genie_navigation_submenu_callback(void* context) {
    UNUSED(context);
    return GenieViewSubmenu;
}

/**
 * @brief      Callback for navigation events
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      context  The context
 * @return     next view id
*/
static uint32_t genie_navigation_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void genie_submenu_callback(void* context, uint32_t index) {
    GenieApp* app = (GenieApp*)context;
    ViewDispatcher* view_dispatcher = genie_app_get_view_dispatcher(app);
    switch(index) {
    case GenieSubmenuIndexSend:
        view_dispatcher_switch_to_view(view_dispatcher, GenieViewSend);
        break;
    case GenieSubmenuIndexConfig: {
        view_dispatcher_switch_to_view(view_dispatcher, GenieViewConfig);
        break;
    }
    case GenieSubmenuIndexLearn:
        view_dispatcher_switch_to_view(view_dispatcher, GenieViewLearn);
        break;
    case GenieSubmenuIndexAbout:
        view_dispatcher_switch_to_view(view_dispatcher, GenieViewAbout);
        break;
    default:
        break;
    }
}

GenieSubmenu* genie_submenu_alloc(GenieApp* app) {
    GenieSubmenu* genie_submenu = malloc(sizeof(GenieSubmenu));
    Submenu* submenu = submenu_alloc();
    genie_submenu->submenu = submenu;
    submenu_add_item(submenu, "Send", GenieSubmenuIndexSend, genie_submenu_callback, app);
    submenu_add_item(submenu, "Config", GenieSubmenuIndexConfig, genie_submenu_callback, app);
    submenu_add_item(submenu, "Learn", GenieSubmenuIndexLearn, genie_submenu_callback, app);
    submenu_add_item(submenu, "About", GenieSubmenuIndexAbout, genie_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(submenu), genie_navigation_exit_callback);
    return genie_submenu;
}

View* genie_submenu_get_view(GenieSubmenu* genie_submenu) {
    furi_assert(genie_submenu);
    furi_assert(genie_submenu->submenu);
    return submenu_get_view(genie_submenu->submenu);
}

void genie_submenu_free(GenieSubmenu* genie_submenu) {
    submenu_free(genie_submenu->submenu);
    genie_submenu->submenu = NULL;
    free(genie_submenu);
}