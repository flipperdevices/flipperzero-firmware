#pragma once

#include <stdbool.h>
#include <stdint.h>

//https://www.silabs.com/content/usergenerated/asi/cloud/attachments/siliconlabs/en/community/wireless/bluetooth/forum/jcr:content/content/primary/qna/what_27s_the_command-5ZTk/for_all_info_refert-EAz6/Si446X%20API%20DESCRIPTIONS.pdf
//https://www.silabs.com/documents/public/application-notes/EZRadioPRO_REVC2_API.zip

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */
//#define SI446X_QUARTZ 30000000
//#define SI446X_QUARTZ 26000000
#define SI446X_QUARTZ 32000000
#define SI446X_CTS_OK 0xFF
#define SI446X_TIMEOUT 500
#define SI446X_SET_MAX_PA 0x1F
#define SI446X_TIMEOUT_NSDN 10000

/*Radio API Commands*/
#define SI446X_CMD_POWER_UP \
    0x02 /* Power-up device and mode selection. Modes include operational function. */
#define SI446X_CMD_NOP 0x00 /* No operation command. */
#define SI446X_CMD_PART_INFO 0x01 /* Reports basic information about the device. */
#define SI446X_CMD_FUNC_INFO 0x10 /* Returns the Function revision information of the device. */
#define SI446X_CMD_SET_PROPERTY 0x11 /* Sets the value of a property. */
#define SI446X_CMD_GET_PROPERTY 0x12 /* Retrieve a property's value. */
#define SI446X_CMD_GPIO_PIN_CFG 0x13 /* Configures the GPIO pins. */
#define SI446X_CMD_GET_ADC_READING 0x14 /* Retrieve the results of possible ADC conversions. */
#define SI446X_CMD_FIFO_INFO \
    0x15 /* Provides access to transmit and receive fifo counts and reset. */
#define SI446X_CMD_PACKET_INFO \
    0x16 /* Returns information about the last packet received and optionally overrides field length. */
#define SI446X_CMD_IRCAL 0x17 /* Calibrate Image Rejection. */
#define SI446X_CMD_PROTOCOL_CFG 0x18 /* Sets the chip up for specified protocol. */
#define SI446X_CMD_IRCAL_MANUAL 0x1a /* Image rejection calibration. */
#define SI446X_CMD_GET_INT_STATUS 0x20 /* Returns the interrupt status byte. */
#define SI446X_CMD_GET_PH_STATUS 0x21 /* Returns the packet handler status. */
#define SI446X_CMD_GET_MODEM_STATUS 0x22 /* Returns the modem status byte. */
#define SI446X_CMD_GET_CHIP_STATUS 0x23 /* Returns the chip status. */
#define SI446X_CMD_START_TX 0x31 /* Switches to TX state and starts packet transmission. */
#define SI446X_CMD_START_RX 0x32 /* Switches to RX state. */
#define SI446X_CMD_REQUEST_DEVICE_STATE 0x33 /* Request current device state. */
#define SI446X_CMD_CHANGE_STATE 0x34 /* Manually switch the chip to a desired operating state. */
#define SI446X_CMD_RX_HOP 0x36 /* Manually hop to a new frequency while in RX mode. */
#define SI446X_CMD_TX_HOP 0x37 /* Hop to a new frequency while in TX. */
#define SI446X_CMD_ \
    OFFLINE_RECAL 0x38 /* Onli si4468 Switches to high temp or low temp settings and recalibrate blocks. */
#define SI446X_CMD_READ_CMD_BUFF 0x44 /* Used to read CTS and the command response. */
#define SI446X_CMD_FRR_A_READ 0x50 /* Reads the fast response registers starting with A. */
#define SI446X_CMD_FRR_B_READ 0x51 /* Reads the fast response registers starting with B. */
#define SI446X_CMD_FRR_C_READ 0x53 /* Reads the fast response registers starting with C. */
#define SI446X_CMD_FRR_D_READ 0x57 /* Reads the fast response registers starting with D. */
#define SI446X_CMD_WRITE_TX_FIFO 0x66 /* Writes the TX FIFO. */
#define SI446X_CMD_READ_RX_FIFO 0x77 /* Reads the RX FIFO. */

#define SI446X_INT_CTL_CHIP_LOW_BATT_EN 1
#define SI446X_INT_CTL_CHIP_WUT_EN 0

/* ADC conf*/
typedef enum {
    SI446X_ADC_CONV_TEMP = 16,
    SI446X_ADC_CONV_BATT = 8,
    SI446X_ADC_CONV_GPIO = 4
} SI446X_ADC_cfg_t;

typedef enum {
    SI446X_ADC_RANGE_0P8 = 0,
    SI446X_ADC_RANGE_1P6 = 4,
    SI446X_ADC_RANGE_3P2 = 5,
    SI446X_ADC_RANGE_2P4 = 8,
    SI446X_ADC_RANGE_3P6 = 9
} SI446X_ADC_Range_t;

/* GPIO */
typedef enum {
    SI446X_GPIO0 = 0, /* GPIO 1 */
    SI446X_GPIO1 = 1, /* GPIO 2 */
    SI446X_GPIO2 = 2, /* GPIO 3 */
    SI446X_GPIO3 = 3, /* GPIO 4 */
    SI446X_NIRQ = 4, /* NIRQ */
    SI446X_SDO = 5 /* SDO */
} SI446X_GPIO_t;

/* GPIO pin modes (see the Si446x API docs for what they all mean) */

typedef enum {
    SI446X_GPIO_MODE_DONOTHING = 0x00,
    SI446X_GPIO_MODE_TRISTATE = 0x01,
    SI446X_GPIO_MODE_DRIVE0 = 0x02,
    SI446X_GPIO_MODE_DRIVE1 = 0x03,
    SI446X_GPIO_MODE_INPUT = 0x04,
    SI446X_GPIO_MODE_32K_CLK = 0x05,
    SI446X_GPIO_MODE_BOOT_CLK = 0x06,
    SI446X_GPIO_MODE_DIV_CLK = 0x07,
    SI446X_GPIO_MODE_CTS = 0x08,
    SI446X_GPIO_MODE_INV_CTS = 0x09,
    SI446X_GPIO_MODE_CMD_OVERLAP = 0x0A,
    SI446X_GPIO_MODE_SDO = 0x0B,
    SI446X_GPIO_MODE_POR = 0x0C,
    SI446X_GPIO_MODE_CAL_WUT = 0x0D,
    SI446X_GPIO_MODE_WUT = 0x0E,
    SI446X_GPIO_MODE_EN_PA = 0x0F,
    SI446X_GPIO_MODE_TX_DATA_CLK = 0x10,
    SI446X_GPIO_MODE_RX_DATA_CLK = 0x11,
    SI446X_GPIO_MODE_EN_LNA = 0x12,
    SI446X_GPIO_MODE_TX_DATA = 0x13,
    SI446X_GPIO_MODE_RX_DATA = 0x14,
    SI446X_GPIO_MODE_RX_RAW_DATA = 0x15,
    SI446X_GPIO_MODE_ANTENNA_1_SW = 0x16,
    SI446X_GPIO_MODE_ANTENNA_2_SW = 0x17,
    SI446X_GPIO_MODE_VALID_PREAMBLE = 0x18,
    SI446X_GPIO_MODE_INVALID_PREAMBLE = 0x19,
    SI446X_GPIO_MODE_SYNC_WORD_DETECT = 0x1A,
    SI446X_GPIO_MODE_CCA = 0x1B,
    SI446X_GPIO_MODE_IN_SLEEP = 0x1C,
    SI446X_GPIO_MODE_PKT_TRACE = 0x1D,
    SI446X_GPIO_MODE_TX_RX_DATA_CLK = 0x1F,
    SI446X_GPIO_MODE_TX_STATE = 0x20,
    SI446X_GPIO_MODE_RX_STATE = 0x21,
    SI446X_GPIO_MODE_RX_FIFO_FULL = 0x22,
    SI446X_GPIO_MODE_TX_FIFO_EMPTY = 0x23,
    SI446X_GPIO_MODE_LOW_BATT = 0x24,
    SI446X_GPIO_MODE_CCA_LATCH = 0x25,
    SI446X_GPIO_MODE_HOPPED = 0x26,
    SI446X_GPIO_MODE_HOP_TABLE_WRAP = 0x27
} SI446X_GPIO_Mode_t;

/* NIRQ pin modes (see the Si446x API docs for what they all mean) */
typedef enum {
    SI446X_NIRQ_MODE_DONOTHING = 0x00,
    SI446X_NIRQ_MODE_TRISTATE = 0x01,
    SI446X_NIRQ_MODE_DRIVE0 = 0x02,
    SI446X_NIRQ_MODE_DRIVE1 = 0x03,
    SI446X_NIRQ_MODE_INPUT = 0x04,
    //	SI446X_NIRQ_MODE_32K_CLK	= 0x05,
    //	SI446X_NIRQ_MODE_BOOT_CLK	= 0x06,
    SI446X_NIRQ_MODE_DIV_CLK = 0x07,
    SI446X_NIRQ_MODE_CTS = 0x08,
    //	SI446X_NIRQ_MODE_INV_CTS	= 0x09,
    //	SI446X_NIRQ_MODE_CMD_OVERLAP	= 0x0A,
    SI446X_NIRQ_MODE_SDO = 0x0B,
    SI446X_NIRQ_MODE_POR = 0x0C,
    //	SI446X_NIRQ_MODE_CAL_WUT	= 0x0D,
    //	SI446X_NIRQ_MODE_WUT		= 0x0E,
    SI446X_NIRQ_MODE_EN_PA = 0x0F,
    SI446X_NIRQ_MODE_TX_DATA_CLK = 0x10,
    SI446X_NIRQ_MODE_RX_DATA_CLK = 0x11,
    SI446X_NIRQ_MODE_EN_LNA = 0x12,
    SI446X_NIRQ_MODE_TX_DATA = 0x13,
    SI446X_NIRQ_MODE_RX_DATA = 0x14,
    SI446X_NIRQ_MODE_RX_RAW_DATA = 0x15,
    SI446X_NIRQ_MODE_ANTENNA_1_SW = 0x16,
    SI446X_NIRQ_MODE_ANTENNA_2_SW = 0x17,
    SI446X_NIRQ_MODE_VALID_PREAMBLE = 0x18,
    SI446X_NIRQ_MODE_INVALID_PREAMBLE = 0x19,
    SI446X_NIRQ_MODE_SYNC_WORD_DETECT = 0x1A,
    SI446X_NIRQ_MODE_CCA = 0x1B,
    //	SI446X_NIRQ_MODE_IN_SLEEP		= 0x1C,
    //	SI446X_NIRQ_MODE_PKT_TRACE		= 0x1D,
    // Nothing for 0x1E (30)
    //	SI446X_NIRQ_MODE_TX_RX_DATA_CLK	= 0x1F,
    //	SI446X_NIRQ_MODE_TX_STATE		= 0x20,
    //	SI446X_NIRQ_MODE_RX_STATE		= 0x21,
    //	SI446X_NIRQ_MODE_RX_FIFO_FULL	= 0x22,
    //	SI446X_NIRQ_MODE_TX_FIFO_EMPTY	= 0x23,
    //	SI446X_NIRQ_MODE_LOW_BATT		= 0x24,
    //	SI446X_NIRQ_MODE_CCA_LATCH		= 0x25,
    //	SI446X_NIRQ_MODE_HOPPED			= 0x26,
    SI446X_NIRQ_MODE_NIRQ = 0x27
} SI446X_NIRQ_Mode_t;

/* SDO pin modes (see the Si446x API docs for what they all mean) */
typedef enum {
    SI446X_SDO_MODE_DONOTHING = 0x00,
    SI446X_SDO_MODE_TRISTATE = 0x01,
    SI446X_SDO_MODE_DRIVE0 = 0x02,
    SI446X_SDO_MODE_DRIVE1 = 0x03,
    SI446X_SDO_MODE_INPUT = 0x04,
    SI446X_SDO_MODE_32K_CLK = 0x05,
    //	SI446X_SDO_MODE_BOOT_CLK	= 0x06,
    SI446X_SDO_MODE_DIV_CLK = 0x07,
    SI446X_SDO_MODE_CTS = 0x08,
    //	SI446X_SDO_MODE_INV_CTS	= 0x09,
    //	SI446X_SDO_MODE_CMD_OVERLAP	= 0x0A,
    SI446X_SDO_MODE_SDO = 0x0B,
    SI446X_SDO_MODE_POR = 0x0C,
    //	SI446X_SDO_MODE_CAL_WUT	= 0x0D,
    SI446X_SDO_MODE_WUT = 0x0E,
    SI446X_SDO_MODE_EN_PA = 0x0F,
    SI446X_SDO_MODE_TX_DATA_CLK = 0x10,
    SI446X_SDO_MODE_RX_DATA_CLK = 0x11,
    SI446X_SDO_MODE_EN_LNA = 0x12,
    SI446X_SDO_MODE_TX_DATA = 0x13,
    SI446X_SDO_MODE_RX_DATA = 0x14,
    SI446X_SDO_MODE_RX_RAW_DATA = 0x15,
    SI446X_SDO_MODE_ANTENNA_1_SW = 0x16,
    SI446X_SDO_MODE_ANTENNA_2_SW = 0x17,
    SI446X_SDO_MODE_VALID_PREAMBLE = 0x18,
    SI446X_SDO_MODE_INVALID_PREAMBLE = 0x19,
    SI446X_SDO_MODE_SYNC_WORD_DETECT = 0x1A,
    SI446X_SDO_MODE_CCA = 0x1B,
    //	SI446X_SDO_MODE_IN_SLEEP		= 0x1C,
    //	SI446X_SDO_MODE_PKT_TRACE		= 0x1D,
    // Nothing for 0x1E (30)
    //	SI446X_SDO_MODE_TX_RX_DATA_CLK	= 0x1F,
    //	SI446X_SDO_MODE_TX_STATE		= 0x20,
    //	SI446X_SDO_MODE_RX_STATE		= 0x21,
    //	SI446X_SDO_MODE_RX_FIFO_FULL	= 0x22,
    //	SI446X_SDO_MODE_TX_FIFO_EMPTY	= 0x23,
    //	SI446X_SDO_MODE_LOW_BATT		= 0x24,
    //	SI446X_SDO_MODE_CCA_LATCH		= 0x25,
    //	SI446X_SDO_MODE_HOPPED			= 0x26,
    //	SI446X_SDO_MODE_HOP_TABLE_WRAP	= 0x27
} SI446X_SDO_Mode_t;

/* GPIO pull-up modifier */
#define SI446X_GPIO_PULL_EN 0x40 /* Pullup enable for GPIO pins */
#define SI446X_GPIO_PULL_DIS 0x00 /* Pullup disable for GPIO pins */
#define SI446X_NIRQ_PULL_EN 0x40 /* Pullup enable for NIRQ pin */
#define SI446X_NIRQ_PULL_DIS 0x00 /* Pullup disable for NIRQ pin */
#define SI446X_SDO_PULL_EN 0x40 /* Pullup enable for SDO pin */
#define SI446X_SDO_PULL_DIS 0x00 /* Pullup disable for SDO pin */
#define SI446X_PIN_PULL_EN 0x40 /* Pullup enable for any pin */
#define SI446X_PIN_PULL_DIS 0x00 /* Pullup disable for any pin */

/* GPIO drive strength modifier */
#define SI446X_GPIO_DRV_HIGH 0x00 /* GPIO drive strength high */
#define SI446X_GPIO_DRV_MED_HIGH 0x20 /* GPIO drive strength medium-high */
#define SI446X_GPIO_DRV_MED_LOW 0x40 /* GPIO drive strength medium-low */
#define SI446X_GPIO_DRV_LOW 0x60 /* GPIO drive strength low */

/* Property group */
#define SI446X_PROP_GROUP_GLOBAL 0x00 /* Property group global */
#define SI446X_PROP_GROUP_INT 0x01 /* Property group interrupts */
#define SI446X_PROP_GROUP_FRR 0x02 /* Property group fast response registers */
#define SI446X_PROP_GROUP_PREAMBLE 0x10 /* Property group preamble */
#define SI446X_PROP_GROUP_SYNC 0x11 /* Property group sync */
#define SI446X_PROP_GROUP_PKT 0x12 /* Property group packet config */
#define SI446X_PROP_GROUP_MODEM 0x20 /* Property group modem */
#define SI446X_PROP_GROUP_MODEM_CHFLT 0x21 /* Property group RX coefficients */
#define SI446X_PROP_GROUP_PA 0x22 /* Property group power amp */
#define SI446X_PROP_GROUP_SYNTH 0x23 /* Property group synthesizer */
#define SI446X_PROP_GROUP_MATCH 0x30 /* Property group address match */
#define SI446X_PROP_GROUP_FREQ_CONTROL 0x40 /* Property group frequency control */
#define SI446X_PROP_GROUP_RX_HOP 0x50 /* Property group RX hop */
#define SI446X_PROP_GROUP_PTI 0xF0 /* Property group packet trace interface */

/* Radio states used  SI446X_CMD_START_RX, SI446X_CMD_START_RX, SI446X_CMD_REQUEST_DEVICE_STATE, SI446X_CMD_CHANGE_STATE*/
typedef enum {
    SI446X_STATE_NOCHANGE = 0x00, /* No change. */
    SI446X_STATE_SLEEP = 0x01, /* Sleep state. */
    SI446X_STATE_SPI_ACTIVE = 0x02, /* Spi Active state. */
    SI446X_STATE_READY = 0x03, /* Ready state. */
    SI446X_STATE_READY2 = 0x04, /* Another enumeration for Ready state. */
    SI446X_STATE_TX_TUNE = 0x05, /* Tune state for TX. */
    SI446X_STATE_RX_TUNE = 0x06, /* Tune state for RX. */
    SI446X_STATE_TX = 0x07, /* TX state. */
    SI446X_STATE_RX = 0x08 /* RX state. */
} SI446X_State_t;

typedef enum {
    //PROP_GLOBAL
    SI446X_PROP_GLOBAL_XO_TUNE =
        0x0000, //0x40	Configure the internal capacitor frequency tuning bank for the crystal oscillator.
    SI446X_PROP_GLOBAL_CLK_CFG = 0x0001, //0x00	Clock configuration options.
    SI446X_PROP_GLOBAL_LOW_BATT_THRESH =
        0x0002, //0x18	Configures the threshold voltage for low-battery detection.
    SI446X_PROP_GLOBAL_CONFIG = 0x0003, //0x20	Global configuration settings.
    SI446X_PROP_GLOBAL_WUT_CONFIG = 0x0004, //0x00	General Wakeup Timer feature configuration.
    SI446X_PROP_GLOBAL_WUT_M_1 =
        0x0005, //0x00 	Configure the mantissa of the Wake-Up Timer (WUT) value.
    SI446X_PROP_GLOBAL_WUT_M_0 =
        0x0006, //0x01	Configure the mantissa of the Wake-Up Timer (WUT) value.
    SI446X_PROP_GLOBAL_WUT_R =
        0x0007, //0x60	Configure the exponent of the Wake-Up Timer (WUT) value.
    SI446X_PROP_GLOBAL_WUT_LDC =
        0x0008, //0x00	Configures the period of time the chip remains active after automatic wake-up in LDC mode.
    SI446X_PROP_GLOBAL_WUT_CAL =
        0x0009, //0x00	Controls if calibration of the 32K R-C Oscillator will be performed on intervals of the WUT.

    //PROP_INT_CTL
    SI446X_PROP_INT_CTL_ENABLE =
        0x0100, //0x04	This property provides for global enabling of the three interrupt groups (Chip, Modem and Packet Handler) in order to generate HW interrupts at the NIRQ pin.
    SI446X_PROP_INT_CTL_PH_ENABLE =
        0x0101, //0x00	Enable individual interrupt sources within the Packet Handler Interrupt Group to generate a HW interrupt on the NIRQ output pin.
    SI446X_PROP_INT_CTL_MODEM_ENABLE =
        0x0102, //0x00	Enable individual interrupt sources within the Modem Interrupt Group to generate a HW interrupt on the NIRQ output pin.
    SI446X_PROP_INT_CTL_CHIP_ENABLE =
        0x0103, //0x04	Enable individual interrupt sources within the Chip Interrupt Group to generate a HW interrupt on the NIRQ output pin.

    //PROP_FRR_CTL
    SI446X_PROP_FRR_CTL_A_MODE = 0x0200, //0x01	Fast Response Register A Configuration.
    SI446X_PROP_FRR_CTL_B_MODE = 0x0201, //0x02	Fast Response Register B Configuration.
    SI446X_PROP_FRR_CTL_C_MODE = 0x0202, //0x09	Fast Response Register C Configuration.
    SI446X_PROP_FRR_CTL_D_MODE = 0x0203, //0x00	Fast Response Register D Configuration.

    //PROP_PREAMBLE
    SI446X_PROP_PREAMBLE_TX_LENGTH = 0x1000, //0x08	Configure length of TX Preamble.
    SI446X_PROP_PREAMBLE_CONFIG_STD_1 =
        0x1001, //0x14	Configuration of reception of a packet with a Standard Preamble pattern.
    SI446X_PROP_PREAMBLE_CONFIG_NSTD =
        0x1002, //0x00	Configuration of transmission/reception of a packet with a Non-Standard Preamble pattern.
    SI446X_PROP_PREAMBLE_CONFIG_STD_2 =
        0x1003, //0x0f	Configuration of timeout periods during reception of a packet with Standard Preamble pattern.
    SI446X_PROP_PREAMBLE_CONFIG =
        0x1004, //0x21	General configuration bits for the Preamble field.
    SI446X_PROP_PREAMBLE_PATTERN_3 =
        0x1005, //0x00	Configuration of the bit values describing a Non-Standard Preamble pattern.
    SI446X_PROP_PREAMBLE_PATTERN_2 =
        0x1006, //0x00	Configuration of the bit values describing a Non-Standard Preamble pattern.
    SI446X_PROP_PREAMBLE_PATTERN_1 =
        0x1007, //0x00	Configuration of the bit values describing a Non-Standard Preamble pattern.
    SI446X_PROP_PREAMBLE_PATTERN_0 =
        0x1008, //0x00	Configuration of the bit values describing a Non-Standard Preamble pattern.
    SI446X_PROP_PREAMBLE_POSTAMBLE_CONFIG =
        0x1009, //0x00	Configuration of Postamble functionality and the Postamble pattern bits.
    SI446X_PROP_PREAMBLE_POSTAMBLE_PATTERN_3 = 0x100a, //0x00	Defines the Postamble pattern.
    SI446X_PROP_PREAMBLE_POSTAMBLE_PATTERN_2 = 0x100b, //0x00	Defines the Postamble pattern.
    SI446X_PROP_PREAMBLE_POSTAMBLE_PATTERN_1 = 0x100c, //0x00	Defines the Postamble pattern.
    SI446X_PROP_PREAMBLE_POSTAMBLE_PATTERN_0 = 0x100d, //0x00	Defines the Postamble pattern.

    //PROP_SYNC
    SI446X_PROP_SYNC_CONFIG = 0x1100, //0x01	Sync Word configuration bits.
    SI446X_PROP_SYNC_BITS_3 = 0x1101, //0x2d	Sync word.
    SI446X_PROP_SYNC_BITS_2 = 0x1102, //0xd4	Sync word.
    SI446X_PROP_SYNC_BITS_1 = 0x1103, //0x2d	Sync word.
    SI446X_PROP_SYNC_BITS_0 = 0x1104, //0xd4	Sync word.
    SI446X_PROP_SYNC_CONFIG2 = 0x1105, //0x00	Sync Word configuration bits.

    //PROP_PKT
    SI446X_PROP_PKT_CRC_CONFIG = 0x1200, //0x00	Select a CRC polynomial and seed.
    SI446X_PROP_PKT_WHT_POLY_1 =
        0x1201, //0x01	16-bit polynomial value for the PN Generator (e.g., for Data Whitening)
    SI446X_PROP_PKT_WHT_POLY_0 =
        0x1202, //0x08	16-bit polynomial value for the PN Generator (e.g., for Data Whitening)
    SI446X_PROP_PKT_WHT_SEED_1 =
        0x1203, //0xff	16-bit seed value for the PN Generator (e.g., for Data Whitening)
    SI446X_PROP_PKT_WHT_SEED_0 =
        0x1204, //0xff	16-bit seed value for the PN Generator (e.g., for Data Whitening)
    SI446X_PROP_PKT_WHT_BIT_NUM =
        0x1205, //0x00	Selects which bit of the LFSR (used to generate the PN / data whitening sequence) is used as the output bit for data scrambling.
    SI446X_PROP_PKT_CONFIG1 =
        0x1206, //0x00	General configuration bits for transmission or reception of a packet.
    SI446X_PROP_PKT_CONFIG2 = 0x1207, //0x00	General packet configuration bits.
    SI446X_PROP_PKT_LEN =
        0x1208, //0x00	Configuration bits for reception of a variable length packet.
    SI446X_PROP_PKT_LEN_FIELD_SOURCE =
        0x1209, //0x00	Field number containing the received packet length byte(s).
    SI446X_PROP_PKT_LEN_ADJUST =
        0x120a, //0x00	Provides for adjustment/offset of the received packet length value (in order to accommodate a variety of methods of defining total packet length).
    SI446X_PROP_PKT_TX_THRESHOLD = 0x120b, //0x30	TX FIFO almost empty threshold.
    SI446X_PROP_PKT_RX_THRESHOLD = 0x120c, //0x30	RX FIFO Almost Full threshold.
    SI446X_PROP_PKT_FIELD_1_LENGTH_1 = 0x120d, //0x00	Unsigned 13-bit Field 1 length value.
    SI446X_PROP_PKT_FIELD_1_LENGTH_0 = 0x120e, //0x00	Unsigned 13-bit Field 1 length value.
    SI446X_PROP_PKT_FIELD_1_CONFIG =
        0x120f, //0x00	General data processing and packet configuration bits for Field 1.
    SI446X_PROP_PKT_FIELD_1_CRC_CONFIG =
        0x1210, //0x00	Configuration of CRC control bits across Field 1.
    SI446X_PROP_PKT_FIELD_2_LENGTH_1 = 0x1211, //0x00	Unsigned 13-bit Field 2 length value.
    SI446X_PROP_PKT_FIELD_2_LENGTH_0 = 0x1212, //0x00	Unsigned 13-bit Field 2 length value.
    SI446X_PROP_PKT_FIELD_2_CONFIG =
        0x1213, //0x00	General data processing and packet configuration bits for Field 2.
    SI446X_PROP_PKT_FIELD_2_CRC_CONFIG =
        0x1214, //0x00	Configuration of CRC control bits across Field 2.
    SI446X_PROP_PKT_FIELD_3_LENGTH_1 = 0x1215, //0x00	Unsigned 13-bit Field 3 length value.
    SI446X_PROP_PKT_FIELD_3_LENGTH_0 = 0x1216, //0x00	Unsigned 13-bit Field 3 length value.
    SI446X_PROP_PKT_FIELD_3_CONFIG =
        0x1217, //0x00	General data processing and packet configuration bits for Field 3.
    SI446X_PROP_PKT_FIELD_3_CRC_CONFIG =
        0x1218, //0x00	Configuration of CRC control bits across Field 3.
    SI446X_PROP_PKT_FIELD_4_LENGTH_1 = 0x1219, //0x00	Unsigned 13-bit Field 4 length value.
    SI446X_PROP_PKT_FIELD_4_LENGTH_0 = 0x121a, //0x00	Unsigned 13-bit Field 4 length value.
    SI446X_PROP_PKT_FIELD_4_CONFIG =
        0x121b, //0x00	General data processing and packet configuration bits for Field 4.
    SI446X_PROP_PKT_FIELD_4_CRC_CONFIG =
        0x121c, //0x00	Configuration of CRC control bits across Field 4.
    SI446X_PROP_PKT_FIELD_5_LENGTH_1 = 0x121d, //0x00	Unsigned 13-bit Field 5 length value.
    SI446X_PROP_PKT_FIELD_5_LENGTH_0 = 0x121e, //0x00	Unsigned 13-bit Field 5 length value.
    SI446X_PROP_PKT_FIELD_5_CONFIG =
        0x121f, //0x00	General data processing and packet configuration bits for Field 5.
    SI446X_PROP_PKT_FIELD_5_CRC_CONFIG =
        0x1220, //0x00	Configuration of CRC control bits across Field 5.
    SI446X_PROP_PKT_RX_FIELD_1_LENGTH_1 = 0x1221, //0x00	Unsigned 13-bit RX Field 1 length value.
    SI446X_PROP_PKT_RX_FIELD_1_LENGTH_0 = 0x1222, //0x00 	Unsigned 13-bit RX Field 1 length value.
    SI446X_PROP_PKT_RX_FIELD_1_CONFIG =
        0x1223, //0x00	General data processing and packet configuration bits for RX Field 1.
    SI446X_PROP_PKT_RX_FIELD_1_CRC_CONFIG =
        0x1224, //0x00	Configuration of CRC control bits across RX Field 1.
    SI446X_PROP_PKT_RX_FIELD_2_LENGTH_1 = 0x1225, //0x00	Unsigned 13-bit RX Field 2 length value.
    SI446X_PROP_PKT_RX_FIELD_2_LENGTH_0 = 0x1226, //0x00	Unsigned 13-bit RX Field 2 length value.
    SI446X_PROP_PKT_RX_FIELD_2_CONFIG =
        0x1227, //0x00	General data processing and packet configuration bits for RX Field 2.
    SI446X_PROP_PKT_RX_FIELD_2_CRC_CONFIG =
        0x1228, //0x00	Configuration of CRC control bits across RX Field 2.
    SI446X_PROP_PKT_RX_FIELD_3_LENGTH_1 = 0x1229, //0x00	Unsigned 13-bit RX Field 3 length value.
    SI446X_PROP_PKT_RX_FIELD_3_LENGTH_0 = 0x122a, //0x00	Unsigned 13-bit RX Field 3 length value.
    SI446X_PROP_PKT_RX_FIELD_3_CONFIG =
        0x122b, //0x00	General data processing and packet configuration bits for RX Field 3.
    SI446X_PROP_PKT_RX_FIELD_3_CRC_CONFIG =
        0x122c, //0x00	Configuration of CRC control bits across RX Field 3.
    SI446X_PROP_PKT_RX_FIELD_4_LENGTH_1 = 0x122d, //0x00	Unsigned 13-bit RX Field 4 length value.
    SI446X_PROP_PKT_RX_FIELD_4_LENGTH_0 = 0x122e, //0x00	Unsigned 13-bit RX Field 4 length value.
    SI446X_PROP_PKT_RX_FIELD_4_CONFIG =
        0x122f, //0x00	General data processing and packet configuration bits for RX Field 4.
    SI446X_PROP_PKT_RX_FIELD_4_CRC_CONFIG =
        0x1230, //0x00	Configuration of CRC control bits across RX Field 4.
    SI446X_PROP_PKT_RX_FIELD_5_LENGTH_1 = 0x1231, //0x00	Unsigned 13-bit RX Field 5 length value.
    SI446X_PROP_PKT_RX_FIELD_5_LENGTH_0 = 0x1232, //0x00	Unsigned 13-bit RX Field 5 length value.
    SI446X_PROP_PKT_RX_FIELD_5_CONFIG =
        0x1233, //0x00	General data processing and packet configuration bits for RX Field 5.
    SI446X_PROP_PKT_RX_FIELD_5_CRC_CONFIG =
        0x1234, //0x00	Configuration of CRC control bits across RX Field 5.
    SI446X_PROP_PKT_CRC_SEED_3 = 0x1236, //0x00	32-bit seed value for the 32-bit CRC engine
    SI446X_PROP_PKT_CRC_SEED_2 = 0x1237, //0x00	32-bit seed value for the 32-bit CRC engine
    SI446X_PROP_PKT_CRC_SEED_1 = 0x1238, //0x00	32-bit seed value for the 32-bit CRC engine
    SI446X_PROP_PKT_CRC_SEED_0 = 0x1239, //0x00	32-bit seed value for the 32-bit CRC engine

    //PROP_MODEM
    SI446X_PROP_MODEM_MOD_TYPE =
        0x2000, //0x02	Selects the type of modulation. In TX mode, additionally selects the source of the modulation.
    SI446X_PROP_MODEM_MAP_CONTROL =
        0x2001, //0x80	Controls polarity and mapping of transmit and receive bits.
    SI446X_PROP_MODEM_DSM_CTRL =
        0x2002, //0x07	Miscellaneous control bits for the Delta-Sigma Modulator (DSM) in the PLL Synthesizer.
    SI446X_PROP_MODEM_DATA_RATE_2 =
        0x2003, //0x0f	Unsigned 24-bit value used to determine the TX data rate
    SI446X_PROP_MODEM_DATA_RATE_1 =
        0x2004, //0x42	Unsigned 24-bit value used to determine the TX data rate
    SI446X_PROP_MODEM_DATA_RATE_0 =
        0x2005, //0x40	Unsigned 24-bit value used to determine the TX data rate
    SI446X_PROP_MODEM_TX_NCO_MODE_3 =
        0x2006, //0x01	TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
    SI446X_PROP_MODEM_TX_NCO_MODE_2 =
        0x2007, //0xc9	TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
    SI446X_PROP_MODEM_TX_NCO_MODE_1 =
        0x2008, //0xc3	TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
    SI446X_PROP_MODEM_TX_NCO_MODE_0 =
        0x2009, //0x80	TX Gaussian filter oversampling ratio and Byte 3 of unsigned 26-bit TX Numerically Controlled Oscillator (NCO) modulus.
    SI446X_PROP_MODEM_FREQ_DEV_2 = 0x200a, //0x00	17-bit unsigned TX frequency deviation word.
    SI446X_PROP_MODEM_FREQ_DEV_1 = 0x200b, //0x06	17-bit unsigned TX frequency deviation word.
    SI446X_PROP_MODEM_FREQ_DEV_0 = 0x200c, //0xd3	17-bit unsigned TX frequency deviation word.
    SI446X_PROP_MODEM_FREQ_OFFSET_1 =
        0x200d, //0x00	Frequency offset adjustment (a 16-bit signed number).
    SI446X_PROP_MODEM_FREQ_OFFSET_0 =
        0x200e, //0x00	Frequency offset adjustment (a 16-bit signed number).
    SI446X_PROP_MODEM_TX_FILTER_COEFF_8 =
        0x200f, //0x67	The 8th coefficient of TX spectral shaping filter.
    SI446X_PROP_MODEM_TX_FILTER_COEFF_7 =
        0x2010, //0x60	The 7th coefficient of TX spectral shaping filter.
    SI446X_PROP_MODEM_TX_FILTER_COEFF_6 =
        0x2011, //0x4d	The 6th coefficient of TX spectral shaping filter.
    SI446X_PROP_MODEM_TX_FILTER_COEFF_5 =
        0x2012, //0x36	The 5th coefficient of TX spectral shaping filter.
    SI446X_PROP_MODEM_TX_FILTER_COEFF_4 =
        0x2013, //0x21	The 4th coefficient of TX spectral shaping filter.
    SI446X_PROP_MODEM_TX_FILTER_COEFF_3 =
        0x2014, //0x11	The 3rd coefficient of TX spectral shaping filter.
    SI446X_PROP_MODEM_TX_FILTER_COEFF_2 =
        0x2015, //0x08	The 2nd coefficient of TX spectral shaping filter.
    SI446X_PROP_MODEM_TX_FILTER_COEFF_1 =
        0x2016, //0x03	The 1st coefficient of TX spectral shaping filter.
    SI446X_PROP_MODEM_TX_FILTER_COEFF_0 =
        0x2017, //0x01	The 0th coefficient of TX spectral shaping filter.
    SI446X_PROP_MODEM_TX_RAMP_DELAY = 0x2018, //0x01	TX ramp-down delay setting.
    SI446X_PROP_MODEM_MDM_CTRL = 0x2019, //0x00	MDM control.
    SI446X_PROP_MODEM_IF_CONTROL =
        0x201a, //0x08	Selects Fixed-IF, Scaled-IF, or Zero-IF mode of RX Modem operation.
    SI446X_PROP_MODEM_IF_FREQ_2 =
        0x201b, //0x03	the IF frequency setting (an 18-bit signed number).
    SI446X_PROP_MODEM_IF_FREQ_1 =
        0x201c, //0xc0	the IF frequency setting (an 18-bit signed number).
    SI446X_PROP_MODEM_IF_FREQ_0 =
        0x201d, //0x00	the IF frequency setting (an 18-bit signed number).
    SI446X_PROP_MODEM_DECIMATION_CFG1 =
        0x201e, //0x10	Specifies three decimator ratios for the Cascaded Integrator Comb (CIC) filter.
    SI446X_PROP_MODEM_DECIMATION_CFG0 =
        0x201f, //0x20	Specifies miscellaneous parameters and decimator ratios for the Cascaded Integrator Comb (CIC) filter.
    SI446X_PROP_MODEM_DECIMATION_CFG2 =
        0x2020, //0x00	Specifies miscellaneous decimator filter selections.
    SI446X_PROP_MODEM_IFPKD_THRESHOLDS = 0x2021, //0xe8
    SI446X_PROP_MODEM_BCR_OSR_1 =
        0x2022, //0x00	RX BCR/Slicer oversampling rate (12-bit unsigned number).
    SI446X_PROP_MODEM_BCR_OSR_0 =
        0x2023, //0x4b	RX BCR/Slicer oversampling rate (12-bit unsigned number).
    SI446X_PROP_MODEM_BCR_NCO_OFFSET_2 =
        0x2024, //0x06	RX BCR NCO offset value (an unsigned 22-bit number).
    SI446X_PROP_MODEM_BCR_NCO_OFFSET_1 =
        0x2025, //0xd3	RX BCR NCO offset value (an unsigned 22-bit number).
    SI446X_PROP_MODEM_BCR_NCO_OFFSET_0 =
        0x2026, //0xa0	RX BCR NCO offset value (an unsigned 22-bit number).
    SI446X_PROP_MODEM_BCR_GAIN_1 = 0x2027, //0x06		The unsigned 11-bit RX BCR loop gain value.
    SI446X_PROP_MODEM_BCR_GAIN_0 = 0x2028, //0xd3	The unsigned 11-bit RX BCR loop gain value.
    SI446X_PROP_MODEM_BCR_GEAR = 0x2029, //0x02	RX BCR loop gear control.
    SI446X_PROP_MODEM_BCR_MISC1 = 0x202a, //0xc0	Miscellaneous control bits for the RX BCR loop.
    SI446X_PROP_MODEM_BCR_MISC0 = 0x202b, //0x00	Miscellaneous RX BCR loop controls.
    SI446X_PROP_MODEM_AFC_GEAR = 0x202c, //0x00	RX AFC loop gear control.
    SI446X_PROP_MODEM_AFC_WAIT = 0x202d, //0x23	RX AFC loop wait time control.
    SI446X_PROP_MODEM_AFC_GAIN_1 =
        0x202e, //0x83	Sets the gain of the PLL-based AFC acquisition loop, and provides miscellaneous control bits for AFC functionality.
    SI446X_PROP_MODEM_AFC_GAIN_0 =
        0x202f, //0x69	Sets the gain of the PLL-based AFC acquisition loop, and provides miscellaneous control bits for AFC functionality.
    SI446X_PROP_MODEM_AFC_LIMITER_1 = 0x2030, //0x00	Set the AFC limiter value.
    SI446X_PROP_MODEM_AFC_LIMITER_0 = 0x2031, //0x40	Set the AFC limiter value.
    SI446X_PROP_MODEM_AFC_MISC = 0x2032, //0xa0	Specifies miscellaneous AFC control bits.
    SI446X_PROP_MODEM_AFC_ZIFOFF = 0x2033, //0x00	AFC fixed frequency offset in zero IF mode.
    SI446X_PROP_MODEM_ADC_CTRL = 0x2034, //0x00	Sigma Delta ADC controls.
    SI446X_PROP_MODEM_AGC_CONTROL =
        0x2035, //0xe0	Miscellaneous control bits for the Automatic Gain Control (AGC) function in the RX Chain.
    SI446X_PROP_MODEM_AGC_WINDOW_SIZE =
        0x2038, //0x11	Specifies the size of the measurement and settling windows for the AGC algorithm.
    SI446X_PROP_MODEM_AGC_RFPD_DECAY =
        0x2039, //0x10	Sets the decay time of the RF peak detectors.
    SI446X_PROP_MODEM_AGC_IFPD_DECAY =
        0x203a, //0x10	Sets the decay time of the IF peak detectors.
    SI446X_PROP_MODEM_FSK4_GAIN1 =
        0x203b, //0x0b	Specifies the gain factor of the secondary branch in 4(G)FSK ISI-suppression.
    SI446X_PROP_MODEM_FSK4_GAIN0 =
        0x203c, //0x1c	Specifies the gain factor of the primary branch in 4(G)FSK ISI-suppression.
    SI446X_PROP_MODEM_FSK4_TH_1 = 0x203d, //0x40	16 bit 4(G)FSK slicer threshold.
    SI446X_PROP_MODEM_FSK4_TH_0 = 0x203e, //0x00	16 bit 4(G)FSK slicer threshold.
    SI446X_PROP_MODEM_FSK4_MAP = 0x203f, //0x00	4(G)FSK symbol mapping code.
    SI446X_PROP_MODEM_OOK_PDTC =
        0x2040, //0x2b	Configures the attack and decay times of the OOK Peak Detector.
    SI446X_PROP_MODEM_OOK_BLOPK =
        0x2041, //0x0c	Configures the slicing reference level of the OOK Peak Detector.
    SI446X_PROP_MODEM_OOK_CNT1 = 0x2042, //0xa4	OOK control.
    SI446X_PROP_MODEM_OOK_MISC =
        0x2043, //0x03	Selects the detector(s) used for demodulation of an OOK signal, or for demodulation of a (G)FSK signal when using the asynchronous demodulator.
    SI446X_PROP_MODEM_RAW_CONTROL =
        0x2045, //0x02	Defines gain and enable controls for raw / nonstandard mode.
    SI446X_PROP_MODEM_RAW_EYE_1 = 0x2046, //0x00	11 bit eye-open detector threshold.
    SI446X_PROP_MODEM_RAW_EYE_0 = 0x2047, //0xa3	11 bit eye-open detector threshold.
    SI446X_PROP_MODEM_ANT_DIV_MODE = 0x2048, //0x02	Antenna diversity mode settings.
    SI446X_PROP_MODEM_ANT_DIV_CONTROL =
        0x2049, //0x80	Specifies controls for the Antenna Diversity algorithm.
    SI446X_PROP_MODEM_RSSI_THRESH = 0x204a, //0xff	Configures the RSSI threshold.
    SI446X_PROP_MODEM_RSSI_JUMP_THRESH =
        0x204b, //0x0c	Configures the RSSI Jump Detection threshold.
    SI446X_PROP_MODEM_RSSI_CONTROL =
        0x204c, //0x01	Control of the averaging modes and latching time for reporting RSSI value(s).
    SI446X_PROP_MODEM_RSSI_CONTROL2 = 0x204d, //0x00	RSSI Jump Detection control.
    SI446X_PROP_MODEM_RSSI_COMP = 0x204e, //0x40	RSSI compensation value.
    SI446X_PROP_MODEM_RAW_SEARCH2 =
        0x2050, //0x00	Defines and controls the search period length for the Moving Average and Min-Max detectors.
    SI446X_PROP_MODEM_CLKGEN_BAND =
        0x2051, //0x08	Select PLL Synthesizer output divider ratio as a function of frequency band.
    SI446X_PROP_MODEM_SPIKE_DET =
        0x2054, //0x00	Configures the threshold for (G)FSK Spike Detection.
    SI446X_PROP_MODEM_ONE_SHOT_AFC =
        0x2055, //0x00	Configures parameters for th e One Shot AFC function and for BCR timing/acquisition.
    SI446X_PROP_MODEM_RSSI_HYSTERESIS =
        0x2056, //0xff	Configures the amount of hysteresis on the RSSI threshold detection function.
    SI446X_PROP_MODEM_RSSI_MUTE =
        0x2057, //0x00	Configures muting of the RSSI to avoid false RSSI interrupts.
    SI446X_PROP_MODEM_FAST_RSSI_DELAY =
        0x2058, //0x00	Configures the delay for fast RSSI Latching mode.
    SI446X_PROP_MODEM_PSM_1 = 0x2059, //0x00	Configures the Preamble Sense Mode feature
    SI446X_PROP_MODEM_PSM_0 = 0x205a, //0x00	Configures the Preamble Sense Mode feature
    SI446X_PROP_MODEM_DSA_CTRL1 =
        0x205b, //0x00	Configures parameters for the Signal Arrival Detection circuit block and algorithm.
    SI446X_PROP_MODEM_DSA_CTRL2 =
        0x205c, //0x00	Configures parameters for the Signal Arrival Detection circuit block and algorithm.
    SI446X_PROP_MODEM_DSA_QUAL =
        0x205d, //0x00	Configures parameters for the Eye Opening qualification m ethod of the Signal Arrival Detection algorithm.
    SI446X_PROP_MODEM_DSA_RSSI = 0x205e, //0x00	Signal Arrival Detect RSSI Qualifier Config
    SI446X_PROP_MODEM_DSA_MISC = 0x205f, //0x00	Miscellaneous detection of signal arrival bits.

    //PROP_MODEM_CHFLT
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_17 =
        0x2100, //0xff	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_16 =
        0x2101, //0xba	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_15 =
        0x2102, //0x0f	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_14 =
        0x2103, //0x51	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_13 =
        0x2104, //0xcf	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_12 =
        0x2105, //0xa9	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_11 =
        0x2106, //0xc9	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_10 =
        0x2107, //0xfc	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_9 =
        0x2108, //0x1b	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_8 =
        0x2109, //0x1e	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_7 =
        0x210a, //0x0f	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_6 =
        0x210b, //0x01	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_5 =
        0x210c, //0xfc	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_4 =
        0x210d, //0xfd	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_3 =
        0x210e, //0x15	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_2 =
        0x210f, //0xff	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_1 =
        0x2110, //0x00	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX1_CHFLT_COE_0 =
        0x2111, //0x0f	Filter coefficients for the first set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_17 =
        0x2112, //0xff	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_16 =
        0x2113, //0xc4	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_15 =
        0x2114, //0x30	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_14 =
        0x2115, //0x7f	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_13 =
        0x2116, //0xf5	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_12 =
        0x2117, //0xb5	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_11 =
        0x2118, //0xb8	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_10 =
        0x2119, //0xde	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_9 =
        0x211a, //0x05	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_8 =
        0x211b, //0x17	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_7 =
        0x211c, //0x16	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_6 =
        0x211d, //0x0c	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_5 =
        0x211e, //0x03	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_4 =
        0x211f, //0x00	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_3 =
        0x2120, //0x15	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_2 =
        0x2121, //0xff	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_1 =
        0x2122, //0x00	Filter coefficients for the second set of RX filter coefficients.
    SI446X_PROP_MODEM_CHFLT_RX2_CHFLT_COE_0 =
        0x2123, //0x00	Filter coefficients for the second set of RX filter coefficients.

    //PROP_PA
    SI446X_PROP_PA_MODE =
        0x2200, //0x08	Selects the PA operating mode, and selects resolution of PA power adjustment (i.e., step size).
    SI446X_PROP_PA_PWR_LVL = 0x2201, //0x7f	Configuration of PA output power level.
    SI446X_PROP_PA_BIAS_CLKDUTY =
        0x2202, //0x00	Configuration of the PA Bias and duty cycle of the TX clock source.
    SI446X_PROP_PA_TC = 0x2203, //0x5d	Configuration of PA ramping parameters.
    SI446X_PROP_PA_RAMP_EX =
        0x2204, //0x80	Select the time constant of the external PA ramp signal.
    SI446X_PROP_PA_RAMP_DOWN_DELAY =
        0x2205, //0x23	Delay from the start of the PA ramp down to disabling of the PA output.
    SI446X_PROP_PA_DIG_PWR_SEQ_CONFIG = 0x2206, //0x03	Configuration for digital power sequencing.

    //PROP_SYNTH
    SI446X_PROP_SYNTH_PFDCP_CPFF = 0x2300, //0x2c	Feed forward charge pump current selection.
    SI446X_PROP_SYNTH_PFDCP_CPINT = 0x2301, //0x0e	Integration charge pump current selection.
    SI446X_PROP_SYNTH_VCO_KV =
        0x2302, //0x0b	Gain scaling factors (Kv) for the VCO tuning varactors on both the integrated-path and feed forward path.
    SI446X_PROP_SYNTH_LPFILT3 =
        0x2303, //0x04	Value of resistor R2 in feed-forward path of loop filter.
    SI446X_PROP_SYNTH_LPFILT2 =
        0x2304, //0x0c	Value of capacitor C2 in feed-forward path of loop filter.
    SI446X_PROP_SYNTH_LPFILT1 =
        0x2305, //0x73	Value of capacitors C1 and C3 in feed-forward path of loop filter.
    SI446X_PROP_SYNTH_LPFILT0 =
        0x2306, //0x03	Bias current of the active amplifier in the feed-forward loop filter.
    SI446X_PROP_SYNTH_VCO_KVCAL = 0x2307, //0x05	Scaling entire KV of VCO.

    //PROP_MATCH
    SI446X_PROP_MATCH_VALUE_1 =
        0x3000, //0x00	Match value to be compared with the result of logically AND-ing (bit-wise) the Mask 1 value with the received Match 1 byte.
    SI446X_PROP_MATCH_MASK_1 =
        0x3001, //0x00	Mask value to be logically AND-ed (bit-wise) with the Match 1 byte.
    SI446X_PROP_MATCH_CTRL_1 =
        0x3002, //0x00	Enable for Packet Match functionality, and configuration of Match Byte 1.
    SI446X_PROP_MATCH_VALUE_2 =
        0x3003, //0x00	Match value to be compared with the result of logically AND-ing (bit-wise) the Mask 2 value with the received Match 2 byte.
    SI446X_PROP_MATCH_MASK_2 =
        0x3004, //0x00	Mask value to be logically AND-ed (bit-wise) with the Match 2 byte.
    SI446X_PROP_MATCH_CTRL_2 = 0x3005, //0x00	Configuration of Match Byte 2.
    SI446X_PROP_MATCH_VALUE_3 =
        0x3006, //0x00	Match value to be compared with the result of logically AND-ing (bit-wise) the Mask 3 value with the received Match 3 byte.
    SI446X_PROP_MATCH_MASK_3 =
        0x3007, //0x00	Mask value to be logically AND-ed (bit-wise) with the Match 3 byte.
    SI446X_PROP_MATCH_CTRL_3 = 0x3008, //0x00	Configuration of Match Byte 3.
    SI446X_PROP_MATCH_VALUE_4 =
        0x3009, //0x00	Match value to be compared with the result of logically AND-ing (bit-wise) the Mask 4 value with the received Match 4 byte.
    SI446X_PROP_MATCH_MASK_4 =
        0x300a, //0x00	Mask value to be logically AND-ed (bit-wise) with the Match 4 byte.
    SI446X_PROP_MATCH_CTRL_4 = 0x300b, //0x00	Configuration of Match Byte 4.

    //PROP_FREQ_CONTROL
    SI446X_PROP_FREQ_CONTROL_INTE = 0x4000, //0x3c	Frac-N PLL Synthesizer integer divide number.
    SI446X_PROP_FREQ_CONTROL_FRAC_2 = 0x4001, //0x08	Frac-N PLL fraction number.
    SI446X_PROP_FREQ_CONTROL_FRAC_1 = 0x4002, //0x00	Frac-N PLL fraction number.
    SI446X_PROP_FREQ_CONTROL_FRAC_0 = 0x4003, //0x00	Frac-N PLL fraction number.
    SI446X_PROP_FREQ_CONTROL_CHANNEL_STEP_SIZE_1 =
        0x4004, //0x00	EZ Frequency Programming channel step size.
    SI446X_PROP_FREQ_CONTROL_CHANNEL_STEP_SIZE_0 =
        0x4005, //0x00	EZ Frequency Programming channel step size.
    SI446X_PROP_FREQ_CONTROL_W_SIZE =
        0x4006, //0x20	Set window gating period (in number of crystal reference clock cycles) for counting VCO frequency during calibration.
    SI446X_PROP_FREQ_CONTROL_VCOCNT_RX_ADJ =
        0x4007, //0xff	Adjust target count for VCO calibration in RX mode.

    //PROP_RX_HOP
    SI446X_PROP_RX_HOP_CONTROL =
        0x5000, //0x04	Configuration options for the automatic RX Hop functionality.
    SI446X_PROP_RX_HOP_TABLE_SIZE =
        0x5001, //0x01	Specifies the number of entries (channels) in the RX Hop table.
    SI446X_PROP_RX_HOP_TABLE_ENTRY_0 =
        0x5002, //0x00	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_1 =
        0x5003, //0x01	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_2 =
        0x5004, //0x02	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_3 =
        0x5005, //0x03	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_4 =
        0x5006, //0x04	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_5 =
        0x5007, //0x05	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_6 =
        0x5008, //0x06	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_7 =
        0x5009, //0x07	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_8 =
        0x500a, //0x08	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_9 =
        0x500b, //0x09	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_10 =
        0x500c, //0x0a	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_11 =
        0x500d, //0x0b	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_12 =
        0x500e, //0x0c	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_13 =
        0x500f, //0x0d	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_14 =
        0x5010, //0x0e	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_15 =
        0x5011, //0x0f	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_16 =
        0x5012, //0x10	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_17 =
        0x5013, //0x11	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_18 =
        0x5014, //0x12	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_19 =
        0x5015, //0x13	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_20 =
        0x5016, //0x14	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_21 =
        0x5017, //0x15	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_22 =
        0x5018, //0x16	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_23 =
        0x5019, //0x17	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_24 =
        0x501a, //0x18	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_25 =
        0x501b, //0x19	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_26 =
        0x501c, //0x1a	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_27 =
        0x501d, //0x1b	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_28 =
        0x501e, //0x1c	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_29 =
        0x501f, //0x1d	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_30 =
        0x5020, //0x1e	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_31 =
        0x5021, //0x1f	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_32 =
        0x5022, //0x20	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_33 =
        0x5023, //0x21	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_34 =
        0x5024, //0x22	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_35 =
        0x5025, //0x23	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_36 =
        0x5026, //0x24	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_37 =
        0x5027, //0x25	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_38 =
        0x5028, //0x26	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_39 =
        0x5029, //0x27	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_40 =
        0x502a, //0x28	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_41 =
        0x502b, //0x29	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_42 =
        0x502c, //0x2a	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_43 =
        0x502d, //0x2b	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_44 =
        0x502e, //0x2c	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_45 =
        0x502f, //0x2d	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_46 =
        0x5030, //0x2e	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_47 =
        0x5031, //0x2f	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_48 =
        0x5032, //0x30	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_49 =
        0x5033, //0x31	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_50 =
        0x5034, //0x32	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_51 =
        0x5035, //0x33	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_52 =
        0x5036, //0x34	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_53 =
        0x5037, //0x35	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_54 =
        0x5038, //0x36	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_55 =
        0x5039, //0x37	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_56 =
        0x503a, //0x38	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_57 =
        0x503b, //0x39	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_58 =
        0x503c, //0x3a	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_59 =
        0x503d, //0x3b	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_60 =
        0x503e, //0x3c	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_61 =
        0x503f, //0x3d	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_62 =
        0x5040, //0x3e	Defines entries in the RX Hopping Table (N=0 to 63)
    SI446X_PROP_RX_HOP_TABLE_ENTRY_63 =
        0x5041, //0x3f	Defines entries in the RX Hopping Table (N=0 to 63)

    //PROP_PTI
    SI446X_PROP_PTI_CTL = 0xf000, //0x80	Packet Trace Interface control fields.
    SI446X_PROP_PTI_BAUD_1 = 0xf001, //0x13	Desired baud rate for the PTI interface.
    SI446X_PROP_PTI_BAUD_0 = 0xf002, //0x88	Desired baud rate for the PTI interface.
    SI446X_PROP_PTI_LOG_EN = 0xf003, //0x00	Enables what the PTI logs.
} SI446X_Prop_t;

typedef enum {
    SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_TYPE_SYNCHRONOUS =
        0x00, //Direct mode operates in synchronous mode, applies to TX only.
    SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_TYPE_ASYNCHRONOUS =
        0x80, //Direct mode operates in asynchronous mode, applies to TX only. GFSK is not supported.
    SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_GPIO0 = 0x00, //TX direct mode uses GPIO0 as data source.
    SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_GPIO1 = 0x20, //TX direct mode uses GPIO1 as data source.
    SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_GPIO2 = 0x40, //TX direct mode uses GPIO2 as data source.
    SI446X_MODEM_MOD_TYPE_TX_DIRECT_MODE_GPIO3 = 0x60, //TX direct mode uses GPIO3 as data source.
    SI446X_MODEM_MOD_TYPE_MOD_SOURCE_PACKET_HANDLER =
        0x00, //The modulation is sourced from the TX FIFO in the packet handler.
    SI446X_MODEM_MOD_TYPE_MOD_SOURCE_DIRECT_MODE =
        0x08, //The modulation is sourced in real-time (i.e., TX Direct Mode) from a GPIO pin, as selected by the TX_DIRECT_MODE_GPIO field
    SI446X_MODEM_MOD_TYPE_MOD_SOURCE_RANDOM_GENERATOR =
        0x10, //The modulation is sourced from the internal pseudo-random generator.
    SI446X_MODEM_MOD_TYPE_MOD_TYPE_CW = 0x00, //Modulation CW
    SI446X_MODEM_MOD_TYPE_MOD_TYPE_OOK = 0x01, //Modulation OOK
    SI446X_MODEM_MOD_TYPE_MOD_TYPE_2FSK = 0x02, //Modulation 2FSK
    SI446X_MODEM_MOD_TYPE_MOD_TYPE_2GFSK = 0x03, //Modulation 2GFSK
    SI446X_MODEM_MOD_TYPE_MOD_TYPE_4FSK = 0x04, //Modulation 4FSK
    SI446X_MODEM_MOD_TYPE_MOD_TYPE_4GFSK = 0x05, //Modulation 4GFSK
} SI446X_Prop_Modem_Mod_Type_t;

#ifdef __cplusplus
}
#endif