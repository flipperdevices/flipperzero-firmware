#pragma once

#include <toolbox/stream/buffered_file_stream.h>

struct ResourceManifestReader {
    Storage* storage;
    Stream* stream;
    FuriString* linebuf;
    ResourceManifestEntry entry;
};
