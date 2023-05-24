#include "../wiegand.h"

void wiegand_add_info_4bit_8bit(FuriString* buffer) {
    if(bit_count == 8) {
        for(int i = 0; i < 4; i++) {
            furi_string_cat_printf(
                buffer, "\nbit %d: %d %d (bit %d)", i, data[i], data[i + 4], i + 4);
            if(data[i] == data[i + 4]) {
                furi_string_cat_printf(buffer, " - ERROR");
            } else {
                furi_string_cat_printf(buffer, " - OK");
            }
        }
    }
    if(bit_count == 4 || bit_count == 8) {
        int code = 0;
        int offset = bit_count == 4 ? 0 : 4;
        for(int i = 0; i < 4; i++) {
            code = code << 1;
            code += data[i + offset] ? 1 : 0;
        }
        if(code <= 9) {
            furi_string_cat_printf(buffer, "\nButton: %d", code);
        } else if(code == 10) {
            furi_string_cat_printf(buffer, "\nButton: Escape");
        } else if(code == 11) {
            furi_string_cat_printf(buffer, "\nButton: Enter");
        }
    }
}

void wiegand_add_info_26bit(FuriString* buffer) {
    // 26 bit wiegand, the first bit is the even parity bit, which is
    // based on the next 12 bits.  The number of bits that are a 1 should
    // be even.
    int parity = 0;
    if(data[0]) {
        parity = 1;
    }
    for(int i = 1; i < 13; i++) {
        if(data[i]) {
            parity++;
        }
    }
    if(parity % 2 == 0) {
        furi_string_cat_printf(buffer, "\nEven Parity (%d): OK", parity);
    } else {
        furi_string_cat_printf(buffer, "\nEven Parity (%d): ERROR", parity);
    }

    // After the parity bit, the next 8 bits are the facility code.
    // Then the next 16 bits are the card id .
    furi_string_cat_printf(buffer, "\nFacility: 0x");
    int code = 0;
    int count = 0;
    for(int i = 1; i < 25; i++) {
        code = code << 1;
        code |= data[i] ? 1 : 0;
        if(++count % 4 == 0) {
            furi_string_cat_printf(buffer, "%X", code);
            code = 0;
        }
        // Parity, then 8 bit facility code, then id.
        if(i == 9) {
            furi_string_cat_printf(buffer, "\nId: 0x");
        }
    }

    if(data[13]) {
        parity = 1;
    } else {
        parity = 0;
    }
    for(int i = 14; i < 26; i++) {
        if(data[i]) {
            parity++;
        }
    }
    if(parity % 2 == 0) {
        furi_string_cat_printf(buffer, "\nOdd Parity (%d): ERROR", parity);
    } else {
        furi_string_cat_printf(buffer, "\nOdd Parity (%d): OK", parity);
    }
}

void wiegand_add_info_24bit(FuriString* buffer) {
    // 24 bit wiegand (no parity info).

    // The First 8 bits are the facility code.
    // Then the next 16 bits are the card id.
    furi_string_cat_printf(buffer, "\nFacility: 0x");
    int code = 0;
    int count = 0;
    for(int i = 0; i < 24; i++) {
        code = code << 1;
        code |= data[i] ? 1 : 0;
        if(++count % 4 == 0) {
            furi_string_cat_printf(buffer, "%X", code);
            code = 0;
        }
        // The first 8 bits are facility code, then comes id.
        if(i == 8) {
            furi_string_cat_printf(buffer, "\nId: 0x");
        }
    }
}

void wiegand_add_info(FuriString* buffer) {
    furi_string_push_back(buffer, '\n');
    if(bit_count == 4 || bit_count == 8) {
        wiegand_add_info_4bit_8bit(buffer);
    } else if(bit_count == 26) {
        wiegand_add_info_26bit(buffer);
    } else if(bit_count == 24) {
        wiegand_add_info_24bit(buffer);
    }
    furi_string_push_back(buffer, '\n');
}

void wiegand_button_callback(GuiButtonType result, InputType type, void* context) {
    App* app = context;
    if(type == InputTypeShort && result == GuiButtonTypeLeft) {
        view_dispatcher_send_custom_event(app->view_dispatcher, WiegandDataSceneSaveButtonEvent);
    } else if(type == InputTypeShort && result == GuiButtonTypeCenter) {
        view_dispatcher_send_custom_event(app->view_dispatcher, WiegandDataScenePlayButtonEvent);
    }
}

void wiegand_data_scene_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);
    widget_add_string_element(app->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "Wiegand Data");
    FuriString* buffer = furi_string_alloc(1024);
    furi_string_printf(buffer, "Bits: %d\n", bit_count);
    for(int i = 0; i < bit_count; i++) {
        furi_string_push_back(buffer, data[i] ? '1' : '0');
        if((bit_count - i - 1) % 22 == 21) {
            furi_string_push_back(buffer, '\n');
        }
    }
    furi_string_cat_printf(buffer, "\nPulse: %ld us", (data_rise[0] - data_fall[0]) / 64);
    furi_string_cat_printf(buffer, "\nPeriod: %ld us", (data_fall[1] - data_fall[0]) / 64);
    wiegand_add_info(buffer);
    for(int i = 0; i < bit_count;) {
        uint32_t pulse = (data_rise[i] - data_fall[i]) / 64;
        i++;
        uint32_t period = (i < bit_count) ? (data_fall[i] - data_fall[i - 1]) / 64 : 0;
        furi_string_cat_printf(
            buffer, "\n%c : %ld us, %ld us", data[i] ? '1' : '0', pulse, period);
    }
    widget_add_text_scroll_element(app->widget, 0, 12, 128, 34, furi_string_get_cstr(buffer));
    if(!data_saved) {
        widget_add_button_element(
            app->widget, GuiButtonTypeLeft, "Save", wiegand_button_callback, app);
    }
    widget_add_button_element(
        app->widget, GuiButtonTypeCenter, "Play", wiegand_button_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, WiegandWidgetView);
}

bool wiegand_data_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case WiegandDataScenePlayButtonEvent:
            wiegand_play();
            consumed = true;
            break;
        case WiegandDataSceneSaveButtonEvent:
            scene_manager_next_scene(app->scene_manager, WiegandSaveScene);
            consumed = true;
            break;
        default:
            consumed = false;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}
