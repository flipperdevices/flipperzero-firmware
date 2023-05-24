#include <furi.h>
#include <stdint.h>

typedef enum {
    TttMultiGamePlayerNone = 0,
    TttMultiGamePlayerX = 1,
    TttMultiGamePlayerO = 2,
} TttMultiGamePlayer;

typedef enum {
    TttMultiGameStateTurnO,
    TttMultiGameStateTurnX,
    TttMultiGameStateFinished
} TttMultiGameState;

typedef enum {
    TttMultiGameResultXWin,
    TttMultiGameResultOWin,
    TttMultiGameResultDraw,
    TttMultiGameResultNone
} TttMultiGameResult;

typedef struct TttMultiGame TttMultiGame;

typedef struct {
    TttMultiGamePlayer player;
    uint8_t x;
    uint8_t y;
} TttMultiGameMove;

TttMultiGameMove* ttt_multi_game_move_alloc();

void ttt_multi_game_move_free(TttMultiGameMove* move);

void ttt_multi_game_move_copy(TttMultiGameMove* dst, const TttMultiGameMove* src);

TttMultiGame* ttt_multi_game_alloc();

void ttt_multi_game_free(TttMultiGame* game);

void ttt_multi_game_reset(TttMultiGame* game);

void ttt_multi_game_swap_player(TttMultiGame* game);

void ttt_multi_game_make_move(TttMultiGame* game, TttMultiGameMove* move);

bool ttt_multi_game_is_move_valid(TttMultiGame* game, TttMultiGameMove* move);

TttMultiGameState ttt_multi_game_get_state(TttMultiGame* game);

TttMultiGameResult ttt_multi_game_get_result(TttMultiGame* game);

TttMultiGamePlayer ttt_multi_game_player_at(TttMultiGame* game, uint8_t x, uint8_t y);

TttMultiGamePlayer ttt_multi_game_current_player(TttMultiGame* game);