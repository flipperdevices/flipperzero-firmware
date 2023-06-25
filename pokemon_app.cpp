#include "pokemon_app.h"
#include "views/trade.hpp"
#include "views/select_pokemon.hpp"

const PokemonTable pokemon_table[] = {
    {"Bulbasaur", &I_bulbasaur, 0x99},
    {"Ivysaur", &I_ivysaur, 0x09},
    {"Venusaur", &I_venusaur, 0x9A},
    {"Charmander", &I_charmander, 0xB0},
    {"Charmeleon", &I_charmeleon, 0xB2},
    {"Charizard", &I_charizard, 0xB4},
    {"Squirtle", &I_squirtle, 0xB1},
    {"Wartortle", &I_wartortle, 0xB3},
    {"Blastoise", &I_blastoise, 0x1C},
    {"Caterpie", &I_caterpie, 0x7B},
    {"Metapod", &I_metapod, 0x7C},
    {"Butterfree", &I_butterfree, 0x7D},
    {"Weedle", &I_weedle, 0x70},
    {"Kakuna", &I_kakuna, 0x71},
    {"Beedrill", &I_beedrill, 0x72},
    {"Pidgey", &I_pidgey, 0x24},
    {"Pidgeotto", &I_pidgeotto, 0x96},
    {"Pidgeot", &I_pidgeot, 0x97},
    {"Rattata", &I_rattata, 0xA5},
    {"Raticate", &I_raticate, 0xA6},
    {"Spearow", &I_spearow, 0x05},
    {"Fearow", &I_fearow, 0x23},
    {"Ekans", &I_ekans, 0x6C},
    {"Arbok", &I_arbok, 0x2D},
    {"Pikachu", &I_pikachu, 0x54},
    {"Raichu", &I_raichu, 0x55},
    {"Sandshrew", &I_sandshrew, 0x60},
    {"Sandslash", &I_sandslash, 0x61},
    {"Nidoran ♀", &I_nidoranf, 0x0F},
    {"Nidorina", &I_nidorina, 0xA8},
    {"Nidoqueen", &I_nidoqueen, 0x10},
    {"Nidoran ♂", &I_nidoranm, 0x03},
    {"Nidorino", &I_nidorino, 0xA7},
    {"Nidoking", &I_nidoking, 0x07},
    {"Clefairy", &I_clefairy, 0x04},
    {"Clefable", &I_clefable, 0x8E},
    {"Vulpix", &I_vulpix, 0x52},
    {"Ninetales", &I_ninetales, 0x53},
    {"Jigglypuff", &I_jigglypuff, 0x64},
    {"Wigglytuff", &I_wigglytuff, 0x65},
    {"Zubat", &I_zubat, 0x6B},
    {"Golbat", &I_golbat, 0x82},
    {"Oddish", &I_oddish, 0xB9},
    {"Gloom", &I_gloom, 0xBA},
    {"Vileplume", &I_vileplume, 0xBB},
    {"Paras", &I_paras, 0x6D},
    {"Parasect", &I_parasect, 0x2E},
    {"Venonat", &I_venonat, 0x41},
    {"Venomoth", &I_venomoth, 0x77},
    {"Diglett", &I_diglett, 0x3B},
    {"Dugtrio", &I_dugtrio, 0x76},
    {"Meowth", &I_meowth, 0x4D},
    {"Persian", &I_persian, 0x90},
    {"Psyduck", &I_psyduck, 0x2F},
    {"Golduck", &I_golduck, 0x80},
    {"Mankey", &I_mankey, 0x39},
    {"Primeape", &I_primeape, 0x75},
    {"Growlithe", &I_growlithe, 0x21},
    {"Arcanine", &I_arcanine, 0x14},
    {"Poliwag", &I_poliwag, 0x47},
    {"Poliwhirl", &I_poliwhirl, 0x6E},
    {"Poliwrath", &I_poliwrath, 0x6F},
    {"Abra", &I_abra, 0x94},
    {"Kadabra", &I_kadabra, 0x26},
    {"Alakazam", &I_alakazam, 0x95},
    {"Machop", &I_machop, 0x6A},
    {"Machoke", &I_machoke, 0x29},
    {"Machamp", &I_machamp, 0x7E},
    {"Bellsprout", &I_bellsprout, 0xBC},
    {"Weepinbell", &I_weepinbell, 0xBD},
    {"Victreebel", &I_victreebel, 0xBE},
    {"Tentacool", &I_tentacool, 0x18},
    {"Tentacruel", &I_tentacruel, 0x9B},
    {"Geodude", &I_geodude, 0xA9},
    {"Graveler", &I_graveler, 0x27},
    {"Golem", &I_golem, 0x31},
    {"Ponyta", &I_ponyta, 0xA3},
    {"Rapidash", &I_rapidash, 0xA4},
    {"Slowpoke", &I_slowpoke, 0x25},
    {"Slowbro", &I_slowbro, 0x08},
    {"Magnemite", &I_magnemite, 0xAD},
    {"Magneton", &I_magneton, 0x36},
    {"Farfetch'd", &I_farfetchd, 0x40},
    {"Doduo", &I_doduo, 0x46},
    {"Dodrio", &I_dodrio, 0x74},
    {"Seel", &I_seel, 0x3A},
    {"Dewgong", &I_dewgong, 0x78},
    {"Grimer", &I_grimer, 0x0D},
    {"Muk", &I_muk, 0x88},
    {"Shellder", &I_shellder, 0x17},
    {"Cloyster", &I_cloyster, 0x8B},
    {"Gastly", &I_gastly, 0x19},
    {"Haunter", &I_haunter, 0x93},
    {"Gengar", &I_gengar, 0x0E},
    {"Onix", &I_onix, 0x22},
    {"Drowzee", &I_drowzee, 0x30},
    {"Hypno", &I_hypno, 0x81},
    {"Krabby", &I_krabby, 0x4E},
    {"Kingler", &I_kingler, 0x8A},
    {"Voltorb", &I_voltorb, 0x06},
    {"Electrode", &I_electrode, 0x8D},
    {"Exeggcute", &I_exeggcute, 0x0C},
    {"Exeggutor", &I_exeggutor, 0x0A},
    {"Cubone", &I_cubone, 0x11},
    {"Marowak", &I_marowak, 0x91},
    {"Hitmonlee", &I_hitmonlee, 0x2B},
    {"Hitmonchan", &I_hitmonchan, 0x2C},
    {"Lickitung", &I_lickitung, 0x0B},
    {"Koffing", &I_koffing, 0x37},
    {"Weezing", &I_weezing, 0x8F},
    {"Rhyhorn", &I_rhyhorn, 0x12},
    {"Rhydon", &I_rhydon, 0x01},
    {"Chansey", &I_chansey, 0x28},
    {"Tangela", &I_tangela, 0x1E},
    {"Kangaskhan", &I_kangaskhan, 0x02},
    {"Horsea", &I_horsea, 0x5C},
    {"Seadra", &I_seadra, 0x5D},
    {"Goldeen", &I_goldeen, 0x9D},
    {"Seaking", &I_seaking, 0x9E},
    {"Staryu", &I_staryu, 0x1B},
    {"Starmie", &I_starmie, 0x98},
    {"Mr. Mime", &I_mr_mime, 0x2A},
    {"Scyther", &I_scyther, 0x1A},
    {"Jynx", &I_jynx, 0x48},
    {"Electabuzz", &I_electabuzz, 0x35},
    {"Magmar", &I_magmar, 0x33},
    {"Pinsir", &I_pinsir, 0x1D},
    {"Tauros", &I_tauros, 0x3C},
    {"Magikarp", &I_magikarp, 0x85},
    {"Gyarados", &I_gyarados, 0x16},
    {"Lapras", &I_lapras, 0x13},
    {"Ditto", &I_ditto, 0x4C},
    {"Eevee", &I_eevee, 0x66},
    {"Vaporeon", &I_vaporeon, 0x69},
    {"Jolteon", &I_jolteon, 0x68},
    {"Flareon", &I_flareon, 0x67},
    {"Porygon", &I_porygon, 0xAA},
    {"Omanyte", &I_omanyte, 0x62},
    {"Omastar", &I_omastar, 0x63},
    {"Kabuto", &I_kabuto, 0x5A},
    {"Kabutops", &I_kabutops, 0x5B},
    {"Aerodactyl", &I_aerodactyl, 0xAB},
    {"Snorlax", &I_snorlax, 0x84},
    {"Articuno", &I_articuno, 0x4A},
    {"Zapdos", &I_zapdos, 0x4B},
    {"Moltres", &I_moltres, 0x49},
    {"Dratini", &I_dratini, 0x58},
    {"Dragonair", &I_dragonair, 0x59},
    {"Dragonite", &I_dragonite, 0x42},
    {"Mewtwo", &I_mewtwo, 0x83},
    {"Mew", &I_mew, 0x15},
    {},
};

TradeBlock OUTPUT_BLOCK = {
    .trainer_name = {F_, l_, i_, p_, p_, e_, r_, TERM_, 0x00, 0x00, 0x00},
    .party_cnt = 1,
    /* Only the first pokemon is ever used even though there are 7 bytes here.
     * If the remaining 6 bytes are _not_ 0xff, then the trade window renders
     * garbage for the Flipper's party.
     */
    .party_members = {0x15, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    /* Only the first pokemon is set up, even though there are 6 total party members */
    .party =
        {
            {.species = 0x4a,
             .hp = 0x2c01,
             .level = 0x4a,
             .status_condition = 0x0,
             .type = {0x14, 0x08},
             .catch_held = 0x1f,
             .move = {0x7e, 0x38, 0x09, 0x19},
             .orig_trainer = 0xd204,
             .exp = {0x3, 0xd, 0x40},
             .hp_ev = 0xffff,
             .atk_ev = 0xffff,
             .def_ev = 0xffff,
             .spd_ev = 0xffff,
             .special_ev = 0xffff,
             .iv = 0xffff,
             .move_pp = {0xc0, 0xc0, 0xc0, 0xc0},
             .level_again = 0x4a,
             .max_hp = 0x2c01,
             .atk = 0x9600,
             .def = 0x9700,
             .spd = 0x9800,
             .special = 0x9900},
        },
    /* Only the first pokemon has an OT name and nickname even though there are 6 members */
    /* NOTE: I think this shouldn't exceed 7 chars */
    .ot_name =
        {
            {.str = {F_, l_, i_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
        },
    .nickname = {
        {.str = {F_, l_, o_, p_, p_, e_, r_, TERM_, 0x00, 0x00}},
    }};

uint32_t pokemon_exit_confirm_view(void* context) {
    UNUSED(context);
    return AppViewExitConfirm;
}

PokemonFap* pokemon_alloc() {
    PokemonFap* pokemon_fap = (PokemonFap*)malloc(sizeof(PokemonFap));

    // View dispatcher
    pokemon_fap->view_dispatcher = view_dispatcher_alloc();

    view_dispatcher_enable_queue(pokemon_fap->view_dispatcher);
    view_dispatcher_set_event_callback_context(pokemon_fap->view_dispatcher, pokemon_fap);
    view_dispatcher_attach_to_gui(
        pokemon_fap->view_dispatcher,
        (Gui*)furi_record_open(RECORD_GUI),
        ViewDispatcherTypeFullscreen);

    //  Start Index first pokemon
    pokemon_fap->curr_pokemon = 0;

    // Set up pointer to pokemon table
    pokemon_fap->pokemon_table = pokemon_table;

    // Set up trade party struct
    pokemon_fap->trade_party = &OUTPUT_BLOCK;

    // Select Pokemon View
    pokemon_fap->select_view = select_pokemon_alloc(pokemon_fap);
    view_set_previous_callback(select_pokemon_get_view(pokemon_fap), pokemon_exit_confirm_view);
    view_dispatcher_add_view(
        pokemon_fap->view_dispatcher, AppViewSelectPokemon, select_pokemon_get_view(pokemon_fap));

    // Trade View
    pokemon_fap->trade_view = trade_alloc(pokemon_fap);
    view_set_previous_callback(pokemon_fap->trade_view, pokemon_exit_confirm_view);
    view_dispatcher_add_view(pokemon_fap->view_dispatcher, AppViewTrade, pokemon_fap->trade_view);

    view_dispatcher_switch_to_view(pokemon_fap->view_dispatcher, AppViewSelectPokemon);

    return pokemon_fap;
}

void free_app(PokemonFap* pokemon_fap) {
    furi_assert(pokemon_fap);

    // Free views
    view_dispatcher_remove_view(pokemon_fap->view_dispatcher, AppViewSelectPokemon);
    select_pokemon_free(pokemon_fap);

    view_dispatcher_remove_view(pokemon_fap->view_dispatcher, AppViewTrade);
    trade_free(pokemon_fap);

    // Close records
    furi_record_close(RECORD_GUI);

    // Free rest
    free(pokemon_fap);
    pokemon_fap = NULL;
}

extern "C" int32_t pokemon_app(void* p) {
    UNUSED(p);
    //App* app = (App*)pokemon_alloc();
    PokemonFap* pokemon_fap = pokemon_alloc();

    furi_hal_light_set(LightRed, 0x00);
    furi_hal_light_set(LightGreen, 0x00);
    furi_hal_light_set(LightBlue, 0x00);

    //switch view and run dispatcher
    view_dispatcher_run(pokemon_fap->view_dispatcher);

    // Free resources
    free_app(pokemon_fap);

    return 0;
}
