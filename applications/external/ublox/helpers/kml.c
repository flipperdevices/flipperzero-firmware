#include "kml.h"

#define TAG "kml"

bool kml_open_file(Storage* storage, KMLFile* kml, const char* path) {
    kml->file = storage_file_alloc(storage);
    if(!storage_file_open(kml->file, path, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(TAG, "failed to open KML file %s", path);
        storage_file_free(kml->file);
        return false;
    }

    // with the file opened, we need to write the intro KML tags
    const char* kml_intro = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                            "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
                            "  <Document>\n"
                            "    <name>Paths</name>\n"
                            "    <Style id=\"yellowLineGreenPoly\">\n"
                            "      <LineStyle>\n"
                            "        <color>7f00ffff</color>\n"
                            "        <width>4</width>\n"
                            "      </LineStyle>\n"
                            "      <PolyStyle>\n"
                            "        <color>7f00ff00</color>\n"
                            "      </PolyStyle>\n"
                            "    </Style>\n"
                            "    <Placemark>\n"
                            "      <name>Path 1</name>\n"
                            "      <description>Path 1</description>\n"
                            "      <styleUrl>#yellowLineGreenPoly</styleUrl>\n"
                            "      <LineString>\n"
                            "        <tessellate>1</tessellate>\n"
                            "        <extrude>1</extrude>\n"
                            "        <altitudeMode>absolute</altitudeMode>\n"
                            "        <coordinates>\n";

    if(!storage_file_write(kml->file, kml_intro, strlen(kml_intro))) {
        storage_file_close(kml->file);
        storage_file_free(kml->file);
        return false;
    }

    return true;
}

bool kml_add_path_point(KMLFile* kml, double lat, double lon, uint32_t alt) {
    // KML is longitude then latitude for some reason
    FuriString* point = furi_string_alloc_printf("          %f,%f,%lu\n", lon, lat, alt);
    if(!storage_file_write(kml->file, furi_string_get_cstr(point), furi_string_size(point))) {
        return false;
    }

    return true;
}

bool kml_close_file(KMLFile* kml) {
    const char* kml_outro = "        </coordinates>\n"
                            "      </LineString>\n"
                            "    </Placemark>\n"
                            "  </Document>\n"
                            "</kml>";

    if(!storage_file_write(kml->file, kml_outro, strlen(kml_outro))) {
        storage_file_close(kml->file);
        storage_file_free(kml->file);
        return false;
    }

    storage_file_close(kml->file);
    storage_file_free(kml->file);

    return true;
}
