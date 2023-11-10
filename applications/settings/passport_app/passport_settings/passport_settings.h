#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PASSPORT_ON_OFF_COUNT 2

#define XP_MODE_COUNT 7

#define XP_MODE_BAR 0
#define XP_MODE_PERCENT 1
#define XP_MODE_INVERTED_PERCENT 2
#define XP_MODE_RETRO_3 3
#define XP_MODE_RETRO_5 4
#define XP_MODE_BAR_PERCENT 5
#define XP_MODE_NONE 6

#define MOOD_SET_COUNT 3

#define MOOD_SET_NONE 0
#define MOOD_SET_REGULAR 1
#define MOOD_SET_420 2

#define PASSPORT_BG_COUNT 13

#define BG_NONE 0
#define BG_ADPOLICE 1
#define BG_CIRCUIT 2
#define BG_DB 3
#define BG_DEDSEC 4
#define BG_STOCK 5
#define BG_FURI 6
#define BG_MARIO 7
#define BG_MEDIEVAL 8
#define BG_MOUNTAINS 9
#define BG_MULTI 10
#define BG_SCROLL 11
#define BG_SLUT 12

#define PROFILE_IMG_COUNT 37

#define PIMG_NONE 0
#define PIMG_ADPOLICE 1
#define PIMG_BRIAREOS 2
#define PIMG_COBRA 3
#define PIMG_DALI 4
#define PIMG_DEDSEC 5
#define PIMG_DEER 6
#define PIMG_DOLPHIN 7
#define PIMG_DOLPHINMOODY 8
#define PIMG_ED209 9
#define PIMG_FSOCIETY 10
#define PIMG_FSOCIETY2 11
#define PIMG_GOKUSET 12
#define PIMG_GOKUKID 13
#define PIMG_GOKUADULT 14
#define PIMG_GOKUSSJ 15
#define PIMG_GOKUSSJ3 16
#define PIMG_GUYFAWKES 17
#define PIMG_LAIN 18
#define PIMG_LEEROY 19
#define PIMG_MARIO 20
#define PIMG_MARVIN 21
#define PIMG_MORELEELLOO 22
#define PIMG_NEUROMANCER 23
#define PIMG_PIKASLEEPY 24
#define PIMG_RABBIT 25
#define PIMG_SHINKAI 26
#define PIMG_SKULL 27
#define PIMG_SLIME 28
#define PIMG_SONIC 29
#define PIMG_SPIDER 30
#define PIMG_TANKGIRL 31
#define PIMG_TOTORO 32
#define PIMG_WAIFU1 33
#define PIMG_WAIFU2 34
#define PIMG_WAIFU3 35
#define PIMG_WRENCH 36

typedef struct {
    uint8_t background;
    uint8_t image;
    bool name;
    uint8_t mood_set;
    bool level;
    bool xp_text;
    uint8_t xp_mode;
    bool multipage;
} PassportSettings;

bool passport_settings_load(PassportSettings* passport_settings);

bool passport_settings_save(PassportSettings* passport_settings);

#ifdef __cplusplus
}
#endif