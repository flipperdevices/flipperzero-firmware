#include "stats.h"
#include "utils.h"

Stats* alloc_stats() {
    Stats* stats = malloc(sizeof(Stats));
    stats->bricksNonZero = furi_string_alloc();
    return stats;
}

void free_stats(Stats* stats) {
    furi_string_free(stats->bricksNonZero);
    free(stats);
}

void update_board_stats(PlayGround* pg, Stats* stats) {
    memset(stats->ofBrick, '\0', sizeof(stats->ofBrick));
    char buff[2];
    memset(buff, '\0', sizeof(buff));

    uint8_t x, y, i, tile;
    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            tile = (*pg)[y][x];
            if(is_block(tile)) {
                stats->ofBrick[tile]++;
            }
        }
    }

    memset(stats->statsNonZero, 0, sizeof(stats->statsNonZero));
    furi_string_reset(stats->bricksNonZero);

    for(i = 0; i < WALL_TILE; i++) {
        if(stats->ofBrick[i] > 0) {
            buff[0] = i;
            furi_string_cat_str(stats->bricksNonZero, buff);
            stats->statsNonZero[furi_string_size(stats->bricksNonZero) - 1] = stats->ofBrick[i];
        }
    }

    FURI_LOG_D(
        TAG,
        "Stats, bricks [%s] counts [%s]",
        furi_string_get_cstr(stats->bricksNonZero),
        (const char*)stats->statsNonZero);
}