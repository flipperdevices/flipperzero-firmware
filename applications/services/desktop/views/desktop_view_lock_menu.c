#include <furi.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include <assets_icons.h>
#include <cfw.h>
#include <furi_hal_rtc.h>

#include "../desktop_i.h"
#include "desktop_view_lock_menu.h"
#include "applications/settings/desktop_settings/desktop_settings_app.h"

#define LOCK_MENU_ITEMS_NB 5

static void desktop_view_lock_menu_dumbmode_changed(bool isThisGameMode) {
    DesktopSettingsApp* app = malloc(sizeof(DesktopSettingsApp));
    DESKTOP_SETTINGS_LOAD(&app->settings);
    app->settings.is_dumbmode = isThisGameMode;
    DESKTOP_SETTINGS_SAVE(&app->settings);
}

static const NotificationSequence sequence_note_c = {
    &message_note_c5,
    &message_delay_100,
    &message_sound_off,
    NULL,
};

typedef enum {
    DesktopLockMenuIndexLefthandedMode,
    DesktopLockMenuIndexStealth,
    DesktopLockMenuIndexDarkMode,
    DesktopLockMenuIndexLock,
    DesktopLockMenuIndexBluetooth,
    DesktopLockMenuIndexDummy,
    DesktopLockMenuIndexBrightness,
    DesktopLockMenuIndexVolume,

    DesktopLockMenuIndexTotalCount
} DesktopLockMenuIndex;

typedef enum {
    DesktopLockMenuIndexBasicLock,
    DesktopLockMenuIndexBasicLockShutdown,
    DesktopLockMenuIndexBasicGameMode,
    DesktopLockMenuIndexBasicStealth,
    DesktopLockMenuIndexBasicDummy,

    DesktopLockMenuIndexBasicTotalCount
} DesktopLockMenuIndexBasic;

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
    furi_assert(idx < DesktopLockMenuIndexTotalCount);
    with_view_model(
        lock_menu->view, DesktopLockMenuViewModel * model, { model->idx = idx; }, true);
}

void desktop_lock_menu_draw_callback(Canvas* canvas, void* model) {
    DesktopLockMenuViewModel* m = model;

    if(CFW_SETTINGS()->lock_menu_type) {
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontBatteryPercent);

        int x, y, w, h;
        bool selected, toggle;
        bool enabled = false;
        uint value = 0;
        int total = 58;
        const Icon* icon = NULL;
        for(size_t i = 0; i < DesktopLockMenuIndexTotalCount; ++i) {
            selected = m->idx == i;
            toggle = i < 6;
            if(toggle) {
                x = 2 + 32 * (i / 2);
                y = 2 + 32 * (i % 2);
                w = 28;
                h = 28;
                enabled = false;
            } else {
                x = 98 + 16 * (i % 2);
                y = 2;
                w = 12;
                h = 60;
                value = 0;
            }

            switch(i) {
            case DesktopLockMenuIndexLefthandedMode:
                icon = &I_CC_LefthandedMode_16x16;
                enabled = furi_hal_rtc_is_flag_set(FuriHalRtcFlagHandOrient);
                break;
            case DesktopLockMenuIndexStealth:
                icon = &I_CC_Stealth_16x16;
                enabled = m->stealth_mode;
                break;
            case DesktopLockMenuIndexDarkMode:
                icon = &I_CC_DarkMode_16x16;
                enabled = CFW_SETTINGS()->dark_mode;
                break;
            case DesktopLockMenuIndexLock:
                icon = &I_CC_Lock_16x16;
                break;
            case DesktopLockMenuIndexBluetooth:
                icon = &I_CC_Bluetooth_16x16;
                enabled = m->lock_menu->bt->bt_settings.enabled;
                break;
            case DesktopLockMenuIndexDummy:
                icon = &I_CC_Dummy_16x16;
                enabled = m->dummy_mode;
                break;
            case DesktopLockMenuIndexBrightness:
                icon = &I_Pin_star_7x7;
                value = total - m->lock_menu->notification->settings.display_brightness * total;
                break;
            case DesktopLockMenuIndexVolume:
                icon = m->stealth_mode ? &I_Muted_8x8 : &I_Volup_8x6;
                value = total - m->lock_menu->notification->settings.speaker_volume * total;
                break;
            default:
                break;
            }

            if(icon != NULL) {
                if(selected) {
                    elements_bold_rounded_frame(canvas, x - 1, y - 1, w + 1, h + 1);
                } else {
                    canvas_draw_rframe(canvas, x, y, w, h, 5);
                }
                if(toggle) {
                    if(enabled) {
                        canvas_draw_rbox(canvas, x, y, w, h, 5);
                        canvas_set_color(canvas, ColorWhite);
                    }
                    canvas_draw_icon(
                        canvas,
                        x + (w - icon_get_width(icon)) / 2,
                        y + (h - icon_get_height(icon)) / 2,
                        icon);
                    if(enabled) {
                        canvas_set_color(canvas, ColorBlack);
                    }
                } else {
                    canvas_draw_icon(
                        canvas,
                        x + (w - icon_get_width(icon)) / 2,
                        y + (h - icon_get_height(icon)) / 2,
                        icon);
                    canvas_set_color(canvas, ColorXOR);
                    canvas_draw_box(canvas, x + 1, y + 1 + value, w - 2, h - 2 - value);
                    if(selected) {
                        canvas_set_color(canvas, ColorBlack);
                    } else {
                        canvas_set_color(canvas, ColorWhite);
                    }
                    canvas_draw_dot(canvas, x + 1, y + 1);
                    canvas_draw_dot(canvas, x + 1, y + h - 2);
                    canvas_draw_dot(canvas, x + w - 2, y + 1);
                    canvas_draw_dot(canvas, x + w - 2, y + h - 2);
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_rframe(canvas, x, y, w, h, 5);
                }
            }
        }

        if(m->show_lock_menu) {
            canvas_set_font(canvas, FontSecondary);
            elements_bold_rounded_frame(canvas, 24, 4, 80, 56);
            canvas_draw_str_aligned(canvas, 64, 16, AlignCenter, AlignCenter, "Lock");
            canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, "Lock + OFF");
            canvas_draw_str_aligned(canvas, 64, 48, AlignCenter, AlignCenter, "Game Mode");
            elements_frame(canvas, 28, 8 + m->pin_lock * 16, 72, 15);
        }
    } else {
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon(canvas, -57, 0 + STATUS_BAR_Y_SHIFT, &I_DoorLeft_70x55);
        canvas_draw_icon(canvas, 116, 0 + STATUS_BAR_Y_SHIFT, &I_DoorRight_70x55);
        canvas_set_font(canvas, FontBatteryPercent);

        for(size_t i = 0; i < DesktopLockMenuIndexBasicTotalCount; ++i) {
            char* str = NULL;

            switch(i) {
            case DesktopLockMenuIndexBasicLock:
                str = "Lock";
                break;
            case DesktopLockMenuIndexBasicLockShutdown:
                str = "Lock + Off";
                break;
            case DesktopLockMenuIndexBasicGameMode:
                str = "Game Mode";
                break;
            case DesktopLockMenuIndexBasicStealth:
                if(m->stealth_mode) {
                    str = "Sound Mode";
                } else {
                    str = "Stealth Mode";
                }
                break;
            case DesktopLockMenuIndexBasicDummy:
                if(m->dummy_mode) {
                    str = "Brainiac Mode";
                } else {
                    str = "Dummy Mode";
                }
                break;
            }

            if(str) //-V547
            {
                canvas_draw_str_aligned(
                    canvas,
                    64,
                    9 + (((i - m->idx) + 1) * 12) + STATUS_BAR_Y_SHIFT,
                    AlignCenter,
                    AlignCenter,
                    str);

                if(m->idx == i) {
                    elements_frame(
                        canvas, 15, 1 + (((i - m->idx) + 1) * 12) + STATUS_BAR_Y_SHIFT, 98, 15);
                }
            }
        }
    }
}

View* desktop_lock_menu_get_view(DesktopLockMenuView* lock_menu) {
    furi_assert(lock_menu);
    return lock_menu->view;
}

bool desktop_lock_menu_input_callback(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    if(CFW_SETTINGS()->lock_menu_type) {
        DesktopLockMenuView* lock_menu = context;
        uint8_t idx = 0;
        int pin_lock = 0;
        bool dummy_mode = false;
        bool show_lock_menu = false;
        bool stealth_mode = false;
        bool consumed = true;

        with_view_model(
            lock_menu->view,
            DesktopLockMenuViewModel * model,
            {
                show_lock_menu = model->show_lock_menu;
                stealth_mode = model->stealth_mode;
                if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                    if(model->show_lock_menu) {
                        if(event->key == InputKeyUp) {
                            model->pin_lock--;
                            if(model->pin_lock < 0) {
                                model->pin_lock = 2;
                            }
                        } else if(event->key == InputKeyDown) {
                            model->pin_lock++;
                            if(model->pin_lock > 2) {
                                model->pin_lock = 0;
                            }
                        } else if(event->key == InputKeyBack || event->key == InputKeyOk) {
                            model->show_lock_menu = false;
                        }
                    } else {
                        if(model->idx == DesktopLockMenuIndexLock && event->key == InputKeyOk) {
                            model->show_lock_menu = true;
                        } else if(model->idx < 6) {
                            if(event->key == InputKeyUp || event->key == InputKeyDown) {
                                if(model->idx % 2) {
                                    model->idx--;
                                } else {
                                    model->idx++;
                                }
                            } else if(event->key == InputKeyLeft) {
                                if(model->idx < 2) {
                                    model->idx = 7;
                                } else {
                                    model->idx -= 2;
                                }
                            } else if(event->key == InputKeyRight) {
                                if(model->idx >= 4) {
                                    model->idx = 6;
                                } else {
                                    model->idx += 2;
                                }
                            }
                        } else {
                            if(event->key == InputKeyLeft) {
                                model->idx--;
                            } else if(event->key == InputKeyRight) {
                                if(model->idx >= 7) {
                                    model->idx = 1;
                                } else {
                                    model->idx++;
                                }
                            }
                        }
                    }
                }
                idx = model->idx;
                pin_lock = model->pin_lock;
                dummy_mode = model->dummy_mode;
                stealth_mode = model->stealth_mode;
            },
            true);

        DesktopEvent desktop_event = 0;
        if(show_lock_menu) {
            if(event->key == InputKeyOk && event->type == InputTypeShort) {
                switch(pin_lock) {
                case 0:
                    desktop_event = DesktopLockMenuEventLock;
                    break;
                case 1:
                    desktop_event = DesktopLockMenuEventLockShutdown;
                    break;
                case 2:
                    dolphin_deed(getRandomDeed());
                    desktop_view_lock_menu_dumbmode_changed(1);
                    desktop_event = DesktopLockMenuEventExit;
                    break;
                default:
                    break;
                }
            }
        } else {
            if(event->key == InputKeyBack) {
                consumed = false;
            } else if(event->key == InputKeyOk && event->type == InputTypeShort) {
                switch(idx) {
                case DesktopLockMenuIndexLefthandedMode:
                    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagHandOrient)) {
                        furi_hal_rtc_reset_flag(FuriHalRtcFlagHandOrient);
                    } else {
                        furi_hal_rtc_set_flag(FuriHalRtcFlagHandOrient);
                    }
                    break;
                case DesktopLockMenuIndexStealth:
                    desktop_event = stealth_mode ? DesktopLockMenuEventStealthModeOff :
                                                   DesktopLockMenuEventStealthModeOn;
                    break;
                case DesktopLockMenuIndexDarkMode:
                    CFW_SETTINGS()->dark_mode = !CFW_SETTINGS()->dark_mode;
                    lock_menu->save_cfw = true;
                    break;
                case DesktopLockMenuIndexDummy:
                    dolphin_deed(getRandomDeed());
                    desktop_event = dummy_mode ? DesktopLockMenuEventDummyModeOff :
                                                 DesktopLockMenuEventDummyModeOn;
                    break;
                case DesktopLockMenuIndexBluetooth:
                    lock_menu->bt->bt_settings.enabled = !lock_menu->bt->bt_settings.enabled;
                    if(lock_menu->bt->bt_settings.enabled) {
                        furi_hal_bt_start_advertising();
                    } else {
                        furi_hal_bt_stop_advertising();
                    }
                    lock_menu->save_bt = true;
                    break;
                case DesktopLockMenuIndexVolume:
                    desktop_event = stealth_mode ? DesktopLockMenuEventStealthModeOff :
                                                   DesktopLockMenuEventStealthModeOn;
                    break;
                default:
                    break;
                }
            } else if(idx >= 6 && (event->type == InputTypeShort || event->type == InputTypeRepeat)) {
                int8_t offset = 0;
                if(event->key == InputKeyUp) {
                    offset = 1;
                } else if(event->key == InputKeyDown) {
                    offset = -1;
                }
                if(offset) {
                    float value;
                    switch(idx) {
                    case DesktopLockMenuIndexBrightness:
                        value =
                            lock_menu->notification->settings.display_brightness + 0.05 * offset;
                        lock_menu->notification->settings.display_brightness =
                            value < 0.00f ? 0.00f : (value > 1.00f ? 1.00f : value);
                        lock_menu->save_notification = true;
                        notification_message(
                            lock_menu->notification, &sequence_display_backlight_on);
                        break;
                    case DesktopLockMenuIndexVolume:
                        value = lock_menu->notification->settings.speaker_volume + 0.05 * offset;
                        lock_menu->notification->settings.speaker_volume =
                            value < 0.00f ? 0.00f : (value > 1.00f ? 1.00f : value);
                        lock_menu->save_notification = true;
                        notification_message(lock_menu->notification, &sequence_note_c);
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        if(desktop_event) {
            lock_menu->callback(desktop_event, lock_menu->context);
        }

        return consumed;
    } else {
        DesktopLockMenuView* lock_menu = context;
        uint8_t idx = 0;
        bool consumed = false;
        bool dummy_mode = false;
        bool stealth_mode = false;
        bool update = false;

        with_view_model(
            lock_menu->view,
            DesktopLockMenuViewModel * model,
            {
                if((event->type == InputTypeShort) || (event->type == InputTypeRepeat)) {
                    if(event->key == InputKeyUp) {
                        if(model->idx == 0) {
                            model->idx = DesktopLockMenuIndexBasicTotalCount - 1;
                        } else {
                            model->idx =
                                CLAMP(model->idx - 1, DesktopLockMenuIndexBasicTotalCount - 1, 0);
                        }
                        update = true;
                        consumed = true;
                    } else if(event->key == InputKeyDown) {
                        if(model->idx == DesktopLockMenuIndexBasicTotalCount - 1) {
                            model->idx = 0;
                        } else {
                            model->idx =
                                CLAMP(model->idx + 1, DesktopLockMenuIndexBasicTotalCount - 1, 0);
                        }
                        update = true;
                        consumed = true;
                    }
                }
                idx = model->idx;
                dummy_mode = model->dummy_mode;
                stealth_mode = model->stealth_mode;
            },
            update);

        if(event->key == InputKeyOk) {
            if((idx == DesktopLockMenuIndexBasicLock)) {
                if(event->type == InputTypeShort) {
                    lock_menu->callback(DesktopLockMenuEventLock, lock_menu->context);
                }
            } else if(
                (idx == DesktopLockMenuIndexBasicLockShutdown) &&
                (event->type == InputTypeShort)) {
                lock_menu->callback(DesktopLockMenuEventLockShutdown, lock_menu->context);
            } else if((idx == DesktopLockMenuIndexBasicGameMode) && (event->type == InputTypeShort)) {
                dolphin_deed(getRandomDeed());
                desktop_view_lock_menu_dumbmode_changed(1);
                lock_menu->callback(DesktopLockMenuEventExit, lock_menu->context);
            } else if(idx == DesktopLockMenuIndexBasicStealth) {
                if((stealth_mode == false) && (event->type == InputTypeShort)) {
                    lock_menu->callback(DesktopLockMenuEventStealthModeOn, lock_menu->context);
                } else if((stealth_mode == true) && (event->type == InputTypeShort)) {
                    lock_menu->callback(DesktopLockMenuEventStealthModeOff, lock_menu->context);
                }
            } else if(idx == DesktopLockMenuIndexBasicDummy) {
                dolphin_deed(getRandomDeed());
                if((dummy_mode == false) && (event->type == InputTypeShort)) {
                    lock_menu->callback(DesktopLockMenuEventDummyModeOn, lock_menu->context);
                } else if((dummy_mode == true) && (event->type == InputTypeShort)) {
                    lock_menu->callback(DesktopLockMenuEventDummyModeOff, lock_menu->context);
                }
            }
            consumed = true;
        }

        return consumed;
    }
}

DesktopLockMenuView* desktop_lock_menu_alloc() {
    DesktopLockMenuView* lock_menu = malloc(sizeof(DesktopLockMenuView));
    lock_menu->bt = furi_record_open(RECORD_BT);
    lock_menu->notification = furi_record_open(RECORD_NOTIFICATION);
    lock_menu->view = view_alloc();
    view_allocate_model(lock_menu->view, ViewModelTypeLocking, sizeof(DesktopLockMenuViewModel));
    with_view_model(
        lock_menu->view,
        DesktopLockMenuViewModel * model,
        { model->lock_menu = lock_menu; },
        false);
    view_set_context(lock_menu->view, lock_menu);
    view_set_draw_callback(lock_menu->view, (ViewDrawCallback)desktop_lock_menu_draw_callback);
    view_set_input_callback(lock_menu->view, desktop_lock_menu_input_callback);

    return lock_menu;
}

void desktop_lock_menu_free(DesktopLockMenuView* lock_menu_view) {
    furi_assert(lock_menu_view);

    view_free(lock_menu_view->view);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_BT);
    free(lock_menu_view);
}