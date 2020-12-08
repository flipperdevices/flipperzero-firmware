#pragma once 

#include "bt.h"

#include <cli/cli.h>
#include <flipper.h>
#include <flipper_v2.h>

typedef struct {
    Cli* cli;
} BT;

BT* bt_alloc();
