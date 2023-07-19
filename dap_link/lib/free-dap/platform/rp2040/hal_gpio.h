// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

/*- Definitions -------------------------------------------------------------*/
#define HAL_GPIO_PIN(name, port, pin, func)					\
  static inline void HAL_GPIO_##name##_set(void)				\
  {										\
    SIO->GPIO_OUT_SET = (1 << pin);						\
    (void)HAL_GPIO_##name##_set;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_clr(void)				\
  {										\
    SIO->GPIO_OUT_CLR = (1 << pin);						\
    (void)HAL_GPIO_##name##_clr;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_toggle(void)				\
  {										\
    SIO->GPIO_OUT_XOR = (1 << pin);						\
    (void)HAL_GPIO_##name##_toggle;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_write(int value)				\
  {										\
    if (value)									\
      SIO->GPIO_OUT_SET = (1 << pin);						\
    else									\
      SIO->GPIO_OUT_CLR = (1 << pin);						\
    (void)HAL_GPIO_##name##_write;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_in(void)					\
  {										\
    SIO->GPIO_OE_CLR = (1 << pin);						\
    IO_BANK##port->GPIO##pin##_CTRL = IO_BANK##port##_GPIO##pin##_CTRL_FUNCSEL_sio_##pin; \
    PADS_BANK##port##_CLR->GPIO##pin = PADS_BANK##port##_GPIO##pin##_PDE_Msk | PADS_BANK##port##_GPIO##pin##_PUE_Msk; \
    PADS_BANK##port##_SET->GPIO##pin = PADS_BANK##port##_GPIO##pin##_IE_Msk;	\
    (void)HAL_GPIO_##name##_in;							\
  }										\
										\
  static inline void HAL_GPIO_##name##_out(void)				\
  {										\
    SIO->GPIO_OE_SET = (1 << pin);						\
    IO_BANK##port->GPIO##pin##_CTRL = IO_BANK##port##_GPIO##pin##_CTRL_FUNCSEL_sio_##pin; \
    (void)HAL_GPIO_##name##_out;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_init(void)				\
  {										\
    IO_BANK##port->GPIO##pin##_CTRL = IO_BANK##port##_GPIO##pin##_CTRL_FUNCSEL_##func; \
    (void)HAL_GPIO_##name##_init;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_pullup(void)				\
  {										\
    PADS_BANK##port##_SET->GPIO##pin = PADS_BANK##port##_GPIO##pin##_PUE_Msk;	\
    PADS_BANK##port##_CLR->GPIO##pin = PADS_BANK##port##_GPIO##pin##_PDE_Msk;	\
    (void)HAL_GPIO_##name##_pullup;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_pulldown(void)				\
  {										\
    PADS_BANK##port##_SET->GPIO##pin = PADS_BANK##port##_GPIO##pin##_PDE_Msk;	\
    PADS_BANK##port##_CLR->GPIO##pin = PADS_BANK##port##_GPIO##pin##_PUE_Msk;	\
    (void)HAL_GPIO_##name##_pulldown;						\
  }										\
										\
  static inline int HAL_GPIO_##name##_read(void)				\
  {										\
    return (SIO->GPIO_IN & (1 << pin)) != 0;					\
    (void)HAL_GPIO_##name##_read;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_funcsel(int fn)				\
  {										\
    IO_BANK##port->GPIO##pin##_CTRL_b.FUNCSEL = fn; 				\
    (void)HAL_GPIO_##name##_funcsel;						\
  }										\

#endif // _HAL_GPIO_H_

