#include "resource.h"

Topic preface[] = {
    {"Preface", NULL, APP_ASSETS_PATH("preface/preface.txt")},
    {"Preface to the first edition", NULL, APP_ASSETS_PATH("preface/preface_to_the_first_edition.txt")}
};

Topic chapter1[] = {
    {"1.0 A Tutorial Introduction", NULL, APP_ASSETS_PATH("chapter1/0-chapter1.txt")},
    {"1.1 Getting Started", NULL, APP_ASSETS_PATH("chapter1/1-chapter1.1.txt")},
    {"Exercises", NULL, APP_ASSETS_PATH("chapter1/exercises.txt")},
};

// Topic chapter2[] = {
// };

// Topic chapter3[] = {
// };

// Topic chapter4[] = {
// };

// Topic chapter5[] = {
// };

// Topic chapter6[] = {
// };

// Topic chapter7[] = {
// };

// Topic chapter8[] = {
// };

Topic license[] = {
    {"LICENSE", NULL, APP_ASSETS_PATH("LICENSE.txt")},
};

Chapter chapters[] = {
    {"Preface", preface, sizeof(preface) / sizeof(Topic)},
    {"Ch1-Introduction", chapter1, sizeof(chapter1) / sizeof(Topic)},
    // {"Ch2-Types, Ops, Expr", chapter2, sizeof(chapter2) / sizeof(Topic)},
    // {"Ch3-Control Flow", chapter3, sizeof(chapter3) / sizeof(Topic)},
    // {"Ch4-Functions", chapter4, sizeof(chapter4) / sizeof(Topic)},
    // {"Ch5-Pointers, Arrays", chapter5, sizeof(chapter5) / sizeof(Topic)},
    // {"Ch6-Structures", chapter6, sizeof(chapter6) / sizeof(Topic)},
    // {"Ch7-Input, Output", chapter7, sizeof(chapter7) / sizeof(Topic)},
    // {"Ch8-UNIX Sys Interface", chapter8, sizeof(chapter8) / sizeof(Topic)},
    {"LICENSE", license, sizeof(license) / sizeof(Topic)},
};

const size_t number_of_chapters = sizeof(chapters) / sizeof(Chapter);