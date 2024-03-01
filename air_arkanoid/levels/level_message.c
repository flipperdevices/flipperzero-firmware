#include <gui/elements.h>
#include "level_message.h"

static void message_update(Entity* self, GameManager* manager, void* context) {
    UNUSED(self);
    UNUSED(context);
    InputState input = game_manager_input_get(manager);
    if(input.pressed & GameKeyOk || input.pressed & GameKeyBack) {
        GameContext* ctx = game_manager_game_context_get(manager);
        game_manager_next_level_set(manager, ctx->levels.menu);
    }
}

static void message_render(Entity* self, GameManager* manager, Canvas* canvas, void* context) {
    UNUSED(self);
    UNUSED(manager);
    UNUSED(context);
    GameContext* game_ctx = game_manager_game_context_get(manager);
    LevelMessageContext* ctx = level_context_get(game_ctx->levels.message);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 64, 30, AlignCenter, AlignTop, furi_string_get_cstr(ctx->message));
    canvas_set_font(canvas, FontSecondary);
}

static const EntityDescription message_desc = {
    .start = NULL,
    .stop = NULL,
    .update = message_update,
    .render = message_render,
    .collision = NULL,
    .event = NULL,
    .context_size = 0,
};

static void level_alloc(Level* level, GameManager* manager, void* ctx) {
    UNUSED(level);
    UNUSED(manager);
    LevelMessageContext* context = ctx;
    context->message = furi_string_alloc();
    level_add_entity(level, &message_desc);
}

static void level_free(Level* level, GameManager* manager, void* context) {
    UNUSED(level);
    UNUSED(manager);
    LevelMessageContext* ctx = context;
    furi_string_free(ctx->message);
}

const LevelBehaviour level_message = {
    .alloc = level_alloc,
    .free = level_free,
    .start = NULL,
    .stop = NULL,
    .context_size = sizeof(LevelMessageContext),
};