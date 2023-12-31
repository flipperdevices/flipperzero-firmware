#include "load.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi/core/string.h>

#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

LevelData* alloc_level_data() {
    LevelData* ld = malloc(sizeof(LevelData));
    ld->solution = furi_string_alloc();
    ld->board = furi_string_alloc();
    ld->title = furi_string_alloc();
    ld->gamePar = 0;
    return ld;
}

void free_level_data(LevelData* ld) {
    furi_string_free(ld->solution);
    furi_string_free(ld->board);
    furi_string_free(ld->title);
    free(ld);
}

LevelSet* alloc_level_set() {
    LevelSet* ls = malloc(sizeof(LevelSet));
    ls->title = furi_string_alloc();
    ls->author = furi_string_alloc();
    ls->description = furi_string_alloc();
    ls->file = furi_string_alloc();
    ls->maxLevel = 0;
    return ls;
}

void free_level_set(LevelSet* ls) {
    furi_string_free(ls->title);
    furi_string_free(ls->author);
    furi_string_free(ls->description);
    furi_string_free(ls->file);
    free(ls);
}

int load_level_row(uint8_t* pb, const char* psz, const char* pszMax) {
    int cBlocks = 0;
    for(; psz < pszMax; psz++) {
        char ch = *psz;

        // Is this a number (non-moveable blocks?)

        int c = 0;
        while(ch >= '0' && ch <= '9') {
            c = c * 10 + ch - '0';
            psz++;
            if(psz >= pszMax) break;
            ch = *psz;
        }
        if(c != 0) {
            cBlocks += c;
            if(pb != NULL) {
                while(c != 0) {
                    *pb++ = 9;
                    c--;
                }
            }
            psz--;
            continue;
        }

        // Is this empty space?

        if(ch == '~') {
            cBlocks++;
            if(pb != NULL) *pb++ = 0;
            continue;
        }

        // This is a block type. Remember it verbatim

        if(ch < 'a' || ch > 'h') return -1;

        cBlocks++;
        if(pb != NULL) *pb++ = ch - 'a' + 1;
    }

    return cBlocks;
}

bool parse_level_notation(const char* pszLevel, PlayGround* level) {
    uint8_t* pbLoad;

    // Validate the board and get the dimensions

    const char* pszLast = pszLevel;
    bool fLoop = true;
    int cRows = 0;
    int cCols = -1;
    while(fLoop) {
        int cColsT;
        const char* pszNext = strchr(pszLast, '/');
        if(pszNext == NULL) {
            pszNext = pszLast + strlen(pszLast);
            fLoop = false;
        }
        cColsT = load_level_row(NULL, pszLast, pszNext);
        if(cCols == -1) {
            cCols = cColsT;
        } else if(cCols != cColsT) {
            return false;
        }
        cRows++;
        pszLast = pszNext + 1;
    }

    // Vexed wants these sizes

    if(cCols != SIZE_X || cRows != SIZE_Y) return false;

    // Load it this time

    memset(level, '\0', sizeof(uint8_t) * SIZE_X * SIZE_Y);

    pbLoad = level[0][0];
    pszLast = pszLevel;
    fLoop = true;
    while(fLoop) {
        // Find the end of this row

        const char* pszNext = strchr(pszLast, '/');
        if(pszNext == NULL) {
            pszNext = pszLast + strlen(pszLast);
            fLoop = false;
        }

        // Load the row

        load_level_row(pbLoad, pszLast, pszNext);

        // Next row...

        pbLoad += SIZE_X;
        pszLast = pszNext + 1;
    }

    return true;
}

bool load_level(Storage* storage, const char* path, int level, LevelData* levelData) {
    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();
    FuriString* value;
    bool loaded = false;

    if(file_stream_open(stream, path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        while(stream_read_line(stream, line)) {
            if(furi_string_start_with(line, "#")) continue;
            size_t level_no_sep = furi_string_search_char(line, ';', 0);
            if(level_no_sep == FURI_STRING_FAILURE) continue;
            size_t level_name_sep = furi_string_search_char(line, ';', level_no_sep + 1);
            if(level_name_sep == FURI_STRING_FAILURE) continue;
            size_t level_board_sep = furi_string_search_char(line, ';', level_name_sep + 1);
            if(level_board_sep == FURI_STRING_FAILURE) continue;

            value = furi_string_alloc_set(line);
            furi_string_left(value, level_no_sep);
            const char* value_raw = furi_string_get_cstr(value);
            int levelNo = atoi(value_raw);
            furi_string_free(value);

            if(levelNo == level) {
                loaded = true;

                furi_string_free(levelData->title);
                levelData->title = furi_string_alloc_set(line);
                furi_string_left(levelData->title, level_name_sep);
                furi_string_right(levelData->title, level_no_sep + 1);
                furi_string_trim(levelData->title, "\n\r\t");

                furi_string_free(levelData->board);
                levelData->board = furi_string_alloc_set(line);
                furi_string_left(levelData->board, level_board_sep);
                furi_string_right(levelData->board, level_name_sep + 1);
                furi_string_trim(levelData->board, "\n\r\t");

                furi_string_free(levelData->solution);
                levelData->solution = furi_string_alloc_set(line);
                furi_string_right(levelData->solution, level_board_sep + 1);
                furi_string_trim(levelData->solution, "\n\r\t");

                levelData->gamePar = strlen(furi_string_get_cstr(levelData->solution)) / 2;

                FURI_LOG_I(TAG, "LEVEL TITLE \"%s\"", furi_string_get_cstr(levelData->title));
                FURI_LOG_D(TAG, "LEVEL BOARD \"%s\"", furi_string_get_cstr(levelData->board));
                FURI_LOG_D(
                    TAG, "LEVEL SOLUTION \"%s\"", furi_string_get_cstr(levelData->solution));

                break;
            } else {
                continue;
            }
        }

        furi_string_free(line);
        file_stream_close(stream);
        stream_free(stream);

        return loaded;
    } else {
        return false;
    }
}

bool load_level_set(Storage* storage, const char* path, LevelSet* levelSet) {
    UNUSED(storage);
    UNUSED(path);
    levelSet->maxLevel = 20;
    return true;
}