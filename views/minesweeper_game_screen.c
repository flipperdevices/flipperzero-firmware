#include "minesweeper_game_screen.h"
#include "minesweeper_icons.h"
#include <gui/elements.h>
#include <gui/icon_animation.h>
#include <input/input.h>

#include <furi.h>
#include <furi_hal.h>

static const Icon* tile_icons[] = {
    &I_tile_empty_8x8,
    &I_tile_0_8x8,
    &I_tile_1_8x8,
    &I_tile_2_8x8,
    &I_tile_3_8x8,
    &I_tile_4_8x8,
    &I_tile_5_8x8,
    &I_tile_6_8x8,
    &I_tile_7_8x8,
    &I_tile_8_8x8,
    &I_tile_mine_8x8,
    &I_tile_flag_8x8,
    &I_tile_uncleared_8x8,
};

// They way this enum is set up allows us to index the Icon* array above for some mine types
typedef enum {
    MineSweeperGameScreenTileNone = 0,
    MineSweeperGameScreenTileZero,
    MineSweeperGameScreenTileOne,
    MineSweeperGameScreenTileTwo,
    MineSweeperGameScreenTileThree,
    MineSweeperGameScreenTileFour,
    MineSweeperGameScreenTileFive,
    MineSweeperGameScreenTileSix,
    MineSweeperGameScreenTileSeven,
    MineSweeperGameScreenTileEight,
    MineSweeperGameScreenTileMine,
    MineSweeperGameScreenTileTypeCount,
} MineSweeperGameScreenTileType;

typedef enum {
    MineSweeperGameScreenTileStateFlagged,
    MineSweeperGameScreenTileStateUncleared,
    MineSweeperGameScreenTileStateCleared,
} MineSweeperGameScreenTileState;

struct MineSweeperGameScreen {
    View* view;
    void* context;
    MineSweeperGameScreenInputCallback input_callback;
};

typedef struct {
    uint8_t x, y;
    const Icon* icon;
} IconElement;

typedef struct {
    IconElement icon_element;
    MineSweeperGameScreenTileState tile_state;
    MineSweeperGameScreenTileType tile_type;
} MineSweeperTile;

typedef struct {
    MineSweeperTile board[ MINESWEEPER_BOARD_TILE_COUNT ];
    uint8_t mines_left;
    uint8_t flags_left;
} MineSweeperGameScreenModel;

void mine_sweeper_game_screen_view_enter(void* context) {
    furi_assert(context);
    UNUSED(context);

    //MineSweeperGameScreen* mine_sweeper_game_screen = context;
    
    //with_view_model(
    //    mine_sweeper_game_screen->view,
    //    MineSweeperGameScreenModel * model,
    //    {
    //    },
    //    true);
}

void mine_sweeper_game_screen_view_exit(void* context) {
    furi_assert(context);
    UNUSED(context);

    //MineSweeperGameScreen* mine_sweeper_game_screen = context;

    //with_view_model(
    //    mine_sweeper_game_screen->view,
    //    MineSweeperGameScreenModel * model,
    //    {
    //    },
    //    true);
}

void mine_sweeper_game_screen_view_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(canvas);
    furi_assert(_model);
    MineSweeperGameScreenModel* model = _model;

    UNUSED(model);

    canvas_clear(canvas);

    canvas_set_color(canvas, ColorWhite);
    canvas_set_color(canvas, ColorBlack);

}

bool mine_sweeper_game_screen_view_input_callback(InputEvent* event, void* context) {
    MineSweeperGameScreen* mine_sweeper_game_screen = context;
    bool consumed = false;

    // The input callback is defined in the scene .c file for the game screen view 
    if (mine_sweeper_game_screen->input_callback != NULL) {
        mine_sweeper_game_screen->input_callback(event, mine_sweeper_game_screen->context); 
        consumed = true;
    }

    return consumed;
}

static void setup_board(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    // Randomly generate a tile type for each board tile
    uint8_t mines_generated = 0;

    MineSweeperGameScreenTileType tiles[MINESWEEPER_BOARD_TILE_COUNT];

    for (uint8_t i = 0; i < MINESWEEPER_BOARD_TILE_COUNT; i++) {
        MineSweeperGameScreenTileType tile_type = MineSweeperGameScreenTileNone;

        do {
            tile_type = (furi_hal_random_get() % (MineSweeperGameScreenTileTypeCount-1)) + 1;
        } while (mines_generated == MINESWEEPER_STARTING_MINES && tile_type == MineSweeperGameScreenTileMine);
        
        if (tile_type == MineSweeperGameScreenTileMine) {
            mines_generated++;
        }

        tiles[i] = tile_type;
    }

    // If not all mines are added randomly add them to board positions
    while (mines_generated < MINESWEEPER_STARTING_MINES) {
        uint8_t rand_pos = furi_hal_random_get() % MINESWEEPER_BOARD_TILE_COUNT;
        
        if (tiles[rand_pos] == MineSweeperGameScreenTileMine)
            continue;

        tiles[rand_pos] = MineSweeperGameScreenTileMine;
        mines_generated++;

    }

    // Save tiles to view model
    // Because of way tile enum and Icon* array is set up we can
    // index tile_icons with the enum type to get the correct Icon*
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            for (uint8_t i = 0; i < MINESWEEPER_BOARD_TILE_COUNT; i++) {
                model->board[i].tile_type = tiles[i];
                model->board[i].tile_state = MineSweeperGameScreenTileStateUncleared;
                model->board[i].icon_element.icon = tile_icons[ tiles[i] ];
                model->board[i].icon_element.x = (i/MINESWEEPER_BOARD_WIDTH) * icon_get_height(tile_icons[ tiles[i] ]);
                model->board[i].icon_element.y = (i%MINESWEEPER_BOARD_WIDTH) * icon_get_width(tile_icons[ tiles[i] ]);
            }
        },
        true);
    
}

MineSweeperGameScreen* mine_sweeper_game_screen_alloc() {
    MineSweeperGameScreen* mine_sweeper_game_screen = (MineSweeperGameScreen*)malloc(sizeof(MineSweeperGameScreen));
    
    mine_sweeper_game_screen->view = view_alloc();

    view_set_context(mine_sweeper_game_screen->view, mine_sweeper_game_screen);
    view_allocate_model(mine_sweeper_game_screen->view, ViewModelTypeLocking, sizeof(MineSweeperGameScreenModel));

    view_set_draw_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_draw_callback);
    view_set_input_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_input_callback);
    
    // Right now these enter/exit callbacks are being used to start/stop animations
    view_set_enter_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_enter);
    view_set_exit_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_exit);

    setup_board(mine_sweeper_game_screen);
    
    return mine_sweeper_game_screen;
}

void mine_sweeper_game_screen_free(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            // Free any dynamically allocated members in the model
            UNUSED(model);
        },
        false);

    // Free view and any dynamically allocated members in main struct
    view_free(instance->view);
    free(instance);
}

void mine_sweeper_game_screen_reset(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            // Reset game model struct members
            UNUSED(model);
        },
        false);

    // Reset main game struct members
}

View* mine_sweeper_game_screen_get_view(MineSweeperGameScreen* instance) {
    furi_assert(instance);
    return instance->view;
}

void mine_sweeper_game_screen_set_input_callback(
        MineSweeperGameScreen* instance,
        MineSweeperGameScreenInputCallback callback) {

    furi_assert(instance);
    instance->input_callback = callback;
}

void mine_sweeper_game_screen_set_context(MineSweeperGameScreen* instance, void* context) {
    furi_assert(instance);
    instance->context = context;
}

bool mine_sweeper_is_tile_mine(MineSweeperGameScreen* instance, uint8_t x, uint8_t y) {
    furi_assert(instance);
    bool is_mine = false;
    
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            uint8_t pos = x * MINESWEEPER_BOARD_WIDTH + y;
            if (model->board[pos].tile_type == MineSweeperGameScreenTileMine) {
                is_mine = true;
            }
        },
        false);

    return is_mine;
}
