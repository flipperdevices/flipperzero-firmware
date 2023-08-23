/*!*****************************************************************************
 * @file    CRC_CAN.h
 * @author  Fabien 'Emandhal' MAILLY
 * @version 1.0.0
 * @date    09/03/2020
 * @brief   CRC15-CAN, CRC17-CAN and CRC21-CAN implementations
 * @details
 *  The CRC15-CAN polynomial is x^15 + x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1 (0xC599)
 *   - Do not use RefIN and RefOUT, the initial value 0x0000
 *   - The result is XORed with 0x0000
 *   - http://reveng.sourceforge.net/crc-catalogue/1-15.htm#crc.cat-bits.15
 *  The CRC17-CAN polynomial is x^17 + x^16 + x^14 + x^13 + x^11 + x^6 + x^4 + x^3 + x^1 + 1 (0x3685B)
 *   - Do not use RefIN and RefOUT, the initial value 0x0000
 *   - The result is XORed with 0x0000
 *   - http://reveng.sourceforge.net/crc-catalogue/17plus.htm#crc.cat-bits.17
 *  The CRC21-CAN polynomial is x^21 + x^20 + x^13 + x^11 + x^7 + x^4 + x^3 + 1 (0x302899)
 *   - Do not use RefIN and RefOUT, the initial value 0x0000
 *   - The result is XORed with 0x0000
 *   - http://reveng.sourceforge.net/crc-catalogue/17plus.htm#crc.cat-bits.21
 ******************************************************************************/
#ifndef CRC_CAN_H_INC
#define CRC_CAN_H_INC
//=============================================================================

//-----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------------





/*! @brief Compute a bit stream with CRC15-CAN bit by bit (shift register)
 *
 * This function compute bit by bit the input stream because CAN messages are not all modulo 8
 * @param[in] *data Is the pointed byte stream. Bit stream should be left aligned in bytes
 * @param[in] bitSize Is the size in bits of the pointed byte stream. WARNING: the byte size of data stream have to have enough byte to contain all bits
 * @return The CRC computed
 */
uint16_t ComputeCRC15CAN(const uint8_t* data, size_t bitSize);


/*! @brief Partial compute a bit stream with CRC15-CAN bit by bit (shift register)
 *
 * This function compute bit by bit the input stream because CAN messages are not all modulo 8
 * @param[in] crc Is the initial shift register value
 * @param[in] *data Is the pointed byte stream. Bit stream should be left aligned in bytes
 * @param[in] bitSize Is the size in bits of the pointed byte stream. WARNING: the byte size of data stream have to have enough byte to contain all bits
 * @return The CRC computed
 */
uint16_t PartialComputeCRC15CAN(uint16_t crc, const uint8_t* data, size_t bitSize);





/*! @brief Compute a bit stream with CRC17-CAN bit by bit (shift register)
 *
 * This function compute bit by bit the input stream because CAN messages are not all modulo 8
 * @param[in] *data Is the pointed byte stream. Bit stream should be left aligned in bytes
 * @param[in] bitSize Is the size in bits of the pointed byte stream. WARNING: the byte size of data stream have to have enough byte to contain all bits
 * @return The CRC computed
 */
uint32_t ComputeCRC17CAN(const uint8_t* data, size_t bitSize);


/*! @brief Partial compute a bit stream with CRC17-CAN bit by bit (shift register)
 *
 * This function compute bit by bit the input stream because CAN messages are not all modulo 8
 * @param[in] crc Is the initial shift register value
 * @param[in] *data Is the pointed byte stream. Bit stream should be left aligned in bytes
 * @param[in] bitSize Is the size in bits of the pointed byte stream. WARNING: the byte size of data stream have to have enough byte to contain all bits
 * @return The CRC computed
 */
uint32_t PartialComputeCRC17CAN(uint32_t crc, const uint8_t* data, size_t bitSize);





/*! @brief Compute a bit stream with CRC21-CAN bit by bit (shift register)
 *
 * This function compute bit by bit the input stream because CAN messages are not all modulo 8
 * @param[in] *data Is the pointed byte stream. Bit stream should be left aligned in bytes
 * @param[in] bitSize Is the size in bits of the pointed byte stream. WARNING: the byte size of data stream have to have enough byte to contain all bits
 * @return The CRC computed
 */
uint32_t ComputeCRC21CAN(const uint8_t* data, size_t bitSize);


/*! @brief Partial compute a bit stream with CRC21-CAN bit by bit (shift register)
 *
 * This function compute bit by bit the input stream because CAN messages are not all modulo 8
 * @param[in] crc Is the initial shift register value
 * @param[in] *data Is the pointed byte stream. Bit stream should be left aligned in bytes
 * @param[in] bitSize Is the size in bits of the pointed byte stream. WARNING: the byte size of data stream have to have enough byte to contain all bits
 * @return The CRC computed
 */
uint32_t PartialComputeCRC21CAN(uint32_t crc, const uint8_t* data, size_t bitSize);





//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif /* CRC_CAN_H_INC */