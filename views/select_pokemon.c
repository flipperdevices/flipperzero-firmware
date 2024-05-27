#include <gui/elements.h>
#include <pokemon_icons.h>

#include "../scenes/pokemon_menu.h"
#include "../pokemon_app.h"
#include "../pokemon_data.h"

struct select_model {
    uint8_t curr_pokemon;
    const void* pokemon_table;
    PokemonData* pdata;
};

/* Anonymous struct */
struct select_ctx {
    View* view;
    PokemonData* pdata;
    SceneManager* scene_manager;
};

static void select_pokemon_render_callback(Canvas* canvas, void* model) {
    struct select_model* view_model = model;
    uint8_t curr_pokemon = view_model->curr_pokemon;
    char pokedex_num[5];

    snprintf(pokedex_num, sizeof(pokedex_num), "#%03d", curr_pokemon + 1);

    /* Update the bitmap in pdata if needed */
    pokemon_icon_get(view_model->pdata, curr_pokemon + 1);
    canvas_draw_xbm(
        canvas,
        0,
        0,
        view_model->pdata->bitmap->width,
        view_model->pdata->bitmap->height,
        view_model->pdata->bitmap->data);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        58,
        27,
        AlignLeft,
        AlignTop,
        table_stat_name_get(view_model->pokemon_table, curr_pokemon));

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 58, 38, AlignLeft, AlignTop, pokedex_num);
    elements_frame(canvas, 55, 0, 71, 18);
    canvas_draw_str_aligned(canvas, 90, 5, AlignCenter, AlignTop, "Select Pokemon");

    canvas_set_font(canvas, FontPrimary);
    elements_button_center(canvas, "OK");
}

static bool select_pokemon_input_callback(InputEvent* event, void* context) {
    struct select_ctx* select = (struct select_ctx*)context;
    bool consumed = false;
    uint8_t selected_pokemon;

    furi_assert(context);

    /* We only handle InputTypePress at the moment */
    if(event->type != InputTypePress) return consumed;

    with_view_model(
        select->view,
        struct select_model * model,
        { selected_pokemon = model->curr_pokemon; },
        false);

    switch(event->key) {
    /* Advance to next view with the selected pokemon */
    case InputKeyOk:
        pokemon_stat_set(select->pdata, STAT_NUM, NONE, selected_pokemon);
        scene_manager_previous_scene(select->scene_manager);
        consumed = true;
        break;

    /* Move back one through the pokedex listing */
    case InputKeyLeft:
        if(selected_pokemon == 0)
            selected_pokemon = select->pdata->dex_max;
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
            selected_pokemon = select->pdata->dex_max;
        consumed = true;
        break;

    /* Move forward one through the pokedex listing */
    case InputKeyRight:
        if(selected_pokemon == select->pdata->dex_max)
            selected_pokemon = 0;
        else
            selected_pokemon++;
        consumed = true;
        break;

    /* Move forward ten through the pokemon listing, wrap to min pokemon on
         * overflow.
         */
    case InputKeyUp:
        if(selected_pokemon <= (select->pdata->dex_max - 10))
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
        select->view,
        struct select_model * model,
        { model->curr_pokemon = selected_pokemon; },
        true);

    return consumed;
}

void select_pokemon_enter_callback(void* context) {
    struct select_ctx* select = (struct select_ctx*)context;

    with_view_model(
        select->view,
        struct select_model * model,
        {
            model->curr_pokemon = pokemon_stat_get(select->pdata, STAT_NUM, NONE);
            model->pokemon_table = select->pdata->pokemon_table;
            model->pdata = select->pdata;
        },
        true);
}

void* select_pokemon_alloc(
    PokemonData* pdata,
    ViewDispatcher* view_dispatcher,
    SceneManager* scene_manager,
    uint32_t viewid) {
    furi_assert(pdata);

    struct select_ctx* select = malloc(sizeof(struct select_ctx));

    select->view = view_alloc();
    select->pdata = pdata;
    select->scene_manager = scene_manager;
    select->pdata = pdata;

    view_set_context(select->view, select);
    view_allocate_model(select->view, ViewModelTypeLockFree, sizeof(struct select_model));

    view_set_draw_callback(select->view, select_pokemon_render_callback);
    view_set_input_callback(select->view, select_pokemon_input_callback);
    view_set_enter_callback(select->view, select_pokemon_enter_callback);

    view_dispatcher_add_view(view_dispatcher, viewid, select->view);

    return select;
}

void select_pokemon_free(ViewDispatcher* view_dispatcher, uint32_t viewid, void* select_ctx) {
    struct select_ctx* select = (struct select_ctx*)select_ctx;

    view_dispatcher_remove_view(view_dispatcher, viewid);

    view_free(select->view);
    free(select);
}
