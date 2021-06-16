#pragma once
#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ButtonPanel anonymous structure */
typedef struct ButtonPanel ButtonPanel;
typedef void (*ButtonItemCallback)(void* context, uint32_t index);
typedef void (*ButtonPanelDrawCallback)(Canvas* canvas, void* _model);
typedef bool (*ButtonPanelInputCallback)(InputEvent* event, void* context);

ButtonPanel* button_panel_alloc(size_t reserve_x, size_t reserve_y);
void button_panel_free(ButtonPanel* button_panel);
void button_panel_clean(ButtonPanel* button_panel);
void button_panel_add_item(
    ButtonPanel* button_panel,
    uint32_t index,
    uint16_t matrix_place_x,
    uint16_t matrix_place_y,
    uint16_t x,
    uint16_t y,
    IconName icon_name,
    IconName icon_name_selected,
    ButtonItemCallback callback,
    void* callback_context);

View* button_panel_get_view(ButtonPanel* button_panel);

void button_panel_add_label(ButtonPanel* button_panel,
                            uint16_t x,
                            uint16_t y,
                            Font font,
                            const char* label_str);

void button_panel_set_draw_callback(ButtonPanel* button_panel, ButtonPanelDrawCallback callback, void* context);
void button_panel_set_input_callback(ButtonPanel* button_panel, ButtonPanelInputCallback callback, void* context);

#ifdef __cplusplus
}
#endif
