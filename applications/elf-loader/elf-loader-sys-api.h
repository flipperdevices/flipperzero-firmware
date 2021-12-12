#pragma once

typedef struct {
    int (*printf)(const char* fmt, ...);
} sysent_t;

typedef void(entry_t)(void);

extern sysent_t syscalls;