#include "button_panel.h"
#include "gui/canvas.h"
#include <m-array.h>
#include <m-i-list.h>
#include <m-list.h>
#include <furi.h>
#include <gui/elements.h>
#include <stdint.h>

typedef struct {
    // uint16_t to support multi-screen, wide button panel
    uint16_t x;
    uint16_t y;
    const char* str;
} LabelElement;

LIST_DEF(LabelList, LabelElement, M_POD_OPLIST)
#define M_OPL_LabelList_t() LIST_OPLIST(LabelList)


typedef struct {
    uint16_t x;
    uint16_t y;
    IconName name;
    IconName name_selected;
} IconElement;

typedef struct ButtonItem {
    uint32_t index;
    ButtonItemCallback callback;
    IconElement icon;
    void* callback_context;
} ButtonItem;

ARRAY_DEF(ButtonArray, ButtonItem*, M_PTR_OPLIST);
#define M_OPL_ButtonArray_t() ARRAY_OPLIST(ButtonArray, M_PTR_OPLIST)
ARRAY_DEF(ButtonMatrix, ButtonArray_t);
#define M_OPL_ButtonMatrix_t() ARRAY_OPLIST(ButtonMatrix, M_OPL_ButtonArray_t())

struct ButtonPanel {
    View* view;
};

typedef struct {
    ButtonMatrix_t button_matrix;
    LabelList_t labels;
    uint16_t reserve_x;
    uint16_t reserve_y;
    uint16_t selected_item_x;
    uint16_t selected_item_y;
} ButtonPanelModel;

static void button_panel_process_up(ButtonPanel* button_panel);
static void button_panel_process_down(ButtonPanel* button_panel);
static void button_panel_process_left(ButtonPanel* button_panel);
static void button_panel_process_right(ButtonPanel* button_panel);
static void button_panel_process_ok(ButtonPanel* button_panel);
static void button_panel_view_draw_callback(Canvas* canvas, void* _model);
static bool button_panel_view_input_callback(InputEvent* event, void* context);

ButtonPanel* button_panel_alloc(size_t reserve_x, size_t reserve_y) {
    furi_check(reserve_x > 0);
    furi_check(reserve_y > 0);
    ButtonPanel* button_panel = furi_alloc(sizeof(ButtonPanel));
    button_panel->view = view_alloc();
    view_set_orientation(button_panel->view, ViewOrientationVertical);
    view_set_context(button_panel->view, button_panel);
    view_allocate_model(button_panel->view, ViewModelTypeLocking, sizeof(ButtonPanelModel));
    view_set_draw_callback(button_panel->view, button_panel_view_draw_callback);
    view_set_input_callback(button_panel->view, button_panel_view_input_callback);

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            model->reserve_x = reserve_x;
            model->reserve_y = reserve_y;
            model->selected_item_x = 0;
            model->selected_item_y = 0;
            ButtonMatrix_init(model->button_matrix);
            ButtonMatrix_reserve(model->button_matrix, model->reserve_y);
            for (size_t i = 0; i > model->reserve_y; ++i) {
                ButtonArray_t* array = ButtonMatrix_get(model->button_matrix, i);
                ButtonArray_init(*array);
                ButtonArray_reserve(*array, reserve_x);
                // TODO: do we need to clear allocated memory of ptr-s to ButtonItem ??
            }
            LabelList_init(model->labels);
            return true;
        });

    return button_panel;
}

static ButtonItem** button_panel_get_item(ButtonPanelModel* model, size_t x, size_t y) {
    furi_check(x < model->reserve_x);
    furi_check(y < model->reserve_y);
    ButtonArray_t* button_array = ButtonMatrix_get_at(model->button_matrix, x);
    ButtonItem** button_item = ButtonArray_get_at(*button_array, y);
    return button_item;
}

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
    void* callback_context) {

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            ButtonItem** button_item_ptr = button_panel_get_item(model, matrix_place_x, matrix_place_y);
            furi_check(*button_item_ptr == NULL);
            *button_item_ptr = furi_alloc(sizeof(ButtonItem));
            ButtonItem* button_item = *button_item_ptr;
            button_item->callback = callback;
            button_item->callback_context = callback_context;
            button_item->icon.x = x;
            button_item->icon.y = y;
            button_item->icon.name = icon_name;
            button_item->icon.name_selected = icon_name_selected;
            button_item->index = index;
            return true;
        });
}


void button_panel_clean(ButtonPanel* button_panel) {
    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            for(size_t x = 0; x < model->reserve_x; ++x) {
                for(size_t y = 0; y < model->reserve_y; ++y) {
                    ButtonItem** button_item = button_panel_get_item(model, x, y);
                    free(*button_item);
                    *button_item = NULL;

                }
            }
            return true;
        });
}


View* button_panel_get_view(ButtonPanel* button_panel) {
    furi_assert(button_panel);
    return button_panel->view;
}


static void button_panel_view_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(canvas);
    furi_assert(_model);

    ButtonPanelModel* model = _model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    for(size_t x = 0; x < model->reserve_x; ++x) {
        for(size_t y = 0; y < model->reserve_y; ++y) {
            ButtonItem* button_item = *button_panel_get_item(model, x, y);
            IconName icon_name = button_item->icon.name;
            if ((model->selected_item_x == x) && (model->selected_item_y == y)) {
                icon_name = button_item->icon.name_selected;
            }
//            printf("[%d][%d] %s\r\n",
//                    x, y,
//                    icon_name == button_item->icon.name_selected ? "SELECTED" : "");
            canvas_draw_icon_name(canvas,
                                  button_item->icon.x,
                                  button_item->icon.y,
                                  icon_name);
        }
    }

    canvas_set_font(canvas, FontSecondary);

    for M_EACH(label, model->labels, LabelList_t) {
        printf("label x(%d), y(%d), str \'%s\'\r\n",
                label->x, label->y, label->str);
        canvas_draw_str(canvas, label->x, label->y, label->str);
    }




}


static void button_panel_process_down(ButtonPanel* button_panel) {

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            printf("DOWN before, selected[%d][%d]\r\n",
                    model->selected_item_x, model->selected_item_y);
            size_t new_selected_item_x = model->selected_item_x;
            size_t new_selected_item_y = model->selected_item_y;
            size_t i;

            if(new_selected_item_y >= (model->reserve_y - 1))
                return false;

            ++new_selected_item_y;

            for (i = 0; i < model->reserve_x; ++i) {
                new_selected_item_x = (model->selected_item_x + i) % model->reserve_x;
                if (*button_panel_get_item(model, new_selected_item_x, new_selected_item_y)) {
                    break;
                }
            }
            if (i == model->reserve_x)
                return false;

            model->selected_item_x = new_selected_item_x;
            model->selected_item_y = new_selected_item_y;
            printf("DOWN after, selected[%d][%d]\r\n",
                    model->selected_item_x, model->selected_item_y);

            return true;
        });
}

static void button_panel_process_up(ButtonPanel* button_panel) {

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            printf("UP before, selected[%d][%d]\r\n",
                    model->selected_item_x, model->selected_item_y);
            size_t new_selected_item_x = model->selected_item_x;
            size_t new_selected_item_y = model->selected_item_y;
            size_t i;

            if(new_selected_item_y <= 0)
                return false;

            --new_selected_item_y;

            for (i = 0; i < model->reserve_x; ++i) {
                new_selected_item_x = (model->selected_item_x + i) % model->reserve_x;
                if (*button_panel_get_item(model, new_selected_item_x, new_selected_item_y)) {
                    break;
                }
            }
            if (i == model->reserve_x)
                return false;

            model->selected_item_x = new_selected_item_x;
            model->selected_item_y = new_selected_item_y;
            printf("UP after, selected[%d][%d]\r\n",
                    model->selected_item_x, model->selected_item_y);
            return true;
        });
}

static void button_panel_process_left(ButtonPanel* button_panel) {

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            printf("LEFT before, selected[%d][%d]\r\n",
                    model->selected_item_x, model->selected_item_y);
            size_t new_selected_item_x = model->selected_item_x;
            size_t new_selected_item_y = model->selected_item_y;
            size_t i;

            if(new_selected_item_x <= 0)
                return false;

            --new_selected_item_x;

            for (i = 0; i < model->reserve_y; ++i) {
                new_selected_item_y = (model->selected_item_y + i) % model->reserve_y;
                if (*button_panel_get_item(model, new_selected_item_x, new_selected_item_y)) {
                    break;
                }
            }
            if (i == model->reserve_y)
                return false;

            model->selected_item_x = new_selected_item_x;
            model->selected_item_y = new_selected_item_y;
            printf("LEFT after, selected[%d][%d]\r\n",
                    model->selected_item_x, model->selected_item_y);
            return true;
        });
}

static void button_panel_process_right(ButtonPanel* button_panel) {

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            printf("RIGHT before, selected[%d][%d]\r\n",
                    model->selected_item_x, model->selected_item_y);
            size_t new_selected_item_x = model->selected_item_x;
            size_t new_selected_item_y = model->selected_item_y;
            size_t i;

            if(new_selected_item_x >= (model->reserve_x - 1))
                return false;

            ++new_selected_item_x;

            for (i = 0; i < model->reserve_y; ++i) {
                new_selected_item_y = (model->selected_item_y + i) % model->reserve_y;
                if (*button_panel_get_item(model, new_selected_item_x, new_selected_item_y)) {
                    break;
                }
            }
            if (i == model->reserve_y)
                return false;

            model->selected_item_x = new_selected_item_x;
            model->selected_item_y = new_selected_item_y;
            printf("RIGHT after, selected[%d][%d]\r\n",
                    model->selected_item_x, model->selected_item_y);
            return true;
        });
}

void button_panel_process_ok(ButtonPanel* button_panel) {

    printf("OK\r\n");
    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            return true;
        });

//    ButtonItem* item = NULL;
//    if(item && item->callback) {
//        item->callback(item->callback_context, item->index);
//    }
}

static bool button_panel_view_input_callback(InputEvent* event, void* context) {
    ButtonPanel* button_panel = context;
    furi_assert(button_panel);
    bool consumed = false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyUp:
            consumed = true;
            button_panel_process_up(button_panel);
            break;
        case InputKeyDown:
            consumed = true;
            button_panel_process_down(button_panel);
            break;
        case InputKeyLeft:
            consumed = true;
            button_panel_process_left(button_panel);
            break;
        case InputKeyRight:
            consumed = true;
            button_panel_process_right(button_panel);
            break;
        case InputKeyOk:
            consumed = true;
            button_panel_process_ok(button_panel);
            break;
        default:
            break;
        }
    }

    return consumed;
}

#if 0

ButtonItem* button_panel_add_item_into_row(
    ButtonPanel* button_panel,
    ButtonRow* row,
    uint32_t index,
    uint16_t x,
    uint16_t y,
    IconName icon_name,
    ButtonItemCallback callback,
    void* callback_context)
{
    furi_assert(button_panel);

    ButtonItem* item = NULL;

    Icon* icon = assets_icons_get(icon_name);
    uint8_t width = icon_get_width(icon);
    icon_free(icon);

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            item = ButtonItemArray_push_new(row->array);
            item->icon.x = x;
            item->icon.y = y;
            item->icon.xcenter = x + width/2;
            item->icon.name = icon_name;
            item->index = index;
            item->callback = callback;
            item->callback_context = callback_context;
            return true;
        });

    return item;
}

static void button_panel_view_draw_callback(Canvas* canvas, void* _model) {
    ButtonPanelModel* model = _model;

    canvas_clear(canvas);

    ButtonIList_it_t it;
    ButtonItemArray_it_t it2;

    for(ButtonIList_it(it, model->buttons);
        !ButtonIList_end_p(it);
        ButtonIList_next(it))
    {
        for(ButtonItemArray_it(it2, ButtonIList_cref(it)->array);
            !ButtonItemArray_end_p(it); 
            ButtonItemArray_next(it2))
        {

        }
    }


    for M_EACH(row, model->buttons, ButtonIList_t) {
        for M_EACH(button_item, row->array, ILIST_OPLIST(ButtonItemArray_t)) {
            canvas_draw_icon_name(canvas,
                                  button_item->x,
                                  button_item->icon.y,
                                  button_item->icon.name);
        }
    }

    canvas_set_font(canvas, FontSecondary);

    M_GET_SUBTYPE
    for M_EACH(label, model->labels, LabelList_t) {
        canvas_draw_str(canvas, label->x, label->y, label->str);
    }




}

static bool button_panel_view_input_callback(InputEvent* event, void* context) {
    ButtonPanel* button_panel = context;
    furi_assert(button_panel);
    bool consumed = false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyUp:
            consumed = true;
            button_panel_process_up(button_panel);
            break;
        case InputKeyOk:
            consumed = true;
            button_panel_process_ok(button_panel);
            break;
        default:
            break;
        }
    }

    return consumed;
}

ButtonPanel* button_panel_alloc() {
    ButtonPanel* button_panel = furi_alloc(sizeof(ButtonPanel));
    button_panel->view = view_alloc();
    view_set_context(button_panel->view, button_panel);
    view_allocate_model(button_panel->view, ViewModelTypeLocking, sizeof(ButtonPanelModel));
    view_set_draw_callback(button_panel->view, button_panel_view_draw_callback);
    view_set_input_callback(button_panel->view, button_panel_view_input_callback);

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            ButtonIList_init(model->buttons);
            LabelList_init(model->labels);
            model->selected_row = NULL;
            model->selected_item_index = 0;
            return true;
        });

    return button_panel;
}

void button_panel_free(ButtonPanel* button_panel) {
    furi_assert(button_panel);

    button_panel_clean(button_panel);
    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            ButtonIList_clear(model->buttons);
            LabelList_clear(model->labels);
            return true;
        });

    view_free(button_panel->view);
    free(button_panel);
}

View* button_panel_get_view(ButtonPanel* button_panel) {
    furi_assert(button_panel);
    return button_panel->view;
}

void button_panel_clean(ButtonPanel* button_panel) {
    furi_assert(button_panel);

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            while(!ButtonIList_empty_p(model->buttons)) {
                ButtonRow* row = ButtonIList_pop_front(model->buttons);
                ButtonItemArray_clear(row->array);
                free(row);
            }

            LabelList_clean(model->labels);
            return true;
        });
}

void button_panel_process_up(ButtonPanel* button_panel) {
    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            return true;
        });
}

void button_panel_process_ok(ButtonPanel* button_panel) {

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            return true;
        });

//    ButtonItem* item = NULL;
//    if(item && item->callback) {
//        item->callback(item->callback_context, item->index);
//    }
}

void button_panel_add_label(ButtonPanel* button_panel,
                            uint16_t x,
                            uint16_t y,
                            const char* label_str) {
    furi_assert(button_panel);

    with_view_model(
        button_panel->view, (ButtonPanelModel * model) {
            LabelElement* label = LabelList_push_raw(model->labels);
            label->x = x;
            label->y = y;
            label->str = label_str;
            return true;
        });
}
#endif
