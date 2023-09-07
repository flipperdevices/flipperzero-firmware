#include "../pokemon_app.h"
#include "select_move4.hpp"

static void select_move4_render_callback(Canvas* canvas, void* context) {
    canvas_clear(canvas);

    SelectPokemonModel* model = (SelectPokemonModel*)context;
    const uint8_t current_index = model->current_move;
    char move_num[5];

    snprintf(move_num, sizeof(move_num), "#%03d", current_index + 1);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 55, 54 / 2, AlignLeft, AlignTop, move_table[current_index].name);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 55, 38, AlignLeft, AlignTop, move_num);
    canvas_draw_icon(canvas, 0, 0, pokemon_table[model->current_pokemon].icon);
    canvas_draw_icon(canvas, 128 - 80, 0, &I_Space_80x18);
    canvas_draw_str_aligned(canvas, (128 - 40), 5, AlignCenter, AlignTop, "Select Move #4");

    canvas_set_font(canvas, FontPrimary);
    elements_button_center(canvas, "OK");
}

static bool select_move4_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    SelectMove4* select_move4 = (SelectMove4*)context;
    bool consumed = false;

    if(event->type == InputTypePress && event->key == InputKeyOk) {
        with_view_model_cpp(
            select_move4->view,
            SelectPokemonModel*,
            model,
            {
                select_move4->app->current_move = model->current_move;
                select_move4->app->move4_hex_code = move_table[model->current_move].hex;
            },
            false);
        view_dispatcher_switch_to_view(select_move4->app->view_dispatcher, AppViewTrade);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyBack) {
        view_dispatcher_switch_to_view(select_move4->app->view_dispatcher, AppViewSelectMove3);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyLeft) {
        with_view_model_cpp(
            select_move4->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_move == 0) {
                    model->current_move = 165;
                } else {
                    model->current_move--;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyDown) {
        with_view_model_cpp(
            select_move4->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_move >= 10) {
                    model->current_move -= 10;
                } else {
                    model->current_move = 165;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyRight) {
        with_view_model_cpp(
            select_move4->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_move == 165) {
                    model->current_move = 0;
                } else {
                    model->current_move++;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyUp) {
        with_view_model_cpp(
            select_move4->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_move <= 155) {
                    model->current_move += 10;
                } else {
                    model->current_move = 0;
                    ;
                }
            },
            true);
        consumed = true;
    }

    return consumed;
}

void select_move4_enter_callback(void* context) {
    furi_assert(context);
    SelectMove4* select_move4 = (SelectMove4*)context;
    with_view_model_cpp(
        select_move4->view,
        SelectPokemonModel*,
        model,
        {
            model->current_pokemon = select_move4->app->current_pokemon;
            model->pokemon_hex_code = select_move4->app->pokemon_hex_code;
            model->current_level = select_move4->app->current_level;
            model->current_stats = select_move4->app->current_stats;
            model->move1_hex_code = select_move4->app->move1_hex_code;
            model->move2_hex_code = select_move4->app->move2_hex_code;
            model->move3_hex_code = select_move4->app->move3_hex_code;
        },
        true);
}

bool select_move4_custom_callback(uint32_t event, void* context) {
    UNUSED(event);
    furi_assert(context);
    SelectMove4* select_move4 = (SelectMove4*)context;
    view_dispatcher_send_custom_event(select_move4->app->view_dispatcher, 0);
    return true;
}

void select_move4_exit_callback(void* context) {
    furi_assert(context);
    UNUSED(context);
}

SelectMove4* select_move4_alloc(App* app) {
    SelectMove4* select_move4 = (SelectMove4*)malloc(sizeof(SelectMove4));
    select_move4->app = app;
    select_move4->view = view_alloc();
    view_set_context(select_move4->view, select_move4);
    view_allocate_model(select_move4->view, ViewModelTypeLockFree, sizeof(SelectPokemonModel));
    with_view_model_cpp(
        select_move4->view,
        SelectPokemonModel*,
        model,
        {
            model->current_move = app->current_move;
            model->move4_hex_code = app->move4_hex_code;
        },
        true);

    view_set_draw_callback(select_move4->view, select_move4_render_callback);
    view_set_input_callback(select_move4->view, select_move4_input_callback);
    view_set_enter_callback(select_move4->view, select_move4_enter_callback);
    view_set_custom_callback(select_move4->view, select_move4_custom_callback);

    view_set_exit_callback(select_move4->view, select_move4_exit_callback);
    return select_move4;
}

void select_move4_free(App* app) {
    furi_assert(app->select_move4);
    view_free(app->select_move4->view);
    free(app->select_move4);
}

View* select_move4_get_view(App* app) {
    furi_assert(app->select_move4);
    return app->select_move4->view;
}
