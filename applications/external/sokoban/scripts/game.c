#include "game.h"
#include "wave/data_structures/queue.h"
#include "wave/data_structures/list.h"
#include "wave/exception_manager.h"
#include <gui/gui.h>
#include <stdlib.h>
#include <math.h>

Card AllCardsData[NUMBER_OF_CARDS];
const int CARD_NONE = -1;

struct LevelState
{
    int cardLocation[NUMBER_OF_CARDS]; // 0 = deck, -1 = discard, 1+ = player index
    int playerTurn;
    int direction;
    int topCard;
    CardSuit forcedSuit;

    int cardToPlay;
    CardSuit forcedSuitToPlay;

    int winner;

    Queue* shuffleQueue;
};