#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include "flipper_world.h"

#include "views/flipper_world_main.h"
#include "views/flipper_world_passport.h"
#include "views/flipper_world_foodgame.h"

#include "scenes/flipper_world_scene.h"

// global
#define SCALE 32
// screen

#define SCREEN_WIDTH GUI_DISPLAY_WIDTH
#define SCREEN_HEIGHT GUI_DISPLAY_HEIGHT
#define BONDARIES_X_LEFT 40
#define BONDARIES_X_RIGHT 88

// player
#define DOLPHIN_WIDTH 32
#define DOLPHIN_HEIGHT 32
#define DOLPHIN_CENTER (SCREEN_WIDTH / 2 - DOLPHIN_WIDTH)
#define SPEED_X 4
#define SPEED_Y 4
#define ACTIONS_NUM 4
#define DOLPHIN_DEFAULT_Y 2

// world
#define WORLD_WIDTH 256
#define WORLD_HEIGHT 192

#define LAYERS 8
#define DOLPHIN_LAYER 6
#define PARALLAX_MOD 7
#define PARALLAX(layer) layer / PARALLAX_MOD - layer

#define DIALOG_PROGRESS 250

typedef enum {
    FlipperWorldViewMain,
    FlipperWorldFoodGame,
    FlipperWorldPassport,
} FlipperWorldViewEnum;

struct FlipperWorld {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    FlipperMainView* main_view;
    FlipperFoodGameView* food_game;
    FlipperPassportView* passport;
};

FlipperWorld* flipper_world_alloc();

void flipper_world_free(FlipperWorld* flipper_world);
