#ifndef BOOK_CHAPTERS_H
#define BOOK_CHAPTERS_H

#include <stdlib.h>

typedef struct {
    const char* name;
    const char* details;
} BookTopic;

typedef struct {
    const char* chapter_title;
    BookTopic* details;
    size_t number_of_details;
} BookChapter;

extern BookChapter chapters[];
extern const size_t number_of_chapters;

#endif // BOOK_CHAPTERS_H