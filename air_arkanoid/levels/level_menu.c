#include "level_menu.h"
#include "../game.h"

typedef struct {
    Sprite* sprite;
    Vector pos_start;
    Vector pos_end;
    float duration;
    float time;
} MovingSpriteContext;

/***** Moving Sprite *****/

static void moving_sprite_update(Entity* entity, GameManager* manager, void* context) {
    UNUSED(manager);
    MovingSpriteContext* sprite_context = context;

    // lerp position between start and end for duration
    if(sprite_context->time < sprite_context->duration) {
        Vector dir = vector_sub(sprite_context->pos_end, sprite_context->pos_start);
        Vector len = vector_mulf(dir, sprite_context->time / sprite_context->duration);
        Vector pos = vector_add(sprite_context->pos_start, len);

        entity_pos_set(entity, pos);
        sprite_context->time += 1.0f;
    } else {
        entity_pos_set(entity, sprite_context->pos_end);
    }
}

static void
    moving_sprite_render(Entity* entity, GameManager* manager, Canvas* canvas, void* context) {
    UNUSED(manager);
    MovingSpriteContext* sprite_context = context;

    if(sprite_context->sprite) {
        Vector pos = entity_pos_get(entity);
        canvas_draw_sprite(canvas, sprite_context->sprite, pos.x, pos.y);
    }
}

static void moving_sprite_init(
    Entity* entity,
    GameManager* manager,
    Vector start,
    Vector end,
    const char* sprite_name) {
    MovingSpriteContext* sprite_context = entity_context_get(entity);
    sprite_context->pos_start = start;
    sprite_context->pos_end = end;
    sprite_context->duration = 30.0f;
    sprite_context->time = 0;
    sprite_context->sprite = game_manager_sprite_load(manager, sprite_name);
}

static void moving_sprite_reset(Entity* entity) {
    MovingSpriteContext* sprite_context = entity_context_get(entity);
    sprite_context->time = 0;
}

static const EntityDescription moving_sprite_desc = {
    .start = NULL,
    .stop = NULL,
    .update = moving_sprite_update,
    .render = moving_sprite_render,
    .collision = NULL,
    .event = NULL,
    .context_size = sizeof(MovingSpriteContext),
};

/***** Menu *****/

typedef struct {
    int selected;
} MenuContext;

static void menu_update(Entity* entity, GameManager* manager, void* context) {
    UNUSED(entity);
    MenuContext* menu_context = context;
    GameContext* game_context = game_manager_game_context_get(manager);

    InputState input = game_manager_input_get(manager);
    if(input.pressed & GameKeyBack) {
        game_manager_game_stop(manager);
    }

    if(input.pressed & GameKeyUp) {
        menu_context->selected--;
        if(menu_context->selected < 0) {
            menu_context->selected = 2;
        }
    }

    if(input.pressed & GameKeyDown) {
        menu_context->selected++;
        if(menu_context->selected > 2) {
            menu_context->selected = 0;
        }
    }

    if(input.pressed & GameKeyUp || input.pressed & GameKeyDown || input.pressed & GameKeyOk) {
        game_sound_play(game_context, &sequence_sound_menu);
    }

    if(input.pressed & GameKeyOk) {
        switch(menu_context->selected) {
        case 0:
            game_manager_next_level_set(manager, game_context->levels.game);
            break;
        case 1:
            game_manager_next_level_set(manager, game_context->levels.settings);
            break;
        case 2:
            game_manager_game_stop(manager);
            break;

        default:
            break;
        }
    }
}

#include "../fonts/fonts.h"

static void menu_render(Entity* entity, GameManager* manager, Canvas* canvas, void* context) {
    UNUSED(entity);
    UNUSED(manager);
    MenuContext* menu_context = context;
    const char* line_1 = "Play";
    const char* line_2 = "Settings";
    const char* line_3 = "Exit";

    if(menu_context->selected == 0) {
        line_1 = ">Play";
    } else if(menu_context->selected == 1) {
        line_2 = ">Settings";
    } else if(menu_context->selected == 2) {
        line_3 = ">Exit";
    }

    canvas_draw_str_aligned(canvas, 64, 39, AlignCenter, AlignCenter, line_1);
    canvas_draw_str_aligned(canvas, 64, 49, AlignCenter, AlignCenter, line_2);
    canvas_draw_str_aligned(canvas, 64, 59, AlignCenter, AlignCenter, line_3);
}

static const EntityDescription menu_desc = {
    .start = NULL,
    .stop = NULL,
    .update = menu_update,
    .render = menu_render,
    .collision = NULL,
    .event = NULL,
    .context_size = sizeof(MenuContext),
};

/***** Level *****/

typedef struct {
    Entity* arkanoid;
    Entity* air;
} LevelMenuContext;

static void level_menu_alloc(Level* level, GameManager* manager, void* context) {
    LevelMenuContext* menu_context = context;

    const float start = 256; // 0, due to the canvas draw limitations

    menu_context->arkanoid = level_add_entity(level, &moving_sprite_desc);
    moving_sprite_init(
        menu_context->arkanoid,
        manager,
        (Vector){.x = start - 50, .y = start + 11},
        (Vector){.x = start + 7, .y = start + 11},
        "logo_arkanoid.fxbm");

    menu_context->air = level_add_entity(level, &moving_sprite_desc);
    moving_sprite_init(
        menu_context->air,
        manager,
        (Vector){.x = start + 20, .y = start - 27},
        (Vector){.x = start + 20, .y = start + 0},
        "logo_air.fxbm");

    level_add_entity(level, &menu_desc);
}

static void level_menu_start(Level* level, GameManager* manager, void* context) {
    UNUSED(level);
    UNUSED(manager);
    LevelMenuContext* menu_context = context;
    moving_sprite_reset(menu_context->arkanoid);
    moving_sprite_reset(menu_context->air);
}

const LevelBehaviour level_menu = {
    .alloc = level_menu_alloc,
    .free = NULL,
    .start = level_menu_start,
    .stop = NULL,
    .context_size = sizeof(LevelMenuContext),
};