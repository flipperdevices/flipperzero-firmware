#pragma once

#include <gui/view.h>
#include "../helpers/meal_pager_custom_event.h"

typedef struct Meal_PagerTransmit Meal_PagerTransmit;

typedef void (*Meal_PagerTransmitCallback)(Meal_PagerCustomEvent event, void* context);

void meal_pager_transmit_set_callback(
    Meal_PagerTransmit* meal_pager_transmit,
    Meal_PagerTransmitCallback callback,
    void* context);

void meal_pager_transmit_model_set_sending(Meal_PagerTransmit* instance, uint32_t value);
void meal_pager_transmit_model_set_type(Meal_PagerTransmit* instance, uint32_t type);
void meal_pager_transmit_model_set_station(Meal_PagerTransmit* instance, uint32_t station);
void meal_pager_transmit_model_set_pager(Meal_PagerTransmit* instance, uint32_t pager);

View* meal_pager_transmit_get_view(Meal_PagerTransmit* meal_pager_static);

Meal_PagerTransmit* meal_pager_transmit_alloc();

void meal_pager_transmit_free(Meal_PagerTransmit* meal_pager_static);