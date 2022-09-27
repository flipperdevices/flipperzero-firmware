/**
 * @file furi_string.h
 * Furi string primitive
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <m-core.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STRING_FAILURE ((size_t)-1)

typedef struct FuriString FuriString;

FuriString* furi_string_alloc();

FuriString* furi_string_alloc_set(const FuriString* s);

FuriString* furi_string_alloc_set_str(const char cstr[]);

FuriString* furi_string_alloc_printf(const char format[], ...);

FuriString* furi_string_alloc_vprintf(const char format[], va_list args);

void furi_string_alloc_move(FuriString* v1, FuriString* v2);

void furi_string_free(FuriString* s);

void furi_string_reserve(FuriString* v, size_t alloc);

void furi_string_reset(FuriString* s);

void furi_string_swap(FuriString* v1, FuriString* v2);

void furi_string_move(FuriString* v1, FuriString* v2);

size_t furi_string_hash(const FuriString* v);

char furi_string_get_char(const FuriString* v, size_t index);

const char* furi_string_get_cstr(const FuriString* s);

void furi_string_set(FuriString* s, FuriString* source);

void furi_string_set_str(FuriString* s, const char cstr[]);

/* Set the string to the n first characters of the C string str */
void furi_string_set_strn(FuriString* s, const char str[], size_t n);

void furi_string_set_char(FuriString* s, size_t index, const char c);

int furi_string_cmp(const FuriString* s1, const FuriString* s2);

int furi_string_cmp_str(const FuriString* s1, const char str[]);

/* Test if the string is equal to the other string 
   (case insentive according to the current locale)
   Note: doesn't work with UTF-8 strings.
*/
int furi_string_cmpi(const FuriString* v1, const FuriString* v2);

/* Test if the string is equal to the C string 
   (case insentive according to the current locale)
   Note: doesn't work with UTF-8 strings.
*/
int furi_string_cmpi_str(const FuriString* v1, const char p2[]);

size_t furi_string_search(const FuriString* v, const FuriString* needle, size_t start);

size_t furi_string_search_str(const FuriString* v, const char needle[], size_t start);

bool furi_string_equal(const FuriString* v1, const FuriString* v2);

bool furi_string_equal_str(const FuriString* v1, const char v2[]);

void furi_string_push_back(FuriString* v, char c);

size_t furi_string_size(const FuriString* s);

int furi_string_printf(FuriString* v, const char format[], ...);

int furi_string_vprintf(FuriString* v, const char format[], va_list args);

int furi_string_cat_printf(FuriString* v, const char format[], ...);

bool furi_string_empty_p(const FuriString* v);

/* Replace in the string the sub-string at position 'pos' for 'len' bytes into the C string str2. */
void furi_string_replace_at(FuriString* v, size_t pos, size_t len, const char str2[]);

size_t furi_string_replace_str(FuriString* v, const char str1[], const char str2[], size_t start);

void furi_string_replace_all_str(FuriString* v, const char str1[], const char str2[]);

void furi_string_replace_all(FuriString* v, const FuriString* str1, const FuriString* str2);

bool furi_string_start_with(const FuriString* v, const FuriString* v2);

bool furi_string_start_with_str(const FuriString* v, const char str[]);

bool furi_string_end_with(const FuriString* v, const FuriString* v2);

bool furi_string_end_with_str(const FuriString* v, const char str[]);

size_t furi_string_search_char(const FuriString* v, char c, size_t start);

size_t furi_string_search_rchar(const FuriString* v, char c, size_t start);

void furi_string_left(FuriString* v, size_t index);

void furi_string_right(FuriString* v, size_t index);

void furi_string_mid(FuriString* v, size_t index, size_t size);

void furi_string_strim(FuriString* v, const char charac[]);

/* Concatene the string with the other string */
void furi_string_cat(FuriString* v, const FuriString* v2);

/* Concatene the string with the C string */
void furi_string_cat_str(FuriString* v, const char str[]);

/* Set the string to the n first characters of other one */
void furi_string_set_n(FuriString* v, const FuriString* ref, size_t offset, size_t length);

/* An unicode value */
typedef unsigned int FuriStringUnicodeValue;

/* Compute the length in UTF8 characters in the string */
size_t furi_string_unicode_length(FuriString* str);

/* Push unicode into string, encoding it in UTF8 */
void furi_string_unicode_push(FuriString* str, FuriStringUnicodeValue u);

/* State of the UTF8 decoding machine state */
typedef enum {
    M_STRING_UTF8_STARTING = 0,
    M_STRING_UTF8_DECODING_1 = 8,
    M_STRING_UTF8_DECODING_2 = 16,
    M_STRING_UTF8_DOCODING_3 = 24,
    M_STRING_UTF8_ERROR = 32
} FuriStringUTF8State;

/* Main generic UTF8 decoder
   It shall be (nearly) branchless on any CPU.
   It takes a character, and the previous state and the previous value of the unicode value.
   It updates the state and the decoded unicode value.
   A decoded unicoded value is valid only when the state is STARTING.
 */
void furi_string_unicode_utf8_decode(
    char c,
    FuriStringUTF8State* state,
    FuriStringUnicodeValue* unicode);

/* 
 * Select either the string function or the str function depending on
 * the b operade to the function.
 * func1 is the string function / func2 is the str function.
 */
#define FURI_STRING_SELECT1(func1, func2, a) \
    _Generic((a), char* : func2, const char* : func2, default : func1)(a)

#define FURI_STRING_SELECT2(func1, func2, a, b) \
    _Generic((b), char* : func2, const char* : func2, default : func1)(a, b)

#define FURI_STRING_SELECT3(func1, func2, a, b, c) \
    _Generic((b), char* : func2, const char* : func2, default : func1)(a, b, c)

/* Init & Set the string a to the string (or C string) b (constructor) */
#define furi_string_alloc_set(a) \
    FURI_STRING_SELECT1(furi_string_alloc_set, furi_string_alloc_set_str, a)

/* Set the string a to the string (or C string) b */
#define furi_string_set(a, b) FURI_STRING_SELECT2(furi_string_set, furi_string_set_str, a, b)

#define furi_string_cmp(a, b) FURI_STRING_SELECT2(furi_string_cmp, furi_string_cmp_str, a, b)

#define furi_string_equal(a, b) FURI_STRING_SELECT2(furi_string_equal, furi_string_equal_str, a, b)

#define furi_string_search(a, b) \
    FURI_STRING_SELECT2(furi_string_search, furi_string_search_str, a, b)

#define furi_string_replace_all(a, b, c) \
    FURI_STRING_SELECT3(furi_string_replace_all, furi_string_replace_all_str, a, b, c)

#undef furi_string_search
/* Search for a string in a string (or C string) (string, string[, start=0]) */
#define furi_string_search(v, ...) \
    M_APPLY(                       \
        FURI_STRING_SELECT3,       \
        furi_string_search,        \
        furi_string_search_str,    \
        v,                         \
        M_IF_DEFAULT1(0, __VA_ARGS__))

#define furi_string_cmpi(a, b) FURI_STRING_SELECT2(furi_string_cmpi, furi_string_cmpi_str, a, b)

#define furi_string_start_with(a, b) \
    FURI_STRING_SELECT2(furi_string_start_with, furi_string_start_with_str, a, b)

#define furi_string_end_with(a, b) \
    FURI_STRING_SELECT2(furi_string_end_with, furi_string_end_with_str, a, b)

#define furi_string_cat(a, b) FURI_STRING_SELECT2(furi_string_cat, furi_string_cat_str, a, b)

#define F_STR_INIT(a) ((a) = furi_string_alloc())
#define F_STR_INIT_SET(a, b) ((a) = furi_string_alloc_set(b))

#define FURI_STRING_OPLIST              \
    (INIT(F_STR_INIT),                  \
     INIT_SET(F_STR_INIT_SET),          \
     SET(furi_string_set),              \
     INIT_MOVE(furi_string_alloc_move), \
     MOVE(furi_string_move),            \
     SWAP(furi_string_swap),            \
     RESET(furi_string_reset),          \
     EMPTY_P(furi_string_empty_p),      \
     CLEAR(furi_string_free),           \
     HASH(furi_string_hash),            \
     EQUAL(furi_string_equal),          \
     CMP(furi_string_cmp),              \
     TYPE(FuriString*))

/* Strim a string from the given set of characters (default is " \n\r\t") */
#define furi_string_strim(...) M_APPLY(furi_string_strim, M_IF_DEFAULT1("  \n\r\t", __VA_ARGS__))

/* Search for a character in a string (string, character[, start=0]) */
#define furi_string_search_char(v, ...) \
    M_APPLY(furi_string_search_char, v, M_IF_DEFAULT1(0, __VA_ARGS__))

/* Reverse Search for a character in a string (string, character[, start=0]) */
#define furi_string_search_rchar(v, ...) \
    M_APPLY(furi_string_search_rchar, v, M_IF_DEFAULT1(0, __VA_ARGS__))

/* Replace a C string to another C string in a string (string, c_src_string, c_dst_string, [, start=0]) */
#define furi_string_replace_str(v, s1, ...) \
    M_APPLY(furi_string_replace_str, v, s1, M_IF_DEFAULT1(0, __VA_ARGS__))

#ifdef __cplusplus
}
#endif