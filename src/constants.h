#ifndef __constants_h__
#define __constants_h__

#include <storage/storage.h> // For APP_DATA_PATH

/* How many ticks to wait for user input before
 * passing to some background activity */
#define USER_INPUT_IDLE_TICKS 1000U

#define GAME_STATE_STORAGE_FILENAME "matagotchi.save"
#define GAME_STATE_STORAGE_PATH APP_DATA_PATH(GAME_STATE_STORAGE_FILENAME)
#define GAME_STATE_HEADER_MAGIC 0xFE
#define GAME_STATE_HEADER_VERSION 0x01

#endif
