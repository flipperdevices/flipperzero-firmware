#include "../pokemon_app.h"
#include "select_stats.hpp"

static void select_stats_render_callback(Canvas* canvas, void* context) {
    canvas_clear(canvas);

    SelectPokemonModel* model = (SelectPokemonModel*)context;
    const uint8_t current_index = model->current_stats;

    canvas_set_font(canvas, FontPrimary);
    if(current_index == 0) {
        canvas_draw_str_aligned(
            canvas, 55, 54 / 2, AlignLeft, AlignTop, "Random IV");
        canvas_draw_str_aligned(
            canvas, 55, 38, AlignLeft, AlignTop, "Zero EV");
    } else if(current_index == 1) {
        canvas_draw_str_aligned(
            canvas, 55, 54 / 2, AlignLeft, AlignTop, "Random IV");
        canvas_draw_str_aligned(
            canvas, 55, 38, AlignLeft, AlignTop, "Max EV / Lvl");
    } else if(current_index == 2) {
        canvas_draw_str_aligned(
            canvas, 55, 54 / 2, AlignLeft, AlignTop, "Random IV");
        canvas_draw_str_aligned(
            canvas, 55, 38, AlignLeft, AlignTop, "Max EV");
    } else if(current_index == 3) {
        canvas_draw_str_aligned(
            canvas, 55, 54 / 2, AlignLeft, AlignTop, "Max IV");
        canvas_draw_str_aligned(
            canvas, 55, 38, AlignLeft, AlignTop, "Zero EV");
    } else if(current_index == 4) {
        canvas_draw_str_aligned(
            canvas, 55, 54 / 2, AlignLeft, AlignTop, "Max IV");
        canvas_draw_str_aligned(
            canvas, 55, 38, AlignLeft, AlignTop, "Max EV / Lvl");
    } else if(current_index == 5) {
        canvas_draw_str_aligned(
            canvas, 55, 54 / 2, AlignLeft, AlignTop, "Max IV");
        canvas_draw_str_aligned(
            canvas, 55, 38, AlignLeft, AlignTop, "Max EV");
    }

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 0, 0, pokemon_table[model->current_pokemon].icon);
    canvas_draw_icon(canvas, 128 - 80, 0, &I_Space_80x18);
    canvas_draw_str_aligned(canvas, (128 - 40), 5, AlignCenter, AlignTop, "Select Stats");

    canvas_set_font(canvas, FontPrimary);
    elements_button_center(canvas, "OK");
}

static bool select_stats_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    SelectStats* select_stats = (SelectStats*)context;
    bool consumed = false;

    if(event->type == InputTypePress && event->key == InputKeyOk) {
        with_view_model_cpp(
            select_stats->view,
            SelectPokemonModel*,
            model,
            {
                select_stats->app->current_stats = model->current_stats;
            },
            false);
        
        view_dispatcher_switch_to_view(select_stats->app->view_dispatcher, AppViewSelectMove1);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyBack) {
        view_dispatcher_switch_to_view(select_stats->app->view_dispatcher, AppViewSelectLevel);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyLeft) {
        with_view_model_cpp(
            select_stats->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_stats == 0) {
                    model->current_stats = 5;
                } else {
                    model->current_stats--;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyDown) {
        with_view_model_cpp(
            select_stats->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_stats == 0) {
                    model->current_stats = 5;
                } else {
                    model->current_stats--;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyRight) {
        with_view_model_cpp(
            select_stats->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_stats == 5) {
                    model->current_stats = 0;
                } else {
                    model->current_stats++;
                }
            },
            true);
        consumed = true;
    } else if(event->type == InputTypePress && event->key == InputKeyUp) {
        with_view_model_cpp(
            select_stats->view,
            SelectPokemonModel*,
            model,
            {
                if(model->current_stats == 5) {
                    model->current_stats = 0;
                } else {
                    model->current_stats++;
                }
            },
            true);
        consumed = true;
    }

    return consumed;
}

void select_stats_enter_callback(void* context) {
    furi_assert(context);
    SelectStats* select_stats = (SelectStats*)context;
    with_view_model_cpp(
        select_stats->view,
        SelectPokemonModel*,
        model,
        {
            model->current_pokemon = select_stats->app->current_pokemon;
            model->pokemon_hex_code = select_stats->app->pokemon_hex_code;
            model->current_level = select_stats->app->current_level;
            model->current_stats = select_stats->app->current_stats;
        },
        true);
}

bool select_stats_custom_callback(uint32_t event, void* context) {
    UNUSED(event);
    furi_assert(context);
    SelectStats* select_stats = (SelectStats*)context;
    view_dispatcher_send_custom_event(select_stats->app->view_dispatcher, 0);
    return true;
}

void select_stats_exit_callback(void* context) {
    furi_assert(context);
    UNUSED(context);
}

SelectStats* select_stats_alloc(App* app) {
    SelectStats* select_stats = (SelectStats*)malloc(sizeof(SelectStats));
    select_stats->app = app;
    select_stats->view = view_alloc();
    view_set_context(select_stats->view, select_stats);
    view_allocate_model(select_stats->view, ViewModelTypeLockFree, sizeof(SelectPokemonModel));
    with_view_model_cpp(
        select_stats->view,
        SelectPokemonModel*,
        model,
        {
            model->current_stats = app->current_stats;
        },
        true);

    view_set_draw_callback(select_stats->view, select_stats_render_callback);
    view_set_input_callback(select_stats->view, select_stats_input_callback);
    view_set_enter_callback(select_stats->view, select_stats_enter_callback);
    view_set_custom_callback(select_stats->view, select_stats_custom_callback);

    view_set_exit_callback(select_stats->view, select_stats_exit_callback);
    return select_stats;
}

void select_stats_free(App* app) {
    furi_assert(app->select_stats);
    view_free(app->select_stats->view);
    free(app->select_stats);
}

View* select_stats_get_view(App* app) {
    furi_assert(app->select_stats);
    return app->select_stats->view;
}
