#pragma once

#define EMPTY_ACTION_INDEX -1

struct Item;

void action_tx(void* context, Item* item, FuriString* error);
