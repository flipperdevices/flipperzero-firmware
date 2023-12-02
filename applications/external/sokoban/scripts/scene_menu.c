#include "app.h"
#include "game.h"
#include "app_gameplay.h"
#include "racso_sokoban_icons.h"
#include "wave/scene_management.h"
#include "wave/pagination.h"
#include "wave/calc.h"
#include "save_data_manager.h"
#include <furi.h>
#include <gui/gui.h>
#include <storage/storage.h>

const int NONE = -1;

typedef enum MenuState
{
    MenuState_Main,
    MenuState_CollectionSelection,
    MenuState_LevelSelection
} MenuState;

static MenuState menu_state(AppGameplayState* gameplayState)
{
    if (gameplayState->selectedCollection == NONE)
        return MenuState_Main;
    else if (gameplayState->selectedLevel == NONE)
        return MenuState_CollectionSelection;
    else
        return MenuState_LevelSelection;
}

void menu_transition_callback(int from, int to, void* context)
{
    AppContext* app = (AppContext*)context;
    AppGameplayState* gameplayState = app->gameplay;

    if (from == SCENE_MANAGER_NO_SCENE && to == SceneType_Menu)
    {
        gameplayState->selectedCollection = NONE;
        gameplayState->selectedLevel = NONE;
    }
}

void menu_render_callback(Canvas* const canvas, void* context)
{
    AppContext* app = (AppContext*)context;
    AppGameplayState* gameplayState = app->gameplay;
    LevelsDatabase* database = app->database;

    canvas_clear(canvas);

    MenuState menuState = menu_state(gameplayState);

    if (menuState == MenuState_Main)
    {
        canvas_draw_icon(canvas, 0, 0, &I_splash);

        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontPrimary);
        //canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignTop, "SOKOBAN");

        for (int x = 0, y = 58, dx = 0; dx <= 3; dx++)
            canvas_draw_line(canvas, x + dx, y - dx, x + dx, y + dx);
        canvas_draw_icon(canvas, 6, 55, &I_question_mark);

        const int START_CENTER_X = 48, START_CENTER_Y = 59;
        canvas_draw_circle(canvas, START_CENTER_X, START_CENTER_Y, 4);
        canvas_draw_disc(canvas, START_CENTER_X, START_CENTER_Y, 2);
        canvas_draw_str_aligned(canvas, START_CENTER_X + 8, START_CENTER_Y + 4, AlignLeft, AlignBottom, "Start");
    }
    else if (menuState == MenuState_CollectionSelection)
    {
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "Collection");

        canvas_set_font(canvas, FontKeyboard);
        ContinuousPageInfo pageInfo = pagination_continuous_centered(database->collectionsCount, 5, gameplayState->selectedCollection);
        for (int i = 0, item = pageInfo.start; i < 5 && item <= pageInfo.end; i++, item++)
        {
            int y = 12 + i * 10;
            char text[64];

            if (item == gameplayState->selectedCollection)
                snprintf(text, 64, "> %s", database->collections[item].name);
            else
                snprintf(text, 64, "  %s", database->collections[item].name);

            canvas_draw_str_aligned(canvas, 0, y, AlignLeft, AlignTop, text);
        }
    }
    else if (menuState == MenuState_LevelSelection)
    {
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "Level");

        canvas_set_font(canvas, FontKeyboard);
        ContinuousPageInfo pageInfo = pagination_continuous_centered(database->collections[gameplayState->selectedCollection].levelsCount, 5, gameplayState->selectedLevel);
        for (int i = 0, item = pageInfo.start; i < 5 && item <= pageInfo.end; i++, item++)
        {
            int y = 12 + i * 10;
            int x = 0;

            char text[64];
            if (item == gameplayState->selectedLevel)
                snprintf(text, 64, "> #%d", item + 1);
            else
                snprintf(text, 64, "  #%d", item + 1);
            canvas_draw_str_aligned(canvas, x, y, AlignLeft, AlignTop, text);

            x += 46;
            LevelItem levelItem = database->collections[gameplayState->selectedCollection].levels[item];
            const Icon* icon;
            if (levelItem.playerBest == 0)
                icon = &I_checkbox_empty;
            else if (levelItem.playerBest > levelItem.worldBest)
                icon = &I_checkbox_checked;
            else
                icon = &I_star;

            canvas_draw_icon(canvas, x, y - 1, icon);

            x += 16;

            if (levelItem.playerBest == 0)
                snprintf(text, 64, "  --/%d", levelItem.worldBest);
            else
                snprintf(text, 64, "  %d/%d", levelItem.playerBest, levelItem.worldBest);

            canvas_draw_str_aligned(canvas, x, y, AlignLeft, AlignTop, text);
        }
    }
}

void menu_input_callback(InputKey key, InputType type, void* context)
{
    AppContext* app = (AppContext*)context;
    AppGameplayState* gameplayState = app->gameplay;
    LevelsDatabase* database = app->database;

    MenuState state = menu_state(gameplayState);
    if (key == InputKeyBack && type == InputTypePress)
    {
        if (state == MenuState_Main)
            scene_manager_set_scene(app->sceneManager, SceneType_None);
        else if (state == MenuState_CollectionSelection)
            gameplayState->selectedCollection = NONE;
        else if (state == MenuState_LevelSelection)
            gameplayState->selectedLevel = NONE;

        return;
    }

    if (state == MenuState_Main && key == InputKeyLeft && type == InputTypePress)
    {
        scene_manager_set_scene(app->sceneManager, SceneType_Credits);
        return;
    }

    if (key == InputKeyOk && type == InputTypePress)
    {
        if (state == MenuState_Main)
            gameplayState->selectedCollection = 0;
        else if (state == MenuState_CollectionSelection)
            gameplayState->selectedLevel = 0;
        else if (state == MenuState_LevelSelection)
        {
            scene_manager_set_scene(app->sceneManager, SceneType_Game);
            return;
        }
    }

    int delta = 0;
    if (key == InputKeyUp && (type == InputTypePress || type == InputTypeRepeat))
        delta = -1;
    else if (key == InputKeyDown && (type == InputTypePress || type == InputTypeRepeat))
        delta = 1;
    else if (key == InputKeyLeft && (type == InputTypePress || type == InputTypeRepeat))
        delta = -5;
    else if (key == InputKeyRight && (type == InputTypePress || type == InputTypeRepeat))
        delta = 5;

    if (state == MenuState_CollectionSelection)
        gameplayState->selectedCollection = wrap_single(gameplayState->selectedCollection + delta, 0, database->collectionsCount - 1);
    else if (state == MenuState_LevelSelection)
        gameplayState->selectedLevel = wrap_single(gameplayState->selectedLevel + delta, 0, database->collections[gameplayState->selectedCollection].levelsCount - 1);
}
