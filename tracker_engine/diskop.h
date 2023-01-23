#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <toolbox/stream/file_stream.h>
#include <storage/storage.h>
#include "tracker_engine_defs.h"
#include "tracker_engine.h"

bool load_song(TrackerSong* song, Stream* stream);