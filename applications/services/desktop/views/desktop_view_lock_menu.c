#include <furi.h>
#include <gui/elements.h>
#include <assets_icons.h>

#include "../desktop_i.h"
#include "desktop_view_lock_menu.h"
#include <toolbox/value_index.h>

#define DESKTOP_LOCK_MENU_VISIBLE_ITEM_COUNT 3

typedef struct DesktopLockMenuItem DesktopLockMenuItem;

typedef FuriString* (*DesktopLockMenuItemGetTextCallback)(
    const DesktopLockMenuItem* item,
    DesktopLockMenuViewModel* model,
    bool is_selected);

typedef bool (*DesktopLockMenuItemOnInputCallback)(
    const DesktopLockMenuItem* item,
    const InputEvent* event,
    DesktopLockMenuView* view);

struct DesktopLockMenuItem {
    DesktopLockMenuItemGetTextCallback get_text;
    DesktopLockMenuItemOnInputCallback on_input;
};

FuriString* desktop_lock_menu_lock_get_text(
    const DesktopLockMenuItem* item,
    DesktopLockMenuViewModel* model,
    bool is_selected) {
    furi_assert(item);
    furi_assert(model);
    UNUSED(is_selected);

    return furi_string_alloc_set("Lock");
}

bool desktop_lock_menu_lock_on_input(
    const DesktopLockMenuItem* item,
    const InputEvent* event,
    DesktopLockMenuView* view) {
    furi_assert(item);
    furi_assert(event);
    furi_assert(view);

    if(event->key == InputKeyOk) {
        view->callback(DesktopLockMenuEventLock, view->context);
        return true;
    } else
        return false;
}

FuriString* desktop_lock_menu_stealth_get_text(
    const DesktopLockMenuItem* item,
    DesktopLockMenuViewModel* model,
    bool is_selected) {
    furi_assert(item);
    furi_assert(model);
    UNUSED(is_selected);

    if(model->stealth_mode)
        return furi_string_alloc_set("Unmute");
    else
        return furi_string_alloc_set("Mute");
}

bool desktop_lock_menu_stealth_on_input(
    const DesktopLockMenuItem* item,
    const InputEvent* event,
    DesktopLockMenuView* view) {
    furi_assert(item);
    furi_assert(event);
    furi_assert(view);

    if(event->key == InputKeyOk) {
        DesktopEvent event_type = DesktopLockMenuEventStealthModeOff;
        with_view_model(
            view->view,
            DesktopLockMenuViewModel * model,
            {
                if(!model->stealth_mode) event_type = DesktopLockMenuEventStealthModeOn;
            },
            false)

            view->callback(event_type, view->context);

        return true;
    } else
        return false;
}

FuriString* desktop_lock_menu_dummy_mode_get_text(
    const DesktopLockMenuItem* item,
    DesktopLockMenuViewModel* model,
    bool is_selected) {
    furi_assert(item);
    furi_assert(model);
    UNUSED(is_selected);

    if(model->dummy_mode)
        return furi_string_alloc_set("Default Mode");
    else
        return furi_string_alloc_set("Dummy Mode");
}

bool desktop_lock_menu_dummy_mode_on_input(
    const DesktopLockMenuItem* item,
    const InputEvent* event,
    DesktopLockMenuView* view) {
    furi_assert(item);
    furi_assert(event);
    furi_assert(view);

    if(event->key == InputKeyOk) {
        DesktopEvent event_type = DesktopLockMenuEventDummyModeOff;
        with_view_model(
            view->view,
            DesktopLockMenuViewModel * model,
            {
                if(!model->dummy_mode) event_type = DesktopLockMenuEventDummyModeOn;
            },
            false)

            view->callback(event_type, view->context);

        return true;
    } else
        return false;
}

FuriString* desktop_lock_menu_display_brightness_get_text(
    const DesktopLockMenuItem* item,
    DesktopLockMenuViewModel* model,
    bool is_selected) {
    furi_assert(item);
    furi_assert(model);
    UNUSED(is_selected);

    const char* value_string = backlight_text[model->display_brightness_index];
    FuriString* str = furi_string_alloc_set("Display ");

    if(model->display_brightness_index > 0)
        furi_string_cat(str, "< ");
    else
        furi_string_cat(str, "  ");

    furi_string_cat(str, value_string);

    if(model->display_brightness_index < BACKLIGHT_COUNT - 1)
        furi_string_cat(str, " >");
    else
        furi_string_cat(str, "  ");

    return str;
}

bool desktop_lock_menu_display_brightness_on_input(
    const DesktopLockMenuItem* item,
    const InputEvent* event,
    DesktopLockMenuView* view) {
    furi_assert(item);
    furi_assert(event);
    furi_assert(view);

    if(event->key == InputKeyLeft || event->key == InputKeyRight) {
        bool update = false;
        uint8_t index = 0;
        with_view_model(
            view->view,
            DesktopLockMenuViewModel * model,
            {
                if(event->key == InputKeyLeft &&
                   (int8_t)model->display_brightness_index - 1 >= 0) {
                    model->display_brightness_index--;
                    index = model->display_brightness_index;
                    update = true;
                } else if(
                    event->key == InputKeyRight &&
                    model->display_brightness_index + 1 < BACKLIGHT_COUNT) {
                    model->display_brightness_index++;
                    index = model->display_brightness_index;
                    update = true;
                }

                if(update) {
                    model->notification_settings_changed = true;
                }
            },
            update);

        if(update) {
            view->notification_record->settings.display_brightness = backlight_value[index];
            notification_message(view->notification_record, &sequence_display_backlight_on);
        }

        return true;
    }

    return false;
}

const DesktopLockMenuItem menu_items[] = {
    {
        .get_text = desktop_lock_menu_lock_get_text,
        .on_input = desktop_lock_menu_lock_on_input,
    },
    {
        .get_text = desktop_lock_menu_stealth_get_text,
        .on_input = desktop_lock_menu_stealth_on_input,
    },
    {
        .get_text = desktop_lock_menu_dummy_mode_get_text,
        .on_input = desktop_lock_menu_dummy_mode_on_input,
    },
    {
        .get_text = desktop_lock_menu_display_brightness_get_text,
        .on_input = desktop_lock_menu_display_brightness_on_input,
    },
};
#define DESKTOP_LOCK_MENU_ITEM_COUNT COUNT_OF(menu_items)

#define DESKTOP_LOCK_MENU_NEEDS_SCROLL_BAR \
    DESKTOP_LOCK_MENU_ITEM_COUNT > DESKTOP_LOCK_MENU_VISIBLE_ITEM_COUNT

void desktop_lock_menu_set_callback(
    DesktopLockMenuView* lock_menu,
    DesktopLockMenuViewCallback callback,
    void* context) {
    furi_assert(lock_menu);
    furi_assert(callback);
    lock_menu->callback = callback;
    lock_menu->context = context;
}

void desktop_lock_menu_set_dummy_mode_state(DesktopLockMenuView* lock_menu, bool dummy_mode) {
    with_view_model(
        lock_menu->view,
        DesktopLockMenuViewModel * model,
        { model->dummy_mode = dummy_mode; },
        true);
}

void desktop_lock_menu_set_stealth_mode_state(DesktopLockMenuView* lock_menu, bool stealth_mode) {
    with_view_model(
        lock_menu->view,
        DesktopLockMenuViewModel * model,
        { model->stealth_mode = stealth_mode; },
        true);
}

void desktop_lock_menu_set_idx(DesktopLockMenuView* lock_menu, uint8_t idx) {
    furi_assert(idx < DESKTOP_LOCK_MENU_ITEM_COUNT);
    with_view_model(
        lock_menu->view, DesktopLockMenuViewModel * model, { model->idx = idx; }, true);
}

void desktop_lock_menu_set_notification_record(
    DesktopLockMenuView* lock_menu,
    NotificationApp* notification) {
    furi_assert(lock_menu);
    furi_assert(notification);

    lock_menu->notification_record = notification;

    desktop_lock_menu_refresh_notification_values(lock_menu);
}

void desktop_lock_menu_refresh_notification_values(DesktopLockMenuView* lock_menu) {
    furi_assert(lock_menu);

    uint8_t index = value_index_float(
        lock_menu->notification_record->settings.display_brightness,
        backlight_value,
        BACKLIGHT_COUNT);
    with_view_model(
        lock_menu->view,
        DesktopLockMenuViewModel * model,
        { model->display_brightness_index = index; },
        true);
}

bool desktop_lock_menu_did_notification_settings_change(DesktopLockMenuView* lock_menu) {
    furi_assert(lock_menu);

    bool ret = false;
    with_view_model(lock_menu->view,
                    DesktopLockMenuViewModel * model,
                    ret = model->notification_settings_changed;
                    , false);
    return ret;
}

uint8_t desktop_lock_menu_get_first_visible_item(uint8_t idx) {
    if(DESKTOP_LOCK_MENU_NEEDS_SCROLL_BAR) {
        if(idx < 1) //first item is selected
            return 0;
        else if(
            idx > DESKTOP_LOCK_MENU_ITEM_COUNT - DESKTOP_LOCK_MENU_VISIBLE_ITEM_COUNT +
                      1) //last item is selected
            return DESKTOP_LOCK_MENU_ITEM_COUNT - DESKTOP_LOCK_MENU_VISIBLE_ITEM_COUNT;
        else //item is not the first or last item
            return idx - 1;
    } else {
        return 0;
    }
}

void desktop_lock_menu_draw_callback(Canvas* canvas, void* model) {
    DesktopLockMenuViewModel* m = model;

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(canvas, -57, 0 + STATUS_BAR_Y_SHIFT, &I_DoorLeft_70x55);
    canvas_draw_icon(canvas, 116, 0 + STATUS_BAR_Y_SHIFT, &I_DoorRight_70x55);
    canvas_set_font(canvas, FontSecondary);

    const uint8_t first_visible_item = desktop_lock_menu_get_first_visible_item(m->idx);

    for(size_t i = 0; i < DESKTOP_LOCK_MENU_VISIBLE_ITEM_COUNT; ++i) {
        const uint32_t menu_item_index = first_visible_item + i;
        const bool is_selected = m->idx == menu_item_index;
        const DesktopLockMenuItem* menu_item = &menu_items[menu_item_index];

        FuriString* text = menu_item->get_text(menu_item, model, is_selected);

        if(text != NULL) {
            canvas_draw_str_aligned(
                canvas,
                64,
                9 + (i * 17) + STATUS_BAR_Y_SHIFT,
                AlignCenter,
                AlignCenter,
                furi_string_get_cstr(text));

            furi_string_free(text);
        }

        if(is_selected)
            elements_frame(
                canvas,
                15,
                1 + (i * 17) + STATUS_BAR_Y_SHIFT,
                DESKTOP_LOCK_MENU_NEEDS_SCROLL_BAR ? 95 : 98,
                15);
    }

    if(DESKTOP_LOCK_MENU_NEEDS_SCROLL_BAR)
        elements_scrollbar_pos(
            canvas, 115, 1 + STATUS_BAR_Y_SHIFT, 51, m->idx, DESKTOP_LOCK_MENU_ITEM_COUNT);
}

View* desktop_lock_menu_get_view(DesktopLockMenuView* lock_menu) {
    furi_assert(lock_menu);
    return lock_menu->view;
}

bool desktop_lock_menu_input_callback(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    DesktopLockMenuView* lock_menu = context;
    uint8_t idx = 0;
    bool consumed = false;
    bool update = false;

    with_view_model(
        lock_menu->view,
        DesktopLockMenuViewModel * model,
        {
            if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                if(event->key == InputKeyUp) {
                    if(model->idx == 0) {
                        model->idx = DESKTOP_LOCK_MENU_ITEM_COUNT - 1;
                    } else {
                        model->idx =
                            CLAMP(model->idx - 1, (int8_t)DESKTOP_LOCK_MENU_ITEM_COUNT - 1, 0);
                    }
                    update = true;
                    consumed = true;
                } else if(event->key == InputKeyDown) {
                    if(model->idx == DESKTOP_LOCK_MENU_ITEM_COUNT - 1) {
                        model->idx = 0;
                    } else {
                        model->idx =
                            CLAMP(model->idx + 1, (int8_t)DESKTOP_LOCK_MENU_ITEM_COUNT - 1, 0);
                    }
                    update = true;
                    consumed = true;
                }
            }
            idx = model->idx;
        },
        update);

    if((event->type == InputTypeShort || event->type == InputTypeRepeat) &&
       (event->key == InputKeyOk || event->key == InputKeyLeft || event->key == InputKeyRight)) {
        const DesktopLockMenuItem* menu_item = &menu_items[idx];
        if(menu_item->on_input)
            consumed = menu_item->on_input(menu_item, event, lock_menu);
        else
            consumed = true;
    }

    return consumed;
}

DesktopLockMenuView* desktop_lock_menu_alloc(void) {
    DesktopLockMenuView* lock_menu = malloc(sizeof(DesktopLockMenuView));
    lock_menu->view = view_alloc();
    view_allocate_model(lock_menu->view, ViewModelTypeLocking, sizeof(DesktopLockMenuViewModel));
    view_set_context(lock_menu->view, lock_menu);
    view_set_draw_callback(lock_menu->view, (ViewDrawCallback)desktop_lock_menu_draw_callback);
    view_set_input_callback(lock_menu->view, desktop_lock_menu_input_callback);

    return lock_menu;
}

void desktop_lock_menu_free(DesktopLockMenuView* lock_menu_view) {
    furi_assert(lock_menu_view);

    view_free(lock_menu_view->view);
    free(lock_menu_view);
}
