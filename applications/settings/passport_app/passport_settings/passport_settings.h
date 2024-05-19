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

#define PROFILE_IMG_COUNT 55

#define PIMG_NONE 0
#define PIMG_ADPOLICE 1
#define PIMG_AKKAN 2
#define PIMG_AKKEI 3
#define PIMG_AKTET 4
#define PIMG_BRIAREOS 5
#define PIMG_DALI 6
#define PIMG_DEDSEC 7
#define PIMG_DEER 8
#define PIMG_DOLPHIN 9
#define PIMG_DOLPHINMOODY 10
#define PIMG_ED209 11
#define PIMG_FSOCIETY 12
#define PIMG_FSOCIETY2 13
#define PIMG_GITSAOI 14
#define PIMG_GITSARA 15
#define PIMG_GITSBAT 16
#define PIMG_GITSHID 17
#define PIMG_GITSISH 18
#define PIMG_GITSKUS 19
#define PIMG_GITSPRO 20
#define PIMG_GITSSAI 21
#define PIMG_GITSTOG 22
#define PIMG_GOKUSET 23
#define PIMG_GOKUKID 24
#define PIMG_GOKUADULT 25
#define PIMG_GOKUSSJ 26
#define PIMG_GOKUSSJ3 27
#define PIMG_GUYFAWKES 28
#define PIMG_LAIN 29
#define PIMG_LEEROY 30
#define PIMG_MARIO 31
#define PIMG_MARVIN 32
#define PIMG_MORELEELLOO 33
#define PIMG_NEUROMANCER 34
#define PIMG_PIKASLEEPY 35
#define PIMG_PIRATE 36 // Pirate Profile Pic (By cyberartemio)
#define PIMG_RABBIT 37
#define PIMG_SCARMLA 38
#define PIMG_SCCOBRA 39
#define PIMG_SCCRYBO 40
#define PIMG_SCDOMRO 41
#define PIMG_SCSANDRA 42
#define PIMG_SCTARBEIGE 43
#define PIMG_SHINKAI 44
#define PIMG_SKULL 45
#define PIMG_SLIME 46
#define PIMG_SONIC 47
#define PIMG_SPIDER 48
#define PIMG_TANKGIRL 49
#define PIMG_TOTORO 50
#define PIMG_WAIFU1 51
#define PIMG_WAIFU2 52
#define PIMG_WAIFU3 53
#define PIMG_WRENCH 54

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