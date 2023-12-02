#include "scene_game.h"
#include "app.h"
#include "game.h"
#include "app_gameplay.h"
#include "wave/scene_management.h"
#include "wave/calc.h"
#include "wave/data_structures/list.h"
#include "wave/exception_manager.h"
#include "racso_zero_icons.h"
#include <gui/gui.h>
#include <furi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define VISUAL_CARD_WIDTH 23
#define VISUAL_CARD_HEIGHT 13
#define VISUAL_ICON_WIDTH 9
#define VISUAL_ICON_HEIGHT 9
#define VISUAL_ICON_BIG_WIDTH 17

#define VISUAL_CARD_SPACING 0
#define VISUAL_CARDS_ROWS 1
#define VISUAL_CARDS_COLUMNS 5

#define VISUAL_PLAYER_CARDS_X 7
#define VISUAL_PLAYER_CARDS_Y 63

typedef enum {
    CardDrawMode_Solid,
    CardDrawMode_Filled,
    CardDrawMode_Dotted,
    CardDrawMode_Dashed,
    CardDrawMode_NoBorder,
} CardDrawMode;

typedef enum { CardDrawAlign_TopLeft, CardDrawAlign_Center } CardDrawAlign;

// Suit selector component.
typedef struct SuitSelector {
    int selectedSuitIndex;
    bool selectionConfirmed;
    bool isEnabled;
} SuitSelector;

static SuitSelector suitSelector;

void suit_selector_set_enabled(SuitSelector* suitSelector, bool isEnabled) {
    suitSelector->isEnabled = isEnabled;
    suitSelector->selectedSuitIndex = 0;
    suitSelector->selectionConfirmed = false;
}

bool suit_selector_is_enabled(SuitSelector* suitSelector) {
    return suitSelector->isEnabled;
}

void suit_selector_set_index(SuitSelector* suitSelector, int index) {
    suitSelector->selectedSuitIndex = index;
}

int suit_selector_get_index(SuitSelector* suitSelector) {
    return suitSelector->selectedSuitIndex;
}

void suit_selector_confirm_selection(SuitSelector* suitSelector) {
    if(suit_selector_get_index(suitSelector) > 0) suitSelector->selectionConfirmed = true;
}

bool suit_selector_is_selection_confirmed(SuitSelector* suitSelector) {
    return suitSelector->selectionConfirmed;
}

void draw_suit_selector(Canvas* canvas, SuitSelector* suitSelector) {
    if(!suit_selector_is_enabled(suitSelector)) return;

    int w = 50, h = VISUAL_ICON_HEIGHT + 8, spacing = 2;
    int x = 64 - w / 2, y = 32 - h / 2;

    // Frame:
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x, y, w, h); // White margin
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, x + 1, y + 1, w - 2, h - 2); // Black border
    // 1 px white padding goes here.

    // Icons:

    // Highlighting an icon uses 1 px around the icon. Therefore, the icons are drawn with an additional 1 px margin.

    x += 3 + 1, y += 3 + 1; // Skips margin, border and padding + additional 1 px margin

    const Icon* icons[4] = {&I_s1, &I_s2, &I_s3, &I_s4};
    canvas_set_color(canvas, ColorBlack);
    for(int i = 0; i < 4; i++)
        canvas_draw_icon(canvas, x + i * (VISUAL_ICON_WIDTH + spacing), y, icons[i]);

    // Highlight selection:
    if(suit_selector_get_index(suitSelector) > 0) {
        canvas_set_color(canvas, ColorXOR);
        canvas_draw_box(
            canvas,
            x - 1 + (suit_selector_get_index(suitSelector) - 1) * (VISUAL_ICON_WIDTH + spacing),
            y - 1,
            VISUAL_ICON_WIDTH + 2,
            VISUAL_ICON_HEIGHT + 2);
    }
}

void suit_selector_handle_input(SuitSelector* suitSelector, InputKey key, InputType type) {
    if(!suit_selector_is_enabled(suitSelector) ||
       suit_selector_is_selection_confirmed(suitSelector))
        return;

    if(type != InputTypePress) return;

    if(key == InputKeyLeft || key == InputKeyUp)
        suitSelector->selectedSuitIndex = wrap_single(suitSelector->selectedSuitIndex - 1, 1, 4);
    if(key == InputKeyRight || key == InputKeyDown)
        suitSelector->selectedSuitIndex = wrap_single(suitSelector->selectedSuitIndex + 1, 1, 4);

    if(key == InputKeyOk) suit_selector_confirm_selection(suitSelector);
}

// End of suit selector.

void screen_printer_print(char* string, Canvas* canvas, int x, int y) {
    char* start = string;
    char* end;
    char lineBuffer[200];

    while((end = strchr(start, '\n')) != NULL) {
        strncpy(lineBuffer, start, end - start);
        lineBuffer[end - start] = '\0';

        canvas_draw_str_aligned(canvas, x, y, AlignLeft, AlignTop, lineBuffer);

        y += 10;
        start = end + 1;
    }
}

const Icon* get_player_icon(int playerIndex) {
    throw_exception_if(playerIndex < 1 || playerIndex > NUMBER_OF_PLAYERS, "Invalid player index");

    static const Icon* playerIcons[NUMBER_OF_PLAYERS] = {
        &I_player1, &I_player2, &I_player3, &I_player4};
    return playerIcons[playerIndex - 1];
}

char* get_card_suit_name(CardSuit suit) {
    switch(suit) {
    case CardSuit_Spades:
        return "Spades";
    case CardSuit_Hearts:
        return "Hearts";
    case CardSuit_Diamonds:
        return "Diamonds";
    case CardSuit_Clubs:
        return "Clubs";
    default:
        return "???";
    }
}

void draw_rectangle_dashed(Canvas* canvas, int x, int y, int w, int h, Color color) {
    canvas_set_color(canvas, color);

    // dot, dot, space, dot, dot, space, ...
    for(int i = 0; i < w; i++) {
        if(i % 6 < 5) canvas_draw_dot(canvas, x + i, y);
        if(i % 6 < 5) canvas_draw_dot(canvas, x + i, y + h - 1);
    }

    for(int i = 0; i < h; i++) {
        if(i % 6 < 5) canvas_draw_dot(canvas, x, y + i);
        if(i % 6 < 5) canvas_draw_dot(canvas, x + w - 1, y + i);
    }
}

void draw_rectangle_dotted(Canvas* canvas, int x, int y, int w, int h, Color color) {
    canvas_set_color(canvas, color);

    for(int i = 0; i < w; i++) {
        if(i % 2 == 0) canvas_draw_dot(canvas, x + i, y);
        if(i % 2 == 0) canvas_draw_dot(canvas, x + i, y + h - 1);
    }

    for(int i = 0; i < h; i++) {
        if(i % 2 == 0) canvas_draw_dot(canvas, x, y + i);
        if(i % 2 == 0) canvas_draw_dot(canvas, x + w - 1, y + i);
    }
}

void draw_card_content(Canvas* canvas, int x, int y, Card card) {
    if(card.action == ActionType_ChangeSuit || card.action == ActionType_ChangeSuitPlus4) {
        int iconX = x - VISUAL_ICON_BIG_WIDTH / 2;
        int iconY = y - VISUAL_ICON_HEIGHT / 2;

        const Icon* icon = card.action == ActionType_ChangeSuit ? &I_joker : &I_joker4;
        canvas_draw_icon(canvas, iconX, iconY, icon);
        return;
    }

    // Draw left part: number or action
    if(card.number >= 0) {
        char numberStr[11];
        snprintf(numberStr, 11, "%d", card.number);
        canvas_set_font(canvas, FontKeyboard);
        canvas_draw_str_aligned(canvas, x - 4, y + 1, AlignCenter, AlignCenter, numberStr);
    } else {
        const Icon* icon = NULL;
        switch(card.action) {
        case ActionType_Skip:
            icon = &I_s;
            break;
        case ActionType_Invert:
            icon = &I_r;
            break;
        case ActionType_Plus2:
            icon = &I_p2;
            break;
        default:
            throw_exception("Invalid action type");
        }
        canvas_draw_icon(canvas, x - 9, y - 4, icon);
    }

    // Draw right part: suit
    const Icon* icon = NULL;
    switch(card.suit) {
    case CardSuit_Spades:
        icon = &I_s1;
        break;
    case CardSuit_Hearts:
        icon = &I_s2;
        break;
    case CardSuit_Diamonds:
        icon = &I_s3;
        break;
    case CardSuit_Clubs:
        icon = &I_s4;
        break;
    default:
        throw_exception("Invalid card suit");
    }

    canvas_draw_icon(canvas, x + 1, y - 4, icon);
}

void draw_card(Canvas* canvas, Card card, int x, int y, CardDrawAlign align, CardDrawMode mode) {
    const int cardWidth = 23, cardHeight = 13;

    if(align == CardDrawAlign_Center) x -= cardWidth / 2, y -= cardHeight / 2;

    int cardCenterX = x + cardWidth / 2;
    int cardCenterY = y + cardHeight / 2;

    canvas_set_color(canvas, ColorBlack);

    draw_card_content(canvas, cardCenterX, cardCenterY, card);

    if(mode == CardDrawMode_Solid) canvas_draw_frame(canvas, x, y, cardWidth, cardHeight);

    if(mode == CardDrawMode_Filled) {
        canvas_set_color(canvas, ColorXOR);
        canvas_draw_box(canvas, x, y, cardWidth, cardHeight);
    }

    if(mode == CardDrawMode_Dotted)
        draw_rectangle_dotted(canvas, x, y, cardWidth, cardHeight, ColorBlack);

    if(mode == CardDrawMode_Dashed)
        draw_rectangle_dashed(canvas, x, y, cardWidth, cardHeight, ColorBlack);
}

void draw_my_cards_numbering(
    Canvas* canvas,
    int x,
    int y,
    int cardsInHandCount,
    int selectedHandIndex) {
    char numberingStr[16];
    snprintf(numberingStr, 16, "%d/%d", selectedHandIndex + 1, cardsInHandCount);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontKeyboard);
    canvas_draw_str_aligned(canvas, x, y, AlignLeft, AlignBottom, numberingStr);
}

void draw_my_cards(
    Canvas* canvas,
    GameState* game,
    int x,
    int y,
    int playerIndex,
    int selectedHandIndex,
    bool showSelection) {
    List* cardsInHand = list_alloc(MAX_HAND_SIZE, sizeof(int));
    game_get_player_hand(game, playerIndex, cardsInHand);
    int cardsInHandCount = list_count(cardsInHand);

    int cardsPerPage = VISUAL_CARDS_ROWS * VISUAL_CARDS_COLUMNS;

    int currentPage = selectedHandIndex / cardsPerPage;
    int currentPageStartIndex = currentPage * cardsPerPage;
    int pageSlotIndex = 0;

    for(int handIndex = currentPageStartIndex; handIndex < currentPageStartIndex + cardsPerPage;
        handIndex++) {
        if(handIndex >= list_count(cardsInHand)) break;

        int cardX = x + (pageSlotIndex / VISUAL_CARDS_COLUMNS) *
                            (VISUAL_CARD_WIDTH - 1 + VISUAL_CARD_SPACING);
        int cardY = y - VISUAL_CARD_HEIGHT -
                    (pageSlotIndex % VISUAL_CARDS_COLUMNS) *
                        (VISUAL_CARD_HEIGHT - 1 + VISUAL_CARD_SPACING);

        int cardInSlot;
        list_get_at(cardsInHand, handIndex, &cardInSlot);

        CardDrawMode drawMode = CardDrawMode_NoBorder;
        if(showSelection) {
            bool isValidCard = game_is_card_valid_to_play(game, cardInSlot);
            bool isSelected = handIndex == selectedHandIndex;
            if(isValidCard && isSelected)
                drawMode = CardDrawMode_Filled;
            else if(isSelected)
                drawMode = CardDrawMode_Solid;
            else if(isValidCard)
                drawMode = CardDrawMode_Dotted;
            else
                drawMode = CardDrawMode_NoBorder;
        }
        draw_card(canvas, AllCardsData[cardInSlot], cardX, cardY, CardDrawAlign_TopLeft, drawMode);

        const int ARROW_ICON_HEIGHT = 6, ARROW_ICON_WIDTH = 5;
        int numberOfPages = (cardsInHandCount - 1) / cardsPerPage + 1;
        if(numberOfPages > 1 &&
           (pageSlotIndex == cardsPerPage - 1 || handIndex == cardsInHandCount - 1))
            canvas_draw_icon(canvas, cardX - ARROW_ICON_WIDTH - 2, cardY, &I_arrow_up);
        if(numberOfPages > 1 && pageSlotIndex == 0)
            canvas_draw_icon_ex(
                canvas,
                cardX + VISUAL_CARD_WIDTH + 2,
                cardY + VISUAL_CARD_HEIGHT - ARROW_ICON_HEIGHT,
                &I_arrow_up,
                IconRotation180);

        pageSlotIndex++;
    }

    if(showSelection && cardsInHandCount > 0)
        draw_my_cards_numbering(
            canvas, x + VISUAL_CARD_WIDTH + 12, y - 2, cardsInHandCount, selectedHandIndex);

    list_free(cardsInHand);
}

void draw_player_card_count(
    Canvas* canvas,
    GameState* game,
    int playerIndex,
    int x,
    int y,
    bool isCurrentTurn) {
    int cardCount = 0;
    for(int i = 0; i < NUMBER_OF_CARDS; i++)
        if(game_get_card_location(game, i) == playerIndex) cardCount++;

    char cardCountStr[16];
    snprintf(cardCountStr, 16, "%d", cardCount);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);

    int boxX = x, boxY = y;

    const Icon* playerIcon = get_player_icon(playerIndex);
    canvas_draw_icon(
        canvas,
        x + VISUAL_CARD_WIDTH + 1,
        y + (VISUAL_CARD_HEIGHT - VISUAL_ICON_HEIGHT) / 2,
        playerIcon);

    canvas_set_font(canvas, FontPrimary);

    if(isCurrentTurn)
        canvas_draw_box(canvas, boxX, y, VISUAL_CARD_WIDTH, VISUAL_CARD_HEIGHT);
    else
        draw_rectangle_dotted(
            canvas, boxX, boxY, VISUAL_CARD_WIDTH, VISUAL_CARD_HEIGHT, ColorBlack);

    canvas_set_color(canvas, ColorXOR);
    canvas_draw_str_aligned(
        canvas,
        boxX + VISUAL_CARD_WIDTH / 2,
        boxY + VISUAL_CARD_HEIGHT / 2,
        AlignCenter,
        AlignCenter,
        cardCountStr);
}

void draw_forced_suit(Canvas* canvas, CardSuit suit, int x, int y, CardDrawMode drawMode) {
    if(suit == CardSuit_None) return;

    canvas_set_color(canvas, ColorBlack);
    const Icon* icon = NULL;

    switch(suit) {
    case CardSuit_Spades:
        icon = &I_s1;
        break;
    case CardSuit_Hearts:
        icon = &I_s2;
        break;
    case CardSuit_Diamonds:
        icon = &I_s3;
        break;
    case CardSuit_Clubs:
        icon = &I_s4;
        break;
    default:
        throw_exception("Invalid card suit");
    }

    canvas_draw_icon(canvas, x, y, icon);
    if(drawMode == CardDrawMode_Filled) {
        canvas_set_color(canvas, ColorXOR);
        canvas_draw_box(canvas, x - 1, y - 1, 11, 11);
    }
}

void draw_winner_screen(Canvas* canvas, int winner) {
    if(winner == 0) return;

    int w = 91, h = 31;
    int x = 64 - w / 2, y = 32 - h / 2;

    // draw a black border with a 1px white margin
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x, y, w, h);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, x + 1, y + 1, w - 2, h - 2);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    char* winnerStr = winner == PLAYER_NUMBER ? "You win!" : "Opponent wins!";
    canvas_draw_str_aligned(canvas, 64, 28, AlignCenter, AlignCenter, winnerStr);
    canvas_draw_icon(canvas, 64 - VISUAL_ICON_WIDTH / 2, 34, get_player_icon(winner));
}

void draw_game(Canvas* canvas, AppContext* context) {
    AppGameplayState* gameplay = context->gameplay;
    GameState* game = gameplay_get_game(gameplay);
    int topCard = game_get_top_card(game);
    int currentTurn = game_get_player_turn(game);

    draw_player_card_count(canvas, game, 2, 128 - VISUAL_CARD_WIDTH - 10, 1, currentTurn == 2);
    draw_player_card_count(
        canvas, game, 3, 128 - VISUAL_CARD_WIDTH - 10, 1 + 12, currentTurn == 3);
    draw_player_card_count(
        canvas, game, 4, 128 - VISUAL_CARD_WIDTH - 10, 1 + 24, currentTurn == 4);
    draw_player_card_count(
        canvas, game, 1, 128 - VISUAL_CARD_WIDTH - 10, 63 - VISUAL_CARD_HEIGHT, currentTurn == 1);
    const Icon* directionIcon = game_get_direction(game) == 1 ? &I_arrow_cw : &I_arrow_ccw;
    canvas_draw_icon(canvas, 84, 1, directionIcon);

    // Draw top card or player card
    int mainCardX = 63, mainCardY = 31;
    int forcedSuitX = mainCardX - VISUAL_ICON_WIDTH / 2,
        forcedSuitY = mainCardY + VISUAL_CARD_HEIGHT / 2 + 3;

    if(game_get_card_to_play(game) == CARD_NONE) {
        draw_card(
            canvas,
            AllCardsData[topCard],
            mainCardX,
            mainCardY,
            CardDrawAlign_Center,
            CardDrawMode_Solid);
        draw_forced_suit(
            canvas, game_get_forced_suit(game), forcedSuitX, forcedSuitY, CardDrawMode_NoBorder);
    } else {
        draw_card(
            canvas,
            AllCardsData[game_get_card_to_play(game)],
            mainCardX,
            mainCardY,
            CardDrawAlign_Center,
            CardDrawMode_Filled);
        draw_forced_suit(
            canvas,
            game_get_forced_suit_to_play(game),
            forcedSuitX,
            forcedSuitY,
            CardDrawMode_Filled);
    }

    int selectedHandIndex = gameplay_selection_get_hand_index(gameplay);
    draw_my_cards(
        canvas,
        game,
        VISUAL_PLAYER_CARDS_X,
        VISUAL_PLAYER_CARDS_Y,
        1,
        selectedHandIndex,
        currentTurn == PLAYER_NUMBER);

    draw_suit_selector(canvas, &suitSelector);

    draw_winner_screen(canvas, game_get_winner(game));
}

void game_transition_callback(int from, int to, void* context) {
    AppContext* app = (AppContext*)context;
    UNUSED(from);

    if(to == SceneType_Game) {
        gameplay_reset(app->gameplay);
        suit_selector_set_enabled(&suitSelector, false);
    }
}

void game_render_callback(Canvas* const canvas, void* context) {
    //AppContext* app = (AppContext*)context;
    //AppGameplayState* gameplay = app->gameplay;
    canvas_clear(canvas);
    draw_game(canvas, context);
}

void ai_play_card(GameState* game, int cardIndex) {
    CardSuit forcedSuit = CardSuit_None;
    Card card = AllCardsData[cardIndex];
    if(card.action == ActionType_ChangeSuit || card.action == ActionType_ChangeSuitPlus4)
        forcedSuit = rand() % 4 + 1;
    game_set_card_to_play(game, cardIndex, forcedSuit);
}

void ai_play_turn(GameState* game) {
    int playerIndex = game_get_player_turn(game);
    int randomCard = rand() % NUMBER_OF_CARDS;
    int cardIndex = randomCard;
    while(true) {
        if(game_get_card_location(game, cardIndex) == playerIndex &&
           game_is_card_valid_to_play(game, cardIndex)) {
            FURI_LOG_D("AI", "AI player %d selected card %d", playerIndex, cardIndex);
            ai_play_card(game, cardIndex);
            return;
        }

        cardIndex = (cardIndex + 1) % NUMBER_OF_CARDS;
        if(cardIndex == randomCard) {
            FURI_LOG_D("AI", "AI player %d has no cards to play", playerIndex);

            int drawnCard = game_step_draw_card(game, playerIndex);
            FURI_LOG_D("AI", "AI player %d drew card %d", playerIndex, drawnCard);

            if(game_is_card_valid_to_play(game, cardIndex)) {
                ai_play_card(game, cardIndex);
                return;
            } else {
                FURI_LOG_D("AI", "AI player %d skipped turn", playerIndex);
                game_step_next_player(game);
                return;
            }
        }
    }
}

void game_handle_input(InputKey key, InputType type, void* context) {
    AppContext* app = (AppContext*)context;
    AppGameplayState* gameplay = app->gameplay;
    GameState* game = gameplay_get_game(gameplay);

    if(type != InputTypePress) return;

    if(key == InputKeyBack) {
        scene_manager_set_scene(app->sceneManager, SceneType_Menu);
        return;
    }

    // State: displaying winner
    if(game_get_winner(game) != 0) return;

    // State: displaying the card that is being played
    if(game_get_card_to_play(game) != CARD_NONE) {
        if(key == InputKeyOk) game_apply_card_to_play(game);
        return;
    }

    // State: player is selecting a suit for a joker
    if(suit_selector_is_enabled(&suitSelector)) {
        suit_selector_handle_input(&suitSelector, key, type);
        if(suit_selector_is_selection_confirmed(&suitSelector)) {
            int selectedCard = gameplay_get_selected_card(gameplay);
            game_set_card_to_play(game, selectedCard, suit_selector_get_index(&suitSelector));
            suit_selector_set_enabled(&suitSelector, false);
        }

        return;
    }

    // States: players select card.

    // If the player has no valid cards to play, a special flow is done.
    if(game_get_player_hand_valid_cards_count(game, game_get_player_turn(game)) == 0) {
        FURI_LOG_D(
            "GAME",
            "Player %d has no valid cards to play. Drawing card.",
            game_get_player_turn(game));
        int card = game_step_draw_card(game, game_get_player_turn(game));
        if(!game_is_card_valid_to_play(game, card)) {
            FURI_LOG_D("GAME", "Card cannot be played. Skipping turn.");
            game_step_next_player(game);
        }

        return;
    }

    // State: player is selecting a card to play
    if(game_get_player_turn(game) == PLAYER_NUMBER) {
        if(key == InputKeyOk) {
            int selectedCard = gameplay_get_selected_card(gameplay);

            if(game_is_card_valid_to_play(game, selectedCard)) {
                bool isJoker = AllCardsData[selectedCard].action == ActionType_ChangeSuit ||
                               AllCardsData[selectedCard].action == ActionType_ChangeSuitPlus4;
                if(isJoker)
                    suit_selector_set_enabled(&suitSelector, true);
                else
                    game_set_card_to_play(game, selectedCard, CardSuit_None);
            }

            return;
        }

        if(key == InputKeyLeft || key == InputKeyUp) {
            gameplay_selection_delta(gameplay, 1);
            return;
        }

        if(key == InputKeyRight || key == InputKeyDown) {
            gameplay_selection_delta(gameplay, -1);
            return;
        }

        return;
    }

    // State: CPU is playing
    if(game_get_player_turn(game) != PLAYER_NUMBER) {
        if(key == InputKeyOk) {
            ai_play_turn(game);
            return;
        }
    }
}

void game_tick_callback(void* context) {
    AppContext* app = (AppContext*)context;
    UNUSED(app);
}