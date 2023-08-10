/* unit.c API unit tests driver
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


#ifndef CyaSSL_UNIT_H
#define CyaSSL_UNIT_H

#include <wolfssl/ssl.h>
#include <wolfssl/test.h>    /* thread and tcp stuff */

#ifdef WOLFSSL_FORCE_MALLOC_FAIL_TEST
#define XABORT() WC_DO_NOTHING
#else
#define XABORT() abort()
#endif

#ifndef WOLFSSL_PASSTHRU_ERR
#define Fail(description, result) do {                                         \
    printf("\nERROR - %s line %d failed with:", __FILE__, __LINE__);           \
    fputs("\n    expected: ", stdout); printf description;                     \
    fputs("\n    result:   ", stdout); printf result; fputs("\n\n", stdout);   \
    fflush(stdout);                                                            \
    XABORT();                                                                  \
} while(0)
#else
#define Fail(description, result) do {                                         \
    printf("\nERROR - %s line %d failed with:", __FILE__, __LINE__);           \
    fputs("\n    expected: ", stdout); printf description;                     \
    fputs("\n    result:   ", stdout); printf result; fputs("\n\n", stdout);   \
    fflush(stdout);                                                            \
} while (0)
#endif

#define Assert(test, description, result) if (!(test)) Fail(description, result)

#define AssertTrue(x)    Assert( (x), ("%s is true",     #x), (#x " => FALSE"))
#define AssertFalse(x)   Assert(!(x), ("%s is false",    #x), (#x " => TRUE"))
#define AssertNotNull(x) Assert( (x), ("%s is not null", #x), (#x " => NULL"))

#define AssertNull(x) do {                                                     \
    PEDANTIC_EXTENSION void* _x = (void*)(x);                                  \
    Assert(!_x, ("%s is null", #x), (#x " => %p", _x));                        \
} while(0)

#define AssertInt(x, y, op, er) do {                                           \
    int _x = (int)(x);                                                         \
    int _y = (int)(y);                                                         \
    Assert(_x op _y, ("%s " #op " %s", #x, #y), ("%d " #er " %d", _x, _y));    \
} while(0)

#define AssertIntEQ(x, y) AssertInt(x, y, ==, !=)
#define AssertIntNE(x, y) AssertInt(x, y, !=, ==)
#define AssertIntGT(x, y) AssertInt(x, y,  >, <=)
#define AssertIntLT(x, y) AssertInt(x, y,  <, >=)
#define AssertIntGE(x, y) AssertInt(x, y, >=,  <)
#define AssertIntLE(x, y) AssertInt(x, y, <=,  >)

#define AssertStr(x, y, op, er) do {                                           \
    const char* _x = (const char*)(x);                                         \
    const char* _y = (const char*)(y);                                         \
    int         _z = (_x && _y) ? strcmp(_x, _y) : -1;                         \
    Assert(_z op 0, ("%s " #op " %s", #x, #y),                                 \
                                            ("\"%s\" " #er " \"%s\"", _x, _y));\
} while(0)

#define AssertStrEQ(x, y) AssertStr(x, y, ==, !=)
#define AssertStrNE(x, y) AssertStr(x, y, !=, ==)
#define AssertStrGT(x, y) AssertStr(x, y,  >, <=)
#define AssertStrLT(x, y) AssertStr(x, y,  <, >=)
#define AssertStrGE(x, y) AssertStr(x, y, >=,  <)
#define AssertStrLE(x, y) AssertStr(x, y, <=,  >)

#ifdef WOLF_C89

#define AssertPtr(x, y, op, er) do {                                           \
    void* _x = (void*)(x);                                                     \
    void* _y = (void*)(y);                                                     \
    Assert(_x op _y, ("%s " #op " %s", #x, #y), ("%p " #er " %p", _x, _y));    \
} while(0)

#else

#define AssertPtr(x, y, op, er) do {                                           \
    PRAGMA_GCC_DIAG_PUSH                                                       \
      /* remarkably, without this inhibition, */                               \
      /* the _Pragma()s make the declarations warn. */                         \
    PRAGMA_GCC("GCC diagnostic ignored \"-Wdeclaration-after-statement\"")     \
      /* inhibit "ISO C forbids conversion of function pointer */              \
      /* to object pointer type [-Werror=pedantic]" */                         \
    PRAGMA_GCC("GCC diagnostic ignored \"-Wpedantic\"")                        \
    void* _x = (void*)(x);                                                     \
    void* _y = (void*)(y);                                                     \
    Assert(_x op _y, ("%s " #op " %s", #x, #y), ("%p " #er " %p", _x, _y));    \
    PRAGMA_GCC_DIAG_POP                                                        \
} while(0)

#endif

#define AssertPtrEq(x, y) AssertPtr(x, y, ==, !=)
#define AssertPtrNE(x, y) AssertPtr(x, y, !=, ==)
#define AssertPtrGT(x, y) AssertPtr(x, y,  >, <=)
#define AssertPtrLT(x, y) AssertPtr(x, y,  <, >=)
#define AssertPtrGE(x, y) AssertPtr(x, y, >=,  <)
#define AssertPtrLE(x, y) AssertPtr(x, y, <=,  >)


#define EXPECT_DECLS \
    int _ret = TEST_SKIPPED
#define EXPECT_RESULT() \
    _ret
#define EXPECT_SUCCESS() \
    (_ret == TEST_SUCCESS)
#define EXPECT_FAIL() \
    (_ret == TEST_FAIL)

#define ExpFail(description, result) do {                                      \
    printf("\nERROR - %s line %d failed with:", __FILE__, __LINE__);           \
    fputs("\n    expected: ", stdout); printf description;                     \
    fputs("\n    result:   ", stdout); printf result; fputs("\n\n", stdout);   \
    fflush(stdout);                                                            \
    _ret = TEST_FAIL;                                                          \
} while (0)

#define Expect(test, description, result) do {                                 \
    if (_ret != TEST_FAIL) { if (!(test)) ExpFail(description, result);        \
                             else _ret = TEST_SUCCESS; }                       \
} while (0)

#define ExpectTrue(x)    Expect( (x), ("%s is true",     #x), (#x " => FALSE"))
#define ExpectFalse(x)   Expect(!(x), ("%s is false",    #x), (#x " => TRUE"))
#define ExpectNotNull(x) Expect( (x), ("%s is not null", #x), (#x " => NULL"))

#define ExpectNull(x) do {                                                     \
    if (_ret != TEST_FAIL) {                                                   \
        PEDANTIC_EXTENSION void* _x = (void*)(x);                              \
        Expect(!_x, ("%s is null", #x), (#x " => %p", _x));                    \
    }                                                                          \
} while(0)

#define ExpectInt(x, y, op, er) do {                                           \
    if (_ret != TEST_FAIL) {                                                   \
        int _x = (int)(x);                                                     \
        int _y = (int)(y);                                                     \
        Expect(_x op _y, ("%s " #op " %s", #x, #y), ("%d " #er " %d", _x, _y));\
    }                                                                          \
} while(0)

#define ExpectIntEQ(x, y) ExpectInt(x, y, ==, !=)
#define ExpectIntNE(x, y) ExpectInt(x, y, !=, ==)
#define ExpectIntGT(x, y) ExpectInt(x, y,  >, <=)
#define ExpectIntLT(x, y) ExpectInt(x, y,  <, >=)
#define ExpectIntGE(x, y) ExpectInt(x, y, >=,  <)
#define ExpectIntLE(x, y) ExpectInt(x, y, <=,  >)

#define ExpectStr(x, y, op, er) do {                                           \
    if (_ret != TEST_FAIL) {                                                   \
        const char* _x = (const char*)(x);                                     \
        const char* _y = (const char*)(y);                                     \
        int         _z = (_x && _y) ? strcmp(_x, _y) : -1;                     \
        Expect(_z op 0, ("%s " #op " %s", #x, #y),                             \
                                            ("\"%s\" " #er " \"%s\"", _x, _y));\
    }                                                                          \
} while(0)

#define ExpectStrEQ(x, y) ExpectStr(x, y, ==, !=)
#define ExpectStrNE(x, y) ExpectStr(x, y, !=, ==)
#define ExpectStrGT(x, y) ExpectStr(x, y,  >, <=)
#define ExpectStrLT(x, y) ExpectStr(x, y,  <, >=)
#define ExpectStrGE(x, y) ExpectStr(x, y, >=,  <)
#define ExpectStrLE(x, y) ExpectStr(x, y, <=,  >)

#define ExpectPtr(x, y, op, er) do {                                           \
    if (_ret != TEST_FAIL) {                                                   \
        PRAGMA_DIAG_PUSH                                                       \
          /* remarkably, without this inhibition, */                           \
          /* the _Pragma()s make the declarations warn. */                     \
        PRAGMA("GCC diagnostic ignored \"-Wdeclaration-after-statement\"")     \
          /* inhibit "ISO C forbids conversion of function pointer */          \
          /* to object pointer type [-Werror=pedantic]" */                     \
        PRAGMA("GCC diagnostic ignored \"-Wpedantic\"")                        \
        void* _x = (void*)(x);                                                 \
        void* _y = (void*)(y);                                                 \
        Expect(_x op _y, ("%s " #op " %s", #x, #y), ("%p " #er " %p", _x, _y));\
        PRAGMA_DIAG_POP                                                        \
    }                                                                          \
} while(0)

#define ExpectPtrEq(x, y) ExpectPtr(x, y, ==, !=)
#define ExpectPtrNE(x, y) ExpectPtr(x, y, !=, ==)
#define ExpectPtrGT(x, y) ExpectPtr(x, y,  >, <=)
#define ExpectPtrLT(x, y) ExpectPtr(x, y,  <, >=)
#define ExpectPtrGE(x, y) ExpectPtr(x, y, >=,  <)
#define ExpectPtrLE(x, y) ExpectPtr(x, y, <=,  >)

#define ExpectBuf(x, y, z, op, er) do {                                        \
    if (_ret != TEST_FAIL) {                                                   \
        const byte* _x = (const byte*)(x);                                     \
        const byte* _y = (const byte*)(y);                                     \
        int         _z = (int)(z);                                             \
        int         _w = ((_x) && (_y)) ? XMEMCMP(_x, _y, _z) : -1;            \
        Expect(_w op 0, ("%s " #op " %s for %s", #x, #y, #z),                  \
                             ("\"%p\" " #er " \"%p\" for \"%d\"", _x, _y, _z));\
    }                                                                          \
} while(0)

#define ExpectBufEQ(x, y, z) ExpectBuf(x, y, z, ==, !=)
#define ExpectBufNE(x, y, z) ExpectBuf(x, y, z, !=, ==)

#define ExpectFail() ExpectTrue(0)


#define DoExpectNull(x) do {                                                   \
    PEDANTIC_EXTENSION void* _x = (void*)(x);                                  \
    Expect(!_x, ("%s is null", #x), (#x " => %p", _x));                        \
} while(0)

#define DoExpectInt(x, y, op, er) do {                                         \
    int _x = (int)(x);                                                         \
    int _y = (int)(y);                                                         \
    Expect(_x op _y, ("%s " #op " %s", #x, #y), ("%d " #er " %d", _x, _y));    \
} while(0)

#define DoExpectIntEQ(x, y) DoExpectInt(x, y, ==, !=)
#define DoExpectIntNE(x, y) DoExpectInt(x, y, !=, ==)
#define DoExpectIntGT(x, y) DoExpectInt(x, y,  >, <=)
#define DoExpectIntLT(x, y) DoExpectInt(x, y,  <, >=)
#define DoExpectIntGE(x, y) DoExpectInt(x, y, >=,  <)
#define DoExpectIntLE(x, y) DoExpectInt(x, y, <=,  >)

#define DoExpectStr(x, y, op, er) do {                                         \
    const char* _x = (const char*)(x);                                         \
    const char* _y = (const char*)(y);                                         \
    int         _z = (_x && _y) ? strcmp(_x, _y) : -1;                         \
    Expect(_z op 0, ("%s " #op " %s", #x, #y),                                 \
                                            ("\"%s\" " #er " \"%s\"", _x, _y));\
} while(0)

#define DoExpectStrEQ(x, y) DoExpectStr(x, y, ==, !=)
#define DoExpectStrNE(x, y) DoExpectStr(x, y, !=, ==)
#define DoExpectStrGT(x, y) DoExpectStr(x, y,  >, <=)
#define DoExpectStrLT(x, y) DoExpectStr(x, y,  <, >=)
#define DoExpectStrGE(x, y) DoExpectStr(x, y, >=,  <)
#define DoExpectStrLE(x, y) DoExpectStr(x, y, <=,  >)

#define DoExpectPtr(x, y, op, er) do {                                         \
    PRAGMA_DIAG_PUSH                                                           \
      /* remarkably, without this inhibition, */                               \
      /* the _Pragma()s make the declarations warn. */                         \
    PRAGMA("GCC diagnostic ignored \"-Wdeclaration-after-statement\"")         \
      /* inhibit "ISO C forbids conversion of function pointer */              \
      /* to object pointer type [-Werror=pedantic]" */                         \
    PRAGMA("GCC diagnostic ignored \"-Wpedantic\"")                            \
    void* _x = (void*)(x);                                                     \
    void* _y = (void*)(y);                                                     \
    Expect(_x op _y, ("%s " #op " %s", #x, #y), ("%p " #er " %p", _x, _y));    \
    PRAGMA_DIAG_POP                                                            \
} while(0)

#define DoExpectPtrEq(x, y) DoExpectPtr(x, y, ==, !=)
#define DoExpectPtrNE(x, y) DoExpectPtr(x, y, !=, ==)
#define DoExpectPtrGT(x, y) DoExpectPtr(x, y,  >, <=)
#define DoExpectPtrLT(x, y) DoExpectPtr(x, y,  <, >=)
#define DoExpectPtrGE(x, y) DoExpectPtr(x, y, >=,  <)
#define DoExpectPtrLE(x, y) DoExpectPtr(x, y, <=,  >)

#define DoExpectBuf(x, y, z, op, er) do {                                      \
    const byte* _x = (const byte*)(x);                                         \
    const byte* _y = (const byte*)(y);                                         \
    int         _z = (int)(z);                                                 \
    int         _w = ((_x) && (_y)) ? XMEMCMP(_x, _y, _z) : -1;                \
    Expect(_w op 0, ("%s " #op " %s for %s", #x, #y, #z),                      \
                             ("\"%p\" " #er " \"%p\" for \"%d\"", _x, _y, _z));\
} while(0)

#define DoExpectBufEQ(x, y, z) DoExpectBuf(x, y, z, ==, !=)
#define DoExpectBufNE(x, y, z) DoExpectBuf(x, y, z, !=, ==)

void ApiTest_PrintTestCases(void);
int ApiTest_RunIdx(int idx);
int ApiTest_RunName(char* name);
int ApiTest(void);

int  SuiteTest(int argc, char** argv);
int  HashTest(void);
void SrpTest(void);
int w64wrapper_test(void);
int QuicTest(void);


#endif /* CyaSSL_UNIT_H */
