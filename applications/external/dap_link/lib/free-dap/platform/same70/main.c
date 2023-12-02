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
#include <stdalign.h>
#include <string.h>
#include "same70.h"
#include "hal_gpio.h"
#include "timer.h"
#include "usb.h"
#include "dap.h"
#include "dap_config.h"

/*- Definitions -------------------------------------------------------------*/
#define STATUS_TIMEOUT         250 // ms

HAL_GPIO_PIN(LED,  D, 8)

/*- Variables ---------------------------------------------------------------*/
static uint8_t app_request_buffer[DAP_CONFIG_PACKET_COUNT][DAP_CONFIG_PACKET_SIZE];
static bool app_request_valid[DAP_CONFIG_PACKET_COUNT];
static bool app_request_pending;
static int  app_request_wr_ptr;
static int  app_request_rd_ptr;

static uint8_t app_response_buffer[DAP_CONFIG_PACKET_COUNT][DAP_CONFIG_PACKET_SIZE];
static bool app_response_valid[DAP_CONFIG_PACKET_COUNT];
static bool app_response_pending;
static int  app_response_wr_ptr;
static int  app_response_rd_ptr;

static Timer app_status_timer;

static bool app_dap_event = false;

/*- Prototypes --------------------------------------------------------------*/
static void receive_request(void);

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
static void sys_init(void)
{
  // Disable watchdog
  WDT->WDT_MR = WDT_MR_WDDIS;

  // Set flash wait states to maximum for 150 MHz operation
  EFC->EEFC_FMR = EEFC_FMR_FWS(6) | EEFC_FMR_CLOE;

  // Enable 12 MHz clock
  PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTBY;
  PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTBY | CKGR_MOR_MOSCSEL;
  while (!(PMC->PMC_SR & PMC_SR_MOSCSELS));

  // Setup PLL (12 MHz * 25 = 300 MHz)
  PMC->CKGR_PLLAR = CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(25-1) |
      CKGR_PLLAR_PLLACOUNT(0x3f) | CKGR_PLLAR_DIVA(1);
  while (!(PMC->PMC_SR & PMC_SR_LOCKA));

  // Switch main clock to PLL (two step process)
  PMC->PMC_MCKR = PMC_MCKR_CSS_MAIN_CLK | PMC_MCKR_MDIV_PCK_DIV2;
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY));

  PMC->PMC_MCKR = PMC_MCKR_CSS_PLLA_CLK | PMC_MCKR_MDIV_PCK_DIV2;
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY));

  // Enable PIOA, PIOB, PIOC, PIOD and PIOE
  PMC->PMC_PCER0 = PMC_PCER0_PID10 | PMC_PCER0_PID11 | PMC_PCER0_PID12 |
      PMC_PCER0_PID16 | PMC_PCER0_PID17;

  // Disable altenate functions on some pins
  MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4 | CCFG_SYSIO_SYSIO5;

  SCB_EnableICache();
}

//-----------------------------------------------------------------------------
__attribute__ ((noinline, section(".ramfunc")))
static void read_uid(uint32_t *uid)
{
  uint32_t *flash = (uint32_t *)IFLASH_ADDR;

  __disable_irq();

  EFC->EEFC_FMR |= EEFC_FMR_SCOD;

  EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_STUI;
  while (EEFC_FSR_FRDY == (EFC->EEFC_FSR & EEFC_FSR_FRDY));

  for (int i = 0; i < 4; i++)
    uid[i] = flash[i];

  EFC->EEFC_FCR = EEFC_FCR_FKEY_PASSWD | EEFC_FCR_FCMD_SPUI;
  while (0 == (EFC->EEFC_FSR & EEFC_FSR_FRDY));

  EFC->EEFC_FMR &= ~EEFC_FMR_SCOD;

  __enable_irq();
}

//-----------------------------------------------------------------------------
static void serial_number_init(void)
{
  alignas(4) uint8_t uid[16];
  uint64_t sn = 0;

  read_uid((uint32_t *)uid);

  for (int i = 0; i < 16; i++)
    sn ^= (uint64_t)(uid[i] - 0x30) << (i * 3);

  for (int i = 0; i < 12; i++)
    usb_serial_number[i] = "0123456789ABCDEF"[(sn >> (i * 4)) & 0xf];

  usb_serial_number[12] = 0;
}

//-----------------------------------------------------------------------------
bool usb_class_handle_request(usb_request_t *request)
{
  return usb_hid_handle_request(request);
}

//-----------------------------------------------------------------------------
void usb_configuration_callback(int config)
{
  app_request_pending  = false;
  app_request_wr_ptr   = 0;
  app_request_rd_ptr   = 0;

  app_response_pending = false;
  app_response_wr_ptr  = 0;
  app_response_rd_ptr  = 0;

  for (int i = 0; i < DAP_CONFIG_PACKET_COUNT; i++)
  {
    app_request_valid[i]  = false;
    app_response_valid[i] = false;
  }

  receive_request();

  (void)config;
}

//-----------------------------------------------------------------------------
static void receive_request(void)
{
  if (app_request_pending)
    return;

  if (app_request_valid[app_request_wr_ptr])
    return;

  app_request_pending = true;

  usb_hid_recv(app_request_buffer[app_request_wr_ptr], DAP_CONFIG_PACKET_SIZE);
}

//-----------------------------------------------------------------------------
void usb_hid_recv_callback(int size)
{
  if (dap_filter_request(app_request_buffer[app_request_wr_ptr]))
  {
    app_request_valid[app_request_wr_ptr] = true;
    app_request_wr_ptr = (app_request_wr_ptr + 1) % DAP_CONFIG_PACKET_COUNT;
  }

  app_request_pending = false;

  receive_request();

  (void)size;
}

//-----------------------------------------------------------------------------
static void send_response(void)
{
  if (app_response_pending)
    return;

  if (!app_response_valid[app_response_rd_ptr])
    return;

  app_response_pending = true;

  usb_hid_send(app_response_buffer[app_response_rd_ptr], DAP_CONFIG_PACKET_SIZE);
}

//-----------------------------------------------------------------------------
void usb_hid_send_callback(void)
{
  app_response_pending = false;
  app_response_valid[app_response_rd_ptr] = false;
  app_response_rd_ptr = (app_response_rd_ptr + 1) % DAP_CONFIG_PACKET_COUNT;

  send_response();
}

//-----------------------------------------------------------------------------
static void dap_task(void)
{
  if (!app_request_valid[app_request_rd_ptr])
    return;

  dap_process_request(app_request_buffer[app_request_rd_ptr], DAP_CONFIG_PACKET_SIZE,
      app_response_buffer[app_response_wr_ptr], DAP_CONFIG_PACKET_SIZE);

  app_response_valid[app_response_wr_ptr] = true;
  app_response_wr_ptr = (app_response_wr_ptr + 1) % DAP_CONFIG_PACKET_COUNT;

  send_response();

  app_request_valid[app_request_rd_ptr] = false;
  app_request_rd_ptr = (app_request_rd_ptr + 1) % DAP_CONFIG_PACKET_COUNT;

  receive_request();

  app_dap_event = true;
}

//-----------------------------------------------------------------------------
static void status_timer_task(void)
{
  if (!app_status_timer.expired)
    return;

  if (app_dap_event)
    HAL_GPIO_LED_toggle();
  else
    HAL_GPIO_LED_clr();

  app_dap_event = false;
  app_status_timer.expired = false;
}

//-----------------------------------------------------------------------------
int main(void)
{
  sys_init();
  timer_init();
  serial_number_init();
  usb_init();
  usb_hid_init();
  dap_init();

  app_status_timer.interval = STATUS_TIMEOUT;
  app_status_timer.repeat = true;
  timer_start(&app_status_timer);

  HAL_GPIO_LED_out();
  HAL_GPIO_LED_clr();

  while (1)
  {
    dap_task();
    status_timer_task();
  }

  return 0;
}

