#include "pokemon_app.h"

struct pokemon_lut pokemon_table[] = {
    {"Bulbasaur", &I_bulbasaur, 0x99, 0x16, 0x03, 1, 45, 49, 49, 45, 65},
    {"Ivysaur", &I_ivysaur, 0x09, 0x16, 0x03, 1, 60, 62, 63, 60, 80},
    {"Venusaur", &I_venusaur, 0x9A, 0x16, 0x03, 1, 80, 82, 83, 80, 100},
    {"Charmander", &I_charmander, 0xB0, 0x14, 0xFF, 1, 39, 52, 43, 65, 50},
    {"Charmeleon", &I_charmeleon, 0xB2, 0x14, 0xFF, 1, 58, 64, 58, 80, 65},
    {"Charizard", &I_charizard, 0xB4, 0x14, 0x02, 1, 78, 84, 78, 100, 85},
    {"Squirtle", &I_squirtle, 0xB1, 0x15, 0xFF, 1, 44, 48, 65, 43, 50},
    {"Wartortle", &I_wartortle, 0xB3, 0x15, 0xFF, 1, 59, 63, 80, 58, 65},
    {"Blastoise", &I_blastoise, 0x1C, 0x15, 0xFF, 1, 79, 83, 100, 78, 85},
    {"Caterpie", &I_caterpie, 0x7B, 0x07, 0xFF, 2, 45, 30, 35, 45, 20},
    {"Metapod", &I_metapod, 0x7C, 0x07, 0xFF, 2, 50, 20, 55, 30, 25},
    {"Butterfree", &I_butterfree, 0x7D, 0x07, 0x02, 2, 60, 45, 50, 70, 80},
    {"Weedle", &I_weedle, 0x70, 0x07, 0x03, 2, 40, 35, 30, 50, 20},
    {"Kakuna", &I_kakuna, 0x71, 0x07, 0x03, 2, 45, 25, 50, 35, 25},
    {"Beedrill", &I_beedrill, 0x72, 0x07, 0x03, 2, 65, 80, 40, 75, 45},
    {"Pidgey", &I_pidgey, 0x24, 0x00, 0x02, 1, 40, 45, 40, 56, 35},
    {"Pidgeotto", &I_pidgeotto, 0x96, 0x00, 0x02, 1, 63, 60, 55, 71, 50},
    {"Pidgeot", &I_pidgeot, 0x97, 0x00, 0x02, 1, 83, 80, 75, 91, 70},
    {"Rattata", &I_rattata, 0xA5, 0x00, 0xFF, 2, 30, 56, 35, 72, 25},
    {"Raticate", &I_raticate, 0xA6, 0x00, 0xFF, 2, 55, 81, 60, 97, 50},
    {"Spearow", &I_spearow, 0x05, 0x00, 0x02, 2, 40, 60, 30, 70, 31},
    {"Fearow", &I_fearow, 0x23, 0x00, 0x02, 2, 65, 90, 65, 100, 61},
    {"Ekans", &I_ekans, 0x6C, 0x03, 0xFF, 2, 35, 60, 44, 55, 40},
    {"Arbok", &I_arbok, 0x2D, 0x03, 0xFF, 2, 60, 85, 69, 80, 65},
    {"Pikachu", &I_pikachu, 0x54, 0x17, 0xFF, 2, 35, 55, 30, 90, 50},
    {"Raichu", &I_raichu, 0x55, 0x17, 0xFF, 2, 60, 90, 55, 100, 90},
    {"Sandshrew", &I_sandshrew, 0x60, 0x04, 0xFF, 2, 50, 75, 85, 40, 30},
    {"Sandslash", &I_sandslash, 0x61, 0x04, 0xFF, 2, 75, 100, 110, 65, 55},
    {"Nidoran@", &I_nidoranf, 0x0F, 0x03, 0xFF, 1, 55, 47, 52, 41, 40},
    {"Nidorina", &I_nidorina, 0xA8, 0x03, 0xFF, 1, 70, 62, 67, 56, 55},
    {"Nidoqueen", &I_nidoqueen, 0x10, 0x03, 0x04, 1, 90, 82, 87, 76, 75},
    {"Nidoran!", &I_nidoranm, 0x03, 0x03, 0xFF, 1, 46, 57, 40, 50, 40},
    {"Nidorino", &I_nidorino, 0xA7, 0x03, 0xFF, 1, 61, 72, 57, 65, 55},
    {"Nidoking", &I_nidoking, 0x07, 0x03, 0x04, 1, 81, 92, 77, 85, 75},
    {"Clefairy", &I_clefairy, 0x04, 0x00, 0xFF, 3, 70, 45, 48, 35, 60},
    {"Clefable", &I_clefable, 0x8E, 0x00, 0xFF, 3, 95, 70, 73, 60, 85},
    {"Vulpix", &I_vulpix, 0x52, 0x14, 0xFF, 2, 38, 41, 40, 65, 65},
    {"Ninetales", &I_ninetales, 0x53, 0x14, 0xFF, 2, 73, 76, 75, 100, 100},
    {"Jigglypuff", &I_jigglypuff, 0x64, 0x00, 0xFF, 3, 115, 45, 20, 20, 25},
    {"Wigglytuff", &I_wigglytuff, 0x65, 0x00, 0xFF, 3, 140, 70, 45, 45, 50},
    {"Zubat", &I_zubat, 0x6B, 0x03, 0x02, 2, 40, 45, 35, 55, 40},
    {"Golbat", &I_golbat, 0x82, 0x03, 0x02, 2, 75, 80, 70, 90, 75},
    {"Oddish", &I_oddish, 0xB9, 0x16, 0x03, 1, 45, 50, 55, 30, 75},
    {"Gloom", &I_gloom, 0xBA, 0x16, 0x03, 1, 60, 65, 70, 40, 85},
    {"Vileplume", &I_vileplume, 0xBB, 0x16, 0x03, 1, 75, 80, 85, 50, 100},
    {"Paras", &I_paras, 0x6D, 0x07, 0x16, 2, 35, 70, 55, 25, 55},
    {"Parasect", &I_parasect, 0x2E, 0x07, 0x16, 2, 60, 95, 80, 30, 80},
    {"Venonat", &I_venonat, 0x41, 0x07, 0x03, 2, 60, 55, 50, 45, 40},
    {"Venomoth", &I_venomoth, 0x77, 0x07, 0x03, 2, 70, 65, 60, 90, 90},
    {"Diglett", &I_diglett, 0x3B, 0x04, 0xFF, 2, 10, 55, 25, 95, 45},
    {"Dugtrio", &I_dugtrio, 0x76, 0x04, 0xFF, 2, 35, 80, 50, 120, 70},
    {"Meowth", &I_meowth, 0x4D, 0x00, 0xFF, 2, 40, 45, 35, 90, 40},
    {"Persian", &I_persian, 0x90, 0x00, 0xFF, 2, 65, 70, 60, 115, 65},
    {"Psyduck", &I_psyduck, 0x2F, 0x15, 0xFF, 2, 50, 52, 48, 55, 50},
    {"Golduck", &I_golduck, 0x80, 0x15, 0xFF, 2, 80, 82, 78, 85, 80},
    {"Mankey", &I_mankey, 0x39, 0x01, 0xFF, 2, 40, 80, 35, 70, 35},
    {"Primeape", &I_primeape, 0x75, 0x01, 0xFF, 2, 65, 105, 60, 95, 60},
    {"Growlithe", &I_growlithe, 0x21, 0x14, 0xFF, 0, 55, 70, 45, 60, 50},
    {"Arcanine", &I_arcanine, 0x14, 0x14, 0xFF, 0, 90, 110, 80, 95, 80},
    {"Poliwag", &I_poliwag, 0x47, 0x15, 0xFF, 1, 40, 50, 40, 90, 40},
    {"Poliwhirl", &I_poliwhirl, 0x6E, 0x15, 0xFF, 1, 65, 65, 65, 90, 50},
    {"Poliwrath", &I_poliwrath, 0x6F, 0x15, 0x01, 1, 90, 85, 95, 70, 70},
    {"Abra", &I_abra, 0x94, 0x18, 0xFF, 1, 25, 20, 15, 90, 105},
    {"Kadabra", &I_kadabra, 0x26, 0x18, 0xFF, 1, 40, 35, 30, 105, 120},
    {"Alakazam", &I_alakazam, 0x95, 0x18, 0xFF, 1, 55, 50, 45, 120, 135},
    {"Machop", &I_machop, 0x6A, 0x01, 0xFF, 1, 70, 80, 50, 35, 35},
    {"Machoke", &I_machoke, 0x29, 0x01, 0xFF, 1, 80, 100, 70, 45, 50},
    {"Machamp", &I_machamp, 0x7E, 0x01, 0xFF, 1, 90, 130, 80, 55, 65},
    {"Bellsprout", &I_bellsprout, 0xBC, 0x16, 0x03, 1, 50, 75, 35, 40, 70},
    {"Weepinbell", &I_weepinbell, 0xBD, 0x16, 0x03, 1, 65, 90, 50, 55, 85},
    {"Victreebel", &I_victreebel, 0xBE, 0x16, 0x03, 1, 80, 105, 65, 70, 100},
    {"Tentacool", &I_tentacool, 0x18, 0x15, 0x03, 0, 40, 40, 35, 70, 100},
    {"Tentacruel", &I_tentacruel, 0x9B, 0x15, 0x03, 0, 80, 70, 65, 100, 120},
    {"Geodude", &I_geodude, 0xA9, 0x05, 0x04, 1, 40, 80, 100, 20, 30},
    {"Graveler", &I_graveler, 0x27, 0x05, 0x04, 1, 55, 95, 115, 35, 45},
    {"Golem", &I_golem, 0x31, 0x05, 0x04, 1, 80, 110, 130, 45, 55},
    {"Ponyta", &I_ponyta, 0xA3, 0x14, 0xFF, 2, 50, 85, 55, 90, 65},
    {"Rapidash", &I_rapidash, 0xA4, 0x14, 0xFF, 2, 65, 100, 70, 105, 80},
    {"Slowpoke", &I_slowpoke, 0x25, 0x15, 0x18, 2, 90, 65, 65, 15, 40},
    {"Slowbro", &I_slowbro, 0x08, 0x15, 0x18, 2, 95, 75, 110, 30, 80},
    {"Magnemite", &I_magnemite, 0xAD, 0x17, 0xFF, 2, 25, 35, 70, 45, 95},
    {"Magneton", &I_magneton, 0x36, 0x17, 0xFF, 2, 50, 60, 95, 70, 120},
    {"Farfetch'd", &I_farfetchd, 0x40, 0x00, 0x02, 2, 52, 65, 55, 60, 58},
    {"Doduo", &I_doduo, 0x46, 0x00, 0x02, 2, 35, 85, 45, 75, 35},
    {"Dodrio", &I_dodrio, 0x74, 0x00, 0x02, 2, 60, 110, 70, 100, 60},
    {"Seel", &I_seel, 0x3A, 0x15, 0xFF, 2, 65, 45, 55, 45, 70},
    {"Dewgong", &I_dewgong, 0x78, 0x15, 0x19, 2, 90, 70, 80, 70, 95},
    {"Grimer", &I_grimer, 0x0D, 0x03, 0xFF, 2, 80, 80, 50, 25, 40},
    {"Muk", &I_muk, 0x88, 0x03, 0xFF, 2, 105, 105, 75, 50, 65},
    {"Shellder", &I_shellder, 0x17, 0x15, 0xFF, 0, 30, 65, 100, 40, 45},
    {"Cloyster", &I_cloyster, 0x8B, 0x15, 0x19, 0, 50, 95, 180, 70, 85},
    {"Gastly", &I_gastly, 0x19, 0x08, 0x03, 1, 30, 35, 30, 80, 100},
    {"Haunter", &I_haunter, 0x93, 0x08, 0x03, 1, 45, 50, 45, 95, 115},
    {"Gengar", &I_gengar, 0x0E, 0x08, 0x03, 1, 60, 65, 60, 110, 130},
    {"Onix", &I_onix, 0x22, 0x05, 0x04, 2, 35, 45, 160, 70, 30},
    {"Drowzee", &I_drowzee, 0x30, 0x18, 0xFF, 2, 60, 48, 45, 42, 90},
    {"Hypno", &I_hypno, 0x81, 0x18, 0xFF, 2, 85, 73, 70, 67, 115},
    {"Krabby", &I_krabby, 0x4E, 0x15, 0xFF, 2, 30, 105, 90, 50, 25},
    {"Kingler", &I_kingler, 0x8A, 0x15, 0xFF, 2, 55, 130, 115, 75, 50},
    {"Voltorb", &I_voltorb, 0x06, 0x17, 0xFF, 2, 40, 30, 50, 100, 55},
    {"Electrode", &I_electrode, 0x8D, 0x17, 0xFF, 2, 60, 50, 70, 140, 80},
    {"Exeggcute", &I_exeggcute, 0x0C, 0x16, 0x18, 0, 60, 40, 80, 40, 60},
    {"Exeggutor", &I_exeggutor, 0x0A, 0x16, 0x18, 0, 95, 95, 85, 55, 125},
    {"Cubone", &I_cubone, 0x11, 0x04, 0xFF, 2, 50, 50, 95, 35, 40},
    {"Marowak", &I_marowak, 0x91, 0x04, 0xFF, 2, 60, 80, 110, 45, 50},
    {"Hitmonlee", &I_hitmonlee, 0x2B, 0x01, 0xFF, 2, 50, 120, 53, 87, 35},
    {"Hitmonchan", &I_hitmonchan, 0x2C, 0x01, 0xFF, 2, 50, 105, 79, 76, 35},
    {"Lickitung", &I_lickitung, 0x0B, 0x00, 0xFF, 2, 90, 55, 75, 30, 60},
    {"Koffing", &I_koffing, 0x37, 0x03, 0xFF, 2, 40, 65, 95, 35, 60},
    {"Weezing", &I_weezing, 0x8F, 0x03, 0xFF, 2, 65, 90, 120, 60, 85},
    {"Rhyhorn", &I_rhyhorn, 0x12, 0x04, 0x05, 0, 80, 85, 95, 25, 30},
    {"Rhydon", &I_rhydon, 0x01, 0x04, 0x05, 0, 105, 130, 120, 40, 45},
    {"Chansey", &I_chansey, 0x28, 0x00, 0xFF, 3, 250, 5, 5, 50, 105},
    {"Tangela", &I_tangela, 0x1E, 0x16, 0xFF, 2, 65, 55, 115, 60, 100},
    {"Kangaskhan", &I_kangaskhan, 0x02, 0x00, 0xFF, 2, 105, 95, 80, 90, 40},
    {"Horsea", &I_horsea, 0x5C, 0x15, 0xFF, 2, 30, 40, 70, 60, 70},
    {"Seadra", &I_seadra, 0x5D, 0x15, 0xFF, 2, 55, 65, 95, 85, 95},
    {"Goldeen", &I_goldeen, 0x9D, 0x15, 0xFF, 2, 45, 67, 60, 63, 50},
    {"Seaking", &I_seaking, 0x9E, 0x15, 0xFF, 2, 80, 92, 65, 68, 80},
    {"Staryu", &I_staryu, 0x1B, 0x15, 0xFF, 0, 30, 45, 55, 85, 70},
    {"Starmie", &I_starmie, 0x98, 0x15, 0x18, 0, 60, 75, 85, 115, 100},
    {"Mr. Mime", &I_mr_mime, 0x2A, 0x18, 0xFF, 2, 40, 45, 65, 90, 100},
    {"Scyther", &I_scyther, 0x1A, 0x07, 0x02, 2, 70, 110, 80, 105, 55},
    {"Jynx", &I_jynx, 0x48, 0x19, 0x18, 2, 65, 50, 35, 95, 95},
    {"Electabuzz", &I_electabuzz, 0x35, 0x17, 0xFF, 2, 65, 83, 57, 105, 85},
    {"Magmar", &I_magmar, 0x33, 0x14, 0xFF, 2, 65, 95, 57, 93, 85},
    {"Pinsir", &I_pinsir, 0x1D, 0x07, 0xFF, 0, 65, 125, 100, 85, 55},
    {"Tauros", &I_tauros, 0x3C, 0x00, 0xFF, 0, 75, 100, 95, 110, 70},
    {"Magikarp", &I_magikarp, 0x85, 0x15, 0xFF, 0, 20, 10, 55, 80, 20},
    {"Gyarados", &I_gyarados, 0x16, 0x15, 0x02, 0, 95, 125, 79, 81, 100},
    {"Lapras", &I_lapras, 0x13, 0x15, 0x19, 0, 130, 85, 80, 60, 95},
    {"Ditto", &I_ditto, 0x4C, 0x00, 0xFF, 2, 48, 48, 48, 48, 48},
    {"Eevee", &I_eevee, 0x66, 0x00, 0xFF, 2, 55, 55, 50, 55, 65},
    {"Vaporeon", &I_vaporeon, 0x69, 0x15, 0xFF, 2, 130, 65, 60, 65, 110},
    {"Jolteon", &I_jolteon, 0x68, 0x17, 0xFF, 2, 65, 65, 60, 130, 110},
    {"Flareon", &I_flareon, 0x67, 0x14, 0xFF, 2, 65, 130, 60, 65, 110},
    {"Porygon", &I_porygon, 0xAA, 0x00, 0xFF, 2, 65, 60, 70, 40, 75},
    {"Omanyte", &I_omanyte, 0x62, 0x05, 0x15, 2, 35, 40, 100, 35, 90},
    {"Omastar", &I_omastar, 0x63, 0x05, 0x15, 2, 70, 60, 125, 55, 115},
    {"Kabuto", &I_kabuto, 0x5A, 0x05, 0x15, 2, 30, 80, 90, 55, 45},
    {"Kabutops", &I_kabutops, 0x5B, 0x05, 0x15, 2, 60, 115, 105, 80, 70},
    {"Aerodactyl", &I_aerodactyl, 0xAB, 0x05, 0x02, 0, 80, 105, 65, 130, 60},
    {"Snorlax", &I_snorlax, 0x84, 0x00, 0xFF, 0, 160, 110, 65, 30, 65},
    {"Articuno", &I_articuno, 0x4A, 0x19, 0x02, 0, 90, 85, 100, 85, 125},
    {"Zapdos", &I_zapdos, 0x4B, 0x17, 0x02, 0, 90, 90, 85, 100, 125},
    {"Moltres", &I_moltres, 0x49, 0x14, 0x02, 0, 90, 100, 90, 90, 125},
    {"Dratini", &I_dratini, 0x58, 0x1A, 0xFF, 0, 41, 64, 45, 50, 50},
    {"Dragonair", &I_dragonair, 0x59, 0x1A, 0xFF, 0, 61, 84, 65, 70, 70},
    {"Dragonite", &I_dragonite, 0x42, 0x1A, 0x02, 0, 91, 134, 95, 80, 100},
    {"Mewtwo", &I_mewtwo, 0x83, 0x18, 0xFF, 0, 106, 110, 90, 130, 154},
    {"Mew", &I_mew, 0x15, 0x18, 0xFF, 1, 100, 100, 100, 100, 100},
    {},
};

struct pokemon_mv move_table[] = {
    {"No Move", 0x00},      {"Absorb", 0x47},       {"Acid Armor", 0x97},   {"Acid", 0x33},
    {"Agility", 0x61},      {"Amnesia", 0x85},      {"Aurora Beam", 0x3E},  {"Barrage", 0x8C},
    {"Barrier", 0x70},      {"Bide", 0x75},         {"Bind", 0x14},         {"Bite", 0x2C},
    {"Blizzard", 0x3B},     {"Body Slam", 0x22},    {"Bone Club", 0x7D},    {"Boomerang", 0x9B},
    {"Bubblebeam", 0x3D},   {"Bubble", 0x91},       {"Clamp", 0x80},        {"Comet Punch", 0x04},
    {"Confuse Ray", 0x6D},  {"Confusion", 0x5D},    {"Constrict", 0x84},    {"Conversion", 0xA0},
    {"Counter", 0x44},      {"Crabhammer", 0x98},   {"Cut", 0x0F},          {"Defense Curl", 0x6F},
    {"Dig", 0x5B},          {"Disable", 0x32},      {"Dizzy Punch", 0x92},  {"Doubleslap", 0x03},
    {"Double Kick", 0x18},  {"Double Team", 0x68},  {"Double-Edge", 0x26},  {"Dragon Rage", 0x52},
    {"Dream Eater", 0x8A},  {"Drill Peck", 0x41},   {"Earthquake", 0x59},   {"Egg Bomb", 0x79},
    {"Ember", 0x34},        {"Explosion", 0x99},    {"Fire Blast", 0x7E},   {"Fire Punch", 0x07},
    {"Fire Spin", 0x53},    {"Fissure", 0x5A},      {"Flamethrower", 0x35}, {"Flash", 0x94},
    {"Fly", 0x13},          {"Focus Energy", 0x74}, {"Fury Attack", 0x1F},  {"Fury Swipes", 0x9A},
    {"Glare", 0x89},        {"Growl", 0x2D},        {"Growth", 0x4A},       {"Guillotine", 0x0C},
    {"Gust", 0x10},         {"Harden", 0x6A},       {"Haze", 0x72},         {"Headbutt", 0x1D},
    {"Hi Jump Kick", 0x88}, {"Horn Attack", 0x1E},  {"Horn Drill", 0x20},   {"Hydro Pump", 0x38},
    {"Hyper Beam", 0x3F},   {"Hyper Fang", 0x9E},   {"Hypnosis", 0x5F},     {"Ice Beam", 0x3A},
    {"Ice Punch", 0x08},    {"Jump Kick", 0x1A},    {"Karate Chop", 0x02},  {"Kinesis", 0x86},
    {"Leech Life", 0x8D},   {"Leech Seed", 0x49},   {"Leer", 0x2B},         {"Lick", 0x7A},
    {"Light Screen", 0x71}, {"Lovely Kiss", 0x8E},  {"Low Kick", 0x43},     {"Meditate", 0x60},
    {"Mega Drain", 0x48},   {"Mega Kick", 0x19},    {"Mega Punch", 0x05},   {"Metronome", 0x76},
    {"Mimic", 0x66},        {"Minimize", 0x6B},     {"Mirror Move", 0x77},  {"Mist", 0x36},
    {"Night Shade", 0x65},  {"Pay Day", 0x06},      {"Peck", 0x40},         {"Petal Dance", 0x50},
    {"Pin Missile", 0x2A},  {"Poisonpowder", 0x4D}, {"Poison Gas", 0x8B},   {"Poison Sting", 0x28},
    {"Pound", 0x01},        {"Psybeam", 0x3C},      {"Psychic", 0x5E},      {"Psywave", 0x95},
    {"Quick Attack", 0x62}, {"Rage", 0x63},         {"Razor Leaf", 0x4B},   {"Razor Wind", 0x0D},
    {"Recover", 0x69},      {"Reflect", 0x73},      {"Rest", 0x9C},         {"Roar", 0x2E},
    {"Rock Slide", 0x9D},   {"Rock Throw", 0x58},   {"Rolling Kick", 0x1B}, {"Sand Attack", 0x1C},
    {"Scratch", 0x0A},      {"Screech", 0x67},      {"Seismic Toss", 0x45}, {"Selfdestruct", 0x78},
    {"Sharpen", 0x9F},      {"Sing", 0x2F},         {"Skull Bash", 0x82},   {"Sky Attack", 0x8F},
    {"Slam", 0x15},         {"Slash", 0xA3},        {"Sleep Powder", 0x4F}, {"Sludge", 0x7C},
    {"Smog", 0x7B},         {"Smokescreen", 0x6C},  {"Softboiled", 0x87},   {"Solar Beam", 0x4C},
    {"Sonicboom", 0x31},    {"Spike Cannon", 0x83}, {"Splash", 0x96},       {"Spore", 0x93},
    {"Stomp", 0x17},        {"Strength", 0x46},     {"String Shot", 0x51},  {"Struggle", 0xA5},
    {"Stun Spore", 0x4E},   {"Submission", 0x42},   {"Substitute", 0xA4},   {"Supersonic", 0x30},
    {"Super Fang", 0xA2},   {"Surf", 0x39},         {"Swift", 0x81},        {"Swords Dance", 0x0E},
    {"Tackle", 0x21},       {"Tail Whip", 0x27},    {"Take Down", 0x24},    {"Teleport", 0x64},
    {"Thrash", 0x25},       {"Thunderbolt", 0x55},  {"Thunderpunch", 0x09}, {"Thundershock", 0x54},
    {"Thunder Wave", 0x56}, {"Thunder", 0x57},      {"Toxic", 0x5C},        {"Transform", 0x90},
    {"Tri Attack", 0xA1},   {"Twineedle", 0x29},    {"Vicegrip", 0x0B},     {"Vine Whip", 0x16},
    {"Waterfall", 0x7F},    {"Water Gun", 0x37},    {"Whirlwind", 0x12},    {"Wing Attack", 0x11},
    {"Withdraw", 0x6E},     {"Wrap", 0x23},
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

    //  Start Index first level
    app->current_level = 3;
    // Select Level View
    app->select_level = select_level_alloc(app);
    view_set_previous_callback(select_level_get_view(app), pokemon_exit_confirm_view);
    view_dispatcher_add_view(app->view_dispatcher, AppViewSelectLevel, select_level_get_view(app));

    //  Start Index first stat
    app->current_stats = 0;
    // Select Level View
    app->select_stats = select_stats_alloc(app);
    view_set_previous_callback(select_stats_get_view(app), pokemon_exit_confirm_view);
    view_dispatcher_add_view(app->view_dispatcher, AppViewSelectStats, select_stats_get_view(app));

    //  Start Index first move
    app->current_move = 0;
    // Select Move View
    app->select_move1 = select_move1_alloc(app);
    view_set_previous_callback(select_move1_get_view(app), pokemon_exit_confirm_view);
    view_dispatcher_add_view(app->view_dispatcher, AppViewSelectMove1, select_move1_get_view(app));

    //  Start Index first move
    app->current_move = 0;
    // Select Move View
    app->select_move2 = select_move2_alloc(app);
    view_set_previous_callback(select_move1_get_view(app), pokemon_exit_confirm_view);
    view_dispatcher_add_view(app->view_dispatcher, AppViewSelectMove2, select_move2_get_view(app));

    //  Start Index first move
    app->current_move = 0;
    // Select Move View
    app->select_move3 = select_move3_alloc(app);
    view_set_previous_callback(select_move3_get_view(app), pokemon_exit_confirm_view);
    view_dispatcher_add_view(app->view_dispatcher, AppViewSelectMove3, select_move3_get_view(app));

    //  Start Index first move
    app->current_move = 0;
    // Select Move View
    app->select_move4 = select_move4_alloc(app);
    view_set_previous_callback(select_move4_get_view(app), pokemon_exit_confirm_view);
    view_dispatcher_add_view(app->view_dispatcher, AppViewSelectMove4, select_move4_get_view(app));

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
    view_dispatcher_remove_view(app->view_dispatcher, AppViewSelectLevel);
    select_level_free(app);
    view_dispatcher_remove_view(app->view_dispatcher, AppViewSelectStats);
    select_stats_free(app);
    view_dispatcher_remove_view(app->view_dispatcher, AppViewSelectMove1);
    select_move1_free(app);
    view_dispatcher_remove_view(app->view_dispatcher, AppViewSelectMove2);
    select_move2_free(app);
    view_dispatcher_remove_view(app->view_dispatcher, AppViewSelectMove3);
    select_move3_free(app);
    view_dispatcher_remove_view(app->view_dispatcher, AppViewSelectMove4);
    select_move4_free(app);
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
