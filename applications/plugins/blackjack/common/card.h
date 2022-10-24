#pragma once

#include <gui/gui.h>
#include <math.h>
#include <stdlib.h>
#include "dml.h"

#define CARD_HEIGHT 24
#define CARD_HALF_HEIGHT CARD_HEIGHT / 2
#define CARD_WIDTH 18
#define CARD_HALF_WIDTH CARD_WIDTH / 2

//region types
typedef struct {
    uint8_t pip; //Pip index 0:spades, 1:hearths, 2:diamonds, 3:clubs
    uint8_t character; //Card letter [0-12], 0 means 2, 12 is Ace
} Card;

typedef struct {
    uint8_t deck_count; //Number of decks used
    Card* cards; //Cards in the deck
    int index; //Card index (to know where we at in the deck)
} Deck;
//endregion

/**
 * Gets card coordinates at the index (range: 0-20).
 *
 * @param index Index to check 0-20
 * @return      Position of the card
 */
Vector card_pos_at_index(uint8_t index);

/**
 * Draws card at a given coordinate (top-left corner)
 *
 * @param pos_x         X position
 * @param pos_y         Y position
 * @param pip           Pip index 0:spades, 1:hearths, 2:diamonds, 3:clubs
 * @param character     Letter [0-12] 0 is 2, 12 is A
 * @param canvas        Pointer to Flipper's canvas object
 */
void draw_card_at(int8_t pos_x, int8_t pos_y, uint8_t pip, uint8_t character, Canvas* const canvas);

/**
 * Draws 'count' cards at the bottom right corner
 *
 * @param cards     List of cards
 * @param count     Count of cards
 * @param canvas    Pointer to Flipper's canvas object
 */
void draw_deck(const Card* cards, uint8_t count, Canvas* const canvas);

/**
 * Draws card back at a given coordinate (top-left corner)
 *
 * @param pos_x     X coordinate
 * @param pos_y     Y coordinate
 * @param canvas    Pointer to Flipper's canvas object
 */
void draw_card_back_at(int8_t pos_x, int8_t pos_y, Canvas* const canvas);

/**
 * Generates the deck
 *
 * @param deck_ptr      Pointer to the deck
 * @param deck_count    Number of decks
 */
void generate_deck(Deck* deck_ptr, uint8_t deck_count);

/**
 * Shuffles the deck
 *
 * @param deck_ptr Pointer to the deck
 */
void shuffle_deck(Deck* deck_ptr);

/**
 * Calculates the hand count for blackjack
 *
 * @param cards     List of cards
 * @param count     Count of cards
 * @return          Hand value
 */
uint8_t hand_count(const Card* cards, uint8_t count);

/**
 * Draws card animation
 *
 * @param animatingCard Card to animate
 * @param from          Starting position
 * @param control       Quadratic lerp control point
 * @param to            End point
 * @param t             Current time (0-1)
 * @param extra_margin  Use extra margin at the end (arrives 0.2 unit before the end so it can stay there a bit)
 * @param canvas        Pointer to Flipper's canvas object
 */
void draw_card_animation(
    Card animatingCard,
    Vector from,
    Vector control,
    Vector to,
    float t,
    bool extra_margin,
    Canvas* const canvas);