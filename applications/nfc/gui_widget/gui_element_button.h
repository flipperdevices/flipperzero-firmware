#pragma once
#include <stdint.h>

typedef struct GuiButton GuiButton;
typedef struct GuiElement GuiElement;

typedef enum {
    GuiButtonTypeLeft,
    GuiButtonTypeCenter,
    GuiButtonTypeRight,
} GuiButtonType;

typedef void (*ButtonCallback)(void* context);

/** Allocate Button Element
 * @param button_type GuiButtonType instance
 * @param text text on allocated button
 * @param callback ButtonCallback instance
 * @param context pointer to context
 */
GuiButton* gui_button_alloc(
    GuiButtonType button_type,
    const char* text,
    ButtonCallback callback,
    void* context);

/** Free Button Element
 * @param gui_button GuiButton instance
 */
void gui_button_free(GuiButton* gui_button);

/** Get generic Element pointer from GuiButton
 * @param gui_button GuiButton instance
 * @return GuiElement instance
 */
GuiElement* gui_button_get_element(GuiButton* gui_button);
