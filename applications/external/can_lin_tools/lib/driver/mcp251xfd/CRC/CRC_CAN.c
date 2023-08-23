/*!*****************************************************************************
 * @file    CRC_CAN.c
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

//-----------------------------------------------------------------------------
#include "CRC_CAN.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
#include <cstdint>
extern "C" {
#endif
//-----------------------------------------------------------------------------





//=============================================================================
// Compute the CRC15-CAN from the bit stream in parameter
//=============================================================================
uint16_t ComputeCRC15CAN(const uint8_t* data, size_t bitSize)
{
#define CRC_nCRC  15
  if (data == NULL) return 0;
  if (bitSize == 0) return 0;
  const uint16_t Polynome = 0xC599;
  uint16_t crc = 0x0000; // Shift register start value

  uint16_t DataNextBit = 8;
  uint16_t CRCnext = 0;
  while (bitSize-- != 0)
  {
    DataNextBit--;
    CRCnext = ((*data >> DataNextBit) ^ (crc >> (CRC_nCRC - 1))) & 0x1;
    if (DataNextBit == 0) // If there is no bit left in this byte, get next one in the next byte of data stream
    {
      DataNextBit = 8;
      data++;
    }
    crc <<= 1;            // Shift register left by 1 position
    if (CRCnext == 1)     // Compute register with polynomial
    {
      crc ^= Polynome;
    }
  }

  return (crc & ((1 << CRC_nCRC) - 1));
#undef CRC_nCRC
}


//=============================================================================
// Partial compute the CRC15-CAN from the bit stream in parameter
//=============================================================================
uint16_t PartialComputeCRC15CAN(uint16_t crc, const uint8_t* data, size_t bitSize)
{
  #define CRC_nCRC  15
  if (data == NULL) return 0;
  if (bitSize == 0) return 0;
  const uint16_t Polynome = 0xC599;

  uint16_t DataNextBit = 8;
  uint16_t CRCnext = 0;
  while (bitSize-- != 0)
  {
    DataNextBit--;
    CRCnext = ((*data >> DataNextBit) ^ (crc >> (CRC_nCRC - 1))) & 0x1;
    if (DataNextBit == 0) // If there is no bit left in this byte, get next one in the next byte of data stream
    {
      DataNextBit = 8;
      data++;
    }
    crc <<= 1;            // Shift register left by 1 position
    if (CRCnext == 1)     // Compute register with polynomial
    {
      crc ^= Polynome;
    }
  }

  return (crc & ((1 << CRC_nCRC) - 1));
  #undef CRC_nCRC
}





//=============================================================================
// Compute the CRC17-CAN from the bit stream in parameter
//=============================================================================
uint32_t ComputeCRC17CAN(const uint8_t* data, size_t bitSize)
{
#define CRC_nCRC  17
  if (data == NULL) return 0;
  if (bitSize == 0) return 0;
  const uint32_t Polynome = 0x3685B;
  uint32_t crc = 0x00000000; // Shift register start value

  uint32_t DataNextBit = 8;
  uint32_t CRCnext = 0;
  while (bitSize-- != 0)
  {
    DataNextBit--;
    CRCnext = ((*data >> DataNextBit) ^ (crc >> (CRC_nCRC - 1))) & 0x1;
    if (DataNextBit == 0) // If there is no bit left in this byte, get next one in the next byte of data stream
    {
      DataNextBit = 8;
      data++;
    }
    crc <<= 1;            // Shift register left by 1 position
    if (CRCnext == 1)     // Compute register with polynomial
    {
      crc ^= Polynome;
    }
  }

  return (crc & ((1 << CRC_nCRC) - 1));
#undef CRC_nCRC
}


//=============================================================================
// Partial compute the CRC17-CAN from the bit stream in parameter
//=============================================================================
uint32_t PartialComputeCRC17CAN(uint32_t crc, const uint8_t* data, size_t bitSize)
{
  #define CRC_nCRC  17
  if (data == NULL) return 0;
  if (bitSize == 0) return 0;
  const uint32_t Polynome = 0x3685B;

  uint32_t DataNextBit = 8;
  uint32_t CRCnext = 0;
  while (bitSize-- != 0)
  {
    DataNextBit--;
    CRCnext = ((*data >> DataNextBit) ^ (crc >> (CRC_nCRC - 1))) & 0x1;
    if (DataNextBit == 0) // If there is no bit left in this byte, get next one in the next byte of data stream
    {
      DataNextBit = 8;
      data++;
    }
    crc <<= 1;            // Shift register left by 1 position
    if (CRCnext == 1)     // Compute register with polynomial
    {
      crc ^= Polynome;
    }
  }

  return (crc & ((1 << CRC_nCRC) - 1));
  #undef CRC_nCRC
}





//=============================================================================
// Compute the CRC21-CAN from the bit stream in parameter
//=============================================================================
uint32_t ComputeCRC21CAN(const uint8_t* data, size_t bitSize)
{
#define CRC_nCRC  21
  if (data == NULL) return 0;
  if (bitSize == 0) return 0;
  const uint32_t Polynome = 0x302899;
  uint32_t crc = 0x00000000; // Shift register start value

  uint32_t DataNextBit = 8;
  uint32_t CRCnext = 0;
  while (bitSize-- != 0)
  {
    DataNextBit--;
    CRCnext = ((*data >> DataNextBit) ^ (crc >> (CRC_nCRC - 1))) & 0x1;
    if (DataNextBit == 0) // If there is no bit left in this byte, get next one in the next byte of data stream
    {
      DataNextBit = 8;
      data++;
    }
    crc <<= 1;            // Shift register left by 1 position
    if (CRCnext == 1)     // Compute register with polynomial
    {
      crc ^= Polynome;
    }
  }

  return (crc & ((1 << CRC_nCRC) - 1));
#undef CRC_nCRC
}


//=============================================================================
// Partial compute the CRC21-CAN from the bit stream in parameter
//=============================================================================
uint32_t PartialComputeCRC21CAN(uint32_t crc, const uint8_t* data, size_t bitSize)
{
  #define CRC_nCRC  21
  if (data == NULL) return 0;
  if (bitSize == 0) return 0;
  const uint32_t Polynome = 0x302899;

  uint32_t DataNextBit = 8;
  uint32_t CRCnext = 0;
  while (bitSize-- != 0)
  {
    DataNextBit--;
    CRCnext = ((*data >> DataNextBit) ^ (crc >> (CRC_nCRC - 1))) & 0x1;
    if (DataNextBit == 0) // If there is no bit left in this byte, get next one in the next byte of data stream
    {
      DataNextBit = 8;
      data++;
    }
    crc <<= 1;            // Shift register left by 1 position
    if (CRCnext == 1)     // Compute register with polynomial
    {
      crc ^= Polynome;
    }
  }

  return (crc & ((1 << CRC_nCRC) - 1));
  #undef CRC_nCRC
}





//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------