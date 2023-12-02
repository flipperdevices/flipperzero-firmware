/*
 * Copyright (c) 2017, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

/*- Definitions -------------------------------------------------------------*/
#define HAL_GPIO_PIN(name, port, pin)						\
  static inline void HAL_GPIO_##name##_set(void)				\
  {										\
    PIO##port->PIO_SODR = (1 << pin);						\
    (void)HAL_GPIO_##name##_set;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_clr(void)				\
  {										\
    PIO##port->PIO_CODR = (1 << pin);						\
    (void)HAL_GPIO_##name##_clr;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_toggle(void)				\
  {										\
    if (PIO##port->PIO_ODSR & (1 << pin))					\
      PIO##port->PIO_CODR = (1 << pin);						\
    else									\
      PIO##port->PIO_SODR = (1 << pin);						\
    (void)HAL_GPIO_##name##_toggle;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_write(int value)				\
  {										\
    if (value)									\
      PIO##port->PIO_SODR = (1 << pin);						\
    else									\
      PIO##port->PIO_CODR = (1 << pin);						\
    (void)HAL_GPIO_##name##_write;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_in(void)					\
  {										\
    PIO##port->PIO_PER = (1 << pin);						\
    PIO##port->PIO_ODR = (1 << pin);						\
    (void)HAL_GPIO_##name##_in;							\
  }										\
										\
  static inline void HAL_GPIO_##name##_out(void)				\
  {										\
    PIO##port->PIO_PER = (1 << pin);						\
    PIO##port->PIO_OER = (1 << pin);						\
    (void)HAL_GPIO_##name##_out;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_pullup(void)				\
  {										\
    PIO##port->PIO_PUER = (1 << pin);						\
    (void)HAL_GPIO_##name##_pullup;						\
  }										\
										\
  static inline int HAL_GPIO_##name##_read(void)				\
  {										\
    return (PIO##port->PIO_PDSR & (1 << pin)) != 0;				\
    (void)HAL_GPIO_##name##_read;						\
  }										\
										\
  static inline int HAL_GPIO_##name##_state(void)				\
  {										\
    return (PIO##port->PIO_ODSR & (1 << pin)) != 0;				\
    (void)HAL_GPIO_##name##_state;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_abcd(int abcd)				\
  {										\
    if (abcd & 1)								\
      PIO##port->PIO_ABCDSR[0] |= (1 << pin);					\
    else									\
      PIO##port->PIO_ABCDSR[0] &= ~(1 << pin);					\
    if (abcd & 2)								\
      PIO##port->PIO_ABCDSR[1] |= (1 << pin);					\
    else									\
      PIO##port->PIO_ABCDSR[1] &= ~(1 << pin);					\
    PIO##port->PIO_PDR = (1 << pin);						\
    (void)HAL_GPIO_##name##_abcd;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_mden(void)				\
  {										\
    PIO##port->PIO_MDER = (1 << pin);						\
    (void)HAL_GPIO_##name##_mden;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_mddis(void)				\
  {										\
    PIO##port->PIO_MDDR = (1 << pin);						\
    (void)HAL_GPIO_##name##_mddis;						\
  }										\

#endif // _HAL_GPIO_H_

