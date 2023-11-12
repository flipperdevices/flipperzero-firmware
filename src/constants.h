#ifndef __constants_h__
#define __constants_h__

#include <storage/storage.h> // For APP_DATA_PATH

/* Delay between two background activities performed
 * by secondary_thread */
#define BACKGROUND_ACTIVITY_TICKS 1000U

/* Strings */
#define LOG_TAG "Matagotchi"
static const char ABOUT_TEXT[] = "Matagotchi\n"
                                 "A Tamagotchi like game\n"
                                 "by MrModd\n"
                                 "\n"
                                 "This application is released\n"
                                 "under the GPLv3 license\n"
                                 "For more info visit\n"
                                 "github.com/MrModd/Matagotchi";

static const char LIFE_STAGE_STRING[][5] = {"Egg",
                                            "Baby",
                                            "Child",
                                            "Teen",
                                            "Adult",
                                            "Dead"};

/* Game state file info */
#define GAME_STATE_STORAGE_FILENAME "matagotchi.save"
#define GAME_STATE_STORAGE_PATH APP_DATA_PATH(GAME_STATE_STORAGE_FILENAME)
#define GAME_STATE_HEADER_MAGIC 0xFE
#define GAME_STATE_HEADER_VERSION 0x01

/* Game parameters */
static const uint32_t MAX_XP_PER_STAGE[] = {1,    // EGG
                                            10,   // BABY
                                            30,   // CHILD
                                            40,   // TEEN
                                            100,  // ADULT
                                            0};   // DEAD
#define NEW_XP_FREQUENCY 120 // One new XP every 120 seconds
#define NEW_XP_PROBABILITY 70 // 70% of probability of getting a new XP

#endif
