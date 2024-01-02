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
    GameScreenInputCallback input_callback;
};

typedef struct {
    uint16_t x_abs, y_abs;
} CurrentPosition;

typedef struct {
    uint16_t x_abs, y_abs;
    const Icon* icon;
} IconElement;

typedef struct {
    IconElement icon_element;
    MineSweeperGameScreenTileState tile_state;
    MineSweeperGameScreenTileType tile_type;
} MineSweeperTile;

typedef struct {
    MineSweeperTile board[ MINESWEEPER_BOARD_MAX_TILES ];
    uint16_t mines_left;
    uint16_t flags_left;
    CurrentPosition curr_pos;
    uint8_t right_boundary, bottom_boundary, board_width, board_height;
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

    canvas_clear(canvas);

    canvas_set_color(canvas, ColorBlack);
    


    /** We can use the boundary uint8_t in the model to transform the relative x/y coordinates
     *  to the absolute x/y positions on the board grid as well as the position in the board buffer.
     *
     *  The relative coordinates start at zero and go to MINESWEEPER_SCREEN_TILE_HEIGHT-1 and
     *  MINESWEEPER_SCREEN_TILE_HEIGHT-1 for x and y repsectively.
     *
     *  Once we have the absolute x/y coord we can use that to access the correct position for the
     *  tile in the board buffer within the model.
     *
     *  We draw the tile located at the absolute position in the grid onto the screen at the position of the
     *  relative x and y.
     *  We also invert the color if it is the current position selected by the user
     */

    for (uint8_t x_rel = 0; x_rel < MINESWEEPER_SCREEN_TILE_HEIGHT; x_rel++) {
        uint16_t x_abs = (model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT) + x_rel;
        
        for (uint8_t y_rel = 0; y_rel < MINESWEEPER_SCREEN_TILE_WIDTH; y_rel++) {
            uint16_t y_abs = (model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH) + y_rel;

            uint16_t board_buffer_pos_abs = x_abs * MINESWEEPER_BOARD_TILE_WIDTH + y_abs;
            MineSweeperTile tile = model->board[board_buffer_pos_abs];

            if (model->curr_pos.x_abs * MINESWEEPER_BOARD_TILE_WIDTH + model->curr_pos.y_abs == board_buffer_pos_abs) {
                inverted_canvas_white_to_black(
                    canvas,
                    {
                        canvas_draw_icon(
                            canvas,
                            y_rel * icon_get_width(tile.icon_element.icon),
                            x_rel * icon_get_height(tile.icon_element.icon),
                            tile.icon_element.icon);
                    });

            } else {
                canvas_draw_icon(
                    canvas,
                    y_rel * icon_get_width(tile.icon_element.icon),
                    x_rel * icon_get_height(tile.icon_element.icon),
                    tile.icon_element.icon);
            }

        }
    }

}

bool mine_sweeper_game_screen_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);

    MineSweeperGameScreen* instance = context;
    bool consumed = false;
    
    // In this input callback we can change the view model according to the event
    // then pass it to the custom input callback defined in the scene manager
    if ((event->type == InputTypePress) && (event->key == InputKeyOk)) {

        // register that something good or bad happened
        // Do we still pass this to the input callback in the scene manager?

        consumed = true;

    } else if ((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch (event->key) {

            case InputKeyUp :
                with_view_model(
                    instance->view,
                    MineSweeperGameScreenModel * model,
                    {
                        model->curr_pos.x_abs = (model->curr_pos.x_abs-1 < 0) ? 0 : model->curr_pos.x_abs-1;

                        bool is_outside_boundary = model->curr_pos.x_abs < (model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT);
                        
                        if (is_outside_boundary /**&& model->bottom_boundary-1 >= MINESWEEPER_SCREEN_TILE_HEIGHT*/) {
                            model->bottom_boundary--;
                        }
                    },
                    true);
                consumed = true;
                break;

            case InputKeyDown :
                with_view_model(
                    instance->view,
                    MineSweeperGameScreenModel * model,
                    {
                        model->curr_pos.x_abs = (model->curr_pos.x_abs+1 >= MINESWEEPER_BOARD_TILE_HEIGHT) ?
                            MINESWEEPER_BOARD_TILE_HEIGHT-1 : model->curr_pos.x_abs+1;

                        bool is_outside_boundary = model->curr_pos.x_abs >= model->bottom_boundary;

                        if (is_outside_boundary /**&& model->bottom_boundary+1 <= MINESWEEPER_BOARD_TILE_HEIGHT*/) {
                            model->bottom_boundary++;
                        }
                    },
                    true);
                consumed = true;
                break;

            case InputKeyLeft :
                with_view_model(
                    instance->view,
                    MineSweeperGameScreenModel * model,
                    {
                        model->curr_pos.y_abs = (model->curr_pos.y_abs-1 < 0) ? 0 : model->curr_pos.y_abs-1;

                        bool is_outside_boundary = model->curr_pos.y_abs < (model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH);
                        
                        if (is_outside_boundary /**&& model->right_boundary-1 >= MINESWEEPER_SCREEN_TILE_WIDTH*/) {
                            model->right_boundary--;
                        }
                    },
                    true);
                consumed = true;
                break;

            case InputKeyRight :
                with_view_model(
                    instance->view,
                    MineSweeperGameScreenModel * model,
                    {
                        model->curr_pos.y_abs = (model->curr_pos.y_abs+1 >= MINESWEEPER_BOARD_TILE_WIDTH) ?
                            MINESWEEPER_BOARD_TILE_WIDTH-1 : model->curr_pos.y_abs+1;

                        bool is_outside_boundary = model->curr_pos.y_abs >= model->right_boundary;

                        if (is_outside_boundary /**&& model->right_boundary+1 <= MINESWEEPER_BOARD_TILE_WIDTH*/) {
                            model->right_boundary++;
                        }
                    },
                    true);
                consumed = true;
                break;

            case InputKeyOk :
                break;

            default:
                break;
        }
    }

    if (!consumed && instance->input_callback != NULL) {
        consumed = instance->input_callback(event, instance->context);
    }

    return consumed;
}

static void setup_board(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    // We can use a temporary buffer to set the tile types initially
    // and manipulate then save to actual model
    MineSweeperGameScreenTileType tiles[MINESWEEPER_BOARD_TILE_COUNT];
    memset(&tiles, 0, sizeof(tiles));

    for (uint16_t i = 0; i < MINESWEEPER_STARTING_MINES; i++) {
        uint16_t rand_pos;

        do {
            rand_pos = furi_hal_random_get() % MINESWEEPER_BOARD_TILE_COUNT;
        } while (tiles[rand_pos] == MineSweeperGameScreenTileMine);

        tiles[rand_pos] = MineSweeperGameScreenTileMine;
    }

    // All mines are set so we look at each tile for surrounding mines
    for (uint16_t i = 0; i < MINESWEEPER_BOARD_TILE_COUNT; i++) {
        MineSweeperGameScreenTileType tile_type = tiles[i];

        if (tile_type == MineSweeperGameScreenTileMine) {
            continue;
        }

        uint16_t x = i / MINESWEEPER_BOARD_TILE_WIDTH;
        uint16_t y = i % MINESWEEPER_BOARD_TILE_WIDTH;

        uint8_t offsets[8][2] = {
            {-1,1},
            {0,1},
            {1,1},
            {1,0},
            {1,-1},
            {0,-1},
            {-1,-1},
            {-1,0},
        };

        uint16_t mine_count = 0;

        for (uint8_t j = 0; j < 8; j++) {
            int16_t dx = x + (uint16_t)offsets[j][0];
            int16_t dy = y + (uint16_t)offsets[j][1];

            if (dx < 0 || dy < 0 || dx >= MINESWEEPER_BOARD_TILE_HEIGHT || dy >= MINESWEEPER_BOARD_TILE_WIDTH) {
                continue;
            }

            uint16_t pos = dx * MINESWEEPER_BOARD_TILE_WIDTH + dy;
            if (tiles[pos] == MineSweeperGameScreenTileMine) {
                mine_count++;
            }
        }

        tiles[i] = (MineSweeperGameScreenTileType) ((mine_count % (MineSweeperGameScreenTileTypeCount-1)) + 1);

    }


    // Save tiles to view model
    // Because of way tile enum and Icon* array is set up we can
    // index tile_icons with the enum type to get the correct Icon*
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            for (uint16_t i = 0; i < MINESWEEPER_BOARD_TILE_COUNT; i++) {
                model->board[i].tile_type = tiles[i];
                model->board[i].tile_state = MineSweeperGameScreenTileStateUncleared;
                model->board[i].icon_element.icon = tile_icons[ tiles[i] ];
                model->board[i].icon_element.x_abs = (i/MINESWEEPER_BOARD_TILE_WIDTH);
                model->board[i].icon_element.y_abs = (i%MINESWEEPER_BOARD_TILE_WIDTH);
            }

            model->mines_left = MINESWEEPER_STARTING_MINES;
            model->flags_left = MINESWEEPER_STARTING_MINES;
            model->curr_pos.x_abs = 0;
            model->curr_pos.y_abs = 0;
            model->right_boundary = MINESWEEPER_SCREEN_TILE_WIDTH;
            model->bottom_boundary = MINESWEEPER_SCREEN_TILE_HEIGHT;
         
        },
        true);
    
}

MineSweeperGameScreen* mine_sweeper_game_screen_alloc(uint8_t width, uint8_t height) {
    MineSweeperGameScreen* mine_sweeper_game_screen = (MineSweeperGameScreen*)malloc(sizeof(MineSweeperGameScreen));
    
    mine_sweeper_game_screen->view = view_alloc();

    view_set_context(mine_sweeper_game_screen->view, mine_sweeper_game_screen);
    view_allocate_model(mine_sweeper_game_screen->view, ViewModelTypeLocking, sizeof(MineSweeperGameScreenModel));

    view_set_draw_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_draw_callback);
    view_set_input_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_input_callback);
    
    // This are currently be unused
    view_set_enter_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_enter);
    view_set_exit_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_exit);

    mine_sweeper_game_screen->input_callback = NULL;
    
    // We need to initize board width and height before setup
    mine_sweeper_game_screen_set_board_dimensions(mine_sweeper_game_screen, width, height);

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

// This function should be called whenever you want to reset the game state
// This should NOT be called in the on_exit in the game scene
void mine_sweeper_game_screen_reset(MineSweeperGameScreen* instance) {
    furi_assert(instance);
    
    instance->input_callback = NULL;

    setup_board(instance);

}

View* mine_sweeper_game_screen_get_view(MineSweeperGameScreen* instance) {
    furi_assert(instance);
    return instance->view;
}

void mine_sweeper_game_screen_set_input_callback(MineSweeperGameScreen* instance, GameScreenInputCallback callback) {
    furi_assert(instance);
    instance->input_callback = callback;
}

void mine_sweeper_game_screen_set_context(MineSweeperGameScreen* instance, void* context) {
    furi_assert(instance);
    instance->context = context;
}

void mine_sweeper_game_screen_set_board_dimensions(MineSweeperGameScreen* instance, uint8_t width, uint8_t height) {
    furi_assert(instance);
    
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            model->board_width = width;
            model->board_height = height;
        },
        false);
}

bool mine_sweeper_is_tile_mine(MineSweeperGameScreen* instance, uint16_t x, uint16_t y) {
    furi_assert(instance);
    bool is_mine = false;
    
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            uint16_t pos = x * MINESWEEPER_BOARD_TILE_WIDTH + y;
            if (model->board[pos].tile_type == MineSweeperGameScreenTileMine) {
                is_mine = true;
            }
        },
        false);

    return is_mine;
}
