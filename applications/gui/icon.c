#include "icon.h"
#include "icon_i.h"

uint8_t icon_get_width(const Icon* icon) {
	return icon->width;
}

uint8_t icon_get_height(const Icon* icon) {
	return icon->height;
}

const uint8_t * icon_get_data(const Icon* icon) {
	return *icon->data;
}