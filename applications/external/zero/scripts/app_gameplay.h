#include <stdbool.h>

typedef enum PlayerType { PlayerType_Human, PlayerType_AiRandom, PlayerType_COUNT } PlayerType;

typedef struct AppGameplayState AppGameplayState;
typedef struct GameState GameState;
typedef enum PlayerTurn PlayerTurn;

AppGameplayState* gameplay_alloc();
void gameplay_free(AppGameplayState* gameplay);
void gameplay_reset(AppGameplayState* gameplay);

GameState* gameplay_get_game(AppGameplayState* gameplay);

int gameplay_selection_get_hand_index(AppGameplayState* gameplay);
int gameplay_get_selected_card(AppGameplayState* gameplay);
void gameplay_selection_delta(AppGameplayState* gameplay, int delta);

void gameplay_selection_handle_delta(AppGameplayState* gameplay, int dx, int dy);
bool gameplay_selection_perform_current(AppGameplayState* gameplay);

PlayerType gameplay_get_player_type(AppGameplayState* gameplay, PlayerTurn playerIndex);
PlayerType gameplay_get_next_player_type(AppGameplayState* gameplay);
void gameplay_set_player_type(
    AppGameplayState* gameplay,
    PlayerTurn playerIndex,
    PlayerType playerType);
int gameplay_get_last_action_at(AppGameplayState* gameplay);
void gameplay_set_last_action_at(AppGameplayState* gameplay, int lastActionAt);
