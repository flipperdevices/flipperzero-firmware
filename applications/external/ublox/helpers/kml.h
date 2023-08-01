#pragma once

#include <furi.h>
#include <storage/storage.h>

#include "../helpers/ublox_types.h"

typedef struct KMLFile {
    File* file;
} KMLFile;

/**
 * Open a KML file and write out initial XML tags for list of points in path.
 * This assumes that `path` is a valid and complete Flipper filesystem path.
 */
bool kml_open_file(Storage* storage, KMLFile* kml, const char* path);

bool kml_add_path_point(KMLFile* kml, double lat, double lon, uint32_t alt);

bool kml_close_file(KMLFile* kml);
