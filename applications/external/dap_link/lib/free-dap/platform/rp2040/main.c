// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rp2040.h"
#include "hal_gpio.h"
#include "usb.h"
#include "uart.h"
#include "dap.h"
#include "dap_config.h"

/*- Definitions -------------------------------------------------------------*/
#define USB_BUFFER_SIZE        64
#define UART_WAIT_TIMEOUT      10 // ms
#define STATUS_TIMEOUT         250 // ms

#define F_REF          12000000
#define F_CPU          120000000
#define F_PER          120000000
#define F_RTC          (F_REF / 256)
#define F_TICK         1000000

/*- Variables ---------------------------------------------------------------*/
static uint8_t app_req_buf_hid[DAP_CONFIG_PACKET_SIZE];
static uint8_t app_resp_buf_hid[DAP_CONFIG_PACKET_SIZE];
static uint8_t app_req_buf_bulk[DAP_CONFIG_PACKET_SIZE];
static uint8_t app_resp_buf_bulk[DAP_CONFIG_PACKET_SIZE];
static uint8_t app_recv_buffer[USB_BUFFER_SIZE];
static uint8_t app_send_buffer[USB_BUFFER_SIZE];
static int app_recv_buffer_size = 0;
static int app_recv_buffer_ptr = 0;
static int app_send_buffer_ptr = 0;
static bool app_send_buffer_free = true;
static bool app_send_zlp = false;
static uint64_t app_system_time = 0;
static uint64_t app_uart_timeout = 0;
static uint64_t app_status_timeout = 0;
static uint64_t app_break_timeout = 0;
static bool app_dap_event = false;
static bool app_vcp_event = false;
static bool app_vcp_open = false;

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
static void sys_init(void)
{
  // Enable XOSC
  XOSC->CTRL     = (XOSC_CTRL_FREQ_RANGE_1_15MHZ << XOSC_CTRL_FREQ_RANGE_Pos);
  XOSC->STARTUP  = 47; // ~1 ms @ 12 MHz
  XOSC_SET->CTRL = (XOSC_CTRL_ENABLE_ENABLE << XOSC_CTRL_ENABLE_Pos);
  while (0 == (XOSC->STATUS & XOSC_STATUS_STABLE_Msk));

  // Setup SYS PLL for 12 MHz * 40 / 4 / 1 = 120 MHz
  RESETS_CLR->RESET = RESETS_RESET_pll_sys_Msk;
  while (0 == RESETS->RESET_DONE_b.pll_sys);

  PLL_SYS->CS = (1 << PLL_SYS_CS_REFDIV_Pos);
  PLL_SYS->FBDIV_INT = 40;
  PLL_SYS->PRIM = (4 << PLL_SYS_PRIM_POSTDIV1_Pos) | (1 << PLL_SYS_PRIM_POSTDIV2_Pos);

  PLL_SYS_CLR->PWR = PLL_SYS_PWR_VCOPD_Msk | PLL_SYS_PWR_PD_Msk;
  while (0 == PLL_SYS->CS_b.LOCK);

  PLL_SYS_CLR->PWR = PLL_SYS_PWR_POSTDIVPD_Msk;

  // Setup USB PLL for 12 MHz * 36 / 3 / 3 = 48 MHz
  RESETS_CLR->RESET = RESETS_RESET_pll_usb_Msk;
  while (0 == RESETS->RESET_DONE_b.pll_usb);

  PLL_USB->CS = (1 << PLL_SYS_CS_REFDIV_Pos);
  PLL_USB->FBDIV_INT = 36;
  PLL_USB->PRIM = (3 << PLL_SYS_PRIM_POSTDIV1_Pos) | (3 << PLL_SYS_PRIM_POSTDIV2_Pos);

  PLL_USB_CLR->PWR = PLL_SYS_PWR_VCOPD_Msk | PLL_SYS_PWR_PD_Msk;
  while (0 == PLL_USB->CS_b.LOCK);

  PLL_USB_CLR->PWR = PLL_SYS_PWR_POSTDIVPD_Msk;

  // Switch clocks to their final socurces
  CLOCKS->CLK_REF_CTRL = (CLOCKS_CLK_REF_CTRL_SRC_xosc_clksrc << CLOCKS_CLK_REF_CTRL_SRC_Pos);

  CLOCKS->CLK_SYS_CTRL = (CLOCKS_CLK_SYS_CTRL_AUXSRC_clksrc_pll_sys << CLOCKS_CLK_SYS_CTRL_AUXSRC_Pos);
  CLOCKS_SET->CLK_SYS_CTRL = (CLOCKS_CLK_SYS_CTRL_SRC_clksrc_clk_sys_aux << CLOCKS_CLK_SYS_CTRL_SRC_Pos);

  CLOCKS->CLK_PERI_CTRL = CLOCKS_CLK_PERI_CTRL_ENABLE_Msk |
      (CLOCKS_CLK_PERI_CTRL_AUXSRC_clk_sys << CLOCKS_CLK_PERI_CTRL_AUXSRC_Pos);

  CLOCKS->CLK_USB_CTRL = CLOCKS_CLK_USB_CTRL_ENABLE_Msk |
      (CLOCKS_CLK_USB_CTRL_AUXSRC_clksrc_pll_usb << CLOCKS_CLK_USB_CTRL_AUXSRC_Pos);

  CLOCKS->CLK_ADC_CTRL = CLOCKS_CLK_ADC_CTRL_ENABLE_Msk |
      (CLOCKS_CLK_ADC_CTRL_AUXSRC_clksrc_pll_usb << CLOCKS_CLK_ADC_CTRL_AUXSRC_Pos);

  CLOCKS->CLK_RTC_DIV = (256 << CLOCKS_CLK_RTC_DIV_INT_Pos); // 12MHz / 256 = 46875 Hz
  CLOCKS->CLK_RTC_CTRL = CLOCKS_CLK_RTC_CTRL_ENABLE_Msk |
      (CLOCKS_CLK_RTC_CTRL_AUXSRC_xosc_clksrc << CLOCKS_CLK_ADC_CTRL_AUXSRC_Pos);

  // Configure 1 us tick for watchdog and timer
  WATCHDOG->TICK = ((F_REF/F_TICK) << WATCHDOG_TICK_CYCLES_Pos) | WATCHDOG_TICK_ENABLE_Msk;

  // Enable GPIOs
  RESETS_CLR->RESET = RESETS_RESET_io_bank0_Msk | RESETS_RESET_pads_bank0_Msk;
  while (0 == RESETS->RESET_DONE_b.io_bank0 || 0 == RESETS->RESET_DONE_b.pads_bank0);
}

//-----------------------------------------------------------------------------
static void serial_number_init(void)
{
  volatile uint8_t *uid = (volatile uint8_t *)0x20041f01;
  uint32_t sn = 5381;

  for (int i = 0; i < 16; i++)
    sn = ((sn << 5) + sn) ^ uid[i];

  for (int i = 0; i < 8; i++)
    usb_serial_number[i] = "0123456789ABCDEF"[(sn >> (i * 4)) & 0xf];

  usb_serial_number[8] = 0;
}

//-----------------------------------------------------------------------------
static void sys_time_init(void)
{
  SysTick->VAL  = 0;
  SysTick->LOAD = 1000; // 1 ms
  SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
  app_system_time = 0;
}

//-----------------------------------------------------------------------------
static void sys_time_task(void)
{
  if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
    app_system_time++;
}

//-----------------------------------------------------------------------------
static void tx_task(void)
{
  while (app_recv_buffer_size)
  {
    if (!uart_write_byte(app_recv_buffer[app_recv_buffer_ptr]))
      break;

    app_recv_buffer_ptr++;
    app_recv_buffer_size--;
    app_vcp_event = true;

    if (0 == app_recv_buffer_size)
      usb_cdc_recv(app_recv_buffer, sizeof(app_recv_buffer));
  }
}

//-----------------------------------------------------------------------------
static void send_buffer(void)
{
  app_send_buffer_free = false;
  app_send_zlp = (USB_BUFFER_SIZE == app_send_buffer_ptr);

  usb_cdc_send(app_send_buffer, app_send_buffer_ptr);

  app_send_buffer_ptr = 0;
}

//-----------------------------------------------------------------------------
static void rx_task(void)
{
  int byte;

  if (!app_send_buffer_free)
    return;

  while (uart_read_byte(&byte))
  {
    int state = (byte >> 8) & 0xff;

    app_uart_timeout = app_system_time + UART_WAIT_TIMEOUT;
    app_vcp_event = true;

    if (state)
    {
      usb_cdc_set_state(state);
    }
    else
    {
      app_send_buffer[app_send_buffer_ptr++] = byte;

      if (USB_BUFFER_SIZE == app_send_buffer_ptr)
      {
        send_buffer();
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
static void break_task(void)
{
  if (app_break_timeout && app_system_time > app_break_timeout)
  {
    uart_set_break(false);
    app_break_timeout = 0;
  }
}

//-----------------------------------------------------------------------------
static void uart_timer_task(void)
{
  if (app_uart_timeout && app_system_time > app_uart_timeout)
  {
    if (app_send_zlp || app_send_buffer_ptr)
      send_buffer();

    app_uart_timeout = 0;
  }
}

//-----------------------------------------------------------------------------
void usb_cdc_line_coding_updated(usb_cdc_line_coding_t *line_coding)
{
  uart_init(line_coding);
}

//-----------------------------------------------------------------------------
void usb_cdc_control_line_state_update(int line_state)
{
  bool status = line_state & USB_CDC_CTRL_SIGNAL_DTE_PRESENT;

  app_vcp_open        = status;
  app_send_buffer_ptr = 0;
  app_uart_timeout    = 0;
  app_break_timeout   = 0;

  if (app_vcp_open)
    uart_init(usb_cdc_get_line_coding());
  else
    uart_close();
}

//-----------------------------------------------------------------------------
void usb_cdc_send_break(int duration)
{
  if (USB_CDC_BREAK_DURATION_DISABLE == duration)
  {
    app_break_timeout = 0;
    uart_set_break(false);
  }
  else if (USB_CDC_BREAK_DURATION_INFINITE == duration)
  {
    app_break_timeout = 0;
    uart_set_break(true);
  }
  else
  {
    app_break_timeout = app_system_time + duration;
    uart_set_break(true);
  }
}

//-----------------------------------------------------------------------------
void usb_cdc_send_callback(void)
{
  app_send_buffer_free = true;
}

//-----------------------------------------------------------------------------
void usb_cdc_recv_callback(int size)
{
  app_recv_buffer_ptr = 0;
  app_recv_buffer_size = size;
}

//-----------------------------------------------------------------------------
void usb_hid_send_callback(void)
{
  usb_hid_recv(app_req_buf_hid, DAP_CONFIG_PACKET_SIZE);
}

//-----------------------------------------------------------------------------
void usb_hid_recv_callback(int size)
{
  app_dap_event = true;
  dap_process_request(app_req_buf_hid, sizeof(app_req_buf_hid),
      app_resp_buf_hid, sizeof(app_resp_buf_hid));
  usb_hid_send(app_resp_buf_hid, sizeof(app_resp_buf_hid));
  (void)size;
}

//-----------------------------------------------------------------------------
static void usb_bulk_send_callback(void)
{
  usb_recv(USB_BULK_EP_RECV, app_req_buf_bulk, sizeof(app_req_buf_bulk));
}

//-----------------------------------------------------------------------------
static void usb_bulk_recv_callback(int size)
{
  app_dap_event = true;
  size = dap_process_request(app_req_buf_bulk, size,
      app_resp_buf_bulk, sizeof(app_resp_buf_bulk));
  usb_send(USB_BULK_EP_SEND, app_resp_buf_bulk, size);
}

//-----------------------------------------------------------------------------
void usb_configuration_callback(int config)
{
  usb_set_send_callback(USB_BULK_EP_SEND, usb_bulk_send_callback);
  usb_set_recv_callback(USB_BULK_EP_RECV, usb_bulk_recv_callback);

  usb_cdc_recv(app_recv_buffer, sizeof(app_recv_buffer));
  usb_hid_recv(app_req_buf_hid, sizeof(app_req_buf_hid));
  usb_recv(USB_BULK_EP_RECV, app_req_buf_bulk, sizeof(app_req_buf_bulk));

  app_send_buffer_free = true;
  app_send_buffer_ptr = 0;

  (void)config;
}

//-----------------------------------------------------------------------------
static void status_timer_task(void)
{
  if (app_system_time < app_status_timeout)
    return;

  app_status_timeout = app_system_time + STATUS_TIMEOUT;

  if (app_dap_event)
    HAL_GPIO_DAP_STATUS_toggle();
  else
    HAL_GPIO_DAP_STATUS_set();

  if (app_vcp_event)
    HAL_GPIO_VCP_STATUS_toggle();
  else
    HAL_GPIO_VCP_STATUS_write(app_vcp_open);

  app_dap_event = false;
  app_vcp_event = false;
}

//-----------------------------------------------------------------------------
int main(void)
{
  sys_init();
  sys_time_init();
  dap_init();
  usb_init();
  usb_cdc_init();
  usb_hid_init();
  serial_number_init();

  app_status_timeout = STATUS_TIMEOUT;

  HAL_GPIO_VCP_STATUS_out();
  HAL_GPIO_VCP_STATUS_clr();

  HAL_GPIO_DAP_STATUS_out();
  HAL_GPIO_DAP_STATUS_set();

  while (1)
  {
    sys_time_task();
    usb_task();
    tx_task();
    rx_task();
    break_task();
    uart_timer_task();
    status_timer_task();
  }

  return 0;
}
