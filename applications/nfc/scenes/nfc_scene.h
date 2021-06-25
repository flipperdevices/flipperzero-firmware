#pragma once

typedef struct {
    void (*on_start)(void* context);
    void (*on_event)(void* context);
    void (*on_exit)(void* context);
    void (*free)(NfcScene* scene);
    void* context;
} NfcScene;
