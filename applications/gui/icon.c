#include "icon.h"

#include <stdint.h>

struct Icon {
	uint8_t			width;
	uint8_t			height;
	uint8_t			frame_count;
	uint8_t			frame_rate;
	const void		*frames;
};

const Icon * icon_get() {

}
