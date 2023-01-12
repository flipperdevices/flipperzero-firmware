#include "tracker_engine.h"

#include "../flizzer_tracker_hal.h"

#include <furi_hal.h>

void tracker_engine_init(TrackerEngine* tracker_engine, uint8_t rate, SoundEngine* sound_engine)
{
	furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdTim1UpTim16, 14, tracker_engine_timer_isr, (void*)tracker_engine);
	tracker_engine_init_hardware(rate);

	tracker_engine->sound_engine = sound_engine;
}

void tracker_engine_deinit(TrackerEngine* tracker_engine)
{
	UNUSED(tracker_engine);
	furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdTim1UpTim16, 13, NULL, NULL);
	tracker_engine_stop();
}

void tracker_engine_advance_tick(TrackerEngine* tracker_engine)
{
	tracker_engine->absolute_position++;
}