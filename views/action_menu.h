#pragma once

#include <stdint.h>
#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

/** ActionMenu anonymous structure */
typedef struct ActionMenu ActionMenu;

/** ActionMenuItem anonymous structure */
typedef struct ActionMenuItem ActionMenuItem;

/** Callback for any button menu actions */
typedef void (*ActionMenuItemCallback)(void* context, int32_t index, InputType type);

/** Type of UI element */
typedef enum {
    ActionMenuItemTypeSubGHz,
    ActionMenuItemTypeRFID,
    ActionMenuItemTypeIR,
    ActionMenuItemTypeNFC,
    ActionMenuItemTypePlaylist,
    ActionMenuItemTypeGroup,
    ActionMenuItemTypeSettings,
    ActionMenuItemTypeUnknown,
    ActionMenuItemType_count
} ActionMenuItemType;

typedef enum {
    ActionMenuLayoutPortrait,
    ActionMenuLayoutLandscape,
} ActionMenuLayout;

/** Get button menu view
 *
 * @param      action_menu  ActionMenu instance
 *
 * @return     View instance that can be used for embedding
 */
View* action_menu_get_view(ActionMenu* action_menu);

/** Clean button menu
 *
 * @param      action_menu  ActionMenu instance
 */
void action_menu_reset(ActionMenu* action_menu);

/** Set the layout
 * 
 * @param      layout       Portrait or Landscape
*/
void action_menu_set_layout(ActionMenu* menu, ActionMenuLayout layout);

/** Show/Hide icons in UI
 * 
 * @param      show_icons   Show or Hide icons
*/
void action_menu_set_show_icons(ActionMenu* menu, bool show_icons);

/** Show/Hide header labels in UI
 * 
 * @param      show_headers   Show or Hide header labels
*/
void action_menu_set_show_headers(ActionMenu* menu, bool show_headers);

/** Add item to button menu instance
 *
 * @param      action_menu       ActionMenu instance
 * @param      label             text inside new button
 * @param      icon              IconAnimation instance
 * @param      index             value to distinct between buttons inside
 *                               ActionMenuItemCallback
 * @param      callback          The callback
 * @param      type              type of button to create. Differ by button
 *                               drawing. Control buttons have no frames, and
 *                               have more squared borders.
 * @param      callback_context  The callback context
 *
 * @return     pointer to just-created item
 */
ActionMenuItem* action_menu_add_item(
    ActionMenu* action_menu,
    const char* label,
    int32_t index,
    ActionMenuItemCallback callback,
    ActionMenuItemType type,
    void* callback_context);

/** Allocate and initialize new instance of ActionMenu model
 *
 * @return     just-created ActionMenu model
 */
ActionMenu* action_menu_alloc(void);

/** Free ActionMenu element
 *
 * @param      action_menu  ActionMenu instance
 */
void action_menu_free(ActionMenu* action_menu);

/** Set ActionMenu header on top of canvas
 *
 * @param      action_menu  ActionMenu instance
 * @param      header       header on the top of button menu
 */
void action_menu_set_header(ActionMenu* action_menu, const char* header);

/** Set selected item
 *
 * @param      action_menu  ActionMenu instance
 * @param      index        index of ActionMenu to be selected
 */
void action_menu_set_selected_item(ActionMenu* action_menu, uint32_t index);

#ifdef __cplusplus
}
#endif