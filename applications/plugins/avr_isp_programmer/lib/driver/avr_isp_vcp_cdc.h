#pragma once

#include <furi_hal.h>
#include "usb_cdc.h"
#include <cli/cli_vcp.h>
#include <cli/cli.h>
#include <furi_hal_usb_cdc.h>

typedef struct AvrIspVcpCdc AvrIspVcpCdc;

#define AVR_ISP_VCP_CDC_CH 1
#define AVR_ISP_VCP_CDC_PKT_LEN CDC_DATA_SZ
#define AVR_ISP_VCP_UART_RX_BUF_SIZE (AVR_ISP_VCP_CDC_PKT_LEN * 5)

void avr_isp_vcp_cdc_init(void);
void avr_isp_vcp_cdc_deinit(void);
int32_t avr_isp_vcp_cdc_receive(uint8_t if_num, uint8_t* buf, uint16_t max_len);
void avr_isp_vcp_cdc_send(uint8_t if_num, uint8_t* buf, uint16_t len);