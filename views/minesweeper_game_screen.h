/**
 * @file minesweeper_game_screen_screen.h
 * GUI: Start Screen view module API
 */

#ifndef MINEWEEPER_GAME_SCREEN_H
#define MINEWEEPER_GAME_SCREEN_H

#include <gui/view.h>

#define MINESWEEPER_BOARD_HEIGHT 7
#define MINESWEEPER_BOARD_WIDTH 16
#define MINESWEEPER_BOARD_TILE_COUNT (MINESWEEPER_BOARD_HEIGHT * MINESWEEPER_BOARD_WIDTH)
#define MINESWEEPER_STARTING_MINES 20

#ifdef __cplusplus
extern "C" {
#endif

/** MineSweeperGameScreen anonymous structure */
typedef struct MineSweeperGameScreen MineSweeperGameScreen;

/** MineSweeperGameScreen callback types
 * @warning     comes from GUI thread
 */
typedef void (*MineSweeperGameScreenTimerCallback)(void* context);
typedef bool (*MineSweeperGameScreenInputCallback)(InputEvent* event, void* context);

/** Allocate and initalize
 *
 * This view is used as the game screen of an application.
 *
 * @return      MineSweeperGameScreen view instance 
 */
MineSweeperGameScreen* mine_sweeper_game_screen_alloc();

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
        MineSweeperGameScreenInputCallback callback);

/** Set MineSweeperGameScreen context 
 *
 * @param       instance MineSweeperGameScreen instance
 * @param       context context pointer, will be passed to callback 
 */
void mine_sweeper_game_screen_set_context(MineSweeperGameScreen* instance, void* context);


/** Return true/false if tile is a mine 
 *
 * @param       instance MineSweeperGameScreen instance
 * @param       x       row in board grid
 * @param       y       column in board grid
 */
bool mine_sweeper_is_tile_mine(MineSweeperGameScreen* instance, uint8_t x, uint8_t y);

#ifdef __cplusplus
}
#endif

#endif
