#include "ui.h"
#include "card.h"
#include <math.h>
#include "util.h"

const char MoneyMul[4] = {
        'K', 'B', 'T', 'S'
};

void draw_player_scene(Canvas *const canvas, const GameState *game_state) {
    int max_card = game_state->player_card_count;

    if (max_card > 0)
        drawPlayerDeck((game_state->player_cards), max_card, canvas);

    drawCardBackAt(13, 5, canvas);

    max_card = game_state->dealer_card_count;
    if (max_card > 1) {
        drawCardAt(2, 2, game_state->dealer_cards[1].pip, game_state->dealer_cards[1].character, Normal,
                   canvas);
    }
}

void draw_dealer_scene(Canvas *const canvas, const GameState *game_state) {
    uint8_t max_card = game_state->dealer_card_count;
    drawPlayerDeck((game_state->dealer_cards), max_card, canvas);
}

void draw_card_animation(const GameState *game_state, Canvas *const canvas) {
    float t = (float) (furi_get_tick() - game_state->animationStart) / (ANIMATION_TIME - ANIMATION_END_MARGIN);
    t *= 2;
    Card animatingCard = game_state->deck.cards[game_state->deck.index];
    if (t > 1) {
        int cardY = round(lerp(-CARD_HEIGHT, 10, 1));
        drawCardAt(64 - CARD_HALF_WIDHT, cardY, animatingCard.pip,
                   animatingCard.character, Normal, canvas);
    } else {
        int cardY = round(lerp(-CARD_HEIGHT, 10, t));
        drawCardAt(64 - CARD_HALF_WIDHT, cardY, animatingCard.pip,
                   animatingCard.character, Normal, canvas);
//        drawCardBackAt(64 - CARD_HALF_WIDHT, cardY, canvas);
    }
}

void popupFrame(Canvas *const canvas) {
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 32, 15, 66, 13);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, 32, 15, 66, 13);
    canvas_set_font(canvas, FontSecondary);
}

void draw_message_scene(Canvas *const canvas, const GameState *game_state) {
    switch (game_state->state) {
        case GameStateStart:
            canvas_set_font(canvas, FontPrimary);
            elements_multiline_text_aligned(canvas, 64, 5, AlignCenter, AlignTop, "Blackjack");
            canvas_set_font(canvas, FontSecondary);
            elements_multiline_text_aligned(canvas, 64, 24, AlignCenter, AlignTop, "Made by Doofy");
            elements_multiline_text_aligned(canvas, 64, 38, AlignCenter, AlignTop, "Press center button\nto start");
            break;
        case GameStateGameOver:
            canvas_set_font(canvas, FontPrimary);
            elements_multiline_text_aligned(canvas, 64, 5, AlignCenter, AlignTop, "Game Over");
            canvas_set_font(canvas, FontSecondary);
            elements_multiline_text_aligned(canvas, 64, 24, AlignCenter, AlignTop, "Press center button\nto start");
            break;
        default:
            break;
    }
}

void draw_play_menu(Canvas *const canvas, const GameState *game_state) {
    const char *menus[3] = {"Double", "Hit", "Stay"};
    for (uint8_t m = 0; m < 3; m++) {
        if (m == 0 && (game_state->doubled || game_state->player_score < ROUND_PRICE)) continue;
        int y = m * 13 + 25;
        canvas_set_color(canvas, ColorBlack);

        if (game_state->selectedMenu == m) {
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_box(canvas, 1, y, 31, 12);
        } else {
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_box(canvas, 1, y, 31, 12);
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_frame(canvas, 1, y, 31, 12);
        }

        if (game_state->selectedMenu == m)
            canvas_set_color(canvas, ColorWhite);
        else
            canvas_set_color(canvas, ColorBlack);
        canvas_draw_str_aligned(canvas, 16, y + 6, AlignCenter, AlignCenter, menus[m]);
    }
}

void draw_score(Canvas *const canvas, bool top, uint8_t amount) {
    char drawChar[20];
    snprintf(drawChar, sizeof(drawChar), "Player score: %i", amount);
    if (top)
        canvas_draw_str_aligned(canvas, 64, 2, AlignCenter, AlignTop, drawChar);
    else
        canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, drawChar);
}

void draw_money(Canvas *const canvas, uint32_t score) {
    canvas_set_font(canvas, FontSecondary);
    char drawChar[10];
    uint32_t currAmount = score;
    if (currAmount < 1000) {
        snprintf(drawChar, sizeof(drawChar), "$%lu", currAmount);
    } else {
        char c = 'K';
        for (uint8_t i = 0; i < 4; i++) {
            currAmount = currAmount / 1000;
            if (currAmount < 1000) {
                c = MoneyMul[i];
                break;
            }
        }

        snprintf(drawChar, sizeof(drawChar), "$%lu %c", currAmount, c);
    }
    canvas_draw_str_aligned(canvas, 126, 2, AlignRight, AlignTop, drawChar);
}