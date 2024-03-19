#include <gui/modules/variable_item_list.h>

#include <named_list.h>

#include "../pokemon_app.h"
#include "../pokemon_data.h"
#include "pokemon_menu.h"

struct type_cb {
    DataStatSub type;
    PokemonFap* pokemon_fap;
};

static struct type_cb type_cb[] = {
    {TYPE_0, NULL},
    {TYPE_1, NULL},
    {},
};

/* TODO: In the future I would like to be able to set the types and then
 * require a "save" button to save them. This would require tracking of
 * the two different VariableItems in a way that I don't know how to do
 * yet with this interface.
 * For now, selecting a type immediately updates the trade_block struct,
 * requiring the user to press Back to go back. I would like to implement
 * an OK press or something to save both. But thats a problem for another
 * day.
 */
static void select_type_callback(VariableItem* item) {
    struct type_cb* context = variable_item_get_context(item);
    uint8_t pos = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(
        item, namedlist_name_get_pos(context->pokemon_fap->pdata->type_list, pos));
    pokemon_stat_set(
        context->pokemon_fap->pdata,
        STAT_TYPE,
        context->type,
        namedlist_index_get(context->pokemon_fap->pdata->type_list, pos));
}

void select_type_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;
    VariableItem* vitype[2];
    char* strings[2] = {"Type 1:", "Type 2:"};
    int type;
    int num_types = namedlist_cnt(pokemon_fap->pdata->type_list);
    int pos;
    int i;

    variable_item_list_reset(pokemon_fap->variable_item_list);

    /* NOTE: 2 is a magic number, but pretty obvious */
    for(i = 0; i < 2; i++) {
        type_cb[i].pokemon_fap = pokemon_fap;
        type = pokemon_stat_get(pokemon_fap->pdata, STAT_TYPE, i);
        pos = namedlist_pos_get(pokemon_fap->pdata->type_list, type);

        vitype[i] = variable_item_list_add(
            pokemon_fap->variable_item_list,
            strings[i],
            num_types,
            select_type_callback,
            &type_cb[i]);

        variable_item_set_current_value_index(vitype[i], pos);
        variable_item_set_current_value_text(
            vitype[i], namedlist_name_get_pos(pokemon_fap->pdata->type_list, pos));
    }

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher,
        AppViewOpts,
        variable_item_list_get_view(pokemon_fap->variable_item_list));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}
