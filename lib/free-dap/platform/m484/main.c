// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "M480.h"
#include "hal_gpio.h"
#include "hal_config.h"
#include "usb.h"
#include "uart.h"
#include "dap.h"
#include "dap_config.h"

/*- Definitions -------------------------------------------------------------*/
#define F_HXT                  12000000
#define F_HCLK                 192000000
#define F_PCLK0                (F_HCLK / 2)
#define F_PCLK1                (F_HCLK / 2)

#define USB_BUFFER_SIZE        USB_VCP_DATA_EP_SIZE
#define UART_WAIT_TIMEOUT      10 // ms
#define STATUS_TIMEOUT         250 // ms

/*- Variables ---------------------------------------------------------------*/
static alignas(4) uint8_t app_req_buf_hid[DAP_CONFIG_PACKET_SIZE];
static alignas(4) uint8_t app_resp_buf_hid[DAP_CONFIG_PACKET_SIZE];
static alignas(4) uint8_t app_req_buf_bulk[DAP_CONFIG_PACKET_SIZE];
static alignas(4) uint8_t app_resp_buf_bulk[DAP_CONFIG_PACKET_SIZE];
static uint64_t app_system_time = 0;
static uint64_t app_status_timeout = 0;
static bool app_dap_event = false;

#ifdef HAL_CONFIG_ENABLE_VCP
static alignas(4) uint8_t app_recv_buffer[USB_BUFFER_SIZE];
static alignas(4) uint8_t app_send_buffer[USB_BUFFER_SIZE];
static int app_recv_buffer_size = 0;
static int app_recv_buffer_ptr = 0;
static int app_send_buffer_ptr = 0;
static bool app_send_buffer_free = true;
static bool app_send_zlp = false;
static uint64_t app_uart_timeout = 0;
static uint64_t app_break_timeout = 0;
static bool app_vcp_event = false;
static bool app_vcp_open = false;
#endif

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
static void sys_init(void)
{
  // Unlock the registers
  SYS->REGLCTL = 0x59;
  SYS->REGLCTL = 0x16;
  SYS->REGLCTL = 0x88;

  // Set flash wait states
  SET_FIELD(FMC, CYCCTL, CYCLE, 8);

  // Configure power down bias
  CLK->LDOCTL |= CLK_LDOCTL_PDBIASEN_Msk;

  // Switch PF4-PF11 to GPIO
  CLK->APBCLK0 |= CLK_APBCLK0_RTCCKEN_Msk;
  RTC->GPIOCTL0 &= ~(RTC_GPIOCTL0_CTLSEL0_Msk | RTC_GPIOCTL0_CTLSEL1_Msk |
      RTC_GPIOCTL0_CTLSEL2_Msk | RTC_GPIOCTL0_CTLSEL3_Msk);
  RTC->GPIOCTL1 &= ~(RTC_GPIOCTL1_CTLSEL4_Msk | RTC_GPIOCTL1_CTLSEL5_Msk |
      RTC_GPIOCTL1_CTLSEL6_Msk | RTC_GPIOCTL1_CTLSEL7_Msk);
  CLK->APBCLK0 &= ~CLK_APBCLK0_RTCCKEN_Msk;

  // Enable 12 MHz oscillator
  PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk); // Switch PF2 and PF3 to inputs
  CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;
  while (0 == (CLK->STATUS & CLK_STATUS_HXTSTB_Msk));

  // Enable PLL (12 MHz / 2 * 32 = 192 MHz)
  CLK->PLLCTL = ((2-1) << CLK_PLLCTL_INDIV_Pos) | ((32-2) << CLK_PLLCTL_FBDIV_Pos) |
      (1 << CLK_PLLCTL_OUTDIV_Pos);
  while (0 == (CLK->STATUS & CLK_STATUS_PLLSTB_Msk));

  // Switch the system to PLL
  CLK->PCLKDIV = (1/*1/2 HCLK*/ << CLK_PCLKDIV_APB0DIV_Pos) |
                 (1/*1/2 HCLK*/ << CLK_PCLKDIV_APB1DIV_Pos);
  SET_FIELD(CLK, CLKSEL0, HCLKSEL, 2/*PLL*/);
}

//-----------------------------------------------------------------------------
static void serial_number_init(void)
{
  uint32_t wuid[3];
  uint8_t *uid = (uint8_t *)wuid;
  uint32_t sn = 5381;

  FMC->ISPCTL =  FMC_ISPCTL_ISPEN_Msk;

  for (int i = 0; i < 3; i++)
  {
    FMC->ISPCMD  = 0x04/*Read Unique ID*/;
    FMC->ISPADDR = i*4;
    FMC->ISPTRG  = FMC_ISPTRG_ISPGO_Msk;
    while (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk);
    wuid[i] = FMC->ISPDAT;
  }

  for (int i = 0; i < 12; i++)
    sn = ((sn << 5) + sn) ^ uid[i];

  for (int i = 0; i < 8; i++)
    usb_serial_number[i] = "0123456789ABCDEF"[(sn >> (i * 4)) & 0xf];

  usb_serial_number[8] = 0;
}

//-----------------------------------------------------------------------------
static void sys_time_init(void)
{
  SysTick->VAL  = 0;
  SysTick->LOAD = F_HCLK / 1000ul;
  SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
  app_system_time = 0;
}

//-----------------------------------------------------------------------------
static void sys_time_task(void)
{
  if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
    app_system_time++;
}

#ifdef HAL_CONFIG_ENABLE_VCP
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
#endif // HAL_CONFIG_ENABLE_VCP

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

  usb_hid_recv(app_req_buf_hid, sizeof(app_req_buf_hid));
  usb_recv(USB_BULK_EP_RECV, app_req_buf_bulk, sizeof(app_req_buf_bulk));

#ifdef HAL_CONFIG_ENABLE_VCP
  usb_cdc_recv(app_recv_buffer, sizeof(app_recv_buffer));

  app_send_buffer_free = true;
  app_send_buffer_ptr = 0;
#endif

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

  app_dap_event = false;

#ifdef HAL_CONFIG_ENABLE_VCP
  if (app_vcp_event)
    HAL_GPIO_VCP_STATUS_toggle();
  else
    HAL_GPIO_VCP_STATUS_write(app_vcp_open);

  app_vcp_event = false;
#endif
}

//-----------------------------------------------------------------------------
__attribute__ ((noinline))
int main(void)
{
  sys_init();
  sys_time_init();
  dap_init();
  usb_init();
#ifdef HAL_CONFIG_ENABLE_VCP
  usb_cdc_init();
#endif
  usb_hid_init();
  serial_number_init();

  app_status_timeout = STATUS_TIMEOUT;

#ifdef HAL_CONFIG_ENABLE_VCP
  HAL_GPIO_VCP_STATUS_out();
  HAL_GPIO_VCP_STATUS_clr();
#endif

  HAL_GPIO_DAP_STATUS_out();
  HAL_GPIO_DAP_STATUS_set();

  HAL_GPIO_BOOT_ENTER_in();
  HAL_GPIO_BOOT_ENTER_pullup();

  while (1)
  {
    sys_time_task();
    status_timer_task();
    usb_task();

#ifdef HAL_CONFIG_ENABLE_VCP
    tx_task();
    rx_task();
    break_task();
    uart_timer_task();
#endif

    if (0 == HAL_GPIO_BOOT_ENTER_read())
      NVIC_SystemReset();
  }

  return 0;
}

