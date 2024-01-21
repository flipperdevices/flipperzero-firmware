#include "../pokemon_app.h"
#include "../pokemon_char_encode.h"

#include "pokemon_menu.h"
#include "pokemon_gen.h"
#include "pokemon_select.h"
#include "pokemon_name_input.h"
#include "pokemon_number_input.h"
#include "pokemon_move.h"
#include "pokemon_type.h"
#include "pokemon_stats.h"
#include "pokemon_trade.h"
#include "pokemon_pins.h"

/* XXX: Make an alloc in here for this, no need for the main app to need
 * to care about it */
static void scene_change_from_main_cb(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    /* Set trade generation based on selected menu item */
    switch(index) {
    case GenITradeScene:
        pokemon_stat_set(pokemon_fap->trade_block, GEN_I, STAT_GEN, NONE, GEN_I);
        break;
    case GenIITradeScene:
        pokemon_stat_set(pokemon_fap->trade_block, GEN_I, STAT_GEN, NONE, GEN_II);
        break;
    default:
        pokemon_stat_set(pokemon_fap->trade_block, GEN_I, STAT_GEN, NONE, 0);
        break;
    }

    /* XXX: Allocate trade block here based on generation selected */

    /* Set scene state to the current index so we can have that element highlighted when
     * we return.
     */
    scene_manager_set_scene_state(pokemon_fap->scene_manager, MainMenuScene, index);
    scene_manager_next_scene(pokemon_fap->scene_manager, index);
}

bool main_menu_back_event_callback(void* context) {
    furi_assert(context);
    PokemonFap* pokemon_fap = context;
    return scene_manager_handle_back_event(pokemon_fap->scene_manager);
}

void main_menu_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    submenu_reset(pokemon_fap->submenu);
    submenu_set_header(pokemon_fap->submenu, "Pokemon Trade Tool");

    submenu_add_item(
        pokemon_fap->submenu,
        "Gen I    (R/B/Y non-JPN)",
        GenITradeScene,
        scene_change_from_main_cb,
        pokemon_fap);
    submenu_add_item(
        pokemon_fap->submenu,
        "Select EXT Pinout",
        SelectPinsScene,
        scene_change_from_main_cb,
        pokemon_fap);

    submenu_set_selected_item(
        pokemon_fap->submenu,
        scene_manager_get_scene_state(pokemon_fap->scene_manager, MainMenuScene));

    view_dispatcher_set_navigation_event_callback(
        pokemon_fap->view_dispatcher, main_menu_back_event_callback);

    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewMainMenu);
}

bool null_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void null_scene_on_exit(void* context) {
    UNUSED(context);
}

void generic_scene_on_exit(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewMainMenu);
    view_dispatcher_remove_view(pokemon_fap->view_dispatcher, AppViewOpts);
}

void (*const pokemon_scene_on_enter_handlers[])(void*) = {
    main_menu_scene_on_enter,
    gen_scene_on_enter,
    null_scene_on_exit,
    select_pokemon_scene_on_enter,
    select_name_scene_on_enter,
    select_number_scene_on_enter,
    select_move_scene_on_enter,
    select_move_index_scene_on_enter,
    select_move_set_scene_on_enter,
    select_type_scene_on_enter,
    select_stats_scene_on_enter,
    select_number_scene_on_enter,
    select_name_scene_on_enter,
    trade_scene_on_enter,
    select_pins_scene_on_enter,
};

void (*const pokemon_scene_on_exit_handlers[])(void*) = {
    null_scene_on_exit,
    null_scene_on_exit,
    null_scene_on_exit,
    null_scene_on_exit,
    generic_scene_on_exit,
    generic_scene_on_exit,
    null_scene_on_exit,
    null_scene_on_exit,
    null_scene_on_exit,
    generic_scene_on_exit,
    null_scene_on_exit,
    generic_scene_on_exit,
    generic_scene_on_exit,
    null_scene_on_exit,
    generic_scene_on_exit,
};

bool (*const pokemon_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
    null_scene_on_event,
};

const SceneManagerHandlers pokemon_scene_manager_handlers = {
    .on_enter_handlers = pokemon_scene_on_enter_handlers,
    .on_exit_handlers = pokemon_scene_on_exit_handlers,
    .on_event_handlers = pokemon_scene_on_event_handlers,
    .scene_num = SceneCount,
};
