/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    templates/halconf.h
 * @brief   Bare-metal OSAL configuration header.
 *
 * @addtogroup OSAL_CONF
 * @{
 */

#ifndef _OSALCONF_H_
#define _OSALCONF_H_

/**
 * @brief   Frequency in Hertz of the system tick.
 */
#if !defined(OSAL_ST_FREQUENCY) || defined(__DOXYGEN__)
#define OSAL_ST_FREQUENCY                   1000
#endif

/**
 * @brief   Enables OSAL assertions.
 */
#if !defined(OSAL_DBG_ENABLE_ASSERTS) || defined(__DOXYGEN__)
#define OSAL_DBG_ENABLE_ASSERTS             FALSE
#endif

/**
 * @brief   Enables OSAL functions parameters checks.
 */
#if !defined(OSAL_DBG_ENABLE_CHECKS) || defined(__DOXYGEN__)
#define OSAL_DBG_ENABLE_CHECKS              FALSE
#endif

/**
 * @brief   OSAL initialization hook.
 */
#if !defined(OSAL_INIT_HOOK) || defined(__DOXYGEN__)
#define OSAL_INIT_HOOK() {                                                  \
}
#endif

/**
 * @brief   Idle loop hook macro.
 */
#if !defined(OSAL_IDLE_HOOK) || defined(__DOXYGEN__)
#define OSAL_IDLE_HOOK() {                                                  \
}
#endif

#endif /* _OSALCONF_H_ */

/** @} */
