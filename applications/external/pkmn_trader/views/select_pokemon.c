#include <gui/elements.h>
#include <pokemon_icons.h>

#include "../scenes/pokemon_menu.h"
#include "../pokemon_app.h"

int selected_pokemon;

static void select_pokemon_render_callback(Canvas* canvas, void* model) {
    PokemonFap* pokemon_fap = *(PokemonFap**)model;
    const uint8_t current_index = selected_pokemon;
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
    bool consumed = false;

    furi_assert(context);

    /* We only handle InputTypePress at the moment */
    if(event->type != InputTypePress) return consumed;

    switch(event->key) {
    /* Advance to next view with the selected pokemon */
    case InputKeyOk:
        pokemon_fap->curr_pokemon = selected_pokemon;
        scene_manager_previous_scene(pokemon_fap->scene_manager);
        consumed = true;
        break;

    /* Move back one through the pokedex listing */
    case InputKeyLeft:
        if(selected_pokemon == 0)
            selected_pokemon = 150;
        else
            selected_pokemon--;
        consumed = true;
        break;

    /* Move back ten through the pokemon listing, wrap to max pokemon on
         * underflow.
         */
    case InputKeyDown:
        if(selected_pokemon >= 10)
            selected_pokemon -= 10;
        else
            selected_pokemon = 150;
        consumed = true;
        break;

    /* Move forward one through the pokedex listing */
    case InputKeyRight:
        if(selected_pokemon == 150)
            selected_pokemon = 0;
        else
            selected_pokemon++;
        consumed = true;
        break;

    /* Move forward ten through the pokemon listing, wrap to min pokemon on
         * overflow.
         */
    case InputKeyUp:
        if(selected_pokemon <= 140)
            selected_pokemon += 10;
        else
            selected_pokemon = 0;
        consumed = true;
        break;

    default:
        // Do Nothing
        break;
    }

    return consumed;
}

void select_pokemon_enter_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    selected_pokemon = pokemon_fap->curr_pokemon;
}

View* select_pokemon_alloc(PokemonFap* pokemon_fap) {
    View* view;

    view = view_alloc();

    view_set_context(view, pokemon_fap);
    view_allocate_model(view, ViewModelTypeLockFree, sizeof(PokemonFap**));
    with_view_model(
        view, PokemonFap * *model_fap, { *model_fap = pokemon_fap; }, false);

    view_set_draw_callback(view, select_pokemon_render_callback);
    view_set_input_callback(view, select_pokemon_input_callback);
    view_set_enter_callback(view, select_pokemon_enter_callback);
    return view;
}

void select_pokemon_free(PokemonFap* pokemon_fap) {
    furi_assert(pokemon_fap);
    view_free_model(pokemon_fap->select_view);
    view_free(pokemon_fap->select_view);
}
