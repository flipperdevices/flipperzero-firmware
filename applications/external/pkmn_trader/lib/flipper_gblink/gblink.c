// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2023 KBEmbedded

#include <furi.h>
#include <furi_hal.h>

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

static void gblink_clk_callback(void *context)
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
	/* Currently assumes external clock source only */
	/* XXX: This might actually be open-drain on real GB hardware */
	furi_hal_gpio_write(gblink->serout, false);
	furi_hal_gpio_init(gblink->serout, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
	furi_hal_gpio_write(gblink->serin, false);
	furi_hal_gpio_init(gblink->serin, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
	furi_hal_gpio_init(gblink->clk, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);

	/* Set up interrupt on clock */
	/* This may not be needed after NFC refactor */
	furi_hal_gpio_remove_int_callback(gblink->clk);
	furi_hal_gpio_add_int_callback(gblink->clk, gblink_clk_callback, gblink);

	return gblink;
}

void gblink_free(void *handle)
{
	furi_assert(handle);
	struct gblink *gblink = handle;

	/* Remove interrupt, set IO to sane state */
	furi_hal_gpio_remove_int_callback(gblink->clk);
	furi_hal_gpio_init_simple(gblink->serin, GpioModeAnalog);
	furi_hal_gpio_init_simple(gblink->serout, GpioModeAnalog);
	furi_hal_gpio_init_simple(gblink->clk, GpioModeAnalog);
	free(gblink);
}
