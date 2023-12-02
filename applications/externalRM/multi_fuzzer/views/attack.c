#include "attack.h"
#include "../fuzzer_i.h"

#include <input/input.h>
#include <gui/elements.h>

#define ATTACK_SCENE_MAX_UID_LENGTH 25
#define UID_MAX_DISPLAYED_LEN (8U)
#define LEFT_RIGHT_OFFSET (3U)

#define LINE_1_Y (12U)
#define LINE_2_Y (24U)
#define LINE_3_Y (36U)
#define LINE_4_Y (48U)

struct FuzzerViewAttack {
    View* view;
    FuzzerViewAttackCallback callback;
    void* context;
};

typedef struct {
    uint8_t time_delay; // 1 = 100ms
    uint8_t time_delay_min; // 1 = 100ms
    uint8_t emu_time; // 1 = 100ms
    uint8_t emu_time_min; // 1 = 100ms
    bool td_emt_cursor; // false - time_delay, true - emu_time
    const char* attack_name;
    const char* protocol_name;
    FuzzerAttackState attack_state;
    FuriString* uid_str;
} FuzzerViewAttackModel;

void fuzzer_view_attack_reset_data(
    FuzzerViewAttack* view,
    const char* attack_name,
    const char* protocol_name) {
    furi_assert(view);

    with_view_model(
        view->view,
        FuzzerViewAttackModel * model,
        {
            model->attack_name = attack_name;
            model->protocol_name = protocol_name;
            model->attack_state = FuzzerAttackStateIdle;
            furi_string_set_str(model->uid_str, "Not_set");
        },
        true);
}

void fuzzer_view_attack_set_uid(FuzzerViewAttack* view, const FuzzerPayload* uid) {
    furi_assert(view);
    furi_assert(uid->data);

    with_view_model(
        view->view,
        FuzzerViewAttackModel * model,
        {
            furi_string_printf(model->uid_str, "%02X", uid->data[0]);
            for(uint8_t i = 1; i < uid->data_size; i++) {
                furi_string_cat_printf(model->uid_str, ":%02X", uid->data[i]);
            }
        },
        true);
}

void fuzzer_view_update_state(FuzzerViewAttack* view, FuzzerAttackState state) {
    furi_assert(view);

    with_view_model(
        view->view, FuzzerViewAttackModel * model, { model->attack_state = state; }, true);
}

void fuzzer_view_attack_set_callback(
    FuzzerViewAttack* view_attack,
    FuzzerViewAttackCallback callback,
    void* context) {
    furi_assert(view_attack);

    view_attack->callback = callback;
    view_attack->context = context;
}

static void
    fuzzer_view_attack_draw_time_delays_line(Canvas* canvas, FuzzerViewAttackModel* model) {
    char temp_str[25];
    uint16_t crt;
    const uint16_t y = LINE_2_Y;

    canvas_set_font(canvas, FontPrimary);

    if(!model->td_emt_cursor) {
        canvas_set_font(canvas, FontSecondary);
        snprintf(temp_str, sizeof(temp_str), "Time delay:");
        canvas_draw_str_aligned(canvas, LEFT_RIGHT_OFFSET, y, AlignLeft, AlignBottom, temp_str);
        crt = canvas_string_width(canvas, temp_str);

        canvas_set_font(canvas, FontPrimary);
        snprintf(
            temp_str, sizeof(temp_str), "%d.%d", model->time_delay / 10, model->time_delay % 10);
        canvas_draw_str_aligned(
            canvas, crt + LEFT_RIGHT_OFFSET + 3, y, AlignLeft, AlignBottom, temp_str);

        canvas_set_font(canvas, FontSecondary);
        snprintf(
            temp_str, sizeof(temp_str), "EmT: %d.%d", model->emu_time / 10, model->emu_time % 10);
        canvas_draw_str_aligned(
            canvas, 128 - LEFT_RIGHT_OFFSET, y, AlignRight, AlignBottom, temp_str);
    } else {
        canvas_set_font(canvas, FontSecondary);
        snprintf(
            temp_str,
            sizeof(temp_str),
            "TD: %d.%d",
            model->time_delay / 10,
            model->time_delay % 10);

        canvas_draw_str_aligned(canvas, LEFT_RIGHT_OFFSET, y, AlignLeft, AlignBottom, temp_str);

        canvas_set_font(canvas, FontPrimary);
        snprintf(temp_str, sizeof(temp_str), "%d.%d", model->emu_time / 10, model->emu_time % 10);
        canvas_draw_str_aligned(
            canvas, 128 - LEFT_RIGHT_OFFSET, y, AlignRight, AlignBottom, temp_str);
        crt = canvas_string_width(canvas, temp_str);

        canvas_set_font(canvas, FontSecondary);
        snprintf(temp_str, sizeof(temp_str), "Emulation time:");
        canvas_draw_str_aligned(
            canvas, 128 - LEFT_RIGHT_OFFSET - crt - 3, y, AlignRight, AlignBottom, temp_str);
    }
}

static void fuzzer_view_attack_draw_time_delays_str(Canvas* canvas, FuzzerViewAttackModel* model) {
    char temp_str[20];
    uint16_t crt;
    const uint16_t y = LINE_2_Y;

    canvas_set_font(canvas, FontSecondary);
    snprintf(
        temp_str,
        sizeof(temp_str),
        "TD: %d.%d Emt: %d.%d",
        model->time_delay / 10,
        model->time_delay % 10,
        model->emu_time / 10,
        model->emu_time % 10);

    crt = canvas_string_width(canvas, temp_str);

    canvas_draw_str_aligned(
        canvas, 128 - LEFT_RIGHT_OFFSET - crt, y, AlignLeft, AlignBottom, temp_str);
}

static void fuzzer_view_attack_draw_idle(Canvas* canvas, FuzzerViewAttackModel* model) {
    if(model->td_emt_cursor) {
        elements_button_center(canvas, "Start");
        elements_button_left(canvas, "EmT -");
        elements_button_right(canvas, "+ EmT");
    } else {
        elements_button_center(canvas, "Start");
        elements_button_left(canvas, "TD -");
        elements_button_right(canvas, "+ TD");
    }
}

static void fuzzer_view_attack_draw_running(Canvas* canvas, FuzzerViewAttackModel* model) {
    UNUSED(model);
    elements_button_left(canvas, "Stop");
    elements_button_center(canvas, "Pause");
}

static void fuzzer_view_attack_draw_end(Canvas* canvas, FuzzerViewAttackModel* model) {
    UNUSED(model);
    // elements_button_center(canvas, "Restart"); // Reset
    elements_button_left(canvas, "Exit");
}

void fuzzer_view_attack_draw(Canvas* canvas, FuzzerViewAttackModel* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    // Header - Attack name
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, LINE_1_Y, AlignCenter, AlignBottom, model->attack_name);

    // Time delays line or Status line
    switch(model->attack_state) {
    case FuzzerAttackStateIdle:
        fuzzer_view_attack_draw_time_delays_line(canvas, model);
        break;

    case FuzzerAttackStateAttacking:
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, LEFT_RIGHT_OFFSET, LINE_2_Y, "Attacking");
        fuzzer_view_attack_draw_time_delays_str(canvas, model);

        break;

    case FuzzerAttackStateEmulating:
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, LINE_2_Y, AlignCenter, AlignBottom, "Emulating:");

        break;

    case FuzzerAttackStatePause:
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, LEFT_RIGHT_OFFSET, LINE_2_Y, "Paused");

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_icon_ex(canvas, 62, LINE_2_Y - 9, &I_Pin_arrow_up_7x9, IconRotation180);
        canvas_draw_icon(canvas, 69, LINE_2_Y - 9, &I_Pin_arrow_up_7x9);
        canvas_draw_str(canvas, 79, LINE_2_Y, "Change UID");
        break;

    case FuzzerAttackStateEnd:
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, LINE_2_Y, AlignCenter, AlignBottom, "Attack is over");

        break;

    default:
        break;
    }

    // Protocol name
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, LINE_3_Y, AlignCenter, AlignBottom, model->protocol_name);

    // Current UID
    canvas_set_font(canvas, FontPrimary);
    if(128 < canvas_string_width(canvas, furi_string_get_cstr(model->uid_str))) {
        canvas_set_font(canvas, FontSecondary);
    }
    canvas_draw_str_aligned(
        canvas, 64, LINE_4_Y, AlignCenter, AlignBottom, furi_string_get_cstr(model->uid_str));

    // Btns
    canvas_set_font(canvas, FontSecondary);
    if(model->attack_state == FuzzerAttackStateAttacking ||
       model->attack_state == FuzzerAttackStateEmulating) {
        fuzzer_view_attack_draw_running(canvas, model);
    } else if(model->attack_state == FuzzerAttackStateIdle) {
        fuzzer_view_attack_draw_idle(canvas, model);
    } else if(model->attack_state == FuzzerAttackStatePause) {
        elements_button_left(canvas, "Back");
        elements_button_right(canvas, "Save");
        elements_button_center(canvas, "Emu");
    } else if(model->attack_state == FuzzerAttackStateEnd) {
        fuzzer_view_attack_draw_end(canvas, model);
    }
}

static bool fuzzer_view_attack_input_idle(
    FuzzerViewAttack* view_attack,
    InputEvent* event,
    FuzzerViewAttackModel* model) {
    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        view_attack->callback(FuzzerCustomEventViewAttackExit, view_attack->context);
        return true;
    } else if(event->key == InputKeyOk && event->type == InputTypeShort) {
        view_attack->callback(FuzzerCustomEventViewAttackRunAttack, view_attack->context);
        return true;
    } else if(event->key == InputKeyLeft) {
        if(!model->td_emt_cursor) {
            // TimeDelay --
            if(event->type == InputTypeShort) {
                if(model->time_delay > model->time_delay_min) {
                    model->time_delay--;
                }
            } else if(event->type == InputTypeLong || event->type == InputTypeRepeat) {
                if((model->time_delay - 10) >= model->time_delay_min) {
                    model->time_delay -= 10;
                } else {
                    model->time_delay = model->time_delay_min;
                }
            }
        } else {
            // EmuTime --
            if(event->type == InputTypeShort) {
                if(model->emu_time > model->emu_time_min) {
                    model->emu_time--;
                }
            } else if(event->type == InputTypeLong || event->type == InputTypeRepeat) {
                if((model->emu_time - 10) >= model->emu_time_min) {
                    model->emu_time -= 10;
                } else {
                    model->emu_time = model->emu_time_min;
                }
            }
        }
        return true;
    } else if(event->key == InputKeyRight) {
        if(!model->td_emt_cursor) {
            // TimeDelay ++
            if(event->type == InputTypeShort) {
                if(model->time_delay < FUZZ_TIME_DELAY_MAX) {
                    model->time_delay++;
                }
            } else if(event->type == InputTypeLong || event->type == InputTypeRepeat) {
                model->time_delay += 10;
                if(model->time_delay > FUZZ_TIME_DELAY_MAX) {
                    model->time_delay = FUZZ_TIME_DELAY_MAX;
                }
            }
        } else {
            // EmuTime ++
            if(event->type == InputTypeShort) {
                if(model->emu_time < FUZZ_TIME_DELAY_MAX) {
                    model->emu_time++;
                }
            } else if(event->type == InputTypeLong || event->type == InputTypeRepeat) {
                model->emu_time += 10;
                if(model->emu_time > FUZZ_TIME_DELAY_MAX) {
                    model->emu_time = FUZZ_TIME_DELAY_MAX;
                }
            }
        }
        return true;
    } else if(
        (event->key == InputKeyUp || event->key == InputKeyDown) &&
        event->type == InputTypeShort) {
        with_view_model(
            view_attack->view,
            FuzzerViewAttackModel * model,
            { model->td_emt_cursor = !model->td_emt_cursor; },
            true);
        return true;
    }
    return true;
}

static bool fuzzer_view_attack_input_end(
    FuzzerViewAttack* view_attack,
    InputEvent* event,
    FuzzerViewAttackModel* model) {
    UNUSED(model);
    if((event->key == InputKeyBack || event->key == InputKeyLeft) &&
       event->type == InputTypeShort) {
        // Exit if Ended
        view_attack->callback(FuzzerCustomEventViewAttackExit, view_attack->context);
    }
    return true;
}

bool fuzzer_view_attack_input(InputEvent* event, void* context) {
    furi_assert(context);
    FuzzerViewAttack* view_attack = context;

    // if(event->key == InputKeyBack && event->type == InputTypeShort) {
    //     view_attack->callback(FuzzerCustomEventViewAttackBack, view_attack->context);
    //     return true;
    // } else if(event->key == InputKeyOk && event->type == InputTypeShort) {
    //     view_attack->callback(FuzzerCustomEventViewAttackOk, view_attack->context);
    //     return true;
    // } else
    // {
    with_view_model(
        view_attack->view,
        FuzzerViewAttackModel * model,
        {
            switch(model->attack_state) {
            case FuzzerAttackStateIdle:
                fuzzer_view_attack_input_idle(view_attack, event, model);
                break;

            case FuzzerAttackStateEnd:
                fuzzer_view_attack_input_end(view_attack, event, model);
                break;

            case FuzzerAttackStateAttacking:
            case FuzzerAttackStateEmulating:
                if((event->key == InputKeyBack || event->key == InputKeyLeft) &&
                   event->type == InputTypeShort) {
                    view_attack->callback(FuzzerCustomEventViewAttackIdle, view_attack->context);
                } else if(event->key == InputKeyOk && event->type == InputTypeShort) {
                    view_attack->callback(FuzzerCustomEventViewAttackPause, view_attack->context);
                }
                break;

            case FuzzerAttackStatePause:
                if((event->key == InputKeyBack || event->key == InputKeyLeft) &&
                   event->type == InputTypeShort) {
                    view_attack->callback(FuzzerCustomEventViewAttackIdle, view_attack->context);
                } else if(event->key == InputKeyRight && event->type == InputTypeShort) {
                    view_attack->callback(FuzzerCustomEventViewAttackSave, view_attack->context);
                } else if(event->key == InputKeyOk && event->type == InputTypeShort) {
                    view_attack->callback(
                        FuzzerCustomEventViewAttackEmulateCurrent, view_attack->context);
                } else if(event->key == InputKeyUp && event->type == InputTypeShort) {
                    view_attack->callback(
                        FuzzerCustomEventViewAttackPrevUid, view_attack->context);
                } else if(event->key == InputKeyDown && event->type == InputTypeShort) {
                    view_attack->callback(
                        FuzzerCustomEventViewAttackNextUid, view_attack->context);
                }
                break;

            default:
                break;
            }
        },
        true);
    // }

    return true;
}

void fuzzer_view_attack_enter(void* context) {
    furi_assert(context);
}

void fuzzer_view_attack_exit(void* context) {
    furi_assert(context);
    FuzzerViewAttack* view_attack = context;
    with_view_model(
        view_attack->view, FuzzerViewAttackModel * model, { model->td_emt_cursor = false; }, true);
}

FuzzerViewAttack* fuzzer_view_attack_alloc() {
    if(fuzzer_proto_get_max_data_size() > UID_MAX_DISPLAYED_LEN) {
        furi_crash("Maximum of displayed bytes exceeded");
    }

    FuzzerViewAttack* view_attack = malloc(sizeof(FuzzerViewAttack));

    // View allocation and configuration
    view_attack->view = view_alloc();
    view_allocate_model(view_attack->view, ViewModelTypeLocking, sizeof(FuzzerViewAttackModel));
    view_set_context(view_attack->view, view_attack);
    view_set_draw_callback(view_attack->view, (ViewDrawCallback)fuzzer_view_attack_draw);
    view_set_input_callback(view_attack->view, fuzzer_view_attack_input);
    view_set_enter_callback(view_attack->view, fuzzer_view_attack_enter);
    view_set_exit_callback(view_attack->view, fuzzer_view_attack_exit);

    with_view_model(
        view_attack->view,
        FuzzerViewAttackModel * model,
        {
            model->time_delay = fuzzer_proto_get_def_idle_time();
            model->time_delay_min = 0; // model->time_delay;

            model->emu_time = fuzzer_proto_get_def_emu_time();

            model->emu_time_min = 2; // model->emu_time;

            model->uid_str = furi_string_alloc_set_str("Not_set");
            // malloc(ATTACK_SCENE_MAX_UID_LENGTH + 1);
            model->attack_state = FuzzerAttackStateOff;
            model->td_emt_cursor = false;

            // strcpy(model->uid_str, "Not_set");
            model->attack_name = "Not_set";
            model->protocol_name = "Not_set";
        },
        true);
    return view_attack;
}

void fuzzer_view_attack_free(FuzzerViewAttack* view_attack) {
    furi_assert(view_attack);

    with_view_model(
        view_attack->view,
        FuzzerViewAttackModel * model,
        { furi_string_free(model->uid_str); },
        true);
    view_free(view_attack->view);
    free(view_attack);
}

View* fuzzer_view_attack_get_view(FuzzerViewAttack* view_attack) {
    furi_assert(view_attack);
    return view_attack->view;
}

uint8_t fuzzer_view_attack_get_time_delay(FuzzerViewAttack* view) {
    furi_assert(view);
    uint8_t time_delay;

    with_view_model(
        view->view, FuzzerViewAttackModel * model, { time_delay = model->time_delay; }, false);

    return time_delay;
}

uint8_t fuzzer_view_attack_get_emu_time(FuzzerViewAttack* view) {
    furi_assert(view);
    uint8_t emu_time;

    with_view_model(
        view->view, FuzzerViewAttackModel * model, { emu_time = model->emu_time; }, false);

    return emu_time;
}