#include "card.h"
#include "dml.h"

#define CORNER_MARGIN 3
#define CARD_DRAW_X_START 108
#define CARD_DRAW_Y_START 38
#define CARD_DRAW_X_SPACE 10
#define CARD_DRAW_Y_SPACE 8
#define CARD_DRAW_X_OFFSET 4
#define CARD_DRAW_FIRST_ROW_LENGTH 7

bool pips[4][49] = {
    {//spades
     0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0},
    {
        //hearts
        0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
    },
    {//diamonds
     0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
     1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {//clubs
     0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0}};

//region Player card positions
uint8_t playerCardPositions[22][4] = {
    //first row
    {108, 38},
    {98, 38},
    {88, 38},
    {78, 38},
    {68, 38},
    {58, 38},
    {48, 38},
    {38, 38},
    //second row
    {104, 26},
    {94, 26},
    {84, 26},
    {74, 26},
    {64, 26},
    {54, 26},
    {44, 26},
    //third row
    {99, 14},
    {89, 14},
    {79, 14},
    {69, 14},
    {59, 14},
    {49, 14},
};
//endregion

bool get_pip_pixel(uint8_t pip, uint8_t x, uint8_t y) {
    return pips[pip][x + y * 7];
}

void draw_card_at(int8_t pos_x, int8_t pos_y, uint8_t pip, uint8_t character, Canvas* const canvas) {
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, pos_x, pos_y, CARD_WIDTH, CARD_HEIGHT);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, pos_x, pos_y, CARD_WIDTH, CARD_HEIGHT);

    uint8_t left = pos_x + CORNER_MARGIN;
    uint8_t right = (pos_x + CARD_WIDTH - CORNER_MARGIN - 7);
    uint8_t top = pos_y + CORNER_MARGIN;
    uint8_t bottom = (pos_y + CARD_HEIGHT - CORNER_MARGIN - 7);

    for(uint8_t x = 0; x < 7; x++) {
        for(uint8_t y = 0; y < 7; y++) {
            if(get_pip_pixel(pip, x, y)) {
                canvas_draw_dot(canvas, right + x + 1, top + y);
                canvas_draw_dot(canvas, left + x - 1, bottom + y);
            }
        }
    }

    canvas_set_font(canvas, FontSecondary);
    char drawChar[3];
    if(character < 9)
        snprintf(drawChar, sizeof(drawChar), "%i", character + 2);
    else {
        if(character == 9)
            snprintf(drawChar, sizeof(drawChar), "J");
        else if(character == 10)
            snprintf(drawChar, sizeof(drawChar), "Q");
        else if(character == 11)
            snprintf(drawChar, sizeof(drawChar), "K");
        else if(character == 12)
            snprintf(drawChar, sizeof(drawChar), "A");
    }

    canvas_set_font_direction(canvas, CanvasDirectionLeftToRight);
    canvas_draw_str_aligned(canvas, left + 2, top + 3, AlignCenter, AlignCenter, drawChar);

    canvas_set_font_direction(canvas, CanvasDirectionRightToLeft);
    //flipper crashes on non center aligned text when upside down
    uint8_t margin = 9;
    if(character == 8) //10 needs bigger margin
        margin = 12;
    canvas_draw_str_aligned(
        canvas, right + margin, bottom - 3, AlignCenter, AlignCenter, drawChar);

    canvas_set_font_direction(canvas, CanvasDirectionLeftToRight);
}

void draw_deck(const Card* cards, uint8_t count, Canvas* const canvas) {
    for(int i = count - 1; i >= 0; i--) {
        draw_card_at(
            playerCardPositions[i][0],
            playerCardPositions[i][1],
            cards[i].pip,
            cards[i].character,
            canvas);
    }
}

Vector card_pos_at_index(uint8_t index) {
    return (Vector){playerCardPositions[index][0], playerCardPositions[index][1]};
}

void draw_card_back_at(int8_t pos_x, int8_t pos_y, Canvas* const canvas) {
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, pos_x, pos_y, CARD_WIDTH, CARD_HEIGHT);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, pos_x, pos_y, CARD_WIDTH, CARD_HEIGHT);
    for(uint8_t x = 0; x < CARD_WIDTH - 2; x++) {
        for(uint8_t y = 0; y < CARD_HEIGHT - 2; y++) {
            if((x + y) % 2 == 1) {
                canvas_draw_dot(canvas, pos_x + x + 1, pos_y + y + 1);
            }
        }
    }
}

void generate_deck(Deck* deck_ptr, uint8_t deck_count) {
    uint16_t counter = 0;
    deck_ptr->deck_count = deck_count;
    deck_ptr->cards = malloc(sizeof(Card) * 52 * deck_count);
    for(uint8_t deck = 0; deck < deck_count; deck++) {
        for(uint8_t pip = 0; pip < 4; pip++) {
            for(uint8_t label = 0; label < 13; label++) {
                deck_ptr->cards[counter] = (Card){pip, label};
                counter++;
            }
        }
    }
}

void shuffle_deck(Deck* deck_ptr) {
    srand(DWT->CYCCNT);
    deck_ptr->index = 0;
    int max = deck_ptr->deck_count * 52;
    for(int i = 0; i < max; i++) {
        int r = i + (rand() % (max - i));
        Card tmp = deck_ptr->cards[i];
        deck_ptr->cards[i] = deck_ptr->cards[r];
        deck_ptr->cards[r] = tmp;
    }
}

uint8_t hand_count(const Card* cards, uint8_t count) {
    uint8_t aceCount = 0;
    uint8_t score = 0;

    for(uint8_t i = 0; i < count; i++) {
        if(cards[i].character == 12)
            aceCount++;
        else {
            if(cards[i].character > 8)
                score += 10;
            else
                score += cards[i].character + 2;
        }
    }

    for(uint8_t i = 0; i < aceCount; i++) {
        if((score + 11) <= 21)
            score += 11;
        else
            score++;
    }

    return score;
}

void draw_card_animation(
    Card animatingCard,
    Vector from,
    Vector control,
    Vector to,
    float t,
    bool extra_margin,
    Canvas* const canvas) {
    float time = t;
    if(extra_margin) {
        time += 0.2;
    }

    Vector currentPos = quadratic_2d(from, control, to, time);
    if(t > 1) {
        draw_card_at(
            currentPos.x, currentPos.y, animatingCard.pip, animatingCard.character, canvas);
    } else {
        if(t < 0.5)
            draw_card_back_at(currentPos.x, currentPos.y, canvas);
        else
            draw_card_at(
                currentPos.x, currentPos.y, animatingCard.pip, animatingCard.character, canvas);
    }
}
