#pragma once
#include <stdbool.h>
#include "canvas.h"
#include <gui/canvas.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GameKeyUp = 1 << 0,
    GameKeyDown = 1 << 1,
    GameKeyRight = 1 << 2,
    GameKeyLeft = 1 << 3,
    GameKeyOk = 1 << 4,
    GameKeyBack = 1 << 5,
} GameKey;

typedef struct {
    uint32_t held; // mask of GameKey held in current frame
    uint32_t pressed; // mask of GameKey pressed in current frame
    uint32_t released; // mask of GameKey released in current frame
} InputState;

typedef struct GameEngine GameEngine;

typedef void (*GameEngineStartCallback)(GameEngine* engine, void* context);

typedef void (*GameEngineStopCallback)(GameEngine* engine, void* context);

typedef void (
    *GameEngineFrameCallback)(GameEngine* engine, Canvas* canvas, InputState input, void* context);

typedef struct {
    float target_fps; // target fps
    bool show_fps; // show fps counter
    bool always_backlight; // keep backlight on
    GameEngineStartCallback start_callback; // called when engine starts
    GameEngineFrameCallback frame_callback; // frame callback, called at target fps
    GameEngineStopCallback stop_callback; // called when engine stops
    void* context; // user context passed to callback
} GameEngineSettings;

/** Default settings initializer */
GameEngineSettings game_engine_settings_init(void);

/** Game Engine allocator
 * @param settings engine settings
 * @return GameEngine*  GameEngine instance
 */
GameEngine* game_engine_alloc(GameEngineSettings settings);

/** Run the Game Engine. Blocks until game_engine_stop() is called.
 * @param engine GameEngine instance
 */
void game_engine_run(GameEngine* engine);

/** Free the Game Engine
 * @param engine GameEngine instance
 */
void game_engine_free(GameEngine* engine);

/** Stop the Game Engine, will not block execution
 * @param engine GameEngine instance
 */
void game_engine_stop(GameEngine* engine);

/** Get delta time between current and previous frame
 * @param engine GameEngine instance
 * @return float  delta time in seconds
 */
float game_engine_get_delta_time(GameEngine* engine);

/** Get delta frames between current and previous frame
 * @param engine GameEngine instance
 * @return float  delta frames
 */
float game_engine_get_delta_frames(GameEngine* engine);

/** Enable/disable show fps counter
 * @param engine GameEngine instance
 * @param show_fps show fps counter
 */
void game_engine_show_fps_set(GameEngine* engine, bool show_fps);

#ifdef __cplusplus
}
#endif