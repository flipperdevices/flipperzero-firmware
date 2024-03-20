#include "../pokemon_app.h"
#include "../pokemon_data.h"
#include "../pokemon_char_encode.h"

#include "pokemon_menu.h"
#include "pokemon_gen.h"
#include "pokemon_select.h"
#include "pokemon_name_input.h"
#include "pokemon_number_input.h"
#include "pokemon_move.h"
#include "pokemon_item.h"
#include "pokemon_type.h"
#include "pokemon_stats.h"
#include "pokemon_shiny.h"
#include "pokemon_gender.h"
#include "pokemon_pokerus.h"
#include "pokemon_trade.h"
#include "pokemon_pins.h"
#include "pokemon_exit_confirm.h"

static void scene_change_from_main_cb(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    /* The same trade scene is used for both gen i and ii. Set the real index to
     * scene's state.
     */
    scene_manager_set_scene_state(pokemon_fap->scene_manager, GenITradeScene, index);

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
        "Gen II   (G/S/C non-JPN)",
        GenIITradeScene,
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
    main_menu_scene_on_enter, //MainMenuScene,
    gen_scene_on_enter, //GenITradeScene,
    gen_scene_on_enter, //GenIITradeScene,
    select_pokemon_scene_on_enter, //SelectPokemonScene,
    select_name_scene_on_enter, //SelectNicknameScene,
    select_number_scene_on_enter, //SelectLevelScene,
    select_move_scene_on_enter, //SelectMoveScene,
    select_move_index_scene_on_enter, //SelectMoveIndexScene,
    select_move_set_scene_on_enter, //SelectMoveSetScene,
    select_item_scene_on_enter, //SelectItemScene,
    select_item_set_scene_on_enter, //SelectItemSetScene,
    select_type_scene_on_enter, //SelectTypeScene,
    select_stats_scene_on_enter, //SelectStatsScene,
    select_shiny_scene_on_enter, //SelectShinyScene,
    select_gender_scene_on_enter, //SelectGenderScene,
    select_pokerus_scene_on_enter, //SelectPokerusScene,
    select_name_scene_on_enter, //SelectUnownFormScene,
    select_number_scene_on_enter, //SelectOTIDScene,
    select_name_scene_on_enter, //SelectOTNameScene,
    trade_scene_on_enter, //TradeScene,
    select_pins_scene_on_enter, //SelectPinsScene,
    pokemon_exit_confirm_on_enter, //ConfirmExitScene,
};

void (*const pokemon_scene_on_exit_handlers[])(void*) = {
    null_scene_on_exit, //MainMenuScene,
    null_scene_on_exit, //GenITradeScene,
    null_scene_on_exit, //GenIITradeScene,
    null_scene_on_exit, //SelectPokemonScene,
    generic_scene_on_exit, //SelectNicknameScene,
    generic_scene_on_exit, //SelectLevelScene,
    null_scene_on_exit, //SelectMoveScene,
    null_scene_on_exit, //SelectMoveIndexScene,
    null_scene_on_exit, //SelectMoveSetScene,
    null_scene_on_exit, //SelectItemScene,
    null_scene_on_exit, //SelectItemSetScene,
    generic_scene_on_exit, //SelectTypeScene,
    null_scene_on_exit, //SelectStatsScene,
    null_scene_on_exit, //SelectShinyScene,
    null_scene_on_exit, //SelectGenderScene,
    generic_scene_on_exit, //SelectPokerusScene,
    generic_scene_on_exit, //SelectUnownFormScene,
    generic_scene_on_exit, //SelectOTIDScene,
    generic_scene_on_exit, //SelectOTNameScene,
    null_scene_on_exit, //TradeScene,
    generic_scene_on_exit, //SelectPinsScene,
    generic_scene_on_exit, //ConfirmExitScene,
};

bool (*const pokemon_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    null_scene_on_event, //MainMenuScene,
    null_scene_on_event, //GenITradeScene,
    null_scene_on_event, //GenIITradeScene,
    null_scene_on_event, //SelectPokemonScene,
    null_scene_on_event, //SelectNicknameScene,
    null_scene_on_event, //SelectLevelScene,
    null_scene_on_event, //SelectMoveScene,
    null_scene_on_event, //SelectMoveIndexScene,
    null_scene_on_event, //SelectMoveSetScene,
    null_scene_on_event, //SelectItemScene,
    null_scene_on_event, //SelectItemSetScene,
    null_scene_on_event, //SelectTypeScene,
    null_scene_on_event, //SelectStatsScene,
    null_scene_on_event, //SelectShinyScene,
    null_scene_on_event, //SelectGenderScene,
    null_scene_on_event, //SelectPokerusScene,
    null_scene_on_event, //SelectUnownFormScene,
    null_scene_on_event, //SelectOTIDScene,
    null_scene_on_event, //SelectOTNameScene,
    null_scene_on_event, //TradeScene,
    null_scene_on_event, //SelectPinsScene,
    pokemon_exit_confirm_on_event, //ConfirmExitScene,
};

const SceneManagerHandlers pokemon_scene_manager_handlers = {
    .on_enter_handlers = pokemon_scene_on_enter_handlers,
    .on_exit_handlers = pokemon_scene_on_exit_handlers,
    .on_event_handlers = pokemon_scene_on_event_handlers,
    .scene_num = SceneCount,
};
