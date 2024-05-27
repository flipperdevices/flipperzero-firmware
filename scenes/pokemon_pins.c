#include <gui/modules/variable_item_list.h>
#include <furi.h>

#include "../pokemon_app.h"
#include "pokemon_menu.h"

/* This is a bit of a hack to save some space and not have to refactor this scene.
 * We re-use the name and pin from the global gpio pin definition, but need to
 * skip the two debug pins in the row of header pins.
 *
 * This is hard-coded, not really portable, but saves a couple hundred bytes :D
 *
 * In the future, the right way to do this would be to build our own table of
 * non-debug pins from whatever the current platforms gpio pin definition is.
 */
#define NUM_PINS 8
static const GpioPinRecord* named_pins[NUM_PINS] = {
    &gpio_pins[0],
    &gpio_pins[1],
    &gpio_pins[2],
    &gpio_pins[3],
    &gpio_pins[4],
    &gpio_pins[5],
    &gpio_pins[10],
    &gpio_pins[11]};

/* This must match gblink's enum order */
static const char* named_groups[] = {
    "Original",
    "MLVK2.5",
    "Custom",
    "",
};

struct itemlist_builder {
    VariableItem* named;
    VariableItem* serin;
    VariableItem* serout;
    VariableItem* clk;
    uint8_t named_index;
    uint8_t serin_index;
    uint8_t serout_index;
    uint8_t clk_index;
};

/* Just make it a global, whatever */
static struct itemlist_builder builder = {0};
static void select_pins_rebuild_list(PokemonFap* pokemon_fap);

static void select_pins_set(PokemonFap* pokemon_fap) {
    pokemon_fap->pins.serin = named_pins[builder.serin_index]->pin;
    pokemon_fap->pins.serout = named_pins[builder.serout_index]->pin;
    pokemon_fap->pins.clk = named_pins[builder.clk_index]->pin;
}

static void select_named_group_callback(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    PokemonFap* pokemon_fap = variable_item_get_context(item);

    variable_item_set_current_value_text(item, named_groups[index]);
    builder.named_index = index;
    select_pins_rebuild_list(pokemon_fap);
    variable_item_list_set_selected_item(pokemon_fap->variable_item_list, 0);
}

static void select_pins_serin_callback(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    PokemonFap* pokemon_fap = variable_item_get_context(item);

    variable_item_set_current_value_text(item, named_pins[index]->name);
    builder.serin_index = index;
    select_pins_rebuild_list(pokemon_fap);
    variable_item_list_set_selected_item(pokemon_fap->variable_item_list, 1);
}

static void select_pins_serout_callback(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    PokemonFap* pokemon_fap = variable_item_get_context(item);

    variable_item_set_current_value_text(item, named_pins[index]->name);
    builder.serout_index = index;
    select_pins_rebuild_list(pokemon_fap);
    variable_item_list_set_selected_item(pokemon_fap->variable_item_list, 2);
}

static void select_pins_clk_callback(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    PokemonFap* pokemon_fap = variable_item_get_context(item);

    variable_item_set_current_value_text(item, named_pins[index]->name);
    builder.clk_index = index;
    select_pins_rebuild_list(pokemon_fap);
    variable_item_list_set_selected_item(pokemon_fap->variable_item_list, 3);
}

static void select_pins_rebuild_list(PokemonFap* pokemon_fap) {
    int num;

    /* HACK: TODO: It would be better to do this programmatically, but, I'm kind
     * of done working on this feature so its going to be hardcoded for now.
     */
    switch(builder.named_index) {
    case 0: // Original
        num = 1;
        builder.serin_index = 5;
        builder.serout_index = 3;
        builder.clk_index = 4;
        break;
    case 1: // MALVEKE
        num = 1;
        builder.serin_index = 1;
        builder.serout_index = 0;
        builder.clk_index = 3;
        break;
    default:
        num = NUM_PINS;
        break;
    }

    select_pins_set(pokemon_fap);

    variable_item_list_reset(pokemon_fap->variable_item_list);

    builder.named = variable_item_list_add(
        pokemon_fap->variable_item_list, "Mode", 3, select_named_group_callback, pokemon_fap);
    builder.serin = variable_item_list_add(
        pokemon_fap->variable_item_list, "SI:", num, select_pins_serin_callback, pokemon_fap);
    builder.serout = variable_item_list_add(
        pokemon_fap->variable_item_list, "SO:", num, select_pins_serout_callback, pokemon_fap);
    builder.clk = variable_item_list_add(
        pokemon_fap->variable_item_list, "CLK:", num, select_pins_clk_callback, pokemon_fap);

    variable_item_set_current_value_index(builder.named, builder.named_index);
    variable_item_set_current_value_text(builder.named, named_groups[builder.named_index]);

    variable_item_set_current_value_index(builder.serin, (num == 1 ? 0 : builder.serin_index));
    variable_item_set_current_value_text(builder.serin, named_pins[builder.serin_index]->name);

    variable_item_set_current_value_index(builder.serout, (num == 1 ? 0 : builder.serout_index));
    variable_item_set_current_value_text(builder.serout, named_pins[builder.serout_index]->name);

    variable_item_set_current_value_index(builder.clk, (num == 1 ? 0 : builder.clk_index));
    variable_item_set_current_value_text(builder.clk, named_pins[builder.clk_index]->name);
}

void select_pins_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    select_pins_rebuild_list(pokemon_fap);

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher,
        AppViewOpts,
        variable_item_list_get_view(pokemon_fap->variable_item_list));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}
