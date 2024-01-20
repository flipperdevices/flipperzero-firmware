#include <gui/modules/variable_item_list.h>
#include <furi.h>

#include "../pokemon_app.h"
#include "pokemon_menu.h"

struct named_pins {
    const char* text;
    const GpioPin* pin;
};

/* XXX: These exist already in Flipper API */
static const struct named_pins named_pins[] = {
    {"PA7", &gpio_ext_pa7},
    {"PA6", &gpio_ext_pa6},
    {"PA4", &gpio_ext_pa4},
    {"PB3", &gpio_ext_pb3},
    {"PB2", &gpio_ext_pb2},
    {"PC3", &gpio_ext_pc3},
    {"PC1", &gpio_ext_pc1},
    {"PC0", &gpio_ext_pc0},
    {},
};

#define NUM_PINS 8

/* This must match gblink's enum order */
static const char* named_groups[] = {
    "Original",
    "Malveke",
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
    pokemon_fap->pins.serin = named_pins[builder.serin_index].pin;
    pokemon_fap->pins.serout = named_pins[builder.serout_index].pin;
    pokemon_fap->pins.clk = named_pins[builder.clk_index].pin;
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

    variable_item_set_current_value_text(item, named_pins[index].text);
    builder.serin_index = index;
    select_pins_rebuild_list(pokemon_fap);
    variable_item_list_set_selected_item(pokemon_fap->variable_item_list, 1);
}

static void select_pins_serout_callback(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    PokemonFap* pokemon_fap = variable_item_get_context(item);

    variable_item_set_current_value_text(item, named_pins[index].text);
    builder.serout_index = index;
    select_pins_rebuild_list(pokemon_fap);
    variable_item_list_set_selected_item(pokemon_fap->variable_item_list, 2);
}

static void select_pins_clk_callback(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    PokemonFap* pokemon_fap = variable_item_get_context(item);

    variable_item_set_current_value_text(item, named_pins[index].text);
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

    /* HACK: */
    pokemon_fap->malveke_detected = builder.named_index;

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
    variable_item_set_current_value_text(builder.serin, named_pins[builder.serin_index].text);

    variable_item_set_current_value_index(builder.serout, (num == 1 ? 0 : builder.serout_index));
    variable_item_set_current_value_text(builder.serout, named_pins[builder.serout_index].text);

    variable_item_set_current_value_index(builder.clk, (num == 1 ? 0 : builder.clk_index));
    variable_item_set_current_value_text(builder.clk, named_pins[builder.clk_index].text);
}

void select_pins_scene_on_enter(void* context) {
    PokemonFap* pokemon_fap = (PokemonFap*)context;

    /* TODO: Figure out what defaults we should use for pins based on attached board! */
    /* HACK: */
    if(builder.named_index < 2) builder.named_index = pokemon_fap->malveke_detected;

    select_pins_rebuild_list(pokemon_fap);

    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher,
        AppViewOpts,
        variable_item_list_get_view(pokemon_fap->variable_item_list));
    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewOpts);
}
