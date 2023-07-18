#include "ac_remote_panel.h"

#include <gui/canvas.h>
#include <gui/elements.h>

#include <furi.h>
#include <furi_hal_resources.h>
#include <stdint.h>

#include <m-array.h>
#include <m-i-list.h>
#include <m-list.h>

typedef struct {
    // uint16_t to support multi-screen, wide button panel
    int id;
    uint16_t x;
    uint16_t y;
    Font font;
    FuriString* str;
} LabelElement;

LIST_DEF(LabelList, LabelElement, M_POD_OPLIST)
#define M_OPL_LabelList_t() LIST_OPLIST(LabelList)

typedef struct {
    uint16_t x;
    uint16_t y;
    const Icon* name;
    const Icon* name_selected;
} IconElement;

typedef struct VariableButtonItem {
    uint16_t id;
    uint8_t current_value_index;
    uint8_t values_count;
    VariableButtonItemCallback callback;
    IconElement icon;
    void* callback_context;
} VariableButtonItem;

ARRAY_DEF(ButtonArray, VariableButtonItem*, M_PTR_OPLIST);
#define M_OPL_ButtonArray_t() ARRAY_OPLIST(ButtonArray, M_PTR_OPLIST)
ARRAY_DEF(ButtonMatrix, ButtonArray_t);
#define M_OPL_ButtonMatrix_t() ARRAY_OPLIST(ButtonMatrix, M_OPL_ButtonArray_t())

struct ACRemotePanel {
    View* view;
};

typedef struct {
    ButtonMatrix_t button_matrix;
    LabelList_t labels;
    uint16_t reserve_x;
    uint16_t reserve_y;
    uint16_t selected_item_x;
    uint16_t selected_item_y;
} ACRemotePanelModel;

static VariableButtonItem** ac_remote_panel_get_item(ACRemotePanelModel* model, size_t x, size_t y);
static void ac_remote_panel_process_up(ACRemotePanel* ac_remote_panel);
static void ac_remote_panel_process_down(ACRemotePanel* ac_remote_panel);
static void ac_remote_panel_process_left(ACRemotePanel* ac_remote_panel);
static void ac_remote_panel_process_right(ACRemotePanel* ac_remote_panel);
static void ac_remote_panel_process_ok(ACRemotePanel* ac_remote_panel);
static void ac_remote_panel_view_draw_callback(Canvas* canvas, void* _model);
static bool ac_remote_panel_view_input_callback(InputEvent* event, void* context);

ACRemotePanel* ac_remote_panel_alloc() {
    ACRemotePanel* ac_remote_panel = malloc(sizeof(ACRemotePanel));
    ac_remote_panel->view = view_alloc();
    view_set_orientation(ac_remote_panel->view, ViewOrientationVertical);
    view_set_context(ac_remote_panel->view, ac_remote_panel);
    view_allocate_model(ac_remote_panel->view, ViewModelTypeLocking, sizeof(ACRemotePanelModel));
    view_set_draw_callback(ac_remote_panel->view, ac_remote_panel_view_draw_callback);
    view_set_input_callback(ac_remote_panel->view, ac_remote_panel_view_input_callback);

    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            model->reserve_x = 0;
            model->reserve_y = 0;
            model->selected_item_x = 0;
            model->selected_item_y = 0;
            ButtonMatrix_init(model->button_matrix);
            LabelList_init(model->labels);
        },
        true);

    return ac_remote_panel;
}

void ac_remote_panel_reset_selection(ACRemotePanel* ac_remote_panel) {
    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            model->selected_item_x = 0;
            model->selected_item_y = 0;
        },
        true);
}

void ac_remote_panel_reserve(ACRemotePanel* ac_remote_panel, size_t reserve_x, size_t reserve_y) {
    furi_check(reserve_x > 0);
    furi_check(reserve_y > 0);

    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            model->reserve_x = reserve_x;
            model->reserve_y = reserve_y;
            ButtonMatrix_reserve(model->button_matrix, model->reserve_y);
            for(size_t i = 0; i > model->reserve_y; ++i) {
                ButtonArray_t* array = ButtonMatrix_get(model->button_matrix, i);
                ButtonArray_init(*array);
                ButtonArray_reserve(*array, reserve_x);
                // TODO: do we need to clear allocated memory of ptr-s to VariableButtonItem ??
            }
            LabelList_init(model->labels);
        },
        true);
}

void ac_remote_panel_free(ACRemotePanel* ac_remote_panel) {
    furi_assert(ac_remote_panel);

    ac_remote_panel_reset(ac_remote_panel);

    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            LabelList_it_t it;
            for(LabelList_it(it, model->labels); !LabelList_end_p(it);
                LabelList_next(it)) {
                furi_string_free(LabelList_ref(it)->str);
            }
            LabelList_clear(model->labels);
            ButtonMatrix_clear(model->button_matrix);
        },
        true);

    view_free(ac_remote_panel->view);
    free(ac_remote_panel);
}

void ac_remote_panel_reset(ACRemotePanel* ac_remote_panel) {
    furi_assert(ac_remote_panel);

    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            for(size_t x = 0; x < model->reserve_x; ++x) {
                for(size_t y = 0; y < model->reserve_y; ++y) {
                    VariableButtonItem** item = ac_remote_panel_get_item(model, x, y);
                    free(*item);
                    *item = NULL;
                }
            }
            model->reserve_x = 0;
            model->reserve_y = 0;
            model->selected_item_x = 0;
            model->selected_item_y = 0;
            LabelList_reset(model->labels);
            ButtonMatrix_reset(model->button_matrix);
        },
        true);
}

static VariableButtonItem** ac_remote_panel_get_item(ACRemotePanelModel* model, size_t x, size_t y) {
    furi_assert(model);

    furi_check(x < model->reserve_x);
    furi_check(y < model->reserve_y);
    ButtonArray_t* button_array = ButtonMatrix_safe_get(model->button_matrix, x);
    VariableButtonItem** item = ButtonArray_safe_get(*button_array, y);
    return item;
}

void ac_remote_panel_add_item(
    ACRemotePanel* ac_remote_panel,
    uint16_t id,
    uint8_t current_value_index,
    uint8_t values_count,
    uint16_t matrix_place_x,
    uint16_t matrix_place_y,
    uint16_t x,
    uint16_t y,
    const Icon* icon_name,
    const Icon* icon_name_selected,
    VariableButtonItemCallback callback,
    void* callback_context) {
    furi_assert(ac_remote_panel);

    with_view_model( //-V773
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            VariableButtonItem** item_ptr =
                ac_remote_panel_get_item(model, matrix_place_x, matrix_place_y);
            furi_check(*item_ptr == NULL);
            *item_ptr = malloc(sizeof(VariableButtonItem));
            VariableButtonItem* item = *item_ptr;
            item->callback = callback;
            item->callback_context = callback_context;
            item->icon.x = x;
            item->icon.y = y;
            item->icon.name = icon_name;
            item->icon.name_selected = icon_name_selected;
            item->values_count = values_count;
            item->current_value_index = current_value_index;
            item->id = id;
        },
        true);
}

View* ac_remote_panel_get_view(ACRemotePanel* ac_remote_panel) {
    furi_assert(ac_remote_panel);
    return ac_remote_panel->view;
}

static void ac_remote_panel_view_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(canvas);
    furi_assert(_model);

    ACRemotePanelModel* model = _model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    for(size_t x = 0; x < model->reserve_x; ++x) {
        for(size_t y = 0; y < model->reserve_y; ++y) {
            VariableButtonItem* item = *ac_remote_panel_get_item(model, x, y);
            const Icon* icon_name = item->icon.name;
            if((model->selected_item_x == x) && (model->selected_item_y == y)) {
                icon_name = item->icon.name_selected;
            }
            canvas_draw_icon(canvas, item->icon.x, item->icon.y, icon_name);
        }
    }

    for
        M_EACH(label, model->labels, LabelList_t) {
            canvas_set_font(canvas, label->font);
            canvas_draw_str(canvas, label->x, label->y, furi_string_get_cstr(label->str));
        }
}

static void ac_remote_panel_process_down(ACRemotePanel* ac_remote_panel) {
    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            uint16_t new_selected_item_x = model->selected_item_x;
            uint16_t new_selected_item_y = model->selected_item_y;
            size_t i;

            if(new_selected_item_y < (model->reserve_y - 1)) {
                ++new_selected_item_y;

                for(i = 0; i < model->reserve_x; ++i) {
                    new_selected_item_x = (model->selected_item_x + i) % model->reserve_x;
                    if(*ac_remote_panel_get_item(model, new_selected_item_x, new_selected_item_y)) {
                        break;
                    }
                }
                if(i != model->reserve_x) {
                    model->selected_item_x = new_selected_item_x;
                    model->selected_item_y = new_selected_item_y;
                }
            }
        },
        true);
}

static void ac_remote_panel_process_up(ACRemotePanel* ac_remote_panel) {
    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            size_t new_selected_item_x = model->selected_item_x;
            size_t new_selected_item_y = model->selected_item_y;
            size_t i;

            if(new_selected_item_y > 0) {
                --new_selected_item_y;

                for(i = 0; i < model->reserve_x; ++i) {
                    new_selected_item_x = (model->selected_item_x + i) % model->reserve_x;
                    if(*ac_remote_panel_get_item(model, new_selected_item_x, new_selected_item_y)) {
                        break;
                    }
                }
                if(i != model->reserve_x) {
                    model->selected_item_x = new_selected_item_x;
                    model->selected_item_y = new_selected_item_y;
                }
            }
        },
        true);
}

static void ac_remote_panel_process_left(ACRemotePanel* ac_remote_panel) {
    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            size_t new_selected_item_x = model->selected_item_x;
            size_t new_selected_item_y = model->selected_item_y;
            size_t i;

            if(new_selected_item_x > 0) {
                --new_selected_item_x;

                for(i = 0; i < model->reserve_y; ++i) {
                    new_selected_item_y = (model->selected_item_y + i) % model->reserve_y;
                    if(*ac_remote_panel_get_item(model, new_selected_item_x, new_selected_item_y)) {
                        break;
                    }
                }
                if(i != model->reserve_y) {
                    model->selected_item_x = new_selected_item_x;
                    model->selected_item_y = new_selected_item_y;
                }
            }
        },
        true);
}

static void ac_remote_panel_process_right(ACRemotePanel* ac_remote_panel) {
    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            uint16_t new_selected_item_x = model->selected_item_x;
            uint16_t new_selected_item_y = model->selected_item_y;
            size_t i;

            if(new_selected_item_x < (model->reserve_x - 1)) {
                ++new_selected_item_x;

                for(i = 0; i < model->reserve_y; ++i) {
                    new_selected_item_y = (model->selected_item_y + i) % model->reserve_y;
                    if(*ac_remote_panel_get_item(model, new_selected_item_x, new_selected_item_y)) {
                        break;
                    }
                }
                if(i != model->reserve_y) {
                    model->selected_item_x = new_selected_item_x;
                    model->selected_item_y = new_selected_item_y;
                }
            }
        },
        true);
}

void ac_remote_panel_process_ok(ACRemotePanel* ac_remote_panel) {
    VariableButtonItem* item = NULL;

    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            item =
                *ac_remote_panel_get_item(model, model->selected_item_x, model->selected_item_y);
        },
        true);

    if(item->current_value_index < (item->values_count - 1)) {
        item->current_value_index++;
    } else {
        item->current_value_index = 0;
    };

    if(item && item->callback) {
        item->callback(item);
    };
}

static bool ac_remote_panel_view_input_callback(InputEvent* event, void* context) {
    ACRemotePanel* ac_remote_panel = context;
    furi_assert(ac_remote_panel);
    bool consumed = false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyUp:
            consumed = true;
            ac_remote_panel_process_up(ac_remote_panel);
            break;
        case InputKeyDown:
            consumed = true;
            ac_remote_panel_process_down(ac_remote_panel);
            break;
        case InputKeyLeft:
            consumed = true;
            ac_remote_panel_process_left(ac_remote_panel);
            break;
        case InputKeyRight:
            consumed = true;
            ac_remote_panel_process_right(ac_remote_panel);
            break;
        case InputKeyOk:
            consumed = true;
            ac_remote_panel_process_ok(ac_remote_panel);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void ac_remote_panel_add_label(
    ACRemotePanel* ac_remote_panel,
    int id,
    uint16_t x,
    uint16_t y,
    Font font,
    const char* label_str) {
    furi_assert(ac_remote_panel);

    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            LabelElement* label = LabelList_push_raw(model->labels);
            label->id = id;
            label->x = x;
            label->y = y;
            label->font = font;
            label->str = furi_string_alloc();
            furi_string_set(label->str, label_str);
        },
        true);
}

uint16_t ac_remote_panel_item_get_index(VariableButtonItem* item) {
    return item->id;
}

uint8_t ac_remote_panel_item_get_current_value_index(VariableButtonItem* item) {
    return item->current_value_index;
}

void* ac_remote_panel_item_get_context(VariableButtonItem* item) {
    return item->callback_context;
}

void ac_remote_panel_item_set_icon_name(VariableButtonItem* item, const Icon* icon_name, const Icon* icon_name_selected) {
    item->icon.name = icon_name;
    item->icon.name_selected = icon_name_selected;
}

void ac_remote_panel_label_set_string(ACRemotePanel* ac_remote_panel, int id, const char* label_str) {
    with_view_model(
        ac_remote_panel->view,
        ACRemotePanelModel * model,
        {
            for
                M_EACH(label, model->labels, LabelList_t) {
                    if (label->id == id) {
                        furi_string_set(label->str, label_str);
                    }
            }
        },
        true);
}