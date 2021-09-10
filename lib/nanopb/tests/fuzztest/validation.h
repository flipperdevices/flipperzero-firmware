/* This module validates that the message structures are in valid state
 * after decoding the input data. */
 
#ifndef VALIDATION_H
#define VALIDATION_H

#include <pb.h>

void validate_message(const void *msg, size_t structsize, const pb_msgdesc_t *msgtype);

#endif

