// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2023 KBEmbedded

#include <furi.h>
#include <furi_hal.h>
#include <stm32wbxx_ll_exti.h>
#include <stm32wbxx_ll_system.h>

#include <stdint.h>

#include "gblink.h"

const struct gblink_pins common_pinouts[PINOUT_COUNT] = {
	/* Original */
	{
		&gpio_ext_pc3,
		&gpio_ext_pb3,
		&gpio_ext_pb2,
		&gpio_ext_pa4,
	},
	/* MALVEKE EXT1 */
	{
		&gpio_ext_pa6,
		&gpio_ext_pa7,
		&gpio_ext_pb3,
		&gpio_ext_pa4,
	},
};

struct gblink {
	const GpioPin *serin;
	const GpioPin *serout;
	const GpioPin *clk;
	const GpioPin *sd;

	uint8_t in;
	uint8_t out;
	uint8_t out_buf;
	bool out_buf_valid;
	uint8_t shift;
	uint8_t nobyte;
	gblink_clk_source source;
	gblink_mode mode;
	gblink_speed speed;

	uint32_t time;

	uint32_t bitclk_timeout_us;
	/* Clocks idle between bytes is nominally 430 us long for burst data,
	 * 15 ms for idle polling (e.g. waiting for menu selection), some oddball
	 * 2 ms gaps that appears between one 0xFE byte from the Game Boy every trade;
	 * clock period is nominally 122 us.
	 * Therefore, if we haven't seen a clock in 500 us, reset our bit counter.
	 * Note that, this should never actually be a concern, but it is an additional
	 * safeguard against desyncing.
	 */

	void (*callback)(void* cb_context, uint8_t in);
	void *cb_context;

	uint32_t* ivt_mirror;
	uint32_t ivt_mirror_offs;
	bool exti3_rise_enable;
	bool exti3_fall_enable;
	bool exti3_event_enable;
};

static void gblink_shift_in(struct gblink *gblink)
{
	const uint32_t time_ticks = furi_hal_cortex_instructions_per_microsecond() * gblink->bitclk_timeout_us;

	/* If we exceeded the bit clock timeout, reset all counters */
	if ((DWT->CYCCNT - gblink->time) > time_ticks) {
		gblink->in = 0;
		gblink->shift = 0;
	}
	gblink->time = DWT->CYCCNT;

	gblink->in <<= 1;
	gblink->in |= furi_hal_gpio_read(gblink->serin);
	gblink->shift++;
	/* If 8 bits transfered, reset shift counter, call registered
	 * callback, re-set nobyte in output buffer.
	 */
	if (gblink->shift == 8) {
		gblink->shift = 0;

		/* Set up next out byte before calling the callback.
		 * This is in case the callback itself sets a new out
		 * byte which it will in most cases. It is up to the
		 * main application at this time to ensure that
		 * gblink_transfer() isn't called multiple times before
		 * a byte has a chance to be sent out.
		 */
		if (gblink->out_buf_valid) {
			gblink->out = gblink->out_buf;
			gblink->out_buf_valid = false;
		} else {
			gblink->out = gblink->nobyte;
		}
		gblink->callback(gblink->cb_context, gblink->in);
	}
}

static void gblink_shift_out(struct gblink *gblink)
{
	furi_hal_gpio_write(gblink->serout, !!(gblink->out & 0x80));
	gblink->out <<= 1;
}

static void gblink_clk_isr(void *context)
{
	furi_assert(context);
	struct gblink *gblink = context;

	if (furi_hal_gpio_read(gblink->clk)) {
		/* Posedge Shift in data */
		gblink_shift_in(gblink);
	} else {
		/* Negedge shift out data */
		gblink_shift_out(gblink);
	}
}

/* NOTE WELL! This function is absurdly hacky and a stupid workaround to a
 * stupid issue that doesn't really have any other solution in the current
 * Flipper/FURI API. I'm over-commenting this so we know exactly what is going
 * on if we ever have to re-visit this mess.
 *
 * This block of text below describes the overall idea, more specific comments
 * in the function body.
 *
 * TODO: make this more generic for any other GPIOs that might conflict with
 * exti interrupts. PA6, PB3, PC3, PB2? (NFC), PA13, PB6
 * NOTE: This is only set up at the moment for PB3, hardcoded
 *
 * There are multiple problems that this workaround is handling. EXTI interrupts
 * are shared among multiple pins. The FURI core maintains per-pin ISRs in a
 * private struct that has no way to read, save, or otherwise be able to put
 * back the ISR that would service a conflicting EXTI. e.g. PB3 and PH3
 * (the OK button) both share EXTI3. Setting an interrupt on PB3 will clobber
 * the FURI ISR callback/context pair as well as change EXTI3 to use PB3 as
 * the interrupt source.
 *
 * To make an interrupt work correctly on PB3 and not break the OK button
 * we need a way to set an interrupt for PB3 in a way that doesn't clobber the
 * private FURI GPIO ISR handles and can let the interrupt for the OK button
 * work again when we're done.
 *
 * The general concept of this workaround is to modify the IVT to create our
 * own handler for EXTI3 interrupts. Doing this leaves the aforementioned private
 * GPIO struct unmodified and disables the OK button from triggering an interrupt.
 * The IVT is normally located at the lowest addresses of flash (which is located
 * at 0x08000000 and mapped at runtime to 0x00000000); this means the IVT cannot
 * be changed at runtime.
 *
 * To make this work, we use the Vector Table Offset Register (VTOR) in the
 * System Control Block (SCB). The VTOR allows for changing the location of the
 * IVT. We copy the IVT to a location in memory, and then do a dance to safely
 * set up the GPIO interrupt to PB3, and swap in our IVT with the modified EXTI3
 * handler.
 *
 * When undoing this, the process is not quite in reverse as we have to put back
 * specific interrupt settings that we very likely would have clobbered but have
 * the ability to save beforehand.
 *
 * Wrapping the steps in disabling the EXTI3 interrupt is probably not needed,
 * but is a precaution since we are changing the interrupt sources in weird ways.
 */
/* Used to map our callback context in a way the handler can access */
static void *exti3_cb_context;
static void gblink_exti3_IRQHandler(void) {
	if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3)) {
		gblink_clk_isr(exti3_cb_context);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
	}
}

static void gblink_gross_exti_workaround(struct gblink *gblink)
{
	/* This process makes a number of assumptions, including that the IVT
	 * is located at 0x00000000, that the lowest flash page is mapped to
	 * that base address, and that the VTOR points to 0x00000000.
	 * There are runtime protections in place to prevent reading from the
	 * first 1 MB of addresses. So we have to always assume that the lowest
	 * page of flash is mapped to 0x00000000 and read the IVT from the that
	 * page in flash directly.
	 * The only check we can really do here is ensuring VTOR is 0 and that
	 * Main memory is mapped to 0x00000000. If either of those are not true,
	 * then we can't continue.
	 */
	furi_check(SCB->VTOR == 0x0);
	furi_check(LL_SYSCFG_GetRemapMemory() == LL_SYSCFG_REMAP_FLASH);

	/* Create a mirror of the existing IVT from CPU 1
	 * The IVT on this platform has 79 entries; 63 maskable, 10 non-maskable,
	 * 6 reserved. The maskable interrupts start at offset 16.
	 * CMSIS documentation says that the boundary for IVT must be aligned to
	 * the number of interrupts, rounded up to the nearest power of two, and
	 * then multiplied by the word width of the CPU. 79 rounds up to 128
	 * with a word width of 4, this is 512/0x200 bytes.
	 * As there is no good way with FreeRTOS to request an alloc at an
	 * aligned boundary, allocate the amount of data we need, plus 0x200
	 * bytes, to guarantee that we can put the table in a location that is
	 * properly aligned. Once we find a suitable base address, this offset
	 * is saved for later.
	 */
	gblink->ivt_mirror = malloc((79 * sizeof(uint32_t)) + 0x200);
	gblink->ivt_mirror_offs = (uint32_t)gblink->ivt_mirror;
	while (gblink->ivt_mirror_offs & 0x1FF)
		gblink->ivt_mirror_offs++;
	/* 0x08000000 is used instead of 0x00000000 because everything complains
	 * using a NULL pointer.
	 */
	memcpy((uint32_t *)gblink->ivt_mirror_offs, ((uint32_t *)0x08000000), 79 * sizeof(uint32_t));

	/* Point our IVT's EXTI3 interrupt to our desired interrupt handler.
	 * Also copy the gblink struct to the global var that the interrupt
	 * handler will use to make further calls.
	 */
	((uint32_t *)gblink->ivt_mirror_offs)[25] = (uint32_t)gblink_exti3_IRQHandler; // 16 NMI + offset of 9 for EXTI3
	exti3_cb_context = gblink;

	/* Disable the EXTI3 interrupt. This lets us do bad things without
	 * fear of an IRQ hitting in the middle.
	 */
	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_3);

	/* Save the existing rise/fall trigger settings. In theory, these should
	 * really never change through the life of the flipper OS. But for safety
	 * we always save them rather than just blindly restoring the same settings
	 * back when we undo this later.
	 */
	gblink->exti3_rise_enable = LL_EXTI_IsEnabledRisingTrig_0_31(LL_EXTI_LINE_3);
	gblink->exti3_fall_enable = LL_EXTI_IsEnabledFallingTrig_0_31(LL_EXTI_LINE_3);
	gblink->exti3_event_enable = LL_EXTI_IsEnabledEvent_0_31(LL_EXTI_LINE_3);

	/* Now, set up our desired pin settings. This will only clobber exti3
	 * settings and will not affect the actual interrupt vector address.
	 * Settings include the rising/falling/event triggers which we just
	 * saved.
	 */
	furi_hal_gpio_init(gblink->clk, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);

	/* Update the NVIC table to point at our desired table.
	 * Out of safety, stop the world around changing the VTOR reg.
	 */
	FURI_CRITICAL_ENTER();
	SCB->VTOR = gblink->ivt_mirror_offs;
	FURI_CRITICAL_EXIT();

	/* Last, enable the interrupts and hope everything works. */
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_3);
}

static void gblink_gross_exti_workaround_undo(struct gblink *gblink)
{
	/* First, disable the EXTI3 interrupt. This lets us do bad things without
	 * fear of an IRQ hitting in the middle.
	 */
	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_3);

	/* Set the correct input source, PH3/OK button, to EXTI3. It is important
	 * to do this before calling furi_hal_gpio_init() on PB3. When that func
	 * is called with no interrupt settings enabled, if the EXTI source
	 * matches the pin, and the interrupt is enabled, interrupts will be
	 * disabled. By manually setting the EXTI3 source here, it no longer
	 * matches the PB3 pin, and our changing of IO settings on our GPIO pin
	 * to no longer have interrupts will not affect the shared IRQ.
	 */
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTH, LL_SYSCFG_EXTI_LINE3);

	/* Set the correct rise/fall/event settings back */
	if (gblink->exti3_rise_enable)
		LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_3);
	else
		LL_EXTI_DisableRisingTrig_0_31(LL_EXTI_LINE_3);

	if (gblink->exti3_fall_enable)
		LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_3);
	else
		LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_3);

	if (gblink->exti3_event_enable)
		LL_EXTI_EnableEvent_0_31(LL_EXTI_LINE_3);
	else
		LL_EXTI_DisableEvent_0_31(LL_EXTI_LINE_3);

	/* "Release" the GPIO by putting it back in a known idle state. */
	furi_hal_gpio_init_simple(gblink->clk, GpioModeAnalog);

	/* Set the IVT back to the normal, in-flash table. Stopping the world
	 * while we do so.
	 * NOTE: This just assumes the VTOR is always at 0x0 by default, if this
	 * ever changes in the Flipper OS, then that will be a problem.
	 */
	FURI_CRITICAL_ENTER();
	SCB->VTOR = 0x0;
	FURI_CRITICAL_EXIT();

	/* Re-enable the interrupt, OK button should work again. */
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_3);

	/* Free the alloc()ed mirror space */
	free(gblink->ivt_mirror);
}

void gblink_clk_source_set(void *handle, int source)
{
	furi_assert(handle);
	struct gblink *gblink = handle;

	gblink->source = source;
	gblink->shift = 0;
}

void gblink_speed_set(void *handle, gblink_speed speed)
{
	furi_assert(handle);
	struct gblink *gblink = handle;

	gblink->speed = speed;
}

/* default is set to 500 us */
void gblink_timeout_set(void *handle, uint32_t us)
{
	furi_assert(handle);
	struct gblink *gblink = handle;

	gblink->bitclk_timeout_us = us;
}

void gblink_transfer(void *handle, uint8_t val)
{
	furi_assert(handle);
	struct gblink *gblink = handle;

	/* This checks the value of gblink->shift which can change in the ISR.
	 * Because of that, disable interrupts when checking gblink->shift and
	 * setting gblink->out_buf_valid
	 * If shift is 0, we're between bytes and can safely set the out byte.
	 * If shift is nonzero, a byte is currently being transmitted. Set the
	 * out_buf and set out_buf_valid. When the ISR is finished writing the
	 * next byte it will check out_buf_valid and copy in out_buf.
	 *
	 * The correct/smart way of doing this would be a mutex rather than
	 * stopping the world.
	 *
	 * Realistically, this should only ever be called from the transfer
	 * complete callback. There are few situations outside of that which
	 * would make sense.
	 *
	 * Note that, this currently has no checks for if there is data already
	 * pending to be transmitted. Calling this back to back can cause data
	 * loss!
	 */
	FURI_CRITICAL_ENTER();
	if (gblink->shift == 0) {
		gblink->out = val;
		gblink->out_buf_valid = false;
	} else {
		gblink->out_buf = val;
		gblink->out_buf_valid = true;
	}
	FURI_CRITICAL_EXIT();
}

void gblink_nobyte_set(void *handle, uint8_t val)
{
	struct gblink *gblink = handle;
	gblink->nobyte = val;
}

void gblink_int_enable(void *handle)
{
	furi_assert(handle);
	struct gblink *gblink = handle;

	furi_hal_gpio_enable_int_callback(gblink->clk);
}

void gblink_int_disable(void *handle)
{
	furi_assert(handle);
	struct gblink *gblink = handle;

	furi_hal_gpio_disable_int_callback(gblink->clk);
}

void *gblink_alloc(struct gblink_def *gblink_def)
{
	struct gblink *gblink;

	/* Allocate and zero struct */
	gblink = malloc(sizeof(struct gblink));

	/* Set struct values from function args */
	gblink->serin = gblink_def->pins->serin;
	gblink->serout = gblink_def->pins->serout;
	gblink->clk = gblink_def->pins->clk;
	gblink->sd = gblink_def->pins->sd;
	gblink->source = gblink_def->source;
	gblink->speed = GBLINK_SPD_8192HZ;

	/* Set up timeout variables */
	gblink->bitclk_timeout_us = 500;
	gblink->time = DWT->CYCCNT;

	/* Set up secondary callback */
	gblink->callback = gblink_def->callback;
	gblink->cb_context = gblink_def->cb_context;

	/* Set up pins */
	/* TODO: Set up a list of pins that are not safe to use with interrupts.
	 * I do believe the main FURI GPIO struct has this data baked in so that
	 * could be used. For now though, we're only checking for the MALVEKE
	 * pinout which uses a clk pin that has its IRQ shared with the Okay
	 * button.
	 * See the work done in pokemon trade tool custom pinout selection for
	 * an idea of how to check all that.
	 */
	/* TODO: Currently assumes external clock source only */
	/* XXX: This might actually be open-drain on real GB hardware */
	furi_hal_gpio_write(gblink->serout, false);
	furi_hal_gpio_init(gblink->serout, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
	furi_hal_gpio_write(gblink->serin, false);
	furi_hal_gpio_init(gblink->serin, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);

	/* Set up interrupt on clock */
	if (gblink->clk == &gpio_ext_pb3) {
		/* The clock pin is on a pin that is not safe to set an interrupt
		 * on, so we do a gross workaround to get an interrupt enabled
		 * on that pin in a way that can be undone safely later with
		 * no impact to the shared IRQ.
		 */
		gblink_gross_exti_workaround(gblink);
	} else {
		furi_hal_gpio_init(gblink->clk, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);
		/* This may not be needed after NFC refactor */
		furi_hal_gpio_remove_int_callback(gblink->clk);
		furi_hal_gpio_add_int_callback(gblink->clk, gblink_clk_isr, gblink);
	}

	return gblink;
}

void gblink_free(void *handle)
{
	furi_assert(handle);
	struct gblink *gblink = handle;

	if (gblink->clk == &gpio_ext_pb3) {
		/* This handles switching the IVT back and putting the EXTI
		 * regs and pin regs in a valid state for normal use.
		 */
		gblink_gross_exti_workaround_undo(gblink);
	} else {
		/* Remove interrupt, set IO to sane state */
		furi_hal_gpio_remove_int_callback(gblink->clk);
	}
	furi_hal_gpio_init_simple(gblink->serin, GpioModeAnalog);
	furi_hal_gpio_init_simple(gblink->serout, GpioModeAnalog);
	furi_hal_gpio_init_simple(gblink->clk, GpioModeAnalog);
	free(gblink);
}
