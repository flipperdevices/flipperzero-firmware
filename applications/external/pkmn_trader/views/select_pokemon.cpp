#include "../pokemon_app.h"
#include "select_pokemon.hpp"

static void select_pokemon_render_callback(Canvas* canvas, void* context) {
    canvas_clear(canvas);

    SelectPokemonModel* model = (SelectPokemonModel*)context;
    const uint8_t current_index = model->current_pokemon;
    char pokedex_num[5];

    snprintf(pokedex_num, sizeof(pokedex_num), "#%03d", current_index + 1);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 55, 54 / 2, AlignLeft, AlignTop, pokemon_table[current_index].name);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 55, 38, AlignLeft, AlignTop, pokedex_num);
    canvas_draw_icon(canvas, 0, 0, pokemon_table[current_index].icon);
    canvas_draw_icon(canvas, 128 - 80, 0, &I_Space_80x18);
    canvas_draw_str_aligned(canvas, (128 - 40), 5, AlignCenter, AlignTop, "Select Pokemon");

    canvas_set_font(canvas, FontPrimary);
    elements_button_center(canvas, "OK");
}

static bool select_pokemon_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    SelectPokemon* select_pokemon = (SelectPokemon*)context;
    bool consumed = false;

    if(event->type == InputTypePress && event->key == InputKeyOk) {
        with_view_model_cpp(
            select_pokemon->view,
            SelectPokemonModel*,
            model,
            {
                select_pokemon->app->current_pokemon = model->current_pokemon;
                select_pokemon->app->pokemon_hex_code = pokemon_table[model->current_pokemon].hex;
            },
            false);
        view_dispatcher_switch_to_view(select_pokemon->app->view_dispatcher, AppViewTrade);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyBack) {
        view_dispatcher_switch_to_view(select_pokemon->app->view_dispatcher, VIEW_NONE);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyLeft) {
        with_view_model_cpp(
            select_pokemon->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_pokemon == 0) {
                    model->current_pokemon = 150;
                } else {
                    model->current_pokemon--;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyDown) {
        with_view_model_cpp(
            select_pokemon->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_pokemon >= 10) {
                    model->current_pokemon -= 10;
                } else {
                    model->current_pokemon = 150;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyRight) {
        with_view_model_cpp(
            select_pokemon->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_pokemon == 150) {
                    model->current_pokemon = 0;
                } else {
                    model->current_pokemon++;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyUp) {
        with_view_model_cpp(
            select_pokemon->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_pokemon <= 140) {
                    model->current_pokemon += 10;
                } else {
                    model->current_pokemon = 0;
                    ;
                }
            },
            true);
        consumed = true;
    }

    return consumed;
}

void select_pokemon_enter_callback(void* context) {
    furi_assert(context);
    UNUSED(context);
}

bool select_pokemon_custom_callback(uint32_t event, void* context) {
    UNUSED(event);
    furi_assert(context);
    SelectPokemon* select_pokemon = (SelectPokemon*)context;
    view_dispatcher_send_custom_event(select_pokemon->app->view_dispatcher, 0);
    return true;
}

void select_pokemon_exit_callback(void* context) {
    furi_assert(context);
    UNUSED(context);
}

SelectPokemon* select_pokemon_alloc(App* app) {
    SelectPokemon* select_pokemon = (SelectPokemon*)malloc(sizeof(SelectPokemon));
    select_pokemon->app = app;
    select_pokemon->view = view_alloc();
    view_set_context(select_pokemon->view, select_pokemon);
    view_allocate_model(select_pokemon->view, ViewModelTypeLockFree, sizeof(SelectPokemonModel));
    with_view_model_cpp(
        select_pokemon->view,
        SelectPokemonModel*,
        model,
        {
            model->current_pokemon = app->current_pokemon;
            model->pokemon_hex_code = app->pokemon_hex_code;
        },
        true);

    view_set_draw_callback(select_pokemon->view, select_pokemon_render_callback);
    view_set_input_callback(select_pokemon->view, select_pokemon_input_callback);
    view_set_enter_callback(select_pokemon->view, select_pokemon_enter_callback);
    view_set_custom_callback(select_pokemon->view, select_pokemon_custom_callback);

    view_set_exit_callback(select_pokemon->view, select_pokemon_exit_callback);
    return select_pokemon;
}

void select_pokemon_free(App* app) {
    furi_assert(app->select_pokemon);
    view_free(app->select_pokemon->view);
    free(app->select_pokemon);
}

View* select_pokemon_get_view(App* app) {
    furi_assert(app->select_pokemon);
    return app->select_pokemon->view;
}
