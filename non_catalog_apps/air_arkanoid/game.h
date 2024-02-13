#pragma once
#include "engine/engine.h"
#include "engine/sensors/imu.h"
#include <notification/notification_messages.h>

typedef struct {
    Level* menu;
    Level* settings;
    Level* game;
    Level* message;
} Levels;

typedef struct {
    bool sound;
    bool show_fps;
} Settings;

typedef struct {
    Imu* imu;
    bool imu_present;

    Levels levels;
    Settings settings;

    NotificationApp* app;
    GameManager* game_manager;
} GameContext;

void game_switch_sound(GameContext* context);

void game_switch_show_fps(GameContext* context);

void game_sound_play(GameContext* context, const NotificationSequence* sequence);

extern const NotificationSequence sequence_sound_menu;