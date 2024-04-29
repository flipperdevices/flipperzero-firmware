/*
 * Copyright 2024 Alexander Taran
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#include <furi.h>
#include <gui/gui.h>
#include <storage/storage.h>
#include <dolphin/dolphin.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#define MAX_TOWER_LENGTH 8
#define FINAL_TOWER_LENGTH 8

#define DIR_RIGHT true
#define DIR_LEFT false

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define LOG_TAG "hanoi_towers" // For logging

#define SAVE_VERSION 1 // Increase when changing save format
#define SAVE_DIR "/ext/apps/Games"
#define SAVE_FILENAME SAVE_DIR "/game_hanoi_towers.save"

enum AppStatus {
    ST_MAIN_MENU,
    ST_PLAYING,
    ST_ABOUT,
    ST_WINNING,
    ST_FINISHING,
};

enum MenuEntryId {
    MN_CONTINUE,
    MN_NEWGAME,
    MN_ABOUT,
    MN_EXIT,
};

typedef struct {
    uint8_t id;
    char* text;
} MenuEntry;

const MenuEntry MainMenu[] = {
    {MN_NEWGAME, "New Game"},
    {MN_ABOUT, "About"},
    {MN_EXIT, "Exit"},
    {0, NULL},
};

const MenuEntry ContinuationMenu[] = {
    {MN_CONTINUE, "Continue"},
    {MN_NEWGAME, "New Game"},
    {MN_ABOUT, "About"},
    {MN_EXIT, "Save & Exit"},
    {0, NULL},
};

const char* AboutStrings[] = {
    "Hanoi Towers game",
    "Move tower to another stick,",
    "putting only small disks on big.",
    "",
    "Alexander Taran, 2024",
    "Press any key",
    0};

typedef struct {
    char* text;
    bool bold;
} MessageBoxLine;

uint8_t menuSize(const MenuEntry* menu) {
    uint8_t res = 0;
    while(menu[res].text != 0) res++;
    return res;
}

const NotificationSequence sequence_winning = {
    &message_green_255,
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    &message_green_0,
    NULL,
};

// You finished the entire game!
const NotificationSequence sequence_finishing = {
    &message_red_255,
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    &message_red_0,

    &message_delay_50,

    &message_green_255,
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    &message_green_0,

    &message_delay_50,

    &message_blue_255,
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    &message_blue_0,
    NULL,
};

// Just small vibrate on exit.
const NotificationSequence sequence_exit = {
    &message_vibro_on,
    &message_delay_25,
    &message_vibro_off,
    NULL,
};

typedef struct {
    uint8_t towerSize; // Difficulty level
    uint8_t towers[3][MAX_TOWER_LENGTH];
    uint8_t selectedTower; // 0, 1, 2
    uint8_t selectedDisk; // 0 if nothing selected otherwise disk size
    uint16_t movesCount;
    uint16_t totalMoves;
} GameState;

typedef struct {
    FuriMutex* mutex;
    GameState gameState;
    uint8_t status;
    uint8_t currentMenuSelection;
} AppState;

uint16_t calculateMinimalPossibleMoves(uint8_t towerSize) {
    uint16_t res = 1;
    while(towerSize > 0) {
        towerSize--;
        res *= 2;
    }
    return res - 1;
}

GameState createNewGameState(uint8_t towerSize) {
    GameState gs;
    gs.towerSize = towerSize;
    for(uint8_t i = 0; i < MAX_TOWER_LENGTH; ++i) {
        gs.towers[0][i] = 0;
        gs.towers[1][i] = 0;
        gs.towers[2][i] = 0;
    }
    for(uint8_t i = 0; i < towerSize; ++i) {
        gs.towers[0][i] = towerSize - i;
    }
    gs.selectedTower = 0;
    gs.selectedDisk = 0;
    gs.movesCount = 0;
    gs.totalMoves = 0;
    return gs;
}

uint8_t calculateRelativeTower(uint8_t towerId, bool dir) {
    if(dir == DIR_LEFT) {
        if(towerId == 0) return 2;
        return towerId - 1;
    } else {
        if(towerId == 2) return 0;
        return towerId + 1;
    }
}

uint8_t add_delta_looped(uint8_t idx, uint8_t nitems, int8_t delta) {
    int8_t v = (int8_t)idx;
    v += delta;
    while(v < 0) v += nitems;
    v %= nitems;
    return (uint8_t)v;
}

uint8_t towerTopIndex(GameState* gs, uint8_t towerId) {
    if(gs->towers[towerId][0] == 0) {
        return 0;
    }
    uint8_t curr = 0;
    while(curr + 1 < MAX_TOWER_LENGTH && gs->towers[towerId][curr + 1] != 0) {
        curr++;
    }
    return curr;
}

// Works correctly if no disk selected
void trySelectCurrentDisk(GameState* gs) {
    uint8_t towerTop = towerTopIndex(gs, gs->selectedTower);
    uint8_t diskSize = gs->towers[gs->selectedTower][towerTop];
    if(diskSize == 0) return; // Nothing could be selected
    gs->selectedDisk = diskSize;
    gs->towers[gs->selectedTower][towerTop] = 0;
}

// Works correctly if some disk already selected
void tryPutCurrentDisk(GameState* gs) {
    uint8_t towerTop = towerTopIndex(gs, gs->selectedTower);
    uint8_t diskSize = gs->towers[gs->selectedTower][towerTop];
    if(diskSize == 0) {
        gs->towers[gs->selectedTower][0] = gs->selectedDisk;
        gs->selectedDisk = 0;
        gs->movesCount++;
    } else if(gs->selectedDisk < diskSize) {
        gs->towers[gs->selectedTower][towerTop + 1] = gs->selectedDisk;
        gs->selectedDisk = 0;
        gs->movesCount++;
    }
}

void handleDiskSelect(GameState* gs) {
    if(gs->selectedDisk == 0) {
        trySelectCurrentDisk(gs);
    } else {
        tryPutCurrentDisk(gs);
    }
}

void handleMoveAction(GameState* gs, bool dir) {
    gs->selectedTower = calculateRelativeTower(gs->selectedTower, dir);
}

bool checkIsWinning(GameState* gs) {
    if(gs->selectedDisk != 0) return false;
    if(gs->towers[0][0] != 0) return false;
    if(gs->towers[1][0] != 0 && gs->towers[2][0] != 0) return false;
    // Assuming GameState is valid - this check is enough.
    return true;
}

void draw_disk_at(Canvas* canvas, uint8_t diskX, uint8_t diskY, uint8_t diskSize) {
    uint8_t diskWidth = 4 + diskSize * 4;
    canvas_draw_box(canvas, diskX, diskY, diskWidth, 4);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_dot(canvas, diskX, diskY);
    canvas_draw_dot(canvas, diskX, diskY + 3);
    canvas_draw_dot(canvas, diskX + diskWidth - 1, diskY);
    canvas_draw_dot(canvas, diskX + diskWidth - 1, diskY + 3);
    canvas_set_color(canvas, ColorBlack);
}

static void draw_game(Canvas* canvas, GameState* gs) {
    UNUSED(gs);

    // Drawing towers
    uint8_t stickSize = gs->towerSize * 6;
    canvas_set_color(canvas, ColorBlack);
    for(uint8_t i = 0; i < 3; ++i) {
        uint8_t towerX = 22 + 42 * i;
        uint8_t platformW = 4 + gs->towerSize;
        canvas_draw_box(canvas, towerX - 2, SCREEN_HEIGHT - stickSize, 4, stickSize);
        canvas_draw_line(
            canvas,
            towerX - platformW,
            SCREEN_HEIGHT - 1,
            towerX + platformW - 1,
            SCREEN_HEIGHT - 1);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_dot(canvas, towerX - 2, SCREEN_HEIGHT - stickSize);
        canvas_draw_dot(canvas, towerX + 1, SCREEN_HEIGHT - stickSize);
        canvas_set_color(canvas, ColorBlack);

        for(uint8_t j = 0; j < MAX_TOWER_LENGTH; ++j) {
            uint8_t diskSize = gs->towers[i][j];
            if(diskSize == 0) break;
            uint8_t diskWidth = 4 + diskSize * 4;
            uint8_t diskX = towerX - diskWidth / 2;
            uint8_t diskY = SCREEN_HEIGHT - j * 6 - 6;
            draw_disk_at(canvas, diskX, diskY, diskSize);
        }
    }

    // Drawing selection ...
    uint8_t selectedTowerX = 22 + 42 * gs->selectedTower;
    if(gs->selectedDisk == 0) {
        canvas_draw_box(canvas, selectedTowerX - 1, 64 - stickSize - 8, 2, 5);
        canvas_draw_dot(canvas, selectedTowerX - 2, 64 - stickSize - 5);
        canvas_draw_dot(canvas, selectedTowerX + 1, 64 - stickSize - 5);
    } else { // or a currently selected disk.
        uint8_t diskWidth = 4 + gs->selectedDisk * 4;
        uint8_t diskX = selectedTowerX - diskWidth / 2;
        draw_disk_at(canvas, diskX, 64 - stickSize - 8, gs->selectedDisk);
    }

    // Drawing moves counter
    size_t s = snprintf(NULL, 0, "%u", gs->movesCount);
    char moves[s + 1];
    snprintf(moves, s + 1, "%u", gs->movesCount);

    canvas_draw_str_aligned(canvas, SCREEN_WIDTH, 0, AlignRight, AlignTop, moves);
}

void draw_menu(Canvas* canvas, const MenuEntry* menu, uint8_t selectedIndex) {
    const uint8_t DLT_FRAME = 2;
    uint8_t nitems = menuSize(menu);
    canvas_set_font(canvas, FontSecondary);
    int item_h = canvas_current_font_height(canvas);
    int max_width = 0;
    for(uint8_t i = 0; i < nitems; ++i) {
        int w = canvas_string_width(canvas, menu[i].text);
        if(w > max_width) {
            max_width = w;
        }
    }
    max_width += 2;
    for(uint8_t i = 0; i < nitems; ++i) {
        canvas_set_color(canvas, ColorBlack);
        if(i == selectedIndex) {
            canvas_draw_box(
                canvas,
                SCREEN_WIDTH / 2 - max_width / 2,
                SCREEN_HEIGHT / 2 - item_h * nitems / 2 + i * item_h,
                max_width,
                item_h);
        }
        canvas_set_color(canvas, i == selectedIndex ? ColorWhite : ColorBlack);
        canvas_draw_str_aligned(
            canvas,
            SCREEN_WIDTH / 2,
            SCREEN_HEIGHT / 2 - item_h * nitems / 2 + i * item_h + item_h / 2,
            AlignCenter,
            AlignCenter,
            menu[i].text);
    }
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(
        canvas,
        SCREEN_WIDTH / 2 - max_width / 2 - DLT_FRAME,
        SCREEN_HEIGHT / 2 - item_h * nitems / 2 - DLT_FRAME,
        max_width + DLT_FRAME * 2,
        item_h * nitems + DLT_FRAME * 2,
        2);
}

void draw_about(Canvas* canvas) {
    uint8_t nitems = 0;
    while(AboutStrings[nitems] != NULL) ++nitems;
    canvas_set_font(canvas, FontSecondary);
    int item_h = canvas_current_font_height(canvas);
    int max_width = 0;
    for(uint8_t i = 0; i < nitems; ++i) {
        int w = canvas_string_width(canvas, AboutStrings[i]);
        if(w > max_width) {
            max_width = w;
        }
    }
    canvas_set_color(canvas, ColorBlack);
    for(uint8_t i = 0; i < nitems; ++i) {
        canvas_set_font(canvas, i == 0 ? FontPrimary : FontSecondary);
        canvas_draw_str_aligned(
            canvas,
            SCREEN_WIDTH / 2 - max_width / 2,
            SCREEN_HEIGHT / 2 - item_h * nitems / 2 + i * item_h + item_h / 2,
            AlignLeft,
            AlignCenter,
            AboutStrings[i]);
    }
}

void draw_message_box(Canvas* canvas, const MessageBoxLine* lines) {
    uint8_t nlines = 0;
    while(lines[nlines].text != NULL) nlines++;

    uint8_t totalH = 0;
    uint8_t maxW = 0;
    for(uint8_t i = 0; i < nlines; ++i) {
        canvas_set_font(canvas, lines[i].bold ? FontPrimary : FontSecondary);
        maxW = MAX(maxW, canvas_string_width(canvas, lines[i].text));
        totalH += canvas_current_font_height(canvas);
    }
    const int paddingV = 2;
    const int paddingH = 4;

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(
        canvas,
        SCREEN_WIDTH / 2 - maxW / 2 - paddingH,
        SCREEN_HEIGHT / 2 - totalH / 2 - paddingV,
        maxW + paddingH * 2,
        totalH + paddingV * 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(
        canvas,
        SCREEN_WIDTH / 2 - maxW / 2 - paddingH,
        SCREEN_HEIGHT / 2 - totalH / 2 - paddingV,
        maxW + paddingH * 2,
        totalH + paddingV * 2,
        2);
    uint8_t currY = SCREEN_HEIGHT / 2 - totalH / 2 + 1;

    for(uint8_t i = 0; i < nlines; ++i) {
        canvas_set_font(canvas, lines[i].bold ? FontPrimary : FontSecondary);
        uint8_t lineH = canvas_current_font_height(canvas);
        canvas_draw_str_aligned(
            canvas, SCREEN_WIDTH / 2, currY, AlignCenter, AlignTop, lines[i].text);
        currY += lineH;
    }
}

void draw_winning(Canvas* canvas, GameState* gs) {
    uint16_t minMoves = calculateMinimalPossibleMoves(gs->towerSize);

    const char* line2pattern = "Total moves: %d";
    const char* line3pattern = "Min possible moves: %d";

    size_t line2len = snprintf(NULL, 0, line2pattern, gs->movesCount) + 1;
    char line2[line2len];
    snprintf(line2, line2len, line2pattern, gs->movesCount);

    size_t line3len = snprintf(NULL, 0, line3pattern, minMoves) + 1;
    char line3[line3len];
    snprintf(line3, line3len, line3pattern, minMoves);

    MessageBoxLine lines[] = {
        {"Level Passed!", true},
        {line2, false},
        {line3, false},
        {NULL, false},
    };

    draw_message_box(canvas, lines);
}

void draw_finishing(Canvas* canvas, GameState* gs) {
    // TODO
    const char* line1 = "You finished the game!";
    const char* line2pattern = "Total moves: %d";
    const char* line3src =
        "\x49\x69\x43\xc\x2\x1d\x1c\x9\x11\x11\x1\x44\x68\x29\xf\x1\x6\x49\x74\x3b\x18\x12\x17\x1";
    const char* line4src = "\x61\xf\xa\x44\x41\xd\x0\x4c\x69\x69\x47\x8\x1b\x54\x57\x16\x12";
    const char* line5src =
        "\x74\x1c\x1\x1a\x53\x4c\x3\x1a\x6\xa\x59\x46\xf\x7\xf\xd\x4c\x53\x10\x11\x17\x0\xb";

    char line3[24];
    line3[0] = line3src[0];
    char line4[17];
    line4[0] = line4src[0];
    char line5[23];
    line5[0] = line5src[0];
    for(uint8_t i = 1; i < 24; ++i) line3[i] = line3[i - 1] ^ line3src[i];
    for(uint8_t i = 1; i < 17; ++i) line4[i] = line4[i - 1] ^ line4src[i];
    for(uint8_t i = 1; i < 23; ++i) line5[i] = line5[i - 1] ^ line5src[i];

    size_t line2len = snprintf(NULL, 0, line2pattern, gs->totalMoves) + 1;
    char line2[line2len];
    snprintf(line2, line2len, line2pattern, gs->totalMoves);

    uint8_t currY = 0;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, SCREEN_WIDTH / 2, currY, AlignCenter, AlignTop, line1);
    currY += canvas_current_font_height(canvas);
    canvas_set_font(canvas, FontSecondary);
    uint8_t lineH = canvas_current_font_height(canvas);
    canvas_draw_str_aligned(canvas, SCREEN_WIDTH / 2, currY, AlignCenter, AlignTop, line2);
    currY += lineH;
    currY += (SCREEN_HEIGHT - currY - lineH * 3) / 2;
    const uint8_t paddingW = 4;
    const uint8_t paddingH = 2;
    uint8_t w =
        MAX(canvas_string_width(canvas, line3),
            MAX(canvas_string_width(canvas, line4), canvas_string_width(canvas, line5)));
    canvas_draw_rframe(
        canvas,
        (SCREEN_WIDTH - w) / 2 - paddingW,
        currY - paddingH - 1,
        w + paddingW * 2,
        lineH * 3 + paddingH * 2,
        3);
    canvas_draw_str_aligned(canvas, SCREEN_WIDTH / 2, currY, AlignCenter, AlignTop, line3);
    currY += lineH;
    canvas_draw_str_aligned(canvas, SCREEN_WIDTH / 2, currY, AlignCenter, AlignTop, line4);
    currY += lineH;
    canvas_draw_str_aligned(canvas, SCREEN_WIDTH / 2, currY, AlignCenter, AlignTop, line5);
}

void save_game(GameState* gs) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    if(storage_common_stat(storage, SAVE_DIR, NULL) == FSE_NOT_EXIST) {
        if(!storage_simply_mkdir(storage, SAVE_DIR)) {
            return;
        }
    }

    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, SAVE_FILENAME, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        uint16_t version = SAVE_VERSION;
        storage_file_write(file, &version, sizeof(uint16_t));
        storage_file_write(file, gs, sizeof(GameState));
    }
    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);
}

void try_load_game(GameState* gs) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    File* file = storage_file_alloc(storage);

    if(storage_file_open(file, SAVE_FILENAME, FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint16_t version = 0;
        uint16_t readBytes = storage_file_read(file, &version, sizeof(uint16_t));
        if(readBytes == sizeof(uint16_t) && version == SAVE_VERSION) {
            readBytes = storage_file_read(file, gs, sizeof(GameState));
            if(readBytes != sizeof(GameState)) {
                gs->towerSize = 0; // Marker of empty game state.
            }
        }
    }

    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);
}

void delete_save() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    storage_simply_remove(storage, SAVE_FILENAME);

    furi_record_close(RECORD_STORAGE);
}

static void game_draw_callback(Canvas* canvas, void* ctx) {
    AppState* appState = (AppState*)ctx;
    furi_mutex_acquire(appState->mutex, FuriWaitForever);
    GameState* gs = &appState->gameState;

    canvas_clear(canvas);
    if(appState->status == ST_PLAYING) {
        draw_game(canvas, gs);
    } else if(appState->status == ST_WINNING) {
        draw_game(canvas, gs);
        draw_winning(canvas, gs);
    } else if(appState->status == ST_FINISHING) {
        draw_finishing(canvas, gs);
    } else if(appState->status == ST_MAIN_MENU) {
        bool canContinue = appState->gameState.towerSize != 0;
        const MenuEntry* menu = canContinue ? ContinuationMenu : MainMenu;
        draw_menu(canvas, menu, appState->currentMenuSelection);
    } else if(appState->status == ST_ABOUT) {
        draw_about(canvas);
    }

    furi_mutex_release(appState->mutex);
}

static void game_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;

    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t flipper_game_hanoi_towers(void* p) {
    UNUSED(p);

    InputEvent event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    AppState* appState = malloc(sizeof(AppState));
    appState->gameState.towerSize = 0;
    appState->status = ST_MAIN_MENU;
    appState->currentMenuSelection = 0;

    try_load_game(&appState->gameState);

    // Allocate our appState->mutex
    appState->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, game_draw_callback, appState);
    view_port_input_callback_set(view_port, game_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    dolphin_deed(DolphinDeedPluginGameStart);

    bool isFinishing = false;
    while(!isFinishing) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if(event.type != InputTypeShort) continue;

        furi_mutex_acquire(appState->mutex, FuriWaitForever);

        if(appState->status == ST_MAIN_MENU) {
            bool canContinue = appState->gameState.towerSize != 0;
            const MenuEntry* menu = canContinue ? ContinuationMenu : MainMenu;
            int sz = menuSize(menu);
            if(event.key == InputKeyBack) {
                if(canContinue) {
                    appState->status = ST_PLAYING;
                } else {
                    isFinishing = true;
                }
            } else if(event.key == InputKeyUp) {
                appState->currentMenuSelection =
                    add_delta_looped(appState->currentMenuSelection, sz, -1);
            } else if(event.key == InputKeyDown) {
                appState->currentMenuSelection =
                    add_delta_looped(appState->currentMenuSelection, sz, 1);
            } else if(event.key == InputKeyOk) {
                uint8_t menuId = menu[appState->currentMenuSelection].id;
                if(menuId == MN_CONTINUE) {
                    appState->status = ST_PLAYING;
                } else if(menuId == MN_NEWGAME) {
                    appState->gameState = createNewGameState(2);
                    appState->status = ST_PLAYING;
                } else if(menuId == MN_ABOUT) {
                    appState->status = ST_ABOUT;
                } else if(menuId == MN_EXIT) {
                    if(appState->gameState.towerSize != 0) {
                        save_game(&appState->gameState);
                    }
                    isFinishing = true;
                }
            }
        } else if(appState->status == ST_PLAYING) {
            if(event.key == InputKeyBack) {
                appState->status = ST_MAIN_MENU;
                appState->currentMenuSelection = 0;
            } else if(event.key == InputKeyLeft) {
                handleMoveAction(&appState->gameState, DIR_LEFT);
            } else if(event.key == InputKeyRight) {
                handleMoveAction(&appState->gameState, DIR_RIGHT);
            } else if(event.key == InputKeyOk) {
                handleDiskSelect(&appState->gameState);
                if(checkIsWinning(&appState->gameState)) {
                    notification_message_block(notification, &sequence_winning);
                    appState->status = ST_WINNING;
                }
            }
        } else if(appState->status == ST_WINNING) {
            // Any key will go to next level or finishing state.
            appState->gameState.totalMoves += appState->gameState.movesCount;
            if(appState->gameState.towerSize < FINAL_TOWER_LENGTH) {
                uint16_t savedTotalMoves = appState->gameState.totalMoves;
                appState->gameState = createNewGameState(appState->gameState.towerSize + 1);
                appState->gameState.totalMoves = savedTotalMoves;
                appState->status = ST_PLAYING;
            } else {
                appState->gameState.towerSize = 0;
                appState->status = ST_FINISHING;
                delete_save();
                notification_message_block(notification, &sequence_finishing);
            }
        } else if(appState->status == ST_FINISHING) {
            // Any key goes to main menu.
            appState->status = ST_MAIN_MENU;
            appState->currentMenuSelection = 0;
        } else if(appState->status == ST_ABOUT) {
            // Any key goes back.
            appState->status = ST_MAIN_MENU;
            appState->currentMenuSelection = 0;
        }

        view_port_update(view_port);
        furi_mutex_release(appState->mutex);
    }
    notification_message_block(notification, &sequence_exit);

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);

    // Message queue and mutex must be freed after removing and closing view port
    // because they are used in its callbacks.
    furi_message_queue_free(event_queue);
    furi_mutex_free(appState->mutex);
    free(appState);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    return 0;
}
