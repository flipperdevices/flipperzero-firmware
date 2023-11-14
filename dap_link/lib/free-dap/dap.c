/*
 * Copyright (c) 2016-2021, Alex Taradov <alex@taradov.com>
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
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "dap_config.h"
#include "dap.h"

/*- Definitions -------------------------------------------------------------*/
#define ARRAY_SIZE(x)  ((int)(sizeof(x) / sizeof(0[x])))

enum
{
  ID_DAP_INFO               = 0x00,
  ID_DAP_HOST_STATUS        = 0x01,
  ID_DAP_CONNECT            = 0x02,
  ID_DAP_DISCONNECT         = 0x03,
  ID_DAP_TRANSFER_CONFIGURE = 0x04,
  ID_DAP_TRANSFER           = 0x05,
  ID_DAP_TRANSFER_BLOCK     = 0x06,
  ID_DAP_TRANSFER_ABORT     = 0x07,
  ID_DAP_WRITE_ABORT        = 0x08,
  ID_DAP_DELAY              = 0x09,
  ID_DAP_RESET_TARGET       = 0x0a,

  ID_DAP_SWJ_PINS           = 0x10,
  ID_DAP_SWJ_CLOCK          = 0x11,
  ID_DAP_SWJ_SEQUENCE       = 0x12,

  ID_DAP_SWD_CONFIGURE      = 0x13,
  ID_DAP_SWD_SEQUENCE       = 0x1d,

  ID_DAP_JTAG_SEQUENCE      = 0x14,
  ID_DAP_JTAG_CONFIGURE     = 0x15,
  ID_DAP_JTAG_IDCODE        = 0x16,

  ID_DAP_SWO_TRANSPORT      = 0x17,
  ID_DAP_SWO_MODE           = 0x18,
  ID_DAP_SWO_BAUDRATE       = 0x19,
  ID_DAP_SWO_CONTROL        = 0x1a,
  ID_DAP_SWO_STATUS         = 0x1b,
  ID_DAP_SWO_EXT_STATUS     = 0x1e,
  ID_DAP_SWO_DATA           = 0x1c,

  ID_DAP_QUEUE_COMMANDS     = 0x7e,
  ID_DAP_EXECUTE_COMMANDS   = 0x7f,

  ID_DAP_VENDOR_0           = 0x80,
  ID_DAP_VENDOR_31          = 0x9f,
  ID_DAP_VENDOR_EX_FIRS     = 0xa0,
  ID_DAP_VENDOR_EX_LAST     = 0xfe,

  ID_DAP_INVALID            = 0xff,
};

enum
{
  DAP_INFO_VENDOR           = 0x01,
  DAP_INFO_PRODUCT          = 0x02,
  DAP_INFO_SER_NUM          = 0x03,
  DAP_INFO_CMSIS_DAP_VER    = 0x04,
  DAP_INFO_DEVICE_VENDOR    = 0x05,
  DAP_INFO_DEVICE_NAME      = 0x06,
  DAP_INFO_BOARD_VENDOR     = 0x07,
  DAP_INFO_BOARD_NAME       = 0x08,
  DAP_INFO_FW_VER           = 0x09,
  DAP_INFO_CAPABILITIES     = 0xf0,
  DAP_INFO_TDT              = 0xf1,
  DAP_INFO_UART_RX_SIZE     = 0xfb,
  DAP_INFO_UART_TX_SIZE     = 0xfc,
  DAP_INFO_SWO_BUF_SIZE     = 0xfd,
  DAP_INFO_PACKET_COUNT     = 0xfe,
  DAP_INFO_PACKET_SIZE      = 0xff,
};

enum
{
  DAP_CAP_SWD               = (1 << 0),
  DAP_CAP_JTAG              = (1 << 1),
  DAP_CAP_SWO_UART          = (1 << 2),
  DAP_CAP_SWO_MANCHESTER    = (1 << 3),
  DAP_CAP_ATOMIC_CMD        = (1 << 4),
  DAP_CAP_TDT               = (1 << 5),
  DAP_CAP_SWO_STREAMING     = (1 << 6),
  DAP_CAP_UART_COM_PORT     = (1 << 7),
};

enum
{
  DAP_TRANSFER_APnDP        = 1 << 0,
  DAP_TRANSFER_RnW          = 1 << 1,
  DAP_TRANSFER_A2           = 1 << 2,
  DAP_TRANSFER_A3           = 1 << 3,
  DAP_TRANSFER_MATCH_VALUE  = 1 << 4,
  DAP_TRANSFER_MATCH_MASK   = 1 << 5,
  DAP_TRANSFER_JTAG_ABORT   = 1 << 16,
};

enum
{
  DAP_TRANSFER_INVALID      = 0,
  DAP_TRANSFER_OK           = 1 << 0,
  DAP_TRANSFER_WAIT         = 1 << 1,
  DAP_TRANSFER_FAULT        = 1 << 2,
  DAP_TRANSFER_ERROR        = 1 << 3,
  DAP_TRANSFER_MISMATCH     = 1 << 4,
};

enum
{
  DAP_PORT_DISABLED         = 0,
  DAP_PORT_AUTODETECT       = 0,
  DAP_PORT_SWD              = 1,
  DAP_PORT_JTAG             = 2,
};

enum
{
  DAP_SWJ_SWCLK_TCK         = 1 << 0,
  DAP_SWJ_SWDIO_TMS         = 1 << 1,
  DAP_SWJ_TDI               = 1 << 2,
  DAP_SWJ_TDO               = 1 << 3,
  DAP_SWJ_nTRST             = 1 << 5,
  DAP_SWJ_nRESET            = 1 << 7,
};

enum
{
  DAP_OK                    = 0x00,
  DAP_ERROR                 = 0xff,
};

enum
{
  SWD_DP_R_IDCODE           = 0x00,
  SWD_DP_W_ABORT            = 0x00,
  SWD_DP_R_RDBUFF           = 0x0c,
};

enum
{
  JTAG_ABORT                = 0x08,
  JTAG_DPACC                = 0x0a,
  JTAG_APACC                = 0x0b,
  JTAG_IDCODE               = 0x0e,
  JTAG_BYPASS               = 0x0f,
  JTAG_INVALID              = 0xff,
};

enum
{
  JTAG_SEQUENCE_COUNT       = 0x3f,
  JTAG_SEQUENCE_TMS         = 0x40,
  JTAG_SEQUENCE_TDO         = 0x80,
};

enum
{
  SWD_SEQUENCE_COUNT        = 0x3f,
  SWD_SEQUENCE_DIN          = 0x80,
};

#define ARM_JTAG_IR_LENGTH  4

/*- Constants ---------------------------------------------------------------*/
static const struct
{
  int    id;
  char   * const str;
} dap_info_strings[] =
{
#ifdef DAP_CONFIG_VENDOR_STR
  { DAP_INFO_VENDOR,        DAP_CONFIG_VENDOR_STR },
#endif
#ifdef DAP_CONFIG_PRODUCT_STR
  { DAP_INFO_PRODUCT,       DAP_CONFIG_PRODUCT_STR },
#endif
#ifdef DAP_CONFIG_SER_NUM_STR
  { DAP_INFO_SER_NUM,       DAP_CONFIG_SER_NUM_STR },
#endif
#ifdef DAP_CONFIG_CMSIS_DAP_VER_STR
  { DAP_INFO_CMSIS_DAP_VER, DAP_CONFIG_CMSIS_DAP_VER_STR },
#endif
#ifdef DAP_CONFIG_DEVICE_VENDOR_STR
  { DAP_INFO_DEVICE_VENDOR, DAP_CONFIG_DEVICE_VENDOR_STR },
#endif
#ifdef DAP_CONFIG_DEVICE_NAME_STR
  { DAP_INFO_DEVICE_NAME,   DAP_CONFIG_DEVICE_NAME_STR },
#endif
#ifdef DAP_CONFIG_BOARD_VENDOR_STR
  { DAP_INFO_BOARD_VENDOR,  DAP_CONFIG_BOARD_VENDOR_STR },
#endif
#ifdef DAP_CONFIG_BOARD_NAME_STR
  { DAP_INFO_BOARD_NAME,    DAP_CONFIG_BOARD_NAME_STR },
#endif
#ifdef DAP_CONFIG_FW_VER_STR
  { DAP_INFO_FW_VER,        DAP_CONFIG_FW_VER_STR },
#endif
};

/*- Variables ---------------------------------------------------------------*/
static int dap_port;
static volatile bool dap_abort;
static uint32_t dap_match_mask;
static int dap_idle_cycles;
static int dap_retry_count;
static int dap_match_retry_count;
static int dap_clock_delay;

static void (*dap_swj_run)(int);
static void (*dap_swd_write)(uint32_t, int);
static uint32_t (*dap_swd_read)(int);

#ifdef DAP_CONFIG_ENABLE_JTAG
static uint32_t (*dap_jtag_write)(uint32_t, int);
static uint32_t (*dap_jtag_read)(int);
static uint32_t (*dap_jtag_rdwr)(uint32_t, int);
#endif

static uint8_t *dap_req_buf;
static int dap_req_size;
static int dap_req_ptr;

static uint8_t *dap_resp_buf;
static int dap_resp_size;
static int dap_resp_ptr;

static bool dap_buf_error;

static int dap_swd_turnaround;
static bool dap_swd_data_phase;

#ifdef DAP_CONFIG_ENABLE_JTAG
static int dap_jtag_dev_count;
static int dap_jtag_dev_index;
static int dap_jtag_ir_length[DAP_CONFIG_JTAG_DEV_COUNT];
static int dap_jtag_ir_before[DAP_CONFIG_JTAG_DEV_COUNT];
static int dap_jtag_ir_after[DAP_CONFIG_JTAG_DEV_COUNT];
static int dap_jtag_ir;
#endif

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
static void dap_delay_us(int delay)
{
  while (delay)
  {
    int del = (delay > 100000) ? 100000 : delay;
    DAP_CONFIG_DELAY((DAP_CONFIG_DELAY_CONSTANT * 2 * del) / 1000);
    delay -= del;
  }
}

//-----------------------------------------------------------------------------
#define DAP_SWJ_FN(ver, delay) \
  DAP_CONFIG_PERFORMANCE_ATTR						\
  static void dap_swj_run_##ver(int cycles)				\
  {									\
    while (cycles--)							\
    {									\
      DAP_CONFIG_SWCLK_TCK_clr();					\
      delay(dap_clock_delay);						\
      DAP_CONFIG_SWCLK_TCK_set();					\
      delay(dap_clock_delay);						\
    }									\
  }
DAP_SWJ_FN(slow, DAP_CONFIG_DELAY)
DAP_SWJ_FN(fast, (void))

//-----------------------------------------------------------------------------
#define DAP_SWD_FN(ver, delay) \
  DAP_CONFIG_PERFORMANCE_ATTR						\
  static void dap_swd_write_##ver(uint32_t value, int size)		\
  {									\
    for (int i = 0; i < size; i++)					\
    {									\
      DAP_CONFIG_SWDIO_TMS_write(value & 1);				\
      DAP_CONFIG_SWCLK_TCK_clr();					\
      delay(dap_clock_delay);						\
      DAP_CONFIG_SWCLK_TCK_set();					\
      delay(dap_clock_delay);						\
      value >>= 1;							\
    }									\
  }									\
									\
  DAP_CONFIG_PERFORMANCE_ATTR						\
  static uint32_t dap_swd_read_##ver(int size)				\
  {									\
    uint32_t value = 0;							\
    uint32_t bit;							\
    for (int i = 0; i < size; i++)					\
    {									\
      DAP_CONFIG_SWCLK_TCK_clr();					\
      delay(dap_clock_delay);						\
      bit = DAP_CONFIG_SWDIO_TMS_read();				\
      DAP_CONFIG_SWCLK_TCK_set();					\
      delay(dap_clock_delay);						\
      value |= (bit << i);						\
    }									\
    return value;							\
  }

DAP_SWD_FN(slow, DAP_CONFIG_DELAY)
DAP_SWD_FN(fast, (void))

//-----------------------------------------------------------------------------
static inline uint32_t dap_parity(uint32_t value)
{
  value ^= value >> 16;
  value ^= value >> 8;
  value ^= value >> 4;
  value &= 0x0f;

  return (0x6996 >> value) & 1;
}

//-----------------------------------------------------------------------------
static int dap_swd_operation(int req, uint32_t *data)
{
  uint32_t value;
  int ack = 0;

  req &= (DAP_TRANSFER_APnDP | DAP_TRANSFER_RnW | DAP_TRANSFER_A2 | DAP_TRANSFER_A3);

  dap_swd_write(0x81 | (dap_parity(req) << 5) | (req << 1), 8);

  DAP_CONFIG_SWDIO_TMS_in();

  dap_swj_run(dap_swd_turnaround);

  ack = dap_swd_read(3);

  if (DAP_TRANSFER_OK == ack)
  {
    if (req & DAP_TRANSFER_RnW)
    {
      value = dap_swd_read(32);

      if (dap_parity(value) != dap_swd_read(1))
        ack = DAP_TRANSFER_ERROR;

      if (data)
        *data = value;

      dap_swj_run(dap_swd_turnaround);

      DAP_CONFIG_SWDIO_TMS_out();
    }
    else
    {
      dap_swj_run(dap_swd_turnaround);

      DAP_CONFIG_SWDIO_TMS_out();

      dap_swd_write(*data, 32);
      dap_swd_write(dap_parity(*data), 1);
    }

    DAP_CONFIG_SWDIO_TMS_write(0);
    dap_swj_run(dap_idle_cycles);
  }

  else if (DAP_TRANSFER_WAIT == ack || DAP_TRANSFER_FAULT == ack)
  {
    if (dap_swd_data_phase && (req & DAP_TRANSFER_RnW))
      dap_swj_run(32 + 1);

    dap_swj_run(dap_swd_turnaround);

    DAP_CONFIG_SWDIO_TMS_out();

    if (dap_swd_data_phase && (0 == (req & DAP_TRANSFER_RnW)))
    {
      DAP_CONFIG_SWDIO_TMS_write(0);
      dap_swj_run(32 + 1);
    }
  }

  else
  {
    dap_swj_run(dap_swd_turnaround + 32 + 1);
  }

  DAP_CONFIG_SWDIO_TMS_write(1);

  return ack;
}

#ifdef DAP_CONFIG_ENABLE_JTAG
//-----------------------------------------------------------------------------
#define DAP_JTAG_FN(ver, delay) \
  DAP_CONFIG_PERFORMANCE_ATTR						\
  static uint32_t dap_jtag_write_##ver(uint32_t value, int size)	\
  {									\
    for (int i = 0; i < size; i++)					\
    {									\
      DAP_CONFIG_TDI_write(value & 1);					\
      DAP_CONFIG_SWCLK_TCK_clr();					\
      delay(dap_clock_delay);						\
      DAP_CONFIG_SWCLK_TCK_set();					\
      delay(dap_clock_delay);						\
      value >>= 1;							\
    }									\
    return value;							\
  }									\
									\
  DAP_CONFIG_PERFORMANCE_ATTR						\
  static uint32_t dap_jtag_read_##ver(int size)				\
  {									\
    uint32_t value = 0;							\
    uint32_t bit;							\
    for (int i = 0; i < size; i++)					\
    {									\
      DAP_CONFIG_SWCLK_TCK_clr();					\
      delay(dap_clock_delay);						\
      bit = DAP_CONFIG_TDO_read();					\
      DAP_CONFIG_SWCLK_TCK_set();					\
      delay(dap_clock_delay);						\
      value |= (bit << i);						\
    }									\
    return value;							\
  }									\
									\
  DAP_CONFIG_PERFORMANCE_ATTR						\
  static uint32_t dap_jtag_rdwr_##ver(uint32_t value, int size)		\
  {									\
    uint32_t rvalue = 0;						\
    uint32_t bit;							\
    for (int i = 0; i < size; i++)					\
    {									\
      DAP_CONFIG_TDI_write(value & 1);					\
      DAP_CONFIG_SWCLK_TCK_clr();					\
      delay(dap_clock_delay);						\
      bit = DAP_CONFIG_TDO_read();					\
      DAP_CONFIG_SWCLK_TCK_set();					\
      delay(dap_clock_delay);						\
      value >>= 1;							\
      rvalue |= (bit << i);						\
    }									\
    return rvalue;							\
  }

DAP_JTAG_FN(slow, DAP_CONFIG_DELAY)
DAP_JTAG_FN(fast, (void))

//-----------------------------------------------------------------------------
static void dap_jtag_write_ir(int ir)
{
  int len = dap_jtag_ir_length[dap_jtag_dev_index];

  DAP_CONFIG_SWDIO_TMS_write(1);
  dap_swj_run(2); // -> Select-IR-Scan
  DAP_CONFIG_SWDIO_TMS_write(0);
  dap_swj_run(2); // -> Shift-IR

  DAP_CONFIG_TDI_write(1);
  dap_swj_run(dap_jtag_ir_before[dap_jtag_dev_index]);

  ir = dap_jtag_write(ir, len-1);

  if (dap_jtag_ir_after[dap_jtag_dev_index])
  {
    dap_jtag_write(ir, 1);

    DAP_CONFIG_TDI_write(1);
    dap_swj_run(dap_jtag_ir_after[dap_jtag_dev_index]-1);

    DAP_CONFIG_SWDIO_TMS_write(1);
    dap_swj_run(1); // -> Exit1-IR
  }
  else
  {
    DAP_CONFIG_SWDIO_TMS_write(1);
    dap_jtag_write(ir, 1); // -> Exit1-IR
  }

  dap_swj_run(1); // -> Update-IR
  DAP_CONFIG_SWDIO_TMS_write(0);
  dap_swj_run(1); // -> Idle
  DAP_CONFIG_TDI_write(1);
}

//-----------------------------------------------------------------------------
static int dap_jtag_operation(int req, uint32_t *data)
{
  int ack, ir;

  if (DAP_TRANSFER_JTAG_ABORT == req)
    ir = JTAG_ABORT;
  else
    ir = (req & DAP_TRANSFER_APnDP) ? JTAG_APACC : JTAG_DPACC;

  if (ir != dap_jtag_ir)
  {
    dap_jtag_ir = ir;
    dap_jtag_write_ir(ir);
  }

  DAP_CONFIG_SWDIO_TMS_write(1);
  dap_swj_run(1); // -> Select-DR-Scan
  DAP_CONFIG_SWDIO_TMS_write(0);
  dap_swj_run(2 + dap_jtag_dev_index); // -> Shift-DR

  ack = dap_jtag_rdwr(req >> 1, 3);

  if (ack == 0x2)
    ack = DAP_TRANSFER_OK; // or FAULT
  else if (ack == 0x1)
    ack = DAP_TRANSFER_WAIT;
  else
    ack = DAP_TRANSFER_INVALID;

  if (DAP_TRANSFER_OK == ack)
  {
    int cnt = dap_jtag_dev_count - dap_jtag_dev_index - 1;
    uint32_t value;

    if (req & DAP_TRANSFER_RnW)
    {
      if (cnt)
      {
        value = dap_jtag_read(32);
        dap_swj_run(cnt-1);
        DAP_CONFIG_SWDIO_TMS_write(1);
        dap_swj_run(1); // -> Exit1-DR
      }
      else
      {
        value = dap_jtag_read(31);
        DAP_CONFIG_SWDIO_TMS_write(1);
        value |= (dap_jtag_read(1) << 31); // -> Exit1-DR
      }

      if (data)
        *data = value;
    }
    else
    {
      value = *data;

      if (cnt)
      {
        dap_jtag_write(value, 32);
        dap_swj_run(cnt-1);
        DAP_CONFIG_SWDIO_TMS_write(1);
        dap_swj_run(1); // -> Exit1-DR
      }
      else
      {
        value = dap_jtag_write(value, 31);
        DAP_CONFIG_SWDIO_TMS_write(1);
        dap_jtag_write(value, 1); // -> Exit1-DR
      }
    }
  }
  else // Not OK
  {
    DAP_CONFIG_SWDIO_TMS_write(1);
    dap_swj_run(1); // -> Exit1-DR
  }

  dap_swj_run(1); // -> Update-DR
  DAP_CONFIG_SWDIO_TMS_write(0);
  dap_swj_run(1); // -> Idle
  DAP_CONFIG_TDI_write(1);

  dap_swj_run(dap_idle_cycles);

  return ack;
}
#endif // DAP_CONFIG_ENABLE_JTAG

//-----------------------------------------------------------------------------
static void dap_setup_clock(int freq)
{
  if (freq > DAP_CONFIG_FAST_CLOCK)
  {
    dap_clock_delay = 0;
    dap_swj_run     = dap_swj_run_fast;
    dap_swd_write   = dap_swd_write_fast;
    dap_swd_read    = dap_swd_read_fast;
#ifdef DAP_CONFIG_ENABLE_JTAG
    dap_jtag_write  = dap_jtag_write_fast;
    dap_jtag_read   = dap_jtag_read_fast;
    dap_jtag_rdwr   = dap_jtag_rdwr_fast;
#endif
  }
  else
  {
    dap_clock_delay = (DAP_CONFIG_DELAY_CONSTANT * 1000) / freq;
    dap_swj_run     = dap_swj_run_slow;
    dap_swd_write   = dap_swd_write_slow;
    dap_swd_read    = dap_swd_read_slow;
#ifdef DAP_CONFIG_ENABLE_JTAG
    dap_jtag_write  = dap_jtag_write_slow;
    dap_jtag_read   = dap_jtag_read_slow;
    dap_jtag_rdwr   = dap_jtag_rdwr_slow;
#endif
  }
}

//-----------------------------------------------------------------------------
static bool dap_select_device(int index)
{
  if (DAP_PORT_SWD == dap_port)
    return true;

#ifdef DAP_CONFIG_ENABLE_JTAG
  if (DAP_PORT_JTAG == dap_port)
  {
    if (index >= dap_jtag_dev_count || dap_jtag_ir_length[index] != ARM_JTAG_IR_LENGTH)
      return false;

    dap_jtag_dev_index = index;

    return true;
  }
#endif

  (void)index;
  return false;
}

//-----------------------------------------------------------------------------
static int dap_transfer_word(int req, uint32_t *data)
{
  int ack = DAP_TRANSFER_INVALID;

  for (int i = 0; i < dap_retry_count; i++)
  {
    if (DAP_PORT_SWD == dap_port)
      ack = dap_swd_operation(req, data);

#ifdef DAP_CONFIG_ENABLE_JTAG
    else if (DAP_PORT_JTAG == dap_port)
      ack = dap_jtag_operation(req, data);
#endif

    if (DAP_TRANSFER_WAIT != ack || dap_abort)
      break;
  }

  return ack;
}

//-----------------------------------------------------------------------------
static bool dap_needs_posted_read(int request)
{
  if (0 == (request & DAP_TRANSFER_RnW))
    return false;

  if (DAP_PORT_SWD == dap_port)
    return (request & DAP_TRANSFER_APnDP);

#ifdef DAP_CONFIG_ENABLE_JTAG
  if (DAP_PORT_JTAG == dap_port)
    return true;
#endif

  return false;
}

//-----------------------------------------------------------------------------
static void dap_buf_init(uint8_t *req, int req_size, uint8_t *resp, int resp_size)
{
  dap_req_buf  = req;
  dap_req_size = req_size;
  dap_req_ptr  = 0;

  dap_resp_buf  = resp;
  dap_resp_size = resp_size;
  dap_resp_ptr  = 0;

  dap_buf_error = false;
}

//-----------------------------------------------------------------------------
uint8_t dap_req_get_byte(void)
{
  if (dap_buf_error || ((dap_req_size - dap_req_ptr) < (int)sizeof(uint8_t)))
  {
    dap_buf_error = true;
    return 0;
  }

  return dap_req_buf[dap_req_ptr++];
}

//-----------------------------------------------------------------------------
uint16_t dap_req_get_half(void)
{
  if (dap_buf_error || ((dap_req_size - dap_req_ptr) < (int)sizeof(uint16_t)))
  {
    dap_buf_error = true;
    return 0;
  }

  uint16_t value =
      ((uint16_t)dap_req_buf[dap_req_ptr + 1] << 8) |
       (uint16_t)dap_req_buf[dap_req_ptr];
  dap_req_ptr += sizeof(uint16_t);

  return value;
}

//-----------------------------------------------------------------------------
uint32_t dap_req_get_word(void)
{
  if (dap_buf_error || ((dap_req_size - dap_req_ptr) < (int)sizeof(uint32_t)))
  {
    dap_buf_error = true;
    return 0;
  }

  uint32_t value =
      ((uint32_t)dap_req_buf[dap_req_ptr + 3] << 24) |
      ((uint32_t)dap_req_buf[dap_req_ptr + 2] << 16) |
      ((uint32_t)dap_req_buf[dap_req_ptr + 1] << 8) |
       (uint32_t)dap_req_buf[dap_req_ptr];
  dap_req_ptr += sizeof(uint32_t);

  return value;
}

//-----------------------------------------------------------------------------
void dap_resp_add_byte(uint8_t value)
{
  if (dap_buf_error || ((dap_resp_size - dap_resp_ptr) < (int)sizeof(uint8_t)))
  {
    dap_buf_error = true;
    return;
  }

  dap_resp_buf[dap_resp_ptr++] = value;
}

//-----------------------------------------------------------------------------
void dap_resp_add_word(uint32_t value)
{
  if (dap_buf_error || ((dap_resp_size - dap_resp_ptr) < (int)sizeof(uint32_t)))
  {
    dap_buf_error = true;
    return;
  }

  dap_resp_buf[dap_resp_ptr + 0] = value;
  dap_resp_buf[dap_resp_ptr + 1] = value >> 8;
  dap_resp_buf[dap_resp_ptr + 2] = value >> 16;
  dap_resp_buf[dap_resp_ptr + 3] = value >> 24;
  dap_resp_ptr += sizeof(uint32_t);
}

//-----------------------------------------------------------------------------
void dap_resp_set_byte(int index, uint8_t value)
{
  if (index < dap_resp_ptr)
    dap_resp_buf[index] = value;
}

//-----------------------------------------------------------------------------
bool dap_is_buf_error(void)
{
  return dap_buf_error;
}

//-----------------------------------------------------------------------------
static void dap_info(void)
{
  int index = dap_req_get_byte();

  if (DAP_INFO_CAPABILITIES == index)
  {
    int cap = DAP_CAP_SWD;
#ifdef DAP_CONFIG_ENABLE_JTAG
    cap |= DAP_CAP_JTAG;
#endif
    dap_resp_add_byte(1);
    dap_resp_add_byte(cap);
  }
  else if (DAP_INFO_PACKET_COUNT == index)
  {
    dap_resp_add_byte(1);
    dap_resp_add_byte(DAP_CONFIG_PACKET_COUNT);
  }
  else if (DAP_INFO_PACKET_SIZE == index)
  {
    dap_resp_add_byte(2);
    dap_resp_add_byte(DAP_CONFIG_PACKET_SIZE & 0xff);
    dap_resp_add_byte((DAP_CONFIG_PACKET_SIZE >> 8) & 0xff);
  }
  else
  {
    dap_resp_add_byte(0); // Size placeholder

    for (int i = 0; i < ARRAY_SIZE(dap_info_strings); i++)
    {
      if (dap_info_strings[i].id == index)
      {
        const char *str = dap_info_strings[i].str;

        while (*str)
          dap_resp_add_byte(*str++);
        dap_resp_add_byte(0);

        dap_resp_set_byte(1, dap_resp_ptr-2);

        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
static void dap_host_status(void)
{
  int index = dap_req_get_byte();
  int state = dap_req_get_byte();

  DAP_CONFIG_LED(index, state);

  dap_resp_add_byte(DAP_OK);
}

//-----------------------------------------------------------------------------
static void dap_connect(void)
{
  int port = dap_req_get_byte();

  if (DAP_PORT_AUTODETECT == port)
    port = DAP_CONFIG_DEFAULT_PORT;

  dap_port = DAP_PORT_DISABLED;

  if (DAP_PORT_SWD == port)
  {
    DAP_CONFIG_CONNECT_SWD();
    dap_port = DAP_PORT_SWD;
  }

#ifdef DAP_CONFIG_ENABLE_JTAG
  else if (DAP_PORT_JTAG == port)
  {
    DAP_CONFIG_CONNECT_JTAG();
    dap_port = DAP_PORT_JTAG;
  }
#endif

  dap_resp_add_byte(dap_port);
}

//-----------------------------------------------------------------------------
static void dap_disconnect(void)
{
  DAP_CONFIG_DISCONNECT();

  dap_port = DAP_PORT_DISABLED;

  dap_resp_add_byte(DAP_OK);
}

//-----------------------------------------------------------------------------
static void dap_transfer_configure(void)
{
  dap_idle_cycles = dap_req_get_byte();
  dap_retry_count = dap_req_get_half();
  dap_match_retry_count = dap_req_get_half();

  dap_resp_add_byte(DAP_OK);
}

//-----------------------------------------------------------------------------
static void dap_transfer(void)
{
  int req_count, resp_count, request, ack;
  bool posted_read, verify_write;
  uint32_t data, match_value;

  dap_resp_add_byte(0); // Count
  dap_resp_add_byte(DAP_TRANSFER_INVALID);

  if (!dap_select_device(dap_req_get_byte()))
    return;

  req_count  = dap_req_get_byte();
  resp_count = 0;

  posted_read = false;
  verify_write = false;
  ack = DAP_TRANSFER_INVALID;

  for (; req_count && !dap_abort && !dap_buf_error; req_count--, resp_count++)
  {
    request = dap_req_get_byte();
    verify_write = false;

    if (posted_read)
    {
      if (dap_needs_posted_read(request))
      {
        ack = dap_transfer_word(request, &data);
      }
      else
      {
        ack = dap_transfer_word(SWD_DP_R_RDBUFF | DAP_TRANSFER_RnW, &data);
        posted_read = false;
      }

      if (ack != DAP_TRANSFER_OK)
        break;

      dap_resp_add_word(data);

      if (posted_read)
        continue;
    }

    if (request & DAP_TRANSFER_RnW)
    {
      if (request & DAP_TRANSFER_MATCH_VALUE)
      {
        match_value = dap_req_get_word();

        if (dap_needs_posted_read(request))
          dap_transfer_word(request, NULL);

        for (int i = 0; i < dap_match_retry_count; i++)
        {
          ack = dap_transfer_word(request, &data);

          if (DAP_TRANSFER_OK != ack || (data & dap_match_mask) == match_value || dap_abort)
            break;
        };

        if ((data & dap_match_mask) != match_value)
          ack |= DAP_TRANSFER_MISMATCH;

        if (ack != DAP_TRANSFER_OK)
          break;
      }
      else if (dap_needs_posted_read(request))
      {
        ack = dap_transfer_word(request, NULL);

        if (ack != DAP_TRANSFER_OK)
          break;

        posted_read = true;
      }
      else
      {
        ack = dap_transfer_word(request, &data);

        if (DAP_TRANSFER_OK != ack)
          break;

        dap_resp_add_word(data);
      }
    }
    else // Write
    {
      data = dap_req_get_word();

      if (request & DAP_TRANSFER_MATCH_MASK)
      {
        ack = DAP_TRANSFER_OK;
        dap_match_mask = data;
      }
      else
      {
        ack = dap_transfer_word(request, &data);

        if (ack != DAP_TRANSFER_OK)
          break;

        verify_write = true;
      }
    }
  }

  if (DAP_TRANSFER_OK == ack)
  {
    if (posted_read)
    {
      ack = dap_transfer_word(SWD_DP_R_RDBUFF | DAP_TRANSFER_RnW, &data);
      dap_resp_add_word(data);
    }
    else if (verify_write)
    {
      ack = dap_transfer_word(SWD_DP_R_RDBUFF | DAP_TRANSFER_RnW, NULL);
    }
  }

  dap_resp_set_byte(1, resp_count);
  dap_resp_set_byte(2, ack);
}

//-----------------------------------------------------------------------------
static void dap_transfer_block(void)
{
  int req_count, resp_count, request, ack;
  uint32_t data;

  dap_resp_add_byte(0); // Count
  dap_resp_add_byte(0); // Count
  dap_resp_add_byte(DAP_TRANSFER_INVALID);

  if (!dap_select_device(dap_req_get_byte()))
    return;

  req_count  = dap_req_get_half();
  resp_count = 0;

  if (0 == req_count)
    return;

  request = dap_req_get_byte();
  ack = DAP_TRANSFER_INVALID;

  if (request & DAP_TRANSFER_RnW)
  {
    bool needs_posted = dap_needs_posted_read(request);
    int transfers = needs_posted ? (req_count + 1) : req_count;

    for (int i = 0; i < transfers; i++)
    {
      if (i == req_count)
        request = SWD_DP_R_RDBUFF | DAP_TRANSFER_RnW;

      ack = dap_transfer_word(request, &data);

      if (DAP_TRANSFER_OK != ack)
        break;

      if (needs_posted && i == 0)
        continue;

      dap_resp_add_word(data);
      resp_count++;
    }
  }
  else // Write
  {
    for (int i = 0; i < req_count; i++)
    {
      data = dap_req_get_word();

      ack = dap_transfer_word(request, &data);

      if (DAP_TRANSFER_OK != ack)
        break;

      resp_count++;
    }

    if (DAP_TRANSFER_OK == ack)
      ack = dap_transfer_word(SWD_DP_R_RDBUFF | DAP_TRANSFER_RnW, NULL);
  }

  dap_resp_set_byte(1, resp_count);
  dap_resp_set_byte(2, resp_count >> 8);
  dap_resp_set_byte(3, ack);
}

//-----------------------------------------------------------------------------
static void dap_transfer_abort(void)
{
  // This request is handled outside of the normal queue.
  // We should never get here.
  dap_resp_add_byte(DAP_OK);
}

//-----------------------------------------------------------------------------
static void dap_write_abort(void)
{
  int status = DAP_ERROR;
  uint32_t data;

  if (!dap_select_device(dap_req_get_byte()))
  {
    dap_resp_add_byte(DAP_ERROR);
    return;
  }

  data = dap_req_get_word();

  if (DAP_PORT_SWD == dap_port)
  {
    dap_swd_operation(SWD_DP_W_ABORT, &data);
    status = DAP_OK;
  }

#ifdef DAP_CONFIG_ENABLE_JTAG
  else if (DAP_PORT_JTAG == dap_port)
  {
    dap_jtag_operation(DAP_TRANSFER_JTAG_ABORT, &data);
    status = DAP_OK;
  }
#endif

  dap_resp_add_byte(status);
}

//-----------------------------------------------------------------------------
static void dap_delay(void)
{
  int delay = dap_req_get_half();
  dap_delay_us(delay);
  dap_resp_add_byte(DAP_OK);
}

//-----------------------------------------------------------------------------
static void dap_reset_target(void)
{
  dap_resp_add_byte(DAP_OK);

#ifdef DAP_CONFIG_RESET_TARGET_FN
  DAP_CONFIG_RESET_TARGET_FN();
  dap_resp_add_byte(1);
#else
  dap_resp_add_byte(0);
#endif
}

//-----------------------------------------------------------------------------
static void dap_swj_pins(void)
{
  int value  = dap_req_get_byte();
  int select = dap_req_get_byte();
  int wait   = dap_req_get_word();

  if (select & DAP_SWJ_SWCLK_TCK)
    DAP_CONFIG_SWCLK_TCK_write(value & DAP_SWJ_SWCLK_TCK);

  if (select & DAP_SWJ_SWDIO_TMS)
    DAP_CONFIG_SWDIO_TMS_write(value & DAP_SWJ_SWDIO_TMS);

  if (select & DAP_SWJ_TDI)
    DAP_CONFIG_TDI_write(value & DAP_SWJ_TDI);

  if (select & DAP_SWJ_nTRST)
    DAP_CONFIG_nTRST_write(value & DAP_SWJ_nTRST);

  if (select & DAP_SWJ_nRESET)
    DAP_CONFIG_nRESET_write(value & DAP_SWJ_nRESET);

  dap_delay_us(wait * 1000);

  value =
    (DAP_CONFIG_SWCLK_TCK_read() ? DAP_SWJ_SWCLK_TCK : 0) |
    (DAP_CONFIG_SWDIO_TMS_read() ? DAP_SWJ_SWDIO_TMS : 0) |
    (DAP_CONFIG_TDI_read()       ? DAP_SWJ_TDI       : 0) |
    (DAP_CONFIG_TDO_read()       ? DAP_SWJ_TDO       : 0) |
    (DAP_CONFIG_nTRST_read()     ? DAP_SWJ_nTRST     : 0) |
    (DAP_CONFIG_nRESET_read()    ? DAP_SWJ_nRESET    : 0);

  dap_resp_add_byte(value);
}

//-----------------------------------------------------------------------------
static void dap_swj_clock(void)
{
  int freq = dap_req_get_word();
  dap_setup_clock(freq);
  dap_resp_add_byte(DAP_OK);
}

//-----------------------------------------------------------------------------
static void dap_swj_sequence(void)
{
  int size = dap_req_get_byte();

  while (size)
  {
    int sz = (size > 8) ? 8 : size;
    dap_swd_write(dap_req_get_byte(), sz);
    size -= sz;
  }

  dap_resp_add_byte(DAP_OK);
}

//-----------------------------------------------------------------------------
static void dap_swd_configure(void)
{
  int data = dap_req_get_byte();

  dap_swd_turnaround = (data & 3) + 1;
  dap_swd_data_phase = (data & 4) ? 1 : 0;

  dap_resp_add_byte(DAP_OK);
}

//-----------------------------------------------------------------------------
static void dap_swd_sequence(void)
{
  int req_count;

  if (DAP_PORT_SWD != dap_port)
  {
    dap_resp_add_byte(DAP_ERROR);
    return;
  }

  dap_resp_add_byte(DAP_OK);

  req_count = dap_req_get_byte();

  for (int i = 0; i < req_count; i++)
  {
    int info  = dap_req_get_byte();
    int count = info & SWD_SEQUENCE_COUNT;
    int din   = info & SWD_SEQUENCE_DIN;

    if (count == 0)
      count = 64U;

    if (din)
    {
      DAP_CONFIG_SWDIO_TMS_in();

      while (count)
      {
        int sz = (count > 8) ? 8 : count;
        int value = dap_swd_read(sz);
        dap_resp_add_byte(value);
        count -= sz;
      }
    }
    else
    {
      DAP_CONFIG_SWDIO_TMS_out();

      while (count)
      {
        int sz = (count > 8) ? 8 : count;
        dap_swd_write(dap_req_get_byte(), sz);
        count -= sz;
      }
    }
  }

  DAP_CONFIG_SWDIO_TMS_out();
}

//-----------------------------------------------------------------------------
static void dap_jtag_sequence(void)
{
#ifdef DAP_CONFIG_ENABLE_JTAG
  int req_count;

  if (DAP_PORT_JTAG != dap_port)
  {
    dap_resp_add_byte(DAP_ERROR);
    return;
  }

  dap_resp_add_byte(DAP_OK);

  req_count = dap_req_get_byte();

  for (int i = 0; i < req_count; i++)
  {
    int info  = dap_req_get_byte();
    int count = info & JTAG_SEQUENCE_COUNT;
    int tms   = info & JTAG_SEQUENCE_TMS;
    int tdo   = info & JTAG_SEQUENCE_TDO;

    if (count == 0)
      count = 64;

    DAP_CONFIG_SWDIO_TMS_write(tms);

    while (count)
    {
      int sz = (count > 8) ? 8 : count;

      if (tdo)
      {
        int value = dap_jtag_rdwr(dap_req_get_byte(), sz);
        dap_resp_add_byte(value);
      }
      else
      {
        dap_jtag_write(dap_req_get_byte(), sz);
      }

      count -= sz;
    }
  }
#else
  dap_resp_add_byte(DAP_ERROR);
#endif
}

//-----------------------------------------------------------------------------
static void dap_jtag_configure(void)
{
#ifdef DAP_CONFIG_ENABLE_JTAG
  int count = dap_req_get_byte();
  int bits = 0;

  if (count > DAP_CONFIG_JTAG_DEV_COUNT)
  {
    dap_resp_add_byte(DAP_ERROR);
    return;
  }

  dap_jtag_dev_count = count;
  dap_jtag_dev_index = 0;

  for (int i = 0; i < dap_jtag_dev_count; i++)
  {
    dap_jtag_ir_length[i] = dap_req_get_byte();
    dap_jtag_ir_before[i] = bits;
    bits += dap_jtag_ir_length[i];
  }

  for (int i = 0; i < dap_jtag_dev_count; i++)
  {
    bits -= dap_jtag_ir_length[i];
    dap_jtag_ir_after[i] = bits;
  }

  dap_resp_add_byte(DAP_OK);
#else
  dap_resp_add_byte(DAP_ERROR);
#endif
}

//-----------------------------------------------------------------------------
static void dap_jtag_idcode(void)
{
#ifdef DAP_CONFIG_ENABLE_JTAG
  uint32_t data;

  if (DAP_PORT_JTAG != dap_port || !dap_select_device(dap_req_get_byte()))
  {
    dap_resp_add_byte(DAP_ERROR);
    return;
  }

  dap_jtag_write_ir(JTAG_IDCODE);

  DAP_CONFIG_SWDIO_TMS_write(1);
  dap_swj_run(1); // -> Select-DR-Scan
  DAP_CONFIG_SWDIO_TMS_write(0);
  dap_swj_run(2 + dap_jtag_dev_index); // -> Shift-DR

  data = dap_jtag_read(31);

  DAP_CONFIG_SWDIO_TMS_write(1);
  data |= (dap_jtag_read(1) << 31); // -> Exit1-DR

  dap_swj_run(1); // -> Update-DR
  DAP_CONFIG_SWDIO_TMS_write(0);
  dap_swj_run(1); // -> Idle
  DAP_CONFIG_TDI_write(1);

  dap_resp_add_byte(DAP_OK);
  dap_resp_add_word(data);
#endif
}

//-----------------------------------------------------------------------------
void dap_init(void)
{
  dap_port              = DAP_PORT_DISABLED;
  dap_abort             = false;
  dap_match_mask        = 0;
  dap_idle_cycles       = 0;
  dap_retry_count       = 100;
  dap_match_retry_count = 100;
  dap_swd_turnaround    = 1;
  dap_swd_data_phase    = false;
#ifdef DAP_CONFIG_ENABLE_JTAG
  dap_jtag_dev_count = 0;
#endif

  dap_setup_clock(DAP_CONFIG_DEFAULT_CLOCK);

  DAP_CONFIG_SETUP();
}

//-----------------------------------------------------------------------------
bool dap_filter_request(uint8_t *req)
{
  int cmd = req[0];

  if (ID_DAP_TRANSFER_ABORT == cmd)
  {
    dap_abort = true;
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
int dap_process_request(uint8_t *req, int req_size, uint8_t *resp, int resp_size)
{
  static const struct
  {
    int    cmd;
    void   (*handler)(void);
  } handlers[] =
  {
    { ID_DAP_INFO,			dap_info },
    { ID_DAP_HOST_STATUS,		dap_host_status },
    { ID_DAP_CONNECT,			dap_connect },
    { ID_DAP_DISCONNECT,		dap_disconnect },
    { ID_DAP_TRANSFER_CONFIGURE,	dap_transfer_configure },
    { ID_DAP_TRANSFER,			dap_transfer },
    { ID_DAP_TRANSFER_BLOCK,		dap_transfer_block },
    { ID_DAP_TRANSFER_ABORT,		dap_transfer_abort },
    { ID_DAP_WRITE_ABORT,		dap_write_abort },
    { ID_DAP_DELAY,			dap_delay },
    { ID_DAP_RESET_TARGET,		dap_reset_target },
    { ID_DAP_SWJ_PINS,			dap_swj_pins },
    { ID_DAP_SWJ_CLOCK,			dap_swj_clock },
    { ID_DAP_SWJ_SEQUENCE,		dap_swj_sequence },
    { ID_DAP_SWD_CONFIGURE,		dap_swd_configure },
    { ID_DAP_SWD_SEQUENCE,		dap_swd_sequence },
    { ID_DAP_JTAG_SEQUENCE,		dap_jtag_sequence },
    { ID_DAP_JTAG_CONFIGURE,		dap_jtag_configure },
    { ID_DAP_JTAG_IDCODE,		dap_jtag_idcode },
  };
  int cmd;

  dap_buf_init(req, req_size, resp, resp_size);

  dap_abort = false;

#ifdef DAP_CONFIG_ENABLE_JTAG
  dap_jtag_ir = JTAG_INVALID;
#endif

  cmd = dap_req_get_byte();
  dap_resp_add_byte(cmd);

  for (int i = 0; i < ARRAY_SIZE(handlers); i++)
  {
    if (cmd == handlers[i].cmd)
    {
      handlers[i].handler();
      return dap_resp_ptr;
    }
  }

  if (ID_DAP_VENDOR_0 <= cmd && cmd <= ID_DAP_VENDOR_31)
  {
#ifdef DAP_CONFIG_VENDOR_FN
    DAP_CONFIG_VENDOR_FN(cmd - ID_DAP_VENDOR_0);
#else
    dap_resp_add_byte(DAP_ERROR);
#endif
    return dap_resp_ptr;
  }

  dap_resp_set_byte(0, ID_DAP_INVALID);

  return dap_resp_ptr;
}

//-----------------------------------------------------------------------------
void dap_clock_test(int delay)
{
  DAP_CONFIG_CONNECT_SWD();

  if (delay)
  {
    dap_clock_delay = delay;

    while (1)
      dap_swj_run_slow(1<<30);
  }
  else
  {
    while (1)
      dap_swj_run_fast(1<<30);
  }
}
