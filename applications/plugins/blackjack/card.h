#ifndef _card_h
#define _card_h

#include <gui/gui.h>

#define DECK_COUNT 6
#define CARD_HEIGHT 24
#define CARD_HALF_HEIGHT CARD_HEIGHT / 2
#define CARD_WIDTH 18
#define CARD_HALF_WIDTH CARD_WIDTH / 2
#define CORNER_MARGIN 3
#define LEGEND_SIZE 10
typedef struct Vector Vector;

typedef enum {
    Normal,
    BottomCut,
    RightCut,
    BottomAndRightCut,
    TopCut,
    LeftCut,
    TopAndLeftCut
} CardState;

typedef struct {
    uint8_t pip;
    uint8_t character;
} Card;

typedef struct {
    Card cards[52 * DECK_COUNT];
    int index;
} Deck;

void drawPlayerDeck(const Card cards[21], uint8_t count, Canvas* const canvas);

void drawCardAt(
    int8_t pos_x,
    int8_t pos_y,
    uint8_t pip,
    uint8_t character,
    CardState state,
    Canvas* const canvas);

void drawCardBackAt(int8_t pos_x, int8_t pos_y, Canvas* const canvas);

void generateDeck(Deck* deck_ptr);

void shuffleDeck(Deck* deck_ptr);

void draw_card_animation(
    Card animatingCard,
    Vector from,
    Vector control,
    Vector to,
    float t,
    bool extra_margin,
    Canvas* const canvas);

Vector card_pos_at_index(uint8_t index);
bool is_at_edge(uint8_t index);
uint8_t handCount(const Card cards[21], uint8_t count);

#endif