/**
 * @file   flipboard_model_ref.h
 * @brief  Reference to a FlipboardModel.
 * @details  A reference to a FlipboardModel, used to pass a FlipboardModel to UI components 
 * that cant take a pointer to an existing FlipboardModel.
*/

#pragma once

#include "flipboard_model.h"

/**
 * @brief FlipboardModelRef is a reference to a FlipboardModel.
 * @details FlipboardModelRef is a reference to a FlipboardModel. 
 *  It is used to pass a FlipboardModel to UI components.
*/
typedef struct {
    FlipboardModel* model;
} FlipboardModelRef;
