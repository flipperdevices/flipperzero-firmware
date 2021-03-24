#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CC1101_WRITE            (1<<7)  /** Write Bit */
#define CC1101_BURST            (1<<6)  /** Burst Bit */

#define CC1101_STROBE_SRES      0x30    /** Reset chip. */
#define CC1101_STROBE_SFSTXON   0x31    /** Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1). If in RX (with CCA): Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround). */
#define CC1101_STROBE_SXOFF     0x32    /** Turn off crystal oscillator. */
#define CC1101_STROBE_SCAL      0x33    /** Calibrate frequency synthesizer and turn it off. SCAL can be strobed from IDLE mode without setting manual calibration mode (MCSM0.FS_AUTOCAL=0) */
#define CC1101_STROBE_SRX       0x34    /** Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1. */
#define CC1101_STROBE_STX       0x35    /** In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled: Only go to TX if channel is clear. */
#define CC1101_STROBE_SIDLE     0x36    /** Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable. */
#define CC1101_STROBE_SWOR      0x38    /** Start automatic RX polling sequence (Wake-on-Radio) as described in Section 19.5 if WORCTRL.RC_PD=0. */
#define CC1101_STROBE_SPWD      0x39    /** Enter power down mode when CSn goes high. */
#define CC1101_STROBE_SFRX      0x3A    /** Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states. */
#define CC1101_STROBE_SFTX      0x3B    /** Flush the TX FIFO buffer. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states. */
#define CC1101_STROBE_SWORRST   0x3C    /** Reset real time clock to Event1 value. */
#define CC1101_STROBE_SNOP      0x3D    /** No operation. May be used to get access to the chip status byte.*/

#define CC1101_STATUS_SRES      0x30    /**  */
#define CC1101_STATUS_SFSTXON   0x31    /**  */
#define CC1101_STATUS_SXOFF     0x32    /**  */
#define CC1101_STATUS_SCAL      0x33    /**  */
#define CC1101_STATUS_SRX       0x34    /**  */
#define CC1101_STATUS_STX       0x35    /**  */
#define CC1101_STATUS_SIDLE     0x36    /**  */
#define CC1101_STATUS_SWOR      0x38    /**  */
#define CC1101_STATUS_SPWD      0x39    /**  */
#define CC1101_STATUS_SFRX      0x3A    /**  */
#define CC1101_STATUS_SFTX      0x3B    /**  */
#define CC1101_STATUS_SWORRST   0x3C    /**  */
#define CC1101_STATUS_SNOP      0x3D    /**  */

#define CC1101_PATABLE          0x3E    /** PATABLE register number, an 8-byte table that defines the PA control settings */
#define CC1101_FIFO             0x3F    /** FIFO register nunmber, can be combined with CC1101_WRITE and/or CC1101_BURST */

typedef enum {
    CC1101StateIDLE=0b000,              /** IDLE state */
    CC1101StateRX=0b001,                /** Receive mode */
    CC1101StateTX=0b010,                /** Transmit mode */
    CC1101StateFSTXON=0b011,            /** Fast TX ready */
    CC1101StateCALIBRATE=0b100,         /** Frequency synthesizer calibration is running */
    CC1101StateSETTLING=0b101,          /** PLL is settling */
    CC1101StateRXFIFO_OVERFLOW=0b110,   /** RX FIFO has overflowed. Read out any useful data, then flush the FIFO with SFRX */
    CC1101StateTXFIFO_UNDERFLOW=0b111,  /** TX FIFO has underflowed. Acknowledge with SFTX */
} CC1101State;

typedef struct {
    uint8_t FIFO_BYTES_AVAILABLE:4;
    CC1101State STATE:3;
    bool CHIP_RDYn:1;
} CC1101Status;

#ifdef __cplusplus
}
#endif