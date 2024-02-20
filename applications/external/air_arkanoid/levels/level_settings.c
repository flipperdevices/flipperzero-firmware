#include "level_settings.h"

/**** Menu ****/

typedef enum {
    Sound = 0,
    ShowFPS,
    Back,
} MenuOption;

typedef struct {
    int selected;
} MenuContext;

static void menu_update(Entity* entity, GameManager* manager, void* context) {
    UNUSED(entity);
    MenuContext* menu_context = context;
    GameContext* game_context = game_manager_game_context_get(manager);

    InputState input = game_manager_input_get(manager);

    if(input.pressed & GameKeyUp || input.pressed & GameKeyDown || input.pressed & GameKeyOk) {
        game_sound_play(game_context, &sequence_sound_menu);
    }

    if(input.pressed & GameKeyBack) {
        game_manager_next_level_set(manager, game_context->levels.menu);
    }

    if(input.pressed & GameKeyUp) {
        menu_context->selected--;
        if(menu_context->selected < Sound) {
            menu_context->selected = Back;
        }
    }

    if(input.pressed & GameKeyDown) {
        menu_context->selected++;
        if(menu_context->selected > Back) {
            menu_context->selected = Sound;
        }
    }

    if(input.pressed & GameKeyOk) {
        switch(menu_context->selected) {
        case Sound:
            game_switch_sound(game_context);
            break;
        case ShowFPS:
            game_switch_show_fps(game_context);
            break;
        case Back:
            game_manager_next_level_set(manager, game_context->levels.menu);
            break;

        default:
            break;
        }
    }

    if(input.pressed & GameKeyRight || input.pressed & GameKeyLeft) {
        switch(menu_context->selected) {
        case Sound:
            game_switch_sound(game_context);
            break;
        case ShowFPS:
            game_switch_show_fps(game_context);
            break;

        default:
            break;
        }
    }
}

static void menu_render(Entity* entity, GameManager* manager, Canvas* canvas, void* context) {
    UNUSED(entity);
    MenuContext* menu_context = context;
    GameContext* game_context = game_manager_game_context_get(manager);
    FuriString* line = furi_string_alloc_set("Sound: ");

    if(menu_context->selected == Sound) {
        furi_string_set(line, ">Sound: ");
    }

    if(game_context->settings.sound) {
        furi_string_cat(line, "On");
    } else {
        furi_string_cat(line, "Off");
    }

    canvas_draw_str_aligned(
        canvas, 64 + 3, 18, AlignLeft, AlignCenter, furi_string_get_cstr(line));

    furi_string_set(line, "FPS: ");
    if(menu_context->selected == ShowFPS) {
        furi_string_set(line, ">FPS: ");
    }

    if(game_context->settings.show_fps) {
        furi_string_cat(line, "On");
    } else {
        furi_string_cat(line, "Off");
    }

    canvas_draw_str_aligned(
        canvas, 64 + 3, 33, AlignLeft, AlignCenter, furi_string_get_cstr(line));

    furi_string_set(line, "Back");

    if(menu_context->selected == Back) {
        furi_string_set(line, ">Back");
    }

    canvas_draw_str_aligned(
        canvas, 64 + 3, 48, AlignLeft, AlignCenter, furi_string_get_cstr(line));

    furi_string_free(line);
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

/**** IMU Debug ****/

typedef struct {
    float pitch;
    float roll;
    float yaw;
    bool imu_present;
} IMUDebugContext;

static void imu_debug_start(Entity* self, GameManager* manager, void* ctx) {
    UNUSED(self);
    IMUDebugContext* context = ctx;
    context->pitch = 0;
    context->roll = 0;
    context->yaw = 0;
    GameContext* game_context = game_manager_game_context_get(manager);
    context->imu_present = game_context->imu_present;
}

static void imu_debug_update(Entity* self, GameManager* manager, void* ctx) {
    UNUSED(self);
    IMUDebugContext* context = (IMUDebugContext*)ctx;
    GameContext* game_context = game_manager_game_context_get(manager);

    if(game_context->imu_present) {
        context->pitch = imu_pitch_get(game_context->imu);
        context->roll = imu_roll_get(game_context->imu);
        context->yaw = imu_yaw_get(game_context->imu);
    }
}

static void imu_debug_render(Entity* self, GameManager* manager, Canvas* canvas, void* context) {
    UNUSED(self);
    UNUSED(manager);

    Vector pos = {32, 32};
    const float radius = 30;
    const float max_angle = 45;
    const float bubble_radius = 3;

    canvas_draw_circle(canvas, pos.x, pos.y, radius);

    IMUDebugContext* imu_debug_context = context;
    if(imu_debug_context->imu_present) {
        const float pitch = -CLAMP(imu_debug_context->pitch, max_angle, -max_angle);
        const float roll = -CLAMP(imu_debug_context->roll, max_angle, -max_angle);
        const float max_bubble_len = radius - bubble_radius - 2;

        Vector ball = {
            max_bubble_len * (pitch / max_angle),
            max_bubble_len * (roll / max_angle),
        };

        float bubble_len = sqrtf(ball.x * ball.x + ball.y * ball.y);
        if(bubble_len > max_bubble_len) {
            ball.x = ball.x * max_bubble_len / bubble_len;
            ball.y = ball.y * max_bubble_len / bubble_len;
        }

        ball = vector_add(pos, ball);

        canvas_draw_disc(canvas, ball.x, ball.y, bubble_radius);
    } else {
        canvas_draw_str_aligned(canvas, pos.x, pos.y + 1, AlignCenter, AlignCenter, "No IMU");
    }
}

static const EntityDescription imu_debug_desc = {
    .start = imu_debug_start,
    .stop = NULL,
    .update = imu_debug_update,
    .render = imu_debug_render,
    .collision = NULL,
    .event = NULL,
    .context_size = sizeof(IMUDebugContext),
};

/**** Level  ****/

static void level_settings_alloc(Level* level, GameManager* manager, void* ctx) {
    UNUSED(ctx);
    UNUSED(manager);
    level_add_entity(level, &imu_debug_desc);
    level_add_entity(level, &menu_desc);
}

const LevelBehaviour level_settings = {
    .alloc = level_settings_alloc,
    .free = NULL,
    .start = NULL,
    .stop = NULL,
    .context_size = 0,
};