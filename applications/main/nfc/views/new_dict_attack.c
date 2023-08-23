#include "new_dict_attack.h"

#include <gui/elements.h>

struct NewDictAttack {
    View* view;
    NewDictAttackCallback callback;
    void* context;
};

typedef struct {
    FuriString* header;
    bool card_detected;
    size_t sectors_total;
    size_t sectors_read;
    size_t current_sector;
    size_t keys_total;
    size_t keys_found;
    size_t dict_keys_total;
    size_t dict_keys_current;
    bool is_key_attack;
    size_t key_attack_current_sector;
} NewDictAttackViewModel;

static void new_dict_attack_draw_callback(Canvas* canvas, void* model) {
    NewDictAttackViewModel* m = model;
    if(!m->card_detected) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 4, AlignCenter, AlignTop, "Lost the tag!");
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text_aligned(
            canvas, 64, 23, AlignCenter, AlignTop, "Make sure the tag is\npositioned correctly.");
    } else {
        char draw_str[32] = {};
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(
            canvas, 64, 0, AlignCenter, AlignTop, furi_string_get_cstr(m->header));
        if(m->is_key_attack) {
            snprintf(
                draw_str,
                sizeof(draw_str),
                "Reuse key check for sector: %d",
                m->key_attack_current_sector);
        } else {
            snprintf(draw_str, sizeof(draw_str), "Unlocking sector: %d", m->current_sector);
        }
        canvas_draw_str_aligned(canvas, 0, 10, AlignLeft, AlignTop, draw_str);
        float dict_progress = m->dict_keys_total == 0 ?
                                  0 :
                                  (float)(m->dict_keys_current) / (float)(m->dict_keys_total);
        float progress = m->sectors_total == 0 ? 0 :
                                                 ((float)(m->current_sector) + dict_progress) /
                                                     (float)(m->sectors_total);
        if(progress > 1.0) {
            progress = 1.0;
        }
        if(m->dict_keys_current == 0) {
            // Cause when people see 0 they think it's broken
            snprintf(draw_str, sizeof(draw_str), "%d/%d", 1, m->dict_keys_total);
        } else {
            snprintf(
                draw_str, sizeof(draw_str), "%d/%d", m->dict_keys_current, m->dict_keys_total);
        }
        elements_progress_bar_with_text(canvas, 0, 20, 128, dict_progress, draw_str);
        canvas_set_font(canvas, FontSecondary);
        snprintf(draw_str, sizeof(draw_str), "Keys found: %d/%d", m->keys_found, m->keys_total);
        canvas_draw_str_aligned(canvas, 0, 33, AlignLeft, AlignTop, draw_str);
        snprintf(
            draw_str, sizeof(draw_str), "Sectors Read: %d/%d", m->sectors_read, m->sectors_total);
        canvas_draw_str_aligned(canvas, 0, 43, AlignLeft, AlignTop, draw_str);
    }
    elements_button_center(canvas, "Skip");
}

static bool new_dict_attack_input_callback(InputEvent* event, void* context) {
    NewDictAttack* instance = context;
    bool consumed = false;

    if(event->type == InputTypeShort && event->key == InputKeyOk) {
        if(instance->callback) {
            instance->callback(NewDictAttackEventSkipPressed, instance->context);
        }
        consumed = true;
    }

    return consumed;
}

NewDictAttack* new_dict_attack_alloc() {
    NewDictAttack* instance = malloc(sizeof(NewDictAttack));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(NewDictAttackViewModel));
    view_set_draw_callback(instance->view, new_dict_attack_draw_callback);
    view_set_input_callback(instance->view, new_dict_attack_input_callback);
    view_set_context(instance->view, instance);
    with_view_model(
        instance->view,
        NewDictAttackViewModel * model,
        { model->header = furi_string_alloc(); },
        false);

    return instance;
}

void new_dict_attack_free(NewDictAttack* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        NewDictAttackViewModel * model,
        { furi_string_free(model->header); },
        false);

    view_free(instance->view);
    free(instance);
}

void new_dict_attack_reset(NewDictAttack* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        NewDictAttackViewModel * model,
        {
            model->card_detected = false;
            model->sectors_total = 0;
            model->sectors_read = 0;
            model->current_sector = 0;
            model->keys_total = 0;
            model->keys_found = 0;
            model->dict_keys_total = 0;
            model->dict_keys_current = 0;
            model->is_key_attack = false;
            furi_string_reset(model->header);
        },
        false);
}

View* new_dict_attack_get_view(NewDictAttack* instance) {
    furi_assert(instance);

    return instance->view;
}

void new_dict_attack_set_callback(
    NewDictAttack* instance,
    NewDictAttackCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

void new_dict_attack_set_header(NewDictAttack* instance, const char* header) {
    furi_assert(instance);
    furi_assert(header);

    with_view_model(
        instance->view,
        NewDictAttackViewModel * model,
        { furi_string_set(model->header, header); },
        true);
}

void new_dict_attack_set_card_state(NewDictAttack* instance, bool detected) {
    furi_assert(instance);

    with_view_model(
        instance->view, NewDictAttackViewModel * model, { model->card_detected = detected; }, true);
}

void new_dict_attack_set_sectors_read(NewDictAttack* instance, size_t sectors_read) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        NewDictAttackViewModel * model,
        { model->sectors_read = sectors_read; },
        true);
}

void new_dict_attack_set_keys_found(NewDictAttack* instance, size_t keys_found) {
    furi_assert(instance);

    with_view_model(
        instance->view, NewDictAttackViewModel * model, { model->keys_found = keys_found; }, true);
}

void new_dict_attack_set_current_sector(NewDictAttack* instance, size_t current_sector) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        NewDictAttackViewModel * model,
        { model->current_sector = current_sector; },
        true);
}

void new_dict_attack_set_total_dict_keys(NewDictAttack* instance, size_t dict_keys_total) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        NewDictAttackViewModel * model,
        { model->dict_keys_total = dict_keys_total; },
        true);
}

void new_dict_attack_set_current_dict_key(NewDictAttack* instance, size_t cur_key_num) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        NewDictAttackViewModel * model,
        { model->dict_keys_current = cur_key_num; },
        true);
}

void new_dict_attack_set_key_attack(NewDictAttack* instance, size_t sector) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        NewDictAttackViewModel * model,
        {
            model->is_key_attack = true;
            model->key_attack_current_sector = sector;
        },
        true);
}

void new_dict_attack_reset_key_attack(NewDictAttack* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, NewDictAttackViewModel * model, { model->is_key_attack = false; }, true);
}
