/* This file is the part of the Lightweight USB device Stack for STM32 microcontrollers
 *
 * Copyright ©2019 Dmitry Filimonchuk <dmitrystu[at]gmail[dot]com>
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

#ifndef _USB_HID_POWER_DEVICES_H_
#define _USB_HID_POWER_DEVICES_H_

/**\ingroup USB_HID
 * \addtogroup USB_HID_POWER HID Usage Tables for Power Devices.
 * \brief Contains USB HID Usage tables definitions for HID Power Devices.
 * \details This module based on
 * + [Universal Serial Bus Usage Tables for HID Power Devices. Release 1.0](https://usb.org/sites/default/files/documents/pdcv10.pdf)
 * @{ */

/**
 * Name             | Physical Unit | HID unit | HID unit code | HID unit exponent | HID size
 * -----------------|---------------|----------|---------------|-------------------|---------
 * AC voltage       | Volt          | Volt     | 0x00F0D121    | 7                 | 8
 * AC current       | centiAmp      | Amp      | 0x00100001    | -2                | 16
 * Frequency        | Hertz         | Hertz    | 0xF001        | 0                 | 8
 * DC voltage       | centiVolt     | Volt     | 0x00F0D121    | 5                 | 16
 * Time             | second        | s        | 0x1001        | 0                 | 16
 * DC current       | centiAmp      | Amp      | 0x00100001    | -2                | 16
 * Power            | VA or W       | VA or W  | 0xD121        | 7                 | 16
 * Temperature      | K degree      | K degree | 0x00010001    | 0                 | 16
 * Battery capacity | AmpSec        | AmpSec   | 0x00101001    | 0                 | 24
 *
 */

/**\name Measures and Physical Units ant it's exponents in Reports  according to this table.*/
//@{
/**AC voltage unit code. Volt.*/
#define HID_POWER_UNIT_AC_VOLTAGE           0x67, 0x21, 0xD1, 0xF0, 0x00, 0x55, 0x07
/**AC current unit code. centiAmpere.*/
#define HID_POWER_UNIT_AC_CURRENT           0x67, 0x01, 0x00, 0x10, 0x00, 0x55, 0xFE
/**Frequency unit code. Hertz.*/
#define HID_POWER_UNIT_FREQUENCY            0x66, 0x01, 0xF0, 0x55, 0x00
/**DC voltage unit code. Volt.*/
#define HID_POWER_UNIT_DC_VOLTAGE           0x67, 0x21, 0xD1, 0xF0, 0x00, 0x55, 0x07
/**Time unit code. Second.*/
#define HID_POWER_UNIT_TIME                 0x66, 0x01, 0x10, 0x55, 0x00
/**DC current unit code. centiAmpere.*/
#define HID_POWER_UNIT_DC_CURRENT           0x67, 0x01, 0x00, 0x10, 0x00, 0x55, 0xFE
/**Power unic code. Watt.*/
#define HID_POWER_UNIT_POWER                0x66, 0x21, 0xD1, 0x55, 0x07
/**Temperature unit code. K degree.*/
#define HID_POWER_UNIT_TEMPERATURE          0x67, 0x01, 0x00, 0x01, 0x00, 0x55, 0x00
/**Battery capacity unit code. AmpSec.*/
#define HID_POWER_UNIT_BATTERY_CAPACITY     0x67, 0x01, 0x10, 0x10, 0x00, 0x55, 0x00
//@}
/** Macro to encode battery manufacturing date.
 * \param y Year [1980 - 2108]
 * \param m Month [1 - 12]
 * \param d Day [1 - 31]
*/
#define BATTERY_MGF_DATE(y,m,d) (((((y) - 1980) & 0x7F) << 9) + (((m) & 0x0F) << 5) + ((d) & 0x1F))

/** \addtogroup HID_POWER_DEVICE Power Device Page */
//@{
#define HID_POWER_PAGE                      0x84    /**<Power device page*/
/**\name Power Device Structure */
//@{
#define HID_POWER_INAME                     0x01    /**<[SV] Index of the name string descriptor.*/
#define HID_POWER_PRESENT_STATUS            0x02    /**<[CL] Collection of Present status information related to an object.*/
#define HID_POWER_CHANGED_STATUS            0x03    /**<[CL] Collection of Changed status information related to an object. */
#define HID_POWER_UPS                       0x04    /**<[CA] Collection that defines an Uninterruptible Power Supply.*/
#define HID_POWER_POWER_SUPPLY              0x05    /**<[CA] Collection that defines a Power Supply.*/
#define HID_POWER_BATTERY_SYSTEM            0x10    /**<[CP] Collection that defines a Battery System power module.*/
#define HID_POWER_BATTERY_SYSTEM_ID         0x11    /**<[SV] Number that points to a particular Battery System.*/
#define HID_POWER_BATTERY                   0x12    /**<[CP] Collection that defines a Battery.*/
#define HID_POWER_BATTERY_ID                0x13    /**<[SV] Number that points to a particular Battery.*/
#define HID_POWER_CHARGER                   0x14    /**<[CP] Collection that defines a Charger.*/
#define HID_POWER_CHARGER_ID                0x15    /**<[SV] Number that points to a particular Charger.*/
#define HID_POWER_POWER_CONVERTER           0x16    /**<[CP] Collection that defines a Power Converter power module.*/
#define HID_POWER_POWER_CONVERTER_ID        0x17    /**<[SV] Number that points to a particular Power Converter.*/
#define HID_POWER_OUTLET_SYSTEM             0x18    /**<[CP] Collection that defines a Outlet System power module.*/
#define HID_POWER_OUTLET_SYSTEM_ID          0x19    /**<[SV] Number that points to a particular Outlet System.*/
#define HID_POWER_INPUT                     0x1A    /**<[CP] Collection that defines an Input.*/
#define HID_POWER_INPUT_ID                  0x1B    /**<[SV] Number that points to a particular Input.*/
#define HID_POWER_OUTPUT                    0x1C    /**<[CP] Collection that defines an Output.*/
#define HID_POWER_OUTPUT_ID                 0x1D    /**<[SV] Number that points to a particular Output.*/
#define HID_POWER_FLOW                      0x1E    /**<[CP] Collection that defines a Flow.*/
#define HID_POWER_FLOW_ID                   0x1F    /**<[SV] Number that points to a particular Flow.*/
#define HID_POWER_OUTLET                    0x20    /**<[CP] Collection that defines an Outlet.*/
#define HID_POWER_OUTLET_ID                 0x21    /**<[SV] Number that points to a particular Outlet*/
#define HID_POWER_GANG                      0x22    /**<[CL/CP] Collection that defines ganged objects.*/
#define HID_POWER_GANG_ID                   0x23    /**<[SV] Number that points to a particular Gang.*/
#define HID_POWER_POWER_SUMMARY             0x24    /**<[CL/CP] Collection that defines a Power Summary.*/
#define HID_POWER_POWER_SUMMARY_ID          0x25    /**<[SV] Number that points to a particular Power Summary.*/
//@}
/**\name Power Measures */
//@{
#define HID_POWER_VOLTAGE                   0x30    /**<[DV] Actual value of the voltage.*/
#define HID_POWER_CURRENT                   0x31    /**<[DV] Actual value of the current.*/
#define HID_POWER_FREQUENCY                 0x32    /**<[DV] Actual value of the frequency.*/
#define HID_POWER_APPARENT_POWER            0x33    /**<[DV] Actual value of the apparent power.*/
#define HID_POWER_ACTIVE_POWER              0x34    /**<[DV] Actual value of the active (RMS) power.*/
#define HID_POWER_PERCENT_LOAD              0x35    /**<[DV] Actual value of the percentage of the power capacity presently being used on this input or output line.*/
#define HID_POWER_TEMPERATURE               0x36    /**<[DV] Actual value of the temperature.*/
#define HID_POWER_HUMIDITY                  0x37    /**<[DV] Actual value of the humidity.*/
#define HID_POWER_BAD_COUNT                 0x38    /**<[DV] Number of times the device, module, or sub-module entered a bad condition.*/
//@}
/**\name Power configuration controls */
//@{
#define HID_POWER_CONFIG_VOLTAGE            0x40    /**<[SV/DV] Nominal value of the voltage.*/
#define HID_POWER_CONFIG_CURRENT            0x41    /**<[SV/DV] Nominal value of the current.*/
#define HID_POWER_CONFIG_FREQUENCY          0x42    /**<[SV/DV] Nominal value of the frequency.*/
#define HID_POWER_CONFIG_APPARENT_POWER     0x43    /**<[SV/DV] Nominal value of the apparent power.*/
#define HID_POWER_CONFIG_ACTIVE_POWER       0x44    /**<[SV/DV] Nominal value of the active (RMS) power.*/
#define HID_POWER_CONFIG_PERCENT_LOAD       0x45    /**<[SV/DV] Nominal value of the percentage load that could be used without critical overload.*/
#define HID_POWER_CONFIG_TEMPERATURE        0x46    /**<[SV/DV] Nominal value of the temperature.*/
#define HID_POWER_CONFIG_HUMIDITY           0x47    /**<[SV/DV] Nominal value of the humidity.*/
//@}
/**\name Power controls */
//@{
#define HID_POWER_SWITCH_ON_CONTROL         0x50    /**<[DV] Controls the Switch ON sequence. \see \ref HID_POWER_SWITCH_VALUES */
#define HID_POWER_SWITCH_OFF_CONTROL        0x51    /**<[DV] Controls the Switch OFF sequence. \see \ref HID_POWER_SWITCH_VALUES */
#define HID_POWER_TOGGLE_CONTROL            0x52    /**<[DV] Controls the Toggle sequence. \see \ref HID_POWER_SWITCH_VALUES */
#define HID_POWER_LOW_VOLTAGE_TRANSFER      0x53    /**<[DV] Minimum line voltage allowed before the PS system transfers to battery backup.*/
#define HID_POWER_HIGH_VOLTAGE_TRANSFER     0x54    /**<[DV] Maximum line voltage allowed before the PS system transfers to battery backup.*/
#define HID_POWER_DELAY_BEFORE_REBOOT       0x55    /**<[DV] Writing this value immediately shuts down (i.e., turns off) the output for a
                                                     * period equal to the indicated number of seconds, after which time the output is
                                                     * started. If the number of seconds required to perform the request is greater than
                                                     * the requested duration, then the requested shutdown and startup cycle shall be
                                                     * performed in the minimum time possible, but in no case shall this require more than
                                                     * the requested duration plus 60 seconds. If the startup should occur during a utility
                                                     * failure, the startup shall not occur until the utility power is restored. \n When read,
                                                     * returns the number of seconds remaining in the countdown, or –1 if no countdown is in
                                                     * progress.*/
#define HID_POWER_DELAY_BEFORE_STARTUP      0x56    /**<[DV] Writing this value starts the output after the indicated number of seconds.
                                                     * Sending this command with 0 causes the startup to occur immediately. Sending this
                                                     * command with –1 aborts the countdown. If the output is already on at the time the
                                                     * countdown reaches 0, nothing happens. On some systems, if the USB driver on the
                                                     * device side is restarted while a startup countdown is in effect, the countdown is
                                                     * aborted. If the countdown expires during a utility failure, the startup shall not
                                                     * occur until the utility power is restored. Writing this value overrides the effect
                                                     * of any countdown in progress. \n When read, returns the number of seconds remaining
                                                     * in the countdown, or –1 if no countdown is in progress. */
#define HID_POWER_DELAY_BEFORE_SHUTDOWN     0x57    /**<[DV] Writing this value shuts down either the output after the indicated number of
                                                     * seconds, or sooner if the batteries become depleted. Sending this command with 0
                                                     * causes the shutdown to occur immediately. Sending this command with –1 aborts the
                                                     * countdown. If the system is already in the desired state at the time the countdown
                                                     * reaches 0, there is no additional action. On some systems, if the USB driver on the
                                                     * device side is restarted while a shutdown countdown is in effect, the countdown may
                                                     * be aborted. Writing this value overrides any DelayBeforeShutdown countdown already
                                                     * in effect. \n When read, will return the number of seconds remaining until shutdown,
                                                     * or –1 if no shutdown countdown is in effect.*/
#define HID_POWER_TEST                      0x58    /**<[DV] Test request or result value. \see \ref HID_POWER_TEST_VALUES */
#define HID_POWER_MODULE_RESET              0x59    /**<[DV] Module Reset request value. \see \ref HID_POWER_RESET_VALUES */
#define HID_POWER_AUDIBLE_ALARM_CONTROL     0x5A    /**<[DV] Audible alarm value. \see \ref HID_POWER_ALARM_VALUES */
//@}
/**\name Power generic status */
//@{
#define HID_POWER_PRESENT                   0x60    /**<[DF] Power present flag.*/
#define HID_POWER_GOOD                      0x61    /**<[DF] Power good flag.*/
#define HID_POWER_INTERNAL_FAILURE          0x62    /**<[DF] Inetrnal failure flag.*/
#define HID_POWER_VOLTAGE_OUT_OF_RANGE      0x63    /**<[DF] Voltage out of range flag.*/
#define HID_POWER_FREQUENCY_OUT_OF_RANGE    0x64    /**<[DF] Frequency out of range flag.*/
#define HID_POWER_OVERLOAD                  0x65    /**<[DF] Overload flag.*/
#define HID_POWER_OVERCHARGED               0x66    /**<[DF] Overcharged flag.*/
#define HID_POWER_OVERTEMPERATURE           0x67    /**<[DF] Overtemperature flag.*/
#define HID_POWER_SUTDOWN_REQUESTED         0x68    /**<[DF] Shutdown requested flag.*/
#define HID_POWER_SHUTDOWN_IMMINENT         0x69    /**<[DF] Shutdown imminent flag.*/
#define HID_POWER_SWITCH_ON_OFF             0x6B    /**<[DF] Switch ON flag.*/
#define HID_POWER_SWITHABLE                 0x6C    /**<[DF] Swithable flag.*/
#define HID_POWER_USED                      0x6D    /**<[DF] Used flag.*/
#define HID_POWER_BOOST                     0x6E    /**<[DF] Voltage boosted flag.*/
#define HID_POWER_BUCK                      0x6F    /**<[DF] Voltage bucked flag.*/
#define HID_POWER_INITIALIZED               0x70    /**<[DF] Initialized flag.*/
#define HID_POWER_TESTED                    0x71    /**<[DF] Tested flag.*/
#define HID_POWER_AWAITING_POWER            0x72    /**<[DF] Awaiting power flag.*/
#define HID_POWER_COMMUNICATION_LOST        0x73    /**<[DF] Communication lost flag.*/
//@}
/**\name Power device identification */
#define HID_POWER_IMANUFACTURER             0xFD    /**<[SV] Index of the manufacturer string descriptor.*/
#define HID_POWER_IPRODUCT                  0xFE    /**<[SV] Index of the prodict string descriptor.*/
#define HID_POWER_ISERIALNUMBER             0xFF    /**<[SV] Index of the serial number string descriptor.*/
/**\name Switch ON/OFF/TOGGLE sequence values
 * \anchor HID_POWER_SWITCH_VALUES */
#define HID_POWER_STOP_SEQUENCE             0x00    /**<Write value. Stop sequence.*/
#define HID_POWER_START_SEQUENCE            0x01    /**<Write value. Start sequence.*/
#define HID_POWER_SEQUENCE_NONE             0x00    /**<Read value. No sequence.*/
#define HID_POWER_SEQUENCE_STARTED          0x01    /**<Read value. Sequence started.*/
#define HID_POWER_SEQUENCE_INPROGRESS       0x02    /**<Read value. Sequence in progress.*/
#define HID_POWER_SEQUENCE_STOPPED          0x03    /**<Read value. Sequence completed.*/
/**\name Test request/result values
 * \anchor HID_POWER_TEST_VALUES */
#define HID_POWER_TEST_NO                   0x00    /**<Write value. No test.*/
#define HID_POWER_TEST_QUICK                0x01    /**<Write value. Quck test.*/
#define HID_POWER_TEST_DEEP                 0x02    /**<Write value. Deep test.*/
#define HID_POWER_TEST_ABORT                0x03    /**<Write valie. Abort test.*/
#define HID_POWER_TEST_PASSED               0x01    /**<Read value. Test done and passed.*/
#define HID_POWER_TEST_WARNED               0x02    /**<Read value. Test done with warnings.*/
#define HID_POWER_TEST_ERROR                0x03    /**<Read value. Test done with errors.*/
#define HID_POWER_TEST_ABORTED              0x04    /**<Read value. Test aborted.*/
#define HID_POWER_TEST_INPROGRESS           0x05    /**<Read value. Test in progress.*/
#define HID_POWER_TEST_NOT_INITIATED        0x06    /**<Read value. No test inititted.*/

/**\name Module reset values
 * \anchor HID_POWER_RESET_VALUES */
#define HID_POWER_RESET_NO                  0x00    /**<Read/Write value. No reset.*/
#define HID_POWER_RESET_MODULE              0x01    /**<Read/Write value. Reset module.*/
#define HID_POWER_RESET_ALARMS              0x02    /**<Read/Write value. Reset module's alarms.*/
#define HID_POWER_RESET_COUNTERS            0x03    /**<Read/Write value. Reset module's counters.*/

/**\name Audible alarm values
 * \anchor HID_POWER_ALARM_VALUES
 * @{ */
#define HID_POWER_ALARM_DISABLED            0x00    /**<Read/Write value. Audible alarm disabled.*/
#define HID_POWER_ALARM_ENABLED             0x01    /**<Read/Write value. Audible alarm enabled.*/
#define HID_POWER_ALARM_MUTED               0x02    /**<Read/Write value. Audible alarm muted.*/
/** @} */
/** @} */

/** \addtogroup HID_BATTERY_DEVICE Battery Device Page
 * @{ */
#define HID_BATTERY_PAGE                    0x85    /**<[CL] Battery usage page.*/
#define HID_BATTERY_SMB_BATTERY_MODE        0x01    /**<[CL] SMB-specific collection used by the battery for mode setting.*/
#define HID_BATTERY_SMB_BATTERY_STATUS      0x02    /**<[CL] SMB-specific collection used by the battery for Status and Alarm read.*/
#define HID_BATTERY_SMB_ALARM_WARNING       0x03    /**<[CL] SMB-specific collection used by the battery for Alarm transmission to Charger and Host.*/
#define HID_BATTERY_SMB_CHARGER_MODE        0x04    /**<[CL] SMB-specific collection used by the Charger for mode setting.*/
#define HID_BATTERY_SMB_CHARGER_STATUS      0x05    /**<[CL] SMB-specific collection used by the Charger for status transmission.*/
#define HID_BATTERY_SMB_CHARGER_SPECINFO    0x06    /**<[CL] SMB-specific collection used by the Charger for extended status information.*/
#define HID_BATTERY_SMB_SELECTOR_STATE      0x07    /**<[CL] SMB-specific collection to manage Selector Features.*/
#define HID_BATTERY_SMB_SELECTOR_PRESETS    0x08    /**<[CL] SMB-specific collection to select the next battery to power the system in the
                                                     * event the current battery is removed or falls below its cutoff voltage.*/
#define HID_BATTERY_SMB_SELECTOR_INFO       0x09    /**<[CL] SMB-specific collection of information used by the host to determine the capabilities of the selector.*/
/**\name Battery system (or selector) settings and controls */
#define HID_BATTERY_OPTIONAL_MFG_FUNC1      0x10    /**<[DV] An optional SMB-manufacturer-specific Read and Write function.*/
#define HID_BATTERY_OPTIONAL_MFG_FUNC2      0x11    /**<[DV] An optional SMB-manufacturer-specific Read and Write function.*/
#define HID_BATTERY_OPTIONAL_MFG_FUNC3      0x12    /**<[DV] An optional SMB-manufacturer-specific Read and Write function.*/
#define HID_BATTERY_OPTIONAL_MFG_FUNC4      0x13    /**<[DV] An optional SMB-manufacturer-specific Read and Write function.*/
#define HID_BATTERY_OPTIONAL_MFG_FUNC5      0x14    /**<[DV] An optional SMB-manufacturer-specific Read and Write function.*/
#define HID_BATTERY_CONNECTION_TO_SMBUS     0x15    /**<[DF] State of connection to the system SMBus.*/
#define HID_BATTERY_OUTPUT_CONNECTION       0x16    /**<[DV] Connection status of the specified Output.*/
#define HID_BATTERY_CHARGER_CONNECTION      0x17    /**<[DV] ID of the specified Charger to the specified Battery.*/
#define HID_BATTERY_BATTERY_INSERTION       0x18    /**<[DF] Insertion status of the specified Battery into the system.*/
#define HID_BATTERY_USE_NEXT                0x19    /**<[DF] Whether or not this Battery will be used for next discharge.*/
#define HID_BATTERY_OK_TO_USE               0x1A    /**<[DF] Whether or not this Battery is usable.*/
#define HID_BATTERY_BATTERY_SUPPORTED       0x1B    /**<[DF] Whether or not this Battery is supported by the selector.*/
#define HID_BATTERY_SELECTOR_REVISION       0x1C    /**<[DV] Version of the Smart Battery Selector specification.*/
#define HID_BATTERY_CHARGING_INDICATOR      0x1D    /**<[DF] A bit flag that indicates whether the selector reports the charger’s status in the POWERBY nibble of SelectorState.*/
/**\name Battery controls */
#define HID_BATTERY_MANUFACTURER_ACCESS     0x28    /**<[DV] Read/Write according to the Smart Battery Data Specification.*/
#define HID_BATTERY_REMAINING_CAP_LIMIT     0x29    /**<[DV] */
#define HID_BATTERY_REMAINING_TIME_LIMIT    0x2A    /**<[DV] */
#define HID_BATTERY_ATRATE                  0x2B    /**<[DV] */
#define HID_BATTERY_CAPACITY_MODE           0x2C    /**<[DV] Battery capacity units. \see \ref HID_BATTERY_CAPACITY_UNITS */
#define HID_BATTERY_BROADCAST_TO_CHARGER    0x2D    /**<[DF] Enable broadcast to charger.*/
#define HID_BATTERY_PRIMARY_BATTERY         0x2E    /**<[DF] Battery operates in its primary role.*/
#define HID_BATTERY_CHARGE_CONTROLLER       0x2F    /**<[DF] Internal charge control enabled.*/
/**\name Battery status */
#define HID_BATTERY_TERMINATE_CHARGE        0x40    /**<[DF] Terminate charge.*/
#define HID_BATTERY_TERMINATE_DISCHARGE     0x41    /**<[DF] Terminate discharge.*/
#define HID_BATTERY_BELOW_REM_CAP_LIMIT     0x42    /**<[DF] Battery below remained capacity limit.*/
#define HID_BATTERY_REM_TIME_LIMIT_EXPIRED  0x43    /**<[DF] Remaining time limit expired.*/
#define HID_BATTERY_CHARGING                0x44    /**<[DF] Battery charging.*/
#define HID_BATTERY_DISCHARGING             0x45    /**<[DF] Battery discharging.*/
#define HID_BATTERY_FULLY_CHARGED           0x46    /**<[DF] Battery fully charged flag.*/
#define HID_BATTERY_FULLY_DISCHARGED        0x47    /**<[DF] Battery fully discharged flag.*/
#define HID_BATTERY_CONDITIONING_FLAG       0x48    /**<[DF] Battery needs conditioning cycle.*/
#define HID_BATTERY_ATRATE_OK               0x49    /**<[DF] At Rate values recalculated and available.*/
#define HID_BATTERY_SMB_ERROR_CODE          0x4A    /**<[DV] An SMB-specific 4-bit error code.*/
#define HID_BATTERY_NEED_REPLACEMENT        0x4B    /**<[DF] Battery need replacement flag.*/
/**\name Battery measures */
#define HID_BATTERY_ATRATE_TIME_TO_FILL     0x60    /**<[DV] The predicted remaining time in minutes to fully charge the battery at the AtRate value.*/
#define HID_BATTERY_ATRATE_TIME_TO_EMPTY    0x61    /**<[DV] The predicted operating time if the battery is discharged at the AtRate value.*/
#define HID_BATTERY_AVERAGE_CURRENT         0x62    /**<[DV] An one-minute rolling average of the current being supplied or accepted through the battery terminals.*/
#define HID_BATTERY_MAXERROR                0x63    /**<[DV] The expected margin error (%) in the state of charge calculation.*/
#define HID_BATTERY_REL_STATE_OF_CHARGE     0x64    /**<[DV] The predicted remaining battery capacity expressed as a percentage of the last measured full charge capacity. */
#define HID_BATTERY_ABS_STATE_OF_CHARGE     0x65    /**<[DV] The predicted remaining battery capacity expressed as a percentage of design capacity.*/
#define HID_BATTERY_REMAINING_CAPACITY      0x66    /**<[DV] The predicted remaining capacity.*/
#define HID_BATTERY_FULL_CHARGE_CAPACITY    0x67    /**<[DV] The predicted pack capacity when it is fully charged.*/
#define HID_BATTERY_RUN_TIME_TO_EMPTY       0x68    /**<[DV] The predicted remaining battery life, in minutes, at the present rate of discharge.*/
#define HID_BATTERY_AVG_TIME_TO_EMPTY       0x69    /**<[DV] A one-minute rolling average, in minutes, of the predicted remaining battery time life.*/
#define HID_BATTERY_AVG_TIME_TO_FULL        0x6A    /**<[DV] An one-minute rolling average, in minutes, of the predicted remaining time until the battery reaches full charge.*/
#define HID_BATTERY_CYCLE_COUNT             0x6B    /**<[DV] The number, in cycles, of charge/discharge cycles the battery has experienced.*/
/**\name Battery settings */
#define HID_BATTERY_BATT_PACK_MODEL_LEVEL   0x80    /**<[SV] Battery model level for the battery pack. \see \ref HID_BATTERY_MODEL_LEVELS */
#define HID_BATTERY_INT_CHARGE_CONTROLLER   0x81    /**<[SF] Charge controller function supported in the battery pack.*/
#define HID_BATTERY_PRIMARY_BATTERY_SUPPORT 0x82    /**<[SF] Primary battery function supported in the battery pack.*/
#define HID_BATTERY_DESIGN_CAPACITY         0x83    /**<[SV] The theoretical capacity of a new pack.*/
#define HID_BATTERY_SPECIFICATION_INFO      0x84    /**<[SV] The version number of the Smart Battery Data Specification.*/
#define HID_BATTERY_MANUFACTURER_DATE       0x85    /**<[SV] The date the pack was manufactured in a packed integer. \see \ref BATTERY_MGF_DATE(y,m,d) */
#define HID_BATTERY_SERIAL_NUMBER           0x86    /**<[SV] The cell pack serial number.*/
#define HID_BATTERY_IMANUFACTURER_NAME      0x87    /**<[SV] Index of a string descriptor containing the battery manufacturer’s name.*/
#define HID_BATTERY_IDEVICE_NAME            0x88    /**<[SV] Index of a string descriptor containing the battery’s name.*/
#define HID_BATTERY_IDEVICE_CHEMISTERY      0x89    /**<[SV] Index of a string descriptor containing the battery’s chemistry.*/
#define HID_BATTERY_MANUFACTURER_DATA       0x8A    /**<[SV] A binary data block containing manufacturer specific data.*/
/**\name Battery settings (ACPI specific) */
#define HID_BATTERY_RECHARGEABLE            0x8B    /**<[SF] It's a rechargeable battery.*/
#define HID_BATTERY_WARNINIG_CAP_LIMIT      0x8C    /**<[SV] OEM-designed battery warning capacity.*/
#define HID_BATTERY_CAP_GRANULARITY1        0x8D    /**<[SV] Battery capacity granularity between low and warning.*/
#define HID_BATTERY_CAP_GRANULARITY2        0x8E    /**<[SV] Battery capacity granularity between warning and full.*/
#define HID_BATTERY_IOEM_INFORMATION        0x8F    /**<[SV] Index of a string descriptor defining OEM specific information for the battery.*/
/**\name Charger controls */
#define HID_BATTERY_INHIBIT_CHARGE          0xC0    /**<[DF] Inhibit charging.*/
#define HID_BATTERY_ENABLE_POLLING          0xC1    /**<[DF] Enable polling.*/
#define HID_BATTERY_RESET_TO_ZERO           0xC2    /**<[DF] Reset Charging Current and Voltage values to zero.*/
/**\name Charger status */
#define HID_BATTERY_AC_PRESENT              0xD0    /**<[DF] AC present.*/
#define HID_BATTERY_BATTERY_PRESENT         0xD1    /**<[DF] Battery present.*/
#define HID_BATTERY_POWER_FAIL              0xD2    /**<[DF] Power fail.*/
#define HID_BATTERY_ALARM_INHIBITED         0xD3    /**<[DF] Alarm inhibited.*/
#define HID_BATTERY_THERMISTOR_UNDER_RANGE  0xD4    /**<[DF] Thermistor under range.*/
#define HID_BATTERY_THERMISTOR_HOT          0xD5    /**<[DF] Thermistor hot.*/
#define HID_BATTERY_THERMISTOR_COLD         0xD6    /**<[DF] Thermistor cold.*/
#define HID_BATTERY_THERMISTOR_OVER_RANGE   0xD7    /**<[DF] Thermistor over range.*/
#define HID_BATTERY_VOLTAGE_OUT_OF_RANGE    0xD8    /**<[DF] Voltage out of range.*/
#define HID_BATTERY_CURRENT_OUT_OF_RANGE    0xD9    /**<[DF] Current out of range.*/
#define HID_BATTERY_CURRENT_NOT_REGULATED   0xDA    /**<[DF] Current not regulated.*/
#define HID_BATTERY_VOLTAGE_NOT_REGULATED   0xDB    /**<[DF] Voltage not regulated.*/
#define HID_BATTERY_MASTER_MODE             0xDC    /**<[DF] Master mode (polling is enabled).*/
/**\name Charger settings */
//@{
#define HID_BATTERY_CHARGER_SELECTOR_SUPP   0xF0    /**<[SF] Charger selector support.*/
#define HID_BATTERY_CHARGER_SPEC            0xF1    /**<[SV] Specification reference. (0001 for SMB charger 1.0).*/
#define HID_BATTERY_LEVEL2                  0xF2    /**<[SF] Charger level flag 2*/
#define HID_BATTERY_LEVEL3                  0xF3    /**<[SF] Charger level flag 3*/
//@}

/**\name Battery capacity units
 * \anchor HID_BATTERY_CAPACITY_UNITS */
//@{
#define HID_BATTERY_CAPACITY_MAH            0x00    /**<Capacity unit is mAH (used in SMB) */
#define HID_BATTERY_CAPACITY_MWH            0x01    /**<Capacity unit is mWH (used in SMB) */
#define HID_BATTERY_CAPACITY_PERCENT        0x02    /**<Capacity unit is percent. */
#define HID_BATTERY_CAPACITY_BOOL           0x03    /**<Boolean unit (OK or not OK) */
//@}

/**\name Battery model levels
 * \anchor HID_BATTERY_MODEL_LEVELS */
//@{
#define HID_BATTERY_MODEL_BASIC             0x00    /**<Basic model.*/
#define HID_BATTERY_MODEL_INTELLIGENT       0x01    /**<Intelligent model.*/
#define HID_BATTERY_MODEL_SMART             0x02    /**<Smart battery.*/
//@}

//@}
/** @} */


#endif

