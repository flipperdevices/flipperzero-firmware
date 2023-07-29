#pragma once

/**
 * Sends the provided evil portal index file to the ESP32.
 * @param storage - Storage instance.
 * @param path - Path of the index that will be sent to the ESP32.
 * @return Returns true if the file has been sent correctly, false otherwise.
 */
bool evil_portal_set_html(Storage *storage, const char* path);

/**
 * Reads the access point name from the storage and then send it to the
 * ESP32.
 * @param storage - Storage instance.
 * @return Returns true if ap name has been sent to correctly, false otherwise.
 */
bool evil_portal_set_ap_name(Storage *storage);
