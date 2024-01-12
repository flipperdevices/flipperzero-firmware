#include "minesweeper_game_screen.h"
#include "minesweeper_redux_icons.h"

#include <gui/elements.h>
#include <gui/icon_animation.h>
#include <input/input.h>

#include <furi.h>
#include <furi_hal.h>

static const Icon* tile_icons[13] = {
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
    int16_t x_abs, y_abs;
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
    MineSweeperTile board[MINESWEEPER_BOARD_MAX_TILES];
    CurrentPosition curr_pos;
    uint8_t right_boundary, bottom_boundary, board_width, board_height, board_difficulty;
    uint16_t mines_left;
    uint16_t flags_left;
    uint16_t tiles_left;
    uint32_t start_tick;
    FuriString* info_str;
    bool ensure_solvable_board;
    bool is_win_triggered;
    bool is_holding_down_button;
} MineSweeperGameScreenModel;

// Multipliers for ratio of mines to tiles
static const float difficulty_multiplier[3] = {
    0.15f,
    0.17f,
    0.19f,
};

// Offsets array used consistently when checking surrounding tiles
static const int8_t offsets[8][2] = {
    {-1, 1},
    {0, 1},
    {1, 1},
    {1, 0},
    {1, -1},
    {0, -1},
    {-1, -1},
    {-1, 0},
};

static MineSweeperTile board_t[MINESWEEPER_BOARD_MAX_TILES];

/****************************************************************
 * Function declarations
 *
 * Non public function declarations
 ***************************************************************/

// Static helper functions

static void setup_board(MineSweeperGameScreen* instance);

static bool check_board_with_verifier(
    MineSweeperTile* board,
    const uint8_t board_width,
    const uint8_t board_height,
    uint16_t total_mines);

static inline void bfs_tile_clear_verifier(
    MineSweeperTile* board,
    const uint8_t board_width,
    const uint8_t board_height,
    const uint16_t x,
    const uint16_t y,
    point_deq_t* edges,
    point_set_t* visited);

static uint16_t bfs_tile_clear(
    MineSweeperTile* board,
    const uint8_t board_width,
    const uint8_t board_height,
    const uint16_t x,
    const uint16_t y);

static void mine_sweeper_game_screen_set_board_information(
    MineSweeperGameScreen* instance,
    const uint8_t width,
    const uint8_t height,
    const uint8_t difficulty,
    bool is_solvable);

static bool try_clear_surrounding_tiles(MineSweeperGameScreenModel* model);

static Point bfs_to_closest_tile(MineSweeperGameScreenModel* model);

// Currently not using enter/exit callback
static void mine_sweeper_game_screen_view_enter(void* context);
static void mine_sweeper_game_screen_view_exit(void* context);

// Different input/draw callbacks for play/win/lose state
static void mine_sweeper_game_screen_view_win_draw_callback(Canvas* canvas, void* _model);
static void mine_sweeper_game_screen_view_lose_draw_callback(Canvas* canvas, void* _model);
static void mine_sweeper_game_screen_view_play_draw_callback(Canvas* canvas, void* _model);

// These consolidate the function calls for led/haptic/sound for specific events
static void mine_sweeper_long_ok_effect(void* context);
static void mine_sweeper_short_ok_effect(void* context);
static void mine_sweeper_flag_effect(void* context);
static void mine_sweeper_move_effect(void* context);
static void mine_sweeper_oob_effect(void* context);
static void mine_sweeper_lose_effect(void* context);
static void mine_sweeper_win_effect(void* context);

static bool mine_sweeper_game_screen_view_end_input_callback(InputEvent* event, void* context);
static bool mine_sweeper_game_screen_view_play_input_callback(InputEvent* event, void* context);

/**************************************************************
 * Function definitions
 *************************************************************/

/**
 * This function is called on alloc, reset, and win/lose condition.
 * It sets up a random board to be checked by the verifier
 */
static void setup_board(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    uint16_t board_tile_count = 0;
    uint8_t board_width = 0, board_height = 0, board_difficulty = 0;

    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            board_width = model->board_width;
            board_height = model->board_height;
            board_tile_count = (model->board_width * model->board_height);
            board_difficulty = model->board_difficulty;
        },
        false);

    uint16_t num_mines = board_tile_count * difficulty_multiplier[board_difficulty];

    /** We can use a temporary buffer to set the tile types initially
     * and manipulate then save to actual model
     */
    MineSweeperGameScreenTileType tiles[MINESWEEPER_BOARD_MAX_TILES];
    memset(&tiles, MineSweeperGameScreenTileNone, sizeof(tiles));

    // Randomly place tiles except in the corners to help guarantee solvability
    for(uint16_t i = 0; i < num_mines; i++) {
        uint16_t rand_pos;
        uint16_t x;
        uint16_t y;
        bool is_invalid_position;
        do {
            rand_pos = furi_hal_random_get() % board_tile_count;
            x = rand_pos / board_width;
            y = rand_pos % board_width;

            is_invalid_position =
                ((rand_pos == 0) || (x == 0 && y == 1) || (x == 1 && y == 0) ||
                 rand_pos == board_tile_count - 1 || (x == 0 && y == board_width - 1) ||
                 (x == board_height - 1 && y == 0));

        } while(tiles[rand_pos] == MineSweeperGameScreenTileMine || is_invalid_position);

        tiles[rand_pos] = MineSweeperGameScreenTileMine;
    }

    /** All mines are set so we look at each tile for surrounding mines */
    for(uint16_t i = 0; i < board_tile_count; i++) {
        MineSweeperGameScreenTileType tile_type = tiles[i];

        if(tile_type == MineSweeperGameScreenTileMine) {
            continue;
        }

        uint16_t mine_count = 0;

        uint16_t x = i / board_width;
        uint16_t y = i % board_width;

        for(uint8_t j = 0; j < 8; j++) {
            int16_t dx = x + (int16_t)offsets[j][0];
            int16_t dy = y + (int16_t)offsets[j][1];

            if(dx < 0 || dy < 0 || dx >= board_height || dy >= board_width) {
                continue;
            }

            uint16_t pos = dx * board_width + dy;
            if(tiles[pos] == MineSweeperGameScreenTileMine) {
                mine_count++;
            }
        }

        tiles[i] = (MineSweeperGameScreenTileType)mine_count + 1;
    }

    // Save tiles to view model
    // Because of way tile enum and tile_icons array is set up we can
    // index tile_icons with the enum type to get the correct Icon*
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            for(uint16_t i = 0; i < board_tile_count; i++) {
                model->board[i].tile_type = tiles[i];
                model->board[i].tile_state = MineSweeperGameScreenTileStateUncleared;
                model->board[i].icon_element.icon = tile_icons[tiles[i]];
                model->board[i].icon_element.x_abs = (i / model->board_width);
                model->board[i].icon_element.y_abs = (i % model->board_width);
            }

            model->mines_left = num_mines;
            model->flags_left = num_mines;
            model->tiles_left = (model->board_width * model->board_height) - model->mines_left;
            model->curr_pos.x_abs = 0;
            model->curr_pos.y_abs = 0;
            model->right_boundary = MINESWEEPER_SCREEN_TILE_WIDTH;
            model->bottom_boundary = MINESWEEPER_SCREEN_TILE_HEIGHT;
            model->is_win_triggered = false;
        },
        true);
}

/**
 *  This function serves as the verifier for a board to check whether it has to be solved ambiguously or not
 *
 *  Returns true if it is unambiguously solvable.
 */
static bool check_board_with_verifier(
    MineSweeperTile* board,
    const uint8_t board_width,
    const uint8_t board_height,
    uint16_t total_mines) {
    furi_assert(board);

    // Double ended queue used to track edges.
    point_deq_t deq;
    point_set_t visited;

    // Ordered Set for visited points
    point_deq_init(deq);
    point_set_init(visited);

    bool is_solvable = false;

    // Point_t pos will be used to keep track of the current point
    Point_t pos;
    pointobj_init(pos);

    // Starting position is 0,0
    Point start_pos = (Point){.x = 0, .y = 0};
    pointobj_set_point(pos, start_pos);

    // Initially bfs clear from 0,0 as it is safe. We should push all 'edges' found
    // into the deq and this will be where we start off from
    bfs_tile_clear_verifier(board, board_width, board_height, 0, 0, &deq, &visited);

    //While we have valid edges to check and have not solved the board
    while(!is_solvable && point_deq_size(deq) > 0) {
        bool is_stuck =
            true; // This variable will track if any flag was placed for any edge to see if we are stuck

        uint16_t deq_size = point_deq_size(deq);

        // Iterate through all edge tiles and push new ones on
        while(deq_size-- > 0) {
            // Pop point and get 1d position in buffer
            point_deq_pop_front(&pos, deq);
            Point curr_pos = pointobj_get_point(pos);
            uint16_t curr_pos_1d = curr_pos.x * board_width + curr_pos.y;

            // Get tile at 1d position
            MineSweeperTile tile = board[curr_pos_1d];
            uint8_t tile_num = tile.tile_type - 1;

            // Track total surrounding tiles and flagged tiles
            uint8_t num_surrounding_tiles = 0;
            uint8_t num_flagged_tiles = 0;

            for(uint8_t j = 0; j < 8; j++) {
                int16_t dx = curr_pos.x + (int16_t)offsets[j][0];
                int16_t dy = curr_pos.y + (int16_t)offsets[j][1];

                if(dx < 0 || dy < 0 || dx >= board_height || dy >= board_width) {
                    continue;
                }

                uint16_t pos = dx * board_width + dy;
                if(board[pos].tile_state == MineSweeperGameScreenTileStateUncleared) {
                    num_surrounding_tiles++;
                } else if(board[pos].tile_state == MineSweeperGameScreenTileStateFlagged) {
                    num_surrounding_tiles++;
                    num_flagged_tiles++;
                }
            }

            if(num_flagged_tiles == tile_num) {
                // If the tile has the same number of surrounding flags as its type we bfs clear the uncleared surrounding tiles
                // pushing new unvisited edges on deq

                for(uint8_t j = 0; j < 8; j++) {
                    int16_t dx = curr_pos.x + (int16_t)offsets[j][0];
                    int16_t dy = curr_pos.y + (int16_t)offsets[j][1];

                    if(dx < 0 || dy < 0 || dx >= board_height || dy >= board_width) {
                        continue;
                    }

                    uint16_t pos = dx * board_width + dy;
                    if(board[pos].tile_state == MineSweeperGameScreenTileStateUncleared) {
                        bfs_tile_clear_verifier(
                            board, board_width, board_height, dx, dy, &deq, &visited);
                    }
                }

                is_stuck = false;

            } else if(num_surrounding_tiles == tile_num) {
                // If the number of surrounding tiles is the tile num it is unambiguous so we place a flag on those tiles,
                // decrement the mine count appropriately and check win condition, and then mark stuck as false

                for(uint8_t j = 0; j < 8; j++) {
                    int16_t dx = curr_pos.x + (int16_t)offsets[j][0];
                    int16_t dy = curr_pos.y + (int16_t)offsets[j][1];

                    if(dx < 0 || dy < 0 || dx >= board_height || dy >= board_width) {
                        continue;
                    }

                    uint16_t pos = dx * board_width + dy;
                    if(board[pos].tile_state == MineSweeperGameScreenTileStateUncleared) {
                        board[pos].tile_state = MineSweeperGameScreenTileStateFlagged;
                    }
                }

                total_mines -= (num_surrounding_tiles - num_flagged_tiles);

                if(total_mines == 0) is_solvable = true;

                is_stuck = false;

            } else if(num_surrounding_tiles != 0) {
                // If we have tiles around this position but the number of flagged tiles != tile num
                // and the surrounding tiles != tile num this means the tile is ambiguous. We can push
                // it back on the deq to be reprocessed with any other new edges

                point_deq_push_back(deq, pos);
            }
        }

        // If we are stuck we break as it is an ambiguous map generation
        if(is_stuck) {
            break;
        }
    }

    point_set_clear(visited);
    point_deq_clear(deq);

    return is_solvable;
}

/**
 * This is a bfs_tile clear used by the verifier which performs the normal tile clear
 * but also pushes new edges to the deq passed in. There is a separate function used
 * for the bfs_tile_clear used on the user click
 */
static inline void bfs_tile_clear_verifier(
    MineSweeperTile* board,
    const uint8_t board_width,
    const uint8_t board_height,
    const uint16_t x,
    const uint16_t y,
    point_deq_t* edges,
    point_set_t* visited) {
    furi_assert(board);
    furi_assert(edges);
    furi_assert(visited);

    // Init both the set and dequeue
    point_deq_t deq;
    point_set_t set;

    point_deq_init(deq);
    point_set_init(set);

    // Point_t pos will be used to keep track of the current point
    Point_t pos;
    pointobj_init(pos);

    // Starting position is current pos
    Point start_pos = (Point){.x = x, .y = y};
    pointobj_set_point(pos, start_pos);

    point_deq_push_back(deq, pos);

    while(point_deq_size(deq) > 0) {
        point_deq_pop_front(&pos, deq);
        Point curr_pos = pointobj_get_point(pos);
        uint16_t curr_pos_1d = curr_pos.x * board_width + curr_pos.y;

        // If in visited set
        if(point_set_cget(set, pos) != NULL || point_set_cget(*visited, pos) != NULL) {
        }

        // If it is cleared continue
        if(board[curr_pos_1d].tile_state == MineSweeperGameScreenTileStateCleared) {
            continue;
        }

        // Else set tile to cleared
        board[curr_pos_1d].tile_state = MineSweeperGameScreenTileStateCleared;

        // Add point to visited set
        point_set_push(set, pos);

        //When we hit a potential edge
        if(board[curr_pos_1d].tile_type != MineSweeperGameScreenTileZero) {
            // We can push this edge into edges if it is not in visited, as it is a new edge
            // and also add to the visited set
            if(point_set_cget(*visited, pos) == NULL) {
                point_deq_push_back(*edges, pos);
                point_set_push(*visited, pos);
            }

            // Continue processing next point for bfs tile clear
            continue;
        }

        // Process all surrounding neighbors and add valid to dequeue
        for(uint8_t i = 0; i < 8; i++) {
            int16_t dx = curr_pos.x + (int16_t)offsets[i][0];
            int16_t dy = curr_pos.y + (int16_t)offsets[i][1];

            if(dx < 0 || dy < 0 || dx >= board_height || dy >= board_width) {
                continue;
            }

            Point neighbor = (Point){.x = dx, .y = dy};
            pointobj_set_point(pos, neighbor);

            if(point_set_cget(set, pos) != NULL || point_set_cget(*visited, pos) != NULL) continue;

            point_deq_push_back(deq, pos);
        }
    }

    point_set_clear(set);
    point_deq_clear(deq);
}

/**
 * This is a bfs_tile clear used in the input callbacks to clear the board on user input
 */
static inline uint16_t bfs_tile_clear(
    MineSweeperTile* board,
    const uint8_t board_width,
    const uint8_t board_height,
    const uint16_t x,
    const uint16_t y) {
    furi_assert(board);

    // We will return this number as the number of tiles cleared
    uint16_t ret = 0;

    // Init both the set and dequeue
    point_deq_t deq;
    point_set_t set;

    point_deq_init(deq);
    point_set_init(set);

    // Point_t pos will be used to keep track of the current point
    Point_t pos;
    pointobj_init(pos);

    // Starting position is current pos
    Point start_pos = (Point){.x = x, .y = y};
    pointobj_set_point(pos, start_pos);

    point_deq_push_back(deq, pos);

    while(point_deq_size(deq) > 0) {
        point_deq_pop_front(&pos, deq);
        Point curr_pos = pointobj_get_point(pos);
        uint16_t curr_pos_1d = curr_pos.x * board_width + curr_pos.y;

        // If in visited set
        if(point_set_cget(set, pos) != NULL) {
            continue;
        }

        // If it is not uncleared continue
        if(board[curr_pos_1d].tile_state != MineSweeperGameScreenTileStateUncleared) {
            continue;
        }

        // Else set tile to cleared
        board[curr_pos_1d].tile_state = MineSweeperGameScreenTileStateCleared;

        // Increment total number of cleared tiles
        ret++;

        // Add point to visited set
        point_set_push(set, pos);

        // If it is not a zero tile continue
        if(board[curr_pos_1d].tile_type != MineSweeperGameScreenTileZero) {
            continue;
        }

        // Process all surrounding neighbors and add valid to dequeue
        for(uint8_t i = 0; i < 8; i++) {
            int16_t dx = curr_pos.x + (int16_t)offsets[i][0];
            int16_t dy = curr_pos.y + (int16_t)offsets[i][1];

            if(dx < 0 || dy < 0 || dx >= board_height || dy >= board_width) {
                continue;
            }

            Point neighbor = (Point){.x = dx, .y = dy};
            pointobj_set_point(pos, neighbor);

            if(point_set_cget(set, pos) != NULL) continue;

            point_deq_push_back(deq, pos);
        }
    }

    point_set_clear(set);
    point_deq_clear(deq);

    return ret;
}

static void mine_sweeper_game_screen_set_board_information(
    MineSweeperGameScreen* instance,
    uint8_t width,
    uint8_t height,
    uint8_t difficulty,
    bool is_solvable) {
    furi_assert(instance);

    // These are the min/max values that can actually be set
    if(width > 146) {
        width = 146;
    }
    if(width < 16) {
        width = 16;
    }
    if(height > 64) {
        height = 64;
    }
    if(height < 7) {
        height = 7;
    }
    if(difficulty > 2) {
        difficulty = 2;
    }

    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            model->board_width = width;
            model->board_height = height;
            model->board_difficulty = difficulty;
            model->ensure_solvable_board = is_solvable;
        },
        true);
}

// THIS FUNCTION CAN TRIGGER THE LOSE CONDITION
static bool try_clear_surrounding_tiles(MineSweeperGameScreenModel* model) {
    furi_assert(model);

    uint8_t curr_x = model->curr_pos.x_abs;
    uint8_t curr_y = model->curr_pos.y_abs;
    uint8_t board_width = model->board_width;
    uint8_t board_height = model->board_height;
    uint16_t curr_pos_1d = curr_x * board_width + curr_y;

    MineSweeperTile tile = model->board[curr_pos_1d];

    // Return true if tile is zero tile or not cleared
    if(tile.tile_state != MineSweeperGameScreenTileStateCleared ||
       tile.tile_type == MineSweeperGameScreenTileZero) {
        return false;
    }

    uint8_t num_surrounding_flagged = 0;
    bool was_mine_found = false;
    bool is_lose_condition_triggered = false;

    for(uint8_t j = 0; j < 8; j++) {
        int16_t dx = curr_x + (int16_t)offsets[j][0];
        int16_t dy = curr_y + (int16_t)offsets[j][1];

        if(dx < 0 || dy < 0 || dx >= board_height || dy >= board_width) {
            continue;
        }

        uint16_t pos = dx * board_width + dy;
        if(model->board[pos].tile_state == MineSweeperGameScreenTileStateFlagged) {
            num_surrounding_flagged++;
        } else if(
            !was_mine_found && model->board[pos].tile_type == MineSweeperGameScreenTileMine &&
            model->board[pos].tile_state != MineSweeperGameScreenTileStateFlagged) {
            was_mine_found = true;
        }
    }

    // We clear surrounding tile
    if(num_surrounding_flagged >= tile.tile_type - 1) {
        if(was_mine_found) is_lose_condition_triggered = true;

        for(uint8_t j = 0; j < 8; j++) {
            int16_t dx = curr_x + (int16_t)offsets[j][0];
            int16_t dy = curr_y + (int16_t)offsets[j][1];

            if(dx < 0 || dy < 0 || dx >= board_height || dy >= board_width) {
                continue;
            }

            uint16_t pos = dx * board_width + dy;
            if(model->board[pos].tile_state == MineSweeperGameScreenTileStateUncleared) {
                // Decrement tiles left by the amount cleared
                uint16_t tiles_cleared =
                    bfs_tile_clear(model->board, model->board_width, model->board_height, dx, dy);
                model->tiles_left -= tiles_cleared;
            }
        }
    }

    return is_lose_condition_triggered;
}

/**
 * Function is used on a long backpress on a cleared tile and returns the position
 * of the first found uncleared tile using a bfs search
 */
static inline Point bfs_to_closest_tile(MineSweeperGameScreenModel* model) {
    furi_assert(model);

    // Init both the set and dequeue
    point_deq_t deq;
    point_set_t set;

    point_deq_init(deq);
    point_set_init(set);

    // Return the value in this point
    Point result;

    // Point_t pos will be used to keep track of the current point
    Point_t pos;
    pointobj_init(pos);

    // Starting position is current pos
    Point start_pos = (Point){.x = model->curr_pos.x_abs, .y = model->curr_pos.y_abs};
    pointobj_set_point(pos, start_pos);

    point_deq_push_back(deq, pos);

    while(point_deq_size(deq) > 0) {
        point_deq_pop_front(&pos, deq);
        Point curr_pos = pointobj_get_point(pos);
        uint16_t curr_pos_1d = curr_pos.x * model->board_width + curr_pos.y;

        // If the current tile is uncleared and not start pos we save result
        // to this position and break
        if(model->board[curr_pos_1d].tile_state == MineSweeperGameScreenTileStateUncleared &&
           !(start_pos.x == curr_pos.x && start_pos.y == curr_pos.y)) {
            result = curr_pos;
            break;
        }

        // If in visited set continue
        if(point_set_cget(set, pos) != NULL) {
            continue;
        }

        // Add point to visited set
        point_set_push(set, pos);

        // Process all surrounding neighbors and add valid to dequeue
        for(uint8_t i = 0; i < 8; i++) {
            int16_t dx = curr_pos.x + (int16_t)offsets[i][0];
            int16_t dy = curr_pos.y + (int16_t)offsets[i][1];

            if(dx < 0 || dy < 0 || dx >= model->board_height || dy >= model->board_width) {
                continue;
            }

            Point neighbor = (Point){.x = dx, .y = dy};
            pointobj_set_point(pos, neighbor);
            point_deq_push_back(deq, pos);
        }
    }

    point_set_clear(set);
    point_deq_clear(deq);

    return result;
}

static void mine_sweeper_game_screen_view_enter(void* context) {
    furi_assert(context);
    UNUSED(context);
}

static void mine_sweeper_game_screen_view_exit(void* context) {
    furi_assert(context);
    UNUSED(context);
}

static void mine_sweeper_game_screen_view_win_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(canvas);
    furi_assert(_model);
    MineSweeperGameScreenModel* model = _model;

    canvas_clear(canvas);

    canvas_set_color(canvas, ColorBlack);

    uint16_t cursor_pos_1d = model->curr_pos.x_abs * model->board_width + model->curr_pos.y_abs;

    for(uint8_t x_rel = 0; x_rel < MINESWEEPER_SCREEN_TILE_HEIGHT; x_rel++) {
        uint16_t x_abs = (model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT) + x_rel;

        for(uint8_t y_rel = 0; y_rel < MINESWEEPER_SCREEN_TILE_WIDTH; y_rel++) {
            uint16_t y_abs = (model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH) + y_rel;

            uint16_t curr_rendering_tile_pos_1d = x_abs * model->board_width + y_abs;
            MineSweeperTile tile = model->board[curr_rendering_tile_pos_1d];

            if(cursor_pos_1d == curr_rendering_tile_pos_1d) {
                canvas_set_color(canvas, ColorWhite);
            } else {
                canvas_set_color(canvas, ColorBlack);
            }

            canvas_draw_icon(
                canvas,
                y_rel * icon_get_width(tile.icon_element.icon),
                x_rel * icon_get_height(tile.icon_element.icon),
                tile.icon_element.icon);
        }
    }

    canvas_set_color(canvas, ColorBlack);
    // If any borders are at the limits of the game board we draw a border line

    // Right border
    if(model->right_boundary == model->board_width) {
        canvas_draw_line(canvas, 127, 0, 127, 63 - 8);
    }

    // Left border
    if((model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH) == 0) {
        canvas_draw_line(canvas, 0, 0, 0, 63 - 8);
    }

    // Bottom border
    if(model->bottom_boundary == model->board_height) {
        canvas_draw_line(canvas, 0, 63 - 8, 127, 63 - 8);
    }

    // Top border
    if((model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT) == 0) {
        canvas_draw_line(canvas, 0, 0, 127, 0);
    }

    // Draw win text
    furi_string_printf(model->info_str, "YOU WIN!");

    canvas_draw_str_aligned(
        canvas, 0, 64 - 7, AlignLeft, AlignTop, furi_string_get_cstr(model->info_str));

    // Draw time text
    uint32_t ticks_elapsed = furi_get_tick() - model->start_tick;
    uint32_t sec = ticks_elapsed / furi_kernel_get_tick_frequency();
    uint32_t minutes = sec / 60;
    sec = sec % 60;

    furi_string_printf(model->info_str, "%02ld:%02ld", minutes, sec);

    canvas_draw_str_aligned(
        canvas,
        126 - canvas_string_width(canvas, furi_string_get_cstr(model->info_str)),
        64 - 7,
        AlignLeft,
        AlignTop,
        furi_string_get_cstr(model->info_str));
}

static void mine_sweeper_game_screen_view_lose_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(canvas);
    furi_assert(_model);
    MineSweeperGameScreenModel* model = _model;

    canvas_clear(canvas);

    uint16_t cursor_pos_1d = model->curr_pos.x_abs * model->board_width + model->curr_pos.y_abs;

    for(uint8_t x_rel = 0; x_rel < MINESWEEPER_SCREEN_TILE_HEIGHT; x_rel++) {
        uint16_t x_abs = (model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT) + x_rel;

        for(uint8_t y_rel = 0; y_rel < MINESWEEPER_SCREEN_TILE_WIDTH; y_rel++) {
            uint16_t y_abs = (model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH) + y_rel;

            uint16_t curr_rendering_tile_pos_1d = x_abs * model->board_width + y_abs;
            MineSweeperTile tile = model->board[curr_rendering_tile_pos_1d];

            if(cursor_pos_1d == curr_rendering_tile_pos_1d) {
                canvas_set_color(canvas, ColorWhite);
            } else {
                canvas_set_color(canvas, ColorBlack);
            }

            canvas_draw_icon(
                canvas,
                y_rel * icon_get_width(tile.icon_element.icon),
                x_rel * icon_get_height(tile.icon_element.icon),
                tile.icon_element.icon);
        }
    }

    canvas_set_color(canvas, ColorBlack);
    // If any borders are at the limits of the game board we draw a border line

    // Right border
    if(model->right_boundary == model->board_width) {
        canvas_draw_line(canvas, 127, 0, 127, 63 - 8);
    }

    // Left border
    if((model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH) == 0) {
        canvas_draw_line(canvas, 0, 0, 0, 63 - 8);
    }

    // Bottom border
    if(model->bottom_boundary == model->board_height) {
        canvas_draw_line(canvas, 0, 63 - 8, 127, 63 - 8);
    }

    // Top border
    if((model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT) == 0) {
        canvas_draw_line(canvas, 0, 0, 127, 0);
    }

    // Draw lose text
    furi_string_printf(model->info_str, "YOU LOSE!");

    canvas_draw_str_aligned(
        canvas, 0, 64 - 7, AlignLeft, AlignTop, furi_string_get_cstr(model->info_str));

    // Draw time text
    uint32_t ticks_elapsed = furi_get_tick() - model->start_tick;
    uint32_t sec = ticks_elapsed / furi_kernel_get_tick_frequency();
    uint32_t minutes = sec / 60;
    sec = sec % 60;

    furi_string_printf(model->info_str, "%02ld:%02ld", minutes, sec);

    canvas_draw_str_aligned(
        canvas,
        126 - canvas_string_width(canvas, furi_string_get_cstr(model->info_str)),
        64 - 7,
        AlignLeft,
        AlignTop,
        furi_string_get_cstr(model->info_str));
}

static void mine_sweeper_game_screen_view_play_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(canvas);
    furi_assert(_model);

    MineSweeperGameScreenModel* model = _model;

    canvas_clear(canvas);

    uint16_t cursor_pos_1d = model->curr_pos.x_abs * model->board_width + model->curr_pos.y_abs;

    for(uint8_t x_rel = 0; x_rel < MINESWEEPER_SCREEN_TILE_HEIGHT; x_rel++) {
        uint16_t x_abs = (model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT) + x_rel;

        for(uint8_t y_rel = 0; y_rel < MINESWEEPER_SCREEN_TILE_WIDTH; y_rel++) {
            uint16_t y_abs = (model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH) + y_rel;

            uint16_t curr_rendering_tile_pos_1d = x_abs * model->board_width + y_abs;
            MineSweeperTile tile = model->board[curr_rendering_tile_pos_1d];

            if(cursor_pos_1d == curr_rendering_tile_pos_1d) {
                canvas_set_color(canvas, ColorWhite);
            } else {
                canvas_set_color(canvas, ColorBlack);
            }

            switch(tile.tile_state) {
            case MineSweeperGameScreenTileStateFlagged:
                canvas_draw_icon(
                    canvas,
                    y_rel * icon_get_width(tile.icon_element.icon),
                    x_rel * icon_get_height(tile.icon_element.icon),
                    tile_icons[11]);

                break;
            case MineSweeperGameScreenTileStateUncleared:
                canvas_draw_icon(
                    canvas,
                    y_rel * icon_get_width(tile.icon_element.icon),
                    x_rel * icon_get_height(tile.icon_element.icon),
                    tile_icons[12]);

                break;
            case MineSweeperGameScreenTileStateCleared:
                canvas_draw_icon(
                    canvas,
                    y_rel * icon_get_width(tile.icon_element.icon),
                    x_rel * icon_get_height(tile.icon_element.icon),
                    tile.icon_element.icon);
                break;
            default:
                break;
            }
        }
    }

    canvas_set_color(canvas, ColorBlack);
    // If any borders are at the limits of the game board we draw a border line

    // Right border
    if(model->right_boundary == model->board_width) {
        canvas_draw_line(canvas, 127, 0, 127, 63 - 8);
    }

    // Left border
    if((model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH) == 0) {
        canvas_draw_line(canvas, 0, 0, 0, 63 - 8);
    }

    // Bottom border
    if(model->bottom_boundary == model->board_height) {
        canvas_draw_line(canvas, 0, 63 - 8, 127, 63 - 8);
    }

    // Top border
    if((model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT) == 0) {
        canvas_draw_line(canvas, 0, 0, 127, 0);
    }

    // Draw X Position Text
    furi_string_printf(model->info_str, "X:%03hhd", model->curr_pos.x_abs);

    canvas_draw_str_aligned(
        canvas, 0, 64 - 7, AlignLeft, AlignTop, furi_string_get_cstr(model->info_str));

    // Draw Y Position Text
    furi_string_printf(model->info_str, "Y:%03hhd", model->curr_pos.y_abs);

    canvas_draw_str_aligned(
        canvas, 33, 64 - 7, AlignLeft, AlignTop, furi_string_get_cstr(model->info_str));

    // Draw flag text
    furi_string_printf(model->info_str, "F:%03hd", model->flags_left);

    canvas_draw_str_aligned(
        canvas, 66, 64 - 7, AlignLeft, AlignTop, furi_string_get_cstr(model->info_str));

    // Draw time text
    uint32_t ticks_elapsed = furi_get_tick() - model->start_tick;
    uint32_t sec = ticks_elapsed / furi_kernel_get_tick_frequency();
    uint32_t minutes = sec / 60;
    sec = sec % 60;

    furi_string_printf(model->info_str, "%02ld:%02ld", minutes, sec);

    canvas_draw_str_aligned(
        canvas,
        126 - canvas_string_width(canvas, furi_string_get_cstr(model->info_str)),
        64 - 7,
        AlignLeft,
        AlignTop,
        furi_string_get_cstr(model->info_str));
}

static void mine_sweeper_short_ok_effect(void* context) {
    furi_assert(context);
    MineSweeperGameScreen* instance = context;

    mine_sweeper_led_blink_magenta(instance->context);
    mine_sweeper_play_ok_sound(instance->context);
    mine_sweeper_play_happy_bump(instance->context);
    mine_sweeper_stop_all_sound(instance->context);
}

static void mine_sweeper_long_ok_effect(void* context) {
    furi_assert(context);
    MineSweeperGameScreen* instance = context;

    mine_sweeper_led_blink_magenta(instance->context);
    mine_sweeper_play_ok_sound(instance->context);
    mine_sweeper_play_long_ok_bump(instance->context);
    mine_sweeper_stop_all_sound(instance->context);
}

static void mine_sweeper_flag_effect(void* context) {
    furi_assert(context);
    MineSweeperGameScreen* instance = context;

    mine_sweeper_led_blink_cyan(instance->context);
    mine_sweeper_play_flag_sound(instance->context);
    mine_sweeper_play_happy_bump(instance->context);
    mine_sweeper_stop_all_sound(instance->context);
}

static void mine_sweeper_move_effect(void* context) {
    furi_assert(context);
    MineSweeperGameScreen* instance = context;

    mine_sweeper_play_happy_bump(instance->context);
}

static void mine_sweeper_oob_effect(void* context) {
    furi_assert(context);
    MineSweeperGameScreen* instance = context;

    mine_sweeper_led_blink_red(instance->context);
    mine_sweeper_play_flag_sound(instance->context);
    mine_sweeper_play_oob_bump(instance->context);
    mine_sweeper_stop_all_sound(instance->context);
}

static void mine_sweeper_lose_effect(void* context) {
    furi_assert(context);
    MineSweeperGameScreen* instance = context;

    mine_sweeper_led_set_rgb(instance->context, 255, 0, 000);
    mine_sweeper_play_lose_sound(instance->context);
    mine_sweeper_play_lose_bump(instance->context);
    mine_sweeper_stop_all_sound(instance->context);
}

static void mine_sweeper_win_effect(void* context) {
    furi_assert(context);
    MineSweeperGameScreen* instance = context;

    mine_sweeper_led_set_rgb(instance->context, 0, 0, 255);
    mine_sweeper_play_win_sound(instance->context);
    mine_sweeper_play_win_bump(instance->context);
    mine_sweeper_stop_all_sound(instance->context);
}

static bool mine_sweeper_game_screen_view_end_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    furi_assert(event);

    MineSweeperGameScreen* instance = context;
    bool consumed = false;

    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        {
            if(event->type == InputTypeRelease) {
                model->is_holding_down_button = false;
                consumed = true;

            } else if(
                !model->is_holding_down_button &&
                (event->type == InputTypePress || event->type == InputTypeRepeat)) {
                bool is_outside_boundary;
                switch(event->key) {
                case InputKeyUp:
                    model->curr_pos.x_abs =
                        (model->curr_pos.x_abs - 1 < 0) ? 0 : model->curr_pos.x_abs - 1;

                    is_outside_boundary = model->curr_pos.x_abs < (model->bottom_boundary -
                                                                   MINESWEEPER_SCREEN_TILE_HEIGHT);

                    if(is_outside_boundary) {
                        model->bottom_boundary--;
                    }

                    consumed = true;
                    break;

                case InputKeyDown:
                    model->curr_pos.x_abs = (model->curr_pos.x_abs + 1 >= model->board_height) ?
                                                model->board_height - 1 :
                                                model->curr_pos.x_abs + 1;

                    is_outside_boundary = model->curr_pos.x_abs >= model->bottom_boundary;

                    if(is_outside_boundary) {
                        model->bottom_boundary++;
                    }

                    consumed = true;
                    break;

                case InputKeyLeft:
                    model->curr_pos.y_abs =
                        (model->curr_pos.y_abs - 1 < 0) ? 0 : model->curr_pos.y_abs - 1;

                    is_outside_boundary = model->curr_pos.y_abs <
                                          (model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH);

                    if(is_outside_boundary) {
                        model->right_boundary--;
                    }

                    consumed = true;
                    break;

                case InputKeyRight:
                    model->curr_pos.y_abs = (model->curr_pos.y_abs + 1 >= model->board_width) ?
                                                model->board_width - 1 :
                                                model->curr_pos.y_abs + 1;

                    is_outside_boundary = model->curr_pos.y_abs >= model->right_boundary;

                    if(is_outside_boundary) {
                        model->right_boundary++;
                    }

                    consumed = true;
                    break;

                default: // Anything other than movement around the screen should restart game
                    mine_sweeper_led_reset(instance->context);

                    mine_sweeper_game_screen_reset_clock(instance);
                    view_set_draw_callback(
                        instance->view, mine_sweeper_game_screen_view_play_draw_callback);
                    view_set_input_callback(
                        instance->view, mine_sweeper_game_screen_view_play_input_callback);

                    // Here we are going to generate a valid map for the player
                    bool is_valid_board = false;

                    size_t memsz = sizeof(MineSweeperTile) * MINESWEEPER_BOARD_MAX_TILES;

                    do {
                        setup_board(instance);

                        memset(board_t, 0, memsz);
                        memcpy(
                            board_t,
                            model->board,
                            sizeof(MineSweeperTile) * (model->board_width * model->board_height));

                        is_valid_board = check_board_with_verifier(
                            board_t, model->board_width, model->board_height, model->mines_left);

                    } while(model->ensure_solvable_board && !is_valid_board);

                    consumed = true;
                    break;
                }

                consumed = true;
            }
        },
        false);

    return consumed;
}

static bool mine_sweeper_game_screen_view_play_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    furi_assert(event);

    MineSweeperGameScreen* instance = context;
    bool consumed = false;

    // Checking button types

    if(event->type == InputTypeRelease) {
        with_view_model(
            instance->view,
            MineSweeperGameScreenModel * model,
            {
                model->is_holding_down_button = false;
                consumed = true;
            },
            true);
    }

    if(!consumed &&
       event->key == InputKeyOk) { // Attempt to Clear Space !! THIS CAN BE A LOSE CONDITION

        bool is_lose_condition_triggered = false;
        bool is_win_condition_triggered = false;

        with_view_model(
            instance->view,
            MineSweeperGameScreenModel * model,
            {
                uint16_t curr_pos_1d =
                    model->curr_pos.x_abs * model->board_width + model->curr_pos.y_abs;

                if(!model->is_holding_down_button && event->type == InputTypePress) {
                    MineSweeperGameScreenTileState state = model->board[curr_pos_1d].tile_state;
                    MineSweeperGameScreenTileType type = model->board[curr_pos_1d].tile_type;

                    // LOSE/WIN CONDITION OR TILE CLEAR
                    if(state == MineSweeperGameScreenTileStateUncleared &&
                       type == MineSweeperGameScreenTileMine) {
                        is_lose_condition_triggered = true;
                        model->board[curr_pos_1d].tile_state =
                            MineSweeperGameScreenTileStateCleared;

                    } else if(state == MineSweeperGameScreenTileStateUncleared) {
                        uint16_t tiles_cleared = bfs_tile_clear(
                            model->board,
                            model->board_width,
                            model->board_height,
                            (uint16_t)model->curr_pos.x_abs,
                            (uint16_t)model->curr_pos.y_abs);

                        model->tiles_left -= tiles_cleared;

                        // Check win condition
                        if(model->mines_left == 0 && model->flags_left == 0 &&
                           model->tiles_left == 0) {
                            is_win_condition_triggered = true;
                        } else {
                            // if not met play ok effect
                            mine_sweeper_short_ok_effect(instance);
                        }
                    }

                    // LOSE/WIN CONDITION OR CLEAR SURROUNDING
                } else if(!model->is_holding_down_button && event->type == InputTypeLong) {
                    // Try to clear surrounding tiles if correct number is flagged.
                    is_lose_condition_triggered = try_clear_surrounding_tiles(model);
                    model->is_holding_down_button = true;

                    // Check win condition
                    if(model->mines_left == 0 && model->flags_left == 0 &&
                       model->tiles_left == 0) {
                        is_win_condition_triggered = true;
                    }

                    // We need to check if it is ok to play this or else we conflict
                    // with the lose effect and crash
                    if(!is_win_condition_triggered && !is_lose_condition_triggered &&
                       model->board[curr_pos_1d].tile_type != MineSweeperGameScreenTileZero) {
                        mine_sweeper_long_ok_effect(instance);
                    }
                }
            },
            true);

        // Check  if win or lose condition was triggered on OK press
        if(is_lose_condition_triggered) {
            mine_sweeper_lose_effect(instance);
            view_set_draw_callback(
                instance->view, mine_sweeper_game_screen_view_lose_draw_callback);
            view_set_input_callback(
                instance->view, mine_sweeper_game_screen_view_end_input_callback);
        } else if(is_win_condition_triggered) {
            mine_sweeper_win_effect(instance);
            view_set_draw_callback(
                instance->view, mine_sweeper_game_screen_view_win_draw_callback);
            view_set_input_callback(
                instance->view, mine_sweeper_game_screen_view_end_input_callback);
        }

        consumed = true;
    }

    if(!consumed &&
       (event->key == InputKeyBack)) { // We can use holding the back button for either
        // Setting a flag on a covered tile, or moving to
        // the next closest covered tile on when on a uncovered
        // tile

        if(event->type == InputTypeLong ||
           event->type == InputTypeRepeat) { // Only process longer back keys;
            // short presses should take
            // us to the menu
            with_view_model(
                instance->view,
                MineSweeperGameScreenModel * model,
                {
                    uint16_t curr_pos_1d =
                        model->curr_pos.x_abs * model->board_width + model->curr_pos.y_abs;

                    MineSweeperGameScreenTileState state = model->board[curr_pos_1d].tile_state;

                    if(state == MineSweeperGameScreenTileStateCleared) {
                        // BFS to closest uncovered position
                        Point res = bfs_to_closest_tile(model);

                        // Save cursor to new closest tile position
                        // If the cursor moves outisde of the model boundaries we need to
                        // move the boundary appropriately

                        model->curr_pos.x_abs = res.x;
                        model->curr_pos.y_abs = res.y;

                        bool is_outside_top_boundary =
                            model->curr_pos.x_abs <
                            (model->bottom_boundary - MINESWEEPER_SCREEN_TILE_HEIGHT);

                        bool is_outside_bottom_boundary = model->curr_pos.x_abs >=
                                                          model->bottom_boundary;

                        bool is_outside_left_boundary =
                            model->curr_pos.y_abs <
                            (model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH);

                        bool is_outside_right_boundary = model->curr_pos.y_abs >=
                                                         model->right_boundary;

                        if(is_outside_top_boundary) {
                            model->bottom_boundary =
                                model->curr_pos.x_abs + MINESWEEPER_SCREEN_TILE_HEIGHT;
                        } else if(is_outside_bottom_boundary) {
                            model->bottom_boundary = model->curr_pos.x_abs + 1;
                        }

                        if(is_outside_right_boundary) {
                            model->right_boundary = model->curr_pos.y_abs + 1;
                        } else if(is_outside_left_boundary) {
                            model->right_boundary =
                                model->curr_pos.y_abs + MINESWEEPER_SCREEN_TILE_WIDTH;
                        }

                        mine_sweeper_play_happy_bump(instance->context);
                        model->is_holding_down_button = true;

                        // Flag or Unflag tile and check win condition
                    } else if(
                        !model->is_holding_down_button &&
                        (state == MineSweeperGameScreenTileStateUncleared ||
                         state == MineSweeperGameScreenTileStateFlagged)) {
                        if(state == MineSweeperGameScreenTileStateFlagged) {
                            if(model->board[curr_pos_1d].tile_type ==
                               MineSweeperGameScreenTileMine)
                                model->mines_left++;
                            model->board[curr_pos_1d].tile_state =
                                MineSweeperGameScreenTileStateUncleared;
                            model->flags_left++;
                            model->is_holding_down_button = true;

                        } else if(model->flags_left > 0) {
                            if(model->board[curr_pos_1d].tile_type ==
                               MineSweeperGameScreenTileMine)
                                model->mines_left--;
                            model->board[curr_pos_1d].tile_state =
                                MineSweeperGameScreenTileStateFlagged;
                            model->flags_left--;
                            model->is_holding_down_button = true;
                        }

                        // WIN CONDITION
                        // This can be a win condition where the non-mine tiles are cleared and they place the last flag
                        if(model->flags_left == 0 && model->mines_left == 0 &&
                           model->tiles_left == 0) {
                            //mine_sweeper_play_long_bump(instance->context);

                            mine_sweeper_win_effect(instance);
                            mine_sweeper_led_set_rgb(instance->context, 0, 0, 255);

                            view_set_draw_callback(
                                instance->view, mine_sweeper_game_screen_view_win_draw_callback);
                            view_set_input_callback(
                                instance->view, mine_sweeper_game_screen_view_end_input_callback);
                        } else {
                            // Making sure that win and flag effect are not played together
                            mine_sweeper_flag_effect(instance);
                        }
                    }
                },
                false);

            consumed = true;
        }
    }

    if(!consumed &&
       (event->type == InputTypePress || event->type == InputTypeRepeat)) { // Finally handle move

        with_view_model(
            instance->view,
            MineSweeperGameScreenModel * model,
            {
                bool is_outside_boundary;
                switch(event->key) {
                case InputKeyUp:
                    (model->curr_pos.x_abs - 1 < 0) ? mine_sweeper_oob_effect(instance) :
                                                      mine_sweeper_move_effect(instance);

                    model->curr_pos.x_abs =
                        (model->curr_pos.x_abs - 1 < 0) ? 0 : model->curr_pos.x_abs - 1;

                    is_outside_boundary = model->curr_pos.x_abs < (model->bottom_boundary -
                                                                   MINESWEEPER_SCREEN_TILE_HEIGHT);

                    if(is_outside_boundary) {
                        model->bottom_boundary--;
                    }

                    consumed = true;
                    break;

                case InputKeyDown:

                    (model->curr_pos.x_abs + 1 >= model->board_height) ?
                        mine_sweeper_oob_effect(instance) :
                        mine_sweeper_move_effect(instance);

                    model->curr_pos.x_abs = (model->curr_pos.x_abs + 1 >= model->board_height) ?
                                                model->board_height - 1 :
                                                model->curr_pos.x_abs + 1;

                    is_outside_boundary = model->curr_pos.x_abs >= model->bottom_boundary;

                    if(is_outside_boundary) {
                        model->bottom_boundary++;
                    }

                    consumed = true;
                    break;

                case InputKeyLeft:
                    (model->curr_pos.y_abs - 1 < 0) ? mine_sweeper_oob_effect(instance) :
                                                      mine_sweeper_move_effect(instance);

                    model->curr_pos.y_abs =
                        (model->curr_pos.y_abs - 1 < 0) ? 0 : model->curr_pos.y_abs - 1;

                    is_outside_boundary = model->curr_pos.y_abs <
                                          (model->right_boundary - MINESWEEPER_SCREEN_TILE_WIDTH);

                    if(is_outside_boundary) {
                        model->right_boundary--;
                    }

                    consumed = true;
                    break;

                case InputKeyRight:
                    (model->curr_pos.y_abs + 1 >= model->board_width) ?
                        mine_sweeper_oob_effect(instance) :
                        mine_sweeper_move_effect(instance);

                    model->curr_pos.y_abs = (model->curr_pos.y_abs + 1 >= model->board_width) ?
                                                model->board_width - 1 :
                                                model->curr_pos.y_abs + 1;

                    is_outside_boundary = model->curr_pos.y_abs >= model->right_boundary;

                    if(is_outside_boundary) {
                        model->right_boundary++;
                    }

                    consumed = true;
                    break;

                default:
                    consumed = true;
                    break;
                }
            },
            true);
    }

    if(!consumed && instance->input_callback != NULL) {
        consumed = instance->input_callback(event, instance->context);
    }

    return consumed;
}

MineSweeperGameScreen* mine_sweeper_game_screen_alloc(
    uint8_t width,
    uint8_t height,
    uint8_t difficulty,
    bool ensure_solvable) {
    MineSweeperGameScreen* mine_sweeper_game_screen =
        (MineSweeperGameScreen*)malloc(sizeof(MineSweeperGameScreen));

    mine_sweeper_game_screen->view = view_alloc();

    view_set_context(mine_sweeper_game_screen->view, mine_sweeper_game_screen);
    view_allocate_model(
        mine_sweeper_game_screen->view, ViewModelTypeLocking, sizeof(MineSweeperGameScreenModel));

    view_set_draw_callback(
        mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_play_draw_callback);
    view_set_input_callback(
        mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_play_input_callback);

    // This are currently unused
    view_set_enter_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_enter);
    view_set_exit_callback(mine_sweeper_game_screen->view, mine_sweeper_game_screen_view_exit);

    // Not being used
    mine_sweeper_game_screen->input_callback = NULL;

    // Allocate strings in model
    with_view_model(
        mine_sweeper_game_screen->view,
        MineSweeperGameScreenModel * model,
        {
            model->info_str = furi_string_alloc();
            model->is_holding_down_button = false;
        },
        true);

    // Reset the clock - This will set the start time at the allocation of the game screen
    // but this is a public api as well and can be called in a scene for more accurate start times
    mine_sweeper_game_screen_reset_clock(mine_sweeper_game_screen);

    // We need to initize board width and height before setup
    mine_sweeper_game_screen_set_board_information(
        mine_sweeper_game_screen, width, height, difficulty, ensure_solvable);

    // Here we are going to generate a valid map for the player
    bool is_valid_board = false;
    size_t memsz = sizeof(MineSweeperTile) * MINESWEEPER_BOARD_MAX_TILES;

    do {
        setup_board(mine_sweeper_game_screen);

        uint16_t num_mines = 1;

        uint16_t board_width = 16; //default values
        uint16_t board_height = 7; //default values

        with_view_model(
            mine_sweeper_game_screen->view,
            MineSweeperGameScreenModel * model,
            {
                num_mines = model->mines_left;
                board_width = model->board_width;
                board_height = model->board_height;

                memset(board_t, 0, memsz);
                memcpy(
                    board_t, model->board, sizeof(MineSweeperTile) * (board_width * board_height));
            },
            true);

        is_valid_board = check_board_with_verifier(board_t, board_width, board_height, num_mines);

    } while(ensure_solvable && !is_valid_board);

    return mine_sweeper_game_screen;
}

void mine_sweeper_game_screen_free(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    // Dealloc strings in model
    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        { furi_string_free(model->info_str); },
        false);

    // Free view and any dynamically allocated members in main struct
    view_free(instance->view);
    free(instance);
}

// This function should be called whenever you want to reset the game state
// This should NOT be called in the on_exit in the game scene
void mine_sweeper_game_screen_reset(
    MineSweeperGameScreen* instance,
    uint8_t width,
    uint8_t height,
    uint8_t difficulty,
    bool ensure_solvable) {
    furi_assert(instance);

    instance->input_callback = NULL;

    // Reset led
    mine_sweeper_led_reset(instance->context);

    // We need to initize board width and height before setup
    mine_sweeper_game_screen_set_board_information(
        instance, width, height, difficulty, ensure_solvable);

    mine_sweeper_game_screen_reset_clock(instance);

    // Here we are going to generate a valid map for the player
    bool is_valid_board = false;
    size_t memsz = sizeof(MineSweeperTile) * MINESWEEPER_BOARD_MAX_TILES;

    do {
        setup_board(instance);

        uint16_t num_mines = 1;

        uint16_t board_width = 16; //default values
        uint16_t board_height = 7; //default values

        with_view_model(
            instance->view,
            MineSweeperGameScreenModel * model,
            {
                num_mines = model->mines_left;
                board_width = model->board_width;
                board_height = model->board_height;

                memset(board_t, 0, memsz);
                memcpy(
                    board_t, model->board, sizeof(MineSweeperTile) * (board_width * board_height));
            },
            true);

        is_valid_board = check_board_with_verifier(board_t, board_width, board_height, num_mines);

    } while(ensure_solvable && !is_valid_board);
}

// This function should be called when you want to reset the game clock
// Already called in reset and alloc function for game, but can be called from
// other scenes that need it like a start scene that plays after alloc
void mine_sweeper_game_screen_reset_clock(MineSweeperGameScreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        MineSweeperGameScreenModel * model,
        { model->start_tick = furi_get_tick(); },
        true);
}

View* mine_sweeper_game_screen_get_view(MineSweeperGameScreen* instance) {
    furi_assert(instance);
    return instance->view;
}

void mine_sweeper_game_screen_set_context(MineSweeperGameScreen* instance, void* context) {
    furi_assert(instance);
    instance->context = context;
}
