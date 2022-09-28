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

/**
 * @brief Furi string failure constant.
 */
#define FURI_STRING_FAILURE ((size_t)-1)

/**
 * @brief Furi string primitive.
 */
typedef struct FuriString FuriString;

//---------------------------------------------------------------------------
//                               Constructors
//---------------------------------------------------------------------------

/**
 * @brief Allocate new FuriString.
 * @return FuriString* 
 */
FuriString* furi_string_alloc();

/**
 * @brief Allocate new FuriString and set it to string.
 * Allocate & Set the string a to the string.
 * @param source 
 * @return FuriString* 
 */
FuriString* furi_string_alloc_set(const FuriString* source);

/**
 * @brief Allocate new FuriString and set it to C string.
 * Allocate & Set the string a to the C string.
 * @param cstr_source 
 * @return FuriString* 
 */
FuriString* furi_string_alloc_set_str(const char cstr_source[]);

/**
 * @brief Allocate new FuriString and printf to it.
 * Initialize and set a string to the given formatted value.
 * @param format 
 * @param ... 
 * @return FuriString* 
 */
FuriString* furi_string_alloc_printf(const char format[], ...);

/**
 * @brief Allocate new FuriString and printf to it.
 * Initialize and set a string to the given formatted value.
 * @param format 
 * @param args 
 * @return FuriString* 
 */
FuriString* furi_string_alloc_vprintf(const char format[], va_list args);

/**
 * @brief Allocate new FuriString and move source string content to it.
 * Allocate the string, set it to the other one, and destroy the other one.
 * @param source 
 * @return FuriString* 
 */
FuriString* furi_string_alloc_move(FuriString* source);

//---------------------------------------------------------------------------
//                               Destructors
//---------------------------------------------------------------------------

/**
 * @brief Free FuriString.
 * @param string 
 */
void furi_string_free(FuriString* string);

//---------------------------------------------------------------------------
//                         String memory management
//---------------------------------------------------------------------------

/**
 * @brief Reserve memory for string.
 * Modify the string capacity to be able to handle at least 'alloc' characters (including final nul char).
 * @param string 
 * @param size 
 */
void furi_string_reserve(FuriString* string, size_t size);

/**
 * @brief Reset string.
 * Make the string empty.
 * @param s 
 */
void furi_string_reset(FuriString* string);

/**
 * @brief Swap two strings.
 * Swap the two strings string_1 and string_2.
 * @param string_1 
 * @param string_2 
 */
void furi_string_swap(FuriString* string_1, FuriString* string_2);

/**
 * @brief Move string_2 content to string_1.
 * Set the string to the other one, and destroy the other one.
 * @param string_1 
 * @param string_2 
 */
void furi_string_move(FuriString* string_1, FuriString* string_2);

/**
 * @brief Compute a hash for the string.
 * @param string 
 * @return size_t 
 */
size_t furi_string_hash(const FuriString* string);

/**
 * @brief Get string size (usually length, but not for UTF-8)
 * @param string 
 * @return size_t 
 */
size_t furi_string_size(const FuriString* string);

/**
 * @brief Check that string is empty or not
 * @param string 
 * @return bool
 */
bool furi_string_empty(const FuriString* string);

//---------------------------------------------------------------------------
//                               Getters
//---------------------------------------------------------------------------

/**
 * @brief Get the character at the given index.
 * Return the selected character of the string.
 * @param string 
 * @param index 
 * @return char 
 */
char furi_string_get_char(const FuriString* string, size_t index);

/**
 * @brief Return the string view a classic C string.
 * @param string 
 * @return const char* 
 */
const char* furi_string_get_cstr(const FuriString* string);

//---------------------------------------------------------------------------
//                               Setters
//---------------------------------------------------------------------------

/**
 * @brief Set the string to the other string.
 * Set the string to the source string.
 * @param string 
 * @param source 
 */
void furi_string_set(FuriString* string, FuriString* source);

/**
 * @brief Set the string to the other C string.
 * Set the string to the source C string.
 * @param string 
 * @param source 
 */
void furi_string_set_str(FuriString* string, const char source[]);

/**
 * @brief Set the string to the n first characters of the C string.
 * @param string 
 * @param source 
 * @param length 
 */
void furi_string_set_strn(FuriString* string, const char source[], size_t length);

/**
 * @brief Set the character at the given index.
 * @param string 
 * @param index 
 * @param c 
 */
void furi_string_set_char(FuriString* string, size_t index, const char c);

/**
 * @brief Set the string to the n first characters of other one.
 * @param string 
 * @param source 
 * @param offset 
 * @param length 
 */
void furi_string_set_n(FuriString* string, const FuriString* source, size_t offset, size_t length);

/**
 * @brief Format in the string the given printf format
 * @param string 
 * @param format 
 * @param ... 
 * @return int 
 */
int furi_string_printf(FuriString* string, const char format[], ...);

/**
 * @brief Format in the string the given printf format
 * @param string 
 * @param format 
 * @param args 
 * @return int 
 */
int furi_string_vprintf(FuriString* string, const char format[], va_list args);

//---------------------------------------------------------------------------
//                               Appending
//---------------------------------------------------------------------------

/**
 * @brief Append a character to the string.
 * @param string 
 * @param c 
 */
void furi_string_push_back(FuriString* string, char c);

/**
 * @brief Append a string to the string.
 * Concatene the string with the other string.
 * @param string_1 
 * @param string_2 
 */
void furi_string_cat(FuriString* string_1, const FuriString* string_2);

/**
 * @brief Append a C string to the string.
 * Concatene the string with the C string.
 * @param string_1 
 * @param cstring_2 
 */
void furi_string_cat_str(FuriString* string_1, const char cstring_2[]);

/**
 * @brief Append to the string the formatted string of the given printf format.
 * @param string 
 * @param format 
 * @param ... 
 * @return int 
 */
int furi_string_cat_printf(FuriString* string, const char format[], ...);

/**
 * @brief Append to the string the formatted string of the given printf format.
 * @param string 
 * @param format 
 * @param args 
 * @return int 
 */
int furi_string_cat_vprintf(FuriString* string, const char format[], va_list args);

//---------------------------------------------------------------------------
//                               Comparators
//---------------------------------------------------------------------------

/**
 * @brief Compare two strings and return the sort order.
 * @param string_1 
 * @param string_2 
 * @return int 
 */
int furi_string_cmp(const FuriString* string_1, const FuriString* string_2);

/**
 * @brief Compare string with C string and return the sort order.
 * @param string_1 
 * @param cstring_2 
 * @return int 
 */
int furi_string_cmp_str(const FuriString* string_1, const char cstring_2[]);

/**
 * @brief Compare two strings (case insentive according to the current locale) and return the sort order.
 * Note: doesn't work with UTF-8 strings.
 * @param string_1 
 * @param string_2 
 * @return int 
 */
int furi_string_cmpi(const FuriString* string_1, const FuriString* string_2);

/**
 * @brief Compare string with C string (case insentive according to the current locale) and return the sort order.
 * Note: doesn't work with UTF-8 strings.
 * @param string_1 
 * @param cstring_2 
 * @return int 
 */
int furi_string_cmpi_str(const FuriString* string_1, const char cstring_2[]);

//---------------------------------------------------------------------------
//                                 Search
//---------------------------------------------------------------------------

/**
 * @brief Search the first occurence of the needle in the string from the position start.
 * Return STRING_FAILURE if not found.
 * By default, start is zero.
 * @param string 
 * @param needle 
 * @param start 
 * @return size_t 
 */
size_t furi_string_search(const FuriString* string, const FuriString* needle, size_t start);

/**
 * @brief Search the first occurence of the needle in the string from the position start.
 * Return STRING_FAILURE if not found.
 * @param string 
 * @param needle 
 * @param start 
 * @return size_t 
 */
size_t furi_string_search_str(const FuriString* string, const char needle[], size_t start);

/**
 * @brief Search for the position of the character c from the position start (include) in the string.
 * Return STRING_FAILURE if not found.
 * By default, start is zero.
 * @param string 
 * @param c 
 * @param start 
 * @return size_t 
 */
size_t furi_string_search_char(const FuriString* string, char c, size_t start);

/**
 * @brief Reverse search for the position of the character c from the position start (include) in the string.
 * Return STRING_FAILURE if not found.
 * By default, start is zero.
 * @param string 
 * @param c 
 * @param start 
 * @return size_t 
 */
size_t furi_string_search_rchar(const FuriString* string, char c, size_t start);

//---------------------------------------------------------------------------
//                                Equality
//---------------------------------------------------------------------------

/**
 * @brief Test if two strings are equal.
 * @param string_1 
 * @param string_2 
 * @return bool 
 */
bool furi_string_equal(const FuriString* string_1, const FuriString* string_2);

/**
 * @brief Test if the string is equal to the C string.
 * @param string_1 
 * @param cstring_2 
 * @return bool 
 */
bool furi_string_equal_str(const FuriString* string_1, const char cstring_2[]);

//---------------------------------------------------------------------------
//                                Replace
//---------------------------------------------------------------------------

/* Replace in the string the sub-string at position 'pos' for 'len' bytes into the C string str2. */
void furi_string_replace_at(FuriString* string, size_t pos, size_t len, const char replace[]);

size_t furi_string_replace_str(
    FuriString* string,
    const char needle[],
    const char replace[],
    size_t start);

void furi_string_replace_all_str(FuriString* string, const char needle[], const char replace[]);

void furi_string_replace_all(
    FuriString* string,
    const FuriString* needle,
    const FuriString* replace);

//---------------------------------------------------------------------------
//                            Start / End tests
//---------------------------------------------------------------------------

bool furi_string_start_with(const FuriString* string, const FuriString* start);

bool furi_string_start_with_str(const FuriString* string, const char start[]);

bool furi_string_end_with(const FuriString* string, const FuriString* v2);

bool furi_string_end_with_str(const FuriString* string, const char str[]);

//---------------------------------------------------------------------------
//                                Misc
//---------------------------------------------------------------------------

void furi_string_left(FuriString* string, size_t index);

void furi_string_right(FuriString* string, size_t index);

void furi_string_mid(FuriString* string, size_t index, size_t size);

void furi_string_strim(FuriString* string, const char chars[]);

//---------------------------------------------------------------------------
//                                UTF8
//---------------------------------------------------------------------------

/* An unicode value */
typedef unsigned int FuriStringUnicodeValue;

/* Compute the length in UTF8 characters in the string */
size_t furi_string_unicode_length(FuriString* str);

/* Push unicode into string, encoding it in UTF8 */
void furi_string_unicode_push(FuriString* str, FuriStringUnicodeValue u);

/* State of the UTF8 decoding machine state */
typedef enum {
    FuriStringUTF8StateStarting = 0,
    FuriStringUTF8StateDecoding1 = 8,
    FuriStringUTF8StateDecoding2 = 16,
    FuriStringUTF8StateDecoding3 = 24,
    FuriStringUTF8StateError = 32
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
     EMPTY_P(furi_string_empty),        \
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