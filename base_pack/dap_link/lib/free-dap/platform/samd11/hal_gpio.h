// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2014-2016, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

/*- Definitions -------------------------------------------------------------*/
#define HAL_GPIO_PORTA       0
#define HAL_GPIO_PORTB       1
#define HAL_GPIO_PORTC       2

#define HAL_GPIO_PMUX_A      0
#define HAL_GPIO_PMUX_B      1
#define HAL_GPIO_PMUX_C      2
#define HAL_GPIO_PMUX_D      3
#define HAL_GPIO_PMUX_E      4
#define HAL_GPIO_PMUX_F      5
#define HAL_GPIO_PMUX_G      6
#define HAL_GPIO_PMUX_H      7
#define HAL_GPIO_PMUX_I      8

#define HAL_GPIO_PIN(name, port, pin)						\
  static inline void HAL_GPIO_##name##_set(void)				\
  {										\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].OUTSET.reg = (1 << pin);		\
    (void)HAL_GPIO_##name##_set;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_clr(void)				\
  {										\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].OUTCLR.reg = (1 << pin);		\
    (void)HAL_GPIO_##name##_clr;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_toggle(void)				\
  {										\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].OUTTGL.reg = (1 << pin);		\
    (void)HAL_GPIO_##name##_toggle;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_write(int value)				\
  {										\
    if (value)									\
      PORT_IOBUS->Group[HAL_GPIO_PORT##port].OUTSET.reg = (1 << pin);		\
    else									\
      PORT_IOBUS->Group[HAL_GPIO_PORT##port].OUTCLR.reg = (1 << pin);		\
    (void)HAL_GPIO_##name##_write;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_in(void)					\
  {										\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].DIRCLR.reg = (1 << pin);		\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].PINCFG[pin].reg |= PORT_PINCFG_INEN;	\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].PINCFG[pin].reg &= ~PORT_PINCFG_PULLEN;\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].CTRL.reg |= (1 << pin);		\
    (void)HAL_GPIO_##name##_in;							\
  }										\
										\
  static inline void HAL_GPIO_##name##_out(void)				\
  {										\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].DIRSET.reg = (1 << pin);		\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].PINCFG[pin].reg |= PORT_PINCFG_INEN;	\
    (void)HAL_GPIO_##name##_out;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_pullup(void)				\
  {										\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].OUTSET.reg = (1 << pin);		\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].PINCFG[pin].reg |= PORT_PINCFG_PULLEN; \
    (void)HAL_GPIO_##name##_pullup;						\
  }										\
										\
  static inline int HAL_GPIO_##name##_read(void)				\
  {										\
    return (PORT_IOBUS->Group[HAL_GPIO_PORT##port].IN.reg & (1 << pin)) != 0;	\
    (void)HAL_GPIO_##name##_read;						\
  }										\
										\
  static inline int HAL_GPIO_##name##_state(void)				\
  {										\
    return (PORT_IOBUS->Group[HAL_GPIO_PORT##port].DIR.reg & (1 << pin)) != 0;	\
    (void)HAL_GPIO_##name##_state;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_pmuxen(int mux)				\
  {										\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].PINCFG[pin].reg |= PORT_PINCFG_PMUXEN; \
    if (pin & 1)								\
      PORT_IOBUS->Group[HAL_GPIO_PORT##port].PMUX[pin>>1].bit.PMUXO = mux;	\
    else									\
      PORT_IOBUS->Group[HAL_GPIO_PORT##port].PMUX[pin>>1].bit.PMUXE = mux;	\
    (void)HAL_GPIO_##name##_pmuxen;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_pmuxdis(void)				\
  {										\
    PORT_IOBUS->Group[HAL_GPIO_PORT##port].PINCFG[pin].reg &= ~PORT_PINCFG_PMUXEN; \
    (void)HAL_GPIO_##name##_pmuxdis;						\
  }										\

#endif // _HAL_GPIO_H_
