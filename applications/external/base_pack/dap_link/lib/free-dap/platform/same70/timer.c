/*
 * Copyright (c) 2020, Alex Taradov <alex@taradov.com>
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

/*- Includes ----------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "same70.h"
#include "timer.h"

/*- Variables ---------------------------------------------------------------*/
static Timer *g_timer_list[TIMER_COUNT];

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
static inline void systick_start(void)
{
  SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

//-----------------------------------------------------------------------------
static inline void systick_stop(void)
{
  SysTick->CTRL = 0;
}

//-----------------------------------------------------------------------------
void timer_init(void)
{
  SysTick->VAL  = 0;
  SysTick->LOAD = (F_CPU / 1000) * TIMER_TICK;

  for (int i = 0; i < TIMER_COUNT; i++)
    g_timer_list[i] = NULL;

  systick_start();
}

//-----------------------------------------------------------------------------
void timer_start(Timer *timer)
{
  timer->expired  = false;
  timer->interval = (timer->interval < TIMER_TICK) ? TIMER_TICK : timer->interval;
  timer->timeout  = timer->interval / TIMER_TICK;

  systick_stop();

  for (int i = 0; i < TIMER_COUNT; i++)
  {
    if (g_timer_list[i] == NULL)
    {
      g_timer_list[i] = timer;
      break;
    }
  }

  systick_start();
}

//-----------------------------------------------------------------------------
void timer_stop(Timer *timer)
{
  systick_stop();

  for (int i = 0; i < TIMER_COUNT; i++)
  {
    if (g_timer_list[i] == timer)
    {
      g_timer_list[i] = NULL;
      break;
    }
  }

  systick_start();
}

//-----------------------------------------------------------------------------
void irq_handler_sys_tick(void)
{
  for (int i = 0; i < TIMER_COUNT; i++)
  {
    Timer *timer = g_timer_list[i];

    if (timer == NULL)
      continue;

    if (--timer->timeout > 0)
      continue;

    timer->expired = true;

    if (timer->repeat)
      timer->timeout = timer->interval / TIMER_TICK;
    else
      g_timer_list[i] = NULL;
  }
}

