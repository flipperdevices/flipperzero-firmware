#pragma once

#include "tracker_engine.h"
#include "tracker_engine_defs.h"
#include <stdbool.h>
#include <stdio.h>
#include <storage/storage.h>
#include <toolbox/stream/file_stream.h>

bool load_song(TrackerSong* song, Stream* stream);
bool load_instrument(Instrument* inst, Stream* stream);
void load_instrument_inner(Stream* stream, Instrument* inst, uint8_t version);