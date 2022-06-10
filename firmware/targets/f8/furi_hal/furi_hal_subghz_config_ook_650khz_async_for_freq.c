/*! @file radio_config.h
 * @brief This file contains the automatically generated
 * configurations.
 *
 * @n WDS GUI Version: 3.2.11.0
 * @n Device: Si4463 Rev.: C2                                 
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2017 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

// USER DEFINED PARAMETERS
// Define your own parameters here

// INPUT DATA
/*
// Crys_freq(Hz): 32000000    Crys_tol(ppm): 20    IF_mode: 2    High_perf_Ch_Fil: 1    OSRtune: 0    Ch_Fil_Bw_AFC: 0    ANT_DIV: 0    PM_pattern: 15    
// MOD_type: 1    Rsymb(sps): 4800    Fdev(Hz): 48000    RXBW(Hz): 50000    Manchester: 0    AFC_en: 0    Rsymb_error: 0.0    Chip-Version: 2    
// RF Freq.(MHz): 433.92    API_TC: 29    fhst: 50000    inputBW: 0    BERT: 0    RAW_dout: 0    D_source: 1    Hi_pfm_div: 1    
// API_ARR_Det_en: 0    Fdev_error: 0    API_ETSI: 0    
// 
// # RX IF frequency is  -500000 Hz
// # WB filter 2 (BW =  54.97 kHz);  NB-filter 2 (BW = 54.97 kHz)
// 
// Modulation index: 20
*/
#include "furi_hal_subghz_config.h"

// CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ                     32000000L
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER                    0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH               0x00
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP        0x03
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET       0xF000



// CONFIGURATION COMMANDS

/*
// Command:                  RF_POWER_UP
// Description:              Command to power-up the device and select the operational mode and functionality.
*/
#define RF_POWER_UP 0x02, 0x81, 0x00, 0x01, 0xE8, 0x48, 0x00

/*
// Command:                  RF_GPIO_PIN_CFG
// Description:              Configures the GPIO pins.
*/
#define RF_GPIO_PIN_CFG 0x13, 0x20, 0x1B, 0x02, 0x02, 0x00, 0x00, 0x00

/*
// Set properties:           RF_GLOBAL_XO_TUNE_1
// Number of properties:     1
// Group ID:                 0x00
// Start ID:                 0x00
// Default values:           0x40, 
// Descriptions:
//   GLOBAL_XO_TUNE - Configure the internal capacitor frequency tuning bank for the crystal oscillator.
*/
#define RF_GLOBAL_XO_TUNE_1 0x11, 0x00, 0x01, 0x00, 0x52

/*
// Set properties:           RF_GLOBAL_CONFIG_1
// Number of properties:     1
// Group ID:                 0x00
// Start ID:                 0x03
// Default values:           0x20, 
// Descriptions:
//   GLOBAL_CONFIG - Global configuration settings.
*/
#define RF_GLOBAL_CONFIG_1 0x11, 0x00, 0x01, 0x03, 0x20

/*
// Set properties:           RF_PREAMBLE_CONFIG_1
// Number of properties:     1
// Group ID:                 0x10
// Start ID:                 0x04
// Default values:           0x21, 
// Descriptions:
//   PREAMBLE_CONFIG - General configuration bits for the Preamble field.
*/
#define RF_PREAMBLE_CONFIG_1 0x11, 0x10, 0x01, 0x04, 0x21

/*
// Set properties:           RF_MODEM_MOD_TYPE_12
// Number of properties:     12
// Group ID:                 0x20
// Start ID:                 0x00
// Default values:           0x02, 0x80, 0x07, 0x0F, 0x42, 0x40, 0x01, 0xC9, 0xC3, 0x80, 0x00, 0x06, 
// Descriptions:
//   MODEM_MOD_TYPE - Selects the type of modulation. In TX mode, additionally selects the source of the modulation.
//   MODEM_MAP_CONTROL - Controls polarity and mapping of transmit and receive bits.
//   MODEM_DSM_CTRL - Miscellaneous control bits for the Delta-Sigma Modulator (DSM) in the PLL Synthesizer.
//   MODEM_DATA_RATE_2 - Unsigned 24-bit value used to determine the TX data rate
//   MODEM_DATA_RATE_1 - Unsigned 24-bit value used to determine the TX data rate
//   MODEM_DATA_RATE_0 - Unsigned 24-bit value used to determine the TX data rate
//   MODEM_TX_NCO_MODE_3 - TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
//   MODEM_TX_NCO_MODE_2 - TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
//   MODEM_TX_NCO_MODE_1 - TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
//   MODEM_TX_NCO_MODE_0 - TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
//   MODEM_FREQ_DEV_2 - 17-bit unsigned TX frequency deviation word.
//   MODEM_FREQ_DEV_1 - 17-bit unsigned TX frequency deviation word.
*/
#define RF_MODEM_MOD_TYPE_12 0x11, 0x20, 0x0C, 0x00, 0x0B, 0x00, 0x07, 0x02, 0x71, 0x00, 0x05, 0xE8, 0x48, 0x00, 0x00, 0x00

/*
// Set properties:           RF_MODEM_FREQ_DEV_0_1
// Number of properties:     1
// Group ID:                 0x20
// Start ID:                 0x0C
// Default values:           0xD3, 
// Descriptions:
//   MODEM_FREQ_DEV_0 - 17-bit unsigned TX frequency deviation word.
*/
#define RF_MODEM_FREQ_DEV_0_1 0x11, 0x20, 0x01, 0x0C, 0x42

/*
// Set properties:           RF_MODEM_TX_RAMP_DELAY_12
// Number of properties:     12
// Group ID:                 0x20
// Start ID:                 0x18
// Default values:           0x01, 0x00, 0x08, 0x03, 0xC0, 0x00, 0x10, 0x20, 0x00, 0x00, 0x00, 0x4B, 
// Descriptions:
//   MODEM_TX_RAMP_DELAY - TX ramp-down delay setting.
//   MODEM_MDM_CTRL - MDM control.
//   MODEM_IF_CONTROL - Selects Fixed-IF, Scaled-IF, or Zero-IF mode of RX Modem operation.
//   MODEM_IF_FREQ_2 - the IF frequency setting (an 18-bit signed number).
//   MODEM_IF_FREQ_1 - the IF frequency setting (an 18-bit signed number).
//   MODEM_IF_FREQ_0 - the IF frequency setting (an 18-bit signed number).
//   MODEM_DECIMATION_CFG1 - Specifies three decimator ratios for the Cascaded Integrator Comb (CIC) filter.
//   MODEM_DECIMATION_CFG0 - Specifies miscellaneous parameters and decimator ratios for the Cascaded Integrator Comb (CIC) filter.
//   MODEM_DECIMATION_CFG2 - Specifies miscellaneous decimator filter selections.
//   MODEM_IFPKD_THRESHOLDS - 
//   MODEM_BCR_OSR_1 - RX BCR/Slicer oversampling rate (12-bit unsigned number).
//   MODEM_BCR_OSR_0 - RX BCR/Slicer oversampling rate (12-bit unsigned number).
*/
#define RF_MODEM_TX_RAMP_DELAY_12 0x11, 0x20, 0x0C, 0x18, 0x01, 0x00, 0x08, 0x03, 0x80, 0x00, 0xB0, 0x10, 0x0C, 0xE8, 0x00, 0x53

/*
// Set properties:           RF_MODEM_BCR_NCO_OFFSET_2_12
// Number of properties:     12
// Group ID:                 0x20
// Start ID:                 0x24
// Default values:           0x06, 0xD3, 0xA0, 0x06, 0xD3, 0x02, 0xC0, 0x00, 0x00, 0x23, 0x83, 0x69, 
// Descriptions:
//   MODEM_BCR_NCO_OFFSET_2 - RX BCR NCO offset value (an unsigned 22-bit number).
//   MODEM_BCR_NCO_OFFSET_1 - RX BCR NCO offset value (an unsigned 22-bit number).
//   MODEM_BCR_NCO_OFFSET_0 - RX BCR NCO offset value (an unsigned 22-bit number).
//   MODEM_BCR_GAIN_1 - The unsigned 11-bit RX BCR loop gain value.
//   MODEM_BCR_GAIN_0 - The unsigned 11-bit RX BCR loop gain value.
//   MODEM_BCR_GEAR - RX BCR loop gear control.
//   MODEM_BCR_MISC1 - Miscellaneous control bits for the RX BCR loop.
//   MODEM_BCR_MISC0 - Miscellaneous RX BCR loop controls.
//   MODEM_AFC_GEAR - RX AFC loop gear control.
//   MODEM_AFC_WAIT - RX AFC loop wait time control.
//   MODEM_AFC_GAIN_1 - Sets the gain of the PLL-based AFC acquisition loop, and provides miscellaneous control bits for AFC functionality.
//   MODEM_AFC_GAIN_0 - Sets the gain of the PLL-based AFC acquisition loop, and provides miscellaneous control bits for AFC functionality.
*/
#define RF_MODEM_BCR_NCO_OFFSET_2_12 0x11, 0x20, 0x0C, 0x24, 0x06, 0x24, 0xDD, 0x00, 0x00, 0x02, 0xC0, 0x08, 0x00, 0x12, 0x00, 0x21

/*
// Set properties:           RF_MODEM_AFC_LIMITER_1_3
// Number of properties:     3
// Group ID:                 0x20
// Start ID:                 0x30
// Default values:           0x00, 0x40, 0xA0, 
// Descriptions:
//   MODEM_AFC_LIMITER_1 - Set the AFC limiter value.
//   MODEM_AFC_LIMITER_0 - Set the AFC limiter value.
//   MODEM_AFC_MISC - Specifies miscellaneous AFC control bits.
*/
#define RF_MODEM_AFC_LIMITER_1_3 0x11, 0x20, 0x03, 0x30, 0x01, 0x71, 0xA0

/*
// Set properties:           RF_MODEM_AGC_CONTROL_1
// Number of properties:     1
// Group ID:                 0x20
// Start ID:                 0x35
// Default values:           0xE0, 
// Descriptions:
//   MODEM_AGC_CONTROL - Miscellaneous control bits for the Automatic Gain Control (AGC) function in the RX Chain.
*/
#define RF_MODEM_AGC_CONTROL_1 0x11, 0x20, 0x01, 0x35, 0xE0
														   

/*
// Set properties:           RF_MODEM_AGC_WINDOW_SIZE_12
// Number of properties:     12
// Group ID:                 0x20
// Start ID:                 0x38
// Default values:           0x11, 0x10, 0x10, 0x0B, 0x1C, 0x40, 0x00, 0x00, 0x2B, 0x0C, 0xA4, 0x03, 
// Descriptions:
//   MODEM_AGC_WINDOW_SIZE - Specifies the size of the measurement and settling windows for the AGC algorithm.
//   MODEM_AGC_RFPD_DECAY - Sets the decay time of the RF peak detectors.
//   MODEM_AGC_IFPD_DECAY - Sets the decay time of the IF peak detectors.
//   MODEM_FSK4_GAIN1 - Specifies the gain factor of the secondary branch in 4(G)FSK ISI-suppression.
//   MODEM_FSK4_GAIN0 - Specifies the gain factor of the primary branch in 4(G)FSK ISI-suppression.
//   MODEM_FSK4_TH1 - 16 bit 4(G)FSK slicer threshold.
//   MODEM_FSK4_TH0 - 16 bit 4(G)FSK slicer threshold.
//   MODEM_FSK4_MAP - 4(G)FSK symbol mapping code.
//   MODEM_OOK_PDTC - Configures the attack and decay times of the OOK Peak Detector.
//   MODEM_OOK_BLOPK - Configures the slicing reference level of the OOK Peak Detector.
//   MODEM_OOK_CNT1 - OOK control.
//   MODEM_OOK_MISC - Selects the detector(s) used for demodulation of an OOK signal, or for demodulation of a (G)FSK signal when using the asynchronous demodulator.
*/
#define RF_MODEM_AGC_WINDOW_SIZE_12 0x11, 0x20, 0x0C, 0x38, 0x11, 0x12, 0x12, 0x80, 0x1A, 0x20, 0x00, 0x00, 0x28, 0x0C, 0xA4, 0x23

/*
// Set properties:           RF_MODEM_RAW_CONTROL_5
// Number of properties:     5
// Group ID:                 0x20
// Start ID:                 0x45
// Default values:           0x02, 0x00, 0xA3, 0x02, 0x80, 
// Descriptions:
//   MODEM_RAW_CONTROL - Defines gain and enable controls for raw / nonstandard mode.
//   MODEM_RAW_EYE_1 - 11 bit eye-open detector threshold.
//   MODEM_RAW_EYE_0 - 11 bit eye-open detector threshold.
//   MODEM_ANT_DIV_MODE - Antenna diversity mode settings.
//   MODEM_ANT_DIV_CONTROL - Specifies controls for the Antenna Diversity algorithm.
*/
#define RF_MODEM_RAW_CONTROL_5 0x11, 0x20, 0x05, 0x45, 0x03, 0x00, 0x7D, 0x01, 0x00

/*
// Set properties:           RF_MODEM_RSSI_JUMP_THRESH_4
// Number of properties:     4
// Group ID:                 0x20
// Start ID:                 0x4B
// Default values:           0x0C, 0x01, 0x00, 0x40, 
// Descriptions:
//   MODEM_RSSI_JUMP_THRESH - Configures the RSSI Jump Detection threshold.
//   MODEM_RSSI_CONTROL - Control of the averaging modes and latching time for reporting RSSI value(s).
//   MODEM_RSSI_CONTROL2 - RSSI Jump Detection control.
//   MODEM_RSSI_COMP - RSSI compensation value.
*/
#define RF_MODEM_RSSI_JUMP_THRESH_4 0x11, 0x20, 0x04, 0x4B, 0x06, 0x09, 0x10, 0x40

/*
// Set properties:           RF_MODEM_RAW_SEARCH2_2
// Number of properties:     2
// Group ID:                 0x20
// Start ID:                 0x50
// Default values:           0x00, 0x08, 
// Descriptions:
//   MODEM_RAW_SEARCH2 - Defines and controls the search period length for the Moving Average and Min-Max detectors.
//   MODEM_CLKGEN_BAND - Select PLL Synthesizer output divider ratio as a function of frequency band.
*/
#define RF_MODEM_RAW_SEARCH2_2 0x11, 0x20, 0x02, 0x50, 0x94, 0x0A

/*
// Set properties:           RF_MODEM_SPIKE_DET_2
// Number of properties:     2
// Group ID:                 0x20
// Start ID:                 0x54
// Default values:           0x00, 0x00, 
// Descriptions:
//   MODEM_SPIKE_DET - Configures the threshold for (G)FSK Spike Detection.
//   MODEM_ONE_SHOT_AFC - Configures parameters for th e One Shot AFC function and for BCR timing/acquisition.
*/
#define RF_MODEM_SPIKE_DET_2 0x11, 0x20, 0x02, 0x54, 0x03, 0x07

/*
// Set properties:           RF_MODEM_RSSI_MUTE_1
// Number of properties:     1
// Group ID:                 0x20
// Start ID:                 0x57
// Default values:           0x00, 
// Descriptions:
//   MODEM_RSSI_MUTE - Configures muting of the RSSI to avoid false RSSI interrupts.
*/
#define RF_MODEM_RSSI_MUTE_1 0x11, 0x20, 0x01, 0x57, 0x00

/*
// Set properties:           RF_MODEM_DSA_CTRL1_5
// Number of properties:     5
// Group ID:                 0x20
// Start ID:                 0x5B
// Default values:           0x00, 0x00, 0x00, 0x00, 0x00, 
// Descriptions:
//   MODEM_DSA_CTRL1 - Configures parameters for the Signal Arrival Detection circuit block and algorithm.
//   MODEM_DSA_CTRL2 - Configures parameters for the Signal Arrival Detection circuit block and algorithm.
//   MODEM_DSA_QUAL - Configures parameters for the Eye Opening qualification m ethod of the Signal Arrival Detection algorithm.
//   MODEM_DSA_RSSI - Signal Arrival Detect RSSI Qualifier Config
//   MODEM_DSA_MISC - Miscellaneous detection of signal arrival bits.
*/
#define RF_MODEM_DSA_CTRL1_5 0x11, 0x20, 0x05, 0x5B, 0x40, 0x04, 0x04, 0x78, 0x20

/*
// Set properties:           RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12
// Number of properties:     12
// Group ID:                 0x21
// Start ID:                 0x00
// Default values:           0xFF, 0xBA, 0x0F, 0x51, 0xCF, 0xA9, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01, 
// Descriptions:
//   MODEM_CHFLT_RX1_CHFLT_COE13_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE12_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE11_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE10_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE9_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE8_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE7_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE6_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE5_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE4_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE3_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE2_7_0 - Filter coefficients for the first set of RX filter coefficients.
*/
#define RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12 0x11, 0x21, 0x0C, 0x00, 0x7E, 0x64, 0x1B, 0xBA, 0x58, 0x0B, 0xDD, 0xCE, 0xD6, 0xE6, 0xF6, 0x00

/*
// Set properties:           RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12
// Number of properties:     12
// Group ID:                 0x21
// Start ID:                 0x0C
// Default values:           0xFC, 0xFD, 0x15, 0xFF, 0x00, 0x0F, 0xFF, 0xC4, 0x30, 0x7F, 0xF5, 0xB5, 
// Descriptions:
//   MODEM_CHFLT_RX1_CHFLT_COE1_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE0_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COEM0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COEM1 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COEM2 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COEM3 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE13_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE12_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE11_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE10_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE9_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE8_7_0 - Filter coefficients for the second set of RX filter coefficients.
*/
#define RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12 0x11, 0x21, 0x0C, 0x0C, 0x03, 0x03, 0x15, 0xF0, 0x3F, 0x00, 0x7E, 0x64, 0x1B, 0xBA, 0x58, 0x0B

/*
// Set properties:           RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12
// Number of properties:     12
// Group ID:                 0x21
// Start ID:                 0x18
// Default values:           0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C, 0x03, 0x00, 0x15, 0xFF, 0x00, 0x00, 
// Descriptions:
//   MODEM_CHFLT_RX2_CHFLT_COE7_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE6_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE5_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE4_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE3_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE2_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE1_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE0_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COEM0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COEM1 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COEM2 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COEM3 - Filter coefficients for the second set of RX filter coefficients.
*/
#define RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12 0x11, 0x21, 0x0C, 0x18, 0xDD, 0xCE, 0xD6, 0xE6, 0xF6, 0x00, 0x03, 0x03, 0x15, 0xF0, 0x3F, 0x00

/*
// Set properties:           RF_PA_TC_1
// Number of properties:     1
// Group ID:                 0x22
// Start ID:                 0x03
// Default values:           0x5D, 
// Descriptions:
//   PA_TC - Configuration of PA ramping parameters.
*/
#define RF_PA_TC_1 0x11, 0x22, 0x01, 0x03, 0x1E

/*
// Set properties:           RF_SYNTH_PFDCP_CPFF_7
// Number of properties:     7
// Group ID:                 0x23
// Start ID:                 0x00
// Default values:           0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03, 
// Descriptions:
//   SYNTH_PFDCP_CPFF - Feed forward charge pump current selection.
//   SYNTH_PFDCP_CPINT - Integration charge pump current selection.
//   SYNTH_VCO_KV - Gain scaling factors (Kv) for the VCO tuning varactors on both the integrated-path and feed forward path.
//   SYNTH_LPFILT3 - Value of resistor R2 in feed-forward path of loop filter.
//   SYNTH_LPFILT2 - Value of capacitor C2 in feed-forward path of loop filter.
//   SYNTH_LPFILT1 - Value of capacitors C1 and C3 in feed-forward path of loop filter.
//   SYNTH_LPFILT0 - Bias current of the active amplifier in the feed-forward loop filter.
*/
#define RF_SYNTH_PFDCP_CPFF_7 0x11, 0x23, 0x07, 0x00, 0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03

/*
// Set properties:           RF_FREQ_CONTROL_INTE_8
// Number of properties:     8
// Group ID:                 0x40
// Start ID:                 0x00
// Default values:           0x3C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x20, 0xFF, 
// Descriptions:
//   FREQ_CONTROL_INTE - Frac-N PLL Synthesizer integer divide number.
//   FREQ_CONTROL_FRAC_2 - Frac-N PLL fraction number.
//   FREQ_CONTROL_FRAC_1 - Frac-N PLL fraction number.
//   FREQ_CONTROL_FRAC_0 - Frac-N PLL fraction number.
//   FREQ_CONTROL_CHANNEL_STEP_SIZE_1 - EZ Frequency Programming channel step size.
//   FREQ_CONTROL_CHANNEL_STEP_SIZE_0 - EZ Frequency Programming channel step size.
//   FREQ_CONTROL_W_SIZE - Set window gating period (in number of crystal reference clock cycles) for counting VCO frequency during calibration.
//   FREQ_CONTROL_VCOCNT_RX_ADJ - Adjust target count for VCO calibration in RX mode.
*/
#define RF_FREQ_CONTROL_INTE_8 0x11, 0x40, 0x08, 0x00, 0x37, 0x09, 0x00, 0x00, 0x0C, 0xCD, 0x20, 0xFE

/*
// Command:                  RF_START_RX
// Description:              Switches to RX state and starts reception of a packet.
*/
#define RF_START_RX 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

/*
// Command:                  RF_IRCAL
// Description:              Image rejection calibration.
*/
#define RF_IRCAL 0x17, 0x56, 0x10, 0xCA, 0xF0

/*
// Command:                  RF_IRCAL_1
// Description:              Image rejection calibration.
*/
#define RF_IRCAL_1 0x17, 0x13, 0x10, 0xCA, 0xF0

/*
// Set properties:           RF_GLOBAL_CLK_CFG_1
// Number of properties:     1
// Group ID:                 0x00
// Start ID:                 0x01
// Default values:           0x00, 
// Descriptions:
//   GLOBAL_CLK_CFG - Clock configuration options.
*/
#define RF_GLOBAL_CLK_CFG_1 0x11, 0x00, 0x01, 0x01, 0x00

/*
// Set properties:           RF_GLOBAL_CONFIG_1_1
// Number of properties:     1
// Group ID:                 0x00
// Start ID:                 0x03
// Default values:           0x20, 
// Descriptions:
//   GLOBAL_CONFIG - Global configuration settings.
*/
#define RF_GLOBAL_CONFIG_1_1 0x11, 0x00, 0x01, 0x03, 0x20

/*
// Set properties:           RF_INT_CTL_ENABLE_1
// Number of properties:     1
// Group ID:                 0x01
// Start ID:                 0x00
// Default values:           0x04, 
// Descriptions:
//   INT_CTL_ENABLE - This property provides for global enabling of the three interrupt groups (Chip, Modem and Packet Handler) in order to generate HW interrupts at the NIRQ pin.
*/
#define RF_INT_CTL_ENABLE_1 0x11, 0x01, 0x01, 0x00, 0x00

/*
// Set properties:           RF_FRR_CTL_A_MODE_4
// Number of properties:     4
// Group ID:                 0x02
// Start ID:                 0x00
// Default values:           0x01, 0x02, 0x09, 0x00, 
// Descriptions:
//   FRR_CTL_A_MODE - Fast Response Register A Configuration.
//   FRR_CTL_B_MODE - Fast Response Register B Configuration.
//   FRR_CTL_C_MODE - Fast Response Register C Configuration.
//   FRR_CTL_D_MODE - Fast Response Register D Configuration.
*/
#define RF_FRR_CTL_A_MODE_4 0x11, 0x02, 0x04, 0x00, 0x0A, 0x07, 0x00, 0x00

/*
// Set properties:           RF_PREAMBLE_CONFIG_STD_1_1
// Number of properties:     1
// Group ID:                 0x10
// Start ID:                 0x01
// Default values:           0x14, 
// Descriptions:
//   PREAMBLE_CONFIG_STD_1 - Configuration of reception of a packet with a Standard Preamble pattern.
*/
#define RF_PREAMBLE_CONFIG_STD_1_1 0x11, 0x10, 0x01, 0x01, 0x14

/*
// Set properties:           RF_PKT_CONFIG1_1
// Number of properties:     1
// Group ID:                 0x12
// Start ID:                 0x06
// Default values:           0x00, 
// Descriptions:
//   PKT_CONFIG1 - General configuration bits for transmission or reception of a packet.
*/
#define RF_PKT_CONFIG1_1 0x11, 0x12, 0x01, 0x06, 0x40

/*
// Set properties:           RF_MODEM_MOD_TYPE_12_1
// Number of properties:     12
// Group ID:                 0x20
// Start ID:                 0x00
// Default values:           0x02, 0x80, 0x07, 0x0F, 0x42, 0x40, 0x01, 0xC9, 0xC3, 0x80, 0x00, 0x06, 
// Descriptions:
//   MODEM_MOD_TYPE - Selects the type of modulation. In TX mode, additionally selects the source of the modulation.
//   MODEM_MAP_CONTROL - Controls polarity and mapping of transmit and receive bits.
//   MODEM_DSM_CTRL - Miscellaneous control bits for the Delta-Sigma Modulator (DSM) in the PLL Synthesizer.
//   MODEM_DATA_RATE_2 - Unsigned 24-bit value used to determine the TX data rate
//   MODEM_DATA_RATE_1 - Unsigned 24-bit value used to determine the TX data rate
//   MODEM_DATA_RATE_0 - Unsigned 24-bit value used to determine the TX data rate
//   MODEM_TX_NCO_MODE_3 - TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
//   MODEM_TX_NCO_MODE_2 - TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
//   MODEM_TX_NCO_MODE_1 - TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
//   MODEM_TX_NCO_MODE_0 - TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
//   MODEM_FREQ_DEV_2 - 17-bit unsigned TX frequency deviation word.
//   MODEM_FREQ_DEV_1 - 17-bit unsigned TX frequency deviation word.
*/
#define RF_MODEM_MOD_TYPE_12_1 0x11, 0x20, 0x0C, 0x00, 0x09, 0x00, 0x07, 0x00, 0xBB, 0x80, 0x01, 0xE8, 0x48, 0x00, 0x00, 0x00

/*
// Set properties:           RF_MODEM_FREQ_DEV_0_1_1
// Number of properties:     1
// Group ID:                 0x20
// Start ID:                 0x0C
// Default values:           0xD3, 
// Descriptions:
//   MODEM_FREQ_DEV_0 - 17-bit unsigned TX frequency deviation word.
*/
#define RF_MODEM_FREQ_DEV_0_1_1 0x11, 0x20, 0x01, 0x0C, 0x00

/*
// Set properties:           RF_MODEM_TX_RAMP_DELAY_12_1
// Number of properties:     12
// Group ID:                 0x20
// Start ID:                 0x18
// Default values:           0x01, 0x00, 0x08, 0x03, 0xC0, 0x00, 0x10, 0x20, 0x00, 0x00, 0x00, 0x4B, 
// Descriptions:
//   MODEM_TX_RAMP_DELAY - TX ramp-down delay setting.
//   MODEM_MDM_CTRL - MDM control.
//   MODEM_IF_CONTROL - Selects Fixed-IF, Scaled-IF, or Zero-IF mode of RX Modem operation.
//   MODEM_IF_FREQ_2 - the IF frequency setting (an 18-bit signed number).
//   MODEM_IF_FREQ_1 - the IF frequency setting (an 18-bit signed number).
//   MODEM_IF_FREQ_0 - the IF frequency setting (an 18-bit signed number).
//   MODEM_DECIMATION_CFG1 - Specifies three decimator ratios for the Cascaded Integrator Comb (CIC) filter.
//   MODEM_DECIMATION_CFG0 - Specifies miscellaneous parameters and decimator ratios for the Cascaded Integrator Comb (CIC) filter.
//   MODEM_DECIMATION_CFG2 - Specifies miscellaneous decimator filter selections.
//   MODEM_IFPKD_THRESHOLDS - 
//   MODEM_BCR_OSR_1 - RX BCR/Slicer oversampling rate (12-bit unsigned number).
//   MODEM_BCR_OSR_0 - RX BCR/Slicer oversampling rate (12-bit unsigned number).
*/
#define RF_MODEM_TX_RAMP_DELAY_12_1 0x11, 0x20, 0x0C, 0x18, 0x00, 0x80, 0x08, 0x03, 0x80, 0x00, 0x34, 0x20, 0x0C, 0xE8, 0x00, 0x68

/*
// Set properties:           RF_MODEM_BCR_NCO_OFFSET_2_12_1
// Number of properties:     12
// Group ID:                 0x20
// Start ID:                 0x24
// Default values:           0x06, 0xD3, 0xA0, 0x06, 0xD3, 0x02, 0xC0, 0x00, 0x00, 0x23, 0x83, 0x69, 
// Descriptions:
//   MODEM_BCR_NCO_OFFSET_2 - RX BCR NCO offset value (an unsigned 22-bit number).
//   MODEM_BCR_NCO_OFFSET_1 - RX BCR NCO offset value (an unsigned 22-bit number).
//   MODEM_BCR_NCO_OFFSET_0 - RX BCR NCO offset value (an unsigned 22-bit number).
//   MODEM_BCR_GAIN_1 - The unsigned 11-bit RX BCR loop gain value.
//   MODEM_BCR_GAIN_0 - The unsigned 11-bit RX BCR loop gain value.
//   MODEM_BCR_GEAR - RX BCR loop gear control.
//   MODEM_BCR_MISC1 - Miscellaneous control bits for the RX BCR loop.
//   MODEM_BCR_MISC0 - Miscellaneous RX BCR loop controls.
//   MODEM_AFC_GEAR - RX AFC loop gear control.
//   MODEM_AFC_WAIT - RX AFC loop wait time control.
//   MODEM_AFC_GAIN_1 - Sets the gain of the PLL-based AFC acquisition loop, and provides miscellaneous control bits for AFC functionality.
//   MODEM_AFC_GAIN_0 - Sets the gain of the PLL-based AFC acquisition loop, and provides miscellaneous control bits for AFC functionality.
*/
#define RF_MODEM_BCR_NCO_OFFSET_2_12_1 0x11, 0x20, 0x0C, 0x24, 0x04, 0xEA, 0x4B, 0x02, 0x76, 0x00, 0xC2, 0x00, 0x54, 0x23, 0x00, 0x27

/*
// Set properties:           RF_MODEM_AFC_LIMITER_1_3_1
// Number of properties:     3
// Group ID:                 0x20
// Start ID:                 0x30
// Default values:           0x00, 0x40, 0xA0, 
// Descriptions:
//   MODEM_AFC_LIMITER_1 - Set the AFC limiter value.
//   MODEM_AFC_LIMITER_0 - Set the AFC limiter value.
//   MODEM_AFC_MISC - Specifies miscellaneous AFC control bits.
*/
#define RF_MODEM_AFC_LIMITER_1_3_1 0x11, 0x20, 0x03, 0x30, 0x07, 0x75, 0x80

/*
// Set properties:           RF_MODEM_AGC_CONTROL_1_1
// Number of properties:     1
// Group ID:                 0x20
// Start ID:                 0x35
// Default values:           0xE0, 
// Descriptions:
//   MODEM_AGC_CONTROL - Miscellaneous control bits for the Automatic Gain Control (AGC) function in the RX Chain.
*/
#define RF_MODEM_AGC_CONTROL_1_1 0x11, 0x20, 0x01, 0x35, 0x60

/*
// Set properties:           RF_MODEM_AGC_WINDOW_SIZE_12_1
// Number of properties:     12
// Group ID:                 0x20
// Start ID:                 0x38
// Default values:           0x11, 0x10, 0x10, 0x0B, 0x1C, 0x40, 0x00, 0x00, 0x2B, 0x0C, 0xA4, 0x03, 
// Descriptions:
//   MODEM_AGC_WINDOW_SIZE - Specifies the size of the measurement and settling windows for the AGC algorithm.
//   MODEM_AGC_RFPD_DECAY - Sets the decay time of the RF peak detectors.
//   MODEM_AGC_IFPD_DECAY - Sets the decay time of the IF peak detectors.
//   MODEM_FSK4_GAIN1 - Specifies the gain factor of the secondary branch in 4(G)FSK ISI-suppression.
//   MODEM_FSK4_GAIN0 - Specifies the gain factor of the primary branch in 4(G)FSK ISI-suppression.
//   MODEM_FSK4_TH1 - 16 bit 4(G)FSK slicer threshold.
//   MODEM_FSK4_TH0 - 16 bit 4(G)FSK slicer threshold.
//   MODEM_FSK4_MAP - 4(G)FSK symbol mapping code.
//   MODEM_OOK_PDTC - Configures the attack and decay times of the OOK Peak Detector.
//   MODEM_OOK_BLOPK - Configures the slicing reference level of the OOK Peak Detector.
//   MODEM_OOK_CNT1 - OOK control.
//   MODEM_OOK_MISC - Selects the detector(s) used for demodulation of an OOK signal, or for demodulation of a (G)FSK signal when using the asynchronous demodulator.
*/

												 
#define RF_MODEM_AGC_WINDOW_SIZE_12_1 0x11, 0x20, 0x0C, 0x38, 0x11, 0x17, 0x17, 0x80, 0x02, 0xFF, 0xFF, 0x00, 0x28, 0x0C, 0x84, 0x21
																																	

/*
// Set properties:           RF_MODEM_RAW_CONTROL_10
// Number of properties:     10
// Group ID:                 0x20
// Start ID:                 0x45
// Default values:           0x02, 0x00, 0xA3, 0x02, 0x80, 0xFF, 0x0C, 0x01, 0x00, 0x40, 
// Descriptions:
//   MODEM_RAW_CONTROL - Defines gain and enable controls for raw / nonstandard mode.
//   MODEM_RAW_EYE_1 - 11 bit eye-open detector threshold.
//   MODEM_RAW_EYE_0 - 11 bit eye-open detector threshold.
//   MODEM_ANT_DIV_MODE - Antenna diversity mode settings.
//   MODEM_ANT_DIV_CONTROL - Specifies controls for the Antenna Diversity algorithm.
//   MODEM_RSSI_THRESH - Configures the RSSI threshold.
//   MODEM_RSSI_JUMP_THRESH - Configures the RSSI Jump Detection threshold.
//   MODEM_RSSI_CONTROL - Control of the averaging modes and latching time for reporting RSSI value(s).
//   MODEM_RSSI_CONTROL2 - RSSI Jump Detection control.
//   MODEM_RSSI_COMP - RSSI compensation value.
*/
#define RF_MODEM_RAW_CONTROL_10 0x11, 0x20, 0x0A, 0x45, 0x8C, 0x07, 0xFF, 0x01, 0x00, 0x70, 0x06, 0x06, 0x18, 0x40

/*
// Set properties:           RF_MODEM_RAW_SEARCH2_2_1
// Number of properties:     2
// Group ID:                 0x20
// Start ID:                 0x50
// Default values:           0x00, 0x08, 
// Descriptions:
//   MODEM_RAW_SEARCH2 - Defines and controls the search period length for the Moving Average and Min-Max detectors.
//   MODEM_CLKGEN_BAND - Select PLL Synthesizer output divider ratio as a function of frequency band.
*/
#define RF_MODEM_RAW_SEARCH2_2_1 0x11, 0x20, 0x02, 0x50, 0x94, 0x0A

/*
// Set properties:           RF_MODEM_SPIKE_DET_2_1
// Number of properties:     2
// Group ID:                 0x20
// Start ID:                 0x54
// Default values:           0x00, 0x00, 
// Descriptions:
//   MODEM_SPIKE_DET - Configures the threshold for (G)FSK Spike Detection.
//   MODEM_ONE_SHOT_AFC - Configures parameters for th e One Shot AFC function and for BCR timing/acquisition.
*/
#define RF_MODEM_SPIKE_DET_2_1 0x11, 0x20, 0x02, 0x54, 0x14, 0x07

/*
// Set properties:           RF_MODEM_RSSI_MUTE_1_1
// Number of properties:     1
// Group ID:                 0x20
// Start ID:                 0x57
// Default values:           0x00, 
// Descriptions:
//   MODEM_RSSI_MUTE - Configures muting of the RSSI to avoid false RSSI interrupts.
*/
#define RF_MODEM_RSSI_MUTE_1_1 0x11, 0x20, 0x01, 0x57, 0x00

/*
// Set properties:           RF_MODEM_DSA_CTRL1_5_1
// Number of properties:     5
// Group ID:                 0x20
// Start ID:                 0x5B
// Default values:           0x00, 0x00, 0x00, 0x00, 0x00, 
// Descriptions:
//   MODEM_DSA_CTRL1 - Configures parameters for the Signal Arrival Detection circuit block and algorithm.
//   MODEM_DSA_CTRL2 - Configures parameters for the Signal Arrival Detection circuit block and algorithm.
//   MODEM_DSA_QUAL - Configures parameters for the Eye Opening qualification m ethod of the Signal Arrival Detection algorithm.
//   MODEM_DSA_RSSI - Signal Arrival Detect RSSI Qualifier Config
//   MODEM_DSA_MISC - Miscellaneous detection of signal arrival bits.
*/
#define RF_MODEM_DSA_CTRL1_5_1 0x11, 0x20, 0x05, 0x5B, 0x40, 0x04, 0x32, 0x78, 0x20

/*
// Set properties:           RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12_1
// Number of properties:     12
// Group ID:                 0x21
// Start ID:                 0x00
// Default values:           0xFF, 0xBA, 0x0F, 0x51, 0xCF, 0xA9, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01, 
// Descriptions:
//   MODEM_CHFLT_RX1_CHFLT_COE13_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE12_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE11_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE10_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE9_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE8_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE7_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE6_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE5_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE4_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE3_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE2_7_0 - Filter coefficients for the first set of RX filter coefficients.
*/
#define RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12_1 0x11, 0x21, 0x0C, 0x00, 0xFF, 0xC4, 0x30, 0x7F, 0xF5, 0xB5, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C

/*
// Set properties:           RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12_1
// Number of properties:     12
// Group ID:                 0x21
// Start ID:                 0x0C
// Default values:           0xFC, 0xFD, 0x15, 0xFF, 0x00, 0x0F, 0xFF, 0xC4, 0x30, 0x7F, 0xF5, 0xB5, 
// Descriptions:
//   MODEM_CHFLT_RX1_CHFLT_COE1_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COE0_7_0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COEM0 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COEM1 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COEM2 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX1_CHFLT_COEM3 - Filter coefficients for the first set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE13_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE12_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE11_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE10_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE9_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE8_7_0 - Filter coefficients for the second set of RX filter coefficients.
*/
#define RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12_1 0x11, 0x21, 0x0C, 0x0C, 0x03, 0x00, 0x15, 0xFF, 0x00, 0x00, 0xFF, 0xC4, 0x30, 0x7F, 0xF5, 0xB5

/*
// Set properties:           RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12_1
// Number of properties:     12
// Group ID:                 0x21
// Start ID:                 0x18
// Default values:           0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C, 0x03, 0x00, 0x15, 0xFF, 0x00, 0x00, 
// Descriptions:
//   MODEM_CHFLT_RX2_CHFLT_COE7_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE6_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE5_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE4_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE3_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE2_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE1_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COE0_7_0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COEM0 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COEM1 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COEM2 - Filter coefficients for the second set of RX filter coefficients.
//   MODEM_CHFLT_RX2_CHFLT_COEM3 - Filter coefficients for the second set of RX filter coefficients.
*/
#define RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12_1 0x11, 0x21, 0x0C, 0x18, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C, 0x03, 0x00, 0x15, 0xFF, 0x00, 0x00

/*
// Set properties:           RF_PA_TC_1_1
// Number of properties:     1
// Group ID:                 0x22
// Start ID:                 0x03
// Default values:           0x5D, 
// Descriptions:
//   PA_TC - Configuration of PA ramping parameters.
*/
#define RF_PA_TC_1_1 0x11, 0x22, 0x01, 0x03, 0x5E

/*
// Set properties:           RF_SYNTH_PFDCP_CPFF_7_1
// Number of properties:     7
// Group ID:                 0x23
// Start ID:                 0x00
// Default values:           0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03, 
// Descriptions:
//   SYNTH_PFDCP_CPFF - Feed forward charge pump current selection.
//   SYNTH_PFDCP_CPINT - Integration charge pump current selection.
//   SYNTH_VCO_KV - Gain scaling factors (Kv) for the VCO tuning varactors on both the integrated-path and feed forward path.
//   SYNTH_LPFILT3 - Value of resistor R2 in feed-forward path of loop filter.
//   SYNTH_LPFILT2 - Value of capacitor C2 in feed-forward path of loop filter.
//   SYNTH_LPFILT1 - Value of capacitors C1 and C3 in feed-forward path of loop filter.
//   SYNTH_LPFILT0 - Bias current of the active amplifier in the feed-forward loop filter.
*/
#define RF_SYNTH_PFDCP_CPFF_7_1 0x11, 0x23, 0x07, 0x00, 0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03

/*
// Set properties:           RF_FREQ_CONTROL_INTE_8_1
// Number of properties:     8
// Group ID:                 0x40
// Start ID:                 0x00
// Default values:           0x3C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x20, 0xFF, 
// Descriptions:
//   FREQ_CONTROL_INTE - Frac-N PLL Synthesizer integer divide number.
//   FREQ_CONTROL_FRAC_2 - Frac-N PLL fraction number.
//   FREQ_CONTROL_FRAC_1 - Frac-N PLL fraction number.
//   FREQ_CONTROL_FRAC_0 - Frac-N PLL fraction number.
//   FREQ_CONTROL_CHANNEL_STEP_SIZE_1 - EZ Frequency Programming channel step size.
//   FREQ_CONTROL_CHANNEL_STEP_SIZE_0 - EZ Frequency Programming channel step size.
//   FREQ_CONTROL_W_SIZE - Set window gating period (in number of crystal reference clock cycles) for counting VCO frequency during calibration.
//   FREQ_CONTROL_VCOCNT_RX_ADJ - Adjust target count for VCO calibration in RX mode.
*/
#define RF_FREQ_CONTROL_INTE_8_1 0x11, 0x40, 0x08, 0x00, 0x35, 0x09, 0xEB, 0x85, 0x0C, 0xCD, 0x20, 0xFE


// AUTOMATICALLY GENERATED CODE! 
// DO NOT EDIT/MODIFY BELOW THIS LINE!
// --------------------------------------------

#ifndef FIRMWARE_LOAD_COMPILE
#define RADIO_CONFIGURATION_DATA_ARRAY { \
        SI446X_PATCH_CMDS, \
        0x07, RF_POWER_UP, \
        0x08, RF_GPIO_PIN_CFG, \
        0x05, RF_GLOBAL_XO_TUNE_1, \
        0x05, RF_GLOBAL_CONFIG_1, \
        0x05, RF_PREAMBLE_CONFIG_1, \
        0x10, RF_MODEM_MOD_TYPE_12, \
        0x05, RF_MODEM_FREQ_DEV_0_1, \
        0x10, RF_MODEM_TX_RAMP_DELAY_12, \
        0x10, RF_MODEM_BCR_NCO_OFFSET_2_12, \
        0x07, RF_MODEM_AFC_LIMITER_1_3, \
        0x05, RF_MODEM_AGC_CONTROL_1, \
        0x10, RF_MODEM_AGC_WINDOW_SIZE_12, \
        0x09, RF_MODEM_RAW_CONTROL_5, \
        0x08, RF_MODEM_RSSI_JUMP_THRESH_4, \
        0x06, RF_MODEM_RAW_SEARCH2_2, \
        0x06, RF_MODEM_SPIKE_DET_2, \
        0x05, RF_MODEM_RSSI_MUTE_1, \
        0x09, RF_MODEM_DSA_CTRL1_5, \
        0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12, \
        0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12, \
        0x10, RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12, \
        0x05, RF_PA_TC_1, \
        0x0B, RF_SYNTH_PFDCP_CPFF_7, \
        0x0C, RF_FREQ_CONTROL_INTE_8, \
        0x08, RF_START_RX, \
        0x05, RF_IRCAL, \
        0x05, RF_IRCAL_1, \
        0x05, RF_GLOBAL_CLK_CFG_1, \
        0x05, RF_GLOBAL_CONFIG_1_1, \
        0x05, RF_INT_CTL_ENABLE_1, \
        0x08, RF_FRR_CTL_A_MODE_4, \
        0x05, RF_PREAMBLE_CONFIG_STD_1_1, \
        0x05, RF_PKT_CONFIG1_1, \
        0x10, RF_MODEM_MOD_TYPE_12_1, \
        0x05, RF_MODEM_FREQ_DEV_0_1_1, \
        0x10, RF_MODEM_TX_RAMP_DELAY_12_1, \
        0x10, RF_MODEM_BCR_NCO_OFFSET_2_12_1, \
        0x07, RF_MODEM_AFC_LIMITER_1_3_1, \
        0x05, RF_MODEM_AGC_CONTROL_1_1, \
        0x10, RF_MODEM_AGC_WINDOW_SIZE_12_1, \
        0x0E, RF_MODEM_RAW_CONTROL_10, \
        0x06, RF_MODEM_RAW_SEARCH2_2_1, \
        0x06, RF_MODEM_SPIKE_DET_2_1, \
        0x05, RF_MODEM_RSSI_MUTE_1_1, \
        0x09, RF_MODEM_DSA_CTRL1_5_1, \
        0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE13_7_0_12_1, \
        0x10, RF_MODEM_CHFLT_RX1_CHFLT_COE1_7_0_12_1, \
        0x10, RF_MODEM_CHFLT_RX2_CHFLT_COE7_7_0_12_1, \
        0x05, RF_PA_TC_1_1, \
        0x0B, RF_SYNTH_PFDCP_CPFF_7_1, \
        0x0C, RF_FREQ_CONTROL_INTE_8_1, \
        0x00 \
 }
#else
#define RADIO_CONFIGURATION_DATA_ARRAY { 0 }
#endif

// DEFAULT VALUES FOR CONFIGURATION PARAMETERS
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ_DEFAULT                     32000000L
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER_DEFAULT                    0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH_DEFAULT               0x10
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP_DEFAULT        0x01
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET_DEFAULT       0x1000

#define RADIO_CONFIGURATION_DATA_RADIO_PATCH_INCLUDED                      0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PATCH_SIZE                          0x00
#define RADIO_CONFIGURATION_DATA_RADIO_PATCH                               {  }

#ifndef RADIO_CONFIGURATION_DATA_ARRAY
#error "This property must be defined!"
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ
#define RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ          RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ_DEFAULT 
#endif

#ifndef RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER
#define RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER         RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER_DEFAULT 
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH
#define RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH    RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH_DEFAULT 
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP
#define RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP   RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP_DEFAULT 
#endif

#ifndef RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET
#define RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET  RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET_DEFAULT 
#endif

#define RADIO_CONFIGURATION_DATA { \
                            Radio_Configuration_Data_Array,                            \
                            RADIO_CONFIGURATION_DATA_CHANNEL_NUMBER,                   \
                            RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH,              \
                            RADIO_CONFIGURATION_DATA_RADIO_STATE_AFTER_POWER_UP,       \
                            RADIO_CONFIGURATION_DATA_RADIO_DELAY_CNT_AFTER_RESET       \
                            }


const uint8_t furi_hal_subghz_preset_ook_650khz_async_for_freq_regs[] = RADIO_CONFIGURATION_DATA_ARRAY;