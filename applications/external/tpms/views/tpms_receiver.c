#include "tpms_receiver.h"
#include "../tpms_app_i.h"
#include <tpms_icons.h>
#include <math.h>

#include <input/input.h>
#include <gui/elements.h>
#include <m-array.h>

#define TAG "TPMSReceiver"

#define FRAME_HEIGHT 12
#define MAX_LEN_PX 112
#define MENU_ITEMS 4u
#define UNLOCK_CNT 3

#define SUBGHZ_RAW_THRESHOLD_MIN -90.0f
typedef struct {
    FuriString* item_str;
    uint8_t type;
} TPMSReceiverMenuItem;

ARRAY_DEF(TPMSReceiverMenuItemArray, TPMSReceiverMenuItem, M_POD_OPLIST)

#define M_OPL_TPMSReceiverMenuItemArray_t() ARRAY_OPLIST(TPMSReceiverMenuItemArray, M_POD_OPLIST)

struct TPMSReceiverHistory {
    TPMSReceiverMenuItemArray_t data;
};

typedef struct TPMSReceiverHistory TPMSReceiverHistory;

// static const Icon* ReceiverItemIcons[] = {
//     [SubGhzProtocolTypeUnknown] = &I_Quest_7x8,
//     [SubGhzProtocolTypeStatic] = &I_Unlock_7x8,
//     [SubGhzProtocolTypeDynamic] = &I_Lock_7x8,
//     //[SubGhzProtocolWeatherStation] = &I_station_icon,
// };

typedef enum {
    TPMSReceiverBarShowDefault,
    TPMSReceiverBarShowLock,
    TPMSReceiverBarShowToUnlockPress,
    TPMSReceiverBarShowUnlock,
} TPMSReceiverBarShow;

struct TPMSReceiver {
    TPMSLock lock;
    uint8_t lock_count;
    FuriTimer* lock_timer;
    FuriTimer* relearn_timer;
    bool relearn_active;
    View* view;
    TPMSReceiverCallback callback;
    void* context;
};

typedef struct {
    FuriString* frequency_str;
    FuriString* preset_str;
    FuriString* history_stat_str;
    TPMSReceiverHistory* history;
    uint16_t idx;
    uint16_t list_offset;
    uint16_t history_item;
    TPMSReceiverBarShow bar_show;
    uint8_t u_rssi;
    bool external_radio;
} TPMSReceiverModel;

void tpms_view_receiver_set_rssi(TPMSReceiver* instance, float rssi) {
    furi_assert(instance);
    with_view_model(
        instance->view,
        TPMSReceiverModel * model,
        {
            if(rssi < SUBGHZ_RAW_THRESHOLD_MIN) {
                model->u_rssi = 0;
            } else {
                model->u_rssi = (uint8_t)(rssi - SUBGHZ_RAW_THRESHOLD_MIN);
            }
        },
        true);
}

void tpms_view_receiver_set_lock(TPMSReceiver* tpms_receiver, TPMSLock lock) {
    furi_assert(tpms_receiver);
    tpms_receiver->lock_count = 0;
    if(lock == TPMSLockOn) {
        tpms_receiver->lock = lock;
        with_view_model(
            tpms_receiver->view,
            TPMSReceiverModel * model,
            { model->bar_show = TPMSReceiverBarShowLock; },
            true);
        furi_timer_start(tpms_receiver->lock_timer, pdMS_TO_TICKS(1000));
    } else {
        with_view_model(
            tpms_receiver->view,
            TPMSReceiverModel * model,
            { model->bar_show = TPMSReceiverBarShowDefault; },
            true);
    }
}

void tpms_view_receiver_set_callback(
    TPMSReceiver* tpms_receiver,
    TPMSReceiverCallback callback,
    void* context) {
    furi_assert(tpms_receiver);
    furi_assert(callback);
    tpms_receiver->callback = callback;
    tpms_receiver->context = context;
}

static void tpms_view_receiver_update_offset(TPMSReceiver* tpms_receiver) {
    furi_assert(tpms_receiver);

    with_view_model(
        tpms_receiver->view,
        TPMSReceiverModel * model,
        {
            size_t history_item = model->history_item;
            uint16_t bounds = history_item > 3 ? 2 : history_item;

            if(history_item > 3 && model->idx >= (int16_t)(history_item - 1)) {
                model->list_offset = model->idx - 3;
            } else if(model->list_offset < model->idx - bounds) {
                model->list_offset =
                    CLAMP(model->list_offset + 1, (int16_t)(history_item - bounds), 0);
            } else if(model->list_offset > model->idx - bounds) {
                model->list_offset = CLAMP(model->idx - 1, (int16_t)(history_item - bounds), 0);
            }
        },
        true);
}

void tpms_view_receiver_add_item_to_menu(
    TPMSReceiver* tpms_receiver,
    const char* name,
    uint8_t type) {
    furi_assert(tpms_receiver);
    with_view_model(
        tpms_receiver->view,
        TPMSReceiverModel * model,
        {
            TPMSReceiverMenuItem* item_menu =
                TPMSReceiverMenuItemArray_push_raw(model->history->data);
            item_menu->item_str = furi_string_alloc_set(name);
            item_menu->type = type;
            if((model->idx == model->history_item - 1)) {
                model->history_item++;
                model->idx++;
            } else {
                model->history_item++;
            }
        },
        true);
    tpms_view_receiver_update_offset(tpms_receiver);
}

void tpms_view_receiver_add_data_statusbar(
    TPMSReceiver* tpms_receiver,
    const char* frequency_str,
    const char* preset_str,
    const char* history_stat_str,
    bool external) {
    furi_assert(tpms_receiver);
    with_view_model(
        tpms_receiver->view,
        TPMSReceiverModel * model,
        {
            furi_string_set_str(model->frequency_str, frequency_str);
            furi_string_set_str(model->preset_str, preset_str);
            furi_string_set_str(model->history_stat_str, history_stat_str);
            model->external_radio = external;
        },
        true);
}

static void tpms_view_receiver_draw_frame(Canvas* canvas, uint16_t idx, bool scrollbar) {
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_box(canvas, 0, 0 + idx * FRAME_HEIGHT, scrollbar ? 122 : 127, FRAME_HEIGHT);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_dot(canvas, 0, 0 + idx * FRAME_HEIGHT);
    canvas_draw_dot(canvas, 1, 0 + idx * FRAME_HEIGHT);
    canvas_draw_dot(canvas, 0, (0 + idx * FRAME_HEIGHT) + 1);

    canvas_draw_dot(canvas, 0, (0 + idx * FRAME_HEIGHT) + 11);
    canvas_draw_dot(canvas, scrollbar ? 121 : 126, 0 + idx * FRAME_HEIGHT);
    canvas_draw_dot(canvas, scrollbar ? 121 : 126, (0 + idx * FRAME_HEIGHT) + 11);
}

static void tpms_view_rssi_draw(Canvas* canvas, TPMSReceiverModel* model) {
    for(uint8_t i = 1; i < model->u_rssi; i++) {
        if(i % 5) {
            canvas_draw_dot(canvas, 46 + i, 50);
            canvas_draw_dot(canvas, 47 + i, 51);
            canvas_draw_dot(canvas, 46 + i, 52);
        }
    }
}

void tpms_view_receiver_draw(Canvas* canvas, TPMSReceiverModel* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    elements_button_left(canvas, "Config");

    bool scrollbar = model->history_item > 4;
    FuriString* str_buff;
    str_buff = furi_string_alloc();

    TPMSReceiverMenuItem* item_menu;

    for(size_t i = 0; i < MIN(model->history_item, MENU_ITEMS); ++i) {
        size_t idx = CLAMP((uint16_t)(i + model->list_offset), model->history_item, 0);
        item_menu = TPMSReceiverMenuItemArray_get(model->history->data, idx);
        furi_string_set(str_buff, item_menu->item_str);
        elements_string_fit_width(canvas, str_buff, scrollbar ? MAX_LEN_PX - 6 : MAX_LEN_PX);
        if(model->idx == idx) {
            tpms_view_receiver_draw_frame(canvas, i, scrollbar);
        } else {
            canvas_set_color(canvas, ColorBlack);
        }
        // canvas_draw_icon(canvas, 4, 2 + i * FRAME_HEIGHT, ReceiverItemIcons[item_menu->type]);
        canvas_draw_str(canvas, 4, 9 + i * FRAME_HEIGHT, furi_string_get_cstr(str_buff));
        furi_string_reset(str_buff);
    }
    if(scrollbar) {
        elements_scrollbar_pos(canvas, 128, 0, 49, model->idx, model->history_item);
    }
    furi_string_free(str_buff);

    canvas_set_color(canvas, ColorBlack);

    if(model->history_item == 0) {
        canvas_draw_icon(
            canvas, 0, 0, model->external_radio ? &I_Fishing_123x52 : &I_Scanning_123x52);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 63, 46, "Scanning...");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 44, 10, model->external_radio ? "Ext" : "Int");
        canvas_draw_str(canvas, 70, 9, "-> to relearn");
    }

    // Draw RSSI
    tpms_view_rssi_draw(canvas, model);

    switch(model->bar_show) {
    case TPMSReceiverBarShowLock:
        canvas_draw_icon(canvas, 64, 55, &I_Lock_7x8);
        canvas_draw_str(canvas, 74, 62, "Locked");
        break;
    case TPMSReceiverBarShowToUnlockPress:
        canvas_draw_str(canvas, 44, 62, furi_string_get_cstr(model->frequency_str));
        canvas_draw_str(canvas, 79, 62, furi_string_get_cstr(model->preset_str));
        canvas_draw_str(canvas, 96, 62, furi_string_get_cstr(model->history_stat_str));
        canvas_set_font(canvas, FontSecondary);
        elements_bold_rounded_frame(canvas, 14, 8, 99, 48);
        elements_multiline_text(canvas, 65, 26, "To unlock\npress:");
        canvas_draw_icon(canvas, 65, 42, &I_Pin_back_arrow_10x8);
        canvas_draw_icon(canvas, 80, 42, &I_Pin_back_arrow_10x8);
        canvas_draw_icon(canvas, 95, 42, &I_Pin_back_arrow_10x8);
        canvas_draw_icon(canvas, 16, 13, &I_WarningDolphin_45x42);
        canvas_draw_dot(canvas, 17, 61);
        break;
    case TPMSReceiverBarShowUnlock:
        canvas_draw_icon(canvas, 64, 55, &I_Unlock_7x8);
        canvas_draw_str(canvas, 74, 62, "Unlocked");
        break;
    default:
        canvas_draw_str(canvas, 44, 62, furi_string_get_cstr(model->frequency_str));
        canvas_draw_str(canvas, 79, 62, furi_string_get_cstr(model->preset_str));
        canvas_draw_str(canvas, 96, 62, furi_string_get_cstr(model->history_stat_str));
        break;
    }
}

static void tpms_view_receiver_lock_timer_callback(void* context) {
    furi_assert(context);
    TPMSReceiver* tpms_receiver = context;
    with_view_model(
        tpms_receiver->view,
        TPMSReceiverModel * model,
        { model->bar_show = TPMSReceiverBarShowDefault; },
        true);
    if(tpms_receiver->lock_count < UNLOCK_CNT) {
        tpms_receiver->callback(TPMSCustomEventViewReceiverOffDisplay, tpms_receiver->context);
    } else {
        tpms_receiver->lock = TPMSLockOff;
        tpms_receiver->callback(TPMSCustomEventViewReceiverUnlock, tpms_receiver->context);
    }
    tpms_receiver->lock_count = 0;
}

static void tpms_relearn_stop(void* context) {
    furi_assert(context);
    TPMSReceiver* tpms_receiver = context;
    if(tpms_receiver->relearn_active) {
        tpms_receiver->relearn_active = false;
        furi_timer_stop(tpms_receiver->relearn_timer);
        furi_hal_rfid_tim_read_stop();
    }
}

static void tpms_relearn_start(void* context) {
    furi_assert(context);
    TPMSReceiver* tpms_receiver = context;
    if(tpms_receiver->relearn_active) tpms_relearn_stop(context);
    tpms_receiver->relearn_active = true;
    furi_hal_rfid_tim_read_start(125000, 0.5);
    furi_timer_start(tpms_receiver->relearn_timer, pdMS_TO_TICKS(3000));
}

static void tpms_view_receiver_relearn_timer_callback(void* context) {
    furi_assert(context);
    tpms_relearn_stop(context);
}

bool tpms_view_receiver_input(InputEvent* event, void* context) {
    furi_assert(context);
    TPMSReceiver* tpms_receiver = context;

    if(tpms_receiver->lock == TPMSLockOn) {
        with_view_model(
            tpms_receiver->view,
            TPMSReceiverModel * model,
            { model->bar_show = TPMSReceiverBarShowToUnlockPress; },
            true);
        if(tpms_receiver->lock_count == 0) {
            furi_timer_start(tpms_receiver->lock_timer, pdMS_TO_TICKS(1000));
        }
        if(event->key == InputKeyBack && event->type == InputTypeShort) {
            tpms_receiver->lock_count++;
        }
        if(tpms_receiver->lock_count >= UNLOCK_CNT) {
            tpms_receiver->callback(TPMSCustomEventViewReceiverUnlock, tpms_receiver->context);
            with_view_model(
                tpms_receiver->view,
                TPMSReceiverModel * model,
                { model->bar_show = TPMSReceiverBarShowUnlock; },
                true);
            tpms_receiver->lock = TPMSLockOff;
            furi_timer_start(tpms_receiver->lock_timer, pdMS_TO_TICKS(650));
        }

        return true;
    }

    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        tpms_receiver->callback(TPMSCustomEventViewReceiverBack, tpms_receiver->context);
    } else if(
        event->key == InputKeyUp &&
        (event->type == InputTypeShort || event->type == InputTypeRepeat)) {
        with_view_model(
            tpms_receiver->view,
            TPMSReceiverModel * model,
            {
                if(model->idx != 0) model->idx--;
            },
            true);
    } else if(
        event->key == InputKeyDown &&
        (event->type == InputTypeShort || event->type == InputTypeRepeat)) {
        with_view_model(
            tpms_receiver->view,
            TPMSReceiverModel * model,
            {
                if(model->history_item && model->idx != model->history_item - 1) model->idx++;
            },
            true);
    } else if(event->key == InputKeyLeft && event->type == InputTypeShort) {
        tpms_receiver->callback(TPMSCustomEventViewReceiverConfig, tpms_receiver->context);
    } else if(event->key == InputKeyRight && event->type == InputTypeShort) {
        tpms_relearn_start(tpms_receiver);
    } else if(event->key == InputKeyOk && event->type == InputTypeShort) {
        with_view_model(
            tpms_receiver->view,
            TPMSReceiverModel * model,
            {
                if(model->history_item != 0) {
                    tpms_receiver->callback(TPMSCustomEventViewReceiverOK, tpms_receiver->context);
                }
            },
            false);
    }

    tpms_view_receiver_update_offset(tpms_receiver);

    return true;
}

void tpms_view_receiver_enter(void* context) {
    furi_assert(context);
}

void tpms_view_receiver_exit(void* context) {
    furi_assert(context);
    TPMSReceiver* tpms_receiver = context;
    with_view_model(
        tpms_receiver->view,
        TPMSReceiverModel * model,
        {
            furi_string_reset(model->frequency_str);
            furi_string_reset(model->preset_str);
            furi_string_reset(model->history_stat_str);
                for
                    M_EACH(item_menu, model->history->data, TPMSReceiverMenuItemArray_t) {
                        furi_string_free(item_menu->item_str);
                        item_menu->type = 0;
                    }
                TPMSReceiverMenuItemArray_reset(model->history->data);
                model->idx = 0;
                model->list_offset = 0;
                model->history_item = 0;
        },
        false);
    furi_timer_stop(tpms_receiver->lock_timer);
    tpms_relearn_stop(tpms_receiver);
}

TPMSReceiver* tpms_view_receiver_alloc() {
    TPMSReceiver* tpms_receiver = malloc(sizeof(TPMSReceiver));

    // View allocation and configuration
    tpms_receiver->view = view_alloc();

    tpms_receiver->lock = TPMSLockOff;
    tpms_receiver->lock_count = 0;
    view_allocate_model(tpms_receiver->view, ViewModelTypeLocking, sizeof(TPMSReceiverModel));
    view_set_context(tpms_receiver->view, tpms_receiver);
    view_set_draw_callback(tpms_receiver->view, (ViewDrawCallback)tpms_view_receiver_draw);
    view_set_input_callback(tpms_receiver->view, tpms_view_receiver_input);
    view_set_enter_callback(tpms_receiver->view, tpms_view_receiver_enter);
    view_set_exit_callback(tpms_receiver->view, tpms_view_receiver_exit);

    with_view_model(
        tpms_receiver->view,
        TPMSReceiverModel * model,
        {
            model->frequency_str = furi_string_alloc();
            model->preset_str = furi_string_alloc();
            model->history_stat_str = furi_string_alloc();
            model->bar_show = TPMSReceiverBarShowDefault;
            model->history = malloc(sizeof(TPMSReceiverHistory));
            model->external_radio = false;
            TPMSReceiverMenuItemArray_init(model->history->data);
        },
        true);
    tpms_receiver->lock_timer =
        furi_timer_alloc(tpms_view_receiver_lock_timer_callback, FuriTimerTypeOnce, tpms_receiver);
    tpms_receiver->relearn_timer = furi_timer_alloc(
        tpms_view_receiver_relearn_timer_callback, FuriTimerTypeOnce, tpms_receiver);
    return tpms_receiver;
}

void tpms_view_receiver_free(TPMSReceiver* tpms_receiver) {
    furi_assert(tpms_receiver);

    with_view_model(
        tpms_receiver->view,
        TPMSReceiverModel * model,
        {
            furi_string_free(model->frequency_str);
            furi_string_free(model->preset_str);
            furi_string_free(model->history_stat_str);
                for
                    M_EACH(item_menu, model->history->data, TPMSReceiverMenuItemArray_t) {
                        furi_string_free(item_menu->item_str);
                        item_menu->type = 0;
                    }
                TPMSReceiverMenuItemArray_clear(model->history->data);
                free(model->history);
        },
        false);
    furi_timer_free(tpms_receiver->lock_timer);
    furi_timer_free(tpms_receiver->relearn_timer);
    view_free(tpms_receiver->view);
    free(tpms_receiver);
}

View* tpms_view_receiver_get_view(TPMSReceiver* tpms_receiver) {
    furi_assert(tpms_receiver);
    return tpms_receiver->view;
}

uint16_t tpms_view_receiver_get_idx_menu(TPMSReceiver* tpms_receiver) {
    furi_assert(tpms_receiver);
    uint32_t idx = 0;
    with_view_model(
        tpms_receiver->view, TPMSReceiverModel * model, { idx = model->idx; }, false);
    return idx;
}

void tpms_view_receiver_set_idx_menu(TPMSReceiver* tpms_receiver, uint16_t idx) {
    furi_assert(tpms_receiver);
    with_view_model(
        tpms_receiver->view,
        TPMSReceiverModel * model,
        {
            model->idx = idx;
            if(model->idx > 2) model->list_offset = idx - 2;
        },
        true);
    tpms_view_receiver_update_offset(tpms_receiver);
}
