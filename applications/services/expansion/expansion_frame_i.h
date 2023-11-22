#include "expansion_frame.h"

size_t expansion_frame_get_encoded_size(const ExpansionFrame* frame);

size_t expansion_frame_get_remaining_size(const ExpansionFrame* frame, size_t received_size);
