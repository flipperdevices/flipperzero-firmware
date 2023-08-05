/*******************************************************************************
    File name:    CRC16_USB.h
    Author:       FMA
    Version:      1.0
    Date (d/m/y): 09/03/2020
    Description:  CRC16-USB implementation
                  The CRC16_USB polynomial is x^16 + x^15 + x^5 + 1 (0x8005)
                  Use RefIN and RefOUT, the initial value 0xFFFF
                  The result is XORed with 0xFFFF
          http://reveng.sourceforge.net/crc-catalogue/16.htm

    History :
*******************************************************************************/
#ifndef CRC16_USB_H_INC
#define CRC16_USB_H_INC
//=============================================================================

//-----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
//-----------------------------------------------------------------------------
/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond
//-----------------------------------------------------------------------------





#ifndef CRC16USB_NOTABLE
/*! @brief Compute a byte stream with CRC16-USB with a table
 *
 * @param[in] *data Is the pointed byte stream
 * @param[in] size Is the size of the pointed byte stream
 * @return The CRC computed
 */
uint16_t ComputeCRC16USB(const uint8_t* data, uint32_t size);

#else

/*! @brief Compute a byte stream with CRC16-USB without a table
 *
 * @param[in] *data Is the pointed byte stream
 * @param[in] size Is the size of the pointed byte stream
 * @return The CRC computed
 */
uint16_t ComputeCRC16USB(const uint8_t* data, uint16_t size);

#endif





//-----------------------------------------------------------------------------
/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
//-----------------------------------------------------------------------------
#endif /* CRC16_USB_H_INC */