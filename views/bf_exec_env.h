#pragma once
#include "../brainfuck_i.h"
#include <gui/view.h>

typedef void (*ExecEnvOkCallback)(InputType type, void* context);

BFExecEnv* bf_exec_env_alloc(BFApp* application);

void bf_exec_set_file_path(FuriString* path);

void bf_exec_env_free(BFExecEnv* execEnv);

View* bf_exec_env_get_view(BFExecEnv* execEnv);

void bf_exec_env_set_ok(
    BFExecEnv* execEnv,
    ExecEnvOkCallback callback,
    void* context);
