#include "pokemon_app.h"

struct pokemon_lut pokemon_table[] = {
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

uint32_t pokemon_exit_confirm_view(void* context) {
    UNUSED(context);
    return AppViewExitConfirm;
}
App* pokemon_alloc() {
    App* app = (App*)malloc(sizeof(App));

    // Gui
    app->gui = (Gui*)furi_record_open(RECORD_GUI);
    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    //  Start Index first pokemon
    app->current_pokemon = 0;
    // Select Pokemon View
    app->select_pokemon = select_pokemon_alloc(app);
    view_set_previous_callback(select_pokemon_get_view(app), pokemon_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, AppViewSelectPokemon, select_pokemon_get_view(app));

    // Trade View
    app->trade = trade_alloc(app);
    view_set_previous_callback(trade_get_view(app), pokemon_exit_confirm_view);
    view_dispatcher_add_view(app->view_dispatcher, AppViewTrade, trade_get_view(app));

    view_dispatcher_switch_to_view(app->view_dispatcher, AppViewSelectPokemon);

    return app;
}

void free_app(App* app) {
    furi_assert(app);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, AppViewSelectPokemon);
    select_pokemon_free(app);
    view_dispatcher_remove_view(app->view_dispatcher, AppViewTrade);
    trade_free(app);
    // Close records
    furi_record_close(RECORD_GUI);
    app->gui = NULL;

    // Free rest
    free(app);
}

extern "C" int32_t pokemon_app(void* p) {
    UNUSED(p);
    //FURI_LOG_D(TAG, "init scene");
    App* app = (App*)pokemon_alloc();

    furi_hal_light_set(LightRed, 0x00);
    furi_hal_light_set(LightGreen, 0x00);
    furi_hal_light_set(LightBlue, 0x00);
    //switch view  and run dispatcher
    view_dispatcher_run(app->view_dispatcher);

    // Free resources
    free_app(app);
    furi_record_close(RECORD_GUI);

    return 0;
}
