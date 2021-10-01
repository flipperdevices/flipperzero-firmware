/* This file is the part of the Lightweight USB device Stack for STM32 microcontrollers
 *
 * Copyright ©2016 Dmitry Filimonchuk <dmitrystu[at]gmail[dot]com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _USB_HID_USAGE_SIMUL_H_
#define _USB_HID_USAHE_SUMUL_H_
#ifdef __cplusplus
    extern "C" {
#endif


/**\ingroup USB_HID
 * \addtogroup USB_HID_USAGES_SIMUL HID Usage Tables for Simulation
 * \brief Contains USB HID Usages definitions for Simulation Controls Page
 * \details This module based on
 * + [HID Usage Tables Version 1.12](https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
 * @{ */

#define HID_PAGE_SIMULATION             0x02    /**<\brief Sumulation usage page.*/
#define HID_SIMUL_SPORTS                0x08    /**<\brief CA Genetic sports simulation device.*/

/**\name Flight Simulation Devices
 * @{ */
#define HID_SIMUL_FLIGHT                0x01    /**<\brief CA Airplane simulation device.*/
#define HID_SIMUL_SPACESHIP             0x04    /**<\brief CA Spaceship simulation device.*/
#define HID_SIMUL_AIRPLANE              0x09    /**<\brief CA Airplane simulation device.*/
#define HID_SIMUL_HELICOPTER            0x0A    /**<\brief CA Helicopter simulation device.*/
#define HID_SIMUL_ALIERON               0xB0    /**<\brief DV Aileron control.*/
#define HID_SIMUL_ALIERIN_TRIM          0xB1    /**<\brief DV Aileron fine adjustment.*/
#define HID_SIMUL_ANTI_TORQUE           0xB2    /**<\brief DV Rudder pedals.*/
#define HID_SIMUL_AUTOPILOT_ENABLE      0xB3    /**<\brief OOC Autopilot switch.*/
#define HID_SIMUL_CHAFF_RELEASE         0xB4    /**<\brief OCS Chaff Release control.*/
#define HID_SIMUL_COLLECTIVE_CONTROL    0xB5    /**<\brief DV Vertical acceleration lift confrol.*/
#define HID_SIMUL_CYCLIC_CONTROL        0x22    /**<\brief CP Helicopter cyclic control.*/
#define HID_SIMUL_CYCLIC_TRIM           0x23    /**<\brief CP Cyclic fine adjustments.*/
#define HID_SIMUL_DRIVE_BRAKE           0xB6    /**<\brief DV Air brake control.*/
#define HID_SIMUL_ELECTR_COUNTERMEAS    0xB7    /**<\brief OOC Enables electronic countermeasures.*/
#define HID_SIMUL_ELEVATOR              0xB8    /**<\brief DV Elevator control.*/
#define HID_SIMUL_ELEVATOR_TRIM         0xB9    /**<\brief DV Elevator fine adjustment.*/
#define HID_SIMUL_FLIGHT_COMM           0xBC    /**<\brief OOC Flight Communications switch.*/
#define HID_SIMUL_FLARE_RELEASE         0xBD    /**<\brief OCS Flare release button.*/
#define HID_SIMUL_FLIGHT_CONTROL_STICK  0x20    /**<\brief CA Pitch and Roll control.*/
#define HID_SIMUL_FLIGHT_STICK          0x21    /**<\brief CA Pitch and Roll control for games.*/
#define HID_SIMUL_LANDING_GEAR          0xBE    /**<\brief OOC Landing gear control.*/
#define HID_SIMUL_RUDDER                0xBA    /**<\brief DV Rudder control.*/
#define HID_SIMUL_TOE_BRAKE             0xBF    /**<\brief DV Toe Brake control.*/
#define HID_SIMUL_THROTTLE              0xBB    /**<\brief DV Trottle control.*/
#define HID_SIMUL_TRIGGER               0xC0    /**<\brief MC Firearm trigger control.*/
#define HID_SIMUL_WEAPONS_ARM           0xC1    /**<\brief OOC Enables weapons system.*/
#define HID_SIMUL_WEAPONS_SELECT        0xC2    /**<\brief OSC Select weapon.*/
#define HID_SIMUL_WING_FLAPS            0xC3    /**<\brief DV wing flap control.*/
#define HID_SIMUL_FLIGHT_YOKE           0x24    /**<\brief CA A Flight Yoke controls.*/
/** @} */

/**\name Automobile Simulation Devices
 * @{ */
#define HID_SIMUL_AUTOMOBILE            0x02    /**<\brief CA Automobile or truck simulation device.*/
#define HID_SIMUL_ACCELERATOR           0xC4    /**<\brief DV Accelerator control.*/
#define HID_SIMUL_BRAKE                 0xC5    /**<\brief DV Brake control.*/
#define HID_SIMUL_CLUTCH                0xC6    /**<\brief DV Clutch control.*/
#define HID_SIMUL_SHIFTER               0xC7    /**<\brief DV Shifting gears control.*/
#define HID_SIMUL_STEERING              0xC8    /**<\brief DV Steering wheel control.*/
/** @} */

/**\name Tank Simulation Devices
 * @{ */
#define HID_SIMUL_TANK                  0x03    /**<\brief CA Treaded vehicle simulation device.*/
#define HID_SIMUL_TRACK_CONTROL         0x25    /**<\brief CP Direction and velocity controls.*/
#define HID_SIMUL_TURRET_DIRECTION      0xC9    /**<\brief DV Turret control right-left.*/
#define HID_SIMUL_BARREL_ELEVATION      0xCA    /**<\brief DV Gun elevation control.*/
/** @} */

/**\name Maritime Simulation Devices
 * @{ */
#define HID_SIMUL_SUBMARINE             0x05    /**<\brief CA Submarine control.*/
#define HID_SIMUL_SAILING               0x06    /**<\brief CA Sailing simulatiion control.*/
#define HID_SIMUL_DIVE_PLANE            0xCB    /**<\brief DV Dive plane control*/
#define HID_SIMUL_BALLAST               0xCC    /**<\brief DV Ballast control.*/
/** @} */

/**\name Two-wheeled Simulation Devices
 * @{ */
#define HID_SIMUL_MOTOCYCLE             0x07    /**<\brief CA Motocycle simulation device.*/
#define HID_SIMUL_BICYCLE               0x0C    /**<\brief CA Bycicle simulation device*/
#define HID_SIMUL_BICYCLE_CRANK         0xCD    /**<\brief DV Bycicle crank control.*/
#define HID_SIMUL_HANDLE_BARS           0xCE    /**<\brief DV Steering control.*/
#define HID_SIMUL_FRONT_BRAKE           0xCF    /**<\brief DV Front brake control.*/
#define HID_SIMUL_REAR_BRAKE            0xD0    /**<\brief DV Rear brake control.*/
/** @} */

/**\name Miscellaneous Simulation Devices
 * @{ */
#define HID_SIMUL_MAGIC_CARPET          0x0B    /**<\brief CA Magic carpet simulation device.*/
/** @}  */
/** @}  */

#ifdef __cplusplus
    }
#endif

#endif

