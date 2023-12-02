#pragma once

/**
 * @file   flipboard_file.h
 * @brief  Load and save configuration of the flipboard.
*/

#include "flipboard_model.h"
#include "button_model.h"

/**
 * @brief    Save the flipboard model to the settings file.
 * @param    model    The flipboard model to save.
 * @param    fields   The fields to save.
*/
bool flipboard_model_save(FlipboardModel* model, ButtonModelFields fields);

/**
 * @brief    Load the flipboard model from the settings file.
 * @param    model    The flipboard model to populate.
*/
bool flipboard_model_load(FlipboardModel* model);