#include <math.h>
#include "card.h"
#include "util.h"

//region CardDesign
bool pips[4][49] =
        {
                {
                        //spades
                        0, 0, 0, 1, 0, 0, 0,
                        0, 0, 1, 1, 1, 0, 0,
                        0, 1, 1, 1, 1, 1, 0,
                        1, 1, 1, 1, 1, 1, 1,
                        1, 1, 0, 1, 0, 1, 1,
                        0, 0, 0, 1, 0, 0, 0,
                        0, 0, 1, 1, 1, 0, 0
                },
                {
                        //hearts
                        0, 1, 0, 0, 0, 1, 0,
                        1, 1, 1, 0, 1, 1, 1,
                        1, 1, 1, 1, 1, 1, 1,
                        1, 1, 1, 1, 1, 1, 1,
                        0, 1, 1, 1, 1, 1, 0,
                        0, 0, 1, 1, 1, 0, 0,
                        0, 0, 0, 1, 0, 0, 0,
                },
                {
                        //diamonds
                        0, 0, 0, 1, 0, 0, 0,
                        0, 0, 1, 1, 1, 0, 0,
                        0, 1, 1, 1, 1, 1, 0,
                        1, 1, 1, 1, 1, 1, 1,
                        0, 1, 1, 1, 1, 1, 0,
                        0, 0, 1, 1, 1, 0, 0,
                        0, 0, 0, 1, 0, 0, 0
                },
                {
                        //clubs
                        0, 0, 1, 1, 1, 0, 0,
                        0, 0, 1, 1, 1, 0, 0,
                        1, 1, 0, 1, 0, 1, 1,
                        1, 1, 1, 1, 1, 1, 1,
                        1, 1, 0, 1, 0, 1, 1,
                        0, 0, 0, 1, 0, 0, 0,
                        0, 0, 1, 1, 1, 0, 0
                }
        };

bool backDesign[4] = {
        0, 1,
        1, 0
};
//endregion

uint8_t characters[13] =
        {
                2, 3, 4, 5, 6, 7, 8, 9, 10, 'J', 'Q', 'K', 'A'
        };

uint8_t edge_cards[3] = {
        0, 8, 15
};
//region Player card positions
uint8_t playerCardPositions[22][4] = {
        //first row
        {108, 38, 0, 0},
        {98,  38, 0, 1},
        {88,  38, 0, 1},
        {78,  38, 0, 1},
        {68,  38, 0, 1},
        {58,  38, 0, 1},
        {48,  38, 0, 1},
        {38,  38, 0, 1},
        //second row
        {104, 26, 1, 0},
        {94,  26, 1, 1},
        {84,  26, 1, 1},
        {74,  26, 1, 1},
        {64,  26, 1, 1},
        {54,  26, 1, 1},
        {44,  26, 1, 1},
        //third row
        {99,  14, 1, 0},
        {89,  14, 1, 1},
        {79,  14, 1, 1},
        {69,  14, 1, 1},
        {59,  14, 1, 1},
        {49,  14, 1, 1},
};
//endregion


void drawPlayerDeck(const Card cards[21], uint8_t count, Canvas *const canvas) {
    for (uint8_t i = 0; i < count; i++) {
        CardState state = Normal;
        if (playerCardPositions[i][2] == 1 && playerCardPositions[i][3] == 1)
            state = BottomAndRightCut;
        else if (playerCardPositions[i][3] == 1)
            state = RightCut;
        else if (playerCardPositions[i][2] == 1)
            state = BottomCut;
        drawCardAt(playerCardPositions[i][0], playerCardPositions[i][1], cards[i].pip, cards[i].character, state,
                   canvas);
    }
}

bool is_at_edge(uint8_t index) {
    for (uint8_t i = 0; i < 3; i++)
        if (edge_cards[i] == index) return true;

    return false;
}

Vector card_pos_at_index(uint8_t index) {
    return (Vector) {
            playerCardPositions[index][0],
            playerCardPositions[index][1]
    };
}

void drawCardAt(int8_t pos_x, int8_t pos_y, uint8_t pip, uint8_t character, CardState state, Canvas *const canvas) {
    if (state == Normal) {
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, pos_x, pos_y, CARD_WIDTH, CARD_HEIGHT);

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_frame(canvas, pos_x, pos_y, CARD_WIDTH, CARD_HEIGHT);
    } else {
        if (state == BottomCut || state == BottomAndRightCut)
            canvas_draw_line(canvas, pos_x, pos_y, pos_x, pos_y + CARD_HALF_HEIGHT - 1);   //half height line

        if (state == BottomCut) {
            canvas_draw_line(canvas, pos_x, pos_y, pos_x + CARD_WIDTH - 1, pos_y);  //full width line
            canvas_draw_line(canvas, pos_x + CARD_WIDTH - 1, pos_y, pos_x + CARD_WIDTH - 1,
                             pos_y + CARD_HALF_HEIGHT - 1); //half height line
        }

        if (state == BottomAndRightCut) {
            canvas_draw_line(canvas, pos_x, pos_y, pos_x + CARD_HALF_WIDTH - 1, pos_y);  //half width
        }

        if (state == RightCut) {
            canvas_draw_line(canvas, pos_x, pos_y, pos_x + CARD_HALF_WIDTH - 1, pos_y);   //half width
            canvas_draw_line(canvas, pos_x, pos_y, pos_x, pos_y + CARD_HEIGHT - 1);    //full height line
            canvas_draw_line(canvas, pos_x, pos_y + CARD_HEIGHT - 1, pos_x + CARD_HALF_WIDTH - 1,
                             pos_y + CARD_HEIGHT - 1);  //full height line
        }

    }

    uint8_t left = pos_x + CORNER_MARGIN;
    uint8_t right = (pos_x + CARD_WIDTH - CORNER_MARGIN - 7);
    uint8_t top = pos_y + CORNER_MARGIN;
    uint8_t bottom = (pos_y + CARD_HEIGHT - CORNER_MARGIN - 7);

    for (uint8_t x = 0; x < 7; x++) {
        for (uint8_t y = 0; y < 7; y++) {
            if (pips[pip][x + y * 7]) {
                if (state == Normal || state == BottomCut)
                    canvas_draw_dot(canvas, right + x + 1, top + y);
                if (state == Normal || state == RightCut)
                    canvas_draw_dot(canvas, left + x - 1, bottom + y);
            }
        }
    }

    canvas_set_font(canvas, FontSecondary);
    char drawChar[3];
    if (character < 9)
        snprintf(drawChar, sizeof(drawChar), "%i", character + 2);
    else {
        snprintf(drawChar, sizeof(drawChar), "%c", characters[character]);
    }

    canvas_set_font_direction(canvas, CanvasDirectionLeftToRight);
    canvas_draw_str_aligned(canvas, left + 2, top + 3, AlignCenter, AlignCenter, drawChar);

    canvas_set_font_direction(canvas, CanvasDirectionRightToLeft);
    if (state == Normal) { //flipper crashes on non center aligned text when upside down
        uint8_t margin = 9;
        if (character == 8) //10 needs bigger margin
            margin = 12;
        canvas_draw_str_aligned(canvas, right + margin, bottom - 3, AlignCenter, AlignCenter, drawChar);
    }

    canvas_set_font_direction(canvas, CanvasDirectionLeftToRight);
    //canvas_draw_str(canvas, left, top, drawChar );
}

void drawCardBackAt(int8_t pos_x, int8_t pos_y, Canvas *const canvas) {
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, pos_x, pos_y, CARD_WIDTH, CARD_HEIGHT);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, pos_x, pos_y, CARD_WIDTH, CARD_HEIGHT);
    for (uint8_t x = 0; x < CARD_WIDTH - 2; x++) {
        for (uint8_t y = 0; y < CARD_HEIGHT - 2; y++) {
            uint8_t _x = x;
            uint8_t _y = y * 2;
            if (backDesign[(_x + _y) % 4]) {
                canvas_draw_dot(canvas, pos_x + x + 1, pos_y + y + 1);
            }
        }
    }
}

void generateDeck(Deck *deck_ptr) {
    uint16_t counter = 0;
    for (uint8_t deck = 0; deck < DECK_COUNT; deck++) {
        for (uint8_t pip = 0; pip < 4; pip++) {
            for (uint8_t label = 0; label < 13; label++) {
                deck_ptr->cards[counter] = (Card)
                        {
                                pip, label
                        };
                counter++;
            }
        }
    }
}

void shuffleDeck(Deck *deck_ptr) {
    srand(DWT->CYCCNT);
    deck_ptr->index = 0;
    int max = DECK_COUNT * 52;
    for (int i = 0; i < max; i++) {
        int r = i + (rand() % (max - i));
        Card tmp = deck_ptr->cards[i];
        deck_ptr->cards[i] = deck_ptr->cards[r];
        deck_ptr->cards[r] = tmp;
    }
}

uint8_t handCount(const Card cards[21], uint8_t count) {
    uint8_t aceCount = 0;
    uint8_t score = 0;

    for (uint8_t i = 0; i < count; i++) {
        if (cards[i].character == 12)
            aceCount++;
        else {
            if (cards[i].character > 8)
                score += 10;
            else
                score += cards[i].character + 2;
        }
    }

    for (uint8_t i = 0; i < aceCount; i++) {
        if ((score + 11) <= 21) score += 11;
        else score++;
    }

    return score;
}

void draw_card_animation(Card animatingCard, Vector from, Vector control, Vector to, float t, bool extra_margin,
                         Canvas *const canvas) {
    float time = t;
    if (extra_margin) {
        time += 0.2;
    }

    Vector currentPos = quadratic_2d(from, control, to, time);
    if (t > 1) {
        drawCardAt(currentPos.x, currentPos.y, animatingCard.pip,
                   animatingCard.character, Normal, canvas);
    } else {
        if (t < 0.5)
            drawCardBackAt(currentPos.x, currentPos.y, canvas);
        else
            drawCardAt(currentPos.x, currentPos.y, animatingCard.pip,
                       animatingCard.character, Normal, canvas);
    }
}