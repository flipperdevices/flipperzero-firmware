#ifndef __constants_h__
#define __constants_h__

#include <storage/storage.h> // For APP_DATA_PATH

/* How many ticks to wait for user input before
 * passing to some background activity */
#define USER_INPUT_IDLE_TICKS 1000U

/* Log strings */
#define LOG_TAG "Matagotchi"

/* Game state file info */
#define GAME_STATE_STORAGE_FILENAME "matagotchi.save"
#define GAME_STATE_STORAGE_PATH APP_DATA_PATH(GAME_STATE_STORAGE_FILENAME)
#define GAME_STATE_HEADER_MAGIC 0xFE
#define GAME_STATE_HEADER_VERSION 0x01

/* Game parameters */
static const uint32_t MAX_XP_PER_STAGE[] = {1,    // EGG
                                            10,   // BABY
                                            30,   //CHILD
                                            40,   //TEEN
                                            100}; //ADULT
#define NEW_XP_FREQUENCY 120 // One new XP every 120 seconds
#define NEW_XP_PROBABILITY 70 // 70% of probability of getting a new XP

#endif
