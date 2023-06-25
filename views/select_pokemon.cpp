#include "../pokemon_app.h"
#include "select_pokemon.hpp"

static void select_pokemon_render_callback(Canvas* canvas, void* model) {
    PokemonFap* pokemon_fap = *(PokemonFap**)model;
    const uint8_t current_index = pokemon_fap->curr_pokemon;
    char pokedex_num[5];

    snprintf(pokedex_num, sizeof(pokedex_num), "#%03d", current_index + 1);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 55, 54 / 2, AlignLeft, AlignTop, pokemon_fap->pokemon_table[current_index].name);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 55, 38, AlignLeft, AlignTop, pokedex_num);
    canvas_draw_icon(canvas, 0, 0, pokemon_fap->pokemon_table[current_index].icon);
    canvas_draw_icon(canvas, 128 - 80, 0, &I_Space_80x18);
    canvas_draw_str_aligned(canvas, (128 - 40), 5, AlignCenter, AlignTop, "Select Pokemon");

    canvas_set_font(canvas, FontPrimary);
    elements_button_center(canvas, "OK");
}

static bool select_pokemon_input_callback(InputEvent* event, void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    int pokemon_num = pokemon_fap->curr_pokemon;
    bool consumed = false;

    furi_assert(context);

    /* We only handle InputTypePress at the moment */
    if(event->type != InputTypePress) return consumed;

    switch(event->key) {
    /* Advance to next view with the selected pokemon */
    case InputKeyOk:
        view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewTrade);
        consumed = true;
        break;

    /* Return to the previous view */
    case InputKeyBack:
        view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, VIEW_NONE);
        consumed = true;
        break;

    /* Move back one through the pokedex listing */
    case InputKeyLeft:
        if(pokemon_num == 0)
            pokemon_num = 150;
        else
            pokemon_num--;
        consumed = true;
        break;

    /* Move back ten through the pokemon listing, wrap to max pokemon on
         * underflow.
         */
    case InputKeyDown:
        if(pokemon_num >= 10)
            pokemon_num -= 10;
        else
            pokemon_num = 150;
        consumed = true;
        break;

    /* Move forward one through the pokedex listing */
    case InputKeyRight:
        if(pokemon_num == 150)
            pokemon_num = 0;
        else
            pokemon_num++;
        consumed = true;
        break;

    /* Move forward ten through the pokemon listing, wrap to min pokemon on
         * overflow.
         */
    case InputKeyUp:
        if(pokemon_num <= 140)
            pokemon_num += 10;
        else
            pokemon_num = 0;
        consumed = true;
        break;

    default:
        // Do Nothing
        break;
    }

    pokemon_fap->curr_pokemon = pokemon_num;

    return consumed;
}

void select_pokemon_enter_callback(void* context) {
    furi_assert(context);
    UNUSED(context);
}

bool select_pokemon_custom_callback(uint32_t event, void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    UNUSED(event);
    furi_assert(context);
    view_dispatcher_send_custom_event(pokemon_fap->view_dispatcher, 0);
    return true;
}

void select_pokemon_exit_callback(void* context) {
    furi_assert(context);
    UNUSED(context);
}

View* select_pokemon_alloc(PokemonFap* pokemon_fap) {
    View* view;

    view = view_alloc();

    view_set_context(view, pokemon_fap);
    view_allocate_model(view, ViewModelTypeLockFree, sizeof(PokemonFap**));
    with_view_model_cpp(
        view, PokemonFap**, model_fap, { *model_fap = pokemon_fap; }, false);

    view_set_draw_callback(view, select_pokemon_render_callback);
    view_set_input_callback(view, select_pokemon_input_callback);
    view_set_enter_callback(view, select_pokemon_enter_callback);
    view_set_custom_callback(view, select_pokemon_custom_callback);

    view_set_exit_callback(view, select_pokemon_exit_callback);
    return view;
}

void select_pokemon_free(PokemonFap* pokemon_fap) {
    furi_assert(pokemon_fap);
    view_free(pokemon_fap->select_view);
}
