#pragma once

#include "../spotify_remote.h"

void save_ssid(const char* ssid);
void send_ssid(void* context);
void save_password(const char* password);
void send_password(void* context);