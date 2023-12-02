#include "../pokemon_app.h"
#include "../pokemon_char_encode.h"

#include "pokemon_menu.h"
#include "pokemon_select.h"
#include "pokemon_nickname.h"
#include "pokemon_level.h"
#include "pokemon_move.h"
#include "pokemon_type.h"
#include "pokemon_stats.h"
#include "pokemon_ot_id.h"
#include "pokemon_ot_name.h"
#include "pokemon_trade.h"

static void scene_change_from_main_cb(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

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
    char buf[32];
    char name_buf[11]; // All name buffers are 11 bytes at most, including term
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    /* Clear the scene state of the Move scene since that is used to set the
     * highlighted meny item.
     */
    scene_manager_set_scene_state(pokemon_fap->scene_manager, SelectMoveScene, 0);

    submenu_reset(pokemon_fap->submenu);

    snprintf(
        buf,
        sizeof(buf),
        "Pokemon:   %s",
        pokemon_fap->pokemon_table[pokemon_fap->curr_pokemon].name);
    submenu_add_item(
        pokemon_fap->submenu, buf, SelectPokemonScene, scene_change_from_main_cb, pokemon_fap);
    pokemon_encoded_array_to_str(
        name_buf, (uint8_t*)pokemon_fap->trade_block->nickname, sizeof(name_buf));
    snprintf(buf, sizeof(buf), "Nickname:  %s", name_buf);
    submenu_add_item(
        pokemon_fap->submenu, buf, SelectNicknameScene, scene_change_from_main_cb, pokemon_fap);
    snprintf(buf, sizeof(buf), "Level:           %d", pokemon_fap->trade_block->party[0].level);
    submenu_add_item(
        pokemon_fap->submenu, buf, SelectLevelScene, scene_change_from_main_cb, pokemon_fap);
    submenu_add_item(
        pokemon_fap->submenu,
        "Select Moves",
        SelectMoveScene,
        scene_change_from_main_cb,
        pokemon_fap);
    submenu_add_item(
        pokemon_fap->submenu,
        "Select Types",
        SelectTypeScene,
        scene_change_from_main_cb,
        pokemon_fap);
    submenu_add_item(
        pokemon_fap->submenu,
        stats_text[pokemon_fap->curr_stats],
        SelectStatsScene,
        scene_change_from_main_cb,
        pokemon_fap);
    snprintf(
        buf,
        sizeof(buf),
        "OT ID#:          %05d",
        __builtin_bswap16(pokemon_fap->trade_block->party[0].ot_id));
    submenu_add_item(
        pokemon_fap->submenu, buf, SelectOTIDScene, scene_change_from_main_cb, pokemon_fap);
    pokemon_encoded_array_to_str(
        name_buf, (uint8_t*)pokemon_fap->trade_block->ot_name, sizeof(name_buf));
    snprintf(buf, sizeof(buf), "OT Name:      %s", name_buf);
    submenu_add_item(
        pokemon_fap->submenu, buf, SelectOTNameScene, scene_change_from_main_cb, pokemon_fap);
    submenu_add_item(
        pokemon_fap->submenu, "Trade PKMN", TradeScene, scene_change_from_main_cb, pokemon_fap);

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

void (*const pokemon_scene_on_enter_handlers[])(void*) = {
    main_menu_scene_on_enter,
    select_pokemon_scene_on_enter,
    select_nickname_scene_on_enter,
    select_level_scene_on_enter,
    select_move_scene_on_enter,
    select_move_index_scene_on_enter,
    select_move_set_scene_on_enter,
    select_type_scene_on_enter,
    select_stats_scene_on_enter,
    select_ot_id_scene_on_enter,
    select_ot_name_scene_on_enter,
    trade_scene_on_enter,
};

void (*const pokemon_scene_on_exit_handlers[])(void*) = {
    null_scene_on_exit,
    select_pokemon_scene_on_exit,
    select_nickname_scene_on_exit,
    select_level_scene_on_exit,
    null_scene_on_exit,
    null_scene_on_exit,
    null_scene_on_exit,
    select_type_scene_on_exit,
    null_scene_on_exit,
    select_ot_id_scene_on_exit,
    select_ot_name_scene_on_exit,
    null_scene_on_exit,
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
};

const SceneManagerHandlers pokemon_scene_manager_handlers = {
    .on_enter_handlers = pokemon_scene_on_enter_handlers,
    .on_exit_handlers = pokemon_scene_on_exit_handlers,
    .on_event_handlers = pokemon_scene_on_event_handlers,
    .scene_num = SceneCount,
};
