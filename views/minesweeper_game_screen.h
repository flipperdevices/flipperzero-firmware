/**
 * @file minesweeper_game_screen_screen.h
 * GUI: Start Screen view module API
 */

#ifndef MINEWEEPER_GAME_SCREEN_H
#define MINEWEEPER_GAME_SCREEN_H

#include <gui/view.h>

// MAX TILES ALLOWED
#define MINESWEEPER_BOARD_MAX_TILES  (1<<10)

// These defines represent how many tiles
// can be visually representen on the screen 
#define MINESWEEPER_SCREEN_TILE_HEIGHT 8
#define MINESWEEPER_SCREEN_TILE_WIDTH 16

// These defines currently represent the actual
// width and hight in tiles for the board.
// This should be replaced with uint8_t members in the 
// view model that can be changed by the user
#define MINESWEEPER_BOARD_TILE_HEIGHT 8 
#define MINESWEEPER_BOARD_TILE_WIDTH 128 

#define MINESWEEPER_BOARD_TILE_COUNT (MINESWEEPER_BOARD_TILE_HEIGHT * MINESWEEPER_BOARD_TILE_WIDTH)
#define MINESWEEPER_STARTING_MINES (MINESWEEPER_BOARD_TILE_COUNT * .20f)

#ifdef __cplusplus
extern "C" {
#endif

/** MineSweeperGameScreen anonymous structure */
typedef struct MineSweeperGameScreen MineSweeperGameScreen;

/** StartScreen callback types
 * @warning     comes from GUI thread
 */
typedef bool (*GameScreenInputCallback)(InputEvent* event, void* context);

/** Allocate and initalize
 *
 * This view is used as the game screen of an application.
 *
 * @return      MineSweeperGameScreen view instance 
 */
MineSweeperGameScreen* mine_sweeper_game_screen_alloc(uint8_t width, uint8_t height);

/** Deinitialize and free Start Screen view
 *
 * @param       instsance MineSweeperGameScreen instance
 */
void mine_sweeper_game_screen_free(MineSweeperGameScreen* instance);

/** Reset MineSweeperGameScreen
 *
 * @param       instance MineSweeperGameScreen instance
 */
void mine_sweeper_game_screen_reset(MineSweeperGameScreen* instance);

/** Get MineSweeperGameScreen view
 *
 * @param       instance MineSweeperGameScreen instance
 *
 * @return      view instance that can be used for embedding
 */
View* mine_sweeper_game_screen_get_view(MineSweeperGameScreen* instance);

/** Set MineSweeperGameScreen input callback 
 *
 * @param       instance MineSweeperGameScreen instance
 * @param       callback MineSweeperGameScreenInputCallback callback
 */
void mine_sweeper_game_screen_set_input_callback(
        MineSweeperGameScreen* instance,
        GameScreenInputCallback callback);

/** Set MineSweeperGameScreen context 
 *
 * @param       instance MineSweeperGameScreen instance
 * @param       context context pointer, will be passed to callback 
 */
void mine_sweeper_game_screen_set_context(MineSweeperGameScreen* instance, void* context);

/**
 * ADD LATER
 */
void mine_sweeper_game_screen_set_board_dimensions(MineSweeperGameScreen* instance, uint8_t width, uint8_t height);

/** Return true/false if tile is a mine 
 *
 * @param       instance MineSweeperGameScreen instance
 * @param       x       row in board grid
 * @param       y       column in board grid
 */
bool mine_sweeper_is_tile_mine(MineSweeperGameScreen* instance, uint16_t x, uint16_t y);


#define inverted_canvas_white_to_black(canvas, code)      \
    {                                           \
        canvas_set_color(canvas, ColorWhite);   \
        {code};                                 \
        canvas_set_color(canvas, ColorBlack);   \
    }                           

#ifdef __cplusplus
}
#endif

#endif
