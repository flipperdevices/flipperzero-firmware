#include "../malveke_gb_photo.h"
#include <lib/toolbox/value_index.h>

enum SettingsIndex {
    SettingsIndexHaptic = 10,
    SettingsIndexValue1,
    SettingsIndexValue2,
};

const char* const palette_text[57] = {
    "B&W",
    "Original",
    "Splash Up",
    "GB Light",
    "Pocket",
    "aqpp",
    "azc",
    "banana",
    "bgb",
    "blackzero",
    "cctr",
    "cfp",
    "cga1",
    "cga2",
    "chig",
    "cmyk",
    "cybl",
    "d2kr",
    "datn",
    "dhg",
    "dimwm",
    "ffs",
    "fsil",
    "gbcd",
    "gbcda",
    "gbcdb",
    "gbceuus",
    "gbcl",
    "gbcla",
    "gbclb",
    "gbcr",
    "gbcrb",
    "gbcua",
    "gbcub",
    "gelc",
    "glmo",
    "grafixkidgray",
    "grafixkidgreen",
    "hipster",
    "kditw",
    "llawk",
    "marmx",
    "nc",
    "ppr",
    "rcs",
    "roga",
    "sfh",
    "shmgy",
    "shzol",
    "slmem",
    "spezi",
    "tdoyc",
    "tpa",
    "tsk",
    "vb85",
    "wtfp",
    "yirl"};
const uint32_t palette_value[57] = {
    BoilerplatePaletteBlackAndWhite,
    BoilerplatePaletteOriginal,
    BoilerplatePaletteSplashUp,
    BoilerplatePaletteGBLight,
    BoilerplatePalettePocket,
    BoilerplatePaletteAudiQuattroPikesPeak,
    BoilerplatePaletteAzureClouds,
    BoilerplatePaletteTheresalwaysmoney,
    BoilerplatePaletteBGBEmulator,
    BoilerplatePaletteGameBoyBlackZeropalette,
    BoilerplatePaletteCandyCottonTowerRaid,
    BoilerplatePaletteCaramelFudgeParanoia,
    BoilerplatePaletteCGAPaletteCrush1,
    BoilerplatePaletteCGAPaletteCrush2,
    BoilerplatePaletteChildhoodinGreenland,
    BoilerplatePaletteCMYKeystone,
    BoilerplatePaletteCyanideBlues,
    BoilerplatePaletteDune2000remastered,
    BoilerplatePaletteDrowningatnight,
    BoilerplatePaletteDeepHazeGreen,
    BoilerplatePaletteDiesistmeineWassermelone,
    BoilerplatePaletteFlowerfeldstrabe,
    BoilerplatePaletteFloydSteinberginLove,
    BoilerplatePaletteGameBoyColorSplashDown,
    BoilerplatePaletteGameBoyColorSplashDownA,
    BoilerplatePaletteGameBoyColorSplashDownB,
    BoilerplatePaletteGameBoyColorSplashRightAGameBoyCamera,
    BoilerplatePaletteGameBoyColorSplashLeft,
    BoilerplatePaletteGameBoyColorSplashLeftA,
    BoilerplatePaletteGameBoyColorSplashLeftB,
    BoilerplatePaletteGameBoyColorSplashRight,
    BoilerplatePaletteGameBoyColorSplashRightB,
    BoilerplatePaletteGameBoyColorSplashUpA,
    BoilerplatePaletteGameBoyColorSplashUpB,
    BoilerplatePaletteGoldenElephantCurry,
    BoilerplatePaletteGlowingMountains,
    BoilerplatePaletteGrafixkidGray,
    BoilerplatePaletteGrafixkidGreen,
    BoilerplatePaletteArtisticCaffeinatedLactose,
    BoilerplatePaletteKneeDeepintheWood,
    BoilerplatePaletteLinkslateAwakening,
    BoilerplatePaletteMetroidAranremixed,
    BoilerplatePaletteNortoriousComandante,
    BoilerplatePalettePurpleRain,
    BoilerplatePaletteRustedCitySign,
    BoilerplatePaletteRomerosGarden,
    BoilerplatePaletteSunflowerHolidays,
    BoilerplatePaletteSuperHyperMegaGameboy,
    BoilerplatePaletteSpaceHazeOverload,
    BoilerplatePaletteStarlitMemories,
    BoilerplatePaletteMyFriendfromBavaria,
    BoilerplatePaletteThedeathofYungColumbus,
    BoilerplatePaletteTramontoalParcodegliAcquedotti,
    BoilerplatePaletteThestarryknight,
    BoilerplatePaletteVirtualBoy1985,
    BoilerplatePaletteWaterfrontPlaza,
    BoilerplatePaletteYouthIkarusreloaded};

const char* const info_text[2] = {
    "OFF",
    "ON",
};
const uint32_t info_value[2] = {
    BoilerplateInfoOff,
    BoilerplateInfoOn,
};

static void boilerplate_scene_settings_set_palette(VariableItem* item) {
    Boilerplate* app = variable_item_get_context(item);
    UNUSED(app);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, palette_text[index]);
    app->palette = palette_value[index];
    if(app->palette == BoilerplatePaletteBlackAndWhite) {
        app->palette_color_hex_a = 0xFFFFFF;
        app->palette_color_hex_b = 0xAAAAAA;
        app->palette_color_hex_c = 0x555555;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteOriginal) {
        app->palette_color_hex_a = 0x9bbc0f;
        app->palette_color_hex_b = 0x77a112;
        app->palette_color_hex_c = 0x306230;
        app->palette_color_hex_d = 0x0f380f;
    } else if(app->palette == BoilerplatePaletteSplashUp) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0xffad63;
        app->palette_color_hex_c = 0x833100;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGBLight) {
        app->palette_color_hex_a = 0x1ddece;
        app->palette_color_hex_b = 0x19c7b3;
        app->palette_color_hex_c = 0x16a596;
        app->palette_color_hex_d = 0x0b7a6d;
    } else if(app->palette == BoilerplatePalettePocket) {
        app->palette_color_hex_a = 0xc4cfa1;
        app->palette_color_hex_b = 0x8b956d;
        app->palette_color_hex_c = 0x4d533c;
        app->palette_color_hex_d = 0x1f1f1f;
    } else if(app->palette == BoilerplatePaletteAudiQuattroPikesPeak) {
        app->palette_color_hex_a = 0xebeee7;
        app->palette_color_hex_b = 0x868779;
        app->palette_color_hex_c = 0xfa2b25;
        app->palette_color_hex_d = 0x2a201e;
    } else if(app->palette == BoilerplatePaletteAzureClouds) {
        app->palette_color_hex_a = 0x47ff99;
        app->palette_color_hex_b = 0x32b66d;
        app->palette_color_hex_c = 0x124127;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteTheresalwaysmoney) {
        app->palette_color_hex_a = 0xfdfe0a;
        app->palette_color_hex_b = 0xfed638;
        app->palette_color_hex_c = 0x977b25;
        app->palette_color_hex_d = 0x221a09;
    } else if(app->palette == BoilerplatePaletteBGBEmulator) {
        app->palette_color_hex_a = 0xe0f8d0;
        app->palette_color_hex_b = 0x88c070;
        app->palette_color_hex_c = 0x346856;
        app->palette_color_hex_d = 0x081820;
    } else if(app->palette == BoilerplatePaletteGameBoyBlackZeropalette) {
        app->palette_color_hex_a = 0x7e8416;
        app->palette_color_hex_b = 0x577b46;
        app->palette_color_hex_c = 0x385d49;
        app->palette_color_hex_d = 0x2e463d;
    } else if(app->palette == BoilerplatePaletteCandyCottonTowerRaid) {
        app->palette_color_hex_a = 0xe6aec4;
        app->palette_color_hex_b = 0xe65790;
        app->palette_color_hex_c = 0x8f0039;
        app->palette_color_hex_d = 0x380016;
    } else if(app->palette == BoilerplatePaletteCaramelFudgeParanoia) {
        app->palette_color_hex_a = 0xcf9255;
        app->palette_color_hex_b = 0xcf7163;
        app->palette_color_hex_c = 0xb01553;
        app->palette_color_hex_d = 0x3f1711;
    } else if(app->palette == BoilerplatePaletteCGAPaletteCrush1) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0x55ffff;
        app->palette_color_hex_c = 0xff55ff;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteCGAPaletteCrush2) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0x55ffff;
        app->palette_color_hex_c = 0xff5555;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteChildhoodinGreenland) {
        app->palette_color_hex_a = 0xd0d058;
        app->palette_color_hex_b = 0xa0a840;
        app->palette_color_hex_c = 0x708028;
        app->palette_color_hex_d = 0x405010;
    } else if(app->palette == BoilerplatePaletteCMYKeystone) {
        app->palette_color_hex_a = 0xffff00;
        app->palette_color_hex_b = 0x0be8fd;
        app->palette_color_hex_c = 0xfb00fa;
        app->palette_color_hex_d = 0x373737;
    } else if(app->palette == BoilerplatePaletteCyanideBlues) {
        app->palette_color_hex_a = 0x9efbe3;
        app->palette_color_hex_b = 0x21aff5;
        app->palette_color_hex_c = 0x1e4793;
        app->palette_color_hex_d = 0x0e1e3d;
    } else if(app->palette == BoilerplatePaletteDune2000remastered) {
        app->palette_color_hex_a = 0xfbf1cd;
        app->palette_color_hex_b = 0xc09e7d;
        app->palette_color_hex_c = 0x725441;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteDrowningatnight) {
        app->palette_color_hex_a = 0xa9b0b3;
        app->palette_color_hex_b = 0x586164;
        app->palette_color_hex_c = 0x20293f;
        app->palette_color_hex_d = 0x030c22;
    } else if(app->palette == BoilerplatePaletteDeepHazeGreen) {
        app->palette_color_hex_a = 0xa1d909;
        app->palette_color_hex_b = 0x467818;
        app->palette_color_hex_c = 0x27421f;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteDiesistmeineWassermelone) {
        app->palette_color_hex_a = 0xffdbcb;
        app->palette_color_hex_b = 0xf27d7a;
        app->palette_color_hex_c = 0x558429;
        app->palette_color_hex_d = 0x222903;
    } else if(app->palette == BoilerplatePaletteFlowerfeldstrabe) {
        app->palette_color_hex_a = 0xe9d9cc;
        app->palette_color_hex_b = 0xc5c5ce;
        app->palette_color_hex_c = 0x75868f;
        app->palette_color_hex_d = 0x171f62;
    } else if(app->palette == BoilerplatePaletteFloydSteinberginLove) {
        app->palette_color_hex_a = 0xeaf5fa;
        app->palette_color_hex_b = 0x5fb1f5;
        app->palette_color_hex_c = 0xd23c4e;
        app->palette_color_hex_d = 0x4c1c2d;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashDown) {
        app->palette_color_hex_a = 0xffffa5;
        app->palette_color_hex_b = 0xfe9494;
        app->palette_color_hex_c = 0x9394fe;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashDownA) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0xffff00;
        app->palette_color_hex_c = 0xfe0000;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashDownB) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0xffff00;
        app->palette_color_hex_c = 0x7d4900;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashRightAGameBoyCamera) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0x7bff30;
        app->palette_color_hex_c = 0x0163c6;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashLeft) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0x65a49b;
        app->palette_color_hex_c = 0x0000fe;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashLeftA) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0x8b8cde;
        app->palette_color_hex_c = 0x53528c;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashLeftB) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0xa5a5a5;
        app->palette_color_hex_c = 0x525252;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashRight) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0x51ff00;
        app->palette_color_hex_c = 0xff4200;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashRightB) {
        app->palette_color_hex_a = 0x000000;
        app->palette_color_hex_b = 0x008486;
        app->palette_color_hex_c = 0xffde00;
        app->palette_color_hex_d = 0xffffff;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashUpA) {
        app->palette_color_hex_a = 0xffffff;
        app->palette_color_hex_b = 0xff8f84;
        app->palette_color_hex_c = 0x943a3a;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGameBoyColorSplashUpB) {
        app->palette_color_hex_a = 0xffe7c5;
        app->palette_color_hex_b = 0xce9c85;
        app->palette_color_hex_c = 0x846b29;
        app->palette_color_hex_d = 0x5b3109;
    } else if(app->palette == BoilerplatePaletteGoldenElephantCurry) {
        app->palette_color_hex_a = 0xff9c00;
        app->palette_color_hex_b = 0xc27600;
        app->palette_color_hex_c = 0x4f3000;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteGlowingMountains) {
        app->palette_color_hex_a = 0xffbf98;
        app->palette_color_hex_b = 0xa1a8b8;
        app->palette_color_hex_c = 0x514f6c;
        app->palette_color_hex_d = 0x2f1c35;
    } else if(app->palette == BoilerplatePaletteGrafixkidGray) {
        app->palette_color_hex_a = 0xe0dbcd;
        app->palette_color_hex_b = 0xa89f94;
        app->palette_color_hex_c = 0x706b66;
        app->palette_color_hex_d = 0x2b2b26;
    } else if(app->palette == BoilerplatePaletteGrafixkidGreen) {
        app->palette_color_hex_a = 0xdbf4b4;
        app->palette_color_hex_b = 0xabc396;
        app->palette_color_hex_c = 0x7b9278;
        app->palette_color_hex_d = 0x4c625a;
    } else if(app->palette == BoilerplatePaletteArtisticCaffeinatedLactose) {
        app->palette_color_hex_a = 0xfdfef5;
        app->palette_color_hex_b = 0xdea963;
        app->palette_color_hex_c = 0x9e754f;
        app->palette_color_hex_d = 0x241606;
    } else if(app->palette == BoilerplatePaletteKneeDeepintheWood) {
        app->palette_color_hex_a = 0xfffe6e;
        app->palette_color_hex_b = 0xd5690f;
        app->palette_color_hex_c = 0x3c3ca9;
        app->palette_color_hex_d = 0x2c2410;
    } else if(app->palette == BoilerplatePaletteLinkslateAwakening) {
        app->palette_color_hex_a = 0xffffb5;
        app->palette_color_hex_b = 0x7bc67b;
        app->palette_color_hex_c = 0x6b8c42;
        app->palette_color_hex_d = 0x5a3921;
    } else if(app->palette == BoilerplatePaletteMetroidAranremixed) {
        app->palette_color_hex_a = 0xaedf1e;
        app->palette_color_hex_b = 0x047e60;
        app->palette_color_hex_c = 0xb62558;
        app->palette_color_hex_d = 0x2c1700;
    } else if(app->palette == BoilerplatePaletteNortoriousComandante) {
        app->palette_color_hex_a = 0xfcfe54;
        app->palette_color_hex_b = 0x54fefc;
        app->palette_color_hex_c = 0x04aaac;
        app->palette_color_hex_d = 0x0402ac;
    } else if(app->palette == BoilerplatePalettePurpleRain) {
        app->palette_color_hex_a = 0xadfffc;
        app->palette_color_hex_b = 0x8570b2;
        app->palette_color_hex_c = 0xff0084;
        app->palette_color_hex_d = 0x68006a;
    } else if(app->palette == BoilerplatePaletteRustedCitySign) {
        app->palette_color_hex_a = 0xedb4a1;
        app->palette_color_hex_b = 0xa96868;
        app->palette_color_hex_c = 0x764462;
        app->palette_color_hex_d = 0x2c2137;
    } else if(app->palette == BoilerplatePaletteRomerosGarden) {
        app->palette_color_hex_a = 0xebc4ab;
        app->palette_color_hex_b = 0x649a57;
        app->palette_color_hex_c = 0x574431;
        app->palette_color_hex_d = 0x323727;
    } else if(app->palette == BoilerplatePaletteSunflowerHolidays) {
        app->palette_color_hex_a = 0xffff55;
        app->palette_color_hex_b = 0xff5555;
        app->palette_color_hex_c = 0x881400;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteSuperHyperMegaGameboy) {
        app->palette_color_hex_a = 0xf7e7c6;
        app->palette_color_hex_b = 0xd68e49;
        app->palette_color_hex_c = 0xa63725;
        app->palette_color_hex_d = 0x331e50;
    } else if(app->palette == BoilerplatePaletteSpaceHazeOverload) {
        app->palette_color_hex_a = 0xf8e3c4;
        app->palette_color_hex_b = 0xcc3495;
        app->palette_color_hex_c = 0x6b1fb1;
        app->palette_color_hex_d = 0x0b0630;
    } else if(app->palette == BoilerplatePaletteStarlitMemories) {
        app->palette_color_hex_a = 0x869ad9;
        app->palette_color_hex_b = 0x6d53bd;
        app->palette_color_hex_c = 0x6f2096;
        app->palette_color_hex_d = 0x4f133f;
    } else if(app->palette == BoilerplatePaletteMyFriendfromBavaria) {
        app->palette_color_hex_a = 0xfeda1b;
        app->palette_color_hex_b = 0xdf7925;
        app->palette_color_hex_c = 0xb60077;
        app->palette_color_hex_d = 0x382977;
    } else if(app->palette == BoilerplatePaletteThedeathofYungColumbus) {
        app->palette_color_hex_a = 0xb5ff32;
        app->palette_color_hex_b = 0xff2261;
        app->palette_color_hex_c = 0x462917;
        app->palette_color_hex_d = 0x1d1414;
    } else if(app->palette == BoilerplatePaletteTramontoalParcodegliAcquedotti) {
        app->palette_color_hex_a = 0xf3c677;
        app->palette_color_hex_b = 0xe64a4e;
        app->palette_color_hex_c = 0x912978;
        app->palette_color_hex_d = 0x0c0a3e;
    } else if(app->palette == BoilerplatePaletteThestarryknight) {
        app->palette_color_hex_a = 0xf5db37;
        app->palette_color_hex_b = 0x37cae5;
        app->palette_color_hex_c = 0x0f86b6;
        app->palette_color_hex_d = 0x123f77;
    } else if(app->palette == BoilerplatePaletteVirtualBoy1985) {
        app->palette_color_hex_a = 0xff0000;
        app->palette_color_hex_b = 0xdb0000;
        app->palette_color_hex_c = 0x520000;
        app->palette_color_hex_d = 0x000000;
    } else if(app->palette == BoilerplatePaletteWaterfrontPlaza) {
        app->palette_color_hex_a = 0xcecece;
        app->palette_color_hex_b = 0x6f9edf;
        app->palette_color_hex_c = 0x42678e;
        app->palette_color_hex_d = 0x102533;
    } else if(app->palette == BoilerplatePaletteYouthIkarusreloaded) {
        app->palette_color_hex_a = 0xcef7f7;
        app->palette_color_hex_b = 0xf78e50;
        app->palette_color_hex_c = 0x9e0000;
        app->palette_color_hex_d = 0x1e0000;
    }
}

static void boilerplate_scene_settings_set_info(VariableItem* item) {
    Boilerplate* app = variable_item_get_context(item);
    UNUSED(app);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, info_text[index]);
    app->info = info_value[index];
}

// static void boilerplate_scene_settings_set_led(VariableItem* item) {
//     Boilerplate* app = variable_item_get_context(item);
//     uint8_t index = variable_item_get_current_value_index(item);
//     variable_item_set_current_value_text(item, led_text[index]);
//     app->led = led_value[index];
// }

// static void boilerplate_scene_settings_set_save_settings(VariableItem* item) {
//     Boilerplate* app = variable_item_get_context(item);
//     uint8_t index = variable_item_get_current_value_index(item);
//     variable_item_set_current_value_text(item, settings_text[index]);
//     app->save_settings = settings_value[index];
// }

void boilerplate_scene_settings_submenu_callback(void* context, uint32_t index) {
    Boilerplate* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void boilerplate_scene_settings_on_enter(void* context) {
    Boilerplate* app = context;
    VariableItem* item;
    uint8_t value_index;

    // Palette
    item = variable_item_list_add(
        app->variable_item_list, "Palette:", 57, boilerplate_scene_settings_set_palette, app);
    value_index = value_index_uint32(app->palette, palette_value, 1);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, palette_text[value_index]);

    // Info
    item = variable_item_list_add(
        app->variable_item_list, "Info:", 2, boilerplate_scene_settings_set_info, app);
    value_index = value_index_uint32(app->info, info_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, info_text[value_index]);

    // // LED Effects on/off
    // item = variable_item_list_add(
    //     app->variable_item_list,
    //     "Save Prefix",
    //     2,
    //     boilerplate_scene_settings_set_led,
    //     app);
    // value_index = value_index_uint32(app->led, led_value, 2);
    // variable_item_set_current_value_index(item, value_index);
    // variable_item_set_current_value_text(item, led_text[value_index]);

    // // Save Settings to File
    // item = variable_item_list_add(
    //     app->variable_item_list,
    //     "Save Settings",
    //     2,
    //     boilerplate_scene_settings_set_save_settings,
    //     app);
    // value_index = value_index_uint32(app->save_settings, settings_value, 2);
    // variable_item_set_current_value_index(item, value_index);
    // variable_item_set_current_value_text(item, settings_text[value_index]);

    view_dispatcher_switch_to_view(app->view_dispatcher, BoilerplateViewIdSettings);
}

bool boilerplate_scene_settings_on_event(void* context, SceneManagerEvent event) {
    Boilerplate* app = context;
    UNUSED(app);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
    }
    return consumed;
}

void boilerplate_scene_settings_on_exit(void* context) {
    Boilerplate* app = context;
    variable_item_list_set_selected_item(app->variable_item_list, 0);
    variable_item_list_reset(app->variable_item_list);
}