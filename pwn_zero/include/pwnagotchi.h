#pragma once

#include <furi.h>
#include <stdbool.h>

#include "constants.h"

#define PWNAGOTCHI_HEIGHT FLIPPER_SCREEN_HEIGHT
#define PWNAGOTCHI_WIDTH FLIPPER_SCREEN_WIDTH
#define PWNAGOTCHI_FACE_I           43
#define PWNAGOTCHI_FACE_J           3
#define PWNAGOTCHI_NAME_I           17
#define PWNAGOTCHI_NAME_J           64
#define PWNAGOTCHI_CHANNEL_I        6
#define PWNAGOTCHI_CHANNEL_J        3
#define PWNAGOTCHI_APS_I            6
#define PWNAGOTCHI_APS_J            18
#define PWNAGOTCHI_UPTIME_I         6
#define PWNAGOTCHI_UPTIME_J         93
#define PWNAGOTCHI_LINE1_START_I    8
#define PWNAGOTCHI_LINE1_START_J    3
#define PWNAGOTCHI_LINE1_END_I      8
#define PWNAGOTCHI_LINE1_END_J      122
#define PWNAGOTCHI_LINE2_START_I    55
#define PWNAGOTCHI_LINE2_START_J    3
#define PWNAGOTCHI_LINE2_END_I      55
#define PWNAGOTCHI_LINE2_END_J      122
#define PWNAGOTCHI_FRIEND_FACE_I    52
#define PWNAGOTCHI_FRIEND_FACE_J    3
#define PWNAGOTCHI_FRIEND_STAT_I    52
#define PWNAGOTCHI_FRIEND_STAT_J    24
#define PWNAGOTCHI_MODE_I           60
#define PWNAGOTCHI_MODE_J           106


/**
 * Enum to represent possible faces to save them locally rather than transmit every time
 */
enum PwnagotchiFace {
    NoFace = 0,
    Look_r,
    Look_l,
    Look_r_happy,
    Look_l_happy,
    Sleep,
    Sleep2,
    Awake,
    Bored,
    Intense,
    Cool,
    Happy,
    Grateful,
    Excited,
    Motivated,
    Demotivated,
    Smart,
    Lonely,
    Sad,
    Angry,
    Friend,
    Broken,
    Debug,
    Upload,
    Upload1,
    Upload2
};

/** All of the faces as macros so we don't have to worry about size */
#define LOOK_R          "( ⚆_⚆)"
#define LOOK_L          "(☉_☉ )"
#define LOOK_R_HAPPY    "( ◕‿◕)"
#define LOOK_L_HAPPY    "(◕‿◕ )"
#define SLEEP           "(⇀‿‿↼)"
#define SLEEP2          "(≖‿‿≖)"
#define AWAKE           "(◕‿‿◕)"
#define BORED           "(-__-)"
#define INTENSE         "(°▃▃°)"
#define COOL            "(⌐■_■)"
#define HAPPY           "(•‿‿•)"
#define GRATEFUL        "(^‿‿^)"
#define EXCITED         "(ᵔ◡◡ᵔ)"
#define MOTIVATED       "(☼‿‿☼)"
#define DEMOTIVATED     "(≖__≖)"
#define SMART           "(✜‿‿✜)"
#define LONELY          "(ب__ب)"
#define SAD             "(╥☁╥ )"
#define ANGRY           "(-_-')"
#define FRIEND          "(♥‿‿♥)"
#define BROKEN          "(☓‿‿☓)"
#define DEBUG           "(#__#)"
#define UPLOAD          "(1__0)"
#define UPLOAD1         "(1__1)"
#define UPLOAD2         "(0__1)"

/**
 * Enum for current mode of the pwnagotchi
 */
enum PwnagotchiMode {
    Auto,
    Ai,
    Manual
};

typedef struct {
    /// Current face
    enum PwnagotchiFace face;
    char* faceStr;
    /// CH channel display at top left
    char channel[PWNAGOTCHI_MAX_CHANNEL_LEN];
    /// AP text shown at the top
    char apStat[PWNAGOTCHI_MAX_APS_LEN];
    /// Uptime as text
    char uptime[PWNAGOTCHI_MAX_UPTIME_LEN];
    /// Hostname of the unit
    char hostname[PWNAGOTCHI_MAX_HOSTNAME_LEN];
    /// Message that is displayed
    char message[PWNAGOTCHI_MAX_MESSAGE_LEN];
    /// LAST SSID and other handshake information for the bottom
    char handshakes[PWNAGOTCHI_MAX_SSID_LEN];
    /// Current mode the pwnagotchi is in
    enum PwnagotchiMode mode;
    /// Friend face
    enum PwnagotchiFace friendFace;
    /// Name and aps of friend
    char friendStat[PWNAGOTCHI_MAX_FRIEND_STAT_LEN];

} Pwnagotchi;

/**
 * @brief Allocates and constructs a pwnagotchi struct
 * 
 * @return Pwnagotchi* Constructed pwnagotchi pointer
 */
Pwnagotchi* pwnagotchi_alloc();

/**
 * @brief Destruct and free pwnagotchi
 * 
 * @param pwn Pwnagotchi to destruct
 */
void pwnagotchi_free(Pwnagotchi* pwn);

/**
 * Draw the default display with no additional information provided
 * 
 * @param pwn Pwnagotchi device to draw
 * @param canvas Canvas to draw on
 */
// void pwnagotchi_draw_blank(Pwnagotchi* pwn, Canvas* canvas);

// /**
//  * Draw the stored pwnagotchi's face on the device
//  * 
//  * @param pwn Pwnagotchi to draw
//  * @param canvas Canvas to draw on
//  */
// void pwnagotchi_draw_face(Pwnagotchi* pwn, Canvas* canvas);

/**
 * Clears the screen buffer of the pwnagotchi
 * 
 * @param pwn Pwn to clear
 */
void pwnagotchi_screen_clear(Pwnagotchi* pwn);

