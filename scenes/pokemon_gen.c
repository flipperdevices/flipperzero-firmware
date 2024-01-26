#include "../pokemon_app.h"
#include "../pokemon_data.h"
#include "../pokemon_char_encode.h"

#include "pokemon_menu.h"
#include "pokemon_stats.h"

static void scene_change_from_main_cb(void* context, uint32_t index) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    /* Reuse of scenes to allow for using the same functions to set names */
    switch(index) {
    case SelectNicknameScene:
        scene_manager_set_scene_state(
            pokemon_fap->scene_manager, SelectNicknameScene, SelectNicknameScene);
        break;
    case SelectOTNameScene:
        scene_manager_set_scene_state(
            pokemon_fap->scene_manager, SelectNicknameScene, SelectOTNameScene);
        break;
    case SelectLevelScene:
        scene_manager_set_scene_state(
            pokemon_fap->scene_manager, SelectLevelScene, SelectLevelScene);
        break;
    case SelectOTIDScene:
        scene_manager_set_scene_state(
            pokemon_fap->scene_manager, SelectLevelScene, SelectOTIDScene);
        break;
    }

    /* Set scene state to the current index so we can have that element highlighted when
     * we return.
     */
    scene_manager_set_scene_state(pokemon_fap->scene_manager, GenITradeScene, index);
    scene_manager_next_scene(pokemon_fap->scene_manager, index);
}

/* XXX: Potentially trap Back when pressed here to prompt to prevent accidental leaving? */
/* XXX: Free the trade and other pointers here */
bool gen_back_event_callback(void* context) {
    furi_assert(context);
    PokemonFap* pokemon_fap = context;
    return scene_manager_handle_back_event(pokemon_fap->scene_manager);
}

/* XXX: Probably put which gen we're using in the FAP */
/* XXX: Should we allocate and free the trade block around here? */
/* XXX: Does flipper have a monospace font available? */
/* XXX: First entry here needs to have trade and other fap pointers set up */
void gen_scene_on_enter(void* context) {
    char buf[32];
    char name_buf[11]; // All name buffers are 11 bytes at most, including term
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    int pkmn_num;

    pkmn_num = pokemon_stat_get(pokemon_fap, STAT_NUM, NONE);

    /* Clear the scene state of the Move scene since that is used to set the
     * highlighted menu item.
     */
    scene_manager_set_scene_state(pokemon_fap->scene_manager, SelectMoveScene, 0);

    submenu_reset(pokemon_fap->submenu);

    snprintf(buf, sizeof(buf), "Pokemon:   %s", table_stat_name_get(pokemon_fap->pokemon_table, pkmn_num));
    submenu_add_item(
        pokemon_fap->submenu, buf, SelectPokemonScene, scene_change_from_main_cb, pokemon_fap);

    pokemon_name_get(pokemon_fap, STAT_NICKNAME, name_buf, sizeof(name_buf));
    snprintf(buf, sizeof(buf), "Nickname:  %s", name_buf);
    submenu_add_item(
        pokemon_fap->submenu, buf, SelectNicknameScene, scene_change_from_main_cb, pokemon_fap);

    snprintf(buf, sizeof(buf), "Level:           %d", pokemon_stat_get(pokemon_fap, STAT_LEVEL, NONE));
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
        named_list_name_from_index_get(pokemon_fap->stat_list, pokemon_stat_get(pokemon_fap, STAT_SEL, NONE)),
        SelectStatsScene,
        scene_change_from_main_cb,
        pokemon_fap);

    snprintf(buf, sizeof(buf), "OT ID#:          %05d", pokemon_stat_get(pokemon_fap, STAT_OT_ID, NONE));
    submenu_add_item(
        pokemon_fap->submenu, buf, SelectOTIDScene, scene_change_from_main_cb, pokemon_fap);

    pokemon_name_get(pokemon_fap, STAT_OT_NAME, name_buf, sizeof(name_buf));
    snprintf(buf, sizeof(buf), "OT Name:      %s", name_buf);
    submenu_add_item(
        pokemon_fap->submenu, buf, SelectOTNameScene, scene_change_from_main_cb, pokemon_fap);

    submenu_add_item(
        pokemon_fap->submenu, "Trade PKMN", TradeScene, scene_change_from_main_cb, pokemon_fap);

    /* TODO: Add Save pokemon option here */

    /* HACK: No matter what gen were in, we just store the scene state in GenITradeScene */
    submenu_set_selected_item(
        pokemon_fap->submenu,
        scene_manager_get_scene_state(pokemon_fap->scene_manager, GenITradeScene));

    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewMainMenu);
}
