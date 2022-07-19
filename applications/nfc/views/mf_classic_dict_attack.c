#include "mf_classic_dict_attack.h"

#include <m-string.h>
#include <gui/elements.h>

typedef enum {
    MfClassicDictAttackStateRead,
    MfClassicDictAttackStateCardRemoved,
} MfClassicDictAttackState;

struct MfClassicDictAttack {
    View* view;
    MfClassicDictAttackCallback callback;
    void* context;
};

typedef struct {
    MfClassicDictAttackState state;
    MfClassicType type;
    string_t header;
    uint8_t sectors_total;
    uint8_t sectors_read;
    uint8_t sector_current;
    uint8_t keys_total;
    uint8_t keys_found;
} MfClassicDictAttackViewModel;

static void mf_classic_dict_attack_draw_callback(Canvas* canvas, void* model) {
    MfClassicDictAttackViewModel* m = model;
    if(m->state == MfClassicDictAttackStateCardRemoved) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(
            canvas, 64, 32, AlignCenter, AlignTop, "Place card back to flipper");
    } else if(m->state == MfClassicDictAttackStateRead) {
        char draw_str[32] = {};
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 2, AlignCenter, AlignTop, string_get_cstr(m->header));
        canvas_set_font(canvas, FontSecondary);
        float progress =
            m->sectors_total == 0 ? 0 : (float)(m->sector_current) / (float)(m->sectors_total);
        elements_progress_bar(canvas, 5, 12, 120, progress);
        canvas_set_font(canvas, FontSecondary);
        snprintf(
            draw_str, sizeof(draw_str), "Sectors read: %d/%d", m->sectors_read, m->sectors_total);
        canvas_draw_str_aligned(canvas, 1, 23, AlignLeft, AlignTop, draw_str);
        snprintf(draw_str, sizeof(draw_str), "Keys found: %d/%d", m->keys_found, m->keys_total);
        canvas_draw_str_aligned(canvas, 1, 35, AlignLeft, AlignTop, draw_str);
    }
    elements_button_center(canvas, "Skip");
}

static bool mf_classic_dict_attack_input_callback(InputEvent* event, void* context) {
    MfClassicDictAttack* dict_attack = context;
    bool consumed = false;
    if(event->type == InputTypeShort && event->key == InputKeyOk) {
        if(dict_attack->callback) {
            dict_attack->callback(dict_attack->context);
        }
        consumed = true;
    }
    return consumed;
}

MfClassicDictAttack* mf_classic_dict_attack_alloc() {
    MfClassicDictAttack* dict_attack = malloc(sizeof(MfClassicDictAttack));
    dict_attack->view = view_alloc();
    view_allocate_model(
        dict_attack->view, ViewModelTypeLocking, sizeof(MfClassicDictAttackViewModel));
    view_set_draw_callback(dict_attack->view, mf_classic_dict_attack_draw_callback);
    view_set_input_callback(dict_attack->view, mf_classic_dict_attack_input_callback);
    view_set_context(dict_attack->view, dict_attack);
    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            string_init(model->header);
            return false;
        });
    return dict_attack;
}

void mf_classic_dict_attack_free(MfClassicDictAttack* dict_attack) {
    furi_assert(dict_attack);
    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            string_clear(model->header);
            return false;
        });
    view_free(dict_attack->view);
    free(dict_attack);
}

void mf_classic_dict_attack_reset(MfClassicDictAttack* dict_attack) {
    furi_assert(dict_attack);
    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            model->state = MfClassicDictAttackStateRead;
            model->type = MfClassicType1k;
            model->sectors_total = 0;
            model->sectors_read = 0;
            model->sector_current = 0;
            model->keys_total = 0;
            model->keys_found = 0;
            string_reset(model->header);
            return false;
        });
}

View* mf_classic_dict_attack_get_view(MfClassicDictAttack* dict_attack) {
    furi_assert(dict_attack);
    return dict_attack->view;
}

void mf_classic_dict_attack_set_callback(
    MfClassicDictAttack* dict_attack,
    MfClassicDictAttackCallback callback,
    void* context) {
    furi_assert(dict_attack);
    furi_assert(callback);
    dict_attack->callback = callback;
    dict_attack->context = context;
}

void mf_classic_dict_attack_set_header(MfClassicDictAttack* dict_attack, const char* header) {
    furi_assert(dict_attack);
    furi_assert(header);

    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            string_set_str(model->header, header);
            return true;
        });
}

void mf_classic_dict_attack_set_card_detected(MfClassicDictAttack* dict_attack, MfClassicType type) {
    furi_assert(dict_attack);
    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            model->state = MfClassicDictAttackStateRead;
            model->sectors_total = mf_classic_get_total_sectors_num(type);
            model->keys_total = model->sectors_total * 2;
            return true;
        });
}

void mf_classic_dict_attack_set_card_removed(MfClassicDictAttack* dict_attack) {
    furi_assert(dict_attack);
    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            model->state = MfClassicDictAttackStateCardRemoved;
            return true;
        });
}

void mf_classic_dict_attack_set_sector_read(MfClassicDictAttack* dict_attack, uint8_t sec_read) {
    furi_assert(dict_attack);
    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            model->sectors_read = sec_read;
            return true;
        });
}

void mf_classic_dict_attack_set_keys_found(MfClassicDictAttack* dict_attack, uint8_t keys_found) {
    furi_assert(dict_attack);
    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            model->keys_found = keys_found;
            return true;
        });
}

void mf_classic_dict_attack_set_current_sector(MfClassicDictAttack* dict_attack, uint8_t curr_sec) {
    furi_assert(dict_attack);
    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            model->sector_current = curr_sec;
            return true;
        });
}

void mf_classic_dict_attack_inc_current_sector(MfClassicDictAttack* dict_attack) {
    furi_assert(dict_attack);
    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            if(model->sector_current < model->sectors_total) {
                model->sector_current++;
            }
            return true;
        });
}

void mf_classic_dict_attack_inc_keys_found(MfClassicDictAttack* dict_attack) {
    furi_assert(dict_attack);
    with_view_model(
        dict_attack->view, (MfClassicDictAttackViewModel * model) {
            if(model->keys_found < model->keys_total) {
                model->keys_found++;
            }
            return true;
        });
}
