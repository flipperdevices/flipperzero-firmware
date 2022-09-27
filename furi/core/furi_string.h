/**
 * @file furi_string.h
 * Furi string primitive
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STRING_FAILURE ((size_t)-1)

typedef struct FuriString FuriString;

FuriString* furi_string_alloc();

FuriString* furi_string_alloc_set(FuriString* s);

FuriString* furi_string_alloc_set_str(const char cstr[]);

FuriString* furi_string_alloc_vprintf(const char format[], va_list args);

void furi_string_reset(FuriString* s);

void furi_string_free(FuriString* s);

const char* furi_string_get_cstr(const FuriString* s);

void furi_string_set(FuriString* s, FuriString* source);

void furi_string_set_str(FuriString* s, const char cstr[]);

bool furi_string_start_with_str(const FuriString* s, const char str[]);

int furi_string_cmp(const FuriString* s1, const FuriString* s2);

int furi_string_cmp_str(const FuriString* s1, const char str[]);

size_t furi_string_search(const FuriString* v, const FuriString* needle);

size_t furi_string_search_str(const FuriString* v, const char needle[]);

size_t furi_string_search_from(const FuriString* v1, const FuriString* v2, size_t start);

size_t furi_string_search_from_str(const FuriString* v1, const char needle[], size_t start);

bool furi_string_equal(const FuriString* v1, const FuriString* v2);

bool furi_string_equal_str(const FuriString* v1, const char v2[]);

void furi_string_push_back(FuriString* v, char c);

size_t furi_string_size(const FuriString* s);

int furi_string_printf(FuriString* v, const char format[], ...);

int furi_string_cat_printf(FuriString* v, const char format[], ...);

bool furi_string_empty_p(const FuriString* v);

#define furi_string_alloc_set(x) \
    _Generic((x), \
    FuriString * : furi_string_alloc_set, \
    const FuriString * : furi_string_alloc_set, \
    char* : furi_string_alloc_set_str, \
    const char* : furi_string_alloc_set_str \
    )(x)

#define furi_string_set(v, x) \
    _Generic((x), \
    FuriString * : furi_string_set, \
    const FuriString * : furi_string_set, \
    char* : furi_string_set_str, \
    const char* : furi_string_set_str \
    )(v, x)

#define furi_string_equal(v, x) \
    _Generic((x), \
    FuriString * : furi_string_equal, \
    const FuriString * : furi_string_equal, \
    char* : furi_string_equal_str, \
    const char* : furi_string_equal_str \
    )(v, x)

#define furi_string_search(v, x) \
    _Generic((x), \
    FuriString * : furi_string_search, \
    const FuriString * : furi_string_search, \
    char* : furi_string_search_str, \
    const char* : furi_string_search_str \
    )(v, x)

#define furi_string_search_from(v, x, s) \
    _Generic((x), \
    FuriString * : furi_string_search_from, \
    const FuriString * : furi_string_search_from, \
    char* : furi_string_search_from_str, \
    const char* : furi_string_search_from_str \
    )(v, x, s)

#ifdef __cplusplus
}
#endif