/**
 * @file minesweeper_game_screen_screen.h
 * GUI: Start Screen view module API
 */

#ifndef MINEWEEPER_GAME_SCREEN_H
#define MINEWEEPER_GAME_SCREEN_H

#include <gui/view.h>

#include "minesweeper_game_screen_i.h"
#include "../helpers/mine_sweeper_haptic.h"
#include "../helpers/mine_sweeper_led.h"
#include "../helpers/mine_sweeper_speaker.h"

// MAX TILES ALLOWED
#define MINESWEEPER_BOARD_MAX_TILES (1 << 10)

// These defines represent how many tiles
// can be visually representen on the screen
// due to icon sizes
#define MINESWEEPER_SCREEN_TILE_HEIGHT 7
#define MINESWEEPER_SCREEN_TILE_WIDTH 16

#define MS_DEBUG_TAG "Mine Sweeper Module/View"

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
 * @param       width       uint8_t width for board
 * @param       height      uint8_t height for board
 * @param       difficulty  uint8_t difficulty for board
 *
 * @return      MineSweeperGameScreen* instance 
 */
MineSweeperGameScreen* mine_sweeper_game_screen_alloc(
    uint8_t width,
    uint8_t height,
    uint8_t difficulty,
    bool ensure_solvable);

/** Deinitialize and free Start Screen view
 *
 * @param       instsance   MineSweeperGameScreen instance
 */
void mine_sweeper_game_screen_free(MineSweeperGameScreen* instance);

/** Reset MineSweeperGameScreen
 *
 * @param       instance    MineSweeperGameScreen* instance
 * @param       width       uint8_t width for board
 * @param       height      uint8_t height for board
 * @param       difficulty  uint8_t difficulty for board
 */
void mine_sweeper_game_screen_reset(
    MineSweeperGameScreen* instance,
    uint8_t width,
    uint8_t height,
    uint8_t difficulty,
    bool ensure_solvable);

/** Reset MineSweeperGameScreen clock 
 *
 * @param       instance    MineSweeperGameScreen* instance
 */
void mine_sweeper_game_screen_reset_clock(MineSweeperGameScreen* instance);

/** Get MineSweeperGameScreen view
 *
 * @param       instance    MineSweeperGameScreen instance
 *
 * @return      View* instance that can be used for embedding
 */
View* mine_sweeper_game_screen_get_view(MineSweeperGameScreen* instance);

/** Set MineSweeperGameScreen context 
 *
 * @param       instance    MineSweeperGameScreen* instance
 * @param       context     void* context for MineSweeperGameScreen instance 
 */
void mine_sweeper_game_screen_set_context(MineSweeperGameScreen* instance, void* context);

#define inverted_canvas_white_to_black(canvas, code) \
    {                                                \
        canvas_set_color(canvas, ColorWhite);        \
        {code};                                      \
        canvas_set_color(canvas, ColorBlack);        \
    }

#ifdef __cplusplus
}
#endif

#endif
