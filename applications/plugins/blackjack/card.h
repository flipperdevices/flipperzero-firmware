#ifndef _card_h
#define _card_h

#include <gui/gui.h>

#define DECK_COUNT 6
#define CARD_HEIGHT 24
#define CARD_HALF_HEIGHT CARD_HEIGHT/2
#define CARD_WIDHT 18
#define CARD_HALF_WIDHT CARD_WIDHT/2
#define CORNER_MARGIN 3
#define LEGEND_SIZE 10

typedef enum {
    Normal, BottomCut, RightCut, BottomAndRightCut, TopCut, LeftCut, TopAndLeftCut
} CardState;

typedef struct {
    uint8_t pip;
    uint8_t character;
} Card;

typedef struct {
    Card cards[52 * DECK_COUNT];
    int index;
} Deck;

void drawPlayerDeck(const Card cards[21], uint8_t count, Canvas *const canvas);

void drawCardAt(uint8_t pos_x, uint8_t pos_y, uint8_t pip, uint8_t character, CardState state, Canvas *const canvas);

void drawCardBackAt(uint8_t pos_x, uint8_t pos_y, Canvas *const canvas);

void generateDeck(Deck *deck_ptr);

void shuffleDeck(Deck *deck_ptr);

uint8_t handCount(const Card cards[21], uint8_t count);

#endif