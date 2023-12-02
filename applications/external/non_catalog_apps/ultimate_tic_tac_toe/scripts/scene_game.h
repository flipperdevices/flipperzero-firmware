#include <gui/gui.h>

void game_tick_callback(void* context);
void game_handle_input(InputKey key, InputType type, void* context);
void game_render_callback(Canvas* const canvas, void* context);
void game_transition_callback(int from, int to, void* context);