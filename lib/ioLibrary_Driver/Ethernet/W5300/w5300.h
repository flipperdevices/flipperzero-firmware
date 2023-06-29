#ifndef	_W5300_H_
#define	_W5300_H_

#ifdef __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
//! \file w5300.h
//! \brief W5300 HAL Header File.
//! \version 1.0.0
//! \date 2015/05/01
//! \par  Revision history
//!       <2015/05/01> 1st Released for integrating with ioLibrary 
//!        Download the latest version directly from GitHub. Please visit the our GitHub repository for ioLibrary.
//!        >> https://github.com/Wiznet/ioLibrary_Driver
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2015, WIZnet Co., LTD.
//! All rights reserved.
//! 
//! Redistribution and use in source and binary forms, with or without 
//! modification, are permitted provided that the following conditions 
//! are met: 
//! 
//!     * Redistributions of source code must retain the above copyright 
//! notice, this list of conditions and the following disclaimer. 
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution. 
//!     * Neither the name of the <ORGANIZATION> nor the names of its 
//! contributors may be used to endorse or promote products derived 
//! from this software without specific prior written permission. 
//! 
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include <stdint.h>
#include "wizchip_conf.h"

/// \cond DOXY_APPLY_CODE
#if   (_WIZCHIP_ == 5300)
/// \endcond

#define _WIZCHIP_SN_BASE_  (0x0200)
#define _WIZCHIP_SN_SIZE_  (0x0040)


#define WIZCHIP_CREG_BLOCK      	      0x00   ///< Common register block
#define WIZCHIP_SREG_BLOCK(N)    	   (_WIZCHIP_SN_BASE_+ _WIZCHIP_SN_SIZE_*N) ///< Socket N register block

#define WIZCHIP_OFFSET_INC(ADDR, N)    (ADDR + N) ///< Increase offset address

#if (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_DIR_)
   #define _W5300_IO_BASE_     _WIZCHIP_IO_BASE_
#elif (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_BUS_INDIR_)
	#define IDM_AR             ((_WIZCHIP_IO_BASE_ + 0x0002))  ///< Indirect mode address register
	#define IDM_DR             ((_WIZCHIP_IO_BASE_ + 0x0004))  ///< Indirect mode data register
	#define _W5300_IO_BASE_    0x0000
#elif (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SPI_)
   #error "Unkonw _WIZCHIP_IO_MODE_"
#endif

///////////////////////////////////////
// Definition For Legacy Chip Driver //
///////////////////////////////////////
#define IINCHIP_READ(ADDR)                WIZCHIP_READ(ADDR)               ///< The defined for legacy chip driver
#define IINCHIP_WRITE(ADDR,VAL)           WIZCHIP_WRITE(ADDR,VAL)          ///< The defined for legacy chip driver
//#define IINCHIP_READ_BUF(ADDR,BUF,LEN)    WIZCHIP_READ_BUF(ADDR,BUF,LEN)   ///< The defined for legacy chip driver
//#define IINCHIP_WRITE_BUF(ADDR,BUF,LEN)   WIZCHIP_WRITE(ADDR,BUF,LEN)      ///< The defined for legacy chip driver

//--------------------------  defgroup ---------------------------------
/**
 * @defgroup W5300 W5300
 *
 * @brief WHIZCHIP register defines and I/O functions of @b W5300.
 *
 * - @ref WIZCHIP_register_W5300 : @ref Common_register_group_W5300 and @ref Socket_register_group_W5300
 * - @ref WIZCHIP_IO_Functions_W5300 : @ref Basic_IO_function_W5300, @ref Common_register_access_function_W5300 and @ref Socket_register_access_function_W5300
 */
 
 
/**
 * @defgroup WIZCHIP_register_W5300 WIZCHIP register
 * @ingroup W5300
 *
 * @brief WHIZCHIP register defines register group of @b W5300.
 *
 * - @ref Common_register_group_W5300 : Common register group
 * - @ref Socket_register_group_W5300 : \c SOCKET n register group
 */


/**
 * @defgroup WIZCHIP_IO_Functions_W5300 WIZCHIP I/O functions
 * @ingroup W5300
 *
 * @brief This supports the basic I/O functions for @ref WIZCHIP_register_W5300.
 *
 * - <b> Basic I/O function </b> \n
 *   WIZCHIP_READ(), WIZCHIP_WRITE() \n\n
 *
 * - @ref Common_register_group_W5300 <b>access functions</b> \n
 * 	-# @b Mode \n
 *    getMR(), setMR()
 * 	-# @b Interrupt \n
 *    getIR(), setIR(), getIMR(), setIMR(), getSIR(), setSIR(), getSIMR(), setSIMR()
 * 	-# <b> Network Information </b> \n
 *    getSHAR(), setSHAR(), getGAR(), setGAR(), getSUBR(), setSUBR(), getSIPR(), setSIPR()
 * 	-# @b Retransmission \n
 *    getRCR(), setRCR(), getRTR(), setRTR()
 * 	-# @b PPPoE \n
 *    getPTIMER(), setPTIMER(), getPMAGIC(), getPMAGIC(), getPSID(), setPSID(), getPHAR(), setPHAR(), getPMRU(), setPMRU()
 * 	-# <b> ICMP packet </b>\n
 *    getUIPR(), getUPORTR()
 *    -# @b Socket Memory \n
 *    getMTYPER(), setMTYPER() \n
 *    getTMS01R(), getTMS23R(), getTMS45R(), getTMS67R(), setTMS01R(), setTMS23R(), setTMS45R(), setTMS67R() \n
 *    getRMS01R(), getRMS23R(), getRMS45R(), getRMS67R(), setRMS01R(), setRMS23R(), setRMS45R(), setRMS67R() \n 
 * 	-# @b etc. \n
 *    getPn_BRDYR(), setPn_BRDYR(), getPn_BDPTHR(), setPn_BDPTHR(), getIDR() \n\n
 *
 * - \ref Socket_register_group_W5300 <b>access functions</b> \n
 *   -# <b> SOCKET control</b> \n
 *      getSn_MR(), setSn_MR(), getSn_CR(), setSn_CR(), getSn_IMR(), setSn_IMR(), getSn_IR(), setSn_IR()
 *   -# <b> SOCKET information</b> \n
 *      getSn_SR(), getSn_DHAR(), setSn_DHAR(), getSn_PORT(), setSn_PORT(), getSn_DIPR(), setSn_DIPR(), getSn_DPORT(), setSn_DPORT()
 *      getSn_MSSR(), setSn_MSSR()
 *   -# <b> SOCKET communication </b> \n
 *      getSn_RXBUF_SIZE(), setSn_RXBUF_SIZE(), getSn_TXBUF_SIZE(), setSn_TXBUF_SIZE() \n
 *      getSn_TX_RD(), getSn_TX_WR(), setSn_TX_WR() \n
 *      getSn_RX_RD(), setSn_RX_RD(), getSn_RX_WR() \n
 *      getSn_TX_FSR(), getSn_RX_RSR(), getSn_KPALVTR(), setSn_KPALVTR()
 *   -# <b> IP header field </b> \n
 *      getSn_FRAG(), setSn_FRAG(),  getSn_TOS(), setSn_TOS() \n
 *      getSn_TTL(), setSn_TTL()
 */


/**
 * @defgroup Common_register_group_W5300 Common register
 * @ingroup WIZCHIP_register_W5300
 *
 * @brief Common register group\n
 * It set the basic for the networking\n
 * It set the configuration such as interrupt, network information, ICMP, etc.
 * @details
 * @sa MR : Mode register.
 * @sa GAR, SUBR, SHAR, SIPR : Network Configuration
 * @sa IR, _IMR_ : Interrupt.
 * @sa MTYPER, TMS01R,TMS23R, TMS45R, TMS67R,RMS01R,RMS23R, RMS45R, RMS67R : Socket TX/RX memory
 * @sa _RTR_, _RCR_ : Data retransmission.
 * @sa PTIMER, PMAGIC, PSID, PDHAR : PPPoE.
 * @sa UIPR, UPORTR, FMTUR : ICMP message.
 * @sa Pn_BRDYR, Pn_BDPTHR, IDR : etc.
 */
  
 
/**
 * @defgroup Socket_register_group_W5300 Socket register
 * @ingroup WIZCHIP_register_W5300
 *
 * @brief Socket register group.\n
 * Socket register configures and control SOCKETn which is necessary to data communication.
 * @details
 * @sa Sn_MR, Sn_CR, Sn_IR, Sn_IMR : SOCKETn Control
 * @sa Sn_SR, Sn_PORT, Sn_DHAR, Sn_DIPR, Sn_DPORT : SOCKETn Information
 * @sa Sn_MSSR, Sn_TOS, Sn_TTL, Sn_KPALVTR, Sn_FRAG : Internet protocol.
 * @sa Sn_TX_WRSR, Sn_TX_FSR, Sn_TX_RD, Sn_TX_WR, Sn_RX_RSR, Sn_RX_RD, Sn_RX_WR, Sn_TX_FIFOR, Sn_RX_FIFOR : Data communication
 */
 
 
 /**
 * @defgroup Basic_IO_function_W5300 Basic I/O function
 * @ingroup WIZCHIP_IO_Functions_W5300
 * @brief These are basic input/output functions to read values from register or write values to register.
 */

/**
 * @defgroup Common_register_access_function_W5300 Common register access functions
 * @ingroup WIZCHIP_IO_Functions_W5300
 * @brief These are functions to access <b>common registers</b>.
 */

/**
 * @defgroup Socket_register_access_function_W5300 Socket register access functions
 * @ingroup WIZCHIP_IO_Functions_W5300
 * @brief These are functions to access <b>socket registers</b>.
 */

//------------------------------- defgroup end --------------------------------------------

//----------------------------- W5300 Common Registers -----------------------------
/**
 * @ingroup Common_register_group_W5300
 * @brief Mode Register address(R/W)\n
 * @ref MR is used for S/W reset, ping block mode, PPPoE mode and etc.
 * @details Each bit of @ref MR defined as follows.
 * <table>
 * 		<tr>  <td>15</td> <td>14</td> <td>13</td> <td>12</td> <td>11</td> <td>10</td> <td>9</td> <td>8</td>   </tr>
 * 		<tr>  <td>DBW</td> <td>MPF</td> <td colspan=3>WDF</td><td>RDF</td> <td>Reserved</td> <td>FS</td> 
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>RST</td> <td>Reserved</td> <td>WOL</td> <td>PB</td> <td>PPPoE</td> <td>Reserved</td> <td>FARP</td> <td>Reserved</td> </tr>
 * </table>
 * - \ref MR_DBW        : Data bus width (0 : 8 Bit, 1 : 16 Bit), Read Only
 * - \ref MR_MPF        : Received a Pause Frame from MAC layer (0 : Normal Frame, 1 : Pause Frame), Read Only
 * - \ref MR_WDF        : Write Data Fetch time (When CS signal is low, W5300 Fetch a written data by Host after PLL_CLK * MR_WDF)
 * - \ref MR_RDH        : Read Data Hold time (0 : No use data hold time, 1 : Use data hold time, 2 PLL_CLK)
 * - \ref MR_FS         : FIFO Swap (0 : Disable Swap, 1 : Enable Swap)
 * - \ref MR_RST		 	: Reset
 * - \ref MR_WOL       	: Wake on LAN
 * - \ref MR_PB         : Ping block
 * - \ref MR_PPPOE      : PPPoE mode
 * - \ref MR_FARP			: Force ARP mode
 */
#define MR              (_WIZCHIP_IO_BASE_)

/**
 * @ingroup Common_register_group_W5300
 * @brief Interrupt Register(R/W)
 * @details \ref IR indicates the interrupt status. Each bit of \ref IR will be still until the bit will be written to by the host.
 * If \ref IR is not equal to 0x0000 INTn PIN is asserted to low until it is 0x0000\n\n
 * Each bit of \ref IR defined as follows.
 * <table>
 * 		<tr>  <td>15</td> <td>14</td> <td>13</td> <td>12</td> <td>11</td> <td>10</td> <td>9</td> <td>8</td>   </tr>
 * 		<tr>  <td>IPCF</td> <td>DPUR</td> <td>PPPT</td> <td>FMTU</td> <td>Reserved</td> <td>Reserved</td> <td>Reserved</td> <td>Reserved</td> </tr>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>S7_INT</td> <td>S6_INT</td> <td>S5_INT</td> <td>S4_INT</td> <td>S3_INT</td> <td>S2_INT</td> <td>S1_INT</td> <td>S0_INT</td> </tr>
 * </table>
 * - \ref IR_IPCF : IP conflict
 * - \ref IR_DPUR	: Destination Port Unreachable
 * - \ref IR_PPPT : PPPoE Termination
 * - \ref IR_FMTU : Fragmented MTU 
 * - \ref IR_SnINT(n) : Interrupted from SOCKETn
 *
 * @note : In W5300, IR is operated same as IR and SIR in other WIZCHIP(5100,5200,W5500)
 */
#define IR              (_W5300_IO_BASE_ + 0x02)

/**
 * @ingroup Common_register_group_W5300
 * @brief Socket Interrupt Mask Register(R/W)
 * @details Each bit of \ref _IMR_ corresponds to each bit of \ref IR.
 * When a bit of _IMR_ is and the corresponding bit of \ref IR is  Interrupt will be issued.
 * In other words, if a bit of _IMR_, an interrupt will be not issued even if the corresponding bit of \ref IR is set
 * @note : In W5300, _IMR_ is operated same as _IMR_ and SIMR in other WIZCHIP(5100,5200,W5500)
 */
#define _IMR_             (_W5300_IO_BASE_ + 0x04)


//#define ICFGR           (_W5300_IO_BASE_ + 0x06)
//#define INTLEVEL        ICFGR

/**
 * @ingroup Common_register_group_W5300
 * @brief Source MAC Register address(R/W)
 * @details @ref SHAR configures the source hardware address.
 */
#define SHAR            (_W5300_IO_BASE_ + 0x08)


/**
 * @ingroup Common_register_group_W5300
 * @brief Gateway IP Register address(R/W)
 * @details @ref GAR configures the default gateway address.
 */
 #define GAR             (_W5300_IO_BASE_ + 0x10)

/**
 * @ingroup Common_register_group_W5300
 * @brief Subnet mask Register address(R/W)
 * @details @ref SUBR configures the subnet mask address.
 */
#define SUBR            (_W5300_IO_BASE_ + 0x14)

/**
 * @ingroup Common_register_group_W5300
 * @brief Source IP Register address(R/W)
 * @details @ref SIPR configures the source IP address.
 */
#define SIPR            (_W5300_IO_BASE_ + 0x18)

/**
 * @ingroup Common_register_group_W5300
 * @brief Timeout register address( 1 is 100us )(R/W)
 * @details @ref _RTR_ configures the retransmission timeout period. The unit of timeout period is 100us and the default of @ref _RTR_ is x07D0.
 * And so the default timeout period is 200ms(100us X 2000). During the time configured by @ref _RTR_, W5300 waits for the peer response
 * to the packet that is transmitted by \ref Sn_CR (CONNECT, DISCON, CLOSE, SEND, SEND_MAC, SEND_KEEP command).
 * If the peer does not respond within the @ref _RTR_ time, W5300 retransmits the packet or issues timeout.
 */
 #define _RTR_          (_W5300_IO_BASE_ + 0x1C)

/**
 * @ingroup Common_register_group_W5300
 * @brief Retry count register(R/W)
 * @details @ref _RCR_ configures the number of time of retransmission.
 * When retransmission occurs as many as ref _RCR_+1 Timeout interrupt is issued (@ref Sn_IR_TIMEOUT = '1').
 */
#define _RCR_           (_W5300_IO_BASE_ + 0x1E)

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 0 & 1
 * @details TMS01R configures the TX buffer block size of \c SOCKET 0 & 1.  The default value is configured with 8KB and can be configure from 0 to 64KB with unit 1KB.
 * But the sum of all SOCKET TX buffer size should be multiple of 8 and the sum of all SOCKET TX and RX memory size can't exceed 128KB.
 * When exceeded nor multiple of 8, the data transmittion is invalid.
 */
#define TMS01R          (_W5300_IO_BASE_ + 0x20)

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 2 & 3
 * @details refer to \ref TMS01R
 */
#define TMS23R          (TMS01R + 2) 

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 4 & 5
 * @details refer to \ref TMS01R
 */
#define TMS45R          (TMS01R + 4)

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 6 & 7
 * @details refer to \ref TMS01R
 */
#define TMS67R          (TMS01R + 6) 

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 0.
 * @details refer to \ref TMS01R
 */
#define TMSR0           TMS01R

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 1.
 * @details refer to \ref TMS01R
 */
#define TMSR1           (TMSR0 + 1)

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 2.
 * @details refer to \ref TMS01R
 */
#define TMSR2           (TMSR0 + 2)

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 3.
 * @details refer to \ref TMS01R
 */
#define TMSR3           (TMSR0 + 3)

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 4.
 * @details refer to \ref TMS01R
 */
#define TMSR4           (TMSR0 + 4)

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 5.
 * @details refer to \ref TMS01R
 */
#define TMSR5           (TMSR0 + 5)

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 6.
 * @details refer to \ref TMS01R
 */
#define TMSR6           (TMSR0 + 6)

/**
 * @ingroup Common_register_group_W5300
 * @brief   TX memory size of \c SOCKET 7.
 * @details refer to \ref TMS01R
 */
#define TMSR7           (TMSR0 + 7)


/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 0 & 1
 * @details RMS01R configures the RX buffer block size of \c SOCKET 0 & 1.  The default value is configured with 8KB and can be configure from 0 to 64KB with unit 1KB.
 * But the sum of all SOCKET RX buffer size should be multiple of 8 and the sum of all SOCKET RX and TX memory size can't exceed 128KB.
 * When exceeded nor multiple of 8, the data reception is invalid.
 */
#define RMS01R          (_W5300_IO_BASE_ + 0x28)

/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 2 & 3
 * @details Refer to \ref RMS01R
 */
#define RMS23R          (RMS01R + 2)   

/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 4 & 5
 * @details Refer to \ref RMS01R
 */
#define RMS45R          (RMS01R + 4)  

/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 6 & 7
 * @details Refer to \ref RMS01R
 */
#define RMS67R          (RMS01R + 6)   

/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 0.
 * @details refer to \ref RMS01R
 */
#define RMSR0           RMS01R

/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 1.
 * @details refer to \ref RMS01R
 */
#define RMSR1           (RMSR0 + 1)

/**
 * @ingroup Common_register_group_5300
 * @brief   RX memory size of \c SOCKET 2.
 * @details refer to \ref RMS01R
 */
#define RMSR2           (RMSR0 + 2)

/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 3.
 * @details refer to \ref RMS01R
 */
#define RMSR3           (RMSR0 + 3)

/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 4.
 * @details refer to \ref RMS01R
 */
#define RMSR4           (RMSR0 + 4)

/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 5.
 * @details refer to \ref RMS01R
 */
#define RMSR5           (RMSR0 + 5)

/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 6.
 * @details refer to \ref RMS01R
 */
#define RMSR6           (RMSR0 + 6)

/**
 * @ingroup Common_register_group_W5300
 * @brief   RX memory size of \c SOCKET 7.
 * @details refer to \ref RMS01R
 */
#define RMSR7           (RMSR0 + 7)



/**
 *  @ingroup Common_register_group_W5300
 *  @brief Memory Type Register
 *  @details W5300’s 128Kbytes data memory (Internal TX/RX memory) is composed of 16 memory blocks
 *  of 8Kbytes. MTYPER configures type of each 8KB memory block in order to select RX or TX memory. 
 *  The type of 8KB memory block corresponds to each bit of MTYPER. When the bit is ‘1’, it is used as TX
 *  memory, and the bit is ‘0’, it is used as RX memory. MTYPER is configured as TX memory type
 *  from the lower bit. The rest of the bits not configured as TX memory, should be set as ‘0’.
 */
#define MTYPER          (_W5300_IO_BASE_ + 0x30)

/**
 *  @ingroup Common_register_group_W5300
 *  @brief PPPoE Authentication Type register
 *  @details It notifies authentication method negotiated with PPPoE server.
 *  W5300 supports 2 types of authentication methods. 
 *  - PAP  : 0xC023
 *  - CHAP : 0xC223
 */
#define PATR            (_W5300_IO_BASE_ + 0x32)

//#define PPPALGOR      (_W5300_IO_BASE_ + 0x34)

/**
 *  @ingroup Common_register_group_W5300
 *  @brief PPP Link Control Protocol Request Timer Register
 *  @details It configures transmitting timer of link control protocol (LCP) echo request. Value 1 is about 25ms.
 */
#define PTIMER          (_W5300_IO_BASE_ + 0x36)

/**
 *  @ingroup Common_register_group_W5300
 *  @brief PPP LCP magic number register
 *  @details It configures byte value to be used for 4bytes “Magic Number” during LCP negotiation with PPPoE server.
 */
#define PMAGICR         (_W5300_IO_BASE_ + 0x38)

//#define PSTATER       (_W5300_IO_BASE_ + 0x3A)

/**
 *  @ingroup Common_register_group_W5300
 *  @brief PPPoE session ID register
 *  @details It notifies PPP session ID to be used for communication with PPPoE server (acquired by PPPoE-process of W5300).
 */
#define PSIDR           (_W5300_IO_BASE_ + 0x3C)

/**
 *  @ingroup Common_register_group_W5300
 *  @brief PPPoE destination hardware address register
 *  @details It notifies hardware address of PPPoE server (acquired by PPPoE-process of W5300).
 */
#define PDHAR           (_W5300_IO_BASE_ + 0x40)

/**
 *  @ingroup Common_register_group_W5300 
 *  @brief Unreachable IP address register 
 *  @details When trying to transmit UDP data to destination port number which is not open, 
 *   W5300 can receive ICMP (Destination port unreachable) packet. \n
 *   In this case, \ref IR_DPUR bit of \ref IR becomes '1'. 
 *   And destination IP address and unreachable port number of ICMP packet can be acquired through UIPR and \ref UPORTR.
 */
#define UIPR            (_W5300_IO_BASE_ + 0x48)

/**
 *  @ingroup Common_register_group_W5300 
 *  @brief Unreachable port number register
 *  @details Refer to \ref UIPR.
 */
#define UPORTR          (_W5300_IO_BASE_ + 0x4C)

/**
 *  @ingroup Common_register_group_W5300 
 *  @brief Fragment MTU register
 *  @details When communicating with the peer having a different MTU, W5300 can receive an ICMP(Fragment MTU) packet. 
 *  At this case, IR(FMTU) becomes ‘1’ and destination IP address and fragment MTU value of ICMP packet can be acquired through UIPR and FMTUR. 
 *  In order to keep communicating with the peer having Fragment MTU, set the FMTUR first in Sn_MSSR of the SOCKETn, and try the next communication. 
 */
#define FMTUR           (_W5300_IO_BASE_ + 0x4E)

//#define Sn_RTCR(n)      (_W5300_IO_BASE_ + 0x50 + n*2)

/**
 *  @ingroup Common_register_group_W5300 
 *  @brief PIN 'BRDYn' configure register
 *  @details It configures the PIN "BRDYn" which is monitoring TX/RX memory status of the specified SOCKET. 
 *  If the free buffer size of TX memory is same or bigger than the buffer depth of \ref Pn_BDPTHR, 
 *  or received buffer size of RX memory is same or bigger than the \ref Pn_BDPTHR, 
 *  PIN "BRDYn" is signaled.
 * <table>
 * 		<tr>  <td>15</td> <td>14</td> <td>13</td> <td>12</td> <td>11</td> <td>10</td> <td>9</td> <td>8</td>   </tr>
 * 		<tr>  <td colspan=8>Reserved, Read as 0</td> </tr>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td> </tr>
 * 		<tr>  <td>PEN</td> <td>MT</td> <td>PPL</td> <td colspan=2>Reserved</td> <td colspan=3>SN</td> </tr>
 * </table>
 *
 * - \ref Pn_PEN           Enable PIN 'BRDYn' (0 : Disable, 1 : Enable)
 * - \ref Pn_MT            Monitoring Memory type (0 : RX memory, 1 : TX Memory)
 * - \ref Pn_PPL           PIN Polarity bit of Pn_BRDYR. (0 : Low sensitive, 1 : High sensitive)
 * - \ref Pn_SN(n)         Monitoring SOCKET number of Pn_BRDYR
 */
#define Pn_BRDYR(n)     (_W5300_IO_BASE_ + 0x60 + n*4)

/**
 *  @ingroup Common_register_group_W5300 
 *  @brief PIN 'BRDYn' buffer depth Register
 *  @details It configures buffer depth of PIN "BRDYn". 
 *  When monitoring TX memory and \ref Sn_TX_FSR is same or bigger than Pn_BDPTHR, the PIN "BRDYn" is signaled. 
 *  When monitoring RX memory and if \ref Sn_RX_RSR is same or bigger than Pn_BDPTHR, PIN "BRDYn" is signaled. 
 *  The value for Pn_BDPTHR can't exceed TX/RX memory size allocated by TMSR or RMSR such like as \ref TMS01R or \ref RMS01R.
 */
#define Pn_BDPTHR(n)    (_W5300_IO_BASE_ + 0x60 + n*4 + 2)

/**
 *  @ingroup Common_register_group_W5300 
 *  @brief W5300 identification register.
 *  @details Read Only. 0x5300.
 */
#define IDR             (_W5300_IO_BASE_ + 0xFE)
#define VERSIONR        IDR


//----------------------------- W5300 SOCKET Registers -----------------------------

/**
 * @ingroup Socket_register_group_W5300
 * @brief Socket Mode register(R/W)
 * @details @ref Sn_MR configures the option or protocol type of Socket n.\n\n
 * Each bit of @ref Sn_MR defined as the following.
 * <table>
 * 		<tr>  <td>15</td> <td>14</td> <td>13</td> <td>12</td> <td>11</td> <td>10</td> <td>9</td> <td>8</td>   </tr>
 * 		<tr>  <td colspan=7> Reserved. Read as 0 </td> <td>ALIGN</td> </tr>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>MULTI</td> <td>MF</td> <td>ND/IGMPv</td> <td>Reserved</td> <td colspan=4>PROTOCOL[3:0]</td> </tr>
 * </table>
 * - @ref Sn_MR_ALIGN   : Alignment bit of Sn_MR, Only valid in \ref Sn_MR_TCP. (C0 : Include TCP PACK_INFO, 1 : Not include TCP PACK_INFO)
 * - @ref Sn_MR_MULTI	: Support UDP Multicasting
 * - @ref Sn_MR_MF	   : Enable MAC Filter (0 : Disable, 1 - Enable), When enabled, W5300 can receive only both own and broadcast packet.
 * - @ref Sn_MR_ND		: No Delayed Ack(TCP) flag
 * - @ref Sn_MR_IGMPv 	: IGMP version used <b>in UDP mulitcasting</b>. (0 : Version 2, 1 : Version 2)
 * - <b>PROTOCOL[3:0]</b>
 * <table>
 * 		<tr>   <td><b>Protocol[3]</b></td> <td><b>Protocol[2]</b></td> <td><b>Protocol[1]</b></td> <td><b>Protocol[0]</b></td> <td>@b Meaning</td>   </tr>
 * 		<tr>   <td>0</td> <td>0</td> <td>0</td> <td>0</td> <td>Closed</td>   </tr>
 * 		<tr>   <td>0</td> <td>0</td> <td>0</td> <td>1</td> <td>TCP</td>   </tr>
 * 		<tr>   <td>0</td> <td>0</td> <td>1</td> <td>0</td> <td>UDP</td>   </tr>
 * 		<tr>   <td>0</td> <td>0</td> <td>1</td> <td>1</td> <td>IPCRAW</td>   </tr>
 *       <tr>   <td>0</td> <td>1</td> <td>0</td> <td>0</td> <td>MACRAW</td>   </tr>
 *       <tr>   <td>0</td> <td>1</td> <td>0</td> <td>1</td> <td>PPPoE</td>   </tr>
 * </table>
 *
 *  - @ref Sn_MR_PPPoE  : PPPoE 
 *	 - @ref Sn_MR_MACRAW	: MAC LAYER RAW SOCK
 *  - @ref Sn_MR_IPRAW  : IP LAYER RAW SOCK 
 *  - @ref Sn_MR_UDP		: UDP
 *  - @ref Sn_MR_TCP		: TCP
 *  - @ref Sn_MR_CLOSE	: Unused socket
 *  @note MACRAW mode should be only used in Socket 0.
 */
#define Sn_MR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x00)

/**
 * @ingroup Socket_register_group_W5300
 * @brief Socket command register(R/W)
 * @details This is used to set the command for Socket n such as OPEN, CLOSE, CONNECT, LISTEN, SEND, and RECEIVE.\n
 * After W5500 accepts the command, the @ref Sn_CR register is automatically cleared to 0x00.
 * Even though @ref Sn_CR is cleared to 0x00, the command is still being processed.\n
 * To check whether the command is completed or not, please check the @ref Sn_IR or @ref Sn_SR.
 * - @ref Sn_CR_OPEN 		: Initialize or open socket.
 * - @ref Sn_CR_LISTEN 		: Wait connection request in TCP mode(<b>Server mode</b>)
 * - @ref Sn_CR_CONNECT 	: Send connection request in TCP mode(<b>Client mode</b>)
 * - @ref Sn_CR_DISCON 		: Send closing request in TCP mode.
 * - @ref Sn_CR_CLOSE   	: Close socket.
 * - @ref Sn_CR_SEND    	: Update TX buffer pointer and send data.
 * - @ref Sn_CR_SEND_MAC	: Send data with MAC address, so without ARP process.
 * - @ref Sn_CR_SEND_KEEP 	: Send keep alive message.
 * - @ref Sn_CR_RECV		   : Update RX buffer pointer and receive data.
 * - @ref Sn_CR_PCON       : PPPoE connection begins by transmitting PPPoE discovery packet.
 * - @ref Sn_CR_PDISCON    : Closes PPPoE connection.
 * - @ref Sn_CR_PCR        : In each phase, it transmits REQ message.
 * - @ref Sn_CR_PCN        : In each phase, it transmits NAK message.
 * - @ref Sn_CR_PCJ        : In each phase, it transmits REJECT message.
 */
#define Sn_CR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x02)

/**
 * @ingroup Socket_register_group_W5300
 * @brief socket interrupt mask register(R)
 * @details @ref Sn_IMR masks the interrupt of Socket n.
 * Each bit corresponds to each bit of @ref Sn_IR. When a Socket n Interrupt is occurred and the corresponding bit of @ref Sn_IMR is 
 * the corresponding bit of @ref Sn_IR becomes  When both the corresponding bit of @ref Sn_IMR and @ref Sn_IR are and the n-th bit of @ref IR is 
 * Host is interrupted by asserted INTn PIN to low.
 */
#define Sn_IMR(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x04)

/**
 * @ingroup Socket_register_group_W5300
 * @brief Socket interrupt register(R)
 * @details @ref Sn_IR indicates the status of Socket Interrupt such as establishment, termination, receiving data, timeout).\n
 * When an interrupt occurs and the corresponding bit of @ref Sn_IMR is  the corresponding bit of @ref Sn_IR becomes \n
 * In order to clear the @ref Sn_IR bit, the host should write the bit to \n
 * <table>
 * 		<tr>  <td>15</td> <td>14</td> <td>13</td> <td>12</td> <td>11</td> <td>10</td> <td>9</td> <td>8</td>   </tr>
 * 		<tr>  <td colspan=8> Reserved. Read as 0</td> </tr>
 * 		<tr>  <td>7</td> <td>6</td> <td>5</td> <td>4</td> <td>3</td> <td>2</td> <td>1</td> <td>0</td>   </tr>
 * 		<tr>  <td>PRECV</td> <td>PFAIL</td> <td>PNEXT</td> <td>SENDOK</td> <td>TIMEOUT</td> <td>RECV</td> <td>DISCON</td> <td>CON</td> </tr>
 * </table>
 * - \ref Sn_IR_PRECV   : PPP receive
 * - \ref Sn_IR_PFAIL   : PPP fail
 * - \ref Sn_IR_PNEXT   : PPP next phase
 * - \ref Sn_IR_SENDOK  : SENDOK 
 * - \ref Sn_IR_TIMEOUT : TIMEOUT
 * - \ref Sn_IR_RECV    : RECV 
 * - \ref Sn_IR_DISCON  : DISCON
 * - \ref Sn_IR_CON     : CON
 */
#define Sn_IR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x06)

/**
 * @ingroup Socket_register_group_W5300
 * @brief Socket status register(R)
 * @details @ref Sn_SSR indicates the status of Socket n.\n
 * The status of Socket n is changed by @ref Sn_CR or some special control packet as SYN, FIN packet in TCP.
 * @par Normal status
 * - @ref SOCK_CLOSED 		: Closed
 * - @ref SOCK_INIT   		: Initiate state
 * - @ref SOCK_LISTEN    	: Listen state
 * - @ref SOCK_ESTABLISHED : Success to connect
 * - @ref SOCK_CLOSE_WAIT  : Closing state
 * - @ref SOCK_UDP   		: UDP socket
 * - @ref SOCK_IPRAW       : IPRAW socket
 * - @ref SOCK_MACRAW  		: MAC raw mode socket
 * - @ref SOCK_PPPoE       : PPPoE mode Socket
 *@par Temporary status during changing the status of Socket n.
 * - @ref SOCK_SYNSENT   	: This indicates Socket n sent the connect-request packet (SYN packet) to a peer.
 * - @ref SOCK_SYNRECV    	: It indicates Socket n successfully received the connect-request packet (SYN packet) from a peer.
 * - @ref SOCK_FIN_WAIT		: Connection state
 * - @ref SOCK_CLOSING		: Closing state
 * - @ref SOCK_TIME_WAIT	: Closing state
 * - @ref SOCK_LAST_ACK 	: Closing state
 * - @ref SOCK_ARP         : ARP request state 
 */
#define Sn_SSR(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x08)
#define Sn_SR(n)        Sn_SSR(n)   ///< For Compatible ioLibrary. Refer to @ref Sn_SSR(n)

/**
 * @ingroup Socket_register_group_W5300
 * @brief source port register(R/W)
 * @details @ref Sn_PORTR configures the source port number of Socket n.
 * It is valid when Socket n is used in TCP/UPD mode. It should be set before OPEN command is ordered.
 */
#define Sn_PORTR(n)     (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x0A)
#define Sn_PORT(n)      Sn_PORTR(n)       ///< For compatible ioLibrary. Refer to @ref Sn_PORTR(n).

/**
 * @ingroup Socket_register_group_W5300
 * @brief Peer MAC register address(R/W)
 * @details @ref Sn_DHAR configures the destination hardware address of Socket n when using SEND_MAC command in UDP mode or
 * it indicates that it is acquired in ARP-process by CONNECT/SEND command.
 */
#define Sn_DHAR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x0C)

/**
 * @ingroup Socket_register_group_W5300
 * @brief Peer port register address(R/W)
 * @details @ref Sn_DPORTR configures or indicates the destination port number of Socket n. It is valid when Socket n is used in TCP/UDP mode.
 * In TCP clientmode, it configures the listen port number of TCP serverbefore CONNECT command.
 * In TCP Servermode, it indicates the port number of TCP client after successfully establishing connection.
 * In UDP mode, it configures the port number of peer to be transmitted the UDP packet by SEND/SEND_MAC command.
 */
#define Sn_DPORTR(n)    (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x12)
#define Sn_DPORT(n)     Sn_DPORTR(n)    ///< For compatible ioLibrary. Refer to \ref Sn_DPORTR.


/**
 * @ingroup Socket_register_group_W5300
 * @brief Peer IP register address(R/W)
 * @details @ref Sn_DIPR configures or indicates the destination IP address of Socket n. It is valid when Socket n is used in TCP/UDP mode.
 * In TCP client mode, it configures an IP address of TCP serverbefore CONNECT command.
 * In TCP server mode, it indicates an IP address of TCP clientafter successfully establishing connection.
 * In UDP mode, it configures an IP address of peer to be received the UDP packet by SEND or SEND_MAC command.
 */
 #define Sn_DIPR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x14)

/**
 * @ingroup Socket_register_group_W5300
 * @brief Maximum Segment Size(Sn_MSSR0) register address(R/W)
 * @details @ref Sn_MSSR configures or indicates the MTU(Maximum Transfer Unit) of Socket n.
 */
#define Sn_MSSR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x18)

/**
 * @ingroup Socket_register_group_W5300
 * @brief Keep Alive Timer register(R/W)
 * @details @ref Sn_KPALVTR configures the transmitting timer of KEEP ALIVE(KA)packet of SOCKETn. It is valid only in TCP mode,
 * and ignored in other modes. The time unit is 5s.
 * KA packet is transmittable after @ref Sn_SR is changed to SOCK_ESTABLISHED and after the data is transmitted or received to/from a peer at least once.
 * In case of '@ref Sn_KPALVTR > 0', W5500 automatically transmits KA packet after time-period for checking the TCP connection (Auto-keepalive-process).
 * In case of '@ref Sn_KPALVTR = 0', Auto-keep-alive-process will not operate,
 * and KA packet can be transmitted by SEND_KEEP command by the host (Manual-keep-alive-process).
 * Manual-keep-alive-process is ignored in case of '@ref Sn_KPALVTR > 0'.
 */
#define Sn_KPALVTR(n)   (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x1A)

/**
 * @ingroup Socket_register_group_W5300
 * @brief IP Protocol(PROTO) Register(R/W)
 * @details \ref Sn_PROTO that sets the protocol number field of the IP header at the IP layer. It is
 * valid only in IPRAW mode, and ignored in other modes.
 */
#define Sn_PROTOR(n)    Sn_KPALVTR(n)


/**
 * @ingroup Socket_register_group_W5300
 * @brief IP Type of Service(TOS) Register(R/W)
 * @details @ref Sn_TOSR configures the TOS(Type Of Service field in IP Header) of Socket n.
 * It is set before OPEN command.
 */
#define Sn_TOSR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x1C)
#define Sn_TOS(n)       Sn_TOSR(n)  ///< For compatible ioLibrary. Refer to Sn_TOSR

/**
 * @ingroup Socket_register_group_W5300
 * @brief IP Time to live(TTL) Register(R/W)
 * @details @ref Sn_TTLR configures the TTL(Time To Live field in IP header) of Socket n.
 * It is set before OPEN command.
 */
#define Sn_TTLR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x1E)
#define Sn_TTL(n)       Sn_TTLR(n)  ///< For compatible ioLibrary. Refer to Sn_TTLR

/**
 * @ingroup Socket_register_group_W5300
 * @brief  SOCKETn TX write size register(R/W)
 * @details It sets the byte size of the data written in internal TX memory through @ref Sn_TX_FIFOR.
 * It is set before SEND or SEND_MAC command, and can't be bigger than internal TX memory
 * size set by TMSR such as @ref TMS01R, TMS23R and etc.
 */
#define Sn_TX_WRSR(n)		(_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x20)

/**
 * @ingroup Socket_register_group_W5300
 * @brief Transmit free memory size register(R)
 * @details Sn_TX_FSR indicates the free size of Socket n TX Buffer Block. It is initialized to the configured size by TMSR such as @ref TMS01SR.
 * Data bigger than Sn_TX_FSR should not be saved in the Socket n TX Buffer because the bigger data overwrites the previous saved data not yet sent.
 * Therefore, check before saving the data to the Socket n TX Buffer, and if data is equal or smaller than its checked size,
 * transmit the data with SEND/SEND_MAC command after saving the data in Socket n TX buffer. But, if data is bigger than its checked size,
 * transmit the data after dividing into the checked size and saving in the Socket n TX buffer.
 */
#define Sn_TX_FSR(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x0024)

/**
 * @ingroup Socket_register_group_w5300
 * @brief Received data size register(R)
 * @details @ref Sn_RX_RSR indicates the data size received and saved in Socket n RX Buffer.
 * @ref Sn_RX_RSR does not exceed the RMSR such as @ref RMS01SR  and is calculated as the difference between
 * ?Socket n RX Write Pointer (@ref Sn_RX_WR)and Socket n RX Read Pointer (@ref Sn_RX_RD)
 */
#define Sn_RX_RSR(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x0028)

/**
 * @ingroup Socket_register_group_W5300
 * @brief Fragment field value in IP header register(R/W)
 * @details @ref Sn_FRAGR configures the FRAG(Fragment field in IP header).
 */
#define Sn_FRAGR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x002C)
#define Sn_FRAG(n)         Sn_FRAGR(n)

/**
 * @ingroup Socket_register_group_W5300
 * @brief SOCKET n TX FIFO regsiter
 * @details It indirectly accesses internal TX memory of SOCKETn.
 * The internal TX memory can't be accessed directly by the host, but can be accessed through Sn_TX_FIFOR. 
 * If @ref MR(MT) = '0', only the Host-Write of internal TX memory is allowed through Sn_TX_FIFOR. 
 * But if @ref MR(MT) is '1', both of Host-Read and Host-Write are allowed.
 */
#define Sn_TX_FIFOR(n)     (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x2E)

/**
 * @ingroup Socket_register_group_W5300
 * @brief SOCKET n RX FIFO register
 * @details It indirectly accesses to internal RX memory of SOCKETn.
 * The internal RX memory can't be directly accessed by the host, but can be accessed through Sn_RX_FIFOR. 
 * If MR(MT) = '0', only the Host-Read of internal RX memory is allowed through Sn_RX_FIFOR. 
 * But if MR(MT) is '1', both of Host-Read and Host-Write are allowed. 
 */
#define Sn_RX_FIFOR(n)     (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x30)

//#define Sn_TX_SADR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x32)

//#define Sn_RX_SADR(n)      (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x34)

//#define Sn_TX_RD(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x36)

//#define Sn_TX_WR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x38)

//#define Sn_TX_ACK(n)       (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x3A)

//#define Sn_RX_RD(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x3C)

//#define Sn_RX_WR(n)        (_W5300_IO_BASE_ + WIZCHIP_SREG_BLOCK(n) + 0x3E)


/************************************/
/* The bit of MR regsiter defintion */
/************************************/
#define MR_DBW             (1 << 15)            /**< Data bus width bit of \ref MR. Read Only. (0 : 8Bit, 1 : 16Bit)*/
#define MR_MPF             (1 << 14)            /**< Mac layer pause frame bit of \ref MR. (0 : Disable, 1 : Enable)*/
#define MR_WDF(X)          ((X & 0x07) << 11)   /**< Write data fetch time bit of  \ref MR. Fetch Data from DATA bus after PLL_CLK * MR_WDF[2:0]*/
#define MR_RDH             (1 << 10)            /**< Read data hold time bit of \ref MR. Hold Data on DATA bus during 2 * PLL_CLK after CS high*/
#define MR_FS              (1 << 8)             /**< FIFO swap bit of \ref MR. Swap MSB & LSB of \ref Sn_TX_FIFOR & Sn_RX_FIFOR (0 : No swap, 1 : Swap) */
#define MR_RST             (1 << 7)             /**< S/W reset bit of \ref MR. (0 : Normal Operation,  1 : Reset (automatically clear after reset))*/
#define MR_MT              (1 << 5)             /**< Memory test bit of \ref MR. (0 : Normal, 1 : Internal Socket memory write & read Test)*/
#define MR_PB              (1 << 4)             /**< Ping block bit of \ref MR. (0 : Unblock, 1 : Block)*/
#define MR_PPPoE           (1 << 3)             /**< PPPoE bit of \ref MR. (0 : No use PPPoE, 1: Use PPPoE)*/
#define MR_DBS             (1 << 2)             /**< Data bus swap of \ref MR. Valid only 16bit mode (0 : No swap, 1 : Swap)*/
#define MR_IND             (1 << 0)             /**< Indirect mode bit of \ref MR. (0 : Direct mode, 1 : Indirect mode) */


/************************************/ 
/* The bit of IR regsiter definition */ 
/************************************/ 
#define IR_IPCF            (1 << 7)             /**< IP conflict bit of \ref IR. To clear, Write the bit to '1'. */
#define IR_DPUR            (1 << 6)             /**< Destination port unreachable bit of \ref IR. To clear, Write the bit to '1'. */
#define IR_PPPT            (1 << 5)             /**< PPPoE terminate bit of \ref IR. To clear, Write the bit to '1'. */
#define IR_FMTU            (1 << 4)             /**< Fragment MTU bit of IR. To clear, Write the bit to '1'. */
#define IR_SnINT(n)        (0x01 << n)          /**< SOCKETn interrupt occurrence bit of \ref IR. To clear, Clear \ref Sn_IR*/

/*****************************************/ 
/* The bit of Pn_BRDYR regsiter definition*/ 
/*****************************************/ 
#define Pn_PEN             (1 << 7)             /**< PIN 'BRDYn' enable bit of Pn_BRDYR. */
#define Pn_MT              (1 << 6)             /**< PIN memory type bit of Pn_BRDYR. */
#define Pn_PPL             (1 << 5)             /**< PIN Polarity bit of Pn_BRDYR. */
#define Pn_SN(n)           ((n & 0x07) << 0)    /**< What socket to monitor. */


/***************************************/ 
/* The bit of Sn_MR regsiter definition */ 
/***************************************/ 
/**
 * @brief Alignment bit of \ref Sn_MR. 
 * @details It is valid only in the TCP (\ref Sn_MR_TCP) with TCP communication, 
 * when every the received DATA packet size is of even number and set as '1', 
 * data receiving performance can be improved by removing PACKET-INFO(data size) that is attached to every the received DATA packet.
 */
#define Sn_MR_ALIGN        (1 << 8)     

/**
 * @brief Multicasting bit of \ref Sn_MR
 * @details It is valid only in UDP (\ref Sn_MR_UDP).
 * In order to implement multicasting, set the IP address and port number in @ref Sn_DIPR and @ref Sn_DPORTR respectively before "OPEN" command(@ref Sn_CR_OPEN).\n
 * 0 : Disable, 1 : Enable
 */
#define Sn_MR_MULTI        (1 << 7)     

/**
 * @brief  MAC filter bit of \ref Sn_MR
 * @details It is valid in MACRAW(@ref Sn_MR_MACRAW).
 * When this bit is set as ‘1’, W5300 can receive packet that is belong in itself or broadcasting. 
 * When this bit is set as ‘0’, W5300 can receive all packets on Ethernet. 
 * When using the hybrid TCP/IP stack, it is recommended to be set as ‘1’ for reducing the receiving overhead of host. \n
 * 0 : Disable, 1 : Enable
 */
#define Sn_MR_MF           (1 << 6)             

/**
 * @brief IGMP version bit of \ref Sn_MR
 * details It is valid in case of @ref Sn_MR_MULTI='1' and UDP(@ref Sn_MR_UDP).
 * It configures IGMP version to send IGMP message such as <b>Join/Leave/Report</b> to multicast-group. \n
 * 0 : IGMPv2, 1 : IGMPv1
 */
#define Sn_MR_IGMPv        (1 << 5)
#define Sn_MR_MC           Sn_MR_IGMPv       ///< For compatible ioLibrary

/**
 * @brief No delayed ack bit of \ref Sn_MR
 * @details It is valid in TCP(@ref Sn_MR_TCP).
 * In case that it is set as '1', ACK packet is transmitted right after receiving DATA packet from the peer. 
 * It is recommended to be set as '1' for TCP performance improvement.
 * In case that it is set as '0', ACK packet is transmitted after the time set in @ref _RTR_ regardless of DATA packet receipt.\n
 * 0 : No use, 1 : Use 
 */
#define Sn_MR_ND           (1 << 5)             

/**
 * @brief No mode
 * @details This configures the protocol mode of Socket n.
 * @sa Sn_MR
 */
#define Sn_MR_CLOSE        0x00                 

/**
 * @brief TCP mode 
 * @details This configures the protocol mode of Socket n.
 * @sa Sn_MR
 */
#define Sn_MR_TCP          0x01                

/**
 * @brief UDP mode
 * @details This configures the protocol mode of Socket n.
 * @sa Sn_MR
 */
#define Sn_MR_UDP          0x02                 /**< Protocol bits of \ref Sn_MR. */

/**
 * @brief IP LAYER RAW mode
 * @details This configures the protocol mode of Socket n.
 * @sa Sn_MR 
 */
#define Sn_MR_IPRAW        0x03                 /**< Protocol bits of \ref Sn_MR. */

/**
 * @brief MAC LAYER RAW mode
 * @details This configures the protocol mode of Socket 0.
 * @sa Sn_MR 
 * @note MACRAW mode should be only used in Socket 0.
 */
#define Sn_MR_MACRAW       0x04 

/**
 * @brief PPPoE mode
 * @details This configures the protocol mode of Socket 0.
 * @sa Sn_MR 
 * @note PPPoE mode should be only used in Socket 0.
 */                
#define Sn_MR_PPPoE        0x05                 /**< Protocol bits of \ref Sn_MR. */

#define SOCK_STREAM        Sn_MR_TCP            /**< For Berkeley Socket API, Refer to @ref Sn_MR_TCP */
#define SOCK_DGRAM         Sn_MR_UDP            /**< For Berkeley Socket API, Refer to @ref Sn_MR_UDP */



/******************************/ 
/* The values of CR definition */ 
/******************************/
/**
 * @brief Initialize or open a socket
 * @details Socket n is initialized and opened according to the protocol selected in Sn_MR(P3:P0).
 * The table below shows the value of @ref Sn_SR corresponding to @ref Sn_MR.\n
 * <table>
 *   <tr>  <td>\b Sn_MR (P[3:0])</td> <td>\b Sn_SR</td>            		 </tr>
 *   <tr>  <td>Sn_MR_CLOSE  (000)</td> <td></td>         	   		 </tr>
 *   <tr>  <td>Sn_MR_TCP  (001)</td> <td>SOCK_INIT (0x13)</td>  		 </tr>
 *   <tr>  <td>Sn_MR_UDP  (010)</td>  <td>SOCK_UDP (0x22)</td>  		 </tr>
 *   <tr>  <td>Sn_MR_IPRAW  (010)</td>  <td>SOCK_IPRAW (0x32)</td>  		 </tr>
 *   <tr>  <td>Sn_MR_MACRAW  (100)</td>  <td>SOCK_MACRAW (0x42)</td>  </tr>
 *   <tr>  <td>Sn_MR_PPPoE  (101)</td>  <td>SOCK_PPPoE (0x5F)</td>  </tr>
 * </table>
 */
#define Sn_CR_OPEN         0x01                 

/**
 * @brief Wait connection request in TCP mode(Server mode)
 * @details This is valid only in TCP mode (\ref Sn_MR(P3:P0) = \ref Sn_MR_TCP).
 * In this mode, Socket n operates as a TCP serverand waits for  connection-request (SYN packet) from any TCP client
 * The @ref Sn_SR changes the state from \ref SOCK_INIT to \ref SOCKET_LISTEN.
 * When a TCP clientconnection request is successfully established,
 * the @ref Sn_SR changes from SOCK_LISTEN to SOCK_ESTABLISHED and the @ref Sn_IR(0) becomes 
 * But when a TCP clientconnection request is failed, @ref Sn_IR(3) becomes and the status of @ref Sn_SR changes to SOCK_CLOSED.
 */
#define Sn_CR_LISTEN       0x02                 

/**
 * @brief Send connection request in TCP mode(Client mode)
 * @details  To connect, a connect-request (SYN packet) is sent to <b>TCP server</b>configured by @ref Sn_DIPR & Sn_DPORT(destination address & port).
 * If the connect-request is successful, the @ref Sn_SR is changed to @ref SOCK_ESTABLISHED and the Sn_IR(0) becomes \n\n
 * The connect-request fails in the following three cases.\n
 * 1. When a @b ARPTO occurs (@ref Sn_IR[3] =  '1') because destination hardware address is not acquired through the ARP-process.\n
 * 2. When a @b SYN/ACK packet is not received and @b TCPTO (Sn_IR(3) =  )\n
 * 3. When a @b RST packet is received instead of a @b SYN/ACK packet. In these cases, @ref Sn_SR is changed to @ref SOCK_CLOSED.
 * @note This is valid only in TCP mode and operates when Socket n acts as <b>TCP client</b>
 */
#define Sn_CR_CONNECT      0x04                 

/**
 * @brief Send closing request in TCP mode
 * @details Regardless of <b>TCP server</b>or <b>TCP client</b> the DISCON command processes the disconnect-process (b>Active close</b>or <b>Passive close</b>.\n
 * @par Active close
 * it transmits disconnect-request(FIN packet) to the connected peer\n
 * @par Passive close
 * When FIN packet is received from peer, a FIN packet is replied back to the peer.\n
 * @details When the disconnect-process is successful (that is, FIN/ACK packet is received successfully), @ref Sn_SR is changed to @ref SOCK_CLOSED.\n
 * Otherwise, @b TCPTO occurs (\ref Sn_IR[3]='1') and then @ref Sn_SR is changed to @ref SOCK_CLOSED.
 * @note Valid only in TCP mode.
 */
#define Sn_CR_DISCON       0x08        

/**
 * @brief Close socket
 * @details @ref Sn_SR is changed to @ref SOCK_CLOSED.
 */
#define Sn_CR_CLOSE        0x10 

/**
 * @brief Update TX buffer pointer and send data
 * @details SEND command transmits all the data in the Socket n TX buffer thru @ref Sn_TX_FIFOR.\n
 * For more details, please refer to Socket n TX Free Size Register (@ref Sn_TX_FSR) and Socket TX Write Size register (@ref Sn_TX_WRSR).
 */
#define Sn_CR_SEND         0x20                

/**
 * @brief Send data with MAC address, so without ARP process
 * @details The basic operation is same as SEND.\n
 * Normally SEND command transmits data after destination hardware address is acquired by the automatic ARP-process(Address Resolution Protocol).\n
 * But SEND_MAC command transmits data without the automatic ARP-process.\n
 * In this case, the destination hardware address is acquired from @ref Sn_DHAR configured by host, instead of APR-process.
 * @note Valid only in UDP mode.
 */
#define Sn_CR_SEND_MAC     0x21                

/**
 * @brief Send keep alive message
 * @details It checks the connection status by sending 1byte keep-alive packet.\n
 * If the peer can not respond to the keep-alive packet during timeout time, the connection is terminated and the timeout interrupt will occur.
 * @note Valid only in TCP mode.
 */
#define Sn_CR_SEND_KEEP    0x22                 

/**
 * @brief Update RX buffer pointer and receive data
 * @details RECV completes the processing of the received data in Socket n RX Buffer thru @ref Sn_RX_FIFOR).\n
 * For more details, refer to Socket n RX Received Size Register (@ref Sn_RX_RSR) & @ref Sn_RX_FIFOR.
 */
#define Sn_CR_RECV         0x40                 /**< RECV command value of \ref Sn_CR */

#define Sn_CR_PCON         0x23                 /**< PPPoE connection begins by transmitting PPPoE discovery packet. Refer to \ref Sn_CR */
#define Sn_CR_PDISCON      0x24                 /**< Closes PPPoE connection. Refer to \ref Sn_CR */ 
#define Sn_CR_PCR          0x25                 /**< In each phase, it transmits REQ message. Refer to \ref Sn_CR */
#define Sn_CR_PCN          0x26                 /**< In each phase, it transmits NAK message. Refer to \ref Sn_CR */
#define Sn_CR_PCJ          0x27                 /**< In each phase, it transmits REJECT message. Refer to \ref Sn_CR */


/*********************************/ 
/* The values of Sn_IR definition */ 
/*********************************/
#define Sn_IR_PRECV        0x80                 /**< It is set in the case that option data which is not supported is received. Refer to \ref Sn_IR */
#define Sn_IR_PFAIL        0x40                 /**< It is set in the case that PAP authentication is failed. Refer to \ref Sn_IR */
#define Sn_IR_PNEXT        0x20                 /**< It is set in the case that the phase is changed during PPPoE connection process. \ref Sn_IR */
#define Sn_IR_SENDOK       0x10                 /**< It is set when SEND command is completed. Refer to \ref Sn_IR */
#define Sn_IR_TIMEOUT      0x08                 /**< It is set when ARPTO or TCPTO is occured.  Refer to \ref Sn_IR */
#define Sn_IR_RECV         0x04                 /**< It is set whenever data is received from a peer.  Refer to \ref Sn_IR */
#define Sn_IR_DISCON       0x02                 /**< It is set when FIN or FIN/ACK packet is received from a peer.  Refer to \ref Sn_IR */
#define Sn_IR_CON          0x01                 /**< It is set one time when the connection is successful and then @ref Sn_SR is changed to @ref SOCK_ESTABLISHED. */

/**********************************/ 
/* The values of Sn_SSR definition */ 
/**********************************/
/**
 * @brief The state of SOCKET intialized or closed
 * @details This indicates that Socket n is released.\n
 * When DICON, CLOSE command is ordered, or when a timeout occurs, it is changed to @ref SOCK_CLOSED regardless of previous status.
 */
#define SOCK_CLOSED        0x00                 

/**
 * @brief The state of ARP process
 * @details It is temporary state for getting a peer MAC address when TCP connect or UDP Data Send\n
 * When DICON, CLOSE command is ordered, or when a timeout occurs, it is changed to @ref SOCK_CLOSED regardless of previous status.
 */
#define SOCK_ARP           0x01                 /**< ARP-request is transmitted in order to acquire destination hardware address. */

/**
 * @brief Initiate state in TCP.
 * @details This indicates Socket n is opened with TCP mode.\n
 * It is changed to @ref SOCK_INIT when \ref Sn_MR(P[3:0]) = '001' and OPEN command(\ref Sn_CR_OPEN) is ordered.\n
 * After SOCK_INIT, user can use LISTEN(@ref Sn_CR_LISTEN)/CONNECT(@ref Sn_CR_CONNET) command.
 */
#define SOCK_INIT          0x13                 

/**
 * @brief Listen state
 * @details This indicates Socket n is operating as <b>TCP server</b>mode and waiting for connection-request (SYN packet) from a peer <b>TCP client</b>.\n
 * It will change to @ref SOCK_ESTALBLISHED when the connection-request is successfully accepted.\n
 * Otherwise it will change to @ref SOCK_CLOSED after TCPTO (@ref Sn_IR_TIMEOUT = '1') is occurred.
 */
#define SOCK_LISTEN        0x14                 

/**
 * @brief Connection state
 * @details This indicates Socket n sent the connect-request packet (SYN packet) to a peer.\n
 * It is temporarily shown when @ref Sn_SR is changed from @ref SOCK_INIT to @ref SOCK_ESTABLISHED by @ref Sn_CR_CONNECT command.\n
 * If connect-accept(SYN/ACK packet) is received from the peer at SOCK_SYNSENT, it changes to @ref SOCK_ESTABLISHED.\n
 * Otherwise, it changes to @ref SOCK_CLOSED after TCPTO (@ref Sn_IR_TIMEOUT = '1') is occurred.
 */
#define SOCK_SYNSENT       0x15                 

/**
 * @brief Connection state
 * @details It indicates Socket n successfully received the connect-request packet (SYN packet) from a peer.\n
 * If socket n sends the response (SYN/ACK  packet) to the peer successfully,  it changes to @ref SOCK_ESTABLISHED. \n
 * If not, it changes to @ref SOCK_CLOSED after timeout (@ref Sn_IR_TIMEOUT = '1') is occurred.
 */
#define SOCK_SYNRECV       0x16                 

/**
 * @brief Success to connect
 * @details This indicates the status of the connection of Socket n.\n
 * It changes to @ref SOCK_ESTABLISHED when the <b>TCP SERVER</b>processed the SYN packet from the <b>TCP CLIENT</b>during @ref SOCK_LISTEN, or
 * when the @ref Sn_CR_CONNECT command is successful.\n
 * During @ref SOCK_ESTABLISHED, DATA packet can be transferred using @ref Sn_CR_SEND or @ref Sn_CR_RECV command.
 */
#define SOCK_ESTABLISHED   0x17                 

/**
 * @brief Closing state
 * @details These indicate Socket n is closing.\n
 * These are shown in disconnect-process such as active-close and passive-close.\n
 * When Disconnect-process is successfully completed, or when timeout(@ref Sn_CR_TIMTEOUT = '1') is occurred, these change to @ref SOCK_CLOSED.
 */
#define SOCK_FIN_WAIT      0x18                 

/**
 * @brief Closing state
 * @details These indicate Socket n is closing.\n
 * These are shown in disconnect-process such as active-close and passive-close.\n
 * When Disconnect-process is successfully completed, or when timeout occurs, these change to @ref SOCK_CLOSED.
 */
#define SOCK_CLOSING       0x1A                 

/**
 * @brief Closing state
 * @details These indicate Socket n is closing.\n
 * These are shown in disconnect-process such as active-close and passive-close.\n
 * When Disconnect-process is successfully completed, or when timeout occurs, these change to @ref SOCK_CLOSED.
 */
#define SOCK_TIME_WAIT     0x1B                

/**
 * @brief Closing state
 * @details This indicates Socket n received the disconnect-request (FIN packet) from the connected peer.\n
 * This is half-closing status, and data can be transferred.\n
 * For full-closing, @ref Sn_CR_DISCON command is used. But For just-closing, @ref Sn_CR_CLOSE command is used.
 */
#define SOCK_CLOSE_WAIT    0x1C                 

/**
 * @brief Closing state
 * @details This indicates Socket n is waiting for the response (FIN/ACK packet) to the disconnect-request (FIN packet) by passive-close.\n
 * It changes to @ref SOCK_CLOSED when Socket n received the response successfully, or when timeout (@ref Sn_IR_TIMEOUT = '1') is occurred.
 */
#define SOCK_LAST_ACK      0x1D                 

/**
 * @brief UDP socket
 * @details This indicates Socket n is opened in UDP mode(@ref Sn_MR(P[3:0]) = '010').\n
 * It changes to SOCK_UDP when @ref Sn_MR(P[3:0]) = '010' and @ref Sn_CR_OPEN command is ordered.\n
 * Unlike TCP mode, data can be transfered without the connection-process.
 */
#define SOCK_UDP           0x22                 

/**
 * @brief IP raw mode socket
 * @details TThe socket is opened in IPRAW mode. The SOCKET status is change to SOCK_IPRAW when @ref Sn_MR (P3:P0) is
 * Sn_MR_IPRAW and @ref Sn_CR_OPEN command is used.\n
 * IP Packet can be transferred without a connection similar to the UDP mode.
*/
#define SOCK_IPRAW         0x32                 

/**
 * @brief MAC raw mode socket
 * @details This indicates Socket 0 is opened in MACRAW mode (@ref Sn_MR(P[3:0]) = '100' and n = 0) and is valid only in Socket 0.\n
 * It changes to SOCK_MACRAW when @ref Sn_MR(P[3:0] = 100)and @ ref Sn_CR_OPEN command is ordered.\n
 * Like UDP mode socket, MACRAW mode Socket 0 can transfer a MAC packet (Ethernet frame) without the connection-process.
 */
#define SOCK_MACRAW        0x42                 /**< SOCKET0 is open as MACRAW mode. */

/**
 * @brief PPPoE mode socket
 * @details It is the status that SOCKET0 is opened as PPPoE mode. 
 * It is changed to SOCK_PPPoE in case of @ref Sn_CR_OPEN command is ordered and @ref Sn_MR(P3:P0)= @ref Sn_MR_PPPoE\n
 * It is temporarily used at the PPPoE connection.
 */
#define SOCK_PPPoE         0x5F                 /**< SOCKET0 is open as PPPoE mode. */

/* IP PROTOCOL */
#define IPPROTO_IP                   0        //< Dummy for IP 
#define IPPROTO_ICMP                 1        //< Control message protocol
#define IPPROTO_IGMP                 2        //< Internet group management protocol
#define IPPROTO_GGP                  3        //< Gateway^2 (deprecated)
#define IPPROTO_TCP                  6        //< TCP
#define IPPROTO_PUP                  12       //< PUP
#define IPPROTO_UDP                  17       //< UDP
#define IPPROTO_IDP                  22       //< XNS idp
#define IPPROTO_ND                   77       //< UNOFFICIAL net disk protocol
#define IPPROTO_RAW                  255      //< Raw IP packet


/**
 * @brief Enter a critical section
 *
 * @details It is provided to protect your shared code which are executed without distribution. \n \n
 *
 * In non-OS environment, It can be just implemented by disabling whole interrupt.\n
 * In OS environment, You can replace it to critical section api supported by OS.
 *
 * \sa WIZCHIP_READ(), WIZCHIP_WRITE()
 * \sa WIZCHIP_CRITICAL_EXIT()
 */
#define WIZCHIP_CRITICAL_ENTER()    WIZCHIP.CRIS._enter()

#ifdef _exit
#undef _exit
#endif

/**
 * @brief Exit a critical section
 *
 * @details It is provided to protect your shared code which are executed without distribution. \n\n
 *
 * In non-OS environment, It can be just implemented by disabling whole interrupt. \n
 * In OS environment, You can replace it to critical section api supported by OS.
 *
 * @sa WIZCHIP_READ(), WIZCHIP_WRITE()
 * @sa WIZCHIP_CRITICAL_ENTER()
 */
#define WIZCHIP_CRITICAL_EXIT()     WIZCHIP.CRIS._exit()

////////////////////////
// Basic I/O Function //
////////////////////////

/**
 * @ingroup Basic_IO_function_W5300
 * @brief It reads 1 byte value from a register.
 * @param AddrSel Register address
 * @return The value of register
 */
uint16_t  WIZCHIP_READ (uint32_t AddrSel);

/**
 * @ingroup Basic_IO_function_W5300
 * @brief It writes 1 byte value to a register.
 * @param AddrSel Register address
 * @param wb Write data
 * @return void
 */
void     WIZCHIP_WRITE(uint32_t AddrSel, uint16_t wb );

/***********************************
 * COMMON Register Access Function *
 ***********************************/

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set Mode Register
 * @param (@ref iodata_t)mr The value to be set.
 * @sa getMR()
 */
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_BUS_)
   #if (_WIZCHIP_IO_BUS_WIDTH_ == 8)
      #define setMR(mr) \
         (*((uint8_t*)MR) = (uint8_t)((mr) >> 8)); (*((uint8_t*)WIZCHIP_OFFSET_INC(MR,1)) = (uint8_t)((mr) & 0xFF))
   #elif (_WIZCHIP_IO_BUS_WIDTH_ == 16)
      #define setMR(mr)    (*((uint16_t*)MR) = (uint16_t)((mr) & 0xFFFF))
   #else
      #error "Unknown _WIZCHIP_IO_BUS_WIDTH_. You should be define _WIZCHIP_IO_BUS_WIDTH as 8 or 16."   
   #endif
#else
   #error "Unknown _WIZCHIP_IO_MODE_"   
#endif

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref MR.
 * @return @ref iodata_t. The value of Mode register.
 * @sa setMR()
 */
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_BUS_)
   #if (_WIZCHIP_IO_BUS_WIDTH_ == 8)
      #define getMR()    (((uint16_t)(*((uint8_t*)MR)) << 8) + (((uint16_t)(*((uint8_t*)WIZCHIP_OFFSET_INC(MR,1)))) & 0x00FF))
   #elif(_WIZCHIP_IO_BUS_WIDTH_ == 16)
      #define getMR()    (*((uint16_t*)MR))
   #else
      #error "Unknown _WIZCHIP_IO_BUS_WIDTH_. You should be define _WIZCHIP_IO_BUS_WIDTH as 8 or 16."   
   #endif
#else
   #error "Unknown _WIZCHIP_IO_MODE_"   
#endif

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set \ref IR register
 * @param (uint16_t)ir Value to set \ref IR register. 
 * @sa getIR()
 */
#define  setIR(ir) \
   WIZCHIP_WRITE(IR, ir & 0xF0FF)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get \ref IR register
 * @return uint8_t. Value of \ref IR register.
 * @sa setIR()
 */
#define  getIR()  \
   (WIZCHIP_READ(IR) & 0xF0FF)


/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set \ref _IMR_ register
 * @param (uint16_t)imr Value to set @ref _IMR_ register.
 * @sa getIMR()
 */
#define  setIMR(imr) \
   WIZCHIP_WRITE(_IMR_, imr & 0xF0FF)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get \ref _IMR_ register
 * @return uint16_t. Value of \ref IR register.
 * @sa setIMR()
 */
#define  getIMR() \
   (WIZCHIP_READ(_IMR_) & 0xF0FF)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set local MAC address
 * @param (uint8_t*)shar Pointer variable to set local MAC address. It should be allocated 6 bytes.
 * @sa getSHAR()
 */
#define  setSHAR(shar) { \
      WIZCHIP_WRITE(SHAR,                       (((uint16_t)((shar)[0])) << 8) + (((uint16_t)((shar)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(SHAR,2), (((uint16_t)((shar)[2])) << 8) + (((uint16_t)((shar)[3])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(SHAR,4), (((uint16_t)((shar)[4])) << 8) + (((uint16_t)((shar)[5])) & 0x00FF)); \
   }

/**
 * @ingroup Common_register_access_function
 * @brief Get local MAC address
 * @param (uint8_t*)shar Pointer variable to get local MAC address. It should be allocated 6 bytes.
 * @sa setSHAR()
 */
#define  getSHAR(shar) { \
		(shar)[0] = (uint8_t)(WIZCHIP_READ(SHAR) >> 8); \
		(shar)[1] = (uint8_t)(WIZCHIP_READ(SHAR));      \
		(shar)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SHAR,2)) >> 8); \
		(shar)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SHAR,2))); \
		(shar)[4] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SHAR,4)) >> 8); \
		(shar)[5] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SHAR,4))); \
   }		

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set gateway IP address
 * @param (uint8_t*)gar Pointer variable to set gateway IP address. It should be allocated 4 bytes.
 * @sa getGAR()
 */
#define  setGAR(gar) { \
      WIZCHIP_WRITE(GAR,                       (((uint16_t)((gar)[0])) << 8) + (((uint16_t)((gar)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(GAR,2), (((uint16_t)((gar)[2])) << 8) + (((uint16_t)((gar)[3])) & 0x00FF)); \
   }

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get gateway IP address
 * @param (uint8_t*)gar Pointer variable to get gateway IP address. It should be allocated 4 bytes.
 * @sa setGAR()
 */
#define  getGAR(gar) { \
		(gar)[0] = (uint8_t)(WIZCHIP_READ(GAR) >> 8); \
		(gar)[1] = (uint8_t)(WIZCHIP_READ(GAR));      \
		(gar)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(GAR,2)) >> 8); \
		(gar)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(GAR,2))); \
   }		

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set subnet mask address
 * @param (uint8_t*)subr Pointer variable to set subnet mask address. It should be allocated 4 bytes.
 * @sa getSUBR()
 */
#define  setSUBR(subr) { \
      WIZCHIP_WRITE(SUBR,                       (((uint16_t)((subr)[0])) << 8) + (((uint16_t)((subr)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(SUBR,2), (((uint16_t)((subr)[2])) << 8) + (((uint16_t)((subr)[3])) & 0x00FF)); \
   }

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get subnet mask address
 * @param (uint8_t*)subr Pointer variable to get subnet mask address. It should be allocated 4 bytes.
 * @sa setSUBR()
 */
#define  getSUBR(subr) { \
		(subr)[0] = (uint8_t)(WIZCHIP_READ(SUBR) >> 8); \
		(subr)[1] = (uint8_t)(WIZCHIP_READ(SUBR));      \
		(subr)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SUBR,2)) >> 8); \
		(subr)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SUBR,2))); \
   }

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set local IP address
 * @param (uint8_t*)sipr Pointer variable to set local IP address. It should be allocated 4 bytes.
 * @sa getSIPR()
 */
#define  setSIPR(sipr) { \
      WIZCHIP_WRITE(SIPR,                       (((uint16_t)((sipr)[0])) << 8) + (((uint16_t)((sipr)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(SIPR,2), (((uint16_t)((sipr)[2])) << 8) + (((uint16_t)((sipr)[3])) & 0x00FF)); \
   }

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get local IP address
 * @param (uint8_t*)sipr Pointer variable to get local IP address. It should be allocated 4 bytes.
 * @sa setSIPR()
 */
#define  getSIPR(sipr) { \
		(sipr)[0] = (uint8_t)(WIZCHIP_READ(SIPR) >> 8); \
		(sipr)[1] = (uint8_t)(WIZCHIP_READ(SIPR));      \
	   (sipr)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SIPR,2)) >> 8); \
		(sipr)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(SIPR,2))); \
   }


/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref _RTR_ register
 * @param (uint16_t)rtr Value to set @ref _RTR_ register.
 * @sa getRTR()
 */
#define  setRTR(rtr)   \
		WIZCHIP_WRITE(_RTR_, rtr)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref _RTR_ register
 * @return uint16_t. Value of @ref _RTR_ register.
 * @sa setRTR()
 */
#define  getRTR() \
		WIZCHIP_READ(_RTR_)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref _RCR_ register
 * @param (uint8_t)rcr Value to set @ref _RCR_ register.
 * @sa getRCR()
 */
#define  setRCR(rcr) \
		WIZCHIP_WRITE(_RCR_, ((uint16_t)rcr)&0x00FF)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref _RCR_ register
 * @return uint8_t. Value of @ref _RCR_ register.
 * @sa setRCR()
 */
#define  getRCR() \
		((uint8_t)(WIZCHIP_READ(_RCR_) & 0x00FF))

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref TMS01R register
 * @param (uint16_t)tms01r Value to set @ref TMS01R register. The lower socket memory size is located at MSB of tms01r.
 * @sa getTMS01R()
 */
#define  setTMS01R(tms01r) \
   WIZCHIP_WRITE(TMS01R,tms01r)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref TMS01R register
 * @return uint16_t. Value of @ref TMS01R register.
 * @sa setTMS01R()
 */
#define  getTMS01R()  \
   WIZCHIP_READ(TMS01R)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref TMS23R register
 * @param (uint16_t)tms23r Value to set @ref TMS23R register. The lower socket memory size is located at MSB of tms01r.
 * @sa getTMS23R()
 */
#define  setTMS23R(tms23r) \
   WIZCHIP_WRITE(TMS23R,tms23r)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref TMS23R register
 * @return uint16_t. Value of @ref TMS23R register.
 * @sa setTMS23R()
 */
#define  getTMS23R()  \
   WIZCHIP_READ(TMS23R)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref TMS45R register
 * @param (uint16_t)tms45r Value to set @ref TMS45R register. The lower socket memory size is located at MSB of tms45r.
 * @sa getTMS45R()
 */
#define  setTMS45R(tms45r) \
   WIZCHIP_WRITE(TMS45R,tms45r)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref TMS45R register
 * @return uint16_t. Value of @ref TMS45R register.
 * @sa setTMS45R()
 */
#define  getTMS45R()  \
   WIZCHIP_READ(TMS45R)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref TMS67R register
 * @param (uint16_t)tms67r Value to set @ref TMS67R register. The lower socket memory size is located at MSB of tms67r.
 * @sa getTMS67R()
 */
#define  setTMS67R(tms67r) \
   WIZCHIP_WRITE(TMS67R,tms67r)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref TMS67R register
 * @return uint16_t. Value of @ref TMS67R register.
 * @sa setTMS67R()
 */
#define  getTMS67R()  \
   WIZCHIP_READ(TMS67R)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref TMSR0 ~ @ref TMSR7 register
 * @param (uint8_t)sn Socket number. It should be 0 ~ 7. 
 * @param (uint8_t)tmsr Value to set @ref TMSR0 ~@ref TMSR7 register. 
 * @sa getTMSR()
 */
void  setTMSR(uint8_t sn,uint8_t tmsr);
#define setSn_TXBUF_SIZE(sn, tmsr)   setTMSR(sn, tmsr) ///< For compatible ioLibrary

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref TMSR0 ~ @ref TMSR7 register
 * @param (uint8_t)sn Socket number. It should be 0 ~ 7.
 * @return uint8_t. Value of @ref TMSR0 ~ @ref TMSR7
 * @sa getTMSR()
 */
uint8_t getTMSR(uint8_t sn);
#define getSn_TXBUF_SIZE(sn)  getTMSR(sn) ///< For compatible ioLibrary

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref RMS01R register
 * @param (uint16_t)rms01r Value to set @ref RMS01R register. The lower socket memory size is located at MSB of rms01r.
 * @sa getRMS01R()
 */
#define setRMS01R(rms01r) \
   WIZCHIP_WRITE(RMS01R,rms01r)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref RMS01R register
 * @return uint16_t. Value of @ref RMS01R register.
 * @sa setRMS01R()
 */
#define getRMS01R()  \
   WIZCHIP_READ(RMS01R)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref RMS23R register
 * @param (uint16_t)rms23r Value to set @ref RMS23R register. The lower socket memory size is located at MSB of rms01r.
 * @sa getRMS23R()
 */
#define setRMS23R(rms23r) \
   WIZCHIP_WRITE(RMS23R,rms23r)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref RMS23R register
 * @return uint16_t. Value of @ref RMS23R register.
 * @sa setRMS23R()
 */
#define getRMS23R()  \
   WIZCHIP_READ(RMS23R)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref RMS45R register
 * @param (uint16_t)rms45r Value to set @ref RMS45R register. The lower socket memory size is located at MSB of rms45r.
 * @sa getRMS45R()
 */
#define setRMS45R(rms45r) \
   WIZCHIP_WRITE(RMS45R,rms45r)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref RMS45R register
 * @return uint16_t. Value of @ref RMS45R register.
 * @sa setRMS45R()
 */
#define getRMS45R()  \
   WIZCHIP_READ(RMS45R)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref RMS67R register
 * @param (uint16_t)rms67r Value to set @ref RMS67R register. The lower socket memory size is located at MSB of rms67r.
 * @sa getRMS67R()
 */
#define setRMS67R(rms67r) \
   WIZCHIP_WRITE(RMS67R,rms67r)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref RMS67R register
 * @return uint16_t. Value of @ref RMS67R register.
 * @sa setRMS67R()
 */
#define getRMS67R()  \
   WIZCHIP_READ(RMS67R)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref RMS01R ~ @ref RMS67R register
 * @param (uint8_t)sn Socket number. It should be 0 ~ 7.
 * @param (uint8_t)rmsr Value to set @ref RMSR0 ~@ref RMSR7 register. 
 * @sa getTMSR()
 */
void setRMSR(uint8_t sn,uint8_t rmsr);
#define setSn_RXBUF_SIZE(sn,rmsr)   setRMSR(sn, rmsr)   ///< For compatible ioLibrary

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref RMS01R ~ @ref RMS67R register
 * @param (uint8_t)sn Socket number. It shoudl be 0 ~ 7.
 * @return uint8_t. Value of @ref RMSR0 ~ @ref RMSR7 register.
 * @sa setRMSR()
 */
uint8_t getRMSR(uint8_t sn);
#define getSn_RXBUF_SIZE(sn)  getRMSR(sn)    ///< For compatible ioLibrary

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref MTYPER register
 * @param (uint16_t)mtyper Value to set @ref MTYPER register. 
 * @sa getMTYPER()
 */
#define setMTYPER(mtype) \
   WIZCHIP_WRITE(MTYPER, mtype)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref MTYPER register
 * @return uint16_t. Value of @ref MTYPER register. 
 * @sa setMTYPER()
 */
#define getMTYPER() \
   WIZCHIP_READ(MTYPER)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref RATR register
 * @return uint16_t. Value of @ref PATR register.
 */
#define getPATR() \
   WIZCHIP_READ(PATR)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref PTIMER register
 * @param (uint8_t)ptimer Value to set @ref PTIMER register.
 * @sa getPTIMER()
 */
#define setPTIMER(ptimer) \
		WIZCHIP_WRITE(PTIMER, ((uint16_t)ptimer) & 0x00FF)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref PTIMER register
 * @return uint8_t. Value of @ref PTIMER register.
 * @sa setPTIMER()
 */
#define getPTIMER() \
		((uint8_t)(WIZCHIP_READ(PTIMER) & 0x00FF))

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref PMAGIC register
 * @param (uint8_t)pmagic Value to set @ref PMAGIC register.
 * @sa getPMAGIC()
 */
#define setPMAGIC(pmagic) \
		WIZCHIP_WRITE(PMAGIC, ((uint16_t)pmagic) & 0x00FF)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref PMAGIC register
 * @return uint8_t. Value of @ref PMAGIC register.
 * @sa setPMAGIC()
 */
#define getPMAGIC() \
		((uint8_t)(WIZCHIP_READ(PMAGIC) & 0x00FF))

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref PSID register
 * @return uint16_t. Value of @ref PSID register.
 */
#define getPSIDR() \
		WIZCHIP_READ(PSIDR)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref PDHAR register
 * @param (uint8_t*)pdhar Pointer variable to PPP destination MAC register address. It should be allocated 6 bytes.
 */
#define getPDHAR(pdhar) { \
		(pdhar)[0] = (uint8_t)(WIZCHIP_READ(PDHAR) >> 8); \
		(pdhar)[1] = (uint8_t)(WIZCHIP_READ(PDHAR));      \
		(pdhar)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(PDHAR,2)) >> 8); \
		(pdhar)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(PDHAR,2))); \
		(pdhar)[4] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(PDHAR,4)) >> 8); \
		(pdhar)[5] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(PDHAR,4))); \
   }

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get unreachable IP address. @ref UIPR
 * @param (uint8_t*)uipr Pointer variable to get unreachable IP address. It should be allocated 4 bytes.
 */
#define getUIPR(uipr) { \
		(uipr)[0] = (uint8_t)(WIZCHIP_READ(UIPR) >> 8); \
		(uipr)[1] = (uint8_t)(WIZCHIP_READ(UIPR));      \
		(uipr)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(UIPR,2)) >> 8); \
		(uipr)[3] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(UIPR,2))); \
   }

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref UPORTR register
 * @return uint16_t. Value of @ref UPORTR register.
 */
#define getUPORTR() \
	WIZCHIP_READ(UPORTR)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref FMTUR register
 * @return uint16_t. Value of @ref FMTUR register.
 */
#define getFMTUR() \
   WIZCHIP_READ(FMTUR)


/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref Pn_BRDYR register
 * @return uint8_t. Value of @ref Pn_BRDYR register.
 */
#define getPn_BRDYR(p) \
   ((uint8_t)(WIZCHIP_READ(Pn_BRDYR(p)) & 0x00FF))

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref Pn_BRDYR register
 * @param p Pin number (p = 0,1,2,3)
 * @param brdyr Set a value @ref Pn_BRDYR(p). 
 */
#define setPn_BRDYR(p, brdyr) \
   WIZCHIP_WRITE(Pn_BRDYR(p), brdyr & 0x00E7)

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref Pn_BDPTHR register
 * @param p Pin number (p = 0,1,2,3)
 * @return uint16_t. Value of @ref Pn_BDPTHR register.
 */
#define getPn_BDPTHR(p) \
   WIZCHIP_READ(Pn_BDPTHR(p))

/**
 * @ingroup Common_register_access_function_W5300
 * @brief Set @ref Pn_BDPTHR register
 * @param p Pin number (p = 0,1,2,3)
 * @param bdpthr Value of @ref Pn_BDPTHR
 */
#define setPn_BDPTHR(p, bdpthr) \
   WIZCHIP_WRITE(Pn_BDPTHR(p),bdpthr)


/**
 * @ingroup Common_register_access_function_W5300
 * @brief Get @ref IDR register
 * @return uint16_t. Always 0x5300.
 */
#define getIDR() \
   WIZCHIP_READ(IDR)


/***********************************
 * SOCKET Register Access Function *
 ***********************************/

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_MR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)mr Value to set @ref Sn_MR
 * @sa getSn_MR()
 */
#define setSn_MR(sn, mr) \
   WIZCHIP_WRITE(Sn_MR(sn),mr)

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_MR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref Sn_MR.
 * @sa setSn_MR()
 */
#define getSn_MR(sn) \
	WIZCHIP_READ(Sn_MR(sn))

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_CR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)cr Value to set @ref Sn_CR
 * @sa getSn_CR()
 */
#define setSn_CR(sn, cr) \
   WIZCHIP_WRITE(Sn_CR(sn), ((uint16_t)cr) & 0x00FF)

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_CR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref Sn_CR.
 * @sa setSn_CR()
 */
#define getSn_CR(sn) \
   ((uint8_t)WIZCHIP_READ(Sn_CR(sn)))

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_IMR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)imr Value to set @ref Sn_IMR
 * @sa getSn_IMR()
 */
#define setSn_IMR(sn, imr) \
   WIZCHIP_WRITE(Sn_IMR(sn), ((uint16_t)imr) & 0x00FF)

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_IMR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref Sn_IMR.
 * @sa setSn_IMR()
 */
#define getSn_IMR(sn) \
   ((uint8_t)WIZCHIP_READ(Sn_IMR(sn)))

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_IR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)ir Value to set @ref Sn_IR
 * @sa getSn_IR()
 */
#define setSn_IR(sn, ir) \
   WIZCHIP_WRITE(Sn_IR(sn), ((uint16_t)ir) & 0x00FF)

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_IR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref Sn_IR.
 * @sa setSn_IR()
 */
#define getSn_IR(sn) \
   ((uint8_t)WIZCHIP_READ(Sn_IR(sn)))

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_SR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref Sn_SR.
 */
#define getSn_SSR(sn) \
   ((uint8_t)WIZCHIP_READ(Sn_SR(sn)))
#define getSn_SR(sn) getSn_SSR(sn)  ///< For compatible ioLibrary. Refer to getSn_SSR().

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_PORTR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)port Variable to set @ref Sn_PORTR.
 * @sa getSn_PORTR()
 */
#define setSn_PORTR(sn, port) \
   WIZCHIP_WRITE(Sn_PORTR(sn), port)
#define setSn_PORT(sn, port)   setSn_PORTR(sn, port)   ///< For compatible ioLibrary

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_PORTR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Variable of @ref Sn_PORTR.
 * @sa setSn_PORTR()
 */
#define getSn_PORTR(sn) \
   WIZCHIP_READ(Sn_PORTR(sn))
#define getSn_PORT(sn)   getSn_PORTR(sn)   ///< For compatible ioLibrary

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_DHAR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t*)dhar Pointer variable to set socket n destination hardware address. It should be allocated 6 bytes.
 * @sa getSn_DHAR()
 */
#define setSn_DHAR(sn, dhar) { \
      WIZCHIP_WRITE(Sn_DHAR(sn),                       (((uint16_t)((dhar)[0])) << 8) + (((uint16_t)((dhar)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),2), (((uint16_t)((dhar)[0])) << 8) + (((uint16_t)((dhar)[1])) & 0x00FF)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),4), (((uint16_t)((dhar)[0])) << 8) + (((uint16_t)((dhar)[1])) & 0x00FF)); \
   }      

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_MR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t*)dhar Pointer variable to get socket n destination hardware address. It should be allocated 6 bytes.
 * @sa setSn_DHAR()
 */
#define getSn_DHAR(sn, dhar) { \
      (dhar)[0] = (uint8_t)(WIZCHIP_READ(Sn_DHAR(sn)) >> 8); \
      (dhar)[1] = (uint8_t) WIZCHIP_READ(Sn_DHAR(sn)); \
      (dhar)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),2)) >> 8); \
      (dhar)[3] = (uint8_t) WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),2)); \
      (dhar)[4] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),4)) >> 8); \
      (dhar)[5] = (uint8_t) WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DHAR(sn),4)); \
   }

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_DPORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)dport Value to set @ref Sn_DPORT
 * @sa getSn_DPORT()
 */
#define setSn_DPORTR(sn, dport) \
   WIZCHIP_WRITE(Sn_DPORTR(sn),dport) 
#define setSn_DPORT(sn, dport)   setSn_DPORTR(sn,dport) ///< For compatible ioLibrary. Refer to @ref Sn_DPORTR.


/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_DPORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref Sn_DPORT.
 * @sa setSn_DPORT()
 * @note This function is not available because W5300 have a bug to read @ref Sn_DPORTR. \n
 *       Don't use this function. 
 */
#define getSn_DPORTR(sn) \
   WIZCHIP_READ(Sn_DPORTR(sn))
#define getSn_DPORT(sn) getSn_DPORTR(sn)  ///< For compatible ioLibrary. Refer to @ref Sn_DPORTR.

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_DIPR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t*)dipr Pointer variable to set socket n destination IP address. It should be allocated 4 bytes.
 * @sa getSn_DIPR()
 */
#define setSn_DIPR(sn, dipr) { \
		WIZCHIP_WRITE(Sn_DIPR(sn),                       (((uint16_t)((dipr)[0])) << 8) + (((uint16_t)((dipr)[1])) & 0x00FF)); \
		WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_DIPR(sn),2), (((uint16_t)((dipr)[2])) << 8) + (((uint16_t)((dipr)[3])) & 0x00FF)); \
   }
   
/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_DIPR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t*)dipr Pointer variable to get socket n destination IP address. It should be allocated 4 bytes.
 * @sa setSn_DIPR()
 */
#define getSn_DIPR(sn, dipr) { \
		(dipr)[0] = (uint8_t)(WIZCHIP_READ(Sn_DIPR(sn)) >> 8); \
		(dipr)[1] = (uint8_t) WIZCHIP_READ(Sn_DIPR(sn));  \
		(dipr)[2] = (uint8_t)(WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DIPR(sn),2)) >> 8); \
		(dipr)[3] = (uint8_t) WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_DIPR(sn),2));  \
   }

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_MSSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)mss Value to set @ref Sn_MSSR
 * @sa setSn_MSSR()
 */
#define setSn_MSSR(sn, mss) \
   WIZCHIP_WRITE(Sn_MSSR(sn), mss)

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_MSSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref Sn_MSSR.
 * @sa setSn_MSSR()
 */
#define getSn_MSSR(sn) \
   WIZCHIP_READ(Sn_MSSR(sn))

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_KPALVTR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)kpalvt Value to set @ref Sn_KPALVTR
 * @sa getSn_KPALVTR()
 */
#define setSn_KPALVTR(sn, kpalvt) \
   WIZCHIP_WRITE(Sn_KPALVTR(sn), (WIZCHIP_READ(Sn_KPALVTR(sn)) & 0x00FF) | (((uint16_t)kpalvt)<<8))

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_KPALVTR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref Sn_KPALVTR.
 * @sa setSn_KPALVTR()
 */
#define getSn_KPALVTR(sn) \
		((uint8_t)(WIZCHIP_READ(Sn_KPALVTR(sn)) >> 8))

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_PROTOR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)proto Value to set \ref Sn_PROTOR
 * @sa getSn_PROTOR()
 */
#define setSn_PROTOR(sn, proto) \
   WIZCHIP_WRITE(Sn_PROTOR(sn),(WIZCHIP_READ(Sn_PROTOR(sn)) & 0xFF00) | (((uint16_t)proto) & 0x00FF))
#define setSn_PROTO(sn,proto)    setSn_PROTOR(sn,proto)  ///< For compatible ioLibrary   

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_PROTOR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @return uint8_t. Value of @ref Sn_PROTOR.
 * @sa setSn_PROTOR()
 */
#define getSn_PROTOR(sn) \
   ((uint8_t)WIZCHIP_READ(Sn_PROTOR(sn)))
#define getSn_PROTO(sn)    getSn_PROTOR(sn)  ///< For compatible ioLibrary   

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_TX_WRSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint32_t)txwrs Value to set @ref Sn_KPALVTR (It should be <= 0x00010000)
 * @sa getSn_TX_WRSR()
 */
#define  setSn_TX_WRSR(sn, txwrs) { \
      WIZCHIP_WRITE(Sn_TX_WRSR(sn),                       (uint16_t)(((uint32_t)txwrs) >> 16)); \
      WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(Sn_TX_WRSR(sn),2), (uint16_t)txwrs); \
   }
   
/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_TX_WRSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint32_t. Value of Sn_TX_WRSR.
 * @sa setSn_TX_WRSR()
 */
#define  getSn_TX_WRSR(sn) \
   ( (((uint32_t)WIZCHIP_READ(Sn_TX_WRSR(sn))) << 16) + (((uint32_t)WIZCHIP_READ(WIZCHIP_OFFSET_INC(Sn_TX_WRSR(sn),1))) & 0x0000FFFF) )

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_TX_FSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint32_t. Value of @ref Sn_TX_FSR.
 */
uint32_t getSn_TX_FSR(uint8_t sn);

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_RX_RSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint32_t. Value of @ref Sn_RX_RSR.
 */
uint32_t getSn_RX_RSR(uint8_t sn);

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_TX_FIFOR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)txfifo. Value to set @ref Sn_TX_FIFOR.
 */
#define  setSn_TX_FIFOR(sn, txfifo) \
   WIZCHIP_WRITE(Sn_TX_FIFOR(sn), txfifo);

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_RX_FIFOR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref Sn_RX_FIFOR.
 */
#define getSn_RX_FIFOR(sn) \
   WIZCHIP_READ(Sn_RX_FIFOR(sn));

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_TOSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param (uint8_t)tos Value to set @ref Sn_TOSR
 * @sa getSn_TOSR()
 */
#define setSn_TOSR(sn, tos) \
   WIZCHIP_WRITE(Sn_TOS(sn), ((uint16_t)tos) & 0x00FF)
#define setSn_TOS(sn,tos)  setSn_TOSR(sn,tos)   ///< For compatible ioLibrar

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_TOSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_ </b>.
 * @return uint8_t. Value of Sn_TOSR.
 * @sa setSn_TOSR()
 */
#define getSn_TOSR(sn) \
   ((uint8_t)WIZCHIP_READ(Sn_TOSR(sn)))
#define getSn_TOS(sn)   getSn_TOSR(sn)    ///< For compatible ioLibrar

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_TTLR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)ttl Value to set @ref Sn_TTLR
 * @sa getSn_TTLR()
 */
#define setSn_TTLR(sn, ttl) \
   WIZCHIP_WRITE(Sn_TTLR(sn), ((uint16_t)ttl) & 0x00FF)
#define setSn_TTL(sn,ttl)     setSn_TTLR(sn,ttl)   ///< For compatible ioLibrary

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_TTLR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref Sn_TTLR.
 * @sa setSn_TTLR()
 */
#define getSn_TTLR(sn) \
   ((uint8_t)WIZCHIP_READ(Sn_TTL(sn)))
#define getSn_TTL(sn)     getSn_TTLR(sn)    ///< For compatible ioLibrary

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Set @ref Sn_FRAGR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)frag Value to set @ref Sn_FRAGR
 * @sa getSn_FRAGR()
 */
#define setSn_FRAGR(sn, frag) \
   WIZCHIP_WRITE(Sn_FRAGR(sn),  ((uint16_t)frag) & 0x00FF)
#define setSn_FRAG(sn,frag)   setSn_FRAGR(sn,flag)

/**
 * @ingroup Socket_register_access_function_W5300
 * @brief Get @ref Sn_FRAGR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref Sn_FRAGR.
 * @sa setSn_FRAGR()
 */
#define getSn_FRAGR(sn) \
   (WIZCHIP_READ(Sn_FRAG(sn)))
#define getSn_FRAG(sn)    getSn_FRAGR(sn)


/////////////////////////////////////
// Sn_TXBUF & Sn_RXBUF IO function //
/////////////////////////////////////

/**  
 * @brief Socket_register_access_function_W5300
 * @brief Gets the max buffer size of socket sn passed as parameter.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint32_t. Value of Socket n RX max buffer size.
 */
#define getSn_RxMAX(sn) \
		(((uint32_t)getSn_RXBUF_SIZE(sn)) << 10)		

/**  
 * @brief Socket_register_access_function_W5300
 * @brief Gets the max buffer size of socket sn passed as parameters.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint32_t. Value of Socket n TX max buffer size.
 */
#define getSn_TxMAX(sn) \
		(((uint32_t)getSn_TXBUF_SIZE(sn)) << 10)		

/**
 * @ingroup Basic_IO_function_W5300
 * @brief It copies data to internal TX memory
 *
 * @details This function reads the Tx write pointer register and after that,
 * it copies the <i>wizdata(pointer buffer)</i> of the length of <i>len(variable)</i> bytes to internal TX memory
 * and updates the Tx write pointer register.
 * This function is being called by send() and sendto() function also.
 *
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param wizdata Pointer buffer to write data
 * @param len Data length
 * @sa wiz_recv_data()
 */
void wiz_send_data(uint8_t sn, uint8_t *wizdata, uint32_t len);

/**
 * @ingroup Basic_IO_function_W5300
 * @brief It copies data to your buffer from internal RX memory
 *
 * @details This function read the Rx read pointer register and after that,
 * it copies the received data from internal RX memory
 * to <i>wizdata(pointer variable)</i> of the length of <i>len(variable)</i> bytes.
 * This function is being called by recv() also.
 *
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param wizdata Pointer buffer to read data
 * @param len Data length
 * @sa wiz_send_data()
 */
void wiz_recv_data(uint8_t sn, uint8_t *wizdata, uint32_t len);

/**
 * @ingroup Basic_IO_function_W5300
 * @brief It discard the received data in RX memory.
 * @details It discards the data of the length of <i>len(variable)</i> bytes in internal RX memory.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param len Data length
 */
void wiz_recv_ignore(uint8_t sn, uint32_t len);

/// \cond DOXY_APPLY_CODE
#endif
/// \endcond

#ifdef __cplusplus
}
#endif

#endif   // _W5300_H_
