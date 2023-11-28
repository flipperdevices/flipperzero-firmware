#include "kml.h"

#define TAG "kml"

bool kml_open_file(Storage* storage, KMLFile* kml, const char* path) {
    kml->file = storage_file_alloc(storage);
    FS_Error e = storage_file_open(kml->file, path, FSAM_WRITE, FSOM_CREATE_ALWAYS);
    if(e != FSE_OK) {
	// must call close() even if the operation fails
        FURI_LOG_E(TAG, "failed to open KML file %s: %s", path, filesystem_api_error_get_desc(e));
	storage_file_close(kml->file);
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

    e = storage_file_write(kml->file, kml_intro, strlen(kml_intro));
    if(e != FSE_OK) {
	FURI_LOG_E(TAG, "failed to write KML starting header! error %d", e);
        storage_file_close(kml->file);
        storage_file_free(kml->file);
        return false;
    }

    // keeps track of writes for periodic flushes
    kml->write_counter = 0;
    FURI_LOG_I(TAG, "file opened successfully");
    return true;
}

bool kml_add_path_point(KMLFile* kml, double lat, double lon, uint32_t alt) {
    // KML is longitude then latitude for some reason
    FuriString* point = furi_string_alloc_printf("          %f,%f,%lu\n", lon, lat, alt);
    FS_Error e = storage_file_write(kml->file, furi_string_get_cstr(point), furi_string_size(point));
    if(e != FSE_OK) {
	FURI_LOG_E(TAG, "failed to write line, error %s (%d)", filesystem_api_error_get_desc(e), e);
        return false;
    }

    furi_string_free(point);

    kml->write_counter += 1;
    if (kml->write_counter == 16) {
	if (!storage_file_sync(kml->file)) {
	    FURI_LOG_E(TAG, "failed to periodic flush file!");
	}
	// reset
	kml->write_counter = 0;
    }
    
    return true;
}

bool kml_close_file(KMLFile* kml) {
    const char* kml_outro = "        </coordinates>\n"
                            "      </LineString>\n"
                            "    </Placemark>\n"
                            "  </Document>\n"
                            "</kml>";

    FS_Error e = storage_file_write(kml->file, kml_outro, strlen(kml_outro));
    if(e != FSE_OK) {
	FURI_LOG_E(TAG, "failed to close file, error %s (%d)", filesystem_api_error_get_desc(e), e);
        storage_file_close(kml->file);
        storage_file_free(kml->file);
        return false;
    }

    storage_file_close(kml->file);
    storage_file_free(kml->file);

    return true;
}
