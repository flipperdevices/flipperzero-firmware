#pragma once

#include <gui/view.h>
#include <gui/elements.h>
#include "../ttt_multi_custom_event.h"
#include "../helpers/ttt_multi_game.h"

typedef void (*TttMultiGameViewCallback)(void* context, TttMultiCustomEvent event);
typedef struct TttMultiGameView TttMultiGameView;

TttMultiGameView* ttt_multi_game_view_alloc();

void ttt_multi_game_view_free(TttMultiGameView* game_view);

View* ttt_multi_game_get_view(TttMultiGameView* game_view);

void ttt_multi_game_view_move(TttMultiGameView* game_view, TttMultiGameMove* move);

void ttt_multi_game_view_set_callback(
    TttMultiGameView* game_view,
    TttMultiGameViewCallback callback,
    void* context);

void ttt_multi_game_view_set_remote_play(TttMultiGameView* game_view, TttMultiGamePlayer player);

void ttt_multi_game_view_set_local_play(TttMultiGameView* game_view);

void ttt_multi_game_view_get_last_move(TttMultiGameView* game_view, TttMultiGameMove* move);

TttMultiGameResult ttt_multi_game_view_get_result(TttMultiGameView* game_view);

void ttt_multi_game_view_reset(TttMultiGameView* game_view);