#pragma once
#include <stdlib.h>

int getRandomIntInRange(int lower, int upper) {
        return (rand() %
        (upper - lower + 1)) + lower;
}