#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdlib.h>
#include "../constants/constants.h"
#include "../app/app.h"

typedef struct {
    const char* name;
    char* content;
    char file_path[128];
} Topic;

typedef struct {
    const char* name;
    Topic* content;
    size_t number_of_topics;
} Chapter;

extern Chapter chapters[];
extern const size_t number_of_chapters;

#endif // RESOURCE_H