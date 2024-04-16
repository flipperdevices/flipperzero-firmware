#pragma once

#define EMPTY_ACTION_INDEX -1

struct Item;

/** Transmits the selected item
 * 
 * @param   context     The App
 * @param   item        Selected item to transmit
 * @param   error       Error message if unsuccessful
*/
void action_tx(void* context, Item* item, FuriString* error);