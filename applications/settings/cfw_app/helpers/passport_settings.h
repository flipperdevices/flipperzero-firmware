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

#define PASSPORT_BG_COUNT 15

#define BG_NONE 0
#define BG_ADPOLICE 1
#define BG_CIRCUIT 2
#define BG_DB 3
#define BG_DEDSEC 4
#define BG_STOCK 5
#define BG_FURI 6
#define BG_GITS 7
#define BG_MARIO 8
#define BG_MEDIEVAL 9
#define BG_MEMCHIP 10
#define BG_MOUNTAINS 11
#define BG_MULTI 12
#define BG_SCROLL 13
#define BG_SLUT 14

#define PROFILE_IMG_COUNT 52

#define PIMG_NONE 0
#define PIMG_ADPOLICE 1
#define PIMG_BRIAREOS 2
#define PIMG_DALI 3
#define PIMG_DEDSEC 4
#define PIMG_DEER 5
#define PIMG_DOLPHIN 6
#define PIMG_DOLPHINMOODY 7
#define PIMG_ED209 8
#define PIMG_FSOCIETY 9
#define PIMG_FSOCIETY2 10
#define PIMG_GITSAOI 11
#define PIMG_GITSARA 12
#define PIMG_GITSBAT 13
#define PIMG_GITSHID 14
#define PIMG_GITSISH 15
#define PIMG_GITSKUS 16
#define PIMG_GITSPRO 17
#define PIMG_GITSSAI 18
#define PIMG_GITSTOG 19
#define PIMG_GOKUSET 20
#define PIMG_GOKUKID 21
#define PIMG_GOKUADULT 22
#define PIMG_GOKUSSJ 23
#define PIMG_GOKUSSJ3 24
#define PIMG_GUYFAWKES 25
#define PIMG_LAIN 26
#define PIMG_LEEROY 27
#define PIMG_MARIO 28
#define PIMG_MARVIN 29
#define PIMG_MORELEELLOO 30
#define PIMG_NEUROMANCER 31
#define PIMG_PIKASLEEPY 32
#define PIMG_PIRATE 33 // Pirate Profile Pic (By cyberartemio)
#define PIMG_RABBIT 34
#define PIMG_SCARMLA 35
#define PIMG_SCCOBRA 36
#define PIMG_SCCRYBO 37
#define PIMG_SCDOMRO 38
#define PIMG_SCSANDRA 39
#define PIMG_SCTARBEIGE 40
#define PIMG_SHINKAI 41
#define PIMG_SKULL 42
#define PIMG_SLIME 43
#define PIMG_SONIC 44
#define PIMG_SPIDER 45
#define PIMG_TANKGIRL 46
#define PIMG_TOTORO 47
#define PIMG_WAIFU1 48
#define PIMG_WAIFU2 49
#define PIMG_WAIFU3 50
#define PIMG_WRENCH 51

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