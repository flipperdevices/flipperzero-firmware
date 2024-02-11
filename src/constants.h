#ifndef __constants_h__
#define __constants_h__

#include <storage/storage.h> // For APP_DATA_PATH

/* Delay between two background activities performed
 * by secondary_thread */
#define BACKGROUND_ACTIVITY_TICKS 1000U

/* How many ticks a popup animation should stay
 * on the screen */
#define ANIMATION_TICKS_DURATION 1U

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

static const char LIFE_STAGE_STRING[][6] = {"Egg",
                                            "Baby",
                                            "Child",
                                            "Teen",
                                            "Adult",
                                            "Dead"};

/* Game state file info */
#define GAME_STATE_STORAGE_STATE_FILENAME "matagotchi.save"
#define GAME_STATE_STORAGE_SETTINGS_FILENAME "matagotchi.settings"
#define GAME_STATE_STORAGE_STATE_PATH APP_DATA_PATH(GAME_STATE_STORAGE_STATE_FILENAME)
#define GAME_STATE_STORAGE_SETTINGS_PATH APP_DATA_PATH(GAME_STATE_STORAGE_SETTINGS_FILENAME)
#define GAME_STATE_HEADER_MAGIC 0xFE
#define GAME_STATE_HEADER_VERSION 0x01

/* Game parameters */
#ifdef DEBUG
/*
 * DEBUG PARAMETERS (fast evolution)
 * To compile with fbt use the following command:
 * ./fbt --extra-define=DEBUG launch APPSRC=matagotchi
 */
static const uint32_t MAX_XP_PER_STAGE[] = {1,    // EGG
                                            10,   // BABY
                                            10,   // CHILD
                                            10,   // TEEN
                                            10,  // ADULT
                                            0};   // DEAD
/* Experience */
#define NEW_XP_FREQUENCY 2 // One new XP every 2 seconds
#define NEW_XP_PROBABILITY 50 // 50% of probability of getting a new XP
/* Hunger */
#define MAX_HU 10
#define LOSE_HU_FREQUENCY 5 // Lose one HU every 5 seconds
#define LOSE_HU_PROBABILITY 50 // 50% of probability of losing an HU
#define LOSE_HU_MIN 1 // Lose a random number between MIN and MAX when the probability is hit
#define LOSE_HU_MAX 2
#define MIN_CANDY_HU_RESTORE 5 // Restore a random number between MIN and MAX
#define MAX_CANDY_HU_RESTORE 6
/* Health */
#define MAX_HP 10
#define CHECK_HP_FREQUENCY 5 // Check every 5 seconds
#define LOSE_HP_PROBABILITY 50 // 50% of probability of losing an HP (getting sick)
#define LOSE_HP_MIN 1 // Lose a random number between MIN and MAX when the probability is hit
#define LOSE_HP_MAX 2
#define MIN_PILL_HP_RESTORE 5 // Restore a random number between MIN and MAX
#define MAX_PILL_HP_RESTORE 6
#else
/* NORMAL PARAMETERS */
static const uint32_t MAX_XP_PER_STAGE[] = {1,    // EGG
                                            100,   // BABY
                                            300,   // CHILD
                                            400,   // TEEN
                                            999,  // ADULT
                                            0};   // DEAD
/* Experience */
#define NEW_XP_FREQUENCY 120 // One new XP every 2 mins
#define NEW_XP_PROBABILITY 30 // 30% of probability of getting a new XP
/* Hunger */
#define MAX_HU 100
#define LOSE_HU_FREQUENCY 600 // Lose one HU every 10 mins
#define LOSE_HU_PROBABILITY 40 // 40% of probability of losing an HU
#define LOSE_HU_MIN 1 // Lose a random number between MIN and MAX when the probability is hit
#define LOSE_HU_MAX 4
#define MIN_CANDY_HU_RESTORE 10 // Restore a random number between MIN and MAX
#define MAX_CANDY_HU_RESTORE 50
/* Health */
#define MAX_HP 100
#define CHECK_HP_FREQUENCY 300 // Check every 5 mins
#define LOSE_HP_PROBABILITY 5 // 5% of probability of losing an HP (getting sick)
#define LOSE_HP_MIN 1 // Lose a random number between MIN and MAX when the probability is hit
#define LOSE_HP_MAX 5
#define MIN_PILL_HP_RESTORE 10 // Restore a random number between MIN and MAX
#define MAX_PILL_HP_RESTORE 50
#endif

#endif
