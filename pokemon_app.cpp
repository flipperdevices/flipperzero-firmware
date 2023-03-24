#include "pokemon_app.h"

const char* pokemon_names[] = {
    "bulbasaur",  "ivysaur",   "venusaur",   "charmander", "charmeleon", "charizard",
    "squirtle",   "wartortle", "blastoise",  "caterpie",   "metapod",    "butterfree",
    "weedle",     "kakuna",    "beedrill",   "pidgey",     "pidgeotto",  "pidgeot",
    "rattata",    "raticate",  "spearow",    "fearow",     "ekans",      "arbok",
    "pikachu",    "raichu",    "sandshrew",  "sandslash",  "nidoran ♀",  "nidorina",
    "nidoqueen",  "nidoran ♂", "nidorino",   "nidoking",   "clefairy",   "clefable",
    "vulpix",     "ninetales", "jigglypuff", "wigglytuff", "zubat",      "golbat",
    "oddish",     "gloom",     "vileplume",  "paras",      "parasect",   "venonat",
    "venomoth",   "diglett",   "dugtrio",    "meowth",     "persian",    "psyduck",
    "golduck",    "mankey",    "primeape",   "growlithe",  "arcanine",   "poliwag",
    "poliwhirl",  "poliwrath", "abra",       "kadabra",    "alakazam",   "machop",
    "machoke",    "machamp",   "bellsprout", "weepinbell", "victreebel", "tentacool",
    "tentacruel", "geodude",   "graveler",   "golem",      "ponyta",     "rapidash",
    "slowpoke",   "slowbro",   "magnemite",  "magneton",   "farfetchd",  "doduo",
    "dodrio",     "seel",      "dewgong",    "grimer",     "muk",        "shellder",
    "cloyster",   "gastly",    "haunter",    "gengar",     "onix",       "drowzee",
    "hypno",      "krabby",    "kingler",    "voltorb",    "electrode",  "exeggcute",
    "exeggutor",  "cubone",    "marowak",    "hitmonlee",  "hitmonchan", "lickitung",
    "koffing",    "weezing",   "rhyhorn",    "rhydon",     "chansey",    "tangela",
    "kangaskhan", "horsea",    "seadra",     "goldeen",    "seaking",    "staryu",
    "starmie",    "mr. mime",  "scyther",    "jynx",       "electabuzz", "magmar",
    "pinsir",     "tauros",    "magikarp",   "gyarados",   "lapras",     "ditto",
    "eevee",      "vaporeon",  "jolteon",    "flareon",    "porygon",    "omanyte",
    "omastar",    "kabuto",    "kabutops",   "aerodactyl", "snorlax",    "articuno",
    "zapdos",     "moltres",   "dratini",    "dragonair",  "dragonite",  "mewtwo",
    "mew"};
const Icon* pokemon_icons[] = {
    &I_bulbasaur,  &I_ivysaur,   &I_venusaur,   &I_charmander, &I_charmeleon, &I_charizard,
    &I_squirtle,   &I_wartortle, &I_blastoise,  &I_caterpie,   &I_metapod,    &I_butterfree,
    &I_weedle,     &I_kakuna,    &I_beedrill,   &I_pidgey,     &I_pidgeotto,  &I_pidgeot,
    &I_rattata,    &I_raticate,  &I_spearow,    &I_fearow,     &I_ekans,      &I_arbok,
    &I_pikachu,    &I_raichu,    &I_sandshrew,  &I_sandslash,  &I_nidoranf,   &I_nidorina,
    &I_nidoqueen,  &I_nidoranm,  &I_nidorino,   &I_nidoking,   &I_clefairy,   &I_clefable,
    &I_vulpix,     &I_ninetales, &I_jigglypuff, &I_wigglytuff, &I_zubat,      &I_golbat,
    &I_oddish,     &I_gloom,     &I_vileplume,  &I_paras,      &I_parasect,   &I_venonat,
    &I_venomoth,   &I_diglett,   &I_dugtrio,    &I_meowth,     &I_persian,    &I_psyduck,
    &I_golduck,    &I_mankey,    &I_primeape,   &I_growlithe,  &I_arcanine,   &I_poliwag,
    &I_poliwhirl,  &I_poliwrath, &I_abra,       &I_kadabra,    &I_alakazam,   &I_machop,
    &I_machoke,    &I_machamp,   &I_bellsprout, &I_weepinbell, &I_victreebel, &I_tentacool,
    &I_tentacruel, &I_geodude,   &I_graveler,   &I_golem,      &I_ponyta,     &I_rapidash,
    &I_slowpoke,   &I_slowbro,   &I_magnemite,  &I_magneton,   &I_farfetchd,  &I_doduo,
    &I_dodrio,     &I_seel,      &I_dewgong,    &I_grimer,     &I_muk,        &I_shellder,
    &I_cloyster,   &I_gastly,    &I_haunter,    &I_gengar,     &I_onix,       &I_drowzee,
    &I_hypno,      &I_krabby,    &I_kingler,    &I_voltorb,    &I_electrode,  &I_exeggcute,
    &I_exeggutor,  &I_cubone,    &I_marowak,    &I_hitmonlee,  &I_hitmonchan, &I_lickitung,
    &I_koffing,    &I_weezing,   &I_rhyhorn,    &I_rhydon,     &I_chansey,    &I_tangela,
    &I_kangaskhan, &I_horsea,    &I_seadra,     &I_goldeen,    &I_seaking,    &I_staryu,
    &I_starmie,    &I_mr_mime,   &I_scyther,    &I_jynx,       &I_electabuzz, &I_magmar,
    &I_pinsir,     &I_tauros,    &I_magikarp,   &I_gyarados,   &I_lapras,     &I_ditto,
    &I_eevee,      &I_vaporeon,  &I_jolteon,    &I_flareon,    &I_porygon,    &I_omanyte,
    &I_omastar,    &I_kabuto,    &I_kabutops,   &I_aerodactyl, &I_snorlax,    &I_articuno,
    &I_zapdos,     &I_moltres,   &I_dratini,    &I_dragonair,  &I_dragonite,  &I_mewtwo,
    &I_mew};
const unsigned char pokemon_hex_codes[] = {
    0x99, 0x09, 0x9A, 0xB0, 0xB2, 0xB4, 0xB1, 0xB3, 0x1C, 0x7B, 0x7C, 0x7D, 0x70, 0x71, 0x72, 0x24,
    0x96, 0x97, 0xA5, 0xA6, 0x05, 0x23, 0x6C, 0x2D, 0x54, 0x55, 0x60, 0x61, 0x0F, 0xA8, 0x10, 0x03,
    0xA7, 0x07, 0x04, 0x8E, 0x52, 0x53, 0x64, 0x65, 0x6B, 0x82, 0xB9, 0xBA, 0xBB, 0x6D, 0x2E, 0x41,
    0x77, 0x3B, 0x76, 0x4D, 0x90, 0x2F, 0x80, 0x39, 0x75, 0x21, 0x14, 0x47, 0x6E, 0x6F, 0x94, 0x26,
    0x95, 0x6A, 0x29, 0x7E, 0xBC, 0xBD, 0xBE, 0x18, 0x9B, 0xA9, 0x27, 0x31, 0xA3, 0xA4, 0x25, 0x08,
    0xAD, 0x36, 0x40, 0x46, 0x74, 0x3A, 0x78, 0x0D, 0x88, 0x17, 0x8B, 0x19, 0x93, 0x0E, 0x22, 0x30,
    0x81, 0x4E, 0x8A, 0x06, 0x8D, 0x0C, 0x0A, 0x11, 0x91, 0x2B, 0x2C, 0x0B, 0x37, 0x8F, 0x12, 0x01,
    0x28, 0x1E, 0x02, 0x5C, 0x5D, 0x9D, 0x9E, 0x1B, 0x98, 0x2A, 0x1A, 0x48, 0x35, 0x33, 0x1D, 0x3C,
    0x85, 0x16, 0x13, 0x4C, 0x66, 0x69, 0x68, 0x67, 0xAA, 0x62, 0x63, 0x5A, 0x5B, 0xAB, 0x84, 0x4A,
    0x4B, 0x49, 0x58, 0x59, 0x42, 0x83, 0x15};

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
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    //switch view  and run dispatcher
    view_dispatcher_switch_to_view(app->view_dispatcher, AppViewSelectPokemon);
    view_dispatcher_run(app->view_dispatcher);

    // Free resources
    free_app(app);
    furi_record_close(RECORD_GUI);

    return 0;
}