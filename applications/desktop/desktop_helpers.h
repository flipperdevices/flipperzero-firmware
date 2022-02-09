#pragma once
#include <stdbool.h>
#include "desktop.h"

void desktop_helpers_emit_error_notification();
void desktop_helpers_lock_system(Desktop* desktop, bool hard_lock);
void desktop_helpers_unlock_system(Desktop* desktop);

