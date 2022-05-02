#pragma once

#include <stdbool.h>

typedef struct MusicPlayerWorker MusicPlayerWorker;

MusicPlayerWorker* music_player_worker_alloc();

void music_player_worker_free(MusicPlayerWorker* instance);

bool music_player_worker_load(MusicPlayerWorker* instance, const char* file_path);

void music_player_worker_start(MusicPlayerWorker* instance);

void music_player_worker_stop(MusicPlayerWorker* instance);
