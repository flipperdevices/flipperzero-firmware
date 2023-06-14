#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct SimpleArray SimpleArray;

typedef void SimpleArrayElement;

typedef void (*SimpleArrayReset)(SimpleArrayElement* elem);
typedef void (*SimpleArrayCopy)(SimpleArrayElement* elem, const SimpleArrayElement* other);

typedef struct {
    SimpleArrayReset reset;
    SimpleArrayCopy copy;
    const size_t type_size;
} SimpleArrayConfig;

SimpleArray* simple_array_alloc(const SimpleArrayConfig* config);

void simple_array_free(SimpleArray* instance);

void simple_array_init(SimpleArray* instance, uint32_t count);

void simple_array_reset(SimpleArray* instance);

void simple_array_copy(SimpleArray* instance, const SimpleArray* other);

uint32_t simple_array_get_count(const SimpleArray* instance);

SimpleArrayElement* simple_array_get(SimpleArray* instance, uint32_t index);

const SimpleArrayElement* simple_array_cget(const SimpleArray* instance, uint32_t index);
