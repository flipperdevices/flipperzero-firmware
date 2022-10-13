#include "util.h"

static List *afterDelay;

float lerp(float v0, float v1, float t) {
    return (1 - t) * v0 + t * v1;
}

void queue(GameState *game_state,
           void (*callback)(GameState *game_state),
           void (*start)(GameState *game_state),
           void (*processing)(const GameState *gameState, Canvas *const canvas)
) {
    if (afterDelay == NULL) {
        game_state->animationStart = game_state->last_tick;
        afterDelay = malloc(sizeof(List));
        afterDelay->callback = callback;
        afterDelay->processing = processing;
        afterDelay->start = start;
        afterDelay->next = NULL;
    } else {
        List *next = afterDelay;
        while (next->next != NULL) { next = (List *) (next->next); }
        next->next = malloc(sizeof(List));
        ((List *) next->next)->callback = callback;
        ((List *) next->next)->processing = processing;
        ((List *) next->next)->start = start;
        ((List *) next->next)->next = NULL;
    }
}

void queue_clear() {
    while (afterDelay != NULL) {
        List *f = afterDelay;
        free(f);
        afterDelay = f->next;
    }
}

void dequeue(GameState *game_state) {
    ((List *) afterDelay)->callback(game_state);
    List *f = afterDelay;
    afterDelay = (List *) afterDelay->next;
    free(f);
    if (afterDelay != NULL && afterDelay->start != NULL)afterDelay->start(game_state);
    game_state->animationStart = game_state->last_tick;
}

void animateQueue(const GameState *game_state, Canvas *const canvas) {
    if (afterDelay != NULL && ((List *) afterDelay)->processing != NULL) {
        ((List *) afterDelay)->processing(game_state, canvas);
    }
}

bool run_queue(GameState *game_state) {
    if (afterDelay != NULL) {
        game_state->animating = true;
        if ((game_state->last_tick - game_state->animationStart) > ANIMATION_TIME) {
            dequeue(game_state);
        }
        return true;
    }
    game_state->animating = false;
    return false;
}
