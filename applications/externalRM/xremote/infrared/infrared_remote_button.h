/*
   This file was taken from the project: 
   https://github.com/DarkFlippers/unleashed-firmware

   The original project is licensed under the GNU GPLv3
   No modifications were made to this file.
*/

#pragma once

#include "infrared_signal.h"

typedef struct InfraredRemoteButton InfraredRemoteButton;

InfraredRemoteButton* infrared_remote_button_alloc();
void infrared_remote_button_free(InfraredRemoteButton* button);

void infrared_remote_button_set_name(InfraredRemoteButton* button, const char* name);
const char* infrared_remote_button_get_name(InfraredRemoteButton* button);

void infrared_remote_button_set_signal(InfraredRemoteButton* button, InfraredSignal* signal);
InfraredSignal* infrared_remote_button_get_signal(InfraredRemoteButton* button);
