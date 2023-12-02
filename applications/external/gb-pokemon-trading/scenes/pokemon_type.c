#include <gui/modules/variable_item_list.h>

#include "../pokemon_app.h"
#include "pokemon_menu.h"

/* TODO: In the future I would like to be able to set the types and then
 * require a "save" button to save them. This would require tracking of
 * the two different VariableItems in a way that I don't know how to do
 * yet with this interface.
 * For now, selecting a type immediately updates the trade_block struct,
 * requiring the user to press Back to go back. I would like to implement
 * an OK press or something to save both. But thats a problem for another
 * day.
 */
static void select_type_1_callback(VariableItem* item) {
    PokemonFap* pokemon_fap = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, pokemon_fap->type_list[index].name);
    pokemon_fap->trade_block->party[0].type[0] = pokemon_fap->type_list[index].index;
}

static void select_type_2_callback(VariableItem* item) {
    PokemonFap* pokemon_fap = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, pokemon_fap->type_list[index].name);
    pokemon_fap->trade_block->party[0].type[1] = pokemon_fap->type_list[index].index;
}

void select_type_scene_on_exit(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewMainMenu);
    view_dispatcher_remove_view(pokemon_fap->view_dispatcher, AppViewOpts);
}

void select_type_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    VariableItem* type1;
    VariableItem* type2;
    int curr_pokemon_type1 = pokemon_fap->trade_block->party[0].type[0];
    int curr_pokemon_type2 = pokemon_fap->trade_block->party[0].type[1];
    int num_types = pokemon_named_list_get_num_elements(pokemon_fap->type_list);
    const NamedList* type_list = pokemon_fap->type_list;

    variable_item_list_reset(pokemon_fap->variable_item_list);

    type1 = variable_item_list_add(
        pokemon_fap->variable_item_list, "Type 1:", num_types, select_type_1_callback, pokemon_fap);
    type2 = variable_item_list_add(
        pokemon_fap->variable_item_list, "Type 2:", num_types, select_type_2_callback, pokemon_fap);

    variable_item_set_current_value_index(
        type1, pokemon_named_list_get_list_pos_from_index(type_list, curr_pokemon_type1));
    variable_item_set_current_value_text(
        type1, pokemon_named_list_get_name_from_index(type_list, curr_pokemon_type1));

    variable_item_set_current_value_index(
        type2, pokemon_named_list_get_list_pos_from_index(type_list, curr_pokemon_type2));
    variable_item_set_current_value_text(
        type2, pokemon_named_list_get_name_from_index(type_list, curr_pokemon_type2));

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher,
        AppViewOpts,
        variable_item_list_get_view(pokemon_fap->variable_item_list));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}
