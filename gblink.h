// SPDX-License-Identifier: BSD-2-Clause
// Copyright (c) 2023 KBEmbedded

#ifndef __GBLINK_H__
#define __GBLINK_H__

#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	/* Flipper drives the clock line */
	/* Unsupported at this time */
	GBLINK_INTERNAL_CLK,
	/* Game Boy drives the clock line */
	GBLINK_EXTERNAL_CLK,
} gblink_clk_source;

/* Currently unused */
typedef enum {
	GBLINK_MODE_GBC,
	GBLINK_MODE_GBA,
} gblink_mode;

/* Should this just be a macro? */
/* This pretty much only applies to GBC, OG GB is 8192 Hz only */
/* This is only for TX */
typedef enum {
	GBLINK_SPD_8192HZ,
	GBLINK_SPD_16384HZ,
	GBLINK_SPD_262144HZ,
	GBLINK_SPD_524288HZ,
} gblink_speed;

struct gblink_pins {
        const GpioPin *serin;
        const GpioPin *serout;
        const GpioPin *clk;
        const GpioPin *sd;
};

typedef enum {
	PINOUT_ORIGINAL,
	PINOUT_MALVEKE_EXT1,
	PINOUT_COUNT,
} gblink_pinout;

extern const struct gblink_pins common_pinouts[PINOUT_COUNT];

struct gblink_def {
	struct gblink_pins *pins;
	gblink_clk_source source;
	gblink_mode mode;
	void (*callback)(void* cb_context, uint8_t in);
	void *cb_context;
};

void gblink_clk_source_set(void *handle, int clk_source);

void gblink_speed_set(void *handle, gblink_speed speed);

void gblink_timeout_set(void *handle, uint32_t us);

void gblink_transfer(void *handle, uint8_t val);

void gblink_nobyte_set(void *handle, uint8_t val);

void gblink_int_enable(void *handle);

void gblink_int_disable(void *handle);

void *gblink_alloc(struct gblink_def *gblink_def);

void gblink_free(void *handle);

#ifdef __cplusplus
}
#endif

#endif // __GBLINK_H__
