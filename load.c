#include "load.h"

#include <furi.h>
#include <furi_hal.h>
#include <furi/core/string.h>

#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

// i overwrite it because this: https://github.com/flipperdevices/flipperzero-firmware/blob/a7b60bf2a610e1a364d26a925f3713c08d16d49c/applications/services/storage/storage_processing.c#L530
// gets me gui thread instead of app thread

#define MY_APP_DATA_PATH(path)  \
    "/ext/apps_data/game_vexed" \
    "/" path

char* assetLevels[] = {
    "01 Classic Levels",
    "02 Classic Levels 2",
    "03 Childrens Pack",
    "04 Confusion Pack",
    "05 Impossible Pack",
    "06 Panic Pack",
    "07 Twister Levels",
    "08 Variety Pack",
    "09 Variety II Pack"};

//-----------------------------------------------------------------------------

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
    ls->id = furi_string_alloc();
    ls->title = furi_string_alloc();
    ls->author = furi_string_alloc();
    ls->description = furi_string_alloc();
    ls->url = furi_string_alloc();
    ls->maxLevel = 0;
    return ls;
}

void free_level_set(LevelSet* ls) {
    furi_string_free(ls->id);
    furi_string_free(ls->title);
    furi_string_free(ls->author);
    furi_string_free(ls->description);
    furi_string_free(ls->url);
    free(ls);
}

//-----------------------------------------------------------------------------

bool level_set_id_to_path(Storage* storage, FuriString* levelSetId, size_t maxSize, char* path) {
    memset(path, 0, maxSize);

    snprintf(path, maxSize - 1, "/assets/levels/%s.vxl", furi_string_get_cstr(levelSetId));
    if(storage_common_exists(storage, path)) {
        return true;
    }

    snprintf(
        path,
        maxSize - 1,
        "/ext/apps_data/game_vexed/extra_levels/%s.vxl",
        furi_string_get_cstr(levelSetId));
    if(storage_common_exists(storage, path)) {
        FURI_LOG_D(TAG, "Found extra level \"%s\"", path);
        return true;
    } else {
        FURI_LOG_E(TAG, "Extra level not found \"%s\"", path);
    }

    return false;
}

//-----------------------------------------------------------------------------

void level_set_id_to_error_path(FuriString* levelSetId, size_t maxSize, char* path) {
    memset(path, 0, maxSize);
    snprintf(
        path,
        maxSize - 1,
        "/ext/apps_data/game_vexed/extra_levels/%s.error.txt",
        furi_string_get_cstr(levelSetId));
}

//-----------------------------------------------------------------------------

void mark_set_invalid(Storage* storage, FuriString* levelSetId, FuriString* errorMsg) {
    FURI_LOG_D(
        TAG,
        "MARKING LEVEL AS BAD \"%s\" - REASON: %s",
        furi_string_get_cstr(levelSetId),
        furi_string_get_cstr(errorMsg));

    const int bufSize = 1024;
    char filePath[bufSize];

    level_set_id_to_error_path(levelSetId, bufSize, filePath);

    if(ensure_paths(storage)) {
        Stream* stream = file_stream_alloc(storage);
        if(file_stream_open(stream, filePath, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            const char* data = furi_string_get_cstr(errorMsg);
            stream_write(stream, (uint8_t*)data, strlen(data));
            file_stream_close(stream);
            stream_free(stream);
        }
    }
}

//-----------------------------------------------------------------------------

bool level_set_id_to_score_path(
    Storage* storage,
    FuriString* levelSetId,
    size_t maxSize,
    char* path) {
    memset(path, 0, maxSize);

    snprintf(
        path,
        maxSize - 1,
        "/ext/apps_data/game_vexed/scores/%s.sco",
        furi_string_get_cstr(levelSetId));
    if(storage_common_exists(storage, path)) {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

bool parse_level_notation(const char* pszLevel, PlayGround* level) {
    uint8_t* pbLoad;

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

//-----------------------------------------------------------------------------

bool load_level(
    Storage* storage,
    FuriString* levelSetId,
    int level,
    LevelData* levelData,
    FuriString* errorMsg) {
    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();
    FuriString* value;
    bool loaded = false;

    size_t errBufSize = 128;
    char errMsg[errBufSize];

    size_t bufSize = 512;
    char filePath[bufSize];

    if(!level_set_id_to_path(storage, levelSetId, bufSize, filePath)) {
        FURI_LOG_E(TAG, "LEVEL NOT FOUND! \"%s\"", filePath);
        furi_string_set(errorMsg, "Missing level file: ");
        furi_string_cat(errorMsg, filePath);
    }

    if(file_stream_open(stream, filePath, FSAM_READ, FSOM_OPEN_EXISTING)) {
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

        if(!loaded) {
            memset(errMsg, 0, errBufSize);
            snprintf(
                errMsg, errBufSize, "Cannot load level  #%u from levelset %s", level, filePath);
            furi_string_set(errorMsg, errMsg);
        }

        return loaded;
    } else {
        return false;
    }
}

//-----------------------------------------------------------------------------

// https://stackoverflow.com/a/53966346
void btox(char* xp, const char* bb, int n) {
    const char xx[] = "0123456789ABCDEF";
    while(--n >= 0) xp[n] = xx[(bb[n >> 1] >> ((1 - (n & 1)) << 2)) & 0xF];
}

//-----------------------------------------------------------------------------

void debug_dump_hex(const char* label, const char* data, int n) {
    char hexstr[n + 1];
    btox(hexstr, data, n);
    hexstr[n] = 0; /* Terminate! */
    FURI_LOG_D(TAG, "%s = %s", label, hexstr);
}

//-----------------------------------------------------------------------------

bool load_set_scores(Storage* storage, FuriString* levelSetId, LevelScore* scores) {
    bool loaded = false;
    const size_t scoreSize = sizeof(LevelScore) * MAX_LEVELS_PER_SET;
    const size_t bufSize = 512;
    char filePath[bufSize];
    memset(scores, 0, scoreSize);

    if(!level_set_id_to_score_path(storage, levelSetId, bufSize, filePath)) {
        FURI_LOG_E(TAG, "SCORES FOR LEVEL NOT FOUND! \"%s\"", filePath);
        return false;
    }

    Stream* stream = file_stream_alloc(storage);
    if(file_stream_open(stream, filePath, FSAM_READ, FSOM_OPEN_EXISTING)) {
        size_t actualyRead = stream_read(stream, (uint8_t*)scores, scoreSize);

        if(scoreSize != actualyRead) {
            FURI_LOG_E(TAG, "Error while reading scores!");
        } else {
            debug_dump_hex("Scores", (char*)scores, scoreSize << 1);
            loaded = true;
        }

        file_stream_close(stream);
        stream_free(stream);
    }

    return loaded;
}

//-----------------------------------------------------------------------------

bool save_set_scores(FuriString* levelSetId, LevelScore* scores) {
    bool saved = false;
    const size_t scoreSize = sizeof(LevelScore) * MAX_LEVELS_PER_SET;
    const size_t bufSize = 512;
    char filePath[bufSize];

    Storage* storage = furi_record_open(RECORD_STORAGE);

    if(ensure_paths(storage)) {
        //debug_dump_hex("Scores to write", (char*)scores, scoreSize << 1);

        if(!level_set_id_to_score_path(storage, levelSetId, bufSize, filePath)) {
            FURI_LOG_D(TAG, "Writing new scores \"%s\"", filePath);
        } else {
            FURI_LOG_D(TAG, "Overwriting scores \"%s\"", filePath);
        }

        Stream* stream = file_stream_alloc(storage);
        if(file_stream_open(stream, filePath, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            size_t actualyWrote = stream_write(stream, (uint8_t*)scores, scoreSize);

            if(scoreSize != actualyWrote) {
                FURI_LOG_E(TAG, "Error while writing scores!");
            } else {
                saved = true;
            }

            file_stream_close(stream);
            stream_free(stream);
        }
    }

    furi_record_close(RECORD_STORAGE);

    return saved;
}

//-----------------------------------------------------------------------------

bool load_level_set(
    Storage* storage,
    FuriString* levelSetId,
    LevelSet* levelSet,
    FuriString* errorMsg) {
    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();
    FuriString* value;
    bool loaded = true;
    uint8_t levelCount = 0;
    size_t sep, level_no_sep, level_name_sep, level_board_sep;

    memset(levelSet->pars, 0, sizeof(uint8_t) * MAX_LEVELS_PER_SET);

    size_t errBufSize = 128;
    char errMsg[errBufSize];

    const size_t bufSize = 512;
    char filePath[bufSize];

    if(!level_set_id_to_path(storage, levelSetId, bufSize, filePath)) {
        FURI_LOG_E(TAG, "LEVEL NOT FOUND! \"%s\"", filePath);
        furi_string_set(errorMsg, "Missing level file: ");
        furi_string_cat(errorMsg, filePath);
        loaded = false;
    }

    load_set_scores(storage, levelSetId, levelSet->scores);

    furi_string_set(levelSet->id, levelSetId);
    furi_string_set(levelSet->title, levelSetId);

    int lineNo = 0;

    if(file_stream_open(stream, filePath, FSAM_READ, FSOM_OPEN_EXISTING)) {
        while(stream_read_line(stream, line)) {
            lineNo++;
            if(furi_string_start_with(line, "#")) {
                //size_t url_sep = furi_string_search(line, "URL:", 1);
                //size_t descr_sep = furi_string_search(line, "Description:", 1);

                sep = furi_string_search(line, "Author:", 1);
                if(sep != FURI_STRING_FAILURE) {
                    value = furi_string_alloc_set(line);
                    furi_string_right(value, sep + 7);
                    furi_string_trim(value, " \t\n\r");
                    FURI_LOG_D(TAG, "AUTHOR \"%s\"", furi_string_get_cstr(value));
                    furi_string_set(levelSet->author, value);
                    furi_string_free(value);
                }

                sep = furi_string_search(line, "URL:", 1);
                if(sep != FURI_STRING_FAILURE) {
                    value = furi_string_alloc_set(line);
                    furi_string_right(value, sep + 4);
                    furi_string_trim(value, " \t\n\r");
                    FURI_LOG_D(TAG, "URL \"%s\"", furi_string_get_cstr(value));
                    furi_string_set(levelSet->url, value);
                    furi_string_free(value);
                }

                sep = furi_string_search(line, "Description:", 1);
                if(sep != FURI_STRING_FAILURE) {
                    value = furi_string_alloc_set(line);
                    furi_string_right(value, sep + 12);
                    furi_string_trim(value, " \t\n\r");
                    FURI_LOG_D(TAG, "DESCR \"%s\"", furi_string_get_cstr(value));
                    furi_string_set(levelSet->description, value);
                    furi_string_free(value);
                }

                continue;
            }

            level_no_sep = furi_string_search_char(line, ';', 0);
            if(level_no_sep == FURI_STRING_FAILURE) {
                loaded = false;
                memset(errMsg, 0, errBufSize);
                snprintf(
                    errMsg,
                    errBufSize,
                    "Invalid levelset format %s - missing level nr. at line no: %d",
                    filePath,
                    lineNo);
                furi_string_set(errorMsg, errMsg);
                continue;
            }
            level_name_sep = furi_string_search_char(line, ';', level_no_sep + 1);
            if(level_name_sep == FURI_STRING_FAILURE) {
                loaded = false;
                memset(errMsg, 0, errBufSize);
                snprintf(
                    errMsg,
                    errBufSize,
                    "Invalid levelset format %s - missing board data at line no: %d",
                    filePath,
                    lineNo);
                furi_string_set(errorMsg, errMsg);
                continue;
            };
            level_board_sep = furi_string_search_char(line, ';', level_name_sep + 1);
            if(level_board_sep == FURI_STRING_FAILURE) {
                loaded = false;
                memset(errMsg, 0, errBufSize);
                snprintf(
                    errMsg,
                    errBufSize,
                    "Invalid levelset format %s - missing solution at line no: %d",
                    filePath,
                    lineNo);
                furi_string_set(errorMsg, errMsg);
                continue;
            };

            value = furi_string_alloc_set(line);
            furi_string_left(value, level_no_sep);
            const char* value_raw = furi_string_get_cstr(value);
            int levelNo = atoi(value_raw);
            furi_string_free(value);

            if(levelNo < MAX_LEVELS_PER_SET) {
                value = furi_string_alloc_set(line);
                furi_string_right(value, level_board_sep + 1);
                furi_string_trim(value, "\n\r\t");
                levelSet->pars[levelCount] = (furi_string_size(value) / 2) % 256;
                furi_string_free(value);

                levelCount++;
            }
        }

        debug_dump_hex("Pars", (char*)levelSet->pars, sizeof(levelSet->pars) << 1);

        furi_string_free(line);
        file_stream_close(stream);
        stream_free(stream);

        levelSet->maxLevel = levelCount;

        return loaded;
    } else {
        memset(errMsg, 0, errBufSize);
        snprintf(errMsg, errBufSize, "Cannot read file %s", filePath);
        furi_string_set(errorMsg, errMsg);
        return false;
    }
}

//-----------------------------------------------------------------------------

void load_all(File* f, FuriString* target) {
    size_t bufSize = 256;
    size_t bytesRead;
    char buf[bufSize];

    do {
        memset(buf, 0, sizeof(buf));
        bytesRead = storage_file_read(f, buf, bufSize - 1);
        FURI_LOG_D(TAG, "Bufer read: %s", buf);
        if(bytesRead > 0) {
            furi_string_cat_str(target, buf);
            FURI_LOG_D(TAG, "Furi: %s", furi_string_get_cstr(target));
        }
    } while(bytesRead > 0);
}

//-----------------------------------------------------------------------------

bool load_last_level(FuriString* lastLevelSetId, uint8_t* levelNo) {
    FuriString* fbuf = furi_string_alloc();
    size_t pos;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    bool loaded = false;

    if(!storage_common_exists(storage, MY_APP_DATA_PATH("scores/game.txt"))) {
        FURI_LOG_I(TAG, "No scores file (yet)");
        return false;
    }

    if(!storage_file_open(
           file, MY_APP_DATA_PATH("scores/game.txt"), FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Failed to open file");
        return false;
    }

    load_all(file, fbuf);
    FURI_LOG_T(TAG, "Loaded scores: %s", furi_string_get_cstr(fbuf));

    if(furi_string_size(fbuf) > 0) {
        pos = furi_string_search_char(fbuf, '\n', 0);
        if(pos == FURI_STRING_FAILURE) {
            FURI_LOG_E(TAG, "ERROR - no file ID in scores file");
        } else {
            furi_string_set_n(lastLevelSetId, fbuf, 0, pos);
            furi_string_trim(lastLevelSetId);
            furi_string_right(fbuf, pos + 1);

            FURI_LOG_T(
                TAG,
                "AFTER LEVEL LOAD: [%s] [%s]",
                furi_string_get_cstr(lastLevelSetId),
                furi_string_get_cstr(fbuf));

            pos = furi_string_search_char(fbuf, '\n', 0);
            if(pos == FURI_STRING_FAILURE) {
                FURI_LOG_E(TAG, "ERROR - no level NO in scores file");
            } else {
                furi_string_left(fbuf, pos);
                int levelRead = atoi(furi_string_get_cstr(fbuf));
                *levelNo = (uint8_t)(levelRead % 256);
                loaded = true;
            }
        }
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    furi_string_free(fbuf);
    return loaded;
}

//-----------------------------------------------------------------------------

bool ensure_paths(Storage* storage) {
    if(!storage_common_exists(storage, "/ext/apps_data")) {
        if(storage_common_mkdir(storage, "/ext/apps_data/") != FSE_OK) {
            FURI_LOG_E(TAG, "Cannot created /ext/apps_data/ dir");
            return false;
        }
    }

    if(!storage_common_exists(storage, "/ext/apps_data/game_vexed")) {
        if(storage_common_mkdir(storage, "/ext/apps_data/game_vexed") != FSE_OK) {
            FURI_LOG_E(TAG, "Cannot created /ext/apps_data/game_vexed dir");
            return false;
        }
    }

    if(!storage_common_exists(storage, MY_APP_DATA_PATH("scores"))) {
        if(storage_common_mkdir(storage, MY_APP_DATA_PATH("scores")) != FSE_OK) {
            FURI_LOG_E(TAG, "Cannot created scored data dir");
            return false;
        }
    }

    if(!storage_common_exists(storage, MY_APP_DATA_PATH("extra_levels"))) {
        if(storage_common_mkdir(storage, MY_APP_DATA_PATH("extra_levels")) != FSE_OK) {
            FURI_LOG_E(TAG, "Cannot create dir for extra_levels");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------

bool save_last_level(FuriString* lastLevelSetId, uint8_t levelNo) {
    const int bufSize = 256;
    char buf[bufSize];
    memset(buf, 0, bufSize);
    snprintf(buf, sizeof(buf), "%s\n%u\n", furi_string_get_cstr(lastLevelSetId), levelNo);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(ensure_paths(storage)) {
        File* file = storage_file_alloc(storage);

        if(!storage_file_open(
               file, MY_APP_DATA_PATH("scores/game.txt"), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            FURI_LOG_E(TAG, "Failed to open file");
            return false;
        }
        if(!storage_file_write(file, buf, strlen(buf))) {
            FURI_LOG_E(TAG, "Failed to write to file");
            return false;
        }
        storage_file_close(file);
        storage_file_free(file);
    }
    furi_record_close(RECORD_STORAGE);
    return true;
}

//-----------------------------------------------------------------------------

void delete_progress(LevelScore* scores) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(storage_common_exists(storage, MY_APP_DATA_PATH("scores/game.txt"))) {
        storage_simply_remove_recursive(storage, MY_APP_DATA_PATH("scores"));
    }
    furi_record_close(RECORD_STORAGE);
    memset(scores, 0, sizeof(LevelScore) * MAX_LEVELS_PER_SET);
}

//-----------------------------------------------------------------------------

void init_level_list(LevelList* ls, int capacity) {
    ls->count = capacity;
    if(capacity > 0) {
        ls->ids = malloc(sizeof(FuriString*) * capacity);
    } else {
        ls->ids = NULL;
    }
}

//-----------------------------------------------------------------------------

void free_level_list(LevelList* ls) {
    if(ls->count > 0) {
        for(int i = 0; i < ls->count; i++) {
            furi_string_free(ls->ids[i]);
        }
        free(ls->ids);
        ls->ids = NULL;
    }
}

//-----------------------------------------------------------------------------

void list_extra_levels(Storage* storage, LevelList* levelList) {
    const int bufSize = 1024;
    char buf[bufSize];
    char buf2[bufSize];
    bool fileFound = false;
    FileInfo fileinfo;
    File* file = storage_file_alloc(storage);
    FuriString* levelId;
    size_t levelCount = 0, levelNo = 0;
    if(storage_dir_open(file, MY_APP_DATA_PATH("extra_levels"))) {
        do {
            memset(buf, 0, bufSize);
            fileFound = storage_dir_read(file, &fileinfo, buf, bufSize);
            if(fileFound) {
                levelId = furi_string_alloc_set(buf);
                if(furi_string_end_with_str(levelId, ".vxl")) {
                    furi_string_left(levelId, furi_string_size(levelId) - 4);
                    memset(buf2, 0, bufSize);
                    level_set_id_to_error_path(levelId, bufSize, buf2);
                    if(!storage_common_exists(storage, buf2)) levelCount++;
                }
                furi_string_free(levelId);
            }
        } while(fileFound);

        if(levelCount > 0) {
            init_level_list(levelList, levelCount);

            storage_dir_close(file);
            storage_file_free(file);

            file = storage_file_alloc(storage);
            storage_dir_open(file, MY_APP_DATA_PATH("extra_levels"));

            do {
                memset(buf, 0, bufSize);
                fileFound = storage_dir_read(file, &fileinfo, buf, bufSize);
                if(fileFound) {
                    levelId = furi_string_alloc_set(buf);
                    if(furi_string_end_with_str(levelId, ".vxl")) {
                        furi_string_left(levelId, furi_string_size(levelId) - 4);
                        memset(buf2, 0, bufSize);
                        level_set_id_to_error_path(levelId, bufSize, buf2);
                        if(!storage_common_exists(storage, buf2)) {
                            FURI_LOG_D(
                                TAG, "EXTRA LEVEL FILE \"%s\"", furi_string_get_cstr(levelId));
                            levelList->ids[levelNo] = furi_string_alloc_set(levelId);
                            levelNo++;
                        } else {
                            FURI_LOG_W(
                                TAG,
                                "CANNOT LOAD LEVEL \"%s\" - error file exists at %s",
                                furi_string_get_cstr(levelId),
                                buf2);
                        }
                    }
                    furi_string_free(levelId);
                }
            } while(fileFound);
        }
    }

    storage_dir_close(file);
    storage_file_free(file);
}