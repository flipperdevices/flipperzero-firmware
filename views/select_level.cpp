#include "../pokemon_app.h"
#include "select_level.hpp"

static void select_level_render_callback(Canvas* canvas, void* context) {
    canvas_clear(canvas);

    SelectPokemonModel* model = (SelectPokemonModel*)context;
    const uint8_t current_index = model->current_level;

    char level_num[5];

    snprintf(level_num, sizeof(level_num), "#%03d", current_index);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 55, 54 / 2, AlignLeft, AlignTop, "Level:");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 55, 38, AlignLeft, AlignTop, level_num);
    canvas_draw_icon(canvas, 0, 0, pokemon_table[model->current_pokemon].icon);
    canvas_draw_icon(canvas, 128 - 80, 0, &I_Space_80x18);
    canvas_draw_str_aligned(canvas, (128 - 40), 5, AlignCenter, AlignTop, "Select Level");

    canvas_set_font(canvas, FontPrimary);
    elements_button_center(canvas, "OK");
}

static bool select_level_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    SelectLevel* select_level = (SelectLevel*)context;
    bool consumed = false;

    if(event->type == InputTypePress && event->key == InputKeyOk) {
        with_view_model_cpp(
            select_level->view,
            SelectPokemonModel*,
            model,
            {
                select_level->app->current_level = model->current_level;
            },
            false);
        
        view_dispatcher_switch_to_view(select_level->app->view_dispatcher, AppViewSelectStats);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyBack) {
        view_dispatcher_switch_to_view(select_level->app->view_dispatcher, AppViewSelectPokemon);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyLeft) {
        with_view_model_cpp(
            select_level->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_level == 3) {
                    model->current_level = 100;
                } else {
                    model->current_level--;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyDown) {
        with_view_model_cpp(
            select_level->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_level >= 10) {
                    model->current_level -= 10;
                } else {
                    model->current_level = 100;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyRight) {
        with_view_model_cpp(
            select_level->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_level == 100) {
                    model->current_level = 3;
                } else {
                    model->current_level++;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyUp) {
        with_view_model_cpp(
            select_level->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_level <= 90) {
                    model->current_level += 10;
                } else {
                    model->current_level = 3;
                    ;
                }
            },
            true);
        consumed = true;
    }

    return consumed;
}

void select_level_enter_callback(void* context) {
    furi_assert(context);
    SelectLevel* select_level = (SelectLevel*)context;
    with_view_model_cpp(
        select_level->view,
        SelectPokemonModel*,
        model,
        {
            model->current_pokemon = select_level->app->current_pokemon;
            model->pokemon_hex_code = select_level->app->pokemon_hex_code;
            model->current_level = select_level->app->current_level;
        },
        true);
}

bool select_level_custom_callback(uint32_t event, void* context) {
    UNUSED(event);
    furi_assert(context);
    SelectLevel* select_level = (SelectLevel*)context;
    view_dispatcher_send_custom_event(select_level->app->view_dispatcher, 0);
    return true;
}

void select_level_exit_callback(void* context) {
    furi_assert(context);
    UNUSED(context);
}

SelectLevel* select_level_alloc(App* app) {
    SelectLevel* select_level = (SelectLevel*)malloc(sizeof(SelectLevel));
    select_level->app = app;
    select_level->view = view_alloc();
    view_set_context(select_level->view, select_level);
    view_allocate_model(select_level->view, ViewModelTypeLockFree, sizeof(SelectPokemonModel));
    with_view_model_cpp(
        select_level->view,
        SelectPokemonModel*,
        model,
        {
            model->current_level = app->current_level;
        },
        true);

    view_set_draw_callback(select_level->view, select_level_render_callback);
    view_set_input_callback(select_level->view, select_level_input_callback);
    view_set_enter_callback(select_level->view, select_level_enter_callback);
    view_set_custom_callback(select_level->view, select_level_custom_callback);

    view_set_exit_callback(select_level->view, select_level_exit_callback);
    return select_level;
}

void select_level_free(App* app) {
    furi_assert(app->select_level);
    view_free(app->select_level->view);
    free(app->select_level);
}

View* select_level_get_view(App* app) {
    furi_assert(app->select_level);
    return app->select_level->view;
}
