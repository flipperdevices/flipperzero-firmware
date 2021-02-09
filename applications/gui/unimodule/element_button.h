#pragma once
#include <stdint.h>

typedef struct GuiButton GuiButton;
typedef struct GuiElement GuiElement;

typedef enum {
    ButtonLeft,
    ButtonCenter,
    ButtonRight,
} GuiButtonType;

typedef void (*ButtonCallback)(void* context);

// allocate element
GuiButton* gui_button_alloc(
    GuiButtonType button_type,
    const char* text,
    ButtonCallback callback,
    void* context);

// free element
void gui_button_free(GuiButton* gui_button);

// get generic element pointer
GuiElement* gui_button_get_element(GuiButton* gui_button);

// element setters
void gui_button_set_text(GuiButton* gui_button, const char* text);