#include <gui/elements.h>
#include <pokemon_icons.h>

#include "../scenes/pokemon_menu.h"
#include "../pokemon_app.h"

struct select_model {
    uint8_t curr_pokemon;
    const PokemonTable* pokemon_table;
};

static void select_pokemon_render_callback(Canvas* canvas, void* model) {
    struct select_model* view_model = model;
    uint8_t curr_pokemon = view_model->curr_pokemon;
    char pokedex_num[5];

    snprintf(pokedex_num, sizeof(pokedex_num), "#%03d", curr_pokemon + 1);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 55, 54 / 2, AlignLeft, AlignTop, view_model->pokemon_table[curr_pokemon].name);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 55, 38, AlignLeft, AlignTop, pokedex_num);
    canvas_draw_icon(canvas, 0, 0, view_model->pokemon_table[curr_pokemon].icon);
    canvas_draw_icon(canvas, 128 - 80, 0, &I_Space_80x18);
    canvas_draw_str_aligned(canvas, (128 - 40), 5, AlignCenter, AlignTop, "Select Pokemon");

    canvas_set_font(canvas, FontPrimary);
    elements_button_center(canvas, "OK");
}

static bool select_pokemon_input_callback(InputEvent* event, void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    bool consumed = false;
    uint8_t selected_pokemon;

    furi_assert(context);

    /* We only handle InputTypePress at the moment */
    if(event->type != InputTypePress) return consumed;

    with_view_model(
        pokemon_fap->select_view,
        struct select_model * model,
        { selected_pokemon = model->curr_pokemon; },
        false);

    switch(event->key) {
    /* Advance to next view with the selected pokemon */
    case InputKeyOk:
        pokemon_fap->curr_pokemon = selected_pokemon;
        FURI_LOG_D(TAG, "[Select] Selected %s", pokemon_fap->pokemon_table[selected_pokemon].name);
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

    with_view_model(
        pokemon_fap->select_view,
        struct select_model * model,
        { model->curr_pokemon = selected_pokemon; },
        true);

    return consumed;
}

void select_pokemon_enter_callback(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    with_view_model(
        pokemon_fap->select_view,
        struct select_model * model,
        {
            model->curr_pokemon = (uint8_t)pokemon_fap->curr_pokemon;
            model->pokemon_table = pokemon_fap->pokemon_table;
        },
        true);
}

View* select_pokemon_alloc(PokemonFap* pokemon_fap) {
    View* view;

    view = view_alloc();

    view_set_context(view, pokemon_fap);
    view_allocate_model(view, ViewModelTypeLockFree, sizeof(struct select_model));

    view_set_draw_callback(view, select_pokemon_render_callback);
    view_set_input_callback(view, select_pokemon_input_callback);
    view_set_enter_callback(view, select_pokemon_enter_callback);
    return view;
}

void select_pokemon_free(PokemonFap* pokemon_fap) {
    furi_assert(pokemon_fap);
    view_free(pokemon_fap->select_view);
}
