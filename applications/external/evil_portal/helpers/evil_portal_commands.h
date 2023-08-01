#pragma once

/**
 * Sends the specified evil portal index file to the ESP32.
 * @param storage - The storage instance to use.
 * @param path - The path of the index file that will be sent to the ESP32.
 * @return Returns true if the file has been sent successfully, false otherwise.
 */
bool evil_portal_set_html(Storage* storage, const char* path);

/**
 * Sets the access point name that will be created by the ESP32.
 * @param ap_name - The access point (AP) name to be sent to the ESP32.
 * @return Returns true if the AP name has been sent correctly, false otherwise.
 */
bool evil_portal_set_ap_name(const char* ap_name);
