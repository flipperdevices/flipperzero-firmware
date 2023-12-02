/* @file secret_toggle.c
 * @author @nostrumuva, Isomer Programming LLC
 * @license GNU General Public License Version 3
 * @note yup it's open source for the flipper zero
 */

#include <dialogs/dialogs.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <stdbool.h>
#include <stdlib.h>
#include "util.h"

#define BOARD_HEIGHT 7
#define BOARD_WIDTH 7
#define BOARD_NUM_SQUARES (BOARD_HEIGHT * BOARD_WIDTH)
#define MAX_LEVELS 10

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef enum { SquareActiveOff, SquareActiveOn } SquareActive;

typedef struct {
    SquareActive mySquareActive;
    int myConnectedSquares[BOARD_NUM_SQUARES]; //0 if not connected, 1 if connected
} Square;

typedef struct {
    Square myBoard[BOARD_NUM_SQUARES];
    int myLevel;
    int myCursorX;
    int myCursorY;
    int myNumToggles;
    bool myWin;
} Game;

static void showStartupScreen() {
    const char* tempTitle = "Secret Toggle";
    const char* tempMessage =
        "Toggle all squares to be bright.\nSome squares are connected.\ngithub.com/nostrumuva";
    const char* tempConfirmButtonText = "Play";
    utilShowDialog(tempTitle, tempMessage, tempConfirmButtonText);
}

static void showLevelScreen(Game* paramGame) {
    const char* tempTitle = "Secret Toggle";
    FuriString* tempMessage;
    tempMessage = furi_string_alloc();
    furi_string_printf(tempMessage, "LEVEL %d!", paramGame->myLevel);
    const char* tempConfirmButtonText = "Play";
    utilShowDialog(tempTitle, furi_string_get_cstr(tempMessage), tempConfirmButtonText);
    furi_string_free(tempMessage);
}

static void showExitScreen(Game* paramGame) {
    const char* tempExit = "Never give up!";
    const char* tempMessage = "Thanks for playing!\ngithub.com/nostrumuva";
    const char* tempConfirmButtonText = "Bye";
    FuriString* tempWin;
    tempWin = furi_string_alloc();
    furi_string_printf(tempWin, "You Win! T: %d", paramGame->myNumToggles);
    if(paramGame->myWin == true) {
        utilShowDialog(furi_string_get_cstr(tempWin), tempMessage, tempConfirmButtonText);
    } else {
        utilShowDialog(tempExit, tempMessage, tempConfirmButtonText);
    }
    furi_string_free(tempWin);
}

static bool gameCheckWinLevel(Game* paramGame) {
    bool tempWinLevel = true;
    int tempIterateSquares = 0;
    for(tempIterateSquares = 0; tempIterateSquares < BOARD_NUM_SQUARES; tempIterateSquares++) {
        if(paramGame->myBoard[tempIterateSquares].mySquareActive == SquareActiveOff) {
            tempWinLevel = false;
            break;
        }
    }
    return tempWinLevel;
}

static void gameToggleSquare(Game* paramGame, int paramSquareIndex) {
    int tempIterateConnected = 0;
    for(tempIterateConnected = 0; tempIterateConnected < BOARD_NUM_SQUARES;
        tempIterateConnected++) {
        if(paramGame->myBoard[paramSquareIndex].myConnectedSquares[tempIterateConnected] == 1 &&
           tempIterateConnected != paramSquareIndex) {
            paramGame->myBoard[tempIterateConnected].mySquareActive =
                (SquareActive)(!((bool)paramGame->myBoard[tempIterateConnected].mySquareActive));
        }
    }
    paramGame->myBoard[paramSquareIndex].mySquareActive =
        (SquareActive)(!((bool)paramGame->myBoard[paramSquareIndex].mySquareActive));
}

static void gameSetup(Game* paramGame, int paramLevel) {
    int tempIterateLevel = 0;
    int tempIterateSquares = 0;
    int tempIterateConnected = 0;
    int tempRandomSquare = 0;
    paramGame->myLevel = paramLevel;
    paramGame->myCursorX = 0;
    paramGame->myCursorY = 0;
    //turn them all on and set to no connections
    for(tempIterateSquares = 0; tempIterateSquares < BOARD_NUM_SQUARES; tempIterateSquares++) {
        paramGame->myBoard[tempIterateSquares].mySquareActive = SquareActiveOn;
        for(tempIterateConnected = 0; tempIterateConnected < BOARD_NUM_SQUARES;
            tempIterateConnected++) {
            paramGame->myBoard[tempIterateSquares].myConnectedSquares[tempIterateConnected] = 0;
        }
    }
    //based on level, connect more squares
    for(tempIterateSquares = 0; tempIterateSquares < BOARD_NUM_SQUARES; tempIterateSquares++) {
        for(tempIterateLevel = 0; tempIterateLevel < paramGame->myLevel; tempIterateLevel++) {
            tempRandomSquare = furi_hal_random_get() % BOARD_NUM_SQUARES;
            paramGame->myBoard[tempIterateSquares].myConnectedSquares[tempRandomSquare] = 1;
        }
    }
    //based on level, toggle more squares to scramble
    for(tempIterateLevel = 0; tempIterateLevel < paramGame->myLevel; tempIterateLevel++) {
        tempRandomSquare = rand() % BOARD_NUM_SQUARES;
        gameToggleSquare(paramGame, tempRandomSquare);
    }
}

static void render_callback(Canvas* const canvas, void* ctx) {
    Game* tempGame = (Game*)ctx;
    FuriString* tempStatusStr;
    tempStatusStr = furi_string_alloc();
    furi_string_printf(
        tempStatusStr, "L: %d/%d   T: %d", tempGame->myLevel, MAX_LEVELS, tempGame->myNumToggles);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas, 0, 0, AlignLeft, AlignTop, furi_string_get_cstr(tempStatusStr));
    static uint8_t staticMinX = 40;
    static uint8_t staticMinY = 8;
    static uint8_t staticSquareActiveOnBits[] = {
        0x00,
        0x7E,
        0x42,
        0x42,
        0x42,
        0x42,
        0x7E,
        0x00,
    }; //1 is black, 0 is bright
    static uint8_t staticSquareActiveOnBitsSelected[] = {
        0xFF,
        0xFF,
        0xC3,
        0xC3,
        0xC3,
        0xC3,
        0xFF,
        0xFF,
    }; //1 is black, 0 is bright
    static uint8_t staticSquareActiveOffBits[] = {
        0x00,
        0x7E,
        0x7E,
        0x7E,
        0x7E,
        0x7E,
        0x7E,
        0x00,
    }; //1 is black, 0 is bright
    static uint8_t staticSquareActiveOffBitsSelected[] = {
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
    }; //1 is black, 0 is bright
    int tempIterateSquares = 0;
    int tempSquareX = 0;
    int tempSquareY = 0;
    for(tempIterateSquares = 0; tempIterateSquares < BOARD_NUM_SQUARES; tempIterateSquares++) {
        tempSquareX = tempIterateSquares % BOARD_WIDTH;
        tempSquareY = (tempIterateSquares - tempSquareX) / BOARD_HEIGHT;
        if(tempGame->myBoard[tempIterateSquares].mySquareActive == SquareActiveOn) {
            if(tempSquareX == tempGame->myCursorX && tempSquareY == tempGame->myCursorY) {
                canvas_draw_xbm(
                    canvas,
                    staticMinX + (tempSquareX * 8), //x position
                    staticMinY + (tempSquareY * 8), //y position
                    8, //width
                    8, //height
                    staticSquareActiveOnBitsSelected);
            } else {
                canvas_draw_xbm(
                    canvas,
                    staticMinX + (tempSquareX * 8), //x position
                    staticMinY + (tempSquareY * 8), //y position
                    8, //width
                    8, //height
                    staticSquareActiveOnBits);
            }
        } else {
            if(tempSquareX == tempGame->myCursorX && tempSquareY == tempGame->myCursorY) {
                canvas_draw_xbm(
                    canvas,
                    staticMinX + (tempSquareX * 8), //x position
                    staticMinY + (tempSquareY * 8), //y position
                    8, //width
                    8, //height
                    staticSquareActiveOffBitsSelected);
            } else {
                canvas_draw_xbm(
                    canvas,
                    staticMinX + (tempSquareX * 8), //x position
                    staticMinY + (tempSquareY * 8), //y position
                    8, //width
                    8, //height
                    staticSquareActiveOffBits);
            }
        }
    }
    furi_string_free(tempStatusStr);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void clampCursor(Game* paramGame) {
    if(paramGame->myCursorX < 0) {
        paramGame->myCursorX = 0;
    } else if(paramGame->myCursorX > BOARD_WIDTH - 1) {
        paramGame->myCursorX = BOARD_WIDTH - 1;
    }
    if(paramGame->myCursorY < 0) {
        paramGame->myCursorY = 0;
    } else if(paramGame->myCursorY > BOARD_HEIGHT - 1) {
        paramGame->myCursorY = BOARD_HEIGHT - 1;
    }
}

static int getCursorSquareIndex(Game* paramGame) {
    return paramGame->myCursorY * BOARD_HEIGHT + paramGame->myCursorX;
}

int32_t secret_toggle_app(void* p) {
    UNUSED(p);
    showStartupScreen();
    Game* newGame = malloc(sizeof(Game));
    newGame->myWin = false;
    newGame->myNumToggles = 0;
    int tempLevel = 1;
    bool tempContinueGame = true;
    while(tempContinueGame == true) {
        gameSetup(newGame, tempLevel);
        showLevelScreen(newGame);
        FuriMessageQueue* newFuriMessageQueue = furi_message_queue_alloc(8, sizeof(PluginEvent));
        Gui* newGui = furi_record_open("gui");
        ViewPort* newViewPort = view_port_alloc();
        view_port_draw_callback_set(newViewPort, render_callback, newGame);
        view_port_input_callback_set(newViewPort, input_callback, newFuriMessageQueue);
        gui_add_view_port(newGui, newViewPort, GuiLayerFullscreen);
        PluginEvent tempPluginEvent;
        for(bool tempProcessing = true; tempProcessing;) {
            FuriStatus tempFuriStatus =
                furi_message_queue_get(newFuriMessageQueue, &tempPluginEvent, 100);
            if(tempFuriStatus == FuriStatusOk) //otherwise, event timed out
            {
                if(tempPluginEvent.type == EventTypeKey) {
                    if(tempPluginEvent.input.type == InputTypeShort) //short press
                    {
                        switch(tempPluginEvent.input.key) {
                        case InputKeyUp:
                            newGame->myCursorY -= 1;
                            clampCursor(newGame);
                            break;
                        case InputKeyDown:
                            newGame->myCursorY += 1;
                            clampCursor(newGame);
                            break;
                        case InputKeyRight:
                            newGame->myCursorX += 1;
                            clampCursor(newGame);
                            break;
                        case InputKeyLeft:
                            newGame->myCursorX -= 1;
                            clampCursor(newGame);
                            break;
                        case InputKeyOk:
                            gameToggleSquare(newGame, getCursorSquareIndex(newGame));
                            newGame->myNumToggles += 1;
                            break;
                        case InputKeyBack:
                            tempProcessing = false;
                            tempContinueGame = false;
                            break;
                        case InputKeyMAX:
                            break;
                        default:
                            break;
                        }
                    } else if(tempPluginEvent.input.type == InputTypeLong) //long press
                    {
                        switch(tempPluginEvent.input.key) {
                        case InputKeyUp:
                        case InputKeyDown:
                        case InputKeyRight:
                        case InputKeyLeft:
                            break;
                        case InputKeyOk:
                            //test win condition
                            ///newGame->myWin=true;
                            ///tempProcessing=false;
                            ///tempContinueGame=false;
                            break;
                        case InputKeyBack:
                            tempProcessing = false;
                            tempContinueGame = false;
                            break;
                        case InputKeyMAX:
                            break;
                        default:
                            break;
                        }
                    }
                }
                if(gameCheckWinLevel(newGame) == true) {
                    tempLevel += 1;
                    if(tempLevel > MAX_LEVELS) {
                        newGame->myWin = true;
                        tempContinueGame = false;
                    }
                    tempProcessing = false;
                }
            }
            view_port_update(newViewPort);
        }
        view_port_enabled_set(newViewPort, false);
        gui_remove_view_port(newGui, newViewPort);
        view_port_free(newViewPort);
        furi_record_close("gui");
        furi_message_queue_free(newFuriMessageQueue);
    }
    showExitScreen(newGame);
    free(newGame);
    return 0;
}
