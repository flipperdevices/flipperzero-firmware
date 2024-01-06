#include "topics.h"

BookTopic chapter1[] = {
    {
        "Chapter 1 - Intro", 
        "Let us begin with a quick introduction in C. Our aim is to show the essential elements of the language in real programs, but without getting bogged down in details, rules, and exceptions. At this point, we are not trying to be complete or even precise (save that the examples are meant to be correct). We want to get you as quickly as possible to the point where you can write useful programs, and to do that we have to concentrate on the basics: variables and constants, arithmetic, control flow, functions, and the rudiments of input and output. We are intentionally leaving out of this chapter features of C that are important for writing bigger programs. These include pointers, structures, most of C's rich set of operators, several control-flow statements, and the standard library.\n\nThis approach and its drawbacks. Most notable is that the complete story on any particular feature is not found here, and the tutorial, by being brief, may also be misleading. And because the examples do not use the full power of C, they are not as concise and elegant as they might be. We have tried to minimize these effects, but be warned. Another drawback is that later chapters will necessarily repeat some of this chapter. We hope that the repetition will help you more than it annoys.\n\nIn any case, experienced programmers should be able to extrapolate from the material in this chapter to their own programming needs. Beginners should supplement it by writing small, similar programs of their own. Both groups can use it as a framework on which to hang the more detailed descriptions that begin in Chapter 2."}, 

    {
        "1.1 Getting Started", 
        "The only way to learn a new programming language is by writing programs in it. The first program to write is the same for all languages:\n\nPrint the words\n'hello, world'\n\nThis is a big hurdle; to leap over it you have to be able to create the program text somewhere, compile it successfully, load it, run it, and find out where your output went. With these mechanical details mastered, everything else is comparatively easy.\n\nIn C, the program to print 'hello, world' is:\n#include <stdio.h>\nmain()\n{\n    printf(\"hello, world\\n\");\n}\n\nJust how to run this program depends on the system you are using. As a specific example, on the UNIX operating system you must create the program in a file whose name ends in '.c', such as 'hello.c', then compile it with the command:\ncc hello.c\n\nIf you haven't botched anything, such as omitting a character or misspelling something, the compilation will proceed silently, and make an executable file called a.out. If you run a.outby typing the command:\na.out\n\nit will print:\n hello, world\n\nOn other systems, the rules will be different; check with a local expert.\n\nNow, for some explanations about the program itself. A C program, whatever its size, consists of functions and variables. A function contains statements that specify the computing operations to be done, and variables store values used during the computation. C functions are like the subroutines and functions in Fortran or the procedures and functions of Pascal. Our example is a function named main. Normally you are at liberty to give functions whatever names you like, but 'main' is special - your program begins executing at the beginning of main. This means that every program must have a main somewhere.\n\n"}
    };


BookChapter chapters[] = {
    {"Ch1-Introduction", chapter1, 2},
    // {"Ch2-Types, Ops, Expr", chapter1, 2},
    // {"Ch3-Control Flow", chapter1, 2},
    // {"Ch4-Functions", chapter1, 2},
    // {"Ch5-Pointers, Arrays", chapter1, 2},
    // {"Ch6-Structures", chapter1, 2},
    // {"Ch7-Input, Output", chapter1, 2},
    // {"Ch8-UNIX Sys Interface", chapter1, 2},
    // {"LICENSE", chapter1, 2},
};

const size_t number_of_chapters = sizeof(chapters) / sizeof(BookChapter);