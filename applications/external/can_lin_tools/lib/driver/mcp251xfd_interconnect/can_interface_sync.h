#pragma once

#include <stdlib.h>
#include "../mcp251xfd/CRC/CRC16_CMS.h"
#include <furi.h>

/*! @brief CAN INT pin configuration for the FLIPPER
 *
 * This function will be called at driver initialization to configure the INT pin to input and more by the user if necessary
 * @param[in] *pComp Is the pointed structure of the device that call this function
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT can_interface_sync_pin_init(MCP251XFD* pComp);

// /*! @brief CAN INT interrupt handler
//  *
//  * This function will be called when an interrupt on the pin linked to the INT pin of the CAN (an interrupt in the CAN occur)
//  * @param[in] id Is the PIO identifier (PIOA, PIOB, PIOC, PIOD or PIOE)
//  * @param[in] mask Is the mask of the pin where the interrupt occur
//  * @return Returns an #eERRORRESULT value enum
//  */
// void MCP251XFD_INT_Handler(uint32_t id, uint32_t mask);

// //*****************************************************************************

/*! @brief CAN INT0 pin configuration for the FLIPPER
 *
 * This function will be called at driver initialization to configure the INT0 (GPIO0) pin to input and more by the user if necessary
 * @param[in] *pComp Is the pointed structure of the device that call this function
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT can_interface_sync_int0_gpio0_pin_init(MCP251XFD* pComp);

// /*! @brief CAN INT0 interrupt handler
//  *
//  * This function will be called when an interrupt on the pin linked to the INT0 pin of the CAN (a TX interrupt in the CAN occur)
//  * @param[in] id Is the PIO identifier (PIOA, PIOB, PIOC, PIOD or PIOE)
//  * @param[in] mask Is the mask of the pin where the interrupt occur
//  * @return Returns an #eERRORRESULT value enum
//  */
// void MCP251XFD_INT0_Handler(uint32_t id, uint32_t mask);

// //*****************************************************************************

/*! @brief CAN INT1 pin configuration for the FLIPPER
 *
 * This function will be called at driver initialization to configure the INT1 (GPIO1) pin to input and more by the user if necessary
 * @param[in] *pComp Is the pointed structure of the device that call this function
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT can_interface_sync_int1_gpio1_pin_init(MCP251XFD* pComp);

// /*! @brief CAN INT1 interrupt handler
//  *
//  * This function will be called when an interrupt on the pin linked to the INT1 pin of the CAN (a RX interrupt in the CAN occur)
//  * @param[in] id Is the PIO identifier (PIOA, PIOB, PIOC, PIOD or PIOE)
//  * @param[in] mask Is the mask of the pin where the interrupt occur
//  * @return Returns an #eERRORRESULT value enum
//  */
// void MCP251XFD_INT1_Handler(uint32_t id, uint32_t mask);

/*! @brief CAN SPI interface configuration for the FLIPPER
 *
 * This function will be called at driver initialization to configure the interface driver SPI
 * @param[in] *pIntDev Is the MCP251XFD_Desc.InterfaceDevice of the device that call the interface initialization
 * @param[in] chipSelect Is the Chip Select index to use for the SPI initialization
 * @param[in] sckFreq Is the SCK frequency in Hz to set at the interface initialization
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT can_interface_sync_init(void* pIntDev, uint8_t chipSelect, const uint32_t sckFreq);

/*! @brief CAN SPI transfer for the FLIPPER
 *
 * This function will be called at driver read/write data from/to the interface driver SPI
 * @param[in] *pIntDev Is the MCP251XFD_Desc.InterfaceDevice of the device that call this function
 * @param[in] chipSelect Is the Chip Select index to use for the SPI transfer
 * @param[in] *txData Is the buffer to be transmit to through the SPI interface
 * @param[out] *rxData Is the buffer to be received to through the SPI interface (can be NULL if it's just a send of data)
 * @param[in] size Is the size of data to be send and received trough SPI. txData and rxData shall be at least the same size
 * @return Returns an #eERRORRESULT value enum
 */
eERRORRESULT can_interface_sync_transfer(
    void* pIntDev,
    uint8_t chipSelect,
    uint8_t* txData,
    uint8_t* rxData,
    size_t size);

/*! @brief CAN SPI de-initialization for the FLIPPER
*
* This function will be called at driver de-initialization to de-initialize the interface driver SPI
* @param[in] *pComp Is the pointed structure of the device that call this function
* @return Returns an #eERRORRESULT value enum
*/
eERRORRESULT can_interface_sync_deinit(MCP251XFD* pComp);

    //********************************************************************************************************************

    /*! @brief CAN get millisecond
 *
 * This function will be called when the driver needs to get current millisecond
 */
    uint32_t can_interface_sync_get_currentms(void);

/*! @brief CAN compute CRC16-CMS
 *
 * This function will be called when a CRC16-CMS computation is needed (ie. in CRC mode or Safe Write). In normal mode, this can point to NULL.
 * @param[in] *data Is the pointed byte stream
 * @param[in] size Is the size of the pointed byte stream
 * @return The CRC computed
 */
uint16_t can_interface_sync_compute_crc16(const uint8_t* data, size_t size);
