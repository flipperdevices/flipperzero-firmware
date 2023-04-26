#ifndef APP_HEADERS
#define APP_HEADERS

#include "flipper.h"

typedef enum { Resistor4Band = 4, Resistor5Band = 5 } ResistorType;

typedef enum {
    BandBlack = 0,
    BandBrown = 1,
    BandRed = 2,
    BandOrange = 3,
    BandYellow = 4,
    BandGreen = 5,
    BandBlue = 6,
    BandPurple = 7,
    BandGray = 8,
    BandWhite = 9,
    BandGold = 10,
    BandSilver = 11
} ResistorBand;

typedef struct AppState {
    uint8_t edit_selection;
    ResistorType resistor_type;
    int resistor_bands[5];
} AppState;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    AppState* state;
} App;

App* app_alloc();
AppState* app_state_alloc();
void app_quit(App* app);
void app_free(App* app);
void app_init_resistor(App* app, ResistorType rtype);

#endif