#pragma once

#define CDC_DATA_SZ     0x40

void cdc_send(uint8_t* buf, uint16_t len);

int32_t cdc_receive(uint8_t* buf, uint16_t max_len);
