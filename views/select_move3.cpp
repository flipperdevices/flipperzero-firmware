#include "../pokemon_app.h"
#include "select_move3.hpp"

static void select_move3_render_callback(Canvas* canvas, void* context) {
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
    canvas_draw_str_aligned(canvas, (128 - 40), 5, AlignCenter, AlignTop, "Select Move #3");

    canvas_set_font(canvas, FontPrimary);
    elements_button_center(canvas, "OK");
}

static bool select_move3_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    SelectMove3* select_move3 = (SelectMove3*)context;
    bool consumed = false;

    if(event->type == InputTypePress && event->key == InputKeyOk) {
        with_view_model_cpp(
            select_move3->view,
            SelectPokemonModel*,
            model,
            {
                select_move3->app->current_move = model->current_move;
                select_move3->app->move3_hex_code = move_table[model->current_move].hex;
            },
            false);
        view_dispatcher_switch_to_view(select_move3->app->view_dispatcher, AppViewSelectMove4);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyBack) {
        view_dispatcher_switch_to_view(select_move3->app->view_dispatcher, AppViewSelectMove2);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyLeft) {
        with_view_model_cpp(
            select_move3->view,
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
            select_move3->view,
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
            select_move3->view,
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
            select_move3->view,
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

void select_move3_enter_callback(void* context) {
    furi_assert(context);
    SelectMove3* select_move3 = (SelectMove3*)context;
    with_view_model_cpp(
        select_move3->view,
        SelectPokemonModel*,
        model,
        {
            model->current_pokemon = select_move3->app->current_pokemon;
            model->pokemon_hex_code = select_move3->app->pokemon_hex_code;
            model->current_level = select_move3->app->current_level;
            model->current_stats = select_move3->app->current_stats;
            model->move1_hex_code = select_move3->app->move1_hex_code;
            model->move2_hex_code = select_move3->app->move2_hex_code;
            model->move3_hex_code = select_move3->app->move3_hex_code;
        },
        true);
}

bool select_move3_custom_callback(uint32_t event, void* context) {
    UNUSED(event);
    furi_assert(context);
    SelectMove3* select_move3 = (SelectMove3*)context;
    view_dispatcher_send_custom_event(select_move3->app->view_dispatcher, 0);
    return true;
}

void select_move3_exit_callback(void* context) {
    furi_assert(context);
    UNUSED(context);
}

SelectMove3* select_move3_alloc(App* app) {
    SelectMove3* select_move3 = (SelectMove3*)malloc(sizeof(SelectMove3));
    select_move3->app = app;
    select_move3->view = view_alloc();
    view_set_context(select_move3->view, select_move3);
    view_allocate_model(select_move3->view, ViewModelTypeLockFree, sizeof(SelectPokemonModel));
    with_view_model_cpp(
        select_move3->view,
        SelectPokemonModel*,
        model,
        {
            model->current_move = app->current_move;
            model->move3_hex_code = app->move3_hex_code;
        },
        true);

    view_set_draw_callback(select_move3->view, select_move3_render_callback);
    view_set_input_callback(select_move3->view, select_move3_input_callback);
    view_set_enter_callback(select_move3->view, select_move3_enter_callback);
    view_set_custom_callback(select_move3->view, select_move3_custom_callback);

    view_set_exit_callback(select_move3->view, select_move3_exit_callback);
    return select_move3;
}

void select_move3_free(App* app) {
    furi_assert(app->select_move3);
    view_free(app->select_move3->view);
    free(app->select_move3);
}

View* select_move3_get_view(App* app) {
    furi_assert(app->select_move3);
    return app->select_move3->view;
}
