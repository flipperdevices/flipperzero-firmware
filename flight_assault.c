#include <string.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

// Define screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Player properties
#define PLAYER_WIDTH 11
#define PLAYER_HEIGHT 15
static const uint8_t player_bitmap[] = {0x07,0x00,0x09,0x00,0x11,0x00,0x21,0x00,0x41,0x00,0x81,0x00,0x02,0x03,0x04,0x04,0x02,0x03,0x81,0x00,0x41,0x00,0x21,0x00,0x11,0x00,0x09,0x00,0x07,0x00};
int player_x = 16; // Changed starting position
int player_y = SCREEN_HEIGHT / 2 - PLAYER_HEIGHT / 2;

// Ball properties
int x = SCREEN_WIDTH / 2;
int y = SCREEN_HEIGHT / 2;
int velocity_x = 0;
int velocity_y = 0;

// Enemy properties
#define MAX_ENEMIES 6 // Increased number of enemies
int enemy_count = MAX_ENEMIES;
int enemy_x[MAX_ENEMIES];
int enemy_y[MAX_ENEMIES];
int enemy_speed[MAX_ENEMIES]; // Removed velocity arrays

// Second enemy properties
int enemy2_x[MAX_ENEMIES];
int enemy2_y[MAX_ENEMIES];
int enemy2_speed[MAX_ENEMIES];
int enemy2_health[MAX_ENEMIES]; // Health for enemy2

// Bullet properties
#define MAX_BULLETS 5
int bullet_count = 0;
int bullet_x[MAX_BULLETS];
int bullet_y[MAX_BULLETS];
int bullet_velocity_x[MAX_BULLETS];
bool bullet_active[MAX_BULLETS];

// Score
int score = 0;
char score_str[8];

// Game states
typedef enum {
    PLAYING,
    GAME_OVER
} GameState;

GameState game_state = PLAYING;

// Enemy bitmap
static const uint8_t enemy_bitmap[] = {0x7c,0x00,0x82,0x00,0x29,0x01,0x01,0x01,0x01,0x01,0xff,0x01};

// Second enemy bitmap
static const uint8_t enemy2_bitmap[] = {0x70,0x48,0x66,0x33,0x66,0x48,0x70};

// Bullet bitmap
static const uint8_t bullet_bitmap[] = {0x07, 0x07, 0x07};

// Declare running variable globally
bool running = true;

static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);

    // Draw player
    canvas_set_bitmap_mode(canvas, true);
    canvas_draw_xbm(canvas, player_x, player_y, PLAYER_WIDTH, PLAYER_HEIGHT, player_bitmap);

    // Draw enemies
    for (int i = 0; i < enemy_count; ++i) {
        canvas_set_bitmap_mode(canvas, true);
        canvas_draw_xbm(canvas, enemy_x[i], enemy_y[i], 9, 6, enemy_bitmap);
        canvas_set_bitmap_mode(canvas, false);
    }

    // Draw second enemies
    for (int i = 0; i < enemy_count; ++i) {
        if (score >= 50) {
            canvas_set_bitmap_mode(canvas, true);
            canvas_draw_xbm(canvas, enemy2_x[i], enemy2_y[i], 7, 7, enemy2_bitmap);
            canvas_set_bitmap_mode(canvas, false);
        }
    }

    // Draw bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (bullet_active[i]) {
            // Adjust bullet position to center it
            int bullet_center_x = bullet_x[i] - 1; // Subtract 1 to center horizontally
            int bullet_center_y = bullet_y[i] - 1; // Subtract 1 to center vertically
            canvas_set_bitmap_mode(canvas, true);
            canvas_draw_xbm(canvas, bullet_center_x, bullet_center_y, 3, 3, bullet_bitmap);
            canvas_set_bitmap_mode(canvas, false);
        }
    }

    // Draw score
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 8, "Score:");
    itoa(score, score_str, 10);
    canvas_draw_str(canvas, 40, 8, score_str);

    // Draw game over message if necessary
    if (game_state == GAME_OVER) {
        canvas_draw_str(canvas, 30, 30, "Game Over");
    }
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);

    // Check if Back button is pressed
    if (input_event->type == InputTypePress && input_event->key == InputKeyBack) {
        game_state = GAME_OVER;
        running = false;
    }
}

void shoot() {
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (!bullet_active[i]) {
            bullet_x[i] = player_x + PLAYER_WIDTH / 2; // Spawn bullet at the center of the player
            bullet_y[i] = player_y + PLAYER_HEIGHT / 2;
            bullet_velocity_x[i] = 2; // Set bullet velocity
            bullet_active[i] = true;
            break;
        }
    }
}

int32_t flight_assault(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure viewport
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    // Register viewport in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;

    // Initialize enemies
    for (int i = 0; i < enemy_count; ++i) {
        enemy_x[i] = SCREEN_WIDTH;
        enemy_y[i] = rand() % SCREEN_HEIGHT;
        enemy_speed[i] = rand() % 3 + 1; // Increased speed to 3
    }

    // Initialize second enemies
    for (int i = 0; i < enemy_count; ++i) {
        enemy2_x[i] = SCREEN_WIDTH;
        enemy2_y[i] = rand() % SCREEN_HEIGHT;
        enemy2_speed[i] = rand() % 3 + 1; // Increased speed to 3
        enemy2_health[i] = 1; // Set health to 1
    }

    bool shot_this_frame = false;
    while (running) {
        if (furi_message_queue_get(event_queue, &event, 90) == FuriStatusOk) { // 10% faster
            if ((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
                switch (event.key) {
                    case InputKeyUp:
                        velocity_y = -1;
                        break;
                    case InputKeyDown:
                        velocity_y = 1;
                        break;
                    case InputKeyLeft:
                        velocity_x = -1;
                        break;
                    case InputKeyRight:
                        velocity_x = 1;
                        break;
                    case InputKeyOk:
                        if (!shot_this_frame) {
                            shoot(); // Shoot when OK button is pressed
                            shot_this_frame = true;
                        }
                        break;
                    default:
                        break;
                }
            }
            if (event.type == InputTypeRelease) {
                switch (event.key) {
                    case InputKeyUp:
                    case InputKeyDown:
                        velocity_y = 0;
                        break;
                    case InputKeyLeft:
                    case InputKeyRight:
                        velocity_x = 0;
                        break;
                    case InputKeyOk:
                        shot_this_frame = false;
                        break;
                    default:
                        break;
                }
            }
        }

        // Move the player
        player_x += velocity_x;
        player_y += velocity_y;

        // Move enemies and check collision with player
        for (int i = 0; i < enemy_count; ++i) {
            enemy_x[i] -= enemy_speed[i];

            // Check if enemy is outside the screen
            if (enemy_x[i] < -9) { // Check if the enemy has completely left the screen
                enemy_x[i] = SCREEN_WIDTH;
                enemy_y[i] = rand() % SCREEN_HEIGHT;
                enemy_speed[i] = rand() % 3 + 1; // Randomize speed again
            }

            // Check for collision with player
            if ((player_x + PLAYER_WIDTH >= enemy_x[i] && player_x <= enemy_x[i] + 9) &&
                (player_y + PLAYER_HEIGHT >= enemy_y[i] && player_y <= enemy_y[i] + 6)) {
                game_state = GAME_OVER;
                running = false;
            }
        }

        // Move second enemies and check collision with player
        for (int i = 0; i < enemy_count; ++i) {
            if (score >= 50) {
                enemy2_x[i] -= enemy2_speed[i];

                // Check if enemy2 is outside the screen
                if (enemy2_x[i] < -7) { // Check if the enemy2 has completely left the screen
                    enemy2_x[i] = SCREEN_WIDTH;
                    enemy2_y[i] = rand() % SCREEN_HEIGHT;
                    enemy2_speed[i] = rand() % 3 + 1; // Randomize speed again
                    enemy2_health[i] = 1; // Reset health
                }

                // Check for collision with player
                if ((player_x + PLAYER_WIDTH >= enemy2_x[i] && player_x <= enemy2_x[i] + 7) &&
                    (player_y + PLAYER_HEIGHT >= enemy2_y[i] && player_y <= enemy2_y[i] + 7)) {
                    game_state = GAME_OVER;
                    running = false;
                }
            }
        }

        // Move bullets and check collision with enemies
        for (int i = 0; i < MAX_BULLETS; ++i) {
            if (bullet_active[i]) {
                bullet_x[i] += bullet_velocity_x[i];

                // Check collision with enemies
                for (int j = 0; j < enemy_count; ++j) {
                    if ((bullet_x[i] >= enemy_x[j] && bullet_x[i] <= enemy_x[j] + 9) &&
                        (bullet_y[i] >= enemy_y[j] && bullet_y[i] <= enemy_y[j] + 6)) {
                        // Destroy enemy
                        enemy_x[j] = SCREEN_WIDTH;
                        enemy_y[j] = rand() % SCREEN_HEIGHT;
                        bullet_active[i] = false;
                        score++; // Increase score
                        break;
                    }
                }

                // Check collision with second enemies
                for (int j = 0; j < enemy_count; ++j) {
                    if ((bullet_x[i] >= enemy2_x[j] && bullet_x[i] <= enemy2_x[j] + 7) &&
                        (bullet_y[i] >= enemy2_y[j] && bullet_y[i] <= enemy2_y[j] + 7)) {
                        // Decrease health of enemy2
                        enemy2_health[j]--;
                        if (enemy2_health[j] <= 0) {
                            // Destroy enemy2
                            enemy2_x[j] = SCREEN_WIDTH;
                            enemy2_y[j] = rand() % SCREEN_HEIGHT;
                            bullet_active[i] = false;
                            score++; // Increase score
                        }
                        break;
                    }
                }

                // Check if bullet is outside the screen
                if (bullet_x[i] >= SCREEN_WIDTH) {
                    bullet_active[i] = false;
                }
            }
        }

        // Check if player is outside the screen
        if (player_x < 0) player_x = 0;
        if (player_x > SCREEN_WIDTH - PLAYER_WIDTH) player_x = SCREEN_WIDTH - PLAYER_WIDTH;
        if (player_y < 0) player_y = 0;
        if (player_y > SCREEN_HEIGHT - PLAYER_HEIGHT) player_y = SCREEN_HEIGHT - PLAYER_HEIGHT;

        // Update viewport
        view_port_update(view_port);
    }

    // Cleanup
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}

