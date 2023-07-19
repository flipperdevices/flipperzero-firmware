// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2016, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _NVM_DATA_H_
#define _NVM_DATA_H_

/*- Definitions -------------------------------------------------------------*/
#define NVM_ADC_LINEARITY_POS        27
#define NVM_ADC_LINEARITY_SIZE       8

#define NVM_ADC_BIASCAL_POS          35
#define NVM_ADC_BIASCAL_SIZE         3

#define NVM_OSC32K_CAL_POS           38
#define NVM_OSC32K_CAL_SIZE          7

#define NVM_USB_TRANSN_POS           45
#define NVM_USB_TRANSN_SIZE          5

#define NVM_USB_TRANSP_POS           50
#define NVM_USB_TRANSP_SIZE          5

#define NVM_USB_TRIM_POS             55
#define NVM_USB_TRIM_SIZE            3

#define NVM_DFLL48M_COARSE_CAL_POS   58
#define NVM_DFLL48M_COARSE_CAL_SIZE  6

#define NVM_DFLL48M_FINE_CAL_POS     64
#define NVM_DFLL48M_FINE_CAL_SIZE    10

#define NVM_READ_CAL(cal) \
    ((*((uint32_t *)NVMCTRL_OTP4 + cal##_POS / 32)) >> (cal##_POS % 32)) & ((1 << cal##_SIZE) - 1)

#endif // _NVM_DATA_H_
