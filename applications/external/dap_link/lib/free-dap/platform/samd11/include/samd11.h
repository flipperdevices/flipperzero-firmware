/**
 * \file
 *
 * \brief Top header file for SAMD11
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMD11_
#define _SAMD11_

/**
 * \defgroup SAMD11_definitions SAMD11 Device Definitions
 * \brief SAMD11 CMSIS Definitions.
 */

#if   defined(__SAMD11C14A__) || defined(__ATSAMD11C14A__)
  #include "samd11c14a.h"
#elif defined(__SAMD11D14AM__) || defined(__ATSAMD11D14AM__)
  #include "samd11d14am.h"
#elif defined(__SAMD11D14AS__) || defined(__ATSAMD11D14AS__)
  #include "samd11d14as.h"
#elif defined(__SAMD11D14AU__) || defined(__ATSAMD11D14AU__)
  #include "samd11d14au.h"
#else
  #error Library does not support the specified device.
#endif

#endif /* _SAMD11_ */
