/*
 * Copyright (c) 2021, Alex Taradov <alex@taradov.com>
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
    P##port->PDIO[pin] = 1;							\
    (void)HAL_GPIO_##name##_set;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_clr(void)				\
  {										\
    P##port->PDIO[pin] = 0;							\
    (void)HAL_GPIO_##name##_clr;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_toggle(void)				\
  {										\
    P##port->DOUT ^= GPIO_DOUT_DOUT##pin##_Msk;					\
    (void)HAL_GPIO_##name##_toggle;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_write(int value)				\
  {										\
    P##port->PDIO[pin] = (value > 0);						\
    (void)HAL_GPIO_##name##_write;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_in(void)					\
  {										\
    P##port->MODE = (P##port->MODE & ~GPIO_MODE_MODE##pin##_Msk);		\
    (void)HAL_GPIO_##name##_in;							\
  }										\
										\
  static inline void HAL_GPIO_##name##_out(void)				\
  {										\
    P##port->MODE = (P##port->MODE & ~GPIO_MODE_MODE##pin##_Msk) | 		\
        (1 << GPIO_MODE_MODE##pin##_Pos);					\
    (void)HAL_GPIO_##name##_out;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_odrain(void)				\
  {										\
    P##port->MODE = (P##port->MODE & ~GPIO_MODE_MODE##pin##_Msk) | 		\
        (2 << GPIO_MODE_MODE##pin##_Pos);					\
    (void)HAL_GPIO_##name##_odrain;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_pullup(void)				\
  {										\
    P##port->PUSEL = (P##port->PUSEL & ~GPIO_PUSEL_PUSEL##pin##_Msk) | 		\
        (1 << GPIO_PUSEL_PUSEL##pin##_Pos);					\
    (void)HAL_GPIO_##name##_pullup;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_pulldown(void)				\
  {										\
    P##port->PUSEL = (P##port->PUSEL & ~GPIO_PUSEL_PUSEL##pin##_Msk) | 		\
        (2 << GPIO_PUSEL_PUSEL##pin##_Pos);					\
    (void)HAL_GPIO_##name##_pulldown;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_pulldis(void)				\
  {										\
    P##port->PUSEL = (P##port->PUSEL & ~GPIO_PUSEL_PUSEL##pin##_Msk); 		\
    (void)HAL_GPIO_##name##_pulldis;						\
  }										\
										\
  static inline int HAL_GPIO_##name##_read(void)				\
  {										\
    return P##port->PDIO[pin];							\
    (void)HAL_GPIO_##name##_read;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_mfp(int value)				\
  {										\
    uint32_t mfp = (pin < 8) ? SYS->GP##port##_MFPL : SYS->GP##port##_MFPH;	\
    uint32_t offs = ((pin < 8) ? pin : (pin-8)) * 4;				\
    mfp = (mfp & ~(0xf << offs)) | (value << offs);				\
    if (pin < 8)								\
      SYS->GP##port##_MFPL = mfp;						\
    else									\
      SYS->GP##port##_MFPH = mfp;						\
    (void)HAL_GPIO_##name##_mfp;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_mfos(int value)				\
  {										\
    if (value)									\
      SYS->GP##port##_MFOS |= SYS_GPA_MFOS_MFOS##pin##_Msk;			\
    else									\
      SYS->GP##port##_MFOS &= ~SYS_GPA_MFOS_MFOS##pin##_Msk;			\
    (void)HAL_GPIO_##name##_mfos;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_smten(int value)				\
  {										\
    if (value)									\
      P##port->SMTEN |= GPIO_SMTEN_SMTEN##pin##_Msk;				\
    else									\
      P##port->SMTEN &= ~GPIO_SMTEN_SMTEN##pin##_Msk;				\
    (void)HAL_GPIO_##name##_smten;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_dinoff(int value)			\
  {										\
    if (value)									\
      P##port->DINOFF |= GPIO_DINOFF_DINOFF##pin##_Msk;				\
    else									\
      P##port->DINOFF &= ~GPIO_DINOFF_DINOFF##pin##_Msk;			\
    (void)HAL_GPIO_##name##_dinoff;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_dben(int value)				\
  {										\
    if (value)									\
      P##port->DBEN |= GPIO_DBEN_DBEN##pin##_Msk;				\
    else									\
      P##port->DBEN &= ~GPIO_DBEN_DBEN##pin##_Msk;				\
    (void)HAL_GPIO_##name##_dben;						\
  }										\
										\
  static inline void HAL_GPIO_##name##_slew(int value)				\
  {										\
    P##port->SLEWCTL = (P##port->SLEWCTL & ~GPIO_SLEWCTL_HSREN##pin##_Msk) |	\
        (value << GPIO_SLEWCTL_HSREN##pin##_Pos);				\
    (void)HAL_GPIO_##name##_slew;						\
  }										\

#endif // _HAL_GPIO_H_


