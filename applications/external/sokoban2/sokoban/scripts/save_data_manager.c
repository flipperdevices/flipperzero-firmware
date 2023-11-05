#include "save_data_manager.h"
#include "wave/files/file_lines_reader.h"

static const char* DATABASE_PATH = APP_ASSETS_PATH("database.txt");
static const char* SAVE_DATA_PATH = APP_DATA_PATH("sokoban.save");

LevelsDatabase* levels_database_alloc(int collectionsCount)
{
    LevelsDatabase* levelsMetadata = malloc(sizeof(LevelsDatabase));
    levelsMetadata->collectionsCount = collectionsCount;
    levelsMetadata->collections = malloc(collectionsCount * sizeof(LevelsCollection));
    return levelsMetadata;
}

void levels_database_free(LevelsDatabase* levelsMetadata)
{
    for (int i = 0; i < levelsMetadata->collectionsCount; i++)
        free(levelsMetadata->collections[i].levels);
    free(levelsMetadata->collections);
    free(levelsMetadata);
}

LevelsDatabase* levels_database_load()
{
    Storage* storage = furi_record_open(RECORD_STORAGE);
    const char* path = DATABASE_PATH;

    const int MAX_LINE_LENGTH = 256;
    LevelsDatabase* levelsMetadata = NULL;

    File* file = storage_file_alloc(storage);
    if (storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING))
    {
        FileLinesReader* reader = file_lines_reader_alloc(file, MAX_LINE_LENGTH);

        char line[MAX_LINE_LENGTH];
        file_lines_reader_readln(reader, line, MAX_LINE_LENGTH);
        if (strcmp(line, "1") == 0)
        {
            file_lines_reader_readln(reader, line, MAX_LINE_LENGTH);
            FURI_LOG_D("GAME", "Collections count: %s", line);
            int collectionsCount = atoi(line);

            levelsMetadata = levels_database_alloc(collectionsCount);

            for (int collectionIndex = 0; collectionIndex < collectionsCount; collectionIndex++)
            {
                LevelsCollection collection;

                file_lines_reader_readln(reader, line, MAX_LINE_LENGTH);
                strcpy(collection.name, line);
                FURI_LOG_D("GAME", "Collection name: %s", collection.name);

                file_lines_reader_readln(reader, line, MAX_LINE_LENGTH);
                FURI_LOG_D("GAME", "Levels count: %s", line);
                int levelsCount = atoi(line);
                collection.levelsCount = levelsCount;
                collection.levels = malloc(levelsCount * sizeof(LevelItem));

                for (int levelInCollectionIndex = 0; levelInCollectionIndex < levelsCount; levelInCollectionIndex++)
                {
                    LevelItem levelItem;
                    file_lines_reader_readln(reader, line, MAX_LINE_LENGTH);
                    levelItem.worldBest = atoi(line);
                    // player best is read elsewhere

                    collection.levels[levelInCollectionIndex] = levelItem;
                }

                levelsMetadata->collections[collectionIndex] = collection;
                FURI_LOG_D("GAME", "Loaded %d levels metadata for collection %d.", levelsCount, collectionIndex);
            }

            FURI_LOG_D("GAME", "Loaded %d collections metadata.", collectionsCount);
        }
        else
        {
            FURI_LOG_D("GAME", "Unsupported levels metadata version: %s", line);
        }

        file_lines_reader_free(reader);
    }
    else
    {
        FURI_LOG_E("GAME", "Failed to open levels metadata file: %s", path);
    }
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    return levelsMetadata;
}

void levels_database_save_player_progress(LevelsDatabase* database)
{
    Storage* storage = furi_record_open(RECORD_STORAGE);
    const char* path = SAVE_DATA_PATH;

    File* file = storage_file_alloc(storage);
    if (storage_file_open(file, path, FSAM_WRITE, FSOM_CREATE_ALWAYS))
    {
        unsigned char formatVersion = '1';
        storage_file_write(file, &formatVersion, 1);
        storage_file_write(file, "\n", 1);

        for (int collectionIndex = 0; collectionIndex < database->collectionsCount; collectionIndex++)
        {
            LevelsCollection collection = database->collections[collectionIndex];
            for (int levelInCollectionIndex = 0; levelInCollectionIndex < collection.levelsCount; levelInCollectionIndex++)
            {
                LevelItem levelItem = collection.levels[levelInCollectionIndex];
                char line[32];
                snprintf(line, 32, "%d", levelItem.playerBest);
                storage_file_write(file, line, strlen(line));
                storage_file_write(file, "\n", 1);
            }
        }
    }
    else
    {
        FURI_LOG_E("GAME", "Failed to open file to save progress: %s", path);
    }

    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

void levels_database_load_player_progress(LevelsDatabase* database)
{
    Storage* storage = furi_record_open(RECORD_STORAGE);
    const char* path = SAVE_DATA_PATH;

    File* file = storage_file_alloc(storage);
    if (storage_file_open(file, path, FSAM_READ, FSOM_OPEN_EXISTING))
    {
        FileLinesReader* reader = file_lines_reader_alloc(file, 32);

        char line[32];
        file_lines_reader_readln(reader, line, 32);

        if (strcmp(line, "1") == 0)
        {
            for (int collectionIndex = 0; collectionIndex < database->collectionsCount; collectionIndex++)
            {
                LevelsCollection* collection = &database->collections[collectionIndex];
                for (int levelInCollectionIndex = 0; levelInCollectionIndex < collection->levelsCount; levelInCollectionIndex++)
                {
                    LevelItem levelItem = collection->levels[levelInCollectionIndex];
                    file_lines_reader_readln(reader, line, 32);
                    levelItem.playerBest = atoi(line);

                    collection->levels[levelInCollectionIndex] = levelItem;
                }
            }
        }
        else
        {
            FURI_LOG_D("GAME", "Unsupported player progress version: %s", line);
        }

        file_lines_reader_free(reader);
    }
    else
    {
        FURI_LOG_D("GAME", "Could not open file to load progress: %s", path);
    }

    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}