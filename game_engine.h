#pragma once
#include <stdbool.h>
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

typedef struct RunningGameEngine RunningGameEngine;

typedef void (*GameEngineStartCallback)(GameEngine* engine, void* context);

typedef void (*GameEngineStopCallback)(GameEngine* engine, void* context);

typedef void (*GameEngineFrameCallback)(
    RunningGameEngine* engine,
    Canvas* canvas,
    InputState input,
    void* context);

typedef struct {
    float fps; // target fps
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

/** Stop the running Game Engine, will not block execution
 * @param engine RunningGameEngine instance
 */
void running_game_engine_stop(RunningGameEngine* engine);

/** Get delta time between current and previous frame
 * @param engine RunningGameEngine instance
 * @return float  delta time in seconds
 */
float running_game_engine_get_delta_time(RunningGameEngine* engine);

/** Get delta frames between current and previous frame
 * @param engine RunningGameEngine instance
 * @return float  delta frames
 */
float running_game_engine_get_delta_frames(RunningGameEngine* engine);

#ifdef __cplusplus
}
#endif