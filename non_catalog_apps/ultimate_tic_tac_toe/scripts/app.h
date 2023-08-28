/*
Ultimate Tic-Tac-Toe. By Racso.
https://rac.so

Licensed under the GNU General Public License v3.0: https://www.gnu.org/licenses/gpl-3.0.en.html

Copyright (C) 2023 Óscar F. Gómez

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

#include <gui/gui.h>

typedef struct SceneManager SceneManager;
typedef struct AppGameplayState AppGameplayState;

typedef struct AppContext {
    Gui* gui;
    SceneManager* sceneManager;
    AppGameplayState* gameplay;

} AppContext;

typedef enum SceneType {
    SceneType_None,
    SceneType_Menu,
    SceneType_Game,
    SceneType_Credits,
} SceneType;