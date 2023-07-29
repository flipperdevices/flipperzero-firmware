#pragma once

/**
 * Sends the specified evil portal index file to the ESP32.
 * @param storage - The storage instance to use.
 * @param path - The path of the index file that will be sent to the ESP32.
 * @return Returns true if the file has been sent successfully, false otherwise.
 */
bool evil_portal_set_html(Storage *storage, const char *path);

/**
 * Reads the access point name from the storage and sends it to the ESP32.
 * @param storage - The storage instance to use.
 * @param ap_config_path - The path of the configuration file that contains the access point (AP) name
 * to be sent to the ESP32.
 * @return Returns true if the AP name has been sent correctly, false otherwise.
 */
bool evil_portal_set_ap_name(Storage *storage, const char *ap_config_path);
