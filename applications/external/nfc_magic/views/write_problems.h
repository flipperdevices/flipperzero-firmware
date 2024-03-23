#pragma once

#include <stdint.h>
#include <gui/view.h>

typedef struct WriteProblems WriteProblems;

typedef enum {
    WriteProblemsEventCenterPressed,
    WriteProblemsEventLeftPressed,
} WriteProblemsEvent;

typedef void (*WriteProblemsCallback)(WriteProblemsEvent event, void* context);

WriteProblems* write_problems_alloc();

void write_problems_free(WriteProblems* instance);

void write_problems_reset(WriteProblems* instance);

View* write_problems_get_view(WriteProblems* instance);

void write_problems_set_callback(
    WriteProblems* instance,
    WriteProblemsCallback callback,
    void* context);

void write_problems_set_content(WriteProblems* instance, const char* content);

void write_problems_set_problem_index(WriteProblems* instance, uint8_t index);

void write_problems_set_problems_total(WriteProblems* instance, uint8_t total);
