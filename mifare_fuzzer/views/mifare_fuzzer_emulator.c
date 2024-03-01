#include "mifare_fuzzer_emulator.h"

#define TAG "MifareFuzzerApp_emulator_view"

// Screen is 128 × 64 pixels

/// @brief mifare_fuzzer_emulator_set_callback
/// @param mifare_fuzzer_emulator
/// @param callback
/// @param context
void mifare_fuzzer_emulator_set_callback(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    MifareFuzzerEmulatorCallback callback,
    void* context) {
    furi_assert(mifare_fuzzer_emulator);
    furi_assert(callback);

    mifare_fuzzer_emulator->callback = callback;
    mifare_fuzzer_emulator->context = context;
}

/// @brief mifare_fuzzer_emulator_draw_callback
/// @param canvas
/// @param _model
static void mifare_fuzzer_emulator_draw_callback(Canvas* canvas, void* _model) {
    MifareFuzzerEmulatorModel* model = _model;
    FuriString* furi_string = furi_string_alloc();

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    // Primary font
    canvas_set_font(canvas, FontPrimary);
    // Title
    canvas_draw_str(canvas, 4, 11, model->title);

    // Emulated UID
    uint8_t cpos;
    char uid[25];
    char uid_char[3];
    cpos = 0;
    for(uint8_t i = 0; i < model->nfc_data.uid_len; i++) {
        if(i > 0) {
            uid[cpos] = ':';
            cpos++;
        }
        snprintf(uid_char, sizeof(uid_char), "%02X", model->nfc_data.uid[i]);
        uid[cpos] = uid_char[0];
        cpos++;
        uid[cpos] = uid_char[1];
        cpos++;
    }
    uid[cpos] = 0x00;
    canvas_draw_str_aligned(canvas, 128 / 2, 43, AlignCenter, AlignCenter, uid);

    // Secondary font
    canvas_set_font(canvas, FontSecondary);
    // Card
    canvas_draw_str(canvas, 4, 22, "c:");
    canvas_draw_str(canvas, 15, 22, model->mifare_card_dsc);
    // Timing
    furi_string_printf(furi_string, "%d", model->ticks_between_cards);
    canvas_draw_str(canvas, 100, 33, "t:");
    canvas_draw_str(canvas, 110, 33, furi_string_get_cstr(furi_string));
    // Attack
    canvas_draw_str(canvas, 4, 33, "a:");
    canvas_draw_str(canvas, 15, 33, model->attack_dsc);

    if(!model->is_attacking) {
        elements_button_left(canvas, "t-1");
        elements_button_center(canvas, "Start");
        elements_button_right(canvas, "t+1");
    } else {
        canvas_draw_line(canvas, 1, 49, (128 * model->tick_num / model->ticks_between_cards), 49);
        elements_button_center(canvas, "Stop");
    }

    // Free temp string
    furi_string_free(furi_string);
}

/// @brief mifare_fuzzer_emulator_input_callback
/// @param event
/// @param context
/// @return
static bool mifare_fuzzer_emulator_input_callback(InputEvent* event, void* context) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_emulator_input_callback()");
    furi_assert(context);
    MifareFuzzerEmulator* mifare_fuzzer_emulator = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight) {
            if(!mifare_fuzzer_emulator->is_attacking) {
                mifare_fuzzer_emulator->callback(
                    MifareFuzzerEventIncrementTicks, mifare_fuzzer_emulator->context);
            };
            consumed = true;
        } else if(event->key == InputKeyLeft) {
            if(!mifare_fuzzer_emulator->is_attacking) {
                mifare_fuzzer_emulator->callback(
                    MifareFuzzerEventDecrementTicks, mifare_fuzzer_emulator->context);
            };
            consumed = true;
        } else if(event->key == InputKeyUp) {
            consumed = true;
        } else if(event->key == InputKeyDown) {
            consumed = true;
        } else if(event->key == InputKeyOk) {
            // Toggle attack
            if(mifare_fuzzer_emulator->is_attacking) {
                mifare_fuzzer_emulator->is_attacking = false;
                mifare_fuzzer_emulator->callback(
                    MifareFuzzerEventStopAttack, mifare_fuzzer_emulator->context);
            } else {
                mifare_fuzzer_emulator->is_attacking = true;
                mifare_fuzzer_emulator->callback(
                    MifareFuzzerEventStartAttack, mifare_fuzzer_emulator->context);
            }

            with_view_model(
                mifare_fuzzer_emulator->view,
                MifareFuzzerEmulatorModel * model,
                { model->is_attacking = mifare_fuzzer_emulator->is_attacking; },
                true);

            consumed = true;
        }
    }

    return consumed;
}

/// @brief mifare_fuzzer_emulator_enter_callback
/// @param context
static void mifare_fuzzer_emulator_enter_callback(void* context) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_emulator_enter_callback()");
    furi_assert(context);
    MifareFuzzerEmulator* mifare_fuzzer_emulator = context;

    //UNUSED(mifare_fuzzer_emulator);
    mifare_fuzzer_emulator->is_attacking = false;
    with_view_model(
        mifare_fuzzer_emulator->view,
        MifareFuzzerEmulatorModel * model,
        { model->is_attacking = false; },
        true);
}

/// @brief mifare_fuzzer_emulator_alloc
/// @return
MifareFuzzerEmulator* mifare_fuzzer_emulator_alloc() {
    MifareFuzzerEmulator* mifare_fuzzer_emulator = malloc(sizeof(MifareFuzzerEmulator));
    mifare_fuzzer_emulator->view = view_alloc();
    view_set_context(mifare_fuzzer_emulator->view, mifare_fuzzer_emulator);
    view_allocate_model(
        mifare_fuzzer_emulator->view, ViewModelTypeLocking, sizeof(MifareFuzzerEmulatorModel));
    view_set_draw_callback(mifare_fuzzer_emulator->view, mifare_fuzzer_emulator_draw_callback);
    view_set_input_callback(mifare_fuzzer_emulator->view, mifare_fuzzer_emulator_input_callback);
    view_set_enter_callback(mifare_fuzzer_emulator->view, mifare_fuzzer_emulator_enter_callback);

    with_view_model(
        mifare_fuzzer_emulator->view,
        MifareFuzzerEmulatorModel * model,
        { model->title = "Mifare Fuzzer (emulator)"; },
        true);

    return mifare_fuzzer_emulator;
}

/// @brief mifare_fuzzer_emulator_free
/// @param context
void mifare_fuzzer_emulator_free(MifareFuzzerEmulator* context) {
    //FURI_LOG_D(TAG, "mifare_fuzzer_emulator_free()");
    furi_assert(context);
    MifareFuzzerEmulator* mifare_fuzzer_emulator = context;
    /*
    with_view_model(
        mifare_fuzzer_emulator->view,
        MifareFuzzerEmulatorModel* model,
        {
            UNUSED(model);
        },
        true
    );
    */

    view_free(mifare_fuzzer_emulator->view);
    free(mifare_fuzzer_emulator);
}

/// @brief mifare_fuzzer_emulator_get_view
/// @param mifare_fuzzer_emulator
/// @return
View* mifare_fuzzer_emulator_get_view(MifareFuzzerEmulator* mifare_fuzzer_emulator) {
    furi_assert(mifare_fuzzer_emulator);
    return mifare_fuzzer_emulator->view;
}

/// @brief Set card type
/// @param mifare_fuzzer_emulator
/// @param mifare_card
void mifare_fuzzer_emulator_set_card(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    MifareCard mifare_card,
    FuriString* name) {
    furi_assert(mifare_fuzzer_emulator);
    furi_assert(mifare_card);

    with_view_model(
        mifare_fuzzer_emulator->view,
        MifareFuzzerEmulatorModel * model,
        {
            model->mifare_card = mifare_card;
            if(name == NULL) {
                switch(mifare_card) {
                case MifareCardClassic1k:
                    model->mifare_card_dsc = "Classic 1k";
                    break;
                case MifareCardClassic4k:
                    model->mifare_card_dsc = "Classic 4k";
                    break;
                case MifareCardUltralight:
                    model->mifare_card_dsc = "Ultralight";
                    break;
                }
            } else {
                FuriString* card_name = furi_string_alloc_set(name);
                size_t filename_start = furi_string_search_rchar(card_name, '/') + 1;
                furi_string_right(card_name, filename_start);
                model->mifare_card_dsc = furi_string_get_cstr(card_name);
            }
        },
        true);
}

/// @brief Set attack type
/// @param mifare_fuzzer_emulator
/// @param mifare_attack
void mifare_fuzzer_emulator_set_attack(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    MifareFuzzerAttack mifare_attack) {
    furi_assert(mifare_fuzzer_emulator);
    furi_assert(mifare_attack);

    with_view_model(
        mifare_fuzzer_emulator->view,
        MifareFuzzerEmulatorModel * model,
        {
            model->attack = mifare_attack;
            switch(mifare_attack) {
            case MifareFuzzerAttackTestValues:
                model->attack_dsc = "Test values";
                break;
            case MifareFuzzerAttackRandomValues:
                model->attack_dsc = "Random values";
                break;
            case MifareFuzzerAttackLoadUidsFromFile:
                model->attack_dsc = "Load Uids From File";
                break;
            }
        },
        true);
}

/// @brief mifare_fuzzer_emulator_set_nfc_dev_data
/// @param mifare_fuzzer_emulator
/// @param nfc_dev_data
void mifare_fuzzer_emulator_set_nfc_data(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    Iso14443_3aData nfc_data) {
    furi_assert(mifare_fuzzer_emulator);

    with_view_model(
        mifare_fuzzer_emulator->view,
        MifareFuzzerEmulatorModel * model,
        { model->nfc_data = nfc_data; },
        true);
}

/// @brief mifare_fuzzer_emulator_set_ticks_between_cards
/// @param mifare_fuzzer_emulator
/// @param ticks
void mifare_fuzzer_emulator_set_ticks_between_cards(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    uint8_t ticks) {
    furi_assert(mifare_fuzzer_emulator);

    with_view_model(
        mifare_fuzzer_emulator->view,
        MifareFuzzerEmulatorModel * model,
        { model->ticks_between_cards = ticks; },
        true);
}

/// @brief mifare_fuzzer_emulator_set_tick_num
/// @param mifare_fuzzer_emulator
/// @param tick_num
void mifare_fuzzer_emulator_set_tick_num(
    MifareFuzzerEmulator* mifare_fuzzer_emulator,
    uint8_t tick_num) {
    furi_assert(mifare_fuzzer_emulator);

    with_view_model(
        mifare_fuzzer_emulator->view,
        MifareFuzzerEmulatorModel * model,
        { model->tick_num = tick_num; },
        true);
}
