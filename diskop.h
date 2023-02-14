#pragma once

#include "flizzer_tracker.h"
#include "tracker_engine/diskop.h"

#define INST_FILE_SIG "FZT!INST"

bool save_song(FlizzerTrackerApp* tracker, FuriString* filepath);

bool load_song_util(FlizzerTrackerApp* tracker, FuriString* filepath);

void save_config(FlizzerTrackerApp* tracker);
void load_config(FlizzerTrackerApp* tracker);