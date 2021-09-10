/* This fakes the situation where other.proto is not found for some reason
 * by making its size indefinite, but then defining it here manually. */

#define SecondOneof_size 5
#include "oneof.pb.h"
