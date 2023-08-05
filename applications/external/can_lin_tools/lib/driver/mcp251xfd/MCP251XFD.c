/*!*****************************************************************************
 * @file    MCP251XFD.c
 * @author  Fabien 'Emandhal' MAILLY
 * @version 1.0.6
 * @date    16/04/2023
 * @brief   MCP251XFD driver
 * @details
 * The MCP251XFD component is a CAN-bus controller supporting CAN2.0A, CAN2.0B
 * and CAN-FD with SPI interface
 * Follow datasheet MCP2517FD Rev.B (July 2019)
 *                  MCP2518FD Rev.B (Dec  2020)
 *                  MCP251863 Rev.A (Sept 2022) [Have a MCP2518FD inside]
 * Follow MCP25XXFD Family Reference Manual (DS20005678D)
 ******************************************************************************/

//-----------------------------------------------------------------------------
#include "MCP251XFD.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
#  include <cstdint>
extern "C" {
#endif
//-----------------------------------------------------------------------------





//=============================================================================
// Prototypes for private functions
//=============================================================================
// Test all RAM address of the MCP251XFD for the driver flag DRIVER_INIT_CHECK_RAM
static eERRORRESULT __MCP251XFD_TestRAM(MCP251XFD *pComp);
//-----------------------------------------------------------------------------
#define MCP251XFD_USE_SID11             ( (pComp->InternalConfig & (MCP251XFD_CANFD_USE_RRS_BIT_AS_SID11 | MCP251XFD_CANFD_ENABLED)) == (MCP251XFD_CANFD_USE_RRS_BIT_AS_SID11 | MCP251XFD_CANFD_ENABLED) )
#define MCP251XFD_TIME_DIFF(begin,end)  ( ((end) >= (begin)) ? ((end) - (begin)) : (UINT32_MAX - ((begin) - (end) - 1)) ) // Works only if time difference is strictly inferior to (UINT32_MAX/2) and call often
//-----------------------------------------------------------------------------





//**********************************************************************************************************************************************************
//=============================================================================
// MCP251XFD device initialization
//=============================================================================
eERRORRESULT Init_MCP251XFD(MCP251XFD *pComp, const MCP251XFD_Config *pConf)
{
#ifdef CHECK_NULL_PARAM
  if ((pComp == NULL) || (pConf == NULL)) return ERR__PARAMETER_ERROR;
  if (pComp->fnSPI_Init == NULL) return ERR__PARAMETER_ERROR;
  if (pComp->fnGetCurrentms  == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;
  uint32_t Result = 0;
  pComp->InternalConfig = 0;

  //--- Check configuration ---------------------------------
  if ((pConf->XtalFreq != 0) && (pConf->XtalFreq < MCP251XFD_XTALFREQ_MIN)) return ERR__FREQUENCY_ERROR; // The device crystal should not be <  4MHz
  if ((pConf->XtalFreq != 0) && (pConf->XtalFreq > MCP251XFD_XTALFREQ_MAX)) return ERR__FREQUENCY_ERROR; // The device crystal should not be > 40MHz
  if ((pConf->OscFreq  != 0) && (pConf->OscFreq  < MCP251XFD_OSCFREQ_MIN )) return ERR__FREQUENCY_ERROR; // The device oscillator should not be <  2MHz
  if ((pConf->OscFreq  != 0) && (pConf->OscFreq  > MCP251XFD_OSCFREQ_MAX )) return ERR__FREQUENCY_ERROR; // The device oscillator should not be > 40MHz
  uint32_t CompFreq = 0;
  if (pConf->XtalFreq != 0) CompFreq = pConf->XtalFreq; else CompFreq = pConf->OscFreq;                  // Select the component frequency
  if (CompFreq == 0) return ERR__CONFIGURATION;                                                          // Both XtalFreq and OscFreq are configured to 0

  //--- Configure SPI Interface -----------------------------
  if (pComp->SPIClockSpeed > MCP251XFD_SPICLOCK_MAX) return ERR__SPI_FREQUENCY_ERROR;                    // The SPI clock should not be superior to the SPI clock max
  if ((pComp->DriverConfig & MCP251XFD_DRIVER_SAFE_RESET) == 0)
  {
    Error = pComp->fnSPI_Init(pComp->InterfaceDevice, pComp->SPI_ChipSelect, pComp->SPIClockSpeed);      // Initialize the SPI interface only in case of no safe reset (the interface will be initialized in the reset at a safe speed)
    if (Error != ERR_OK) return Error;                                                                   // If there is an error while calling fnSPI_Init() then return the error
  }

  //--- Reset -----------------------------------------------
  Error = MCP251XFD_ResetDevice(pComp);                                                                  // Reset the device
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while calling MCP251XFD_ResetDevice() then return the error
  pComp->InternalConfig = MCP251XFD_DEV_PS_SET(MCP251XFD_DEVICE_SLEEP_NOT_CONFIGURED);                   // Device is in normal power state, sleep is not yet configured

  //--- Test SPI connection ---------------------------------
  Error = MCP251XFD_WriteRAM32(pComp, (MCP251XFD_RAM_ADDR + MCP251XFD_RAM_SIZE - 4), 0xAA55AA55);        // Write 0xAA55AA55 at address
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while writing the RAM address then return the error
  Error = MCP251XFD_ReadRAM32(pComp, (MCP251XFD_RAM_ADDR + MCP251XFD_RAM_SIZE - 4), &Result);            // Read again the data
  if ((Error == ERR__CRC_ERROR) || (Result != 0xAA55AA55)) return ERR__NO_DEVICE_DETECTED;               // If CRC mismatch or data read is not 0xAA55AA55 then no device is detected
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while reading the RAM address then return the error

  //--- Configure component clock ---------------------------
  uint8_t Config = MCP251XFD_SFR_OSC_WAKEUP | MCP251XFD_SFR_OSC8_SCLKDIV_SET(MCP251XFD_SCLK_DivBy1) | MCP251XFD_SFR_OSC8_PLLDIS;
  if ((pConf->SysclkConfig == MCP251XFD_SYSCLK_IS_CLKIN_MUL_5) || (pConf->SysclkConfig == MCP251XFD_SYSCLK_IS_CLKIN_MUL_10))
  { Config |= MCP251XFD_SFR_OSC8_PLLEN; CompFreq *= 10; }                                                // Activate the 10x PLL from Xtal/Resonator frequency @4MHz or from oscillator frequency @2MHz
  if (CompFreq > MCP251XFD_CLKINPLL_MAX) return ERR__FREQUENCY_ERROR;                                    // If component clock (CLKIN+PLL) is too high then return an error
  if ((pConf->SysclkConfig == MCP251XFD_SYSCLK_IS_CLKIN_DIV_2) || (pConf->SysclkConfig == MCP251XFD_SYSCLK_IS_CLKIN_MUL_5))
  { Config |= MCP251XFD_SFR_OSC8_SCLKDIV_SET(MCP251XFD_SCLK_DivBy2); CompFreq /= 2; }                    // Configure CLKIN+PLL divisor from Xtal/Resonator and PLL or oscillator frequency
  if (pConf->SYSCLK_Result != NULL) *pConf->SYSCLK_Result = CompFreq;                                    // Save the internal SYSCLK if needed
  if (CompFreq > MCP251XFD_SYSCLK_MAX) return ERR__FREQUENCY_ERROR;                                      // If component clock (SYSCLK) is too high then return an error
  if (pConf->ClkoPinConfig != MCP251XFD_CLKO_SOF) Config |= MCP251XFD_SFR_OSC8_CLKODIV_SET(pConf->ClkoPinConfig); // Configure the CLKO pin (CLKIN+PLL div by 1, 2, 4, 10 or Start Of Frame)
  Config |= MCP251XFD_SFR_OSC8_LPMEN;                                                                    // Set now the Low Power Mode for further check of which module MCP251XFD it is
  Error = MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_OSC_CONFIG, Config);                                   // Write the Oscillator Register configuration
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while calling MCP251XFD_WriteSFR8() then return the error

  //--- Check clocks stabilization --------------------------
  uint8_t CheckVal = ((uint8_t)(Config) & MCP251XFD_SFR_OSC8_CHECKFLAGS) | MCP251XFD_SFR_OSC8_OSCRDY;    // Check if PLL Locked (if enabled), OSC clock is running and stable, and SCLKDIV is synchronized (if divided by 2)
  uint32_t StartTime = pComp->fnGetCurrentms();                                                          // Start the timeout
  while (true)
  {
    Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_OSC_CHECK, &Config);                                  // Read current OSC register mode with
    if (Error != ERR_OK) return Error;                                                                   // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
    if ((Config & MCP251XFD_SFR_OSC8_CHECKFLAGS) == CheckVal) break;                                     // Check if the controller's clocks are ready
    if (MCP251XFD_TIME_DIFF(StartTime, pComp->fnGetCurrentms()) > 4) return ERR__DEVICE_TIMEOUT;         // Wait at least 3ms (see TOSCSTAB in Table 7-3 from datasheet Electrical Specifications) + 1ms because GetCurrentms can be 1 cycle before the new ms. If timeout then return the error
  }

  //--- Set desired SPI clock speed -------------------------
  uint32_t CompFreqTemp = (((CompFreq >> 1) * 85) / 100);
  if (pComp->SPIClockSpeed > CompFreqTemp) return ERR__SPI_FREQUENCY_ERROR;            // Ensure that FSCK is less than or equal to 0.85*(FSYSCLK/2). Follows datasheets errata for: The SPI can write corrupted data to the RAM at fast SPI speeds
  if ((pComp->DriverConfig & MCP251XFD_DRIVER_SAFE_RESET) > 0)
  {
    Error = pComp->fnSPI_Init(pComp->InterfaceDevice, pComp->SPI_ChipSelect, pComp->SPIClockSpeed);      // Set the SPI speed clock to desired clock speed
    if (Error != ERR_OK) return Error;                                                                   // If there is an error while changing SPI interface speed then return the error
  }

  //--- Configure CRC Interrupts ----------------------------
  if ((pComp->DriverConfig & MCP251XFD_DRIVER_USE_READ_WRITE_CRC) > 0)                                   // If there is a DRIVER_USE_READ_WRITE_CRC flag then
  {
    Error = MCP251XFD_ConfigureCRC(pComp, MCP251XFD_CRC_ALL_EVENTS);                                     // Configure the CRC and all interrupts related to CRC
    if (Error != ERR_OK) return Error;                                                                   // If there is an error while calling MCP251XFD_ConfigureCRC() then return the error
  }

  //--- Check which MCP251XFD it is -------------------------                                            // Since the DEVID register return the same value for MCP2517FD and MCP2518FD, this driver use the OSC.LPMEN to check which one it is
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_OSC_CONFIG, &Config);                                   // Read current OSC config mode
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  pComp->InternalConfig |= MCP251XFD_DEV_ID_SET((Config & MCP251XFD_SFR_OSC8_LPMEN) > 0 ? MCP2518FD : MCP2517FD); // Set which one it is to the internal config of the driver
  Config &= ~MCP251XFD_SFR_OSC8_LPMEN;
  Error = MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_OSC_CONFIG, Config);                                   // Write the OSC config mode with the LPM bit cleared
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while calling MCP251XFD_WriteSFR8() then return the error

  //--- Test SPI connection and RAM Test --------------------
  if ((pComp->DriverConfig & MCP251XFD_DRIVER_INIT_CHECK_RAM) > 0)                                       // If there is a DRIVER_INIT_CHECK_RAM flag then
  {
    Error = __MCP251XFD_TestRAM(pComp);                                                                  // Check the all the RAM only of the device
    if (Error != ERR_OK) return Error;                                                                   // If there is an error while calling __MCP251XFD_TestRAM() then return the error
  }
  else                                                                                                   // Else check only SPI interface
  {
    Error = MCP251XFD_WriteRAM32(pComp, (MCP251XFD_RAM_ADDR + MCP251XFD_RAM_SIZE - 4), 0xAA55AA55);      // Write 0xAA55AA55 at address
    if (Error != ERR_OK) return Error;                                                                   // If there is an error while writing the RAM address then return the error
    Error = MCP251XFD_ReadRAM32(pComp, (MCP251XFD_RAM_ADDR + MCP251XFD_RAM_SIZE - 4), &Result);          // Read again the data
    if (Error != ERR_OK) return Error;                                                                   // If there is an error while reading the RAM address then return the error
    if (Result != 0xAA55AA55) return ERR__RAM_TEST_FAIL;                                                 // If data read is not 0xAA55AA55 then return an error
  }

  //--- Configure RAM ECC -----------------------------------
  if ((pComp->DriverConfig & MCP251XFD_DRIVER_ENABLE_ECC) > 0)                                           // If there is a DRIVER_ENABLE_ECC flag then
  {
    Error = MCP251XFD_ConfigureECC(pComp, true, MCP251XFD_ECC_ALL_EVENTS, 0x55);                         // Configure the ECC and enable all interrupts related to ECC
    if (Error != ERR_OK) return Error;                                                                   // If there is an error while calling MCP251XFD_ConfigureECC() then return the error
  }

  //--- Initialize RAM if configured ------------------------
  if ((pComp->DriverConfig & MCP251XFD_DRIVER_INIT_SET_RAM_AT_0) > 0)                                    // If there is a DRIVER_INIT_SET_RAM_AT_0 flag then
  {
    Error = MCP251XFD_InitRAM(pComp);                                                                    // Initialize all RAM addresses with 0x00000000
    if (Error != ERR_OK) return Error;                                                                   // If there is an error while calling MCP251XFD_InitRAM() then return the error
  }

  //--- Initialize Int pins or GPIOs ------------------------
  Error = MCP251XFD_SetGPIOPinsOutputLevel(pComp, pComp->GPIOsOutState, MCP251XFD_GPIO0_Mask | MCP251XFD_GPIO1_Mask); // Set GPIO pins output level before change to mode GPIO. This is to get directly the good output level when (if) pins will be in output mode
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while calling MCP251XFD_ConfigurePins() then return the error
  Error = MCP251XFD_ConfigurePins(pComp, pConf->GPIO0PinMode, pConf->GPIO1PinMode, pConf->INTsOutMode, pConf->TXCANOutMode, (pConf->ClkoPinConfig == MCP251XFD_CLKO_SOF)); // Configure pins
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while calling MCP251XFD_ConfigurePins() then return the error

  //--- Set Nominal and Data bitrate ------------------------
  MCP251XFD_BitTimeConfig ConfBitTime;
  ConfBitTime.Stats = pConf->BitTimeStats;
  Error = MCP251XFD_CalculateBitTimeConfiguration(CompFreq, pConf->NominalBitrate, pConf->DataBitrate, &ConfBitTime); // Calculate Bit Time
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while calling MCP251XFD_CalculateBitTimeConfiguration() then return the error
  Error = MCP251XFD_SetBitTimeConfiguration(pComp, &ConfBitTime, pConf->DataBitrate == MCP251XFD_NO_CANFD); // Set Bit Time configuration to registers
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while calling MCP251XFD_SetBitTimeConfiguration() then return the error

  //--- CAN configuration -----------------------------------
  Error = MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CiCON+2, 0x00);                                        // Disable TEF and TXQ configuration in the RegMCP251XFD_CiCON register (Third byte only)
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while calling MCP251XFD_WriteSFR8() then return the error
  Error = MCP251XFD_ConfigureCANController(pComp, pConf->ControlFlags, pConf->Bandwidth);                // Configure the CAN control
  if (Error != ERR_OK) return Error;                                                                     // If there is an error while calling MCP251XFD_ConfigureCANControl() then return the error

  //--- System interrupt enable -----------------------------
  Error = MCP251XFD_ConfigureInterrupt(pComp, (setMCP251XFD_InterruptEvents)pConf->SysInterruptFlags);   // Configure interrupts to enable
  return Error;
}





//=============================================================================
// [STATIC] RAM initialization of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_InitRAM(MCP251XFD *pComp)
{
  eERRORRESULT Error;

  for (uint16_t Address = MCP251XFD_RAM_ADDR; Address < (MCP251XFD_RAM_ADDR + MCP251XFD_RAM_SIZE); Address += 4)
  {
    Error = MCP251XFD_WriteRAM32(pComp, Address, 0x00000000); // Write 0x00000000 at address
    if (Error != ERR_OK) return Error;                        // If there is an error while writing the RAM address then return the error
  }
  return ERR_OK;
}





//**********************************************************************************************************************************************************
//=============================================================================
// Get actual device of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetDeviceID(MCP251XFD *pComp, eMCP251XFD_Devices* device, uint8_t* deviceId, uint8_t* deviceRev)
{
#ifdef CHECK_NULL_PARAM
  if ((pComp == NULL) || (device == NULL)) return ERR__PARAMETER_ERROR;
#endif
  *device = (eMCP251XFD_Devices)MCP251XFD_DEV_ID_GET(pComp->InternalConfig); // Get device found when initializing the module
  eERRORRESULT Error;

  if ((deviceId != NULL) || (deviceRev != NULL))
  {
    uint8_t Value;
    Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_DEVID, &Value);           // Read value of the DEVID register (First byte only)
    if (Error != ERR_OK) return Error;                                       // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
    if (deviceId  != NULL) *deviceId  = MCP251XFD_SFR_DEVID8_ID_GET(Value);  // Get Device ID
    if (deviceRev != NULL) *deviceRev = MCP251XFD_SFR_DEVID8_REV_GET(Value); // Get Device Revision
  }
  return ERR_OK;
}





//**********************************************************************************************************************************************************
//=============================================================================
// Read data from the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ReadData(MCP251XFD *pComp, uint16_t address, uint8_t* data, uint16_t size)
{
#ifdef CHECK_NULL_PARAM
  if ((pComp == NULL) || (data == NULL))  return ERR__PARAMETER_ERROR;
  if (pComp->fnSPI_Transfer == NULL) return ERR__PARAMETER_ERROR;
#endif
  const bool UseCRC = ((pComp->DriverConfig & MCP251XFD_DRIVER_USE_READ_WRITE_CRC) > 0);
  const bool InRAM  = (address >= MCP251XFD_RAM_ADDR) && (address < (MCP251XFD_RAM_ADDR + MCP251XFD_RAM_SIZE)); // True if address is in the RAM region ; False if address is in Controller SFR or CAN SFR region
  if (address > MCP251XFD_END_ADDR) return ERR__PARAMETER_ERROR;
  uint8_t Buffer[MCP251XFD_TRANS_BUF_SIZE];
  eERRORRESULT Error;

  //--- Define the Increment value ---
  uint16_t Increment = MCP251XFD_TRANS_BUF_SIZE - (UseCRC ? 5 : 2);          // If use CRC for read, Buffer size minus 2 for Command, minus 1 for length, minus 2 for CRC, else just 2 for command
  if (UseCRC && InRAM)                                                       // In RAM region and use CRC for read?
  {
    if ((size & 0b11) != 0) return ERR__DATA_MODULO;                         // size should be a multiple of 4 in case of Write CRC or Safe Write
    Increment &= 0xFFFC;                                                     // If in RAM region then the increment should be the nearest less than or equal value multiple by 4
  }                                                                          // Here Increment cannot be 0 because MCP251XFD_TRANS_BUF_SIZE is compiler protected to be not less than 9

  //--- Create all clusters of data and send them ---
  uint8_t* pBuf;
  size_t BufRemain = 0;
  size_t ByteCount;
  while (size > 0)
  {
    const uint16_t Addr = MCP251XFD_SPI_16BITS_WORD((UseCRC ? MCP251XFD_SPI_INSTRUCTION_READ_CRC : MCP251XFD_SPI_INSTRUCTION_READ), address);
    //--- Compose SPI command ---
    pBuf = &Buffer[0];
    *pBuf = ((Addr >> 8) & 0xFF);                                            // Set first byte of SPI command
    ++pBuf;
    *pBuf = Addr & 0xFF;                                                     // Set next byte of SPI command
    ++pBuf;
    ByteCount = (size > Increment ? Increment : size);                       // Define byte count to send

    //--- If needed, set 0x00 byte while reading data on SPI interface else send garbage data ---
    if ((pComp->DriverConfig & MCP251XFD_DRIVER_CLEAR_BUFFER_BEFORE_READ) > 0)
    {
      const size_t BuffUsedSize = ByteCount + (UseCRC ? 2 + 1 - 2 : 2);      // Here 2 for Command + 1 for Length - 2 for CRC (CRC on the SPI send is not checked by the controller thus will be set to 0 too), else just 2 for command
      for (size_t z = 2; z < BuffUsedSize; ++z) Buffer[z] = 0x00;            // Set to 0
    }

    // Set length of data
    uint8_t LenData = 0;
    if (UseCRC && InRAM) LenData = (ByteCount >> 2) & 0xFF;                  // If use CRC for read and in RAM, set how many data word that is requested
    else LenData = ByteCount & 0xFF;                                         // Set how many data byte that is requested
    if (UseCRC) { *pBuf = LenData; ++pBuf; }                                 // Add Len in the frame if use CRC

    //--- Now send the data through SPI interface ---
    const size_t ByteToReadCount = ByteCount + (UseCRC ? (2 + 1 + 2) : 2);   // In case of use CRC for read, here are 2 bytes for Command + 1 for Length + 2 for CRC, else just 2 for command
    Error = pComp->fnSPI_Transfer(pComp->InterfaceDevice, pComp->SPI_ChipSelect, &Buffer[0], &Buffer[0], ByteToReadCount); // Transfer the data in the buffer
    if (Error != ERR_OK) return Error;                                       // If there is an error while transferring data then return the error

    //--- Copy buffer to data ---
    BufRemain = ByteCount;                                                   // Set how many data that will fit in the buffer
    while ((BufRemain > 0) && (size > 0))
    {
      *data = *pBuf;                                                         // Copy data
      ++pBuf;
      ++data;
      --BufRemain;
      --size;
      ++address;
    }

  //--- Check CRC ---
  if (UseCRC)
  {
      // Replace Head data of the Tx buffer into the Rx buffer
      Buffer[0] = (uint8_t)((Addr >> 8) & 0xFF);
      Buffer[1] = (uint8_t)((Addr >> 0) & 0xFF);
      Buffer[2] = LenData;

      // Compute CRC and compare to the one in the buffer
#ifdef CHECK_NULL_PARAM
      if (pComp->fnComputeCRC16 == NULL) return ERR__PARAMETER_ERROR;        // If the CRC function is not present, raise an error
#endif
      uint16_t CRC = pComp->fnComputeCRC16(&Buffer[0], (ByteCount + 2 + 1)); // Compute CRC on the Buffer data (2 for Command + 1 for Length)
      uint16_t BufCRC = *pBuf << 8;                                          // Get CRC MSB on the next buffer byte
      ++pBuf;
      BufCRC |= *pBuf;                                                       // Get CRC LSB on the next buffer byte
      if (CRC != BufCRC) return ERR__CRC_ERROR;                              // If CRC mismatch, then raise an error
    }
  }

  return ERR_OK;
}



//=============================================================================
// Write data to the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_WriteData(MCP251XFD *pComp, uint16_t address, const uint8_t* data, uint16_t size)
{
#ifdef CHECK_NULL_PARAM
  if ((pComp == NULL) || (data == NULL))  return ERR__PARAMETER_ERROR;
  if (pComp->fnSPI_Transfer == NULL) return ERR__PARAMETER_ERROR;
#endif
  const bool UseCRC  = ((pComp->DriverConfig & (MCP251XFD_DRIVER_USE_READ_WRITE_CRC | MCP251XFD_DRIVER_USE_SAFE_WRITE)) > 0);
  const bool UseSafe = ((pComp->DriverConfig & MCP251XFD_DRIVER_USE_SAFE_WRITE) > 0);
  const bool InRAM   = (address >= MCP251XFD_RAM_ADDR) && (address < (MCP251XFD_RAM_ADDR + MCP251XFD_RAM_SIZE)); // True if address is in the RAM region ; False if address is in Controller SFR or CAN SFR region
  if (address > MCP251XFD_END_ADDR) return ERR__PARAMETER_ERROR;
  uint8_t Buffer[MCP251XFD_TRANS_BUF_SIZE];
  uint32_t Increment;
  eERRORRESULT Error;

  //--- Define the Increment value ---
  uint32_t Instruction = (UseCRC ? MCP251XFD_SPI_INSTRUCTION_WRITE_CRC : MCP251XFD_SPI_INSTRUCTION_WRITE);
  if (UseSafe == false)
  {
    Increment = MCP251XFD_TRANS_BUF_SIZE - (UseCRC ? 5 : 2);                // If use CRC for writeBuffer size minus 2 for Command, minus 1 for length, minus 2 for CRC, else just 2 for command
    if (UseCRC && InRAM)                                                    // In RAM region and use CRC for write
    {
      if ((size & 0b11) != 0) return ERR__DATA_MODULO;                      // size should be a multiple of 4 in case of Write CRC or Safe Write
      Increment &= 0xFFFC;                                                  // If in RAM region then the increment should be the nearest less than or equal value multiple by 4
    }                                                                       // Here Increment cannot be 0 because MCP251XFD_TRANS_BUF_SIZE is compiler protected to be not less than 9
  }
  else
  {
    Instruction = MCP251XFD_SPI_INSTRUCTION_SAFE_WRITE;                     // Set safe write instruction
    if (InRAM)
    {
      if ((size & 0b11) != 0) return ERR__DATA_MODULO;                      // size should be a multiple of 4 in case of Write CRC or Safe Write
      Increment = 4;                                                        // If use safe write and in RAM then the increment is 4 bytes
    }
    else Increment = 1;                                                     // If use safe write and in SFR then the increment is 1 byte
  }

  //--- Create all clusters of data and send them ---
  uint8_t* pBuf;
  size_t BufRemain = 0;
  size_t ByteCount = 0;
  while (size > 0)
  {
    const uint16_t Addr = MCP251XFD_SPI_16BITS_WORD(Instruction , address);
    //--- Compose SPI command ---
    pBuf = &Buffer[0];
    *pBuf = ((Addr >> 8) & 0xFF);                                           // Set first byte of SPI command
    ++pBuf;
    *pBuf = Addr & 0xFF;                                                    // Set next byte of SPI command
    ++pBuf;    

    //--- Set length of data ---
    ByteCount = (size > Increment ? Increment : size);                      // Define byte count to send
    if (UseCRC && (UseSafe == false))                                       // Add Len in the frame if use CRC but not use safe
    {                        
      if (InRAM) *pBuf = (ByteCount >> 2) & 0xFF;                           // If use CRC for write and in RAM, set how many data word that is requested
      else *pBuf = ByteCount & 0xFF;                                        // Set how many data byte that is requested
      ++pBuf;
    }

    //--- Copy data to buffer ---
    BufRemain = Increment;                                                  // Set how many data that will fit in the buffer
    while ((BufRemain > 0) && (size > 0))
    {
      *pBuf = *data;                                                        // Copy data
      ++pBuf;
      ++data;
      --BufRemain;
      --size;
      ++address;
    }

    //--- Compute CRC and add to the buffer ---
    if (UseCRC)
    {
#ifdef CHECK_NULL_PARAM
      if (pComp->fnComputeCRC16 == NULL) return ERR__PARAMETER_ERROR;       // If the CRC function is not present, raise an error
#endif
      const size_t ByteToComputeCount = ByteCount + (UseSafe ? 2 : (2 + 1));
      uint16_t CRC = pComp->fnComputeCRC16(&Buffer[0], ByteToComputeCount); // Compute CRC on the Buffer data (2 for Command + 1 for Length for not safe)
      *pBuf = (CRC >> 8) & 0xFF;                                            // Put CRC MSB on the next buffer byte
      ++pBuf;
      *pBuf = CRC & 0xFF;                                                   // Put CRC LSB on the next buffer byte
    }

    //--- Now send the data through SPI interface ---
    const size_t ByteToWriteCount = ByteCount + (UseSafe ? (2 + 2) : (UseCRC ? (2 + 1 + 2) : 2)); // In case of use CRC for write here are 2 bytes for Command + 1 for Length for not safe + 2 for CRC, else just 2 for command
    Error = pComp->fnSPI_Transfer(pComp->InterfaceDevice, pComp->SPI_ChipSelect, &Buffer[0], NULL, ByteToWriteCount); // Transfer the data in the buffer (2 for Command + 1 for Length + 2 for CRC)
    if (Error != ERR_OK) return Error;                                      // If there is an error while transferring data then return the error
  }

  return ERR_OK;
}





//**********************************************************************************************************************************************************
//=============================================================================
// Transmit a message object (with data) to the FIFO of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_TransmitMessageObjectToFIFO(MCP251XFD *pComp, uint8_t* messageObjectToSend, uint8_t objectSize, eMCP251XFD_FIFO toFIFO, bool andFlush)
{
#ifdef CHECK_NULL_PARAM
  if ((pComp == NULL) || (messageObjectToSend == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (toFIFO == MCP251XFD_TEF) return ERR__PARAMETER_ERROR;
  if ((objectSize & 0x3) > 0) return ERR__BYTE_COUNT_MODULO_4;
  eERRORRESULT Error;

  //--- Get address where to write the frame ---
  uint32_t NextAddress = 0;
  Error = MCP251XFD_GetNextMessageAddressFIFO(pComp, toFIFO, &NextAddress, NULL);        // Get next message address
  if (Error != ERR_OK) return Error;                                                     // If there is an error while calling MCP251XFD_GetNextMessageAddressFIFO() then return the error
  NextAddress += MCP251XFD_RAM_ADDR;

  //--- Write data to RAM ---
  Error =  MCP251XFD_WriteData(pComp, NextAddress, &messageObjectToSend[0], objectSize); // Write data to RAM address
  if (Error != ERR_OK) return Error;                                                     // If there is an error while calling MCP251XFD_WriteData() then return the error

  //--- Update FIFO and flush if asked ---
  return MCP251XFD_UpdateFIFO(pComp, toFIFO, andFlush);
}



//=============================================================================
// Transmit a message to a FIFO of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_TransmitMessageToFIFO(MCP251XFD *pComp, MCP251XFD_CANMessage* messageToSend, eMCP251XFD_FIFO toFIFO, bool andFlush)
{
#ifdef CHECK_NULL_PARAM
  if ((pComp == NULL) || (messageToSend == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (toFIFO == MCP251XFD_TEF) return ERR__PARAMETER_ERROR;
  uint8_t Buffer[MCP251XFD_CAN_TX_MESSAGE_SIZE_MAX];
  MCP251XFD_CAN_TX_Message* Message = (MCP251XFD_CAN_TX_Message*)Buffer; // The first 8 bytes represent the MCP251XFD_CAN_TX_Message struct

  //--- Fill message ID (T0) ---
  bool Extended   = ((messageToSend->ControlFlags & MCP251XFD_EXTENDED_MESSAGE_ID) > 0);
  bool CANFDframe = ((messageToSend->ControlFlags & MCP251XFD_CANFD_FRAME        ) > 0);
  Message->T0.T0  = MCP251XFD_MessageIDtoObjectMessageIdentifier(messageToSend->MessageID, Extended, MCP251XFD_USE_SID11 && CANFDframe);

  //--- Fill message controls (T1) ---
  Message->T1.T1 = 0;                          // Initialize message Controls to 0
  Message->T1.SEQ = messageToSend->MessageSEQ; // Set message Sequence
  if (CANFDframe                                                          )      Message->T1.FDF = 1;
  if ((messageToSend->ControlFlags & MCP251XFD_SWITCH_BITRATE             ) > 0) Message->T1.BRS = 1;
  if ((messageToSend->ControlFlags & MCP251XFD_REMOTE_TRANSMISSION_REQUEST) > 0) Message->T1.RTR = 1;
  if ((messageToSend->ControlFlags & MCP251XFD_EXTENDED_MESSAGE_ID        ) > 0) Message->T1.IDE = 1;
  if ((messageToSend->ControlFlags & MCP251XFD_TRANSMIT_ERROR_PASSIVE     ) > 0) Message->T1.ESI = 1;
  Message->T1.DLC = (uint8_t)messageToSend->DLC;

  //--- Fill payload data ---
  if ((messageToSend->DLC != MCP251XFD_DLC_0BYTE) && (messageToSend->PayloadData == NULL)) return ERR__NO_DATA_AVAILABLE;
  uint8_t BytesDLC = MCP251XFD_DLCToByte(messageToSend->DLC, CANFDframe);
  if (messageToSend->PayloadData != NULL)
  {
    uint8_t* pBuff = &Buffer[sizeof(MCP251XFD_CAN_TX_Message)];          // Next bytes of the Buffer is for payload
    uint8_t* pData = &messageToSend->PayloadData[0];                     // Select the first byte of payload data
    uint8_t BytesToCopy = BytesDLC;                                      // Get how many byte in the payload data will be copied
    while (BytesToCopy-- > 0) *pBuff++ = *pData++;                       // Copy data
    if ((BytesDLC & 0x3) > 0)                                            // Not modulo 4?
      for (uint8_t z = 0; z < (4 - (BytesDLC & 0x3)); z++) *pBuff++ = 0; // Fill with 0
  }

  //--- Send data ---
  uint8_t BytesToSend = (sizeof(MCP251XFD_CAN_TX_Message) + BytesDLC);
  if ((BytesToSend & 0x3) != 0) BytesToSend = (BytesToSend & 0xFC) + 4;  // Adjust to the upper modulo 4 bytes (mandatory for RAM access)
  return MCP251XFD_TransmitMessageObjectToFIFO(pComp, &Buffer[0], BytesToSend, toFIFO, andFlush);
}





//=============================================================================
// Receive a message object (with data) to the FIFO of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ReceiveMessageObjectFromFIFO(MCP251XFD *pComp, uint8_t* messageObjectGet, uint8_t objectSize, eMCP251XFD_FIFO fromFIFO)
{
#ifdef CHECK_NULL_PARAM
  if ((pComp == NULL) || (messageObjectGet == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (fromFIFO == MCP251XFD_TXQ) return ERR__PARAMETER_ERROR;
  if ((objectSize & 0x3) > 0) return ERR__BYTE_COUNT_MODULO_4;
  eERRORRESULT Error;

  //--- Get address where to write the frame ---
  uint32_t NextAddress = 0;
  Error = MCP251XFD_GetNextMessageAddressFIFO(pComp, fromFIFO, &NextAddress, NULL);  // Get next message address
  if (Error != ERR_OK) return Error;                                                 // If there is an error while calling MCP251XFD_GetNextMessageAddressFIFO() then return the error
  NextAddress += MCP251XFD_RAM_ADDR;                                                 // Add RAM offset address

  //--- Write data to RAM ---
  Error =  MCP251XFD_ReadData(pComp, NextAddress, &messageObjectGet[0], objectSize); // Read data to RAM address
  if (Error != ERR_OK) return Error;                                                 // If there is an error while calling MCP251XFD_ReadData() then return the error

  //--- Update FIFO ---
  return MCP251XFD_UpdateFIFO(pComp, fromFIFO, false);                               // Can't flush a receive FIFO
}



//=============================================================================
// Receive a message from a FIFO of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ReceiveMessageFromFIFO(MCP251XFD *pComp, MCP251XFD_CANMessage* messageGet, eMCP251XFD_PayloadSize payloadSize, uint32_t* timeStamp, eMCP251XFD_FIFO fromFIFO)
{
#ifdef CHECK_NULL_PARAM
  if ((pComp == NULL) || (messageGet == NULL)) return ERR__PARAMETER_ERROR;
#endif
  if (fromFIFO == MCP251XFD_TXQ) return ERR__PARAMETER_ERROR;
  uint8_t Buffer[MCP251XFD_CAN_RX_MESSAGE_SIZE_MAX];
  MCP251XFD_CAN_RX_Message* Message = (MCP251XFD_CAN_RX_Message*)Buffer;                   // The first 8 bytes represent the MCP251XFD_CAN_RX_Message struct
  eERRORRESULT Error;

  //--- Get data ---
  uint8_t BytesPayload = MCP251XFD_PayloadToByte(payloadSize);
  uint8_t BytesToGet = (sizeof(MCP251XFD_CAN_RX_Message) + BytesPayload);
  if (fromFIFO == MCP251XFD_TEF) BytesToGet = sizeof(MCP251XFD_CAN_TX_EventObject);        // In case of TEF, there is not data
  if (timeStamp == NULL) BytesToGet -= sizeof(uint32_t);                                   // Time Stamp not needed = 4 bytes less
  if ((BytesToGet & 0x3) != 0) BytesToGet = (BytesToGet & 0xFC) + 4;                       // Adjust to the upper modulo 4 bytes (mandatory for RAM access)
  Error = MCP251XFD_ReceiveMessageObjectFromFIFO(pComp, &Buffer[0], BytesToGet, fromFIFO); // Read bytes from RAM
  if (Error != ERR_OK) return Error;                                                       // If there is an error while calling MCP251XFD_ReceiveMessageObjectFromFIFO() then return the error

  //--- Extract message ID (R0) ---
  bool Extended   = (Message->R1.IDE == 1);
  bool CANFDframe = (Message->R1.FDF == 1);
  messageGet->MessageID = MCP251XFD_ObjectMessageIdentifierToMessageID(Message->R0.R0, Extended, MCP251XFD_USE_SID11 && CANFDframe); // Extract SID/EID from R0

  //--- Extract message controls (R1) ---
  messageGet->ControlFlags = MCP251XFD_NO_MESSAGE_CTRL_FLAGS;
  messageGet->MessageSEQ   = 0u;
  if (fromFIFO == MCP251XFD_TEF) messageGet->MessageSEQ = ((MCP251XFD_CAN_TX_EventObject*)Buffer)->TE1.SEQ; // If it is a TEF, extract the Sequence by casting Buffer into a TEF object and get SEQ in TE1
  if (CANFDframe          ) messageGet->ControlFlags = (setMCP251XFD_MessageCtrlFlags)(messageGet->ControlFlags + MCP251XFD_CANFD_FRAME                );
  if (Message->R1.BRS == 1) messageGet->ControlFlags = (setMCP251XFD_MessageCtrlFlags)(messageGet->ControlFlags + MCP251XFD_SWITCH_BITRATE             );
  if (Message->R1.RTR == 1) messageGet->ControlFlags = (setMCP251XFD_MessageCtrlFlags)(messageGet->ControlFlags + MCP251XFD_REMOTE_TRANSMISSION_REQUEST);
  if (Extended            ) messageGet->ControlFlags = (setMCP251XFD_MessageCtrlFlags)(messageGet->ControlFlags + MCP251XFD_EXTENDED_MESSAGE_ID        );
  if (Message->R1.ESI == 1) messageGet->ControlFlags = (setMCP251XFD_MessageCtrlFlags)(messageGet->ControlFlags + MCP251XFD_TRANSMIT_ERROR_PASSIVE     );
  messageGet->DLC = (eMCP251XFD_DataLength)Message->R1.DLC;

  //--- Extract TimeStamp ---
  uint8_t* pBuff = &Buffer[sizeof(MCP251XFD_CAN_RX_Message_Identifier) + sizeof(MCP251XFD_CAN_RX_Message_Control)]; // Next bytes of the Buffer is for timestamp and/or payload
  if (timeStamp != NULL)
  {
    *timeStamp = Message->TimeStamp;
    pBuff += sizeof(uint32_t);                                             // If TimeStamp extracted, update the pBuff
  }

  //--- Extract payload data ---
  if (fromFIFO != MCP251XFD_TEF)
  {
    if ((messageGet->DLC != MCP251XFD_DLC_0BYTE) && (messageGet->PayloadData == NULL)) return ERR__NO_DATA_AVAILABLE;
    if (messageGet->PayloadData != NULL)
    {
      uint8_t* pData = &messageGet->PayloadData[0];                        // Select the first byte of payload data
      uint8_t BytesDLC = MCP251XFD_DLCToByte(messageGet->DLC, CANFDframe); // Get how many byte need to be extract from the message to correspond to its DLC
      if (BytesPayload < BytesDLC) BytesDLC = BytesPayload;                // Get the least between BytesPayload and BytesDLC
      while (BytesDLC-- > 0) *pData++ = *pBuff++;                          // Copy data
    }
  }

  return ERR_OK;
}





//**********************************************************************************************************************************************************
//=============================================================================
// CRC Configuration of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureCRC(MCP251XFD *pComp, setMCP251XFD_CRCEvents interrupts)
{
  return MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CRC_CONFIG, interrupts); // Write configuration to the CRC register (last byte only)
}





//=============================================================================
// Get CRC Status of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetCRCEvents(MCP251XFD *pComp, setMCP251XFD_CRCEvents* events, uint16_t* lastCRCMismatch)
{
#ifdef CHECK_NULL_PARAM
  if (events == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;

  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CRC_FLAGS, (uint8_t*)events); // Read status of the CRC register (third byte only)
  if (Error != ERR_OK) return Error;                                           // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  *events = (setMCP251XFD_CRCEvents)(*events & MCP251XFD_CRC_EVENTS_MASK);     // Get CRC error interrupt flag status and CRC command format error interrupt flag status
  if (lastCRCMismatch != NULL)
  {
    Error = MCP251XFD_ReadSFR16(pComp, RegMCP251XFD_CRC_CRC, lastCRCMismatch); // Read Address where last CRC error occurred (first 2 bytes only)
  }
  return Error;
}





//**********************************************************************************************************************************************************
//=============================================================================
// ECC Configuration of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureECC(MCP251XFD *pComp, bool enableECC, setMCP251XFD_ECCEvents interrupts, uint8_t fixedParityValue)
{
  uint8_t Config[2] = {MCP251XFD_SFR_ECCCON8_ECCDIS | MCP251XFD_SFR_ECCCON8_SECID | MCP251XFD_SFR_ECCCON8_DEDID, 0}; // By default, disable all controls
  Config[0] |= (uint8_t)interrupts;                                      // Single error correction and double error detection interrupts enable
  if (enableECC) Config[0] |= MCP251XFD_SFR_ECCCON8_ECCEN;               // If ECC enable then add the ECC enable flag
  Config[1] = MCP251XFD_SFR_ECCCON8_PARITY_SET(fixedParityValue);        // Add the fixed parity used during write to RAM when ECC is disabled
  return MCP251XFD_WriteData(pComp, RegMCP251XFD_ECCCON, &Config[0], 2); // Write configuration to the ECC register (first 2 bytes only)
}





//=============================================================================
// Get ECC Status of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetECCEvents(MCP251XFD *pComp, setMCP251XFD_ECCEvents* events, uint16_t* lastErrorAddress)
{
#ifdef CHECK_NULL_PARAM
  if (events == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;

  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_ECCSTAT_FLAGS, (uint8_t*)events);      // Read status of the ECC register (first bytes only)
  if (Error != ERR_OK) return Error;                                                    // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  if (lastErrorAddress != NULL)
  {
    Error = MCP251XFD_ReadSFR16(pComp, RegMCP251XFD_ECCSTAT_ERRADDR, lastErrorAddress); // Read Address where last ECC error occurred (last 2 bytes only)
  }
  return Error;
}





//**********************************************************************************************************************************************************
//=============================================================================
// Configure pins of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigurePins(MCP251XFD *pComp, eMCP251XFD_GPIO0Mode GPIO0PinMode, eMCP251XFD_GPIO1Mode GPIO1PinMode, eMCP251XFD_OutMode INTOutMode, eMCP251XFD_OutMode TXCANOutMode, bool CLKOasSOF)
{
  eERRORRESULT Error;

  uint8_t Config = MCP251XFD_SFR_IOCON8_GPIO0_INT0 | MCP251XFD_SFR_IOCON8_GPIO1_INT1;                                                    // By default, disable all controls
  if ((GPIO0PinMode != MCP251XFD_PIN_AS_INT0_TX) && (GPIO0PinMode != MCP251XFD_PIN_AS_XSTBY)) Config |= MCP251XFD_SFR_IOCON8_GPIO0_MODE; // If the pin INT0/GPIO0/XSTBY is in GPIO mode then set GPIO mode
  if ((GPIO1PinMode != MCP251XFD_PIN_AS_INT1_RX                                            )) Config |= MCP251XFD_SFR_IOCON8_GPIO1_MODE; // If the pin INT1/GPIO1 is in GPIO mode then set GPIO mode
  if ( TXCANOutMode == MCP251XFD_PINS_OPENDRAIN_OUT                                         ) Config |= MCP251XFD_SFR_IOCON8_TXCANOD;    // If the pin TXCAN mode is open drain mode then set open drain output
  if ( CLKOasSOF                                                                            ) Config |= MCP251XFD_SFR_IOCON8_SOF;        // If the pin CLKO/SOF is in SOF mode then set SOF signal on CLKO pin
  if ( INTOutMode   == MCP251XFD_PINS_OPENDRAIN_OUT                                         ) Config |= MCP251XFD_SFR_IOCON8_INTOD;      // If all interrupt pins mode are open drain mode then set open drain output
  Error = MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_IOCON_PINMODE, Config);                                                                // Write configuration to the IOCON register (last byte only)
  if (Error != ERR_OK) return Error;                                                                                                     // If there is an error while calling MCP251XFD_WriteSFR8() then return the error

  Config = MCP251XFD_SFR_IOCON8_XSTBYDIS | MCP251XFD_SFR_IOCON8_GPIO0_OUTPUT | MCP251XFD_SFR_IOCON8_GPIO1_OUTPUT; // By default, disable all controls
  if (GPIO0PinMode == MCP251XFD_PIN_AS_XSTBY   ) Config |= MCP251XFD_SFR_IOCON8_XSTBYEN;                          // If the pin INT0/GPIO0/XSTBY is in XSTBY mode then enable XSTBY mode
  if (GPIO0PinMode == MCP251XFD_PIN_AS_GPIO0_IN) Config |= MCP251XFD_SFR_IOCON8_GPIO0_INPUT;                      // If the pin INT0/GPIO0/XSTBY is in GPIO input mode then set GPIO input mode
  if (GPIO1PinMode == MCP251XFD_PIN_AS_GPIO1_IN) Config |= MCP251XFD_SFR_IOCON8_GPIO1_INPUT;                      // If the pin INT1/GPIO1 is in GPIO input mode then set GPIO input mode
  return MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_IOCON_DIRECTION, Config);                                        // Write configuration to the IOCON register (first byte only)
}



//=============================================================================
// Set GPIO pins direction of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_SetGPIOPinsDirection(MCP251XFD *pComp, uint8_t pinsDirection, uint8_t pinsChangeMask)
{
  eERRORRESULT Error;
  uint8_t Config;

  pinsChangeMask &= 0x3;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_IOCON_DIRECTION, &Config); // Read actual configuration of the IOCON register (first byte only)
  if (Error != ERR_OK) return Error;                                        // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  Config &= ~pinsChangeMask;                                                // Force change bits to 0
  Config |= (pinsDirection & pinsChangeMask);                               // Apply new direction only on changed pins
  return MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_IOCON_DIRECTION, Config);  // Write new configuration to the IOCON register (first byte only)
}





//=============================================================================
// Get GPIO pins input level of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetGPIOPinsInputLevel(MCP251XFD *pComp, uint8_t *pinsState)
{
  return MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_IOCON_INLEVEL, pinsState); // Read actual state of the input pins in the IOCON register (third byte only)
}





//=============================================================================
// Set GPIO pins output level of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_SetGPIOPinsOutputLevel(MCP251XFD *pComp, uint8_t pinsLevel, uint8_t pinsChangeMask)
{
#ifdef CHECK_NULL_PARAM
  if (pComp == NULL) return ERR__PARAMETER_ERROR;
#endif

  pinsChangeMask &= 0x3;
  pComp->GPIOsOutState &= ~pinsChangeMask;                                              // Force change bits to 0
  pComp->GPIOsOutState |= (pinsLevel & pinsChangeMask);                                 // Apply new output level only on changed pins
  return MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_IOCON_OUTLEVEL, pComp->GPIOsOutState); // Write new configuration to the IOCON register (Second byte only)
}





//**********************************************************************************************************************************************************
//=============================================================================
// Calculate Bit Time for CAN2.0 or CAN-FD Configuration for the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_CalculateBitTimeConfiguration(const uint32_t fsysclk, const uint32_t desiredNominalBitrate, const uint32_t desiredDataBitrate, MCP251XFD_BitTimeConfig *pConf)
{
#ifdef CHECK_NULL_PARAM
  if (pConf == NULL) return ERR__PARAMETER_ERROR;
#endif
  //--- Check values ----------------------------------------
  if (fsysclk               < MCP251XFD_SYSCLK_MIN     ) return ERR__PARAMETER_ERROR;
  if (fsysclk               > MCP251XFD_SYSCLK_MAX     ) return ERR__PARAMETER_ERROR;
  if (desiredNominalBitrate < MCP251XFD_NOMBITRATE_MIN ) return ERR__BAUDRATE_ERROR;
  if (desiredNominalBitrate > MCP251XFD_NOMBITRATE_MAX ) return ERR__BAUDRATE_ERROR;
  if (desiredDataBitrate != MCP251XFD_NO_CANFD)
    if (desiredDataBitrate  < MCP251XFD_DATABITRATE_MIN) return ERR__BAUDRATE_ERROR;
  if (desiredDataBitrate    > MCP251XFD_DATABITRATE_MAX) return ERR__BAUDRATE_ERROR;

  //--- Declaration -----------------------------------------
  uint32_t ErrorTQ, ErrorNTQ, ErrorDTQ, DTQbits = 0;
  uint32_t BestBRP = MCP251XFD_NBRP_MAX, BestNTQbits = MCP251XFD_NTQBIT_MAX, BestDTQbits = MCP251XFD_DTQBIT_MAX;

  //--- Calculate Nominal & Data Bit Time parameter ---------
  uint32_t MinErrorBR = UINT32_MAX;
  uint32_t BRP = MCP251XFD_NBRP_MAX;                                                      // Select the worst BRP value. Here all value from max to min will be tested to get the best tuple of NBRP and DBRP, identical TQ in both phases prevents quantization errors during bit rate switching
  while (--BRP >= MCP251XFD_NBRP_MIN)
  {
    uint32_t NTQbits = fsysclk / desiredNominalBitrate / BRP;                             // Calculate the NTQbits according to BRP and the desired Nominal Bitrate
    if ((NTQbits < MCP251XFD_NTQBIT_MIN) || (NTQbits > MCP251XFD_NTQBIT_MAX)) continue;   // This TQbits count is not possible with this BRP, then do the next BRP value
    if (desiredDataBitrate != MCP251XFD_NO_CANFD)
    {
      DTQbits = fsysclk / desiredDataBitrate / BRP;                                       // Calculate the DTQbits according to BRP and the desired Data Bitrate
      if ((DTQbits < MCP251XFD_DTQBIT_MIN) || (DTQbits > MCP251XFD_DTQBIT_MAX)) continue; // This TQbits count is not possible with this BRP, then do the next BRP value
    }

    // NTQ & DTQ bits count
    ErrorNTQ = (fsysclk - (desiredNominalBitrate * NTQbits * BRP));                                   // Calculate NTQ error
    if (desiredDataBitrate != MCP251XFD_NO_CANFD)
    {
      if (ErrorNTQ == 0) ErrorNTQ = 1;                                                                // Adjust NTQ error
      ErrorDTQ = (fsysclk - (desiredDataBitrate * DTQbits * BRP)); if (ErrorDTQ == 0) ErrorDTQ = 1;   // Calculate DTQ error
      ErrorTQ = (ErrorNTQ * ErrorDTQ);
    }
    else ErrorTQ = ErrorNTQ;
    if (ErrorTQ <= MinErrorBR)                                                                        // If better error then
    { MinErrorBR = ErrorTQ; BestBRP = BRP; BestNTQbits = NTQbits; BestDTQbits = DTQbits; }            // Save best parameters

    // NTQ+1 & DTQ bits count
    if (NTQbits < MCP251XFD_NTQBIT_MAX)
    {
      ErrorNTQ = ((desiredNominalBitrate * (NTQbits+1) * BRP) - fsysclk);                             // Calculate NTQ error with NTQbits+1
      if (desiredDataBitrate != MCP251XFD_NO_CANFD)
      {
        if (ErrorNTQ == 0) ErrorNTQ = 1;                                                              // Adjust NTQ error
        ErrorDTQ = (fsysclk - (desiredDataBitrate * DTQbits * BRP)); if (ErrorDTQ == 0) ErrorDTQ = 1; // Calculate DTQ error
        ErrorTQ = (ErrorNTQ * ErrorDTQ);
      }
      else ErrorTQ = ErrorNTQ;
      if (ErrorTQ <= MinErrorBR)                                                                      // If better error then
      { MinErrorBR = ErrorTQ; BestBRP = BRP; BestNTQbits = NTQbits+1; BestDTQbits = DTQbits; }        // Save best parameters
    }

    // NTQ+1 & DTQ or DTQ+1 bits count
    if (desiredDataBitrate != MCP251XFD_NO_CANFD)
    {
      if (DTQbits < MCP251XFD_DTQBIT_MAX)
      {
        ErrorNTQ = (fsysclk - (desiredNominalBitrate * NTQbits * BRP));  if (ErrorNTQ == 0) ErrorNTQ = 1;    // Calculate NTQ error
        ErrorDTQ = ((desiredDataBitrate * (DTQbits+1) * BRP) - fsysclk); if (ErrorDTQ == 0) ErrorDTQ = 1;    // Calculate DTQ error with DTQbits+1
        ErrorTQ = (ErrorNTQ * ErrorDTQ);
        if (ErrorTQ <= MinErrorBR)                                                                           // If better error then
        { MinErrorBR = ErrorTQ; BestBRP = BRP; BestNTQbits = NTQbits; BestDTQbits = DTQbits+1; }             // Save best parameters
      }
      if ((NTQbits < MCP251XFD_NTQBIT_MAX) && (DTQbits < MCP251XFD_DTQBIT_MAX))
      {
        ErrorNTQ = ((desiredNominalBitrate * (NTQbits+1) * BRP) - fsysclk); if (ErrorNTQ == 0) ErrorNTQ = 1; // Calculate NTQ error with NTQbits+1
        ErrorDTQ = ((desiredDataBitrate * (DTQbits+1) * BRP) - fsysclk);    if (ErrorDTQ == 0) ErrorDTQ = 1; // Calculate DTQ error with DTQbits+1
        ErrorTQ = (ErrorNTQ * ErrorDTQ);
        if (ErrorTQ <= MinErrorBR)                                                                           // If better error then
        { MinErrorBR = ErrorTQ; BestBRP = BRP; BestNTQbits = NTQbits+1; BestDTQbits = DTQbits+1; }           // Save best parameters
      }
    }
  }
  if (MinErrorBR == UINT32_MAX) return ERR__BITTIME_ERROR;          // Impossible to find a good BRP

  //--- Calculate Nominal segments --------------------------
  pConf->NBRP = BestBRP - 1;                                        // ** Save the best NBRP in the configuration **
  uint32_t NTSEG2 = BestNTQbits / 5;                                // The Nominal Sample Point must be close to 80% (5x20%) of NTQ per bits so NTSEG2 should be 20% of NTQbits
  if ((BestNTQbits % 5) > 2) NTSEG2++;                              // To be as close as possible to 80%
  if (NTSEG2 < MCP251XFD_NTSEG2_MIN) NTSEG2 = MCP251XFD_NTSEG2_MIN; // Correct NTSEG2 if < 1
  if (NTSEG2 > MCP251XFD_NTSEG2_MAX) NTSEG2 = MCP251XFD_NTSEG2_MAX; // Correct NTSEG2 if > 128
  pConf->NTSEG2 = NTSEG2 - 1;                                       // ** Save the NTSEG2 in the configuration **
  uint32_t NTSEG1 = BestNTQbits - NTSEG2 - MCP251XFD_NSYNC;         // NTSEG1  = NTQbits - NTSEG2 - 1 (NSYNC)
  if (NTSEG1 < MCP251XFD_NTSEG1_MIN) NTSEG1 = MCP251XFD_NTSEG1_MIN; // Correct NTSEG1 if < 1
  if (NTSEG1 > MCP251XFD_NTSEG1_MAX) NTSEG1 = MCP251XFD_NTSEG1_MAX; // Correct NTSEG1 if > 256
  pConf->NTSEG1 = NTSEG1 - 1;                                       // ** Save the NTSEG1 in the configuration **
  uint32_t NSJW = NTSEG2;                                           // Normally NSJW = NTSEG2, maximizing NSJW lessens the requirement for the oscillator tolerance
  if (NTSEG1 < NTSEG2) NSJW = NTSEG1;                               // But NSJW = min(NPHSEG1, NPHSEG2)
  if (NSJW < MCP251XFD_NSJW_MIN) NSJW = MCP251XFD_NSJW_MIN;         // Correct NSJW if < 1
  if (NSJW > MCP251XFD_NSJW_MAX) NSJW = MCP251XFD_NSJW_MAX;         // Correct NSJW if > 128
  pConf->NSJW = NSJW - 1;                                           // ** Save the NSJW in the configuration **

  //--- Calculate Data segments -----------------------------
  if (desiredDataBitrate != MCP251XFD_NO_CANFD)
  {
    pConf->DBRP = BestBRP - 1;                                        // ** Save the best DBRP in the configuration **
    uint32_t DTSEG2 = BestDTQbits / 5;                                // The Data Sample Point must be close to 80% (5x20%) of DTQ per bits so DTSEG2 should be 20% of DTQbits
    if ((BestDTQbits % 5) > 2) DTSEG2++;                              // To be as close as possible to 80%
    if (DTSEG2 < MCP251XFD_NTSEG2_MIN) DTSEG2 = MCP251XFD_NTSEG2_MIN; // Correct DTSEG2 if < 1
    if (DTSEG2 > MCP251XFD_NTSEG2_MAX) DTSEG2 = MCP251XFD_NTSEG2_MAX; // Correct DTSEG2 if > 16
    pConf->DTSEG2 = DTSEG2 - 1;                                       // ** Save the DTSEG2 in the configuration **
    uint32_t DTSEG1 = BestDTQbits - DTSEG2 - MCP251XFD_DSYNC;         // DTSEG1  = DTQbits - DTSEG2 - 1 (DSYNC)
    if (DTSEG1 < MCP251XFD_NTSEG1_MIN) DTSEG1 = MCP251XFD_NTSEG1_MIN; // Correct DTSEG1 if < 1
    if (DTSEG1 > MCP251XFD_NTSEG1_MAX) DTSEG1 = MCP251XFD_NTSEG1_MAX; // Correct DTSEG1 if > 32
    pConf->DTSEG1 = DTSEG1 - 1;                                       // ** Save the DTSEG1 in the configuration **
    uint32_t DSJW = DTSEG2;                                           // Normally DSJW = DTSEG2, maximizing DSJW lessens the requirement for the oscillator tolerance
    if (DTSEG1 < DTSEG2) DSJW = DTSEG1;                               // But DSJW = min(DPHSEG1, DPHSEG2)
    if (DSJW < MCP251XFD_DSJW_MIN) DSJW = MCP251XFD_DSJW_MIN;         // Correct DSJW if < 1
    if (DSJW > MCP251XFD_DSJW_MAX) DSJW = MCP251XFD_DSJW_MAX;         // Correct DSJW if > 128
    pConf->DSJW = DSJW - 1;                                           // ** Save the DSJW in the configuration **

    //--- Calculate Transmitter Delay Compensation ----------
    if (desiredDataBitrate >= 1000000)                                // Enable Automatic TDC for DBR of 1Mbps and Higher
         pConf->TDCMOD = MCP251XFD_AUTO_MODE;                         // ** Set Automatic TDC measurement compensations for transmitter delay variations
    else pConf->TDCMOD = MCP251XFD_MANUAL_MODE;                       // ** Set Manual; Don�t measure, use TDCV + TDCO from register
    const uint32_t SSP = BestBRP * DTSEG1;                            // In order to set the SSP to 80%, SSP = TDCO + TDCV (Equation 3-10 of MCP25XXFD Family Reference Manual). SSP is set to DBRP * (DPRSEG + DPHSEG1) = DBRP * DTSEG1
    uint32_t TDCO = SSP;
    if (TDCO > MCP251XFD_TDCO_MAX) TDCO = MCP251XFD_TDCO_MAX;         // Correct TDCO if > 63
    pConf->TDCO = TDCO;                                               // ** Save the TDCO in the configuration **
    uint32_t TDCV = SSP - TDCO;                                       // TDCV is the remaining of SSP: TDCV = SSP - TDCO (Equation 3-10 of MCP25XXFD Family Reference Manual)
    if (TDCV > MCP251XFD_TDCV_MAX) TDCV = MCP251XFD_TDCV_MAX;         // Correct TDCV if > 63
    pConf->TDCV = TDCV;                                               // ** Save the TDCV in the configuration **
    pConf->EDGE_FILTER = true;                                        // ** Edge Filtering enabled, according to ISO 11898-1:2015 **
  }
  else
  {
    pConf->DBRP   = 0x0;                                              // ** Set the DBRP in the configuration **
    pConf->DTSEG2 = 0x3;                                              // ** Set the DTSEG2 in the configuration **
    pConf->DTSEG1 = 0x0E;                                             // ** Set the DTSEG1 in the configuration **
    pConf->DSJW   = 0x3;                                              // ** Set the DSJW in the configuration **
    pConf->TDCMOD = MCP251XFD_AUTO_MODE;                              // ** Set Automatic TDC measurement compensations for transmitter delay variations
    pConf->TDCO   = 0x10;                                             // ** Set the TDCO in the configuration **
    pConf->TDCV   = 0x00;                                             // ** Set the TDCV in the configuration **
  }

  eERRORRESULT Error = ERR_OK;
  if (pConf->Stats != NULL)
    Error = MCP251XFD_CalculateBitrateStatistics(fsysclk, pConf, desiredDataBitrate == MCP251XFD_NO_CANFD); // If statistics are necessary, then calculate them
  return Error;                                                       // If there is an error while calling MCP251XFD_CalculateBitrateStatistics() then return the error
}



//=============================================================================
// Calculate Bitrate Statistics of a Bit Time configuration
//=============================================================================
eERRORRESULT MCP251XFD_CalculateBitrateStatistics(const uint32_t fsysclk, MCP251XFD_BitTimeConfig *pConf, bool can20only)
{
#ifdef CHECK_NULL_PARAM
  if (pConf == NULL) return ERR__PARAMETER_ERROR;
  if (pConf->Stats == NULL) return ERR__PARAMETER_ERROR;
#endif
  //--- Check values ----------------------------------------
  if (fsysclk < MCP251XFD_SYSCLK_MIN) return ERR__PARAMETER_ERROR;
  if (fsysclk > MCP251XFD_SYSCLK_MAX) return ERR__PARAMETER_ERROR;

  //--- Declaration -----------------------------------------
  uint32_t DTQbits = 0;

  //--- Calculate bus length & Nominal Sample Point ---------
  const uint32_t NTQ = (((pConf->NBRP+1) * 1000000) / (fsysclk / 1000));          // Nominal Time Quanta = 1/FSYSCLK multiply by 1000000000 to get ns (Equation 3-3 of MCP25XXFD Family Reference Manual)
  const uint32_t NPRSEG  = (pConf->NTSEG1+1) - (pConf->NTSEG2+1);                 // Here PHSEG2 (NTSEG2) should be equal to PHSEG1 so NPRSEG = NTSEG1 - NTSEG2 (Figure 3-2 of MCP25XXFD Family Reference Manual)
  pConf->Stats->MaxBusLength = (uint32_t)(((NTQ * NPRSEG) - (2 * MCP251XFD_tTXDtRXD_MAX)) / (2 * MCP251XFD_tBUS_CONV)); // Formula is (2x(tTXD�RXD + (5*BusLen))/NTQ = NPRSEG => BusLen = ((NTQ*NPRESG)-(2*tTXD))/(2*5) in meter (Equation 3-9 of MCP25XXFD Family Reference Manual)
  const uint32_t NTQbits = (MCP251XFD_NSYNC + (pConf->NTSEG1+1) + (pConf->NTSEG2+1)); // NTQ per bits = NSYNC + NTSEG1 + NTSEG2 (Equation 3-5 of MCP25XXFD Family Reference Manual)
  uint32_t SamplePoint = ((MCP251XFD_NSYNC + (pConf->NTSEG1+1)) * 100) / NTQbits; // Calculate actual nominal sample point
  pConf->Stats->NSamplePoint = (uint32_t)(SamplePoint * 100);                     // ** Save actual Nominal sample point with 2 digits after the decimal point (divide by 100 to get percentage)
  pConf->Stats->NominalBitrate = (fsysclk / (pConf->NBRP+1) / NTQbits);           // ** Save actual Nominal Bitrate

  //--- Calculate Data Sample Point -------------------------
  if (can20only == false)
  {
    DTQbits = (MCP251XFD_DSYNC + (pConf->DTSEG1+1) + (pConf->DTSEG2+1));          // DTQ per bits = DSYNC + DTSEG1 + DTSEG2 (Equation 3-6 of MCP25XXFD Family Reference Manual)
    SamplePoint = ((MCP251XFD_DSYNC + (pConf->DTSEG1+1)) * 100) / DTQbits;        // Calculate actual data sample point
    pConf->Stats->DSamplePoint = (uint32_t)(SamplePoint * 100.0f);                // ** Save actual Data sample point with 2 digits after the decimal point (divide by 100 to get percentage)
    pConf->Stats->DataBitrate  = (fsysclk / (pConf->DBRP+1) / DTQbits);           // ** Save actual Data Bitrate
  }
  else
  {
    pConf->Stats->DSamplePoint = 0;                                               // ** Set actual Data sample point
    pConf->Stats->DataBitrate  = 0;                                               // ** Set actual Data Bitrate
  }

  //--- Calculate oscillator tolerance ----------------------
  const uint32_t NPHSEG1    = (pConf->NTSEG1+1) - NPRSEG;                                                                                   // Get NPHSEG1
  const uint32_t MinNPHSEG  = (NPHSEG1 <= (pConf->NTSEG2+1) ? NPHSEG1 : (pConf->NTSEG2+1));                                                 // Get min(NPHSEG1, NPHSEG2)
  pConf->Stats->OscTolC1     = (((pConf->NSJW+1) * 10000) / (2 * 10 * NTQbits));                                                            // Condition 1 for the maximum tolerance of the oscillator with 2 digits after the decimal point (Equation 3-12 of MCP25XXFD Family Reference Manual)
  pConf->Stats->OscTolerance = pConf->Stats->OscTolC1;
  pConf->Stats->OscTolC2     = ((MinNPHSEG * 10000) / (2 * (13 * NTQbits - (pConf->NTSEG2+1))));                                            // Condition 2 for the maximum tolerance of the oscillator with 2 digits after the decimal point (Equation 3-13 of MCP25XXFD Family Reference Manual)
  pConf->Stats->OscTolerance = (pConf->Stats->OscTolC2 < pConf->Stats->OscTolerance ? pConf->Stats->OscTolC2 : pConf->Stats->OscTolerance); // Oscillator Tolerance, minimum of conditions 1-5 (Equation 3-11 of MCP25XXFD Family Reference Manual)
  if (can20only)
  {
    pConf->Stats->OscTolC3   = 0;
    pConf->Stats->OscTolC4   = 0;
    pConf->Stats->OscTolC5   = 0;
  }
  else
  {
    pConf->Stats->OscTolC3   = (((pConf->DSJW+1) * 10000) / (2 * 10 * DTQbits));                                                                                     // Condition 3 for the maximum tolerance of the oscillator with 2 digits after the decimal point (Equation 3-14 of MCP25XXFD Family Reference Manual)
    pConf->Stats->OscTolerance = (pConf->Stats->OscTolC3 < pConf->Stats->OscTolerance ? pConf->Stats->OscTolC3 : pConf->Stats->OscTolerance);                        // Oscillator Tolerance, minimum of conditions 1-5 (Equation 3-11 of MCP25XXFD Family Reference Manual)
    const uint32_t NBRP = (pConf->NBRP + 1), DBRP = (pConf->DBRP + 1);
    pConf->Stats->OscTolC4   = ((MinNPHSEG * 10000) / (2 * ((((6 * DTQbits - (pConf->DTSEG2+1)) * DBRP) / NBRP) + (7 * NTQbits))));                                  // Condition 4 for the maximum tolerance of the oscillator with 2 digits after the decimal point (Equation 3-15 of MCP25XXFD Family Reference Manual)
    pConf->Stats->OscTolerance = (pConf->Stats->OscTolC4 < pConf->Stats->OscTolerance ? pConf->Stats->OscTolC4 : pConf->Stats->OscTolerance);                        // Oscillator Tolerance, minimum of conditions 1-5 (Equation 3-11 of MCP25XXFD Family Reference Manual)
    const int32_t NBRP_DBRP = ((NBRP * 10000) / DBRP), MaxBRP = ((NBRP_DBRP - 10000) > 0 ? (NBRP_DBRP - 10000) : 0);                                                 // NBRP/DBRP and max(0,(NBRP/DBRP-1)). The use of 10000 is to set 2 digits on the C5 result
    pConf->Stats->OscTolC5   = ((((pConf->DSJW+1) * 10000) - MaxBRP) / (2 * (((2 * NTQbits - (pConf->NTSEG2+1)) * NBRP) / DBRP + (pConf->DTSEG2+1) + 4 * DTQbits))); // Condition 5 for the maximum tolerance of the oscillator with 2 digits after the decimal point (Equation 3-16 of MCP25XXFD Family Reference Manual) [WARNING: An error seems to be present in the original formula]
    pConf->Stats->OscTolerance = (pConf->Stats->OscTolC5 < pConf->Stats->OscTolerance ? pConf->Stats->OscTolC5 : pConf->Stats->OscTolerance);                        // Oscillator Tolerance, minimum of conditions 1-5 (Equation 3-11 of MCP25XXFD Family Reference Manual)
  }
  return ERR_OK;
}



//=============================================================================
// Set Bit Time Configuration to the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_SetBitTimeConfiguration(MCP251XFD *pComp, MCP251XFD_BitTimeConfig *pConf, bool can20only)
{
#ifdef CHECK_NULL_PARAM
  if (pConf == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;

  //--- Write Nominal Bit Time configuration ----------------
  MCP251XFD_CiNBTCFG_Register NConfig;
  NConfig.CiNBTCFG = MCP251XFD_CAN_CiNBTCFG_BRP_SET(pConf->NBRP) | MCP251XFD_CAN_CiNBTCFG_TSEG1_SET(pConf->NTSEG1)     // Set Nominal Bit Time configuration
                   | MCP251XFD_CAN_CiNBTCFG_TSEG2_SET(pConf->NTSEG2) | MCP251XFD_CAN_CiNBTCFG_SJW_SET(pConf->NSJW);
  Error = MCP251XFD_WriteData(pComp, RegMCP251XFD_CiNBTCFG, &NConfig.Bytes[0], sizeof(MCP251XFD_CiNBTCFG_Register));   // Write configuration to the CiNBTCFG register
  if (Error != ERR_OK) return Error;                                                                                   // If there is an error while calling MCP251XFD_WriteData() then return the error

  if (!can20only)
  {
    //--- Write Data Bit Time configuration -----------------
    MCP251XFD_CiDBTCFG_Register DConfig;
    DConfig.CiDBTCFG = MCP251XFD_CAN_CiDBTCFG_BRP_SET(pConf->DBRP) | MCP251XFD_CAN_CiDBTCFG_TSEG1_SET(pConf->DTSEG1)   // Set Data Bit Time configuration
                     | MCP251XFD_CAN_CiDBTCFG_TSEG2_SET(pConf->DTSEG2) | MCP251XFD_CAN_CiDBTCFG_SJW_SET(pConf->DSJW);
    Error = MCP251XFD_WriteData(pComp, RegMCP251XFD_CiDBTCFG, &DConfig.Bytes[0], sizeof(MCP251XFD_CiDBTCFG_Register)); // Write configuration to the CiDBTCFG register
    if (Error != ERR_OK) return Error;                                                                                 // If there is an error while calling MCP251XFD_WriteData() then return the error

    //--- Write Data Bit Time configuration -----------------
    MCP251XFD_CiTDC_Register TConfig;
    TConfig.CiTDC = MCP251XFD_CAN_CiTDC_TDCO_SET(pConf->TDCO) | MCP251XFD_CAN_CiTDC_TDCV_SET(pConf->TDCV)              // Set Data Bit Time configuration
                  | MCP251XFD_CAN_CiTDC_TDCMOD_SET(pConf->TDCMOD) | MCP251XFD_CAN_CiTDC_EDGFLTDIS;
    if (pConf->EDGE_FILTER) TConfig.CiTDC |= MCP251XFD_CAN_CiTDC_EDGFLTEN;                                             // Enable Edge Filter if asked
    Error = MCP251XFD_WriteData(pComp, RegMCP251XFD_CiTDC, &TConfig.Bytes[0], sizeof(MCP251XFD_CiTDC_Register));       // Write configuration to the CiDBTCFG register
    if (Error != ERR_OK) return Error;                                                                                 // If there is an error while calling MCP251XFD_WriteData()
    pComp->InternalConfig |= MCP251XFD_CANFD_ENABLED;                                                                  // CAN-FD is enable if Data Bitrate is set
  }
  else pComp->InternalConfig &= ~MCP251XFD_CANFD_ENABLED;                                                              // Set no CAN-FD

  return ERR_OK;
}





//**********************************************************************************************************************************************************
//=============================================================================
// Abort all pending transmissions of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_AbortAllTransmissions(MCP251XFD *pComp)
{
  eERRORRESULT Error;
  uint8_t Config;

  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiCON+3, &Config); // Read actual configuration of the CiCON register (Last byte only)
  if (Error != ERR_OK) return Error;                                // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  Config |= MCP251XFD_CAN_CiCON8_ABAT;                              // Add ABAT flag
  return MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CiCON+3, Config);  // Write the new configuration to the CiCON register (Last byte only)
}



//=============================================================================
// Get actual operation mode of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetActualOperationMode(MCP251XFD *pComp, eMCP251XFD_OperationMode* actualMode)
{
  eERRORRESULT Error;
  uint8_t Config;

  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiCON+2, &Config); // Read actual configuration of the CiCON register (Third byte only)
  if (Error != ERR_OK) return Error;                                // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  *actualMode = MCP251XFD_CAN_CiCON8_OPMOD_GET(Config);             // Get actual mode
  return ERR_OK;
}



//=============================================================================
// Request operation mode change of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_RequestOperationMode(MCP251XFD *pComp, eMCP251XFD_OperationMode newMode, bool waitOperationChange)
{
#ifdef CHECK_NULL_PARAM
  if (pComp == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;
  uint8_t Config;

  if (((pComp->InternalConfig & MCP251XFD_CANFD_ENABLED) == 0)
   && (newMode == MCP251XFD_NORMAL_CANFD_MODE)) return ERR__CONFIGURATION;  // Can't change to CAN-FD mode if the bitrate is not configured for
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiCON+3, &Config);         // Read actual configuration of the CiCON register (Last byte only)
  if (Error != ERR_OK) return Error;                                        // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  Config &= ~MCP251XFD_CAN_CiCON8_REQOP_Mask;                               // Clear request mode bits
  Config |= MCP251XFD_CAN_CiCON8_REQOP_SET(newMode);                        // Set request operation mode bits
  Config |= MCP251XFD_CAN_CiCON8_ABAT;                                      // Need to stop all transmissions before changing configuration
  Error = MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CiCON+3, Config);         // Write the new configuration to the CiCON register (Last byte only)
  if (Error != ERR_OK) return Error;                                        // If there is an error while calling MCP251XFD_WriteSFR8() then return the error

  if (waitOperationChange)
  {
    Error = MCP251XFD_WaitOperationModeChange(pComp, newMode);              // Wait for operation mode change
    if (Error != ERR_OK) return Error;                                      // If there is an error while calling MCP251XFD_WaitOperationModeChange() then return the error
    Error = MCP251XFD_ClearInterruptEvents(pComp, MCP251XFD_INT_OPERATION_MODE_CHANGE_EVENT); // Automatically clear the Operation Mode Change Flag
    if (Error != ERR_OK) return Error;                                      // If there is an error while calling MCP251XFD_ClearInterruptEvents() then return the error
  }
  pComp->InternalConfig &= ~MCP251XFD_DEV_PS_Mask;
  pComp->InternalConfig |= MCP251XFD_DEV_PS_SET(MCP251XFD_DEVICE_NORMAL_POWER_STATE); // Set normal power state even if the operation mode is sleep, this value will be changed to the good value by the function MCP251XFD_EnterSleepMode()
  return ERR_OK;
}



//=============================================================================
// Wait for operation mode change of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_WaitOperationModeChange(MCP251XFD *pComp, eMCP251XFD_OperationMode askedMode)
{
#ifdef CHECK_NULL_PARAM
  if (pComp == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;
  uint8_t Config;

  uint32_t StartTime = pComp->fnGetCurrentms();                       // Start the timeout
  while (true)
  {
    Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiCON+2, &Config); // Read current configuration mode with the current driver configuration
    if (Error != ERR_OK) return Error;                                // If there is an error while reading the register then return the error
    if (MCP251XFD_CAN_CiCON8_OPMOD_GET(Config) == askedMode) break;   // Check if the controller is in configuration mode
    if (MCP251XFD_TIME_DIFF(StartTime, pComp->fnGetCurrentms()) > 7)  // Wait at least 7ms because the longest message is 731 bit long and the minimum bitrate is 125kbit/s that mean 5,8ms + 2x 6bytes @ 1Mbit/s over SPI that mean 96�s = ~6ms + 1ms because GetCurrentms can be 1 cycle before the new ms
      return ERR__DEVICE_TIMEOUT;                                     // Timeout? return the error
  }
  return ERR_OK;
}



//=============================================================================
// Configure CAN Control of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureCANController(MCP251XFD *pComp, setMCP251XFD_CANCtrlFlags flags, eMCP251XFD_Bandwidth bandwidth)
{
  eERRORRESULT Error;
  uint32_t Config;

  Error = MCP251XFD_ReadSFR32(pComp, RegMCP251XFD_CiCON, &Config);                                         // Read actual configuration of the CiTDC register (Last byte only)
  if (Error != ERR_OK) return Error;                                                                       // If there is an error while calling MCP251XFD_ReadSFR32() then return the error
  if (MCP251XFD_CAN_CiCON_OPMOD_GET(Config) != MCP251XFD_CONFIGURATION_MODE) return ERR__NEED_CONFIG_MODE; // Device must be in Configuration Mode to perform the configuration

  Config &= ~(MCP251XFD_CAN_CiCON_TXBWS_Mask | MCP251XFD_CAN_CiCON_REQOP_Mask | MCP251XFD_CAN_CiCON_ABAT   // Clear by default all flags that can be changed
            | MCP251XFD_CAN_CiCON_OPMOD_Mask | MCP251XFD_CAN_CiCON_SERR2LOM | MCP251XFD_CAN_CiCON_ESIGM
            | MCP251XFD_CAN_CiCON_RTXAT | MCP251XFD_CAN_CiCON_BRSDIS | MCP251XFD_CAN_CiCON_PXEDIS
            | MCP251XFD_CAN_CiCON_ISOCRCEN);
  Config |= MCP251XFD_CAN_CiCON_REQOP_SET(MCP251XFD_CONFIGURATION_MODE);                                   // Stay in configuration mode because these change are done in configuration mode only
  Config |= MCP251XFD_CAN_CiCON_TXBWS_SET(bandwidth);                                                      // Set the delay between two consecutive transmissions (in arbitration bit times)
  if ((flags & MCP251XFD_CAN_LISTEN_ONLY_MODE_ON_ERROR    ) > 0) Config |= MCP251XFD_CAN_CiCON_SERR2LOM;   // Set transition to Listen Only Mode on system error
  if ((flags & MCP251XFD_CAN_GATEWAY_MODE_ESI_RECESSIVE   ) > 0) Config |= MCP251XFD_CAN_CiCON_ESIGM;      // Set transmit ESI in Gateway Mode, ESI is transmitted recessive when ESI of message is high or CAN controller error passive
  if ((flags & MCP251XFD_CAN_RESTRICTED_RETRANS_ATTEMPTS  ) > 0) Config |= MCP251XFD_CAN_CiCON_RTXAT;      // Set restricted retransmission attempts, CiFIFOCONm.TXAT is used
  if ((flags & MCP251XFD_CANFD_BITRATE_SWITCHING_DISABLE  ) > 0) Config |= MCP251XFD_CAN_CiCON_BRSDIS;     // Set Bit Rate Switching is Disabled, regardless of BRS in the Transmit Message Object
  if ((flags & MCP251XFD_CAN_PROTOCOL_EXCEPT_AS_FORM_ERROR) > 0) Config |= MCP251XFD_CAN_CiCON_PXEDIS;     // Set Protocol Exception is treated as a Form Error. A recessive "res bit" following a recessive FDF bit is called a Protocol Exception
  if ((flags & MCP251XFD_CANFD_USE_ISO_CRC                ) > 0) Config |= MCP251XFD_CAN_CiCON_ISOCRCEN;   // Set Include Stuff Bit Count in CRC Field and use Non-Zero CRC Initialization Vector according to ISO 11898-1:2015
  Error = MCP251XFD_WriteSFR32(pComp, RegMCP251XFD_CiCON, Config);                                         // Write new configuration to the CiTDC register (Last byte only)
  if (Error != ERR_OK) return Error;                                                                       // If there is an error while calling MCP251XFD_WriteSFR8() then return the error

  uint8_t TConfig;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiTDC_CONFIG, &TConfig); // Read actual configuration of the CiTDC register (Last byte only)
  if (Error != ERR_OK) return Error;                                      // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  TConfig &= ~MCP251XFD_CAN_CiTDC8_SID11EN;                               // Clear the flag
  if ((flags & MCP251XFD_CANFD_USE_RRS_BIT_AS_SID11) > 0)                 // Use SID11?
  {
    TConfig |= MCP251XFD_CAN_CiTDC8_SID11EN;                              // Add use SID11 flag
    pComp->InternalConfig |= MCP251XFD_CANFD_USE_RRS_BIT_AS_SID11;        // Add use SID11 to the internal configuration
  }
  else pComp->InternalConfig &= ~MCP251XFD_CANFD_USE_RRS_BIT_AS_SID11;    // Else clear use SID11 to the internal configuration
  return MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CiTDC_CONFIG, TConfig);  // Write new configuration to the CiTDC register (Last byte only)
}





//**********************************************************************************************************************************************************
//=============================================================================
// Sleep mode configuration of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureSleepMode(MCP251XFD *pComp, bool useLowPowerMode, eMCP251XFD_WakeUpFilter wakeUpFilter, bool interruptBusWakeUp)
{
#ifdef CHECK_NULL_PARAM
  if (pComp == NULL) return ERR__PARAMETER_ERROR;
#endif
  if (useLowPowerMode && (MCP251XFD_DEV_ID_GET(pComp->InternalConfig) == MCP2517FD)) return ERR__NOT_SUPPORTED; // If the device is MCP2517FD then it does not support Low Power Mode
  eERRORRESULT Error;

  uint8_t Config;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_OSC_CONFIG, &Config);                  // Read the Oscillator Register configuration
  if (Error != ERR_OK) return Error;                                                    // If there is an error while reading the SFR then return the error
  if (useLowPowerMode)                                                                  // If the device support the Low Power Mode
  {
    Config |= MCP251XFD_SFR_OSC8_LPMEN;                                                 // Set OSC.LPMEN bit
    pComp->InternalConfig |= MCP251XFD_SFR_OSC8_LPMEN;                                  // Set the LPM in the internal config
  }
  else
  {
    Config &= ~MCP251XFD_SFR_OSC8_LPMEN;                                                // Clear OSC.LPMEN bit
    pComp->InternalConfig &= ~MCP251XFD_SFR_OSC8_LPMEN;                                 // Clear the LPM in the internal config
  }
  Error = MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_OSC_CONFIG, Config);                  // Write the Oscillator Register configuration
  if (Error != ERR_OK) return Error;                                                    // If there is an error while writing the SFR then return the error

  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiCON+1, &Config);                     // Read actual flags configuration of the RegMCP251XFD_CiCON register (Second byte only)
  if (Error != ERR_OK) return Error;                                                    // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  Config &= ~(MCP251XFD_CAN_CiCON8_WFT_Mask | MCP251XFD_CAN_CiCON8_WAKFIL);             // Clear actual filter configuration
  if (wakeUpFilter != MCP251XFD_NO_FILTER)
    Config |= MCP251XFD_CAN_CiCON8_WFT_SET(wakeUpFilter) | MCP251XFD_CAN_CiCON8_WAKFIL; // Enable wake-up filter
  Error = MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CiCON+1, Config);                     // Write new flags configuration of the RegMCP251XFD_CiCON register (Second byte only)
  if (Error != ERR_OK) return Error;                                                    // If there is an error while writing the SFR then return the error

  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiINT_CONFIG+1, &Config);              // Read actual flags configuration of the RegMCP251XFD_CiINT register (Last byte only)
  if (Error != ERR_OK) return Error;                                                    // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  if (interruptBusWakeUp) Config |= MCP251XFD_CAN_CiINT8_WAKIE;                         // Enable the Bus Wake Up Interrupt
  else Config &= ~MCP251XFD_CAN_CiINT8_WAKIE;                                           // Disable the Bus Wake Up Interrupt
  return MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CiINT_CONFIG+1, Config);               // Write new flags configuration of the RegMCP251XFD_CiINT register (Last byte only)
}



//=============================================================================
// Enter the MCP251XFD in sleep mode
//=============================================================================
eERRORRESULT MCP251XFD_EnterSleepMode(MCP251XFD *pComp)
{
#ifdef CHECK_NULL_PARAM
  if (pComp == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;

  eMCP251XFD_PowerStates LastPS = MCP251XFD_DEV_PS_GET(pComp->InternalConfig);               // Get last power state
  if (LastPS == MCP251XFD_DEVICE_SLEEP_NOT_CONFIGURED) return ERR__CONFIGURATION;            // No configuration available to enter sleep mode
  if (LastPS != MCP251XFD_DEVICE_NORMAL_POWER_STATE)   return ERR__ALREADY_IN_SLEEP;         // Device already in sleep mode
  Error = MCP251XFD_RequestOperationMode(pComp, MCP251XFD_SLEEP_MODE, false);                // Set Sleep mode
  if (Error != ERR_OK) return Error;                                                         // If there is an error while calling MCP251XFD_RequestOperationMode() then return the error
  pComp->InternalConfig &= ~MCP251XFD_DEV_PS_Mask;
  if ((pComp->InternalConfig & MCP251XFD_SFR_OSC8_LPMEN) > 0)
       pComp->InternalConfig |= MCP251XFD_DEV_PS_SET(MCP251XFD_DEVICE_LOWPOWER_SLEEP_STATE); // If Low Power Mode then the device will be in low power mode
  else pComp->InternalConfig |= MCP251XFD_DEV_PS_SET(MCP251XFD_DEVICE_SLEEP_STATE);          // Else the device will be in sleep mode
  return ERR_OK;
}



//=============================================================================
// Verify if the MCP251XFD is in sleep mode
//=============================================================================
eERRORRESULT MCP251XFD_IsDeviceInSleepMode(MCP251XFD *pComp, bool* isInSleepMode)
{
#ifdef CHECK_NULL_PARAM
  if (pComp == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;

  eMCP251XFD_PowerStates LastPS = MCP251XFD_DEV_PS_GET(pComp->InternalConfig);          // Get last power state
  if (LastPS == MCP251XFD_DEVICE_SLEEP_NOT_CONFIGURED) return ERR__CONFIGURATION;       // No configuration available to enter sleep mode
  *isInSleepMode = true;
  if (LastPS == MCP251XFD_DEVICE_LOWPOWER_SLEEP_STATE) return ERR__NOT_SUPPORTED;       // Here if the device is in DEVICE_LOWPOWER_SLEEP_STATE, a simple assert of SPI CS exit the LPM then this function is not supported
  uint8_t Config;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_OSC_CONFIG, &Config);                  // Read the Oscillator Register configuration
  if (Error != ERR_OK) return Error;                                                    // If there is an error while reading the SFR then return the error
  *isInSleepMode = ((Config & MCP251XFD_SFR_OSC8_OSCDIS) > 0);                          // Return the actual state of the sleep mode
  if (*isInSleepMode == false)
  {
    pComp->InternalConfig &= ~MCP251XFD_DEV_PS_Mask;
    pComp->InternalConfig |= MCP251XFD_DEV_PS_SET(MCP251XFD_DEVICE_NORMAL_POWER_STATE); // If the function return is not in sleep mode then refresh the internal state of the device
  }
  return ERR_OK;
}



//=============================================================================
// Manually wake up the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_WakeUp(MCP251XFD *pComp, eMCP251XFD_PowerStates *fromState)
{
#ifdef CHECK_NULL_PARAM
  if (pComp == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;

  eMCP251XFD_PowerStates LastPS = MCP251XFD_DEV_PS_GET(pComp->InternalConfig);        // Get last power state
  if (LastPS == MCP251XFD_DEVICE_SLEEP_NOT_CONFIGURED) return ERR__CONFIGURATION;     // No configuration available to wake up

  uint8_t Config;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_OSC_CONFIG, &Config);                // Read the Oscillator Register configuration (Here if the device is in DEVICE_LOWPOWER_SLEEP_STATE, the reset already happened because a simple assert of SPI CS exit the LPM)
  if (Error != ERR_OK) return Error;                                                  // If there is an error while reading the SFR then return the error

  if (LastPS != MCP251XFD_DEVICE_LOWPOWER_SLEEP_STATE)                                // In Sleep mode or a false normal state, there is more to do
  {
    Config &= ~MCP251XFD_SFR_OSC8_OSCDIS;                                             // Clear OSC.OSCDIS bit
    Error = MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_OSC_CONFIG, Config);              // Write the Oscillator Register configuration
    if (Error != ERR_OK) return Error;                                                // If there is an error while writing the SFR then return the error
  }
  if (fromState != NULL) *fromState = LastPS;                                         // Return the previous sleep mode
  pComp->InternalConfig &= ~MCP251XFD_DEV_PS_Mask;
  pComp->InternalConfig |= MCP251XFD_DEV_PS_SET(MCP251XFD_DEVICE_NORMAL_POWER_STATE); // Set normal power state
  return ERR_OK;
}



//=============================================================================
// Retrieve from which state mode the MCP251XFD get a bus wake up from
//=============================================================================
eMCP251XFD_PowerStates MCP251XFD_BusWakeUpFromState(MCP251XFD *pComp)
{
#ifdef CHECK_NULL_PARAM
  if (pComp == NULL) return MCP251XFD_DEVICE_SLEEP_NOT_CONFIGURED;
#endif
  eMCP251XFD_PowerStates PowerState = MCP251XFD_DEV_PS_GET(pComp->InternalConfig);
  pComp->InternalConfig &= ~MCP251XFD_DEV_PS_Mask;
  pComp->InternalConfig |= MCP251XFD_DEV_PS_SET(MCP251XFD_DEVICE_NORMAL_POWER_STATE); // Set normal power state
  return PowerState;                                                                  // Return last power state
}





//**********************************************************************************************************************************************************
//=============================================================================
// Configure the Time Stamp of frames in the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureTimeStamp(MCP251XFD *pComp, bool enableTS, eMCP251XFD_SamplePoint samplePoint, uint16_t prescaler, bool interruptBaseCounter)
{
  if (prescaler < (MCP251XFD_CAN_CiTSCON_TBCPRE_MINVALUE + 1)) return ERR__PARAMETER_ERROR;
  if (prescaler > (MCP251XFD_CAN_CiTSCON_TBCPRE_MAXVALUE + 1)) return ERR__PARAMETER_ERROR;
  eERRORRESULT Error;
  MCP251XFD_CiTSCON_Register Config;

  //--- Write Time Stamp configuration ----------------------
  Config.CiTSCON = MCP251XFD_CAN_CiTSCON_TBCDIS;                                 // Initialize the register
  if (enableTS)
  {
    Config.CiTSCON |= MCP251XFD_CAN_CiTSCON_TBCEN;                               // Add Enable TS flag
    Config.CiTSCON |= MCP251XFD_CAN_CiTSCON_TSSP_SET(samplePoint);               // Set sample point position
    Config.CiTSCON |= MCP251XFD_CAN_CiTSCON_TBCPRE_SET(prescaler - 1);           // Set prescaler (time in �s is: 1/SYSCLK/TBCPRE)
  }
  Error = MCP251XFD_WriteData(pComp, RegMCP251XFD_CiTSCON, &Config.Bytes[0], 3); // Write new configuration to the CiTDC register (First 3-bytes only)
  if (Error != ERR_OK) return Error;                                             // If there is an error while calling MCP251XFD_WriteData() then return the error

  uint8_t Flags;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiINT_CONFIG, &Flags);          // Read actual flags configuration of the CiINT register (Third byte only)
  if (Error != ERR_OK) return Error;                                             // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  if (interruptBaseCounter) Flags |= MCP251XFD_CAN_CiINT8_TBCIE;                 // Add Time Base Counter Interrupt Enable flag
  else Flags &= ~MCP251XFD_CAN_CiINT8_TBCIE;                                     // Else clear the interrupt flag
  return MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CiINT_CONFIG, Flags);           // Write the new flags configuration to the CiINT register (Third byte only)
}



//=============================================================================
// Set the Time Stamp counter the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_SetTimeStamp(MCP251XFD *pComp, uint32_t value)
{
  return MCP251XFD_WriteSFR32(pComp, RegMCP251XFD_CiTBC, value); // Write the new value to the CiTBC register
}



//=============================================================================
// Get the Time Stamp counter the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetTimeStamp(MCP251XFD *pComp, uint32_t* value)
{
  return MCP251XFD_ReadSFR32(pComp, RegMCP251XFD_CiTBC, value); // Read the value to the CiTBC register
}





//**********************************************************************************************************************************************************
//=============================================================================
// Configure TEF of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureTEF(MCP251XFD *pComp, bool enableTEF, MCP251XFD_FIFO *confTEF)
{
  eERRORRESULT Error;

  //--- Enable/Disable TEF ---
  uint8_t CiCONflags;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiCON+2, &CiCONflags);                                         // Read actual flags configuration of the RegMCP251XFD_CiCON register (Third byte only)
  if (Error != ERR_OK) return Error;                                                                            // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  if (MCP251XFD_CAN_CiCON8_OPMOD_GET(CiCONflags) != MCP251XFD_CONFIGURATION_MODE) return ERR__NEED_CONFIG_MODE; // Device must be in Configuration Mode to perform the configuration
  if (enableTEF) CiCONflags |= MCP251XFD_CAN_CiCON8_STEF;                                                       // Add Enable Transmit Event FIFO flag
  else CiCONflags &= ~MCP251XFD_CAN_CiCON8_STEF;                                                                // Else Disable Transmit Event FIFO flag
  Error = MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CiCON+2, CiCONflags);                                         // Write the new flags configuration to the RegMCP251XFD_CiCON register (Third byte only)
  if (Error != ERR_OK) return Error;                                                                            // If there is an error while calling MCP251XFD_WriteSFR8() then return the error

  //--- Configure TEF --
  if (enableTEF)
  {
#ifdef CHECK_NULL_PARAM
    if (confTEF == NULL) return ERR__PARAMETER_ERROR;
#endif
    if (confTEF->Name != MCP251XFD_TEF) return ERR__PARAMETER_ERROR;
    MCP251XFD_CiTEFCON_Register Reg;
    uint8_t Size = MCP251XFD_CAN_TX_EVENTOBJECT_SIZE;                                                              // By default 1 element is 2x4-bytes in RAM
    Reg.CiTEFCON = MCP251XFD_CAN_CiTEFCON_FSIZE_SET(confTEF->Size);                                                // Add FIFO Size
    if ((confTEF->ControlFlags   & MCP251XFD_FIFO_ADD_TIMESTAMP_ON_OBJ    ) > 0)                                   // Transmit Event FIFO Time Stamp Enable
    { Reg.CiTEFCON |= MCP251XFD_CAN_CiTEFCON_TEFTSEN; Size += 4; }
    if ((confTEF->InterruptFlags & MCP251XFD_FIFO_OVERFLOW_INT            ) > 0) Reg.CiTEFCON |= MCP251XFD_CAN_CiTEFCON_TEFOVIE; // Add Transmit Event FIFO Overflow Interrupt Enable
    if ((confTEF->InterruptFlags & MCP251XFD_FIFO_EVENT_FIFO_FULL_INT     ) > 0) Reg.CiTEFCON |= MCP251XFD_CAN_CiTEFCON_TEFFIE;  // Add Transmit Event FIFO Full Interrupt Enable
    if ((confTEF->InterruptFlags & MCP251XFD_FIFO_EVENT_FIFO_HALF_FULL_INT) > 0) Reg.CiTEFCON |= MCP251XFD_CAN_CiTEFCON_TEFHIE;  // Add Transmit Event FIFO Half Full Interrupt Enable
    if ((confTEF->InterruptFlags & MCP251XFD_FIFO_EVENT_FIFO_NOT_EMPTY_INT) > 0) Reg.CiTEFCON |= MCP251XFD_CAN_CiTEFCON_TEFNEIE; // Add Transmit Event FIFO Not Empty Interrupt Enable

    Error = MCP251XFD_WriteData(pComp, RegMCP251XFD_CiTEFCON, &Reg.Bytes[0], sizeof(MCP251XFD_CiTEFCON_Register)); // Write TEF configuration
    if (Error != ERR_OK) return Error;                                                                             // If there is an error while calling MCP251XFD_WriteData() then return the error

    if (confTEF->RAMInfos != NULL)
    {
      confTEF->RAMInfos->ByteInObject    = Size;                                                                   // Set size of 1 object in the TEF
      confTEF->RAMInfos->ByteInFIFO      = (Size * ((uint8_t)confTEF->Size + 1));                                  // Total size of the TEF in RAM is 1 element size x Element Count
      confTEF->RAMInfos->RAMStartAddress = MCP251XFD_RAM_ADDR;                                                     // Set the start address of the TEF. Here the TEF is always the first in RAM
    }
  }

  return ERR_OK;
}



//=============================================================================
// Configure TXQ of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureTXQ(MCP251XFD *pComp, bool enableTXQ, MCP251XFD_FIFO *confTXQ)
{
  eERRORRESULT Error;

  //--- Enable/Disable TXQ ---
  uint8_t CiCONflags;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiCON+2, &CiCONflags);                                         // Read actual flags configuration of the RegMCP251XFD_CiCON register (Third byte only)
  if (Error != ERR_OK) return Error;                                                                            // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  if (MCP251XFD_CAN_CiCON8_OPMOD_GET(CiCONflags) != MCP251XFD_CONFIGURATION_MODE) return ERR__NEED_CONFIG_MODE; // Device must be in Configuration Mode to perform the configuration
  if (enableTXQ) CiCONflags |= MCP251XFD_CAN_CiCON8_TXQEN;                                                      // Add Enable Transmit Queue flag
  else CiCONflags &= ~MCP251XFD_CAN_CiCON8_TXQEN;                                                               // Else Disable Transmit Queue flag
  Error = MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CiCON+2, CiCONflags);                                         // Write the new flags configuration to the RegMCP251XFD_CiCON register (Third byte only)
  if (Error != ERR_OK) return Error;                                                                            // If there is an error while calling MCP251XFD_WriteSFR8() then return the error

  //--- Configure TXQ --
  if (enableTXQ)
  {
#ifdef CHECK_NULL_PARAM
    if (confTXQ == NULL) return ERR__PARAMETER_ERROR;
#endif
    if (confTXQ->Name != MCP251XFD_TXQ) return ERR__PARAMETER_ERROR;
    uint8_t Size = sizeof(MCP251XFD_CAN_TX_Message);                                                               // By default 1 element is 2x4-bytes in RAM
    MCP251XFD_CiTXQCON_Register Reg;
    Reg.CiTXQCON  = MCP251XFD_CAN_CiTXQCON_PLSIZE_SET(confTXQ->Payload);                                           // Add Payload Size
    Size += MCP251XFD_PayloadToByte(confTXQ->Payload);                                                             // Add Payload size to Size of message
    Reg.CiTXQCON |= MCP251XFD_CAN_CiTXQCON_FSIZE_SET(confTXQ->Size);                                               // Add FIFO Size
    Reg.CiTXQCON |= MCP251XFD_CAN_CiTXQCON_TXAT_SET(confTXQ->Attempts);                                            // Add Retransmission Attempts
    Reg.CiTXQCON |= MCP251XFD_CAN_CiTXQCON_TXPRI_SET(confTXQ->Priority);                                           // Add Message transmit priority
    if ((confTXQ->InterruptFlags & MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT ) > 0) Reg.CiTXQCON |= MCP251XFD_CAN_CiTXQCON_TXATIE; // Add Transmit Attempts Exhausted Interrupt Enable
    if ((confTXQ->InterruptFlags & MCP251XFD_FIFO_TRANSMIT_FIFO_EMPTY_INT   ) > 0) Reg.CiTXQCON |= MCP251XFD_CAN_CiTXQCON_TXQEIE; // Add Transmit FIFO Empty Interrupt Enable
    if ((confTXQ->InterruptFlags & MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT) > 0) Reg.CiTXQCON |= MCP251XFD_CAN_CiTXQCON_TXQNIE; // Add Transmit FIFO Not Full Interrupt Enable

    Error = MCP251XFD_WriteData(pComp, RegMCP251XFD_CiTXQCON, &Reg.Bytes[0], sizeof(MCP251XFD_CiTXQCON_Register)); // Write TXQ configuration
    if (Error != ERR_OK) return Error;                                                                             // If there is an error while calling MCP251XFD_WriteData() then return the error

    if (confTXQ->RAMInfos != NULL)
    {
      confTXQ->RAMInfos->ByteInObject    = Size;                                                                   // Set size of 1 object in the TXQ
      confTXQ->RAMInfos->ByteInFIFO      = (Size * ((uint8_t)confTXQ->Size + 1));                                  // Total size of the TXQ in RAM is 1 element size x Element Count
      confTXQ->RAMInfos->RAMStartAddress = 0;                                                                      // Can't know the start address of the TXQ info here
    }
    if ((Size * ((uint8_t)confTXQ->Size + 1u)) > MCP251XFD_RAM_SIZE) return ERR__OUT_OF_MEMORY;
  }

  return ERR_OK;
}



//=============================================================================
// Configure a FIFO of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureFIFO(MCP251XFD *pComp, MCP251XFD_FIFO *confFIFO)
{
#ifdef CHECK_NULL_PARAM
  if (confFIFO == NULL) return ERR__PARAMETER_ERROR;
#endif
  if ((confFIFO->Name == MCP251XFD_TEF) || (confFIFO->Name == MCP251XFD_TXQ) || (confFIFO->Name >= MCP251XFD_FIFO_COUNT)) return ERR__PARAMETER_ERROR;
  eERRORRESULT Error;

  //--- Device in Configuration Mode ---
  eMCP251XFD_OperationMode OpMode;
  Error = MCP251XFD_GetActualOperationMode(pComp, &OpMode);                 // Get actual Operational Mode
  if (Error != ERR_OK) return Error;                                        // If there is an error while calling MCP251XFD_GetActualOperationMode() then return the error
  if (OpMode != MCP251XFD_CONFIGURATION_MODE) return ERR__NEED_CONFIG_MODE; // Device must be in Configuration Mode to perform the configuration

  //--- Configure FIFO --
  MCP251XFD_CiFIFOCONm_Register Reg;
  uint8_t Size = sizeof(MCP251XFD_CAN_TX_Message);                                                          // By default 1 element is 2x4-bytes in RAM
  Reg.CiFIFOCONm  = MCP251XFD_CAN_CiFIFOCONm_PLSIZE_SET(confFIFO->Payload);                                 // Add Payload Size
  Size += MCP251XFD_PayloadToByte(confFIFO->Payload);                                                       // Add Payload size to Size of message
  Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_FSIZE_SET(confFIFO->Size);                                     // Add FIFO Size
  Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_TXAT_SET(confFIFO->Attempts);                                  // Add Retransmission Attempts
  Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_TXPRI_SET(confFIFO->Priority);                                 // Add Message transmit priority
  if (confFIFO->Direction == MCP251XFD_TRANSMIT_FIFO)
  {
    Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_TXEN;                                                        // Transmit Event FIFO Time Stamp Enable
    if ((confFIFO->InterruptFlags & MCP251XFD_FIFO_TX_ATTEMPTS_EXHAUSTED_INT   ) > 0) Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_TXATIE;   // Add Transmit Attempts Exhausted Interrupt Enable
    if ((confFIFO->ControlFlags   & MCP251XFD_FIFO_AUTO_RTR_RESPONSE           ) > 0) Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_RTREN;    // When a remote transmit is received, Transmit Request (TXREQ) of the FIFO will be set
    if ((confFIFO->InterruptFlags & MCP251XFD_FIFO_TRANSMIT_FIFO_EMPTY_INT     ) > 0) Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_TFERFFIE; // Add Transmit FIFO Empty Interrupt Enable
    if ((confFIFO->InterruptFlags & MCP251XFD_FIFO_TRANSMIT_FIFO_HALF_EMPTY_INT) > 0) Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_TFHRFHIE; // Add Transmit FIFO Half Empty Interrupt Enable
    if ((confFIFO->InterruptFlags & MCP251XFD_FIFO_TRANSMIT_FIFO_NOT_FULL_INT  ) > 0) Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_TFNRFNIE; // Add Transmit FIFO Not Full Interrupt Enable
  }
  else
  {
    if ((confFIFO->ControlFlags   & MCP251XFD_FIFO_ADD_TIMESTAMP_ON_RX         ) > 0)                                                      // Receive FIFO Time Stamp Enable
    { Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_RXTSEN; Size += 4; }
    if ((confFIFO->InterruptFlags & MCP251XFD_FIFO_OVERFLOW_INT                ) > 0) Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_RXOVIE;   // Add Transmit Attempts Exhausted Interrupt Enable
    if ((confFIFO->InterruptFlags & MCP251XFD_FIFO_RECEIVE_FIFO_FULL_INT       ) > 0) Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_TFERFFIE; // Add Receive FIFO Full Interrupt Enable
    if ((confFIFO->InterruptFlags & MCP251XFD_FIFO_RECEIVE_FIFO_HALF_FULL_INT  ) > 0) Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_TFHRFHIE; // Add Receive FIFO Half Full Interrupt Enable
    if ((confFIFO->InterruptFlags & MCP251XFD_FIFO_RECEIVE_FIFO_NOT_EMPTY_INT  ) > 0) Reg.CiFIFOCONm |= MCP251XFD_CAN_CiFIFOCONm_TFNRFNIE; // Add Receive FIFO Not Empty Interrupt Enable
  }

  uint16_t Address = RegMCP251XFD_CiFIFOCONm + (MCP251XFD_FIFO_REG_SIZE * ((uint16_t)confFIFO->Name - 1u)); // Select the address of the FIFO
  Error = MCP251XFD_WriteData(pComp, Address, &Reg.Bytes[0], sizeof(MCP251XFD_CiFIFOCONm_Register));        // Write FIFO configuration
  if (Error != ERR_OK) return Error;                                                                        // If there is an error while calling MCP251XFD_WriteData() then return the error

  if (confFIFO->RAMInfos != NULL)
  {
    confFIFO->RAMInfos->ByteInObject    = Size;                                                             // Set size of 1 object in the FIFO
    confFIFO->RAMInfos->ByteInFIFO      = (Size * ((uint8_t)confFIFO->Size + 1));                           // Total size of the FIFO in RAM is 1 element size x Element Count
    confFIFO->RAMInfos->RAMStartAddress = 0;                                                                // Can't know the start address of the FIFO info here
  }
  if ((Size * ((uint8_t)confFIFO->Size + 1u)) > MCP251XFD_RAM_SIZE) return ERR__OUT_OF_MEMORY;

  return ERR_OK;
}



//=============================================================================
// Configure a FIFO list of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureFIFOList(MCP251XFD *pComp, MCP251XFD_FIFO *listFIFO, size_t count)
{
#ifdef CHECK_NULL_PARAM
  if (listFIFO == NULL) return ERR__PARAMETER_ERROR;
#endif
  if (count == 0) return ERR_OK;
  if (count > MCP251XFD_FIFO_CONF_MAX) return ERR__OUT_OF_RANGE;
  eERRORRESULT Error;
  uint16_t TotalSize = 0;
  MCP251XFD_RAMInfos TmpRAMInfos;
  bool UseTmpRAMInfos = false;

  //--- First: Configure TEF if any ---
  uint8_t TEFcount = 0;                                             // The TEF always start at address MCP251XFD_RAM_ADDR so it must be first
  for (size_t zTEF = 0; zTEF < count; zTEF++)
  {
    if (listFIFO[zTEF].Name == MCP251XFD_TEF)
    {
      TEFcount++;
      if (TEFcount > MCP251XFD_TEF_MAX) return ERR__TOO_MANY_TEF;

      //--- Configure TEF ---
      UseTmpRAMInfos = (listFIFO[zTEF].RAMInfos == NULL);           // Is there a RAMInfos structure attached to the configuration?
      if (UseTmpRAMInfos) listFIFO[zTEF].RAMInfos = &TmpRAMInfos;   // If not RAMInfos structure attached, then set a temporary one
      Error = MCP251XFD_ConfigureTEF(pComp, true, &listFIFO[zTEF]); // Configure the TEF
      if (Error != ERR_OK) return Error;                            // If there is an error while calling MCP251XFD_ConfigureTEF() then return the error
      TotalSize += listFIFO[zTEF].RAMInfos->ByteInFIFO;             // Add TEF size to the total
      if (UseTmpRAMInfos) listFIFO[zTEF].RAMInfos = NULL;           // If not RAMInfos structure attached, then unset the temporary one
    }
  }
  if (TEFcount == 0)
  {
    Error = MCP251XFD_ConfigureTEF(pComp, false, NULL);             // Deactivate the TEF
    if (Error != ERR_OK) return Error;                              // If there is an error while calling MCP251XFD_ConfigureTEF() then return the error
  }

  //--- Second: Configure TXQ if any ---
  uint8_t TXQcount = 0;                                             // The TXQ always follow the TEF (if no TEF then it's the first) in RAM
  for (size_t zTXQ = 0; zTXQ < count; zTXQ++)
  {
    if (listFIFO[zTXQ].Name == MCP251XFD_TXQ)
    {
      TXQcount++;
      if (TXQcount > MCP251XFD_TXQ_MAX) return ERR__TOO_MANY_TXQ;

      //--- Configure TXQ --
      UseTmpRAMInfos = (listFIFO[zTXQ].RAMInfos == NULL);                        // Is there a RAMInfos structure attached to the configuration?
      if (UseTmpRAMInfos) listFIFO[zTXQ].RAMInfos = &TmpRAMInfos;                // If not RAMInfos structure attached, then set a temporary one
      Error = MCP251XFD_ConfigureTXQ(pComp, true, &listFIFO[zTXQ]);              // Configure the TXQ
      if (Error != ERR_OK) return Error;                                         // If there is an error while calling MCP251XFD_ConfigureTXQ() then return the error
      listFIFO[zTXQ].RAMInfos->RAMStartAddress = MCP251XFD_RAM_ADDR + TotalSize; // Set start address
      TotalSize += listFIFO[zTXQ].RAMInfos->ByteInFIFO;                          // Add TXQ size to the total
      if (UseTmpRAMInfos) listFIFO[zTXQ].RAMInfos = NULL;                        // If not RAMInfos structure attached, then unset the temporary one
    }
  }
  if (TXQcount == 0)
  {
    Error = MCP251XFD_ConfigureTXQ(pComp, false, NULL);                          // Deactivate the TxQ
    if (Error != ERR_OK) return Error;                                           // If there is an error while calling MCP251XFD_ConfigureTEF() then return the error
  }

  //--- Third: Configure FIFOs if any ---
  int32_t LastFIFO = 0;
  for (int32_t zFIFO = 1; zFIFO < MCP251XFD_FIFO_COUNT; zFIFO++)
  {
    for (size_t z = 0; z < count; z++)
    {
      if (listFIFO[z].Name == zFIFO)
      {
        for (int32_t clearFIFO = LastFIFO + 1; clearFIFO < zFIFO; clearFIFO++)     // For each FIFO not listed between 2 FIFO
        {
          Error = MCP251XFD_ClearFIFOConfiguration(pComp, (eMCP251XFD_FIFO)zFIFO); // Clear FIFO configuration
          if (Error != ERR_OK) return Error;                                       // If there is an error while calling MCP251XFD_ClearFIFOConfiguration() then return the error
          TotalSize += MCP251XFD_FIFO_MIN_SIZE;                                    // Add min FIFO size. A FIFO cannot be completely disable so, it takes the minimum possible size
        }
        LastFIFO = zFIFO;
        //--- Configure FIFO --
        UseTmpRAMInfos = (listFIFO[z].RAMInfos == NULL);                        // Is there a RAMInfos structure attached to the configuration?
        if (UseTmpRAMInfos) listFIFO[z].RAMInfos = &TmpRAMInfos;                // If not RAMInfos structure attached, then set a temporary one
        Error = MCP251XFD_ConfigureFIFO(pComp, &listFIFO[z]);                   // Configure the FIFO
        if (Error != ERR_OK) return Error;                                      // If there is an error while calling MCP251XFD_ConfigureFIFO() then return the error
        listFIFO[z].RAMInfos->RAMStartAddress = MCP251XFD_RAM_ADDR + TotalSize; // Set start address
        TotalSize += listFIFO[z].RAMInfos->ByteInFIFO;                          // Add FIFO size to the total
        if (UseTmpRAMInfos) listFIFO[z].RAMInfos = NULL;                        // If not RAMInfos structure attached, then unset the temporary one
      }
    }
  }

  //--- Check RAM used ---
  if (TotalSize > MCP251XFD_RAM_SIZE) return ERR__OUT_OF_MEMORY;

  return ERR_OK;
}



//=============================================================================
// Reset a FIFO of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ResetFIFO(MCP251XFD *pComp, eMCP251XFD_FIFO name)
{
  if (name >= MCP251XFD_FIFO_COUNT) return ERR__PARAMETER_ERROR;
  eERRORRESULT Error;

  //--- Device in Configuration Mode ---
  eMCP251XFD_OperationMode OpMode;
  Error = MCP251XFD_GetActualOperationMode(pComp, &OpMode);                                               // Get actual Operational Mode
  if (Error != ERR_OK) return Error;                                                                      // If there is an error while calling MCP251XFD_GetActualOperationMode() then return the error
  if (OpMode == MCP251XFD_CONFIGURATION_MODE) return ERR_OK;                                              // Device in Configuration Mode automatically reset the FIFO

  //--- Set Reset of the FIFO ---
  uint16_t Address = RegMCP251XFD_CiFIFOCONm_CONTROL + (MCP251XFD_FIFO_REG_SIZE * ((uint16_t)name - 1u)); // Select the address of the FIFO
  if (name == MCP251XFD_TEF) Address = RegMCP251XFD_CiTEFCON_CONTROL;                                     // If it's the TEF then select its address
  if (name == MCP251XFD_TXQ) Address = RegMCP251XFD_CiTXQCON_CONTROL;                                     // If it's the TXQ then select its address
  Error = MCP251XFD_WriteSFR8(pComp, Address, MCP251XFD_CAN_CiFIFOCONm8_FRESET);                          // Write FIFO configuration (Second byte only)
  if (Error != ERR_OK) return Error;                                                                      // If there is an error while calling MCP251XFD_WriteSFR8() then return the error

  //--- Now wait the reset to be effective ---
  uint8_t Config = 0;
  uint32_t StartTime = pComp->fnGetCurrentms();                                                           // Start the timeout
  while (true)
  {
    Error = MCP251XFD_ReadSFR8(pComp, Address, &Config);                                                  // Read current FIFO configuration (Second byte only)
    if (Error != ERR_OK) return Error;                                                                    // If there is an error while reading the register then return the error
    if ((Config & MCP251XFD_CAN_CiFIFOCONm8_FRESET) == 0) break;                                          // Check if the FIFO was reset
    if (MCP251XFD_TIME_DIFF(StartTime, pComp->fnGetCurrentms()) > 3) return ERR__DEVICE_TIMEOUT;          // Wait at least 3ms. If timeout then return the error
  }
  return ERR_OK;
}



//=============================================================================
// Update (and flush) a FIFO of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_UpdateFIFO(MCP251XFD *pComp, eMCP251XFD_FIFO name, bool andFlush)
{
  if (name >= MCP251XFD_FIFO_COUNT) return ERR__PARAMETER_ERROR;

  //--- Set address of the FIFO ---
  uint16_t Address = RegMCP251XFD_CiFIFOCONm_CONTROL + (MCP251XFD_FIFO_REG_SIZE * ((uint16_t)name - 1u)); // Select the address of the FIFO
  if (name == MCP251XFD_TEF) Address = RegMCP251XFD_CiTEFCON_CONTROL;                                     // If it's the TEF then select its address
  if (name == MCP251XFD_TXQ) Address = RegMCP251XFD_CiTXQCON_CONTROL;                                     // If it's the TXQ then select its address

  //--- Set update (and flush if ask) ---
  uint8_t Config = MCP251XFD_CAN_CiFIFOCONm8_UINC;                                                        // Set update
  if (andFlush) Config |= MCP251XFD_CAN_CiFIFOCONm8_TXREQ;                                                // Add flush if ask
  return MCP251XFD_WriteSFR8(pComp, Address, Config);                                                     // Write FIFO update (and flush) (Second byte only)
}



//=============================================================================
// Flush a FIFO of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_FlushFIFO(MCP251XFD *pComp, eMCP251XFD_FIFO name)
{
  if (name == MCP251XFD_TEF) return ERR__NOT_AVAILABLE;               // In the TEF no flush is possible
  if (name >= MCP251XFD_FIFO_COUNT) return ERR__PARAMETER_ERROR;

  //--- Set the flush ---
  uint16_t Address = RegMCP251XFD_CiTXREQ + ((uint16_t)name >> 3);    // Select the good address of the TXREQ register
  return MCP251XFD_WriteSFR8(pComp, Address, ((uint8_t)name & 0xFF)); // Write FIFO flush by selecting the good bit
}



//=============================================================================
// Get status of a FIFO of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetFIFOStatus(MCP251XFD *pComp, eMCP251XFD_FIFO name, setMCP251XFD_FIFOstatus *statusFlags)
{
#ifdef CHECK_NULL_PARAM
  if (statusFlags == NULL) return ERR__PARAMETER_ERROR;
#endif
  if (name >= MCP251XFD_FIFO_COUNT) return ERR__PARAMETER_ERROR;

  //--- Set address of the FIFO ---
  uint16_t Address = RegMCP251XFD_CiFIFOSTAm_FLAGS + (MCP251XFD_FIFO_REG_SIZE * ((uint16_t)name - 1u)); // Select the address of the FIFO
  if (name == MCP251XFD_TEF) Address = RegMCP251XFD_CiTEFSTA_FLAGS;                                     // If it's the TEF then select its address
  if (name == MCP251XFD_TXQ) Address = RegMCP251XFD_CiTXQSTA_FLAGS;                                     // If it's the TXQ then select its address

  //--- Get FIFO status ---
  return MCP251XFD_ReadSFR8(pComp, Address, (uint8_t*)statusFlags);                                     // Read FIFO status (First byte only)
}



//=============================================================================
// Get next message address and/or index of a FIFO of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetNextMessageAddressFIFO(MCP251XFD *pComp, eMCP251XFD_FIFO name, uint32_t *nextAddress, uint8_t *nextIndex)
{
  if (name >= MCP251XFD_FIFO_COUNT) return ERR__PARAMETER_ERROR;
  eERRORRESULT Error;
  uint16_t Address;

  //--- Get next message address ---
  if (nextAddress != NULL)
  {
    //--- Set address of the FIFO ---
    Address = RegMCP251XFD_CiFIFOUAm + (MCP251XFD_FIFO_REG_SIZE * ((uint16_t)name - 1u)); // Select the address of the FIFO
    if (name == MCP251XFD_TEF) Address = RegMCP251XFD_CiTEFUA;                            // If it's the TEF then select its address
    if (name == MCP251XFD_TXQ) Address = RegMCP251XFD_CiTXQUA;                            // If it's the TXQ then select its address
    //--- Get FIFO status ---
    Error = MCP251XFD_ReadSFR32(pComp, Address, nextAddress);                             // Read FIFO user address
    if (Error != ERR_OK) return Error;                                                    // If there is an error while calling MCP251XFD_ReadSFR32() then return the error
  }

  //--- Get next message index ---
  if (nextIndex != NULL)
  {
    if (name == MCP251XFD_TEF) return ERR__NOT_AVAILABLE;
    //--- Set address of the FIFO ---
    Address = RegMCP251XFD_CiFIFOSTAm_FIFOCI + (MCP251XFD_FIFO_REG_SIZE * ((uint16_t)name - 1u)); // Select the address of the FIFO
    if (name == MCP251XFD_TXQ) Address = RegMCP251XFD_CiTXQSTA_TXQCI;                             // If it's the TXQ then select its address

    //--- Get FIFO status ---
    Error = MCP251XFD_ReadSFR8(pComp, Address, nextIndex);                                        // Read FIFO status (Second byte only)
    if (Error != ERR_OK) return Error;                                                            // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
    *nextIndex &= MCP251XFD_CAN_CiFIFOSTAm8_FIFOCI_Mask;
  }
  return ERR_OK;
}



//=============================================================================
// Clear the FIFO configuration of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ClearFIFOConfiguration(MCP251XFD *pComp, eMCP251XFD_FIFO name)
{
  eERRORRESULT Error = MCP251XFD_ResetFIFO(pComp, name); // Reset the FIFO
  if (Error != ERR_OK) return Error;                     // If there is an error while calling MCP251XFD_ResetFIFO() then return the error
#ifndef __cplusplus
  MCP251XFD_FIFO ClearConf = { .Name = name, .Size = MCP251XFD_FIFO_1_MESSAGE_DEEP, .Payload = MCP251XFD_PAYLOAD_8BYTE, .Direction = MCP251XFD_RECEIVE_FIFO, .Attempts = MCP251XFD_UNLIMITED_ATTEMPTS, .Priority = MCP251XFD_MESSAGE_TX_PRIORITY1, .ControlFlags = MCP251XFD_FIFO_NO_CONTROL_FLAGS, .InterruptFlags = MCP251XFD_FIFO_NO_INTERRUPT_FLAGS, .RAMInfos = NULL, };
#else
  MCP251XFD_FIFO ClearConf = { name, MCP251XFD_FIFO_1_MESSAGE_DEEP, MCP251XFD_PAYLOAD_8BYTE, MCP251XFD_RECEIVE_FIFO, MCP251XFD_UNLIMITED_ATTEMPTS, MCP251XFD_MESSAGE_TX_PRIORITY1, MCP251XFD_FIFO_NO_CONTROL_FLAGS, MCP251XFD_FIFO_NO_INTERRUPT_FLAGS, NULL, };
#endif // !__cplusplus
  return MCP251XFD_ConfigureFIFO(pComp, &ClearConf);     // Clear the configuration
}





//**********************************************************************************************************************************************************
//=============================================================================
// Configure the Device NET filter of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureDeviceNetFilter(MCP251XFD *pComp, eMCP251XFD_DNETFilter filter)
{
  eERRORRESULT Error;

  //--- Enable/Disable DNCNT ---
  uint8_t CiCONflags;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiCON, &CiCONflags); // Read actual flags configuration of the RegMCP251XFD_CiCON register (First byte only)
  if (Error != ERR_OK) return Error;                                  // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  CiCONflags &= ~MCP251XFD_CAN_CiCON8_DNCNT_Mask;                     // Clear DNCNT config
  CiCONflags |=  MCP251XFD_CAN_CiCON8_DNCNT_SET(filter);              // Set new filter configuration
  return MCP251XFD_WriteSFR8(pComp, RegMCP251XFD_CiCON, CiCONflags);  // Write the new flags configuration to the RegMCP251XFD_CiCON register (First byte only)
}



//=============================================================================
// Configure a filter of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureFilter(MCP251XFD *pComp, MCP251XFD_Filter *confFilter)
{
#ifdef CHECK_NULL_PARAM
  if (confFilter == NULL) return ERR__PARAMETER_ERROR;
#endif
  if ((confFilter->PointTo == MCP251XFD_TEF) || (confFilter->PointTo == MCP251XFD_TXQ) || (confFilter->PointTo >= MCP251XFD_FIFO_COUNT)) return ERR__CONFIGURATION;
  eERRORRESULT Error;

  //--- Check if the filter is disabled ---
  MCP251XFD_CiFLTCONm_Register FilterConf;
  uint16_t AddrFLTCON = RegMCP251XFD_CiFLTCONm + confFilter->Filter;      // Select the address of the FLTCON
  Error = MCP251XFD_ReadSFR8(pComp, AddrFLTCON, &FilterConf.CiFLTCONm);   // Read actual flags configuration of the RegMCP251XFD_CiFLTCONm register
  if (Error != ERR_OK) return Error;                                      // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  if ((FilterConf.CiFLTCONm & MCP251XFD_CAN_CiFLTCONm_ENABLE) > 0)
  {
    FilterConf.CiFLTCONm = MCP251XFD_CAN_CiFLTCONm8_DISABLE;              // Disable the filter
    Error = MCP251XFD_WriteSFR8(pComp, AddrFLTCON, FilterConf.CiFLTCONm); // Write the new flags configuration to the RegMCP251XFD_CiFLTCONm register
    if (Error != ERR_OK) return Error;                                    // If there is an error while calling MCP251XFD_WriteSFR8() then return the error
  }

  if (confFilter->EnableFilter)
  {
    //--- Get the SID11 configuration ---
    bool UseSID11 = MCP251XFD_USE_SID11;

    //--- Check values ---
    if ((confFilter->AcceptanceID & confFilter->AcceptanceMask) != confFilter->AcceptanceID) return ERR__FILTER_CONSISTENCY;
    uint32_t MaxMask = (UseSID11 ? 1 : 0);
    switch (confFilter->Match)
    {
      case MCP251XFD_MATCH_ONLY_SID: MaxMask += MCP251XFD_SID_Size;                      break;
      case MCP251XFD_MATCH_ONLY_EID:
      case MCP251XFD_MATCH_SID_EID : MaxMask += MCP251XFD_EID_Size + MCP251XFD_SID_Size; break;
    }
    MaxMask = ~((1 << MaxMask) - 1);
    if ((confFilter->AcceptanceID   & MaxMask) > 0) return ERR__FILTER_TOO_LARGE;
    if ((confFilter->AcceptanceMask & MaxMask) > 0) return ERR__FILTER_TOO_LARGE;


    //=== Fill Filter Object register ===
    MCP251XFD_CiFLTOBJm_Register FltObj;
    FltObj.CiFLTOBJm = MCP251XFD_MessageIDtoObjectMessageIdentifier(confFilter->AcceptanceID, (confFilter->Match != MCP251XFD_MATCH_ONLY_SID), UseSID11);
    if (confFilter->Match == MCP251XFD_MATCH_ONLY_EID) FltObj.CiFLTOBJm |= MCP251XFD_CAN_CiFLTOBJm_EXIDE;      // If filter match only messages with extended identifier set the bit EXIDE
    //--- Send the filter object config ---
    uint16_t AddrFLTObj = RegMCP251XFD_CiFLTOBJm + ((uint16_t)confFilter->Filter * MCP251XFD_FILTER_REG_SIZE); // Select the address of the CiFLTOBJ
    Error = MCP251XFD_WriteSFR32(pComp, AddrFLTObj, FltObj.CiFLTOBJm);                                         // Write the new flags configuration to the RegMCP251XFD_CiFLTOBJm register
    if (Error != ERR_OK) return Error;                                                                         // If there is an error while calling MCP251XFD_WriteSFR32() then return the error


    //=== Fill Filter Mask register ===
    MCP251XFD_CiMASKm_Register FltMask;
    FltMask.CiMASKm = MCP251XFD_MessageIDtoObjectMessageIdentifier(confFilter->AcceptanceMask, (confFilter->Match != MCP251XFD_MATCH_ONLY_SID), UseSID11);
    if (confFilter->Match != MCP251XFD_MATCH_SID_EID) FltMask.CiMASKm |= MCP251XFD_CAN_CiMASKm_MIDE;           // If filter match only messages with standard or extended identifier set the bit MIDE
    //--- Send the filter mask config ---
    uint16_t AddrMask = RegMCP251XFD_CiMASKm + ((uint16_t)confFilter->Filter * MCP251XFD_FILTER_REG_SIZE);     // Select the address of the CiMASK
    Error = MCP251XFD_WriteSFR32(pComp, AddrMask, FltMask.CiMASKm);                                            // Write the new flags configuration to the RegMCP251XFD_CiMASKm register
    if (Error != ERR_OK) return Error;                                                                         // If there is an error while calling MCP251XFD_WriteSFR32() then return the error

    //=== Configure Filter control ===
    FilterConf.CiFLTCONm |= MCP251XFD_CAN_CiFLTCONm8_ENABLE;                                                   // Enable filter
    FilterConf.CiFLTCONm |= MCP251XFD_CAN_CiFLTCONm8_FBP_SET(confFilter->PointTo);                             // Set the Filter pointer to FIFO
    Error = MCP251XFD_WriteSFR8(pComp, AddrFLTCON, FilterConf.CiFLTCONm);                                      // Write the new flags configuration to the RegMCP251XFD_CiFLTCONm register
    if (Error != ERR_OK) return Error;                                                                         // If there is an error while calling MCP251XFD_WriteSFR8() then return the error
  }
  return ERR_OK;
}



//=============================================================================
// Configure a filter list and the DNCNT of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureFilterList(MCP251XFD *pComp, eMCP251XFD_DNETFilter filter, MCP251XFD_Filter *listFilter, size_t count)
{
#ifdef CHECK_NULL_PARAM
  if (listFilter == NULL) return ERR__PARAMETER_ERROR;
#endif
  if (count == 0) return ERR_OK;
  if (count > MCP251XFD_FILTER_COUNT) return ERR__OUT_OF_RANGE;
  eERRORRESULT Error;

  //--- Configure the Device NET Filter ---
  Error = MCP251XFD_ConfigureDeviceNetFilter(pComp, filter); // Configure the DNCNT
  if (Error != ERR_OK) return Error;                         // If there is an error while calling MCP251XFD_ConfigureDeviceNetFilter() then return the error

  //--- Configure filters ---
  bool Modified = false;
  for (int32_t zFilter = 0; zFilter < MCP251XFD_FILTER_COUNT; zFilter++)
  {
    for (size_t z = 0; z < count; z++)
    {
      if (listFilter[z].Filter == zFilter)
      {
        Modified = true;
        Error = MCP251XFD_ConfigureFilter(pComp, &listFilter[z]);         // Configure the filter
        if (Error != ERR_OK) return Error;                                // If there is an error while calling MCP251XFD_ConfigureFilter() then return the error
      }
    }
    if (Modified == false)                                                // Filter not listed
    {
      Error = MCP251XFD_DisableFilter(pComp, (eMCP251XFD_Filter)zFilter); // Disable the Filter configuration
      if (Error != ERR_OK) return Error;                                  // If there is an error while calling MCP251XFD_DisableFilter() then return the error
    }
    Modified = false;
  }

  return ERR_OK;
}



//=============================================================================
// Disable a Filter of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_DisableFilter(MCP251XFD *pComp, eMCP251XFD_Filter name)
{
  if (name >= MCP251XFD_FILTER_COUNT) return ERR__CONFIGURATION;
  eERRORRESULT Error;
  MCP251XFD_CiFLTCONm_Register FilterConf;

  uint16_t AddrFLTCON = RegMCP251XFD_CiFLTCONm + name;                    // Select the address of the FLTCON
  Error = MCP251XFD_ReadSFR8(pComp, AddrFLTCON, &FilterConf.CiFLTCONm);   // Read actual flags configuration of the RegMCP251XFD_CiFLTCONm register
  if (Error != ERR_OK) return Error;                                      // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  if ((FilterConf.CiFLTCONm & MCP251XFD_CAN_CiFLTCONm_ENABLE) > 0)
  {
    FilterConf.CiFLTCONm &= ~MCP251XFD_CAN_CiFLTCONm8_ENABLE;             // Disable the filter
    Error = MCP251XFD_WriteSFR8(pComp, AddrFLTCON, FilterConf.CiFLTCONm); // Write the new flags configuration to the RegMCP251XFD_CiFLTCONm register
    if (Error != ERR_OK) return Error;                                    // If there is an error while calling MCP251XFD_WriteSFR8() then return the error
  }
  return ERR_OK;
}





//**********************************************************************************************************************************************************
//=============================================================================
// Configure interrupt of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ConfigureInterrupt(MCP251XFD *pComp, setMCP251XFD_InterruptEvents interruptsFlags)
{
  eERRORRESULT Error;
  Error = MCP251XFD_ClearInterruptEvents(pComp, MCP251XFD_INT_CLEARABLE_FLAGS_MASK); // Clear all clearable interrupts
  if (Error != ERR_OK) return Error;                                                 // If there is an error while calling MCP251XFD_ReadSFR16() then return the error
  return MCP251XFD_WriteSFR16(pComp, RegMCP251XFD_CiINT_CONFIG, interruptsFlags);    // Write new interrupt configuration (The 2 MSB bytes)
}



//=============================================================================
// Get interrupt events of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetInterruptEvents(MCP251XFD *pComp, setMCP251XFD_InterruptEvents* interruptsFlags)
{
  return MCP251XFD_ReadSFR16(pComp, RegMCP251XFD_CiINT_FLAG, (uint16_t*)interruptsFlags); // Read interrupt flags (The 2 LSB bytes)
}



//=============================================================================
// Get the current interrupt event of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetCurrentInterruptEvent(MCP251XFD *pComp, eMCP251XFD_InterruptFlagCode* currentEvent)
{
  return MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiVEC_ICODE, (uint8_t*)currentEvent); // Read current interrupt event (First byte only)
}



//=============================================================================
// Clear interrupt events of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ClearInterruptEvents(MCP251XFD *pComp, setMCP251XFD_InterruptEvents interruptsFlags)
{
  if ((interruptsFlags & MCP251XFD_INT_CLEARABLE_FLAGS_MASK) == 0) return ERR_OK;
  uint16_t Interrupts = 0;
  eERRORRESULT Error = MCP251XFD_ReadSFR16(pComp, RegMCP251XFD_CiINT_FLAG, &Interrupts); // Read interrupt flags (The 2 LSB bytes)
  if (Error != ERR_OK) return Error;                                                     // If there is an error while calling MCP251XFD_ReadSFR16() then return the error
  Interrupts &= ~((uint16_t)interruptsFlags);                                            // Clear selected flags
  return MCP251XFD_WriteSFR16(pComp, RegMCP251XFD_CiINT_FLAG, Interrupts);               // Write flags cleared (The 2 LSB bytes)
}



//=============================================================================
// Get current receive FIFO name and status that generate an interrupt (if any)
//=============================================================================
eERRORRESULT MCP251XFD_GetCurrentReceiveFIFONameAndStatusInterrupt(MCP251XFD *pComp, eMCP251XFD_FIFO *name, setMCP251XFD_FIFOstatus *flags)
{
#ifdef CHECK_NULL_PARAM
  if (name == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;

  //--- Get current interrupt code ---
  *name = MCP251XFD_NO_FIFO;
  uint8_t RxCode = 0;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiVEC_RXCODE, &RxCode);                              // Read Interrupt code register (Last byte only)
  if (Error != ERR_OK) return Error;                                                                  // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  RxCode = MCP251XFD_CAN_CiVEC8_RXCODE_GET(RxCode);                                                   // Extract RXCODE
  if ( RxCode < MCP251XFD_FIFO1 ) return ERR__UNKNOWN_ELEMENT;                                        // FIFO0 is reserved so not possible
  if ((RxCode > MCP251XFD_FIFO31) && (RxCode != MCP251XFD_NO_INTERRUPT)) return ERR__UNKNOWN_ELEMENT; // Only FIFO1 to 31 and no Interrupt code possible so the rest is not possible
  if (RxCode == MCP251XFD_NO_INTERRUPT) return ERR_OK;                                                // No interrupt? Good
  *name = (eMCP251XFD_FIFO)RxCode;                                                                    // Save the current FIFO name interrupt

  //--- Get status flags of the FIFO ---
  if ((*name != MCP251XFD_NO_FIFO) && (flags != NULL))
    return MCP251XFD_GetFIFOStatus(pComp, *name, flags);                                              // Get status flags of the FIFO
  return ERR_OK;
}



//=============================================================================
// Get current transmit FIFO name and status that generate an interrupt (if any)
//=============================================================================
eERRORRESULT MCP251XFD_GetCurrentTransmitFIFONameAndStatusInterrupt(MCP251XFD *pComp, eMCP251XFD_FIFO *name, setMCP251XFD_FIFOstatus *flags)
{
#ifdef CHECK_NULL_PARAM
  if (name == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;

  //--- Get current interrupt code ---
  *name = MCP251XFD_NO_FIFO;
  uint8_t TxCode = 0;
  Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiVEC_TXCODE, &TxCode);                              // Read Interrupt code register (Third byte only)
  if (Error != ERR_OK) return Error;                                                                  // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  TxCode = MCP251XFD_CAN_CiVEC8_TXCODE_GET(TxCode);                                                   // Extract TXCODE
  if ((TxCode > MCP251XFD_FIFO31) && (TxCode != MCP251XFD_NO_INTERRUPT)) return ERR__UNKNOWN_ELEMENT; // Only FIFO0 (TXQ) to 31 and no Interrupt code possible so the rest is not possible
  if (TxCode == MCP251XFD_NO_INTERRUPT) return ERR_OK;                                                // No interrupt? Good
  *name = (eMCP251XFD_FIFO)TxCode;                                                                    // Save the current FIFO name interrupt

  //--- Get status flags of the FIFO ---
  if ((*name != MCP251XFD_NO_FIFO) && (flags != NULL))
    return MCP251XFD_GetFIFOStatus(pComp, *name, flags);                                              // Get status flags of the FIFO
  return ERR_OK;
}



//=============================================================================
// Clear selected FIFO events of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ClearFIFOEvents(MCP251XFD *pComp, eMCP251XFD_FIFO name, uint8_t events)
{
  if (name >= MCP251XFD_FIFO_COUNT) return ERR__PARAMETER_ERROR;
  if ((events & MCP251XFD_FIFO_CLEARABLE_STATUS_FLAGS) == 0) return ERR_OK;                               // Threat only the clearable ones
  eERRORRESULT Error;

  uint16_t Address = RegMCP251XFD_CiFIFOSTAm_FLAGS + (MCP251XFD_FIFO_REG_SIZE * ((uint16_t)name - 1u));   // Select the address of the FIFO
  if (name == MCP251XFD_TEF) Address = RegMCP251XFD_CiTEFSTA_FLAGS;                                       // If it's the TEF then select its address
  if (name == MCP251XFD_TXQ) Address = RegMCP251XFD_CiTXQSTA_FLAGS;                                       // If it's the TXQ then select its address

  //--- Get FIFO status ---
  uint8_t Status;
  Error = MCP251XFD_ReadSFR8(pComp, Address, &Status);                                                    // Read FIFO status flags
  if (Error != ERR_OK) return Error;                                                                      // If there is an error while calling MCP251XFD_ReadSFR8() then return the error

  //--- Clear selected flags ---
  if (((events & MCP251XFD_TEF_FIFO_OVERFLOW         ) > 0)
   || ((events & MCP251XFD_RX_FIFO_OVERFLOW          ) > 0)) Status &= ~MCP251XFD_CAN_CiFIFOSTAm8_RXOVIF; // Receive & TEF: Clear Overflow event
  if (((events & MCP251XFD_TXQ_ATTEMPTS_EXHAUSTED    ) > 0)
   || ((events & MCP251XFD_TX_FIFO_ATTEMPTS_EXHAUSTED) > 0)) Status &= ~MCP251XFD_CAN_CiFIFOSTAm8_TXATIF; // Transmit & TXQ: Clear Attempts Exhausted event
  if (((events & MCP251XFD_TXQ_BUS_ERROR             ) > 0)
   || ((events & MCP251XFD_TX_FIFO_BUS_ERROR         ) > 0)) Status &= ~MCP251XFD_CAN_CiFIFOSTAm8_TXERR;  // Transmit & TXQ: Clear Error Detected During Transmission event
  if (((events & MCP251XFD_TXQ_ARBITRATION_LOST      ) > 0)
   || ((events & MCP251XFD_TX_FIFO_ARBITRATION_LOST  ) > 0)) Status &= ~MCP251XFD_CAN_CiFIFOSTAm8_TXLARB; // Transmit & TXQ: Clear Message Lost Arbitration event
  if (((events & MCP251XFD_TXQ_STATUS_MASK           ) > 0)
   || ((events & MCP251XFD_TX_FIFO_STATUS_MASK       ) > 0)) Status &= ~MCP251XFD_CAN_CiFIFOSTAm8_TXABT;  // Transmit & TXQ: Clear Message Aborted event
  return MCP251XFD_WriteSFR8(pComp, Address, Status);                                                     // Write new FIFO status flags
}



//=============================================================================
// Get the receive interrupt pending status of all FIFO
//=============================================================================
eERRORRESULT MCP251XFD_GetReceiveInterruptStatusOfAllFIFO(MCP251XFD *pComp, setMCP251XFD_InterruptOnFIFO* interruptPending, setMCP251XFD_InterruptOnFIFO* overflowStatus)
{
  eERRORRESULT Error;
  if (interruptPending != NULL)
  {
    Error = MCP251XFD_ReadSFR32(pComp, RegMCP251XFD_CiRXIF, (uint32_t*)interruptPending); // Read the Receive interrupt status register
    if (Error != ERR_OK) return Error;                                                    // If there is an error while calling MCP251XFD_ReadSFR32() then return the error
  }
  if (overflowStatus != NULL)
  {
    Error = MCP251XFD_ReadSFR32(pComp, RegMCP251XFD_CiRXOVIF, (uint32_t*)overflowStatus); // Read the Receive overflow interrupt status register
    if (Error != ERR_OK) return Error;                                                    // If there is an error while calling MCP251XFD_ReadSFR32() then return the error
  }
  return ERR_OK;
}



//=============================================================================
// Get the transmit interrupt pending status of all FIFO
//=============================================================================
eERRORRESULT MCP251XFD_GetTransmitInterruptStatusOfAllFIFO(MCP251XFD *pComp, setMCP251XFD_InterruptOnFIFO* interruptPending, setMCP251XFD_InterruptOnFIFO* attemptStatus)
{
  eERRORRESULT Error;
  if (interruptPending != NULL)
  {
    Error = MCP251XFD_ReadSFR32(pComp, RegMCP251XFD_CiTXIF, (uint32_t*)interruptPending); // Read the Receive interrupt status register
    if (Error != ERR_OK) return Error;                                                    // If there is an error while calling MCP251XFD_ReadSFR32() then return the error
  }
  if (attemptStatus != NULL)
  {
    Error = MCP251XFD_ReadSFR32(pComp, RegMCP251XFD_CiTXATIF, (uint32_t*)attemptStatus);  // Read the Receive overflow interrupt status register
    if (Error != ERR_OK) return Error;                                                    // If there is an error while calling MCP251XFD_ReadSFR32() then return the error
  }
  return ERR_OK;
}





//**********************************************************************************************************************************************************
//=============================================================================
// Get transmit/receive error count and status of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetTransmitReceiveErrorCountAndStatus(MCP251XFD *pComp, uint8_t* transmitErrorCount, uint8_t* receiveErrorCount, eMCP251XFD_TXRXErrorStatus* status)
{
  eERRORRESULT Error;
  if (transmitErrorCount != NULL)
  {
    Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiTREC_TEC, transmitErrorCount);  // Read the transmit error counter
    if (Error != ERR_OK) return Error;                                               // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  }
  if (receiveErrorCount != NULL)
  {
    Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiTREC_REC, receiveErrorCount);   // Read the receive error counter
    if (Error != ERR_OK) return Error;                                               // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  }
  if (status != NULL)
  {
    Error = MCP251XFD_ReadSFR8(pComp, RegMCP251XFD_CiTREC_STATUS, (uint8_t*)status); // Read the transmit/receive error status
    if (Error != ERR_OK) return Error;                                               // If there is an error while calling MCP251XFD_ReadSFR8() then return the error
  }

  return ERR_OK;
}



//=============================================================================
// Get Bus diagnostic of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_GetBusDiagnostic(MCP251XFD *pComp, MCP251XFD_CiBDIAG0_Register* busDiagnostic0, MCP251XFD_CiBDIAG1_Register* busDiagnostic1)
{
  eERRORRESULT Error;
  if (busDiagnostic0 != NULL)
  {
    Error = MCP251XFD_ReadSFR32(pComp, RegMCP251XFD_CiBDIAG0, &busDiagnostic0->CiBDIAG0); // Read the transmit error counter
    if (Error != ERR_OK) return Error;                                                    // If there is an error while calling MCP251XFD_ReadSFR32() then return the error
  }
  if (busDiagnostic1 != NULL)
  {
    Error = MCP251XFD_ReadSFR32(pComp, RegMCP251XFD_CiBDIAG1, &busDiagnostic1->CiBDIAG1); // Read the receive error counter
    if (Error != ERR_OK) return Error;                                                    // If there is an error while calling MCP251XFD_ReadSFR32() then return the error
  }
  return ERR_OK;
}



//=============================================================================
// Clear Bus diagnostic of the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ClearBusDiagnostic(MCP251XFD *pComp, bool clearBusDiagnostic0, bool clearBusDiagnostic1)
{
  eERRORRESULT Error;
  if (clearBusDiagnostic0)
  {
    Error = MCP251XFD_WriteSFR32(pComp, RegMCP251XFD_CiBDIAG0, 0x00000000); // Read the transmit error counter
    if (Error != ERR_OK) return Error;                                      // If there is an error while calling MCP251XFD_WriteSFR32() then return the error
  }
  if (clearBusDiagnostic1)
  {
    Error = MCP251XFD_WriteSFR32(pComp, RegMCP251XFD_CiBDIAG1, 0x00000000); // Read the receive error counter
    if (Error != ERR_OK) return Error;                                      // If there is an error while calling MCP251XFD_WriteSFR32() then return the error
  }
  return ERR_OK;
}





//**********************************************************************************************************************************************************
//=============================================================================
// Reset the MCP251XFD device
//=============================================================================
eERRORRESULT MCP251XFD_ResetDevice(MCP251XFD *pComp)
{
#ifdef CHECK_NULL_PARAM
  if (pComp == NULL) return ERR__PARAMETER_ERROR;
  if (pComp->fnSPI_Transfer == NULL) return ERR__PARAMETER_ERROR;
#endif
  eERRORRESULT Error;
  uint8_t Buffer[2];

  //--- Do DRIVER_SAFE_RESET flag implementation before reset ---------------
  if ((pComp->DriverConfig & MCP251XFD_DRIVER_SAFE_RESET) > 0)
  {
#ifdef CHECK_NULL_PARAM
    if (pComp->fnSPI_Init == NULL) return ERR__PARAMETER_ERROR;
    if (pComp->fnGetCurrentms == NULL) return ERR__PARAMETER_ERROR;
#endif

    // Set SPI speed to 1MHz
    Error = pComp->fnSPI_Init(pComp->InterfaceDevice, pComp->SPI_ChipSelect, MCP251XFD_DRIVER_SAFE_RESET_SPI_CLK); // Set the SPI speed clock to 1MHz (safe clock speed)
    if (Error != ERR_OK) return Error;                                                                             // If there is an error while reading the register then return the error

    // Request configuration mode to avoid transmission error without aborting them
    Error = MCP251XFD_RequestOperationMode(pComp, MCP251XFD_CONFIGURATION_MODE, true);                             // Request a configuration mode and wait until device is this mode
    if (Error != ERR_OK) return Error;                                                                             // If there is an error while calling MCP251XFD_RequestOperationMode() then return the error
  }


  //-- Perform Reset --------------------------------------------------------
  Buffer[0] = (MCP251XFD_SPI_INSTRUCTION_RESET << 4);
  Buffer[1] = 0;
  return pComp->fnSPI_Transfer(pComp->InterfaceDevice, pComp->SPI_ChipSelect, &Buffer[0], NULL, 2/*bytes only*/); // Perform reset
}





//=============================================================================
// Test all RAM address of the MCP251XFD for the driver flag DRIVER_INIT_CHECK_RAM
//=============================================================================
eERRORRESULT __MCP251XFD_TestRAM(MCP251XFD *pComp)
{
  eERRORRESULT Error;
  uint32_t Result = 0;
  for (uint16_t Address = MCP251XFD_RAM_ADDR; Address < (MCP251XFD_RAM_ADDR + MCP251XFD_RAM_SIZE); Address += 4)
  {
    Error = MCP251XFD_WriteRAM32(pComp, Address, 0x55555555); // Write 0x55555555 at address
    if (Error != ERR_OK) return Error;                        // If there is an error while writing the RAM address then return the error
    Error = MCP251XFD_ReadRAM32(pComp, Address, &Result);     // Read again the data
    if (Error != ERR_OK) return Error;                        // If there is an error while reading the RAM address then return the error
    if (Result != 0x55555555) return ERR__RAM_TEST_FAIL;      // If data read is not 0x55555555 then return an error

    Error = MCP251XFD_WriteRAM32(pComp, Address, 0xAAAAAAAA); // Write 0xAAAAAAAA at address
    if (Error != ERR_OK) return Error;                        // If there is an error while writing the RAM address then return the error
    Error = MCP251XFD_ReadRAM32(pComp, Address, &Result);     // Read again the data
    if (Error != ERR_OK) return Error;                        // If there is an error while reading the RAM address then return the error
    if (Result != 0xAAAAAAAA) return ERR__RAM_TEST_FAIL;      // If data read is not 0xAAAAAAAA then return an error
  }
  return ERR_OK;
}





//**********************************************************************************************************************************************************
//=============================================================================
// Message ID to Object Message Identifier
//=============================================================================
uint32_t MCP251XFD_MessageIDtoObjectMessageIdentifier(uint32_t messageID, bool extended, bool UseSID11)
{
  uint32_t ResultOMI = 0; // Initialize message ID to 0

  //--- Fill message ID (T0 or R0 or TE0) ---
  if (extended) // Message use extended ID?
  {
    ResultOMI = ((messageID >> MCP251XFD_EID_Size) & MCP251XFD_SID_Mask) | ((messageID & MCP251XFD_EID_Mask) << MCP251XFD_SID_Size);
    if (UseSID11) ResultOMI |= (messageID & (1 << (MCP251XFD_EID_Size + MCP251XFD_SID_Size)));
  }
  else
  {
    ResultOMI = messageID & MCP251XFD_SID_Mask;
    if (UseSID11) ResultOMI |= ((messageID & (1 << MCP251XFD_SID_Size)) << (29 - MCP251XFD_SID_Size));
  }
  return ResultOMI;
}



//=============================================================================
// Object Message Identifier to Message ID
//=============================================================================
uint32_t MCP251XFD_ObjectMessageIdentifierToMessageID(uint32_t objectMessageID, bool extended, bool UseSID11)
{
  uint32_t ResultID = 0; // Initialize message ID to 0

  //--- Extract object message ID (T0 or R0 or TE0) ---
  if (extended) // Message use extended ID?
  {
    ResultID = ((objectMessageID >> MCP251XFD_SID_Size) & MCP251XFD_EID_Mask) | ((objectMessageID & MCP251XFD_SID_Mask) << MCP251XFD_EID_Size);
    if (UseSID11) ResultID |= (objectMessageID & (1 << (MCP251XFD_EID_Size + MCP251XFD_SID_Size)));
  }
  else
  {
    ResultID = objectMessageID & MCP251XFD_SID_Mask;
    if (UseSID11) ResultID |= ((objectMessageID & (1 << (MCP251XFD_EID_Size + MCP251XFD_SID_Size))) >> (29 - MCP251XFD_SID_Size));
  }
  return ResultID;
}





//**********************************************************************************************************************************************************
//=============================================================================
// Payload to Byte Count
//=============================================================================
uint8_t MCP251XFD_PayloadToByte(eMCP251XFD_PayloadSize payload)
{
//  const uint8_t PAYLOAD_TO_VALUE[PAYLOAD_COUNT] = {8, 12, 16, 20, 24, 32, 48, 64};
//  if (payload < PAYLOAD_COUNT) return PAYLOAD_TO_VALUE[payload];
//  return 0;
  switch (payload)
  {
    case MCP251XFD_PAYLOAD_8BYTE : return  8; // Payload  8 data bytes
    case MCP251XFD_PAYLOAD_12BYTE: return 12; // Payload 12 data bytes
    case MCP251XFD_PAYLOAD_16BYTE: return 16; // Payload 16 data bytes
    case MCP251XFD_PAYLOAD_20BYTE: return 20; // Payload 20 data bytes
    case MCP251XFD_PAYLOAD_24BYTE: return 24; // Payload 24 data bytes
    case MCP251XFD_PAYLOAD_32BYTE: return 32; // Payload 32 data bytes
    case MCP251XFD_PAYLOAD_48BYTE: return 48; // Payload 48 data bytes
    case MCP251XFD_PAYLOAD_64BYTE: return 64; // Payload 64 data bytes
    default: return 0;
  }
  return 0;
}



//=============================================================================
// Data Length Content to Byte Count
//=============================================================================
uint8_t MCP251XFD_DLCToByte(eMCP251XFD_DataLength dlc, bool isCANFD)
{
  const uint8_t CAN20_DLC_TO_VALUE[MCP251XFD_DLC_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7, 8,  8,  8,  8,  8,  8,  8,  8};
  const uint8_t CANFD_DLC_TO_VALUE[MCP251XFD_DLC_COUNT] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};
  if (dlc >= MCP251XFD_DLC_COUNT) return 0;
  if (isCANFD) return CANFD_DLC_TO_VALUE[dlc];
  return CAN20_DLC_TO_VALUE[dlc];
}





//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------