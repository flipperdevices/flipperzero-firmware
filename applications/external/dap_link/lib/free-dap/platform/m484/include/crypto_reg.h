/**************************************************************************//**
 * @file     crypto_reg.h
 * @version  V1.00
 * @brief    CRYPTO register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __CRYPTO_REG_H__
#define __CRYPTO_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup CRPT Cryptographic Accelerator(CRPT)
    Memory Mapped Structure for Cryptographic Accelerator
@{ */

typedef struct
{

    /**
     * @var CRPT_T::INTEN
     * Offset: 0x00  Crypto Interrupt Enable Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |AESIEN    |AES Interrupt Enable Control
     * |        |          |0 = AES interrupt Disabled.
     * |        |          |1 = AES interrupt Enabled.
     * |        |          |In DMA mode, an interrupt will be triggered when amount of data set in AES_DMA_CNT is fed into the AES engine.
     * |        |          |In Non-DMA mode, an interrupt will be triggered when the AES engine finishes the operation.
     * |[1]     |AESEIEN   |AES Error Flag Enable Control
     * |        |          |0 = AES error interrupt flag Disabled.
     * |        |          |1 = AES error interrupt flag Enabled.
     * |[8]     |TDESIEN   |TDES/DES Interrupt Enable Control
     * |        |          |0 = TDES/DES interrupt Disabled.
     * |        |          |1 = TDES/DES interrupt Enabled.
     * |        |          |In DMA mode, an interrupt will be triggered when amount of data set in TDES_DMA_CNT is fed into the TDES engine.
     * |        |          |In Non-DMA mode, an interrupt will be triggered when the TDES engine finishes the operation.
     * |[9]     |TDESEIEN  |TDES/DES Error Flag Enable Control
     * |        |          |0 = TDES/DES error interrupt flag Disabled.
     * |        |          |1 = TDES/DES error interrupt flag Enabled.
     * |[16]    |PRNGIEN   |PRNG Interrupt Enable Control
     * |        |          |0 = PRNG interrupt Disabled.
     * |        |          |1 = PRNG interrupt Enabled.
     * |[22]    |ECCIEN    |ECC Interrupt Enable Control
     * |        |          |0 = ECC interrupt Disabled.
     * |        |          |1 = ECC interrupt Enabled.
     * |        |          |In DMA mode, an interrupt will be triggered when amount of data set in ECC_DMA_CNT is fed into the ECC engine.
     * |        |          |In Non-DMA mode, an interrupt will be triggered when the ECC engine finishes the operation.
     * |[23]    |ECCEIEN   |ECC Error Interrupt Enable Control
     * |        |          |0 = ECC error interrupt flag Disabled.
     * |        |          |1 = ECC error interrupt flag Enabled.
     * |[24]    |HMACIEN   |SHA/HMAC Interrupt Enable Control
     * |        |          |0 = SHA/HMAC interrupt Disabled.
     * |        |          |1 = SHA/HMAC interrupt Enabled.
     * |        |          |In DMA mode, an interrupt will be triggered when amount of data set in SHA _DMA_CNT is fed into the SHA/HMAC engine
     * |        |          |In Non-DMA mode, an interrupt will be triggered when the SHA/HMAC engine finishes the operation.
     * |[25]    |HMACEIEN  |SHA/HMAC Error Interrupt Enable Control
     * |        |          |0 = SHA/HMAC error interrupt flag Disabled.
     * |        |          |1 = SHA/HMAC error interrupt flag Enabled.
     * @var CRPT_T::INTSTS
     * Offset: 0x04  Crypto Interrupt Flag
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |AESIF     |AES Finish Interrupt Flag
     * |        |          |This bit is cleared by writing 1, and it has no effect by writing 0.
     * |        |          |0 = No AES interrupt.
     * |        |          |= AES encryption/decryption done interrupt.
     * |[1]     |AESEIF    |AES Error Flag
     * |        |          |This bit is cleared by writing 1, and it has no effect by writing 0.
     * |        |          |0 = No AES error.
     * |        |          |1 = AES encryption/decryption done interrupt.
     * |[8]     |TDESIF    |TDES/DES Finish Interrupt Flag
     * |        |          |This bit is cleared by writing 1, and it has no effect by writing 0.
     * |        |          |0 = No TDES/DES interrupt.
     * |        |          |1 = TDES/DES encryption/decryption done interrupt.
     * |[9]     |TDESEIF   |TDES/DES Error Flag
     * |        |          |This bit includes the operating and setting error
     * |        |          |The detailed flag is shown in the CRPT_TDES_STS register
     * |        |          |This includes operating and setting error.
     * |        |          |This bit is cleared by writing 1, and it has no effect by writing 0.
     * |        |          |0 = No TDES/DES error.
     * |        |          |1 = TDES/DES encryption/decryption error interrupt.
     * |[16]    |PRNGIF    |PRNG Finish Interrupt Flag
     * |        |          |This bit is cleared by writing 1, and it has no effect by writing 0.
     * |        |          |0 = No PRNG interrupt.
     * |        |          |1 = PRNG key generation done interrupt.
     * |[22]    |ECCIF     |ECC Finish Interrupt Flag
     * |        |          |This bit is cleared by writing 1, and it has no effect by writing 0.
     * |        |          |0 = No ECC interrupt.
     * |        |          |1 = ECC operation done interrupt.
     * |[23]    |ECCEIF    |ECC Error Flag
     * |        |          |This register includes operating and setting error. The detail flag is shown in CRPT_ECC_STS register.
     * |        |          |This bit is cleared by writing 1, and it has no effect by writing 0.
     * |        |          |0 = No ECC error.
     * |        |          |1 = ECC error interrupt.
     * |[24]    |HMACIF    |SHA/HMAC Finish Interrupt Flag
     * |        |          |This bit is cleared by writing 1, and it has no effect by writing 0.
     * |        |          |0 = No SHA/HMAC interrupt.
     * |        |          |1 = SHA/HMAC operation done interrupt.
     * |[25]    |HMACEIF   |SHA/HMAC Error Flag
     * |        |          |This register includes operating and setting error. The detail flag is shown in CRPT_HMAC_STS register.
     * |        |          |This bit is cleared by writing 1, and it has no effect by writing 0.
     * |        |          |0 = No SHA/HMAC error.
     * |        |          |1 = SHA/HMAC error interrupt.
     * @var CRPT_T::PRNG_CTL
     * Offset: 0x08  PRNG Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |START     |Start PRNG Engine
     * |        |          |0 = Stop PRNG engine.
     * |        |          |1 = Generate new key and store the new key to register CRPT_PRNG_KEYx , which will be cleared when the new key is generated.
     * |[1]     |SEEDRLD   |Reload New Seed for PRNG Engine
     * |        |          |0 = Generating key based on the current seed.
     * |        |          |1 = Reload new seed.
     * |[3:2]   |KEYSZ     |PRNG Generate Key Size
     * |        |          |00 = 64 bits.
     * |        |          |01 = 128 bits.
     * |        |          |10 = 192 bits.
     * |        |          |11 = 256 bits.
     * |[8]     |BUSY      |PRNG Busy (Read Only)
     * |        |          |0 = PRNG engine is idle.
     * |        |          |1 = Indicate that the PRNG engine is generating CRPT_PRNG_KEYx.
     * @var CRPT_T::PRNG_SEED
     * Offset: 0x0C  Seed for PRNG
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SEED      |Seed for PRNG (Write Only)
     * |        |          |The bits store the seed for PRNG engine.
     * @var CRPT_T::PRNG_KEY[8]
     * Offset: 0x10 ~ 0x2C  PRNG Generated Key0 ~ Key7
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |Store PRNG Generated Key (Read Only)
     * |        |          |The bits store the key that is generated by PRNG.
     * @var CRPT_T::AES_FDBCK[4]
     * Offset: 0x50 ~ 0x5C  AES Engine Output Feedback Data after Cryptographic Operation
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |FDBCK     |AES Feedback Information
     * |        |          |The feedback value is 128 bits in size.
     * |        |          |The AES engine uses the data from CRPT_AES_FDBCKx as the data inputted to CRPT_AESn_IVx for the next block in DMA cascade mode.
     * |        |          |The AES engine outputs feedback information for IV in the next block's operation
     * |        |          |Software can use this feedback information to implement more than four DMA channels
     * |        |          |Software can store that feedback value temporarily
     * |        |          |After switching back, fill the stored feedback value to this register in the same channel operation, and then continue the operation with the original setting.
     * @var CRPT_T::TDES_FDBCKH
     * Offset: 0x60  TDES/DES Engine Output Feedback High Word Data after Cryptographic Operation
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |FDBCK     |TDES/DES Feedback
     * |        |          |The feedback value is 64 bits in size.
     * |        |          |The TDES/DES engine uses the data from {CRPT_TDES_FDBCKH, CRPT_TDES_FDBCKL} as the data inputted to {CRPT_TDESn_IVH, CRPT_TDESn_IVL} for the next block in DMA cascade mode
     * |        |          |The feedback register is for CBC, CFB, and OFB mode.
     * |        |          |TDES/DES engine outputs feedback information for IV in the next block's operation
     * |        |          |Software can use this feedback information to implement more than four DMA channels
     * |        |          |Software can store that feedback value temporarily
     * |        |          |After switching back, fill the stored feedback value to this register in the same channel operation
     * |        |          |Then can continue the operation with the original setting.
     * @var CRPT_T::TDES_FDBCKL
     * Offset: 0x64  TDES/DES Engine Output Feedback Low Word Data after Cryptographic Operation
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |FDBCK     |TDES/DES Feedback
     * |        |          |The feedback value is 64 bits in size.
     * |        |          |The TDES/DES engine uses the data from {CRPT_TDES_FDBCKH, CRPT_TDES_FDBCKL} as the data inputted to {CRPT_TDESn_IVH, CRPT_TDESn_IVL} for the next block in DMA cascade mode
     * |        |          |The feedback register is for CBC, CFB, and OFB mode.
     * |        |          |TDES/DES engine outputs feedback information for IV in the next block's operation
     * |        |          |Software can use this feedback information to implement more than four DMA channels
     * |        |          |Software can store that feedback value temporarily
     * |        |          |After switching back, fill the stored feedback value to this register in the same channel operation
     * |        |          |Then can continue the operation with the original setting.
     * @var CRPT_T::AES_CTL
     * Offset: 0x100  AES Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |START     |AES Engine Start
     * |        |          |0 = No effect.
     * |        |          |1 = Start AES engine. BUSY flag will be set.
     * |        |          |Note: This bit is always 0 when it's read back.
     * |[1]     |STOP      |AES Engine Stop
     * |        |          |0 = No effect.
     * |        |          |1 = Stop AES engine.
     * |        |          |Note: This bit is always 0 when it's read back.
     * |[3:2]   |KEYSZ     |AES Key Size
     * |        |          |This bit defines three different key size for AES operation.
     * |        |          |2'b00 = 128 bits key.
     * |        |          |2'b01 = 192 bits key.
     * |        |          |2'b10 = 256 bits key.
     * |        |          |2'b11 = Reserved.
     * |        |          |If the AES accelerator is operating and the corresponding flag BUSY is 1, updating this register has no effect.
     * |[5]     |DMALAST   |AES Last Block
     * |        |          |In DMA mode, this bit must be set as beginning the last DMA cascade round.
     * |        |          |In Non-DMA mode, this bit must be set when feeding in the last block of data in ECB, CBC, CTR, OFB, and CFB mode, and feeding in the (last-1) block of data at CBC-CS1, CBC-CS2, and CBC-CS3 mode.
     * |        |          |This bit is always 0 when it's read back. Must be written again once START is triggered.
     * |[6]     |DMACSCAD  |AES Engine DMA with Cascade Mode
     * |        |          |0 = DMA cascade function Disabled.
     * |        |          |1 = In DMA cascade mode, software can update DMA source address register, destination address register, and byte count register during a cascade operation, without finishing the accelerator operation.
     * |[7]     |DMAEN     |AES Engine DMA Enable Control
     * |        |          |0 = AES DMA engine Disabled.
     * |        |          |The AES engine operates in Non-DMA mode, and gets data from the port CRPT_AES_DATIN.
     * |        |          |1 = AES_DMA engine Enabled.
     * |        |          |The AES engine operates in DMA mode, and data movement from/to the engine is done by DMA logic.
     * |[15:8]  |OPMODE    |AES Engine Operation Modes
     * |        |          |0x00 = ECB (Electronic Codebook Mode)  0x01 = CBC (Cipher Block Chaining Mode).
     * |        |          |0x02 = CFB (Cipher Feedback Mode).
     * |        |          |0x03 = OFB (Output Feedback Mode).
     * |        |          |0x04 = CTR (Counter Mode).
     * |        |          |0x10 = CBC-CS1 (CBC Ciphertext-Stealing 1 Mode).
     * |        |          |0x11 = CBC-CS2 (CBC Ciphertext-Stealing 2 Mode).
     * |        |          |0x12 = CBC-CS3 (CBC Ciphertext-Stealing 3 Mode).
     * |[16]    |ENCRPT    |AES Encryption/Decryption
     * |        |          |0 = AES engine executes decryption operation.
     * |        |          |1 = AES engine executes encryption operation.
     * |[22]    |OUTSWAP   |AES Engine Output Data Swap
     * |        |          |0 = Keep the original order.
     * |        |          |1 = The order that CPU outputs data from the accelerator will be changed from {byte3, byte2, byte1, byte0} to {byte0, byte1, byte2, byte3}.
     * |[23]    |INSWAP    |AES Engine Input Data Swap
     * |        |          |0 = Keep the original order.
     * |        |          |1 = The order that CPU feeds data to the accelerator will be changed from {byte3, byte2, byte1, byte0} to {byte0, byte1, byte2, byte3}.
     * |[25:24] |CHANNEL   |AES Engine Working Channel
     * |        |          |00 = Current control register setting is for channel 0.
     * |        |          |01 = Current control register setting is for channel 1.
     * |        |          |10 = Current control register setting is for channel 2.
     * |        |          |11 = Current control register setting is for channel 3.
     * |[30:26] |KEYUNPRT  |Unprotect Key
     * |        |          |Writing 0 to CRPT_AES_CTL[31] and "10110" to CRPT_AES_CTL[30:26] is to unprotect the AES key.
     * |        |          |The KEYUNPRT can be read and written
     * |        |          |When it is written as the AES engine is operating, BUSY flag is 1, there would be no effect on KEYUNPRT.
     * |[31]    |KEYPRT    |Protect Key
     * |        |          |Read as a flag to reflect KEYPRT.
     * |        |          |0 = No effect.
     * |        |          |1 = Protect the content of the AES key from reading
     * |        |          |The return value for reading CRPT_AESn_KEYx is not the content of the registers CRPT_AESn_KEYx
     * |        |          |Once it is set, it can be cleared by asserting KEYUNPRT
     * |        |          |And the key content would be cleared as well.
     * @var CRPT_T::AES_STS
     * Offset: 0x104  AES Engine Flag
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BUSY      |AES Engine Busy
     * |        |          |0 = The AES engine is idle or finished.
     * |        |          |1 = The AES engine is under processing.
     * |[8]     |INBUFEMPTY|AES Input Buffer Empty
     * |        |          |0 = There are some data in input buffer waiting for the AES engine to process.
     * |        |          |1 = AES input buffer is empty
     * |        |          |Software needs to feed data to the AES engine
     * |        |          |Otherwise, the AES engine will be pending to wait for input data.
     * |[9]     |INBUFFULL |AES Input Buffer Full Flag
     * |        |          |0 = AES input buffer is not full. Software can feed the data into the AES engine.
     * |        |          |1 = AES input buffer is full
     * |        |          |Software cannot feed data to the AES engine
     * |        |          |Otherwise, the flag INBUFERR will be set to 1.
     * |[10]    |INBUFERR  |AES Input Buffer Error Flag
     * |        |          |0 = No error.
     * |        |          |1 = Error happens during feeding data to the AES engine.
     * |[12]    |CNTERR    |CRPT_AESn_CNT Setting Error
     * |        |          |0 = No error in CRPT_AESn_CNT setting.
     * |        |          |1 = CRPT_AESn_CNT is not a multiply of 16 in ECB, CBC, CFB, OFB, and CTR mode.
     * |[16]    |OUTBUFEMPTY|AES Out Buffer Empty
     * |        |          |0 = AES output buffer is not empty. There are some valid data kept in output buffer.
     * |        |          |1 = AES output buffer is empty
     * |        |          |Software cannot get data from CRPT_AES_DATOUT
     * |        |          |Otherwise, the flag OUTBUFERR will be set to 1 since the output buffer is empty.
     * |[17]    |OUTBUFFULL|AES Out Buffer Full Flag
     * |        |          |0 = AES output buffer is not full.
     * |        |          |1 = AES output buffer is full, and software needs to get data from CRPT_AES_DATOUT
     * |        |          |Otherwise, the AES engine will be pending since the output buffer is full.
     * |[18]    |OUTBUFERR |AES Out Buffer Error Flag
     * |        |          |0 = No error.
     * |        |          |1 = Error happens during getting the result from AES engine.
     * |[20]    |BUSERR    |AES DMA Access Bus Error Flag
     * |        |          |0 = No error.
     * |        |          |1 = Bus error will stop DMA operation and AES engine.
     * @var CRPT_T::AES_DATIN
     * Offset: 0x108  AES Engine Data Input Port Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DATIN     |AES Engine Input Port
     * |        |          |CPU feeds data to AES engine through this port by checking CRPT_AES_STS. Feed data as INBUFFULL is 0.
     * @var CRPT_T::AES_DATOUT
     * Offset: 0x10C  AES Engine Data Output Port Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DATOUT    |AES Engine Output Port
     * |        |          |CPU gets results from the AES engine through this port by checking CRPT_AES_STS
     * |        |          |Get data as OUTBUFEMPTY is 0.
     * @var CRPT_T::AES0_KEY[8]
     * Offset: 0x110 ~ 0x12C  AES Key Word 0 ~ 7 Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |CRPT_AESn_KEYx
     * |        |          |The KEY keeps the security key for AES operation.
     * |        |          |n = 0, 1..3.
     * |        |          |x = 0, 1..7.
     * |        |          |The security key for AES accelerator can be 128, 192, or 256 bits and four, six, or eight 32-bit registers are to store each security key
     * |        |          |{CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 128-bit security key for AES operation
     * |        |          |{CRPT_AESn_KEY5, CRPT_AESn_KEY4, CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 192-bit security key for AES operation
     * |        |          |{CRPT_AESn_KEY7, CRPT_AESn_KEY6, CRPT_AESn_KEY5, CRPT_AESn_KEY4, CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 256-bit security key for AES operation.
     * @var CRPT_T::AES0_IV[4]
     * Offset: 0x130 ~ 0x13C  AES Initial Vector Word 0 ~ 3 Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |AES Initial Vectors
     * |        |          |n = 0, 1..3.
     * |        |          |x = 0, 1..3.
     * |        |          |Four initial vectors (CRPT_AESn_IV0, CRPT_AESn_IV1, CRPT_AESn_IV2, and CRPT_AESn_IV3) are for AES operating in CBC, CFB, and OFB mode
     * |        |          |Four registers (CRPT_AESn_IV0, CRPT_AESn_IV1, CRPT_AESn_IV2, and CRPT_AESn_IV3) act as Nonce counter when the AES engine is operating in CTR mode.
     * @var CRPT_T::AES0_SADDR
     * Offset: 0x140  AES DMA Source Address Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SADDR     |AES DMA Source Address
     * |        |          |The AES accelerator supports DMA function to transfer the plain text between system memory and embedded FIFO
     * |        |          |The SADDR keeps the source address of the data buffer where the source text is stored
     * |        |          |Based on the source address, the AES accelerator can read the plain text from system memory and do AES operation
     * |        |          |The start of source address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of SADDR are ignored.
     * |        |          |SADDR can be read and written
     * |        |          |Writing to SADDR while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of SADDR will be updated later on
     * |        |          |Consequently, software can prepare the DMA source address for the next AES operation.
     * |        |          |In DMA mode, software can update the next CRPT_AESn_SADDR before triggering START.
     * |        |          |The value of CRPT_AESn_SADDR and CRPT_AESn_DADDR can be the same.
     * @var CRPT_T::AES0_DADDR
     * Offset: 0x144  AES DMA Destination Address Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DADDR     |AES DMA Destination Address
     * |        |          |The AES accelerator supports DMA function to transfer the cipher text between system memory and embedded FIFO
     * |        |          |The DADDR keeps the destination address of the data buffer where the engine output's text will be stored
     * |        |          |Based on the destination address, the AES accelerator can write the cipher text back to system memory after the AES operation is finished
     * |        |          |The start of destination address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of DADDR are ignored.
     * |        |          |DADDR can be read and written
     * |        |          |Writing to DADDR while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of DADDR will be updated later on
     * |        |          |Consequently, software can prepare the destination address for the next AES operation.
     * |        |          |In DMA mode, software can update the next CRPT_AESn_DADDR before triggering START.
     * |        |          |The value of CRPT_AESn_SADDR and CRPT_AESn_DADDR can be the same.
     * @var CRPT_T::AES0_CNT
     * Offset: 0x148  AES Byte Count Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNT       |AES Byte Count
     * |        |          |The CRPT_AESn_CNT keeps the byte count of source text that is for the AES engine operating in DMA mode
     * |        |          |The CRPT_AESn_CNT is 32-bit and the maximum of byte count is 4G bytes.
     * |        |          |CRPT_AESn_CNT can be read and written
     * |        |          |Writing to CRPT_AESn_CNT while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of CRPT_AESn_CNT will be updated later on
     * |        |          |Consequently, software can prepare the byte count of data for the next AES operation.
     * |        |          |According to CBC-CS1, CBC-CS2, and CBC-CS3 standard, the count of operation data must be at least one block
     * |        |          |Operations that are less than one block will output unexpected result.
     * |        |          |In Non-DMA ECB, CBC, CFB, OFB, and CTR mode, CRPT_AESn_CNT must be set as byte count for the last block of data before feeding in the last block of data
     * |        |          |In Non-DMA CBC-CS1, CBC-CS2, and CBC-CS3 mode, CRPT_AESn_CNT must be set as byte count for the last two blocks of data before feeding in the last two blocks of data.
     * @var CRPT_T::AES1_KEY[8]
     * Offset: 0x14C ~ 0x168  AES Key Word 0 ~ 7 Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |CRPT_AESn_KEYx
     * |        |          |The KEY keeps the security key for AES operation.
     * |        |          |n = 0, 1..3.
     * |        |          |x = 0, 1..7.
     * |        |          |The security key for AES accelerator can be 128, 192, or 256 bits and four, six, or eight 32-bit registers are to store each security key
     * |        |          |{CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 128-bit security key for AES operation
     * |        |          |{CRPT_AESn_KEY5, CRPT_AESn_KEY4, CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 192-bit security key for AES operation
     * |        |          |{CRPT_AESn_KEY7, CRPT_AESn_KEY6, CRPT_AESn_KEY5, CRPT_AESn_KEY4, CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 256-bit security key for AES operation.
     * @var CRPT_T::AES1_IV[4]
     * Offset: 0x16C ~ 0x178  AES Initial Vector Word 0 ~ 3 Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |AES Initial Vectors
     * |        |          |n = 0, 1..3.
     * |        |          |x = 0, 1..3.
     * |        |          |Four initial vectors (CRPT_AESn_IV0, CRPT_AESn_IV1, CRPT_AESn_IV2, and CRPT_AESn_IV3) are for AES operating in CBC, CFB, and OFB mode
     * |        |          |Four registers (CRPT_AESn_IV0, CRPT_AESn_IV1, CRPT_AESn_IV2, and CRPT_AESn_IV3) act as Nonce counter when the AES engine is operating in CTR mode.
     * @var CRPT_T::AES1_SADDR
     * Offset: 0x17C  AES DMA Source Address Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SADDR     |AES DMA Source Address
     * |        |          |The AES accelerator supports DMA function to transfer the plain text between system memory and embedded FIFO
     * |        |          |The SADDR keeps the source address of the data buffer where the source text is stored
     * |        |          |Based on the source address, the AES accelerator can read the plain text from system memory and do AES operation
     * |        |          |The start of source address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of SADDR are ignored.
     * |        |          |SADDR can be read and written
     * |        |          |Writing to SADDR while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of SADDR will be updated later on
     * |        |          |Consequently, software can prepare the DMA source address for the next AES operation.
     * |        |          |In DMA mode, software can update the next CRPT_AESn_SADDR before triggering START.
     * |        |          |The value of CRPT_AESn_SADDR and CRPT_AESn_DADDR can be the same.
     * @var CRPT_T::AES1_DADDR
     * Offset: 0x180  AES DMA Destination Address Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DADDR     |AES DMA Destination Address
     * |        |          |The AES accelerator supports DMA function to transfer the cipher text between system memory and embedded FIFO
     * |        |          |The DADDR keeps the destination address of the data buffer where the engine output's text will be stored
     * |        |          |Based on the destination address, the AES accelerator can write the cipher text back to system memory after the AES operation is finished
     * |        |          |The start of destination address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of DADDR are ignored.
     * |        |          |DADDR can be read and written
     * |        |          |Writing to DADDR while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of DADDR will be updated later on
     * |        |          |Consequently, software can prepare the destination address for the next AES operation.
     * |        |          |In DMA mode, software can update the next CRPT_AESn_DADDR before triggering START.
     * |        |          |The value of CRPT_AESn_SADDR and CRPT_AESn_DADDR can be the same.
     * @var CRPT_T::AES1_CNT
     * Offset: 0x184  AES Byte Count Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNT       |AES Byte Count
     * |        |          |The CRPT_AESn_CNT keeps the byte count of source text that is for the AES engine operating in DMA mode
     * |        |          |The CRPT_AESn_CNT is 32-bit and the maximum of byte count is 4G bytes.
     * |        |          |CRPT_AESn_CNT can be read and written
     * |        |          |Writing to CRPT_AESn_CNT while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of CRPT_AESn_CNT will be updated later on
     * |        |          |Consequently, software can prepare the byte count of data for the next AES operation.
     * |        |          |According to CBC-CS1, CBC-CS2, and CBC-CS3 standard, the count of operation data must be at least one block
     * |        |          |Operations that are less than one block will output unexpected result.
     * |        |          |In Non-DMA ECB, CBC, CFB, OFB, and CTR mode, CRPT_AESn_CNT must be set as byte count for the last block of data before feeding in the last block of data
     * |        |          |In Non-DMA CBC-CS1, CBC-CS2, and CBC-CS3 mode, CRPT_AESn_CNT must be set as byte count for the last two blocks of data before feeding in the last two blocks of data.
     * @var CRPT_T::AES2_KEY[8]
     * Offset: 0x188 ~ 0x1A4  AES Key Word 0 ~ 7 Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |CRPT_AESn_KEYx
     * |        |          |The KEY keeps the security key for AES operation.
     * |        |          |n = 0, 1..3.
     * |        |          |x = 0, 1..7.
     * |        |          |The security key for AES accelerator can be 128, 192, or 256 bits and four, six, or eight 32-bit registers are to store each security key
     * |        |          |{CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 128-bit security key for AES operation
     * |        |          |{CRPT_AESn_KEY5, CRPT_AESn_KEY4, CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 192-bit security key for AES operation
     * |        |          |{CRPT_AESn_KEY7, CRPT_AESn_KEY6, CRPT_AESn_KEY5, CRPT_AESn_KEY4, CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 256-bit security key for AES operation.
     * @var CRPT_T::AES2_IV[4]
     * Offset: 0x1A8 ~ 0x1B4  AES Initial Vector Word 0 ~ 3 Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |AES Initial Vectors
     * |        |          |n = 0, 1..3.
     * |        |          |x = 0, 1..3.
     * |        |          |Four initial vectors (CRPT_AESn_IV0, CRPT_AESn_IV1, CRPT_AESn_IV2, and CRPT_AESn_IV3) are for AES operating in CBC, CFB, and OFB mode
     * |        |          |Four registers (CRPT_AESn_IV0, CRPT_AESn_IV1, CRPT_AESn_IV2, and CRPT_AESn_IV3) act as Nonce counter when the AES engine is operating in CTR mode.
     * @var CRPT_T::AES2_SADDR
     * Offset: 0x1B8  AES DMA Source Address Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SADDR     |AES DMA Source Address
     * |        |          |The AES accelerator supports DMA function to transfer the plain text between system memory and embedded FIFO
     * |        |          |The SADDR keeps the source address of the data buffer where the source text is stored
     * |        |          |Based on the source address, the AES accelerator can read the plain text from system memory and do AES operation
     * |        |          |The start of source address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of SADDR are ignored.
     * |        |          |SADDR can be read and written
     * |        |          |Writing to SADDR while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of SADDR will be updated later on
     * |        |          |Consequently, software can prepare the DMA source address for the next AES operation.
     * |        |          |In DMA mode, software can update the next CRPT_AESn_SADDR before triggering START.
     * |        |          |The value of CRPT_AESn_SADDR and CRPT_AESn_DADDR can be the same.
     * @var CRPT_T::AES2_DADDR
     * Offset: 0x1BC  AES DMA Destination Address Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DADDR     |AES DMA Destination Address
     * |        |          |The AES accelerator supports DMA function to transfer the cipher text between system memory and embedded FIFO
     * |        |          |The DADDR keeps the destination address of the data buffer where the engine output's text will be stored
     * |        |          |Based on the destination address, the AES accelerator can write the cipher text back to system memory after the AES operation is finished
     * |        |          |The start of destination address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of DADDR are ignored.
     * |        |          |DADDR can be read and written
     * |        |          |Writing to DADDR while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of DADDR will be updated later on
     * |        |          |Consequently, software can prepare the destination address for the next AES operation.
     * |        |          |In DMA mode, software can update the next CRPT_AESn_DADDR before triggering START.
     * |        |          |The value of CRPT_AESn_SADDR and CRPT_AESn_DADDR can be the same.
     * @var CRPT_T::AES2_CNT
     * Offset: 0x1C0  AES Byte Count Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNT       |AES Byte Count
     * |        |          |The CRPT_AESn_CNT keeps the byte count of source text that is for the AES engine operating in DMA mode
     * |        |          |The CRPT_AESn_CNT is 32-bit and the maximum of byte count is 4G bytes.
     * |        |          |CRPT_AESn_CNT can be read and written
     * |        |          |Writing to CRPT_AESn_CNT while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of CRPT_AESn_CNT will be updated later on
     * |        |          |Consequently, software can prepare the byte count of data for the next AES operation.
     * |        |          |According to CBC-CS1, CBC-CS2, and CBC-CS3 standard, the count of operation data must be at least one block
     * |        |          |Operations that are less than one block will output unexpected result.
     * |        |          |In Non-DMA ECB, CBC, CFB, OFB, and CTR mode, CRPT_AESn_CNT must be set as byte count for the last block of data before feeding in the last block of data
     * |        |          |In Non-DMA CBC-CS1, CBC-CS2, and CBC-CS3 mode, CRPT_AESn_CNT must be set as byte count for the last two blocks of data before feeding in the last two blocks of data.
     * @var CRPT_T::AES3_KEY[8]
     * Offset: 0x1C4 ~ 0x1E0  AES Key Word 0 ~ 7 Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |CRPT_AESn_KEYx
     * |        |          |The KEY keeps the security key for AES operation.
     * |        |          |n = 0, 1..3.
     * |        |          |x = 0, 1..7.
     * |        |          |The security key for AES accelerator can be 128, 192, or 256 bits and four, six, or eight 32-bit registers are to store each security key
     * |        |          |{CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 128-bit security key for AES operation
     * |        |          |{CRPT_AESn_KEY5, CRPT_AESn_KEY4, CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 192-bit security key for AES operation
     * |        |          |{CRPT_AESn_KEY7, CRPT_AESn_KEY6, CRPT_AESn_KEY5, CRPT_AESn_KEY4, CRPT_AESn_KEY3, CRPT_AESn_KEY2, CRPT_AESn_KEY1, CRPT_AESn_KEY0} stores the 256-bit security key for AES operation.
     * @var CRPT_T::AES3_IV[4]
     * Offset: 0x1E4 ~ 0x1F0  AES Initial Vector Word 0 ~ 3 Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |AES Initial Vectors
     * |        |          |n = 0, 1..3.
     * |        |          |x = 0, 1..3.
     * |        |          |Four initial vectors (CRPT_AESn_IV0, CRPT_AESn_IV1, CRPT_AESn_IV2, and CRPT_AESn_IV3) are for AES operating in CBC, CFB, and OFB mode
     * |        |          |Four registers (CRPT_AESn_IV0, CRPT_AESn_IV1, CRPT_AESn_IV2, and CRPT_AESn_IV3) act as Nonce counter when the AES engine is operating in CTR mode.
     * @var CRPT_T::AES3_SADDR
     * Offset: 0x1F4  AES DMA Source Address Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SADDR     |AES DMA Source Address
     * |        |          |The AES accelerator supports DMA function to transfer the plain text between system memory and embedded FIFO
     * |        |          |The SADDR keeps the source address of the data buffer where the source text is stored
     * |        |          |Based on the source address, the AES accelerator can read the plain text from system memory and do AES operation
     * |        |          |The start of source address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of SADDR are ignored.
     * |        |          |SADDR can be read and written
     * |        |          |Writing to SADDR while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of SADDR will be updated later on
     * |        |          |Consequently, software can prepare the DMA source address for the next AES operation.
     * |        |          |In DMA mode, software can update the next CRPT_AESn_SADDR before triggering START.
     * |        |          |The value of CRPT_AESn_SADDR and CRPT_AESn_DADDR can be the same.
     * @var CRPT_T::AES3_DADDR
     * Offset: 0x1F8  AES DMA Destination Address Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DADDR     |AES DMA Destination Address
     * |        |          |The AES accelerator supports DMA function to transfer the cipher text between system memory and embedded FIFO
     * |        |          |The DADDR keeps the destination address of the data buffer where the engine output's text will be stored
     * |        |          |Based on the destination address, the AES accelerator can write the cipher text back to system memory after the AES operation is finished
     * |        |          |The start of destination address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of DADDR are ignored.
     * |        |          |DADDR can be read and written
     * |        |          |Writing to DADDR while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of DADDR will be updated later on
     * |        |          |Consequently, software can prepare the destination address for the next AES operation.
     * |        |          |In DMA mode, software can update the next CRPT_AESn_DADDR before triggering START.
     * |        |          |The value of CRPT_AESn_SADDR and CRPT_AESn_DADDR can be the same.
     * @var CRPT_T::AES3_CNT
     * Offset: 0x1FC  AES Byte Count Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNT       |AES Byte Count
     * |        |          |The CRPT_AESn_CNT keeps the byte count of source text that is for the AES engine operating in DMA mode
     * |        |          |The CRPT_AESn_CNT is 32-bit and the maximum of byte count is 4G bytes.
     * |        |          |CRPT_AESn_CNT can be read and written
     * |        |          |Writing to CRPT_AESn_CNT while the AES accelerator is operating doesn't affect the current AES operation
     * |        |          |But the value of CRPT_AESn_CNT will be updated later on
     * |        |          |Consequently, software can prepare the byte count of data for the next AES operation.
     * |        |          |According to CBC-CS1, CBC-CS2, and CBC-CS3 standard, the count of operation data must be at least one block
     * |        |          |Operations that are less than one block will output unexpected result.
     * |        |          |In Non-DMA ECB, CBC, CFB, OFB, and CTR mode, CRPT_AESn_CNT must be set as byte count for the last block of data before feeding in the last block of data
     * |        |          |In Non-DMA CBC-CS1, CBC-CS2, and CBC-CS3 mode, CRPT_AESn_CNT must be set as byte count for the last two blocks of data before feeding in the last two blocks of data.
     * @var CRPT_T::TDES_CTL
     * Offset: 0x200  TDES/DES Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |START     |TDES/DES Engine Start
     * |        |          |0 = No effect.
     * |        |          |1 = Start TDES/DES engine. The flag BUSY would be set.
     * |        |          |Note: The bit is always 0 when it's read back.
     * |[1]     |STOP      |TDES/DES Engine Stop
     * |        |          |0 = No effect.
     * |        |          |1 = Stop TDES/DES engine.
     * |        |          |Note: The bit is always 0 when it's read back.
     * |[2]     |TMODE     |TDES/DES Engine Operating Mode
     * |        |          |0 = Set DES mode for TDES/DES engine.
     * |        |          |1 = Set Triple DES mode for TDES/DES engine.
     * |[3]     |3KEYS     |TDES/DES Key Number
     * |        |          |0 = Select KEY1 and KEY2 in TDES/DES engine.
     * |        |          |1 = Triple keys in TDES/DES engine Enabled.
     * |[5]     |DMALAST   |TDES/DES Engine Start for the Last Block
     * |        |          |In DMA mode, this bit must be set as beginning the last DMA cascade round.
     * |        |          |In Non-DMA mode, this bit must be set as feeding in last block of data.
     * |[6]     |DMACSCAD  |TDES/DES Engine DMA with Cascade Mode
     * |        |          |0 = DMA cascade function Disabled.
     * |        |          |1 = In DMA Cascade mode, software can update DMA source address register, destination address register, and byte count register during a cascade operation, without finishing the accelerator operation.
     * |[7]     |DMAEN     |TDES/DES Engine DMA Enable Control
     * |        |          |0 = TDES_DMA engine Disabled.
     * |        |          |TDES engine operates in Non-DMA mode, and get data from the port CRPT_TDES_DATIN.
     * |        |          |1 = TDES_DMA engine Enabled.
     * |        |          |TDES engine operates in DMA mode, and data movement from/to the engine is done by DMA logic.
     * |[10:8]  |OPMODE    |TDES/DES Engine Operation Mode
     * |        |          |0x00 = ECB (Electronic Codebook Mode).
     * |        |          |0x01 = CBC (Cipher Block Chaining Mode).
     * |        |          |0x02 = CFB (Cipher Feedback Mode).
     * |        |          |0x03 = OFB (Output Feedback Mode).
     * |        |          |0x04 = CTR (Counter Mode).
     * |        |          |Others = CTR (Counter Mode).
     * |[16]    |ENCRPT    |TDES/DES Encryption/Decryption
     * |        |          |0 = TDES engine executes decryption operation.
     * |        |          |1 = TDES engine executes encryption operation.
     * |[21]    |BLKSWAP   |TDES/DES Engine Block Double Word Endian Swap
     * |        |          |0 = Keep the original order, e.g. {WORD_H, WORD_L}.
     * |        |          |1 = When this bit is set to 1, the TDES engine would exchange high and low word in the sequence {WORD_L, WORD_H}.
     * |[22]    |OUTSWAP   |TDES/DES Engine Output Data Swap
     * |        |          |0 = Keep the original order.
     * |        |          |1 = The order that CPU outputs data from the accelerator will be changed from {byte3, byte2, byte1, byte0} to {byte0, byte1, byte2, byte3}.
     * |[23]    |INSWAP    |TDES/DES Engine Input Data Swap
     * |        |          |0 = Keep the original order.
     * |        |          |1 = The order that CPU feeds data to the accelerator will be changed from {byte3, byte2, byte1, byte0} to {byte0, byte1, byte2, byte3}.
     * |[25:24] |CHANNEL   |TDES/DES Engine Working Channel
     * |        |          |00 = Current control register setting is for channel 0.
     * |        |          |01 = Current control register setting is for channel 1.
     * |        |          |10 = Current control register setting is for channel 2.
     * |        |          |11 = Current control register setting is for channel 3.
     * |[30:26] |KEYUNPRT  |Unprotect Key
     * |        |          |Writing 0 to CRPT_TDES_CTL [31] and "10110" to CRPT_TDES_CTL [30:26] is to unprotect TDES key.
     * |        |          |The KEYUNPRT can be read and written
     * |        |          |When it is written as the TDES engine is operating, BUSY flag is 1, there would be no effect on KEYUNPRT.
     * |[31]    |KEYPRT    |Protect Key
     * |        |          |Read as a flag to reflect KEYPRT.
     * |        |          |0 = No effect.
     * |        |          |1 = This bit is to protect the content of TDES key from reading
     * |        |          |The return value for reading CRPT_ TDESn_KEYxH/L is not the content in the registers CRPT_ TDESn_KEYxH/L
     * |        |          |Once it is set, it can be cleared by asserting KEYUNPRT
     * |        |          |The key content would be cleared as well.
     * @var CRPT_T::TDES_STS
     * Offset: 0x204  TDES/DES Engine Flag
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BUSY      |TDES/DES Engine Busy
     * |        |          |0 = TDES/DES engine is idle or finished.
     * |        |          |1 = TDES/DES engine is under processing.
     * |[8]     |INBUFEMPTY|TDES/DES in Buffer Empty
     * |        |          |0 = There are some data in input buffer waiting for the TDES/DES engine to process.
     * |        |          |1 = TDES/DES input buffer is empty
     * |        |          |Software needs to feed data to the TDES/DES engine
     * |        |          |Otherwise, the TDES/DES engine will be pending to wait for input data.
     * |[9]     |INBUFFULL |TDES/DES in Buffer Full Flag
     * |        |          |0 = TDES/DES input buffer is not full. Software can feed the data into the TDES/DES engine.
     * |        |          |1 = TDES input buffer is full
     * |        |          |Software cannot feed data to the TDES/DES engine
     * |        |          |Otherwise, the flag INBUFERR will be set to 1.
     * |[10]    |INBUFERR  |TDES/DES in Buffer Error Flag
     * |        |          |0 = No error.
     * |        |          |1 = Error happens during feeding data to the TDES/DES engine.
     * |[16]    |OUTBUFEMPTY|TDES/DES Output Buffer Empty Flag
     * |        |          |0 = TDES/DES output buffer is not empty. There are some valid data kept in output buffer.
     * |        |          |1 = TDES/DES output buffer is empty, Software cannot get data from TDES_DATA_OUT
     * |        |          |Otherwise the flag OUTBUFERR will be set to 1, since output buffer is empty.
     * |[17]    |OUTBUFFULL|TDES/DES Output Buffer Full Flag
     * |        |          |0 = TDES/DES output buffer is not full.
     * |        |          |1 = TDES/DES output buffer is full, and software needs to get data from TDES_DATA_OUT
     * |        |          |Otherwise, the TDES/DES engine will be pending since output buffer is full.
     * |[18]    |OUTBUFERR |TDES/DES Out Buffer Error Flag
     * |        |          |0 = No error.
     * |        |          |1 = Error happens during getting test result from TDES/DES engine.
     * |[20]    |BUSERR    |TDES/DES DMA Access Bus Error Flag
     * |        |          |0 = No error.
     * |        |          |1 = Bus error will stop DMA operation and TDES/DES engine.
     * @var CRPT_T::TDES0_KEY1H
     * Offset: 0x208  TDES/DES Key 1 High Word Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 1 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES0_KEY1L
     * Offset: 0x20C  TDES/DES Key 1 Low Word Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 1 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES0_KEY2H
     * Offset: 0x210  TDES Key 2 High Word Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 2 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES0_KEY2L
     * Offset: 0x214  TDES Key 2 Low Word Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 2 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES0_KEY3H
     * Offset: 0x218  TDES Key 3 High Word Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 3 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES0_KEY3L
     * Offset: 0x21C  TDES Key 3 Low Word Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 3 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES0_IVH
     * Offset: 0x220  TDES/DES Initial Vector High Word Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |TDES/DES Initial Vector High Word
     * |        |          |Initial vector (IV) is for TDES/DES engine in CBC, CFB, and OFB mode
     * |        |          |IV is Nonce counter for TDES/DES engine in CTR mode.
     * @var CRPT_T::TDES0_IVL
     * Offset: 0x224  TDES/DES Initial Vector Low Word Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |TDES/DES Initial Vector Low Word
     * |        |          |Initial vector (IV) is for TDES/DES engine in CBC, CFB, and OFB mode
     * |        |          |IV is Nonce counter for TDES/DES engine in CTR mode.
     * @var CRPT_T::TDES0_SA
     * Offset: 0x228  TDES/DES DMA Source Address Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SADDR     |TDES/DES DMA Source Address
     * |        |          |The TDES/DES accelerator supports DMA function to transfer the plain text between system memory and embedded FIFO
     * |        |          |The CRPT_TDESn_SA keeps the source address of the data buffer where the source text is stored
     * |        |          |Based on the source address, the TDES/DES accelerator can read the plain text from system memory and do TDES/DES operation
     * |        |          |The start of source address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of CRPT_TDESn_SA are ignored.
     * |        |          |CRPT_TDESn_SA can be read and written
     * |        |          |Writing to CRPT_TDESn_SA while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_SA will be updated later on
     * |        |          |Consequently, software can prepare the DMA source address for the next TDES/DES operation.
     * |        |          |In DMA mode, software can update the next CRPT_TDESn_SA before triggering START.
     * |        |          |CRPT_TDESn_SA and CRPT_TDESn_DA can be the same in the value.
     * @var CRPT_T::TDES0_DA
     * Offset: 0x22C  TDES/DES DMA Destination Address Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DADDR     |TDES/DES DMA Destination Address
     * |        |          |The TDES/DES accelerator supports DMA function to transfer the cipher text between system memory and embedded FIFO
     * |        |          |The CRPT_TDESn_DA keeps the destination address of the data buffer where the engine output's text will be stored
     * |        |          |Based on the destination address, the TDES/DES accelerator can write the cipher text back to system memory after the TDES/DES operation is finished
     * |        |          |The start of destination address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of CRPT_TDESn_DA are ignored.
     * |        |          |CRPT_TDESn_DA can be read and written
     * |        |          |Writing to CRPT_TDESn_DA while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_DA will be updated later on
     * |        |          |Consequently, software can prepare the destination address for the next TDES/DES operation.
     * |        |          |In DMA mode, software can update the next CRPT_TDESn_DA before triggering START.
     * |        |          |CRPT_TDESn_SAD and CRPT_TDESn_DA can be the same in the value.
     * @var CRPT_T::TDES0_CNT
     * Offset: 0x230  TDES/DES Byte Count Register for Channel 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNT       |TDES/DES Byte Count
     * |        |          |The CRPT_TDESn_CNT keeps the byte count of source text that is for the TDES/DES engine operating in DMA mode
     * |        |          |The CRPT_TDESn_CNT is 32-bit and the maximum of byte count is 4G bytes.
     * |        |          |CRPT_TDESn_CNT can be read and written
     * |        |          |Writing to CRPT_TDESn_CNT while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_CNT will be updated later on
     * |        |          |Consequently, software can prepare the byte count of data for the next TDES /DES operation.
     * |        |          |In Non-DMA ECB, CBC, CFB, OFB, and CTR mode, CRPT_TDESn_CNT must be set as byte count for the last block of data before feeding in the last block of data.
     * @var CRPT_T::TDES_DATIN
     * Offset: 0x234  TDES/DES Engine Input data Word Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DATIN     |TDES/DES Engine Input Port
     * |        |          |CPU feeds data to TDES/DES engine through this port by checking CRPT_TDES_STS
     * |        |          |Feed data as INBUFFULL is 0.
     * @var CRPT_T::TDES_DATOUT
     * Offset: 0x238  TDES/DES Engine Output data Word Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DATOUT    |TDES/DES Engine Output Port
     * |        |          |CPU gets result from the TDES/DES engine through this port by checking CRPT_TDES_STS
     * |        |          |Get data as OUTBUFEMPTY is 0.
     * @var CRPT_T::TDES1_KEY1H
     * Offset: 0x248  TDES/DES Key 1 High Word Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 1 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES1_KEY1L
     * Offset: 0x24C  TDES/DES Key 1 Low Word Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 1 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES1_KEY2H
     * Offset: 0x250  TDES Key 2 High Word Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 2 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES1_KEY2L
     * Offset: 0x254  TDES Key 2 Low Word Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 2 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES1_KEY3H
     * Offset: 0x258  TDES Key 3 High Word Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 3 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES1_KEY3L
     * Offset: 0x25C  TDES Key 3 Low Word Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 3 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES1_IVH
     * Offset: 0x260  TDES/DES Initial Vector High Word Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |TDES/DES Initial Vector High Word
     * |        |          |Initial vector (IV) is for TDES/DES engine in CBC, CFB, and OFB mode
     * |        |          |IV is Nonce counter for TDES/DES engine in CTR mode.
     * @var CRPT_T::TDES1_IVL
     * Offset: 0x264  TDES/DES Initial Vector Low Word Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |TDES/DES Initial Vector Low Word
     * |        |          |Initial vector (IV) is for TDES/DES engine in CBC, CFB, and OFB mode
     * |        |          |IV is Nonce counter for TDES/DES engine in CTR mode.
     * @var CRPT_T::TDES1_SA
     * Offset: 0x268  TDES/DES DMA Source Address Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SADDR     |TDES/DES DMA Source Address
     * |        |          |The TDES/DES accelerator supports DMA function to transfer the plain text between system memory and embedded FIFO
     * |        |          |The CRPT_TDESn_SA keeps the source address of the data buffer where the source text is stored
     * |        |          |Based on the source address, the TDES/DES accelerator can read the plain text from system memory and do TDES/DES operation
     * |        |          |The start of source address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of CRPT_TDESn_SA are ignored.
     * |        |          |CRPT_TDESn_SA can be read and written
     * |        |          |Writing to CRPT_TDESn_SA while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_SA will be updated later on
     * |        |          |Consequently, software can prepare the DMA source address for the next TDES/DES operation.
     * |        |          |In DMA mode, software can update the next CRPT_TDESn_SA before triggering START.
     * |        |          |CRPT_TDESn_SA and CRPT_TDESn_DA can be the same in the value.
     * @var CRPT_T::TDES1_DA
     * Offset: 0x26C  TDES/DES DMA Destination Address Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DADDR     |TDES/DES DMA Destination Address
     * |        |          |The TDES/DES accelerator supports DMA function to transfer the cipher text between system memory and embedded FIFO
     * |        |          |The CRPT_TDESn_DA keeps the destination address of the data buffer where the engine output's text will be stored
     * |        |          |Based on the destination address, the TDES/DES accelerator can write the cipher text back to system memory after the TDES/DES operation is finished
     * |        |          |The start of destination address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of CRPT_TDESn_DA are ignored.
     * |        |          |CRPT_TDESn_DA can be read and written
     * |        |          |Writing to CRPT_TDESn_DA while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_DA will be updated later on
     * |        |          |Consequently, software can prepare the destination address for the next TDES/DES operation.
     * |        |          |In DMA mode, software can update the next CRPT_TDESn_DA before triggering START.
     * |        |          |CRPT_TDESn_SAD and CRPT_TDESn_DA can be the same in the value.
     * @var CRPT_T::TDES1_CNT
     * Offset: 0x270  TDES/DES Byte Count Register for Channel 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNT       |TDES/DES Byte Count
     * |        |          |The CRPT_TDESn_CNT keeps the byte count of source text that is for the TDES/DES engine operating in DMA mode
     * |        |          |The CRPT_TDESn_CNT is 32-bit and the maximum of byte count is 4G bytes.
     * |        |          |CRPT_TDESn_CNT can be read and written
     * |        |          |Writing to CRPT_TDESn_CNT while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_CNT will be updated later on
     * |        |          |Consequently, software can prepare the byte count of data for the next TDES /DES operation.
     * |        |          |In Non-DMA ECB, CBC, CFB, OFB, and CTR mode, CRPT_TDESn_CNT must be set as byte count for the last block of data before feeding in the last block of data.
     * @var CRPT_T::TDES2_KEY1H
     * Offset: 0x288  TDES/DES Key 1 High Word Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 1 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES2_KEY1L
     * Offset: 0x28C  TDES/DES Key 1 Low Word Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 1 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES2_KEY2H
     * Offset: 0x290  TDES Key 2 High Word Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 2 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES2_KEY2L
     * Offset: 0x294  TDES Key 2 Low Word Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 2 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES2_KEY3H
     * Offset: 0x298  TDES Key 3 High Word Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 3 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES2_KEY3L
     * Offset: 0x29C  TDES Key 3 Low Word Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 3 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES2_IVH
     * Offset: 0x2A0  TDES/DES Initial Vector High Word Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |TDES/DES Initial Vector High Word
     * |        |          |Initial vector (IV) is for TDES/DES engine in CBC, CFB, and OFB mode
     * |        |          |IV is Nonce counter for TDES/DES engine in CTR mode.
     * @var CRPT_T::TDES2_IVL
     * Offset: 0x2A4  TDES/DES Initial Vector Low Word Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |TDES/DES Initial Vector Low Word
     * |        |          |Initial vector (IV) is for TDES/DES engine in CBC, CFB, and OFB mode
     * |        |          |IV is Nonce counter for TDES/DES engine in CTR mode.
     * @var CRPT_T::TDES2_SA
     * Offset: 0x2A8  TDES/DES DMA Source Address Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SADDR     |TDES/DES DMA Source Address
     * |        |          |The TDES/DES accelerator supports DMA function to transfer the plain text between system memory and embedded FIFO
     * |        |          |The CRPT_TDESn_SA keeps the source address of the data buffer where the source text is stored
     * |        |          |Based on the source address, the TDES/DES accelerator can read the plain text from system memory and do TDES/DES operation
     * |        |          |The start of source address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of CRPT_TDESn_SA are ignored.
     * |        |          |CRPT_TDESn_SA can be read and written
     * |        |          |Writing to CRPT_TDESn_SA while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_SA will be updated later on
     * |        |          |Consequently, software can prepare the DMA source address for the next TDES/DES operation.
     * |        |          |In DMA mode, software can update the next CRPT_TDESn_SA before triggering START.
     * |        |          |CRPT_TDESn_SA and CRPT_TDESn_DA can be the same in the value.
     * @var CRPT_T::TDES2_DA
     * Offset: 0x2AC  TDES/DES DMA Destination Address Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DADDR     |TDES/DES DMA Destination Address
     * |        |          |The TDES/DES accelerator supports DMA function to transfer the cipher text between system memory and embedded FIFO
     * |        |          |The CRPT_TDESn_DA keeps the destination address of the data buffer where the engine output's text will be stored
     * |        |          |Based on the destination address, the TDES/DES accelerator can write the cipher text back to system memory after the TDES/DES operation is finished
     * |        |          |The start of destination address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of CRPT_TDESn_DA are ignored.
     * |        |          |CRPT_TDESn_DA can be read and written
     * |        |          |Writing to CRPT_TDESn_DA while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_DA will be updated later on
     * |        |          |Consequently, software can prepare the destination address for the next TDES/DES operation.
     * |        |          |In DMA mode, software can update the next CRPT_TDESn_DA before triggering START.
     * |        |          |CRPT_TDESn_SAD and CRPT_TDESn_DA can be the same in the value.
     * @var CRPT_T::TDES2_CNT
     * Offset: 0x2B0  TDES/DES Byte Count Register for Channel 2
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNT       |TDES/DES Byte Count
     * |        |          |The CRPT_TDESn_CNT keeps the byte count of source text that is for the TDES/DES engine operating in DMA mode
     * |        |          |The CRPT_TDESn_CNT is 32-bit and the maximum of byte count is 4G bytes.
     * |        |          |CRPT_TDESn_CNT can be read and written
     * |        |          |Writing to CRPT_TDESn_CNT while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_CNT will be updated later on
     * |        |          |Consequently, software can prepare the byte count of data for the next TDES /DES operation.
     * |        |          |In Non-DMA ECB, CBC, CFB, OFB, and CTR mode, CRPT_TDESn_CNT must be set as byte count for the last block of data before feeding in the last block of data.
     * @var CRPT_T::TDES3_KEY1H
     * Offset: 0x2C8  TDES/DES Key 1 High Word Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 1 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES3_KEY1L
     * Offset: 0x2CC  TDES/DES Key 1 Low Word Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 1 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES3_KEY2H
     * Offset: 0x2D0  TDES Key 2 High Word Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 2 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES3_KEY2L
     * Offset: 0x2D4  TDES Key 2 Low Word Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 2 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES3_KEY3H
     * Offset: 0x2D8  TDES Key 3 High Word Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 3 High Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES3_KEY3L
     * Offset: 0x2DC  TDES Key 3 Low Word Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEY       |TDES/DES Key 3 Low Word
     * |        |          |The key registers for TDES/DES algorithm calculation
     * |        |          |The security key for the TDES/DES accelerator is 64 bits
     * |        |          |Thus, it needs two 32-bit registers to store a security key
     * |        |          |The register CRPT_TDESn_KEYxH is used to keep the bit [63:32] of security key for the TDES/DES operation, while the register CRPT_TDESn_KEYxL is used to keep the bit [31:0].
     * @var CRPT_T::TDES3_IVH
     * Offset: 0x2E0  TDES/DES Initial Vector High Word Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |TDES/DES Initial Vector High Word
     * |        |          |Initial vector (IV) is for TDES/DES engine in CBC, CFB, and OFB mode
     * |        |          |IV is Nonce counter for TDES/DES engine in CTR mode.
     * @var CRPT_T::TDES3_IVL
     * Offset: 0x2E4  TDES/DES Initial Vector Low Word Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |IV        |TDES/DES Initial Vector Low Word
     * |        |          |Initial vector (IV) is for TDES/DES engine in CBC, CFB, and OFB mode
     * |        |          |IV is Nonce counter for TDES/DES engine in CTR mode.
     * @var CRPT_T::TDES3_SA
     * Offset: 0x2E8  TDES/DES DMA Source Address Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SADDR     |TDES/DES DMA Source Address
     * |        |          |The TDES/DES accelerator supports DMA function to transfer the plain text between system memory and embedded FIFO
     * |        |          |The CRPT_TDESn_SA keeps the source address of the data buffer where the source text is stored
     * |        |          |Based on the source address, the TDES/DES accelerator can read the plain text from system memory and do TDES/DES operation
     * |        |          |The start of source address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of CRPT_TDESn_SA are ignored.
     * |        |          |CRPT_TDESn_SA can be read and written
     * |        |          |Writing to CRPT_TDESn_SA while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_SA will be updated later on
     * |        |          |Consequently, software can prepare the DMA source address for the next TDES/DES operation.
     * |        |          |In DMA mode, software can update the next CRPT_TDESn_SA before triggering START.
     * |        |          |CRPT_TDESn_SA and CRPT_TDESn_DA can be the same in the value.
     * @var CRPT_T::TDES3_DA
     * Offset: 0x2EC  TDES/DES DMA Destination Address Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DADDR     |TDES/DES DMA Destination Address
     * |        |          |The TDES/DES accelerator supports DMA function to transfer the cipher text between system memory and embedded FIFO
     * |        |          |The CRPT_TDESn_DA keeps the destination address of the data buffer where the engine output's text will be stored
     * |        |          |Based on the destination address, the TDES/DES accelerator can write the cipher text back to system memory after the TDES/DES operation is finished
     * |        |          |The start of destination address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of CRPT_TDESn_DA are ignored.
     * |        |          |CRPT_TDESn_DA can be read and written
     * |        |          |Writing to CRPT_TDESn_DA while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_DA will be updated later on
     * |        |          |Consequently, software can prepare the destination address for the next TDES/DES operation.
     * |        |          |In DMA mode, software can update the next CRPT_TDESn_DA before triggering START.
     * |        |          |CRPT_TDESn_SAD and CRPT_TDESn_DA can be the same in the value.
     * @var CRPT_T::TDES3_CNT
     * Offset: 0x2F0  TDES/DES Byte Count Register for Channel 3
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CNT       |TDES/DES Byte Count
     * |        |          |The CRPT_TDESn_CNT keeps the byte count of source text that is for the TDES/DES engine operating in DMA mode
     * |        |          |The CRPT_TDESn_CNT is 32-bit and the maximum of byte count is 4G bytes.
     * |        |          |CRPT_TDESn_CNT can be read and written
     * |        |          |Writing to CRPT_TDESn_CNT while the TDES/DES accelerator is operating doesn't affect the current TDES/DES operation
     * |        |          |But the value of CRPT_TDESn_CNT will be updated later on
     * |        |          |Consequently, software can prepare the byte count of data for the next TDES /DES operation.
     * |        |          |In Non-DMA ECB, CBC, CFB, OFB, and CTR mode, CRPT_TDESn_CNT must be set as byte count for the last block of data before feeding in the last block of data.
     * @var CRPT_T::HMAC_CTL
     * Offset: 0x300  SHA/HMAC Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |START     |SHA/HMAC Engine Start
     * |        |          |0 = No effect.
     * |        |          |1 = Start SHA/HMAC engine. BUSY flag will be set.
     * |        |          |This bit is always 0 when it's read back.
     * |[1]     |STOP      |SHA/HMAC Engine Stop
     * |        |          |0 = No effect.
     * |        |          |1 = Stop SHA/HMAC engine.
     * |        |          |This bit is always 0 when it's read back.
     * |[4]     |HMACEN    |HMAC_SHA Engine Operating Mode
     * |        |          |0 = execute SHA function.
     * |        |          |1 = execute HMAC function.
     * |[5]     |DMALAST   |SHA/HMAC Last Block
     * |        |          |This bit must be set as feeding in last byte of data.
     * |[7]     |DMAEN     |SHA/HMAC Engine DMA Enable Control
     * |        |          |0 = SHA/HMAC DMA engine Disabled.
     * |        |          |SHA/HMAC engine operates in Non-DMA mode, and gets data from the port CRPT_HMAC_DATIN.
     * |        |          |1 = SHA/HMAC DMA engine Enabled.
     * |        |          |SHA/HMAC engine operates in DMA mode, and data movement from/to the engine is done by DMA logic.
     * |[10:8]  |OPMODE    |SHA/HMAC Engine Operation Modes
     * |        |          |0x0xx: SHA160
     * |        |          |0x100: SHA256
     * |        |          |0x101: SHA224
     * |        |          |0x110: SHA512
     * |        |          |0x111: SHA384
     * |        |          |These bits can be read and written. But writing to them wouldn't take effect as BUSY is 1.
     * |[22]    |OUTSWAP   |SHA/HMAC Engine Output Data Swap
     * |        |          |0 = Keep the original order.
     * |        |          |1 = The order that CPU feeds data to the accelerator will be changed from {byte3, byte2, byte1, byte0} to {byte0, byte1, byte2, byte3}.
     * |[23]    |INSWAP    |SHA/HMAC Engine Input Data Swap
     * |        |          |0 = Keep the original order.
     * |        |          |1 = The order that CPU feeds data to the accelerator will be changed from {byte3, byte2, byte1, byte0} to {byte0, byte1, byte2, byte3}.
     * @var CRPT_T::HMAC_STS
     * Offset: 0x304  SHA/HMAC Status Flag
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BUSY      |SHA/HMAC Engine Busy
     * |        |          |0 = SHA/HMAC engine is idle or finished.
     * |        |          |1 = SHA/HMAC engine is busy.
     * |[1]     |DMABUSY   |SHA/HMAC Engine DMA Busy Flag
     * |        |          |0 = SHA/HMAC DMA engine is idle or finished.
     * |        |          |1 = SHA/HMAC DMA engine is busy.
     * |[8]     |DMAERR    |SHA/HMAC Engine DMA Error Flag
     * |        |          |0 = Show the SHA/HMAC engine access normal.
     * |        |          |1 = Show the SHA/HMAC engine access error.
     * |[16]    |DATINREQ  |SHA/HMAC Non-DMA Mode Data Input Request
     * |        |          |0 = No effect.
     * |        |          |1 = Request SHA/HMAC Non-DMA mode data input.
     * @var CRPT_T::HMAC_DGST[16]
     * Offset: 0x308 ~ 0x344  SHA/HMAC Digest Message 0 ~ 15
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DGST      |SHA/HMAC Digest Message Output Register
     * |        |          |For SHA-160, the digest is stored in CRPT_HMAC_DGST0 ~ CRPT_HMAC_DGST4.
     * |        |          |For SHA-224, the digest is stored in CRPT_HMAC_DGST0 ~ CRPT_HMAC_DGST6.
     * |        |          |For SHA-256, the digest is stored in CRPT_HMAC_DGST0 ~ CRPT_HMAC_DGST7.
     * |        |          |For SHA-384, the digest is stored in CRPT_HMAC_DGST0 ~ CRPT_HMAC_DGST11.
     * |        |          |For SHA-512, the digest is stored in CRPT_HMAC_DGST0 ~ CRPT_HMAC_DGST15.
     * @var CRPT_T::HMAC_KEYCNT
     * Offset: 0x348  SHA/HMAC Key Byte Count Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |KEYCNT    |SHA/HMAC Key Byte Count
     * |        |          |The CRPT_HMAC_KEYCNT keeps the byte count of key that SHA/HMAC engine operates
     * |        |          |The register is 32-bit and the maximum byte count is 4G bytes
     * |        |          |It can be read and written.
     * |        |          |Writing to the register CRPT_HMAC_KEYCNT as the SHA/HMAC accelerator operating doesn't affect the current SHA/HMAC operation
     * |        |          |But the value of CRPT_SHA _KEYCNT will be updated later on
     * |        |          |Consequently, software can prepare the key count for the next SHA/HMAC operation.
     * @var CRPT_T::HMAC_SADDR
     * Offset: 0x34C  SHA/HMAC DMA Source Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SADDR     |SHA/HMAC DMA Source Address
     * |        |          |The SHA/HMAC accelerator supports DMA function to transfer the plain text between system memory and embedded FIFO
     * |        |          |The CRPT_HMAC_SADDR keeps the source address of the data buffer where the source text is stored
     * |        |          |Based on the source address, the SHA/HMAC accelerator can read the plain text from system memory and do SHA/HMAC operation
     * |        |          |The start of source address should be located at word boundary
     * |        |          |In other words, bit 1 and 0 of CRPT_HMAC_SADDR are ignored.
     * |        |          |CRPT_HMAC_SADDR can be read and written
     * |        |          |Writing to CRPT_HMAC_SADDR while the SHA/HMAC accelerator is operating doesn't affect the current SHA/HMAC operation
     * |        |          |But the value of CRPT_HMAC_SADDR will be updated later on
     * |        |          |Consequently, software can prepare the DMA source address for the next SHA/HMAC operation.
     * |        |          |In DMA mode, software can update the next CRPT_HMAC_SADDR before triggering START.
     * |        |          |CRPT_HMAC_SADDR and CRPT_HMAC_DADDR can be the same in the value.
     * @var CRPT_T::HMAC_DMACNT
     * Offset: 0x350  SHA/HMAC Byte Count Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DMACNT    |SHA/HMAC Operation Byte Count
     * |        |          |The CRPT_HMAC_DMACNT keeps the byte count of source text that is for the SHA/HMAC engine operating in DMA mode
     * |        |          |The CRPT_HMAC_DMACNT is 32-bit and the maximum of byte count is 4G bytes.
     * |        |          |CRPT_HMAC_DMACNT can be read and written
     * |        |          |Writing to CRPT_HMAC_DMACNT while the SHA/HMAC accelerator is operating doesn't affect the current SHA/HMAC operation
     * |        |          |But the value of CRPT_HMAC_DMACNT will be updated later on
     * |        |          |Consequently, software can prepare the byte count of data for the next SHA/HMAC operation.
     * |        |          |In Non-DMA mode, CRPT_HMAC_DMACNT must be set as the byte count of the last block before feeding in the last block of data.
     * @var CRPT_T::HMAC_DATIN
     * Offset: 0x354  SHA/HMAC Engine Non-DMA Mode Data Input Port Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DATIN     |SHA/HMAC Engine Input Port
     * |        |          |CPU feeds data to SHA/HMAC engine through this port by checking CRPT_HMAC_STS
     * |        |          |Feed data as DATINREQ is 1.
     * @var CRPT_T::ECC_CTL
     * Offset: 0x800  ECC Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |START     |ECC Accelerator Start
     * |        |          |0 = No effect.
     * |        |          |1 = Start ECC accelerator. BUSY flag will be set.
     * |        |          |This bit is always 0 when it's read back.
     * |        |          |ECC accelerator will ignore this START signal when BUSY flag is 1.
     * |[1]     |STOP      |ECC Accelerator Stop
     * |        |          |0 = No effect.
     * |        |          |1 = Abort ECC accelerator and make it into idle state.
     * |        |          |This bit is always 0 when it's read back.
     * |        |          |Remember to clear ECC interrupt flag after stopping ECC accelerator.
     * |[7]     |DMAEN     |ECC Accelerator DMA Enable Control
     * |        |          |0 = ECC DMA engine Disabled.
     * |        |          |1 = ECC DMA engine Enabled.
     * |        |          |Only when START and DMAEN are 1, ECC DMA engine will be active
     * |[8]     |FSEL      |Field Selection
     * |        |          |0 = Binary Field (GF(2^m)).
     * |        |          |1 = Prime Field (GF(p)).
     * |[10:9]  |ECCOP     |Point Operation for BF and PF
     * |        |          |00 = Point multiplication :.
     * |        |          |(POINTX1, POINTY1) = SCALARK * (POINTX1, POINTY1).
     * |        |          |01 = Modulus operation : choose by MODOP (CRPT_ECC_CTL[12:11]).
     * |        |          |10 = Point addition :.
     * |        |          |(POINTX1, POINTY1) = (POINTX1, POINTY1) +.
     * |        |          |(POINTX2, POINTY2)
     * |        |          |11 = Point doubling :.
     * |        |          |(POINTX1, POINTY1) = 2 * (POINTX1, POINTY1).
     * |        |          |Besides above three input data, point operations still need the parameters of elliptic curve (CURVEA, CURVEB, CURVEN and CURVEM) as shown in Figure 6.27-11
     * |[12:11] |MODOP     |Modulus Operation for PF
     * |        |          |00 = Division :.
     * |        |          |POINTX1 = (POINTY1 / POINTX1) % CURVEN.
     * |        |          |01 = Multiplication :.
     * |        |          |POINTX1 = (POINTX1 * POINTY1) % CURVEN.
     * |        |          |10 = Addition :.
     * |        |          |POINTX1 = (POINTX1 + POINTY1) % CURVEN.
     * |        |          |11 = Subtraction :.
     * |        |          |POINTX1 = (POINTX1 - POINTY1) % CURVEN.
     * |        |          |MODOP is active only when ECCOP = 01.
     * |[16]    |LDP1      |The Control Signal of Register for the X and Y Coordinate of the First Point (POINTX1, POINTY1)
     * |        |          |0 = The register for POINTX1 and POINTY1 is not modified by DMA or user.
     * |        |          |1 = The register for POINTX1 and POINTY1 is modified by DMA or user.
     * |[17]    |LDP2      |The Control Signal of Register for the X and Y Coordinate of the Second Point (POINTX2, POINTY2)
     * |        |          |0 = The register for POINTX2 and POINTY2 is not modified by DMA or user.
     * |        |          |1 = The register for POINTX2 and POINTY2 is modified by DMA or user.
     * |[18]    |LDA       |The Control Signal of Register for the Parameter CURVEA of Elliptic Curve
     * |        |          |0 = The register for CURVEA is not modified by DMA or user.
     * |        |          |1 = The register for CURVEA is modified by DMA or user.
     * |[19]    |LDB       |The Control Signal of Register for the Parameter CURVEB of Elliptic Curve
     * |        |          |0 = The register for CURVEB is not modified by DMA or user.
     * |        |          |1 = The register for CURVEB is modified by DMA or user.
     * |[20]    |LDN       |The Control Signal of Register for the Parameter CURVEN of Elliptic Curve
     * |        |          |0 = The register for CURVEN is not modified by DMA or user.
     * |        |          |1 = The register for CURVEN is modified by DMA or user.
     * |[21]    |LDK       |The Control Signal of Register for SCALARK
     * |        |          |0 = The register for SCALARK is not modified by DMA or user.
     * |        |          |1 = The register for SCALARK is modified by DMA or user.
     * |[31:22] |CURVEM    |The key length of elliptic curve.
     * @var CRPT_T::ECC_STS
     * Offset: 0x804  ECC Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BUSY      |ECC Accelerator Busy Flag
     * |        |          |0 = The ECC accelerator is idle or finished.
     * |        |          |1 = The ECC accelerator is under processing and protects all registers.
     * |        |          |Remember to clear ECC interrupt flag after ECC accelerator finished
     * |[1]     |DMABUSY   |ECC DMA Busy Flag
     * |        |          |0 = ECC DMA is idle or finished.
     * |        |          |1 = ECC DMA is busy.
     * |[16]    |BUSERR    |ECC DMA Access Bus Error Flag
     * |        |          |0 = No error.
     * |        |          |1 = Bus error will stop DMA operation and ECC accelerator.
     * @var CRPT_T::ECC_X1[18]
     * Offset: 0x808 ~ 0x84C  ECC The X-coordinate word 0 ~ 17 of the first point
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |POINTX1   |ECC the x-coordinate Value of the First Point (POINTX1)
     * |        |          |For B-163 or K-163, POINTX1 is stored in CRPT_ECC_X1_00~CRPT_ECC_X1_05
     * |        |          |For B-233 or K-233, POINTX1 is stored in CRPT_ECC_X1_00~CRPT_ECC_X1_07
     * |        |          |For B-283 or K-283, POINTX1 is stored in CRPT_ECC_X1_00~CRPT_ECC_X1_08
     * |        |          |For B-409 or K-409, POINTX1 is stored in CRPT_ECC_X1_00~CRPT_ECC_X1_12
     * |        |          |For B-571 or K-571, POINTX1 is stored in CRPT_ECC_X1_00~CRPT_ECC_X1_17
     * |        |          |For P-192, POINTX1 is stored in CRPT_ECC_X1_00~CRPT_ECC_X1_05
     * |        |          |For P-224, POINTX1 is stored in CRPT_ECC_X1_00~CRPT_ECC_X1_06
     * |        |          |For P-256, POINTX1 is stored in CRPT_ECC_X1_00~CRPT_ECC_X1_07
     * |        |          |For P-384, POINTX1 is stored in CRPT_ECC_X1_00~CRPT_ECC_X1_11
     * |        |          |For P-521, POINTX1 is stored in CRPT_ECC_X1_00~CRPT_ECC_X1_16
     * @var CRPT_T::ECC_Y1[18]
     * Offset: 0x850 ~ 0x894  ECC The Y-coordinate word 0 ~ 17 of the first point
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |POINTY1   |ECC the Y-coordinate Value of the First Point (POINTY1)
     * |        |          |For B-163 or K-163, POINTY1 is stored in CRPT_ECC_Y1_00~CRPT_ECC_Y1_05
     * |        |          |For B-233 or K-233, POINTY1 is stored in CRPT_ECC_Y1_00~CRPT_ECC_Y1_07
     * |        |          |For B-283 or K-283, POINTY1 is stored in CRPT_ECC_Y1_00~CRPT_ECC_Y1_08
     * |        |          |For B-409 or K-409, POINTY1 is stored in CRPT_ECC_Y1_00~CRPT_ECC_Y1_12
     * |        |          |For B-571 or K-571, POINTY1 is stored in CRPT_ECC_Y1_00~CRPT_ECC_Y1_17
     * |        |          |For P-192, POINTY1 is stored in CRPT_ECC_Y1_00~CRPT_ECC_Y1_05
     * |        |          |For P-224, POINTY1 is stored in CRPT_ECC_Y1_00~CRPT_ECC_Y1_06
     * |        |          |For P-256, POINTY1 is stored in CRPT_ECC_Y1_00~CRPT_ECC_Y1_07
     * |        |          |For P-384, POINTY1 is stored in CRPT_ECC_Y1_00~CRPT_ECC_Y1_11
     * |        |          |For P-521, POINTY1 is stored in CRPT_ECC_Y1_00~CRPT_ECC_Y1_16
     * @var CRPT_T::ECC_X2[18]
     * Offset: 0x898 ~ 0x8DC  ECC The X-coordinate word 0 ~ 17 of the second point
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |POINTX2   |ECC the x-coordinate Value of the Second Point (POINTX2)
     * |        |          |For B-163 or K-163, POINTX2 is stored in CRPT_ECC_X2_00~CRPT_ECC_X2_05
     * |        |          |For B-233 or K-233, POINTX2 is stored in CRPT_ECC_X2_00~CRPT_ECC_X2_07
     * |        |          |For B-283 or K-283, POINTX2 is stored in CRPT_ECC_X2_00~CRPT_ECC_X2_08
     * |        |          |For B-409 or K-409, POINTX2 is stored in CRPT_ECC_X2_00~CRPT_ECC_X2_12
     * |        |          |For B-571 or K-571, POINTX2 is stored in CRPT_ECC_X2_00~CRPT_ECC_X2_17
     * |        |          |For P-192, POINTX2 is stored in CRPT_ECC_X2_00~CRPT_ECC_X2_05
     * |        |          |For P-224, POINTX2 is stored in CRPT_ECC_X2_00~CRPT_ECC_X2_06
     * |        |          |For P-256, POINTX2 is stored in CRPT_ECC_X2_00~CRPT_ECC_X2_07
     * |        |          |For P-384, POINTX2 is stored in CRPT_ECC_X2_00~CRPT_ECC_X2_11
     * |        |          |For P-521, POINTX2 is stored in CRPT_ECC_X2_00~CRPT_ECC_X2_16
     * @var CRPT_T::ECC_Y2[18]
     * Offset: 0x8E0 ~ 0x924  ECC The Y-coordinate word 0 ~ 17 of the second point
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |POINTY2   |ECC the Y-coordinate Value of the Second Point (POINTY2)
     * |        |          |For B-163 or K-163, POINTY2 is stored in CRPT_ECC_Y2_00~CRPT_ECC_Y2_05
     * |        |          |For B-233 or K-233, POINTY2 is stored in CRPT_ECC_Y2_00~CRPT_ECC_Y2_07
     * |        |          |For B-283 or K-283, POINTY2 is stored in CRPT_ECC_Y2_00~CRPT_ECC_Y2_08
     * |        |          |For B-409 or K-409, POINTY2 is stored in CRPT_ECC_Y2_00~CRPT_ECC_Y2_12
     * |        |          |For B-571 or K-571, POINTY2 is stored in CRPT_ECC_Y2_00~CRPT_ECC_Y2_17
     * |        |          |For P-192, POINTY2 is stored in CRPT_ECC_Y2_00~CRPT_ECC_Y2_05
     * |        |          |For P-224, POINTY2 is stored in CRPT_ECC_Y2_00~CRPT_ECC_Y2_06
     * |        |          |For P-256, POINTY2 is stored in CRPT_ECC_Y2_00~CRPT_ECC_Y2_07
     * |        |          |For P-384, POINTY2 is stored in CRPT_ECC_Y2_00~CRPT_ECC_Y2_11
     * |        |          |For P-521, POINTY2 is stored in CRPT_ECC_Y2_00~CRPT_ECC_Y2_16
     * @var CRPT_T::ECC_A[18]
     * Offset: 0x928 ~ 0x96C  ECC The parameter CURVEA word 0 ~ 17 of elliptic curve
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CURVEA    |ECC the Parameter CURVEA Value of Elliptic Curve (CURVEA)
     * |        |          |The formula of elliptic curve is y2=x3+CURVEA*x+CURVEB in GF(p) and y2+x*y=x3+CURVEA*x2+CURVEB in GF(2^m).
     * |        |          |For B-163 or K-163, CURVEA is stored in CRPT_ECC_A_00~CRPT_ECC_A_05
     * |        |          |For B-233 or K-233, CURVEA is stored in CRPT_ECC_A_00~CRPT_ECC_A_07
     * |        |          |For B-283 or K-283, CURVEA is stored in CRPT_ECC_A_00~CRPT_ECC_A_08
     * |        |          |For B-409 or K-409, CURVEA is stored in CRPT_ECC_A_00~CRPT_ECC_A_12
     * |        |          |For B-571 or K-571, CURVEA is stored in CRPT_ECC_A_00~CRPT_ECC_A_17
     * |        |          |For P-192, CURVEA is stored in CRPT_ECC_A_00~CRPT_ECC_A_05
     * |        |          |For P-224, CURVEA is stored in CRPT_ECC_A_00~CRPT_ECC_A_06
     * |        |          |For P-256, CURVEA is stored in CRPT_ECC_A_00~CRPT_ECC_A_07
     * |        |          |For P-384, CURVEA is stored in CRPT_ECC_A_00~CRPT_ECC_A_11
     * |        |          |For P-521, CURVEA is stored in CRPT_ECC_A_00~CRPT_ECC_A_16
     * @var CRPT_T::ECC_B[18]
     * Offset: 0x970 ~ 0x9B4  ECC The parameter CURVEB word 0 ~ 17 of elliptic curve
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CURVEB    |ECC the Parameter CURVEB Value of Elliptic Curve (CURVEA)
     * |        |          |The formula of elliptic curve is y2=x3+CURVEA*x+CURVEB in GF(p) and y2+x*y=x3+CURVEA*x2+CURVEB in GF(2^m).
     * |        |          |For B-163 or K-163, CURVEB is stored in CRPT_ECC_B_00~CRPT_ECC_B_05
     * |        |          |For B-233 or K-233, CURVEB is stored in CRPT_ECC_B_00~CRPT_ECC_B_07
     * |        |          |For B-283 or K-283, CURVEB is stored in CRPT_ECC_B_00~CRPT_ECC_B_08
     * |        |          |For B-409 or K-409, CURVEB is stored in CRPT_ECC_B_00~CRPT_ECC_B_12
     * |        |          |For B-521 or K-521, CURVEB is stored in CRPT_ECC_B_00~CRPT_ECC_B_17
     * |        |          |For P-192, CURVEB is stored in CRPT_ECC_B_00~CRPT_ECC_B_05
     * |        |          |For P-224, CURVEB is stored in CRPT_ECC_B_00~CRPT_ECC_B_06
     * |        |          |For P-256, CURVEB is stored in CRPT_ECC_B_00~CRPT_ECC_B_07
     * |        |          |For P-384, CURVEB is stored in CRPT_ECC_B_00~CRPT_ECC_B_11
     * |        |          |For P-521, CURVEB is stored in CRPT_ECC_B_00~CRPT_ECC_B_16
     * @var CRPT_T::ECC_N[18]
     * Offset: 0x9B8 ~ 0x9FC  ECC The parameter CURVEN word 0 ~ 17 of elliptic curve
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |CURVEN    |ECC the Parameter CURVEN Value of Elliptic Curve (CURVEN)
     * |        |          |In GF(p), CURVEN is the prime p.
     * |        |          |In GF(2^m), CURVEN is the irreducible polynomial.
     * |        |          |For B-163 or K-163, CURVEN is stored in CRPT_ECC_N_00~CRPT_ECC_N_05
     * |        |          |For B-233 or K-233, CURVEN is stored in CRPT_ECC_N_00~CRPT_ECC_N_07
     * |        |          |For B-283 or K-283, CURVEN is stored in CRPT_ECC_N_00~CRPT_ECC_N_08
     * |        |          |For B-409 or K-409, CURVEN is stored in CRPT_ECC_N_00~CRPT_ECC_N_12
     * |        |          |For B-571 or K-571, CURVEN is stored in CRPT_ECC_N_00~CRPT_ECC_N_17
     * |        |          |For P-192, CURVEN is stored in CRPT_ECC_N_00~CRPT_ECC_N_05
     * |        |          |For P-224, CURVEN is stored in CRPT_ECC_N_00~CRPT_ECC_N_06
     * |        |          |For P-256, CURVEN is stored in CRPT_ECC_N_00~CRPT_ECC_N_07
     * |        |          |For P-384, CURVEN is stored in CRPT_ECC_N_00~CRPT_ECC_N_11
     * |        |          |For P-521, CURVEN is stored in CRPT_ECC_N_00~CRPT_ECC_N_16
     * @var CRPT_T::ECC_K[18]
     * Offset: 0xA00 ~ 0xA44  ECC The scalar SCALARK word0 of point multiplication
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SCALARK   |ECC the Scalar SCALARK Value of Point Multiplication(SCALARK)
     * |        |          |Because the SCALARK usually stores the private key, ECC accelerator do not allow to read the register SCALARK.
     * |        |          |For B-163 or K-163, SCALARK is stored in CRPT_ECC_K_00~CRPT_ECC_K_05
     * |        |          |For B-233 or K-233, SCALARK is stored in CRPT_ECC_K_00~CRPT_ECC_K_07
     * |        |          |For B-283 or K-283, SCALARK is stored in CRPT_ECC_K_00~CRPT_ECC_K_08
     * |        |          |For B-409 or K-409, SCALARK is stored in CRPT_ECC_K_00~CRPT_ECC_K_12
     * |        |          |For B-571 or K-571, SCALARK is stored in CRPT_ECC_K_00~CRPT_ECC_K_17
     * |        |          |For P-192, SCALARK is stored in CRPT_ECC_K_00~CRPT_ECC_K_05
     * |        |          |For P-224, SCALARK is stored in CRPT_ECC_K_00~CRPT_ECC_K_06
     * |        |          |For P-256, SCALARK is stored in CRPT_ECC_K_00~CRPT_ECC_K_07
     * |        |          |For P-384, SCALARK is stored in CRPT_ECC_K_00~CRPT_ECC_K_11
     * |        |          |For P-521, SCALARK is stored in CRPT_ECC_K_00~CRPT_ECC_K_16
     * @var CRPT_T::ECC_SADDR
     * Offset: 0xA48  ECC DMA Source Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |SADDR     |ECC DMA Source Address
     * |        |          |The ECC accelerator supports DMA function to transfer the DATA and PARAMETER between
     * |        |          |SRAM memory space and ECC accelerator. The SADDR keeps the source address of the data
     * |        |          |buffer where the source text is stored. Based on the source address, the ECC accelerator
     * |        |          |can read the DATA and PARAMETER from SRAM memory space and do ECC operation. The start
     * |        |          |of source address should be located at word boundary. That is, bit 1 and 0 of SADDR are
     * |        |          |ignored. SADDR can be read and written. In DMA mode, software must update the CRPT_ECC_SADDR
     * |        |          |before triggering START.
     * @var CRPT_T::ECC_DADDR
     * Offset: 0xA4C  ECC DMA Destination Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |DADDR     |ECC DMA Destination Address
     * |        |          |The ECC accelerator supports DMA function to transfer the DATA and PARAMETER between system memory and ECC accelerator
     * |        |          |The DADDR keeps the destination address of the data buffer where output data of ECC engine will be stored
     * |        |          |Based on the destination address, the ECC accelerator can write the result data back to system memory after the ECC operation is finished
     * |        |          |The start of destination address should be located at word boundary
     * |        |          |That is, bit 1 and 0 of DADDR are ignored
     * |        |          |DADDR can be read and written
     * |        |          |In DMA mode, software must update the CRPT_ECC_DADDR before triggering START
     * @var CRPT_T::ECC_STARTREG
     * Offset: 0xA50  ECC Starting Address of Updated Registers
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |STARTREG  |ECC Starting Address of Updated Registers
     * |        |          |The address of the updated registers that DMA feeds the first data or parameter to ECC engine
     * |        |          |When ECC engine is active, ECC accelerator does not allow users to modify STARTREG
     * |        |          |For example, we want to updated input data from register CRPT_ECC POINTX1
     * |        |          |Thus, the value of STARTREG is 0x808.
     * @var CRPT_T::ECC_WORDCNT
     * Offset: 0xA54  ECC DMA Word Count
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[31:0]  |WORDCNT   |ECC DMA Word Count
     * |        |          |The CRPT_ECC_WORDCNT keeps the word count of source data that is for the required input data of ECC accelerator with various operations in DMA mode
     * |        |          |Although CRPT_ECC_WORDCNT is 32-bit, the maximum of word count in ECC accelerator is 144 words
     * |        |          |CRPT_ECC_WORDCNT can be read and written
     */
    __IO uint32_t INTEN;                 /*!< [0x0000] Crypto Interrupt Enable Control Register                         */
    __IO uint32_t INTSTS;                /*!< [0x0004] Crypto Interrupt Flag                                            */
    __IO uint32_t PRNG_CTL;              /*!< [0x0008] PRNG Control Register                                            */
    __O  uint32_t PRNG_SEED;             /*!< [0x000c] Seed for PRNG                                                    */
    __I  uint32_t PRNG_KEY[8];           /*!< [0x0010] ~ [0x002c] PRNG Generated Key0 ~ Key7                            */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[8];
    /// @endcond //HIDDEN_SYMBOLS
    __I  uint32_t AES_FDBCK[4];          /*!< [0x0050] ~ [0x005c] AES Engine Output Feedback Data after Cryptographic Operation     */
    __I  uint32_t TDES_FDBCKH;           /*!< [0x0060] TDES/DES Engine Output Feedback High Word Data after Cryptographic Operation */
    __I  uint32_t TDES_FDBCKL;           /*!< [0x0064] TDES/DES Engine Output Feedback Low Word Data after Cryptographic Operation  */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE1[38];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t AES_CTL;               /*!< [0x0100] AES Control Register                                             */
    __I  uint32_t AES_STS;               /*!< [0x0104] AES Engine Flag                                                  */
    __IO uint32_t AES_DATIN;             /*!< [0x0108] AES Engine Data Input Port Register                              */
    __I  uint32_t AES_DATOUT;            /*!< [0x010c] AES Engine Data Output Port Register                             */
    __IO uint32_t AES0_KEY[8];           /*!< [0x0110] ~ [0x012c] AES Key Word 0~7 Register for Channel 0               */
    __IO uint32_t AES0_IV[4];            /*!< [0x0130] ~ [0x013c] AES Initial Vector Word 0 ~ 3 Register for Channel 0  */
    __IO uint32_t AES0_SADDR;            /*!< [0x0140] AES DMA Source Address Register for Channel 0                    */
    __IO uint32_t AES0_DADDR;            /*!< [0x0144] AES DMA Destination Address Register for Channel 0               */
    __IO uint32_t AES0_CNT;              /*!< [0x0148] AES Byte Count Register for Channel 0                            */
    __IO uint32_t AES1_KEY[8];           /*!< [0x014c] ~ [0x0168] AES Key Word 0~7 Register for Channel 1               */
    __IO uint32_t AES1_IV[4];            /*!< [0x016c] ~ [0x0178] AES Initial Vector Word 0~3 Register for Channel 1    */
    __IO uint32_t AES1_SADDR;            /*!< [0x017c] AES DMA Source Address Register for Channel 1                    */
    __IO uint32_t AES1_DADDR;            /*!< [0x0180] AES DMA Destination Address Register for Channel 1               */
    __IO uint32_t AES1_CNT;              /*!< [0x0184] AES Byte Count Register for Channel 1                            */
    __IO uint32_t AES2_KEY[8];           /*!< [0x0188] ~ [0x01a4] AES Key Word 0~7 Register for Channel 2               */
    __IO uint32_t AES2_IV[4];            /*!< [0x01a8] ~ [0x01b4] AES Initial Vector Word 0~3 Register for Channel 2    */
    __IO uint32_t AES2_SADDR;            /*!< [0x01b8] AES DMA Source Address Register for Channel 2                    */
    __IO uint32_t AES2_DADDR;            /*!< [0x01bc] AES DMA Destination Address Register for Channel 2               */
    __IO uint32_t AES2_CNT;              /*!< [0x01c0] AES Byte Count Register for Channel 2                            */
    __IO uint32_t AES3_KEY[8];           /*!< [0x01c4] ~ [0x01e0] AES Key Word 0~7 Register for Channel 3               */
    __IO uint32_t AES3_IV[4];            /*!< [0x01e4] ~ [0x01f0] AES Initial Vector Word 0~3 Register for Channel 3    */
    __IO uint32_t AES3_SADDR;            /*!< [0x01f4] AES DMA Source Address Register for Channel 3                    */
    __IO uint32_t AES3_DADDR;            /*!< [0x01f8] AES DMA Destination Address Register for Channel 3               */
    __IO uint32_t AES3_CNT;              /*!< [0x01fc] AES Byte Count Register for Channel 3                            */
    __IO uint32_t TDES_CTL;              /*!< [0x0200] TDES/DES Control Register                                        */
    __I  uint32_t TDES_STS;              /*!< [0x0204] TDES/DES Engine Flag                                             */
    __IO uint32_t TDES0_KEY1H;           /*!< [0x0208] TDES/DES Key 1 High Word Register for Channel 0                  */
    __IO uint32_t TDES0_KEY1L;           /*!< [0x020c] TDES/DES Key 1 Low Word Register for Channel 0                   */
    __IO uint32_t TDES0_KEY2H;           /*!< [0x0210] TDES Key 2 High Word Register for Channel 0                      */
    __IO uint32_t TDES0_KEY2L;           /*!< [0x0214] TDES Key 2 Low Word Register for Channel 0                       */
    __IO uint32_t TDES0_KEY3H;           /*!< [0x0218] TDES Key 3 High Word Register for Channel 0                      */
    __IO uint32_t TDES0_KEY3L;           /*!< [0x021c] TDES Key 3 Low Word Register for Channel 0                       */
    __IO uint32_t TDES0_IVH;             /*!< [0x0220] TDES/DES Initial Vector High Word Register for Channel 0         */
    __IO uint32_t TDES0_IVL;             /*!< [0x0224] TDES/DES Initial Vector Low Word Register for Channel 0          */
    __IO uint32_t TDES0_SA;              /*!< [0x0228] TDES/DES DMA Source Address Register for Channel 0               */
    __IO uint32_t TDES0_DA;              /*!< [0x022c] TDES/DES DMA Destination Address Register for Channel 0          */
    __IO uint32_t TDES0_CNT;             /*!< [0x0230] TDES/DES Byte Count Register for Channel 0                       */
    __IO uint32_t TDES_DATIN;            /*!< [0x0234] TDES/DES Engine Input data Word Register                         */
    __I  uint32_t TDES_DATOUT;           /*!< [0x0238] TDES/DES Engine Output data Word Register                        */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE2[3];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t TDES1_KEY1H;           /*!< [0x0248] TDES/DES Key 1 High Word Register for Channel 1                  */
    __IO uint32_t TDES1_KEY1L;           /*!< [0x024c] TDES/DES Key 1 Low Word Register for Channel 1                   */
    __IO uint32_t TDES1_KEY2H;           /*!< [0x0250] TDES Key 2 High Word Register for Channel 1                      */
    __IO uint32_t TDES1_KEY2L;           /*!< [0x0254] TDES Key 2 Low Word Register for Channel 1                       */
    __IO uint32_t TDES1_KEY3H;           /*!< [0x0258] TDES Key 3 High Word Register for Channel 1                      */
    __IO uint32_t TDES1_KEY3L;           /*!< [0x025c] TDES Key 3 Low Word Register for Channel 1                       */
    __IO uint32_t TDES1_IVH;             /*!< [0x0260] TDES/DES Initial Vector High Word Register for Channel 1         */
    __IO uint32_t TDES1_IVL;             /*!< [0x0264] TDES/DES Initial Vector Low Word Register for Channel 1          */
    __IO uint32_t TDES1_SA;              /*!< [0x0268] TDES/DES DMA Source Address Register for Channel 1               */
    __IO uint32_t TDES1_DA;              /*!< [0x026c] TDES/DES DMA Destination Address Register for Channel 1          */
    __IO uint32_t TDES1_CNT;             /*!< [0x0270] TDES/DES Byte Count Register for Channel 1                       */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE3[5];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t TDES2_KEY1H;           /*!< [0x0288] TDES/DES Key 1 High Word Register for Channel 2                  */
    __IO uint32_t TDES2_KEY1L;           /*!< [0x028c] TDES/DES Key 1 Low Word Register for Channel 2                   */
    __IO uint32_t TDES2_KEY2H;           /*!< [0x0290] TDES Key 2 High Word Register for Channel 2                      */
    __IO uint32_t TDES2_KEY2L;           /*!< [0x0294] TDES Key 2 Low Word Register for Channel 2                       */
    __IO uint32_t TDES2_KEY3H;           /*!< [0x0298] TDES Key 3 High Word Register for Channel 2                      */
    __IO uint32_t TDES2_KEY3L;           /*!< [0x029c] TDES Key 3 Low Word Register for Channel 2                       */
    __IO uint32_t TDES2_IVH;             /*!< [0x02a0] TDES/DES Initial Vector High Word Register for Channel 2         */
    __IO uint32_t TDES2_IVL;             /*!< [0x02a4] TDES/DES Initial Vector Low Word Register for Channel 2          */
    __IO uint32_t TDES2_SA;              /*!< [0x02a8] TDES/DES DMA Source Address Register for Channel 2               */
    __IO uint32_t TDES2_DA;              /*!< [0x02ac] TDES/DES DMA Destination Address Register for Channel 2          */
    __IO uint32_t TDES2_CNT;             /*!< [0x02b0] TDES/DES Byte Count Register for Channel 2                       */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE4[5];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t TDES3_KEY1H;           /*!< [0x02c8] TDES/DES Key 1 High Word Register for Channel 3                  */
    __IO uint32_t TDES3_KEY1L;           /*!< [0x02cc] TDES/DES Key 1 Low Word Register for Channel 3                   */
    __IO uint32_t TDES3_KEY2H;           /*!< [0x02d0] TDES Key 2 High Word Register for Channel 3                      */
    __IO uint32_t TDES3_KEY2L;           /*!< [0x02d4] TDES Key 2 Low Word Register for Channel 3                       */
    __IO uint32_t TDES3_KEY3H;           /*!< [0x02d8] TDES Key 3 High Word Register for Channel 3                      */
    __IO uint32_t TDES3_KEY3L;           /*!< [0x02dc] TDES Key 3 Low Word Register for Channel 3                       */
    __IO uint32_t TDES3_IVH;             /*!< [0x02e0] TDES/DES Initial Vector High Word Register for Channel 3         */
    __IO uint32_t TDES3_IVL;             /*!< [0x02e4] TDES/DES Initial Vector Low Word Register for Channel 3          */
    __IO uint32_t TDES3_SA;              /*!< [0x02e8] TDES/DES DMA Source Address Register for Channel 3               */
    __IO uint32_t TDES3_DA;              /*!< [0x02ec] TDES/DES DMA Destination Address Register for Channel 3          */
    __IO uint32_t TDES3_CNT;             /*!< [0x02f0] TDES/DES Byte Count Register for Channel 3                       */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE5[3];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t HMAC_CTL;              /*!< [0x0300] SHA/HMAC Control Register                                        */
    __I  uint32_t HMAC_STS;              /*!< [0x0304] SHA/HMAC Status Flag                                             */
    __I  uint32_t HMAC_DGST[16];         /*!< [0x0308] ~ [0x0344] SHA/HMAC Digest Message 0~15                          */
    __IO uint32_t HMAC_KEYCNT;           /*!< [0x0348] SHA/HMAC Key Byte Count Register                                 */
    __IO uint32_t HMAC_SADDR;            /*!< [0x034c] SHA/HMAC DMA Source Address Register                             */
    __IO uint32_t HMAC_DMACNT;           /*!< [0x0350] SHA/HMAC Byte Count Register                                     */
    __IO uint32_t HMAC_DATIN;            /*!< [0x0354] SHA/HMAC Engine Non-DMA Mode Data Input Port Register            */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE6[298];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t ECC_CTL;               /*!< [0x0800] ECC Control Register                                             */
    __I  uint32_t ECC_STS;               /*!< [0x0804] ECC Status Register                                              */
    __IO uint32_t ECC_X1[18];            /*!< [0x0808] ~ [0x084c] ECC The X-coordinate word 0~17 of the first point     */
    __IO uint32_t ECC_Y1[18];            /*!< [0x0850] ~ [0x0894] ECC The Y-coordinate word 0~17 of the first point     */
    __IO uint32_t ECC_X2[18];            /*!< [0x0898] ~ [0x08dc] ECC The X-coordinate word 0~17 of the second point    */
    __IO uint32_t ECC_Y2[18];            /*!< [0x08e0] ~ [0x0924] ECC The Y-coordinate word 0~17 of the second point    */
    __IO uint32_t ECC_A[18];             /*!< [0x0928] ~ [0x096c] ECC The parameter CURVEA word 0~17 of elliptic curve  */
    __IO uint32_t ECC_B[18];             /*!< [0x0970] ~ [0x09b4] ECC The parameter CURVEB word 0~17 of elliptic curve  */
    __IO uint32_t ECC_N[18];             /*!< [0x09b8] ~ [0x09fc] ECC The parameter CURVEN word 0~17 of elliptic curve  */
    __O  uint32_t ECC_K[18];             /*!< [0x0a00] ~ [0x0a44] ECC The scalar SCALARK word 0~17 of point multiplication */
    __IO uint32_t ECC_SADDR;             /*!< [0x0a48] ECC DMA Source Address Register                                  */
    __IO uint32_t ECC_DADDR;             /*!< [0x0a4c] ECC DMA Destination Address Register                             */
    __IO uint32_t ECC_STARTREG;          /*!< [0x0a50] ECC Starting Address of Updated Registers                        */
    __IO uint32_t ECC_WORDCNT;           /*!< [0x0a54] ECC DMA Word Count                                               */

} CRPT_T;

/**
    @addtogroup CRPT_CONST CRPT Bit Field Definition
    Constant Definitions for CRPT Controller
@{ */

#define CRPT_INTEN_AESIEN_Pos            (0)                                               /*!< CRPT_T::INTEN: AESIEN Position         */
#define CRPT_INTEN_AESIEN_Msk            (0x1ul << CRPT_INTEN_AESIEN_Pos)                  /*!< CRPT_T::INTEN: AESIEN Mask             */

#define CRPT_INTEN_AESEIEN_Pos           (1)                                               /*!< CRPT_T::INTEN: AESEIEN Position        */
#define CRPT_INTEN_AESEIEN_Msk           (0x1ul << CRPT_INTEN_AESEIEN_Pos)                 /*!< CRPT_T::INTEN: AESEIEN Mask            */

#define CRPT_INTEN_TDESIEN_Pos           (8)                                               /*!< CRPT_T::INTEN: TDESIEN Position        */
#define CRPT_INTEN_TDESIEN_Msk           (0x1ul << CRPT_INTEN_TDESIEN_Pos)                 /*!< CRPT_T::INTEN: TDESIEN Mask            */

#define CRPT_INTEN_TDESEIEN_Pos          (9)                                               /*!< CRPT_T::INTEN: TDESEIEN Position       */
#define CRPT_INTEN_TDESEIEN_Msk          (0x1ul << CRPT_INTEN_TDESEIEN_Pos)                /*!< CRPT_T::INTEN: TDESEIEN Mask           */

#define CRPT_INTEN_PRNGIEN_Pos           (16)                                              /*!< CRPT_T::INTEN: PRNGIEN Position        */
#define CRPT_INTEN_PRNGIEN_Msk           (0x1ul << CRPT_INTEN_PRNGIEN_Pos)                 /*!< CRPT_T::INTEN: PRNGIEN Mask            */

#define CRPT_INTEN_ECCIEN_Pos            (22)                                              /*!< CRPT_T::INTEN: ECCIEN Position         */
#define CRPT_INTEN_ECCIEN_Msk            (0x1ul << CRPT_INTEN_ECCIEN_Pos)                  /*!< CRPT_T::INTEN: ECCIEN Mask             */

#define CRPT_INTEN_ECCEIEN_Pos           (23)                                              /*!< CRPT_T::INTEN: ECCEIEN Position        */
#define CRPT_INTEN_ECCEIEN_Msk           (0x1ul << CRPT_INTEN_ECCEIEN_Pos)                 /*!< CRPT_T::INTEN: ECCEIEN Mask            */

#define CRPT_INTEN_HMACIEN_Pos           (24)                                              /*!< CRPT_T::INTEN: HMACIEN Position        */
#define CRPT_INTEN_HMACIEN_Msk           (0x1ul << CRPT_INTEN_HMACIEN_Pos)                 /*!< CRPT_T::INTEN: HMACIEN Mask            */

#define CRPT_INTEN_HMACEIEN_Pos          (25)                                              /*!< CRPT_T::INTEN: HMACEIEN Position       */
#define CRPT_INTEN_HMACEIEN_Msk          (0x1ul << CRPT_INTEN_HMACEIEN_Pos)                /*!< CRPT_T::INTEN: HMACEIEN Mask           */

#define CRPT_INTSTS_AESIF_Pos            (0)                                               /*!< CRPT_T::INTSTS: AESIF Position         */
#define CRPT_INTSTS_AESIF_Msk            (0x1ul << CRPT_INTSTS_AESIF_Pos)                  /*!< CRPT_T::INTSTS: AESIF Mask             */

#define CRPT_INTSTS_AESEIF_Pos           (1)                                               /*!< CRPT_T::INTSTS: AESEIF Position        */
#define CRPT_INTSTS_AESEIF_Msk           (0x1ul << CRPT_INTSTS_AESEIF_Pos)                 /*!< CRPT_T::INTSTS: AESEIF Mask            */

#define CRPT_INTSTS_TDESIF_Pos           (8)                                               /*!< CRPT_T::INTSTS: TDESIF Position        */
#define CRPT_INTSTS_TDESIF_Msk           (0x1ul << CRPT_INTSTS_TDESIF_Pos)                 /*!< CRPT_T::INTSTS: TDESIF Mask            */

#define CRPT_INTSTS_TDESEIF_Pos          (9)                                               /*!< CRPT_T::INTSTS: TDESEIF Position       */
#define CRPT_INTSTS_TDESEIF_Msk          (0x1ul << CRPT_INTSTS_TDESEIF_Pos)                /*!< CRPT_T::INTSTS: TDESEIF Mask           */

#define CRPT_INTSTS_PRNGIF_Pos           (16)                                              /*!< CRPT_T::INTSTS: PRNGIF Position        */
#define CRPT_INTSTS_PRNGIF_Msk           (0x1ul << CRPT_INTSTS_PRNGIF_Pos)                 /*!< CRPT_T::INTSTS: PRNGIF Mask            */

#define CRPT_INTSTS_ECCIF_Pos            (22)                                              /*!< CRPT_T::INTSTS: ECCIF Position         */
#define CRPT_INTSTS_ECCIF_Msk            (0x1ul << CRPT_INTSTS_ECCIF_Pos)                  /*!< CRPT_T::INTSTS: ECCIF Mask             */

#define CRPT_INTSTS_ECCEIF_Pos           (23)                                              /*!< CRPT_T::INTSTS: ECCEIF Position        */
#define CRPT_INTSTS_ECCEIF_Msk           (0x1ul << CRPT_INTSTS_ECCEIF_Pos)                 /*!< CRPT_T::INTSTS: ECCEIF Mask            */

#define CRPT_INTSTS_HMACIF_Pos           (24)                                              /*!< CRPT_T::INTSTS: HMACIF Position        */
#define CRPT_INTSTS_HMACIF_Msk           (0x1ul << CRPT_INTSTS_HMACIF_Pos)                 /*!< CRPT_T::INTSTS: HMACIF Mask            */

#define CRPT_INTSTS_HMACEIF_Pos          (25)                                              /*!< CRPT_T::INTSTS: HMACEIF Position       */
#define CRPT_INTSTS_HMACEIF_Msk          (0x1ul << CRPT_INTSTS_HMACEIF_Pos)                /*!< CRPT_T::INTSTS: HMACEIF Mask           */

#define CRPT_PRNG_CTL_START_Pos          (0)                                               /*!< CRPT_T::PRNG_CTL: START Position       */
#define CRPT_PRNG_CTL_START_Msk          (0x1ul << CRPT_PRNG_CTL_START_Pos)                /*!< CRPT_T::PRNG_CTL: START Mask           */

#define CRPT_PRNG_CTL_SEEDRLD_Pos        (1)                                               /*!< CRPT_T::PRNG_CTL: SEEDRLD Position     */
#define CRPT_PRNG_CTL_SEEDRLD_Msk        (0x1ul << CRPT_PRNG_CTL_SEEDRLD_Pos)              /*!< CRPT_T::PRNG_CTL: SEEDRLD Mask         */

#define CRPT_PRNG_CTL_KEYSZ_Pos          (2)                                               /*!< CRPT_T::PRNG_CTL: KEYSZ Position       */
#define CRPT_PRNG_CTL_KEYSZ_Msk          (0x3ul << CRPT_PRNG_CTL_KEYSZ_Pos)                /*!< CRPT_T::PRNG_CTL: KEYSZ Mask           */

#define CRPT_PRNG_CTL_BUSY_Pos           (8)                                               /*!< CRPT_T::PRNG_CTL: BUSY Position        */
#define CRPT_PRNG_CTL_BUSY_Msk           (0x1ul << CRPT_PRNG_CTL_BUSY_Pos)                 /*!< CRPT_T::PRNG_CTL: BUSY Mask            */

#define CRPT_PRNG_SEED_SEED_Pos          (0)                                               /*!< CRPT_T::PRNG_SEED: SEED Position       */
#define CRPT_PRNG_SEED_SEED_Msk          (0xfffffffful << CRPT_PRNG_SEED_SEED_Pos)         /*!< CRPT_T::PRNG_SEED: SEED Mask           */

#define CRPT_PRNG_KEYx_KEY_Pos           (0)                                               /*!< CRPT_T::PRNG_KEY[8]: KEY Position      */
#define CRPT_PRNG_KEYx_KEY_Msk           (0xfffffffful << CRPT_PRNG_KEYx_KEY_Pos)          /*!< CRPT_T::PRNG_KEY[8]: KEY Mask          */

#define CRPT_AES_FDBCKx_FDBCK_Pos        (0)                                               /*!< CRPT_T::AES_FDBCK[4]: FDBCK Position   */
#define CRPT_AES_FDBCKx_FDBCK_Msk        (0xfffffffful << CRPT_AES_FDBCKx_FDBCK_Pos)       /*!< CRPT_T::AES_FDBCK[4]: FDBCK Mask       */

#define CRPT_TDES_FDBCKH_FDBCK_Pos       (0)                                               /*!< CRPT_T::TDES_FDBCKH: FDBCK Position    */
#define CRPT_TDES_FDBCKH_FDBCK_Msk       (0xfffffffful << CRPT_TDES_FDBCKH_FDBCK_Pos)      /*!< CRPT_T::TDES_FDBCKH: FDBCK Mask        */

#define CRPT_TDES_FDBCKL_FDBCK_Pos       (0)                                               /*!< CRPT_T::TDES_FDBCKL: FDBCK Position    */
#define CRPT_TDES_FDBCKL_FDBCK_Msk       (0xfffffffful << CRPT_TDES_FDBCKL_FDBCK_Pos)      /*!< CRPT_T::TDES_FDBCKL: FDBCK Mask        */

#define CRPT_AES_CTL_START_Pos           (0)                                               /*!< CRPT_T::AES_CTL: START Position        */
#define CRPT_AES_CTL_START_Msk           (0x1ul << CRPT_AES_CTL_START_Pos)                 /*!< CRPT_T::AES_CTL: START Mask            */

#define CRPT_AES_CTL_STOP_Pos            (1)                                               /*!< CRPT_T::AES_CTL: STOP Position         */
#define CRPT_AES_CTL_STOP_Msk            (0x1ul << CRPT_AES_CTL_STOP_Pos)                  /*!< CRPT_T::AES_CTL: STOP Mask             */

#define CRPT_AES_CTL_KEYSZ_Pos           (2)                                               /*!< CRPT_T::AES_CTL: KEYSZ Position        */
#define CRPT_AES_CTL_KEYSZ_Msk           (0x3ul << CRPT_AES_CTL_KEYSZ_Pos)                 /*!< CRPT_T::AES_CTL: KEYSZ Mask            */

#define CRPT_AES_CTL_DMALAST_Pos         (5)                                               /*!< CRPT_T::AES_CTL: DMALAST Position      */
#define CRPT_AES_CTL_DMALAST_Msk         (0x1ul << CRPT_AES_CTL_DMALAST_Pos)               /*!< CRPT_T::AES_CTL: DMALAST Mask          */

#define CRPT_AES_CTL_DMACSCAD_Pos        (6)                                               /*!< CRPT_T::AES_CTL: DMACSCAD Position     */
#define CRPT_AES_CTL_DMACSCAD_Msk        (0x1ul << CRPT_AES_CTL_DMACSCAD_Pos)              /*!< CRPT_T::AES_CTL: DMACSCAD Mask         */

#define CRPT_AES_CTL_DMAEN_Pos           (7)                                               /*!< CRPT_T::AES_CTL: DMAEN Position        */
#define CRPT_AES_CTL_DMAEN_Msk           (0x1ul << CRPT_AES_CTL_DMAEN_Pos)                 /*!< CRPT_T::AES_CTL: DMAEN Mask            */

#define CRPT_AES_CTL_OPMODE_Pos          (8)                                               /*!< CRPT_T::AES_CTL: OPMODE Position       */
#define CRPT_AES_CTL_OPMODE_Msk          (0xfful << CRPT_AES_CTL_OPMODE_Pos)               /*!< CRPT_T::AES_CTL: OPMODE Mask           */

#define CRPT_AES_CTL_ENCRPT_Pos          (16)                                              /*!< CRPT_T::AES_CTL: ENCRPT Position       */
#define CRPT_AES_CTL_ENCRPT_Msk          (0x1ul << CRPT_AES_CTL_ENCRPT_Pos)                /*!< CRPT_T::AES_CTL: ENCRPT Mask           */

#define CRPT_AES_CTL_OUTSWAP_Pos         (22)                                              /*!< CRPT_T::AES_CTL: OUTSWAP Position      */
#define CRPT_AES_CTL_OUTSWAP_Msk         (0x1ul << CRPT_AES_CTL_OUTSWAP_Pos)               /*!< CRPT_T::AES_CTL: OUTSWAP Mask          */

#define CRPT_AES_CTL_INSWAP_Pos          (23)                                              /*!< CRPT_T::AES_CTL: INSWAP Position       */
#define CRPT_AES_CTL_INSWAP_Msk          (0x1ul << CRPT_AES_CTL_INSWAP_Pos)                /*!< CRPT_T::AES_CTL: INSWAP Mask           */

#define CRPT_AES_CTL_CHANNEL_Pos         (24)                                              /*!< CRPT_T::AES_CTL: CHANNEL Position      */
#define CRPT_AES_CTL_CHANNEL_Msk         (0x3ul << CRPT_AES_CTL_CHANNEL_Pos)               /*!< CRPT_T::AES_CTL: CHANNEL Mask          */

#define CRPT_AES_CTL_KEYUNPRT_Pos        (26)                                              /*!< CRPT_T::AES_CTL: KEYUNPRT Position     */
#define CRPT_AES_CTL_KEYUNPRT_Msk        (0x1ful << CRPT_AES_CTL_KEYUNPRT_Pos)             /*!< CRPT_T::AES_CTL: KEYUNPRT Mask         */

#define CRPT_AES_CTL_KEYPRT_Pos          (31)                                              /*!< CRPT_T::AES_CTL: KEYPRT Position       */
#define CRPT_AES_CTL_KEYPRT_Msk          (0x1ul << CRPT_AES_CTL_KEYPRT_Pos)                /*!< CRPT_T::AES_CTL: KEYPRT Mask           */

#define CRPT_AES_STS_BUSY_Pos            (0)                                               /*!< CRPT_T::AES_STS: BUSY Position         */
#define CRPT_AES_STS_BUSY_Msk            (0x1ul << CRPT_AES_STS_BUSY_Pos)                  /*!< CRPT_T::AES_STS: BUSY Mask             */

#define CRPT_AES_STS_INBUFEMPTY_Pos      (8)                                               /*!< CRPT_T::AES_STS: INBUFEMPTY Position   */
#define CRPT_AES_STS_INBUFEMPTY_Msk      (0x1ul << CRPT_AES_STS_INBUFEMPTY_Pos)            /*!< CRPT_T::AES_STS: INBUFEMPTY Mask       */

#define CRPT_AES_STS_INBUFFULL_Pos       (9)                                               /*!< CRPT_T::AES_STS: INBUFFULL Position    */
#define CRPT_AES_STS_INBUFFULL_Msk       (0x1ul << CRPT_AES_STS_INBUFFULL_Pos)             /*!< CRPT_T::AES_STS: INBUFFULL Mask        */

#define CRPT_AES_STS_INBUFERR_Pos        (10)                                              /*!< CRPT_T::AES_STS: INBUFERR Position     */
#define CRPT_AES_STS_INBUFERR_Msk        (0x1ul << CRPT_AES_STS_INBUFERR_Pos)              /*!< CRPT_T::AES_STS: INBUFERR Mask         */

#define CRPT_AES_STS_CNTERR_Pos          (12)                                              /*!< CRPT_T::AES_STS: CNTERR Position       */
#define CRPT_AES_STS_CNTERR_Msk          (0x1ul << CRPT_AES_STS_CNTERR_Pos)                /*!< CRPT_T::AES_STS: CNTERR Mask           */

#define CRPT_AES_STS_OUTBUFEMPTY_Pos     (16)                                              /*!< CRPT_T::AES_STS: OUTBUFEMPTY Position  */
#define CRPT_AES_STS_OUTBUFEMPTY_Msk     (0x1ul << CRPT_AES_STS_OUTBUFEMPTY_Pos)           /*!< CRPT_T::AES_STS: OUTBUFEMPTY Mask      */

#define CRPT_AES_STS_OUTBUFFULL_Pos      (17)                                              /*!< CRPT_T::AES_STS: OUTBUFFULL Position   */
#define CRPT_AES_STS_OUTBUFFULL_Msk      (0x1ul << CRPT_AES_STS_OUTBUFFULL_Pos)            /*!< CRPT_T::AES_STS: OUTBUFFULL Mask       */

#define CRPT_AES_STS_OUTBUFERR_Pos       (18)                                              /*!< CRPT_T::AES_STS: OUTBUFERR Position    */
#define CRPT_AES_STS_OUTBUFERR_Msk       (0x1ul << CRPT_AES_STS_OUTBUFERR_Pos)             /*!< CRPT_T::AES_STS: OUTBUFERR Mask        */

#define CRPT_AES_STS_BUSERR_Pos          (20)                                              /*!< CRPT_T::AES_STS: BUSERR Position       */
#define CRPT_AES_STS_BUSERR_Msk          (0x1ul << CRPT_AES_STS_BUSERR_Pos)                /*!< CRPT_T::AES_STS: BUSERR Mask           */

#define CRPT_AES_DATIN_DATIN_Pos         (0)                                               /*!< CRPT_T::AES_DATIN: DATIN Position      */
#define CRPT_AES_DATIN_DATIN_Msk         (0xfffffffful << CRPT_AES_DATIN_DATIN_Pos)        /*!< CRPT_T::AES_DATIN: DATIN Mask          */

#define CRPT_AES_DATOUT_DATOUT_Pos       (0)                                               /*!< CRPT_T::AES_DATOUT: DATOUT Position    */
#define CRPT_AES_DATOUT_DATOUT_Msk       (0xfffffffful << CRPT_AES_DATOUT_DATOUT_Pos)      /*!< CRPT_T::AES_DATOUT: DATOUT Mask        */

#define CRPT_AES0_KEYx_KEY_Pos           (0)                                               /*!< CRPT_T::AES0_KEY[8]: KEY Position      */
#define CRPT_AES0_KEYx_KEY_Msk           (0xfffffffful << CRPT_AES0_KEYx_KEY_Pos)          /*!< CRPT_T::AES0_KEY[8]: KEY Mask          */

#define CRPT_AES0_IVx_IV_Pos             (0)                                               /*!< CRPT_T::AES0_IV[4]: IV Position        */
#define CRPT_AES0_IVx_IV_Msk             (0xfffffffful << CRPT_AES0_IVx_IV_Pos)            /*!< CRPT_T::AES0_IV[4]: IV Mask            */

#define CRPT_AES0_SADDR_SADDR_Pos        (0)                                               /*!< CRPT_T::AES0_SADDR: SADDR Position     */
#define CRPT_AES0_SADDR_SADDR_Msk        (0xfffffffful << CRPT_AES0_SADDR_SADDR_Pos)       /*!< CRPT_T::AES0_SADDR: SADDR Mask         */

#define CRPT_AES0_DADDR_DADDR_Pos        (0)                                               /*!< CRPT_T::AES0_DADDR: DADDR Position     */
#define CRPT_AES0_DADDR_DADDR_Msk        (0xfffffffful << CRPT_AES0_DADDR_DADDR_Pos)       /*!< CRPT_T::AES0_DADDR: DADDR Mask         */

#define CRPT_AES0_CNT_CNT_Pos            (0)                                               /*!< CRPT_T::AES0_CNT: CNT Position         */
#define CRPT_AES0_CNT_CNT_Msk            (0xfffffffful << CRPT_AES0_CNT_CNT_Pos)           /*!< CRPT_T::AES0_CNT: CNT Mask             */

#define CRPT_AES1_KEYx_KEY_Pos           (0)                                               /*!< CRPT_T::AES1_KEY[8]: KEY Position      */
#define CRPT_AES1_KEYx_KEY_Msk           (0xfffffffful << CRPT_AES1_KEYx_KEY_Pos)          /*!< CRPT_T::AES1_KEY[8]: KEY Mask          */

#define CRPT_AES1_IVx_IV_Pos             (0)                                               /*!< CRPT_T::AES1_IV[4]: IV Position        */
#define CRPT_AES1_IVx_IV_Msk             (0xfffffffful << CRPT_AES1_IVx_IV_Pos)            /*!< CRPT_T::AES1_IV[4]: IV Mask            */

#define CRPT_AES1_SADDR_SADDR_Pos        (0)                                               /*!< CRPT_T::AES1_SADDR: SADDR Position     */
#define CRPT_AES1_SADDR_SADDR_Msk        (0xfffffffful << CRPT_AES1_SADDR_SADDR_Pos)       /*!< CRPT_T::AES1_SADDR: SADDR Mask         */

#define CRPT_AES1_DADDR_DADDR_Pos        (0)                                               /*!< CRPT_T::AES1_DADDR: DADDR Position     */
#define CRPT_AES1_DADDR_DADDR_Msk        (0xfffffffful << CRPT_AES1_DADDR_DADDR_Pos)       /*!< CRPT_T::AES1_DADDR: DADDR Mask         */

#define CRPT_AES1_CNT_CNT_Pos            (0)                                               /*!< CRPT_T::AES1_CNT: CNT Position         */
#define CRPT_AES1_CNT_CNT_Msk            (0xfffffffful << CRPT_AES1_CNT_CNT_Pos)           /*!< CRPT_T::AES1_CNT: CNT Mask             */

#define CRPT_AES2_KEYx_KEY_Pos           (0)                                               /*!< CRPT_T::AES2_KEY[8]: KEY Position      */
#define CRPT_AES2_KEYx_KEY_Msk           (0xfffffffful << CRPT_AES2_KEYx_KEY_Pos)          /*!< CRPT_T::AES2_KEY[8]: KEY Mask          */

#define CRPT_AES2_IVx_IV_Pos             (0)                                               /*!< CRPT_T::AES2_IV[4]: IV Position        */
#define CRPT_AES2_IVx_IV_Msk             (0xfffffffful << CRPT_AES2_IVx_IV_Pos)            /*!< CRPT_T::AES2_IV[4]: IV Mask            */

#define CRPT_AES2_SADDR_SADDR_Pos        (0)                                               /*!< CRPT_T::AES2_SADDR: SADDR Position     */
#define CRPT_AES2_SADDR_SADDR_Msk        (0xfffffffful << CRPT_AES2_SADDR_SADDR_Pos)       /*!< CRPT_T::AES2_SADDR: SADDR Mask         */

#define CRPT_AES2_DADDR_DADDR_Pos        (0)                                               /*!< CRPT_T::AES2_DADDR: DADDR Position     */
#define CRPT_AES2_DADDR_DADDR_Msk        (0xfffffffful << CRPT_AES2_DADDR_DADDR_Pos)       /*!< CRPT_T::AES2_DADDR: DADDR Mask         */

#define CRPT_AES2_CNT_CNT_Pos            (0)                                               /*!< CRPT_T::AES2_CNT: CNT Position         */
#define CRPT_AES2_CNT_CNT_Msk            (0xfffffffful << CRPT_AES2_CNT_CNT_Pos)           /*!< CRPT_T::AES2_CNT: CNT Mask             */

#define CRPT_AES3_KEYx_KEY_Pos           (0)                                               /*!< CRPT_T::AES3_KEY[8]: KEY Position      */
#define CRPT_AES3_KEYx_KEY_Msk           (0xfffffffful << CRPT_AES3_KEYx_KEY_Pos)          /*!< CRPT_T::AES3_KEY[8]: KEY Mask          */

#define CRPT_AES3_IVx_IV_Pos             (0)                                               /*!< CRPT_T::AES3_IV[4]: IV Position        */
#define CRPT_AES3_IVx_IV_Msk             (0xfffffffful << CRPT_AES3_IVx_IV_Pos)            /*!< CRPT_T::AES3_IV[4]: IV Mask            */

#define CRPT_AES3_SADDR_SADDR_Pos        (0)                                               /*!< CRPT_T::AES3_SADDR: SADDR Position     */
#define CRPT_AES3_SADDR_SADDR_Msk        (0xfffffffful << CRPT_AES3_SADDR_SADDR_Pos)       /*!< CRPT_T::AES3_SADDR: SADDR Mask         */

#define CRPT_AES3_DADDR_DADDR_Pos        (0)                                               /*!< CRPT_T::AES3_DADDR: DADDR Position     */
#define CRPT_AES3_DADDR_DADDR_Msk        (0xfffffffful << CRPT_AES3_DADDR_DADDR_Pos)       /*!< CRPT_T::AES3_DADDR: DADDR Mask         */

#define CRPT_AES3_CNT_CNT_Pos            (0)                                               /*!< CRPT_T::AES3_CNT: CNT Position         */
#define CRPT_AES3_CNT_CNT_Msk            (0xfffffffful << CRPT_AES3_CNT_CNT_Pos)           /*!< CRPT_T::AES3_CNT: CNT Mask             */

#define CRPT_TDES_CTL_START_Pos          (0)                                               /*!< CRPT_T::TDES_CTL: START Position       */
#define CRPT_TDES_CTL_START_Msk          (0x1ul << CRPT_TDES_CTL_START_Pos)                /*!< CRPT_T::TDES_CTL: START Mask           */

#define CRPT_TDES_CTL_STOP_Pos           (1)                                               /*!< CRPT_T::TDES_CTL: STOP Position        */
#define CRPT_TDES_CTL_STOP_Msk           (0x1ul << CRPT_TDES_CTL_STOP_Pos)                 /*!< CRPT_T::TDES_CTL: STOP Mask            */

#define CRPT_TDES_CTL_TMODE_Pos          (2)                                               /*!< CRPT_T::TDES_CTL: TMODE Position       */
#define CRPT_TDES_CTL_TMODE_Msk          (0x1ul << CRPT_TDES_CTL_TMODE_Pos)                /*!< CRPT_T::TDES_CTL: TMODE Mask           */

#define CRPT_TDES_CTL_3KEYS_Pos          (3)                                               /*!< CRPT_T::TDES_CTL: 3KEYS Position       */
#define CRPT_TDES_CTL_3KEYS_Msk          (0x1ul << CRPT_TDES_CTL_3KEYS_Pos)                /*!< CRPT_T::TDES_CTL: 3KEYS Mask           */

#define CRPT_TDES_CTL_DMALAST_Pos        (5)                                               /*!< CRPT_T::TDES_CTL: DMALAST Position     */
#define CRPT_TDES_CTL_DMALAST_Msk        (0x1ul << CRPT_TDES_CTL_DMALAST_Pos)              /*!< CRPT_T::TDES_CTL: DMALAST Mask         */

#define CRPT_TDES_CTL_DMACSCAD_Pos       (6)                                               /*!< CRPT_T::TDES_CTL: DMACSCAD Position    */
#define CRPT_TDES_CTL_DMACSCAD_Msk       (0x1ul << CRPT_TDES_CTL_DMACSCAD_Pos)             /*!< CRPT_T::TDES_CTL: DMACSCAD Mask        */

#define CRPT_TDES_CTL_DMAEN_Pos          (7)                                               /*!< CRPT_T::TDES_CTL: DMAEN Position       */
#define CRPT_TDES_CTL_DMAEN_Msk          (0x1ul << CRPT_TDES_CTL_DMAEN_Pos)                /*!< CRPT_T::TDES_CTL: DMAEN Mask           */

#define CRPT_TDES_CTL_OPMODE_Pos         (8)                                               /*!< CRPT_T::TDES_CTL: OPMODE Position      */
#define CRPT_TDES_CTL_OPMODE_Msk         (0x7ul << CRPT_TDES_CTL_OPMODE_Pos)               /*!< CRPT_T::TDES_CTL: OPMODE Mask          */

#define CRPT_TDES_CTL_ENCRPT_Pos         (16)                                              /*!< CRPT_T::TDES_CTL: ENCRPT Position      */
#define CRPT_TDES_CTL_ENCRPT_Msk         (0x1ul << CRPT_TDES_CTL_ENCRPT_Pos)               /*!< CRPT_T::TDES_CTL: ENCRPT Mask          */

#define CRPT_TDES_CTL_BLKSWAP_Pos        (21)                                              /*!< CRPT_T::TDES_CTL: BLKSWAP Position     */
#define CRPT_TDES_CTL_BLKSWAP_Msk        (0x1ul << CRPT_TDES_CTL_BLKSWAP_Pos)              /*!< CRPT_T::TDES_CTL: BLKSWAP Mask         */

#define CRPT_TDES_CTL_OUTSWAP_Pos        (22)                                              /*!< CRPT_T::TDES_CTL: OUTSWAP Position     */
#define CRPT_TDES_CTL_OUTSWAP_Msk        (0x1ul << CRPT_TDES_CTL_OUTSWAP_Pos)              /*!< CRPT_T::TDES_CTL: OUTSWAP Mask         */

#define CRPT_TDES_CTL_INSWAP_Pos         (23)                                              /*!< CRPT_T::TDES_CTL: INSWAP Position      */
#define CRPT_TDES_CTL_INSWAP_Msk         (0x1ul << CRPT_TDES_CTL_INSWAP_Pos)               /*!< CRPT_T::TDES_CTL: INSWAP Mask          */

#define CRPT_TDES_CTL_CHANNEL_Pos        (24)                                              /*!< CRPT_T::TDES_CTL: CHANNEL Position     */
#define CRPT_TDES_CTL_CHANNEL_Msk        (0x3ul << CRPT_TDES_CTL_CHANNEL_Pos)              /*!< CRPT_T::TDES_CTL: CHANNEL Mask         */

#define CRPT_TDES_CTL_KEYUNPRT_Pos       (26)                                              /*!< CRPT_T::TDES_CTL: KEYUNPRT Position    */
#define CRPT_TDES_CTL_KEYUNPRT_Msk       (0x1ful << CRPT_TDES_CTL_KEYUNPRT_Pos)            /*!< CRPT_T::TDES_CTL: KEYUNPRT Mask        */

#define CRPT_TDES_CTL_KEYPRT_Pos         (31)                                              /*!< CRPT_T::TDES_CTL: KEYPRT Position      */
#define CRPT_TDES_CTL_KEYPRT_Msk         (0x1ul << CRPT_TDES_CTL_KEYPRT_Pos)               /*!< CRPT_T::TDES_CTL: KEYPRT Mask          */

#define CRPT_TDES_STS_BUSY_Pos           (0)                                               /*!< CRPT_T::TDES_STS: BUSY Position        */
#define CRPT_TDES_STS_BUSY_Msk           (0x1ul << CRPT_TDES_STS_BUSY_Pos)                 /*!< CRPT_T::TDES_STS: BUSY Mask            */

#define CRPT_TDES_STS_INBUFEMPTY_Pos     (8)                                               /*!< CRPT_T::TDES_STS: INBUFEMPTY Position  */
#define CRPT_TDES_STS_INBUFEMPTY_Msk     (0x1ul << CRPT_TDES_STS_INBUFEMPTY_Pos)           /*!< CRPT_T::TDES_STS: INBUFEMPTY Mask      */

#define CRPT_TDES_STS_INBUFFULL_Pos      (9)                                               /*!< CRPT_T::TDES_STS: INBUFFULL Position   */
#define CRPT_TDES_STS_INBUFFULL_Msk      (0x1ul << CRPT_TDES_STS_INBUFFULL_Pos)            /*!< CRPT_T::TDES_STS: INBUFFULL Mask       */

#define CRPT_TDES_STS_INBUFERR_Pos       (10)                                              /*!< CRPT_T::TDES_STS: INBUFERR Position    */
#define CRPT_TDES_STS_INBUFERR_Msk       (0x1ul << CRPT_TDES_STS_INBUFERR_Pos)             /*!< CRPT_T::TDES_STS: INBUFERR Mask        */

#define CRPT_TDES_STS_OUTBUFEMPTY_Pos    (16)                                              /*!< CRPT_T::TDES_STS: OUTBUFEMPTY Position */
#define CRPT_TDES_STS_OUTBUFEMPTY_Msk    (0x1ul << CRPT_TDES_STS_OUTBUFEMPTY_Pos)          /*!< CRPT_T::TDES_STS: OUTBUFEMPTY Mask     */

#define CRPT_TDES_STS_OUTBUFFULL_Pos     (17)                                              /*!< CRPT_T::TDES_STS: OUTBUFFULL Position  */
#define CRPT_TDES_STS_OUTBUFFULL_Msk     (0x1ul << CRPT_TDES_STS_OUTBUFFULL_Pos)           /*!< CRPT_T::TDES_STS: OUTBUFFULL Mask      */

#define CRPT_TDES_STS_OUTBUFERR_Pos      (18)                                              /*!< CRPT_T::TDES_STS: OUTBUFERR Position   */
#define CRPT_TDES_STS_OUTBUFERR_Msk      (0x1ul << CRPT_TDES_STS_OUTBUFERR_Pos)            /*!< CRPT_T::TDES_STS: OUTBUFERR Mask       */

#define CRPT_TDES_STS_BUSERR_Pos         (20)                                              /*!< CRPT_T::TDES_STS: BUSERR Position      */
#define CRPT_TDES_STS_BUSERR_Msk         (0x1ul << CRPT_TDES_STS_BUSERR_Pos)               /*!< CRPT_T::TDES_STS: BUSERR Mask          */

#define CRPT_TDES0_KEYxH_KEY_Pos         (0)                                               /*!< CRPT_T::TDES0_KEYxH: KEY Position      */
#define CRPT_TDES0_KEYxH_KEY_Msk         (0xfffffffful << CRPT_TDES0_KEYxH_KEY_Pos)        /*!< CRPT_T::TDES0_KEYxH: KEY Mask          */

#define CRPT_TDES0_KEYxL_KEY_Pos         (0)                                               /*!< CRPT_T::TDES0_KEYxL: KEY Position      */
#define CRPT_TDES0_KEYxL_KEY_Msk         (0xfffffffful << CRPT_TDES0_KEYxL_KEY_Pos)        /*!< CRPT_T::TDES0_KEYxL: KEY Mask          */

#define CRPT_TDES0_IVH_IV_Pos            (0)                                               /*!< CRPT_T::TDES0_IVH: IV Position         */
#define CRPT_TDES0_IVH_IV_Msk            (0xfffffffful << CRPT_TDES0_IVH_IV_Pos)           /*!< CRPT_T::TDES0_IVH: IV Mask             */

#define CRPT_TDES0_IVL_IV_Pos            (0)                                               /*!< CRPT_T::TDES0_IVL: IV Position         */
#define CRPT_TDES0_IVL_IV_Msk            (0xfffffffful << CRPT_TDES0_IVL_IV_Pos)           /*!< CRPT_T::TDES0_IVL: IV Mask             */

#define CRPT_TDES0_SADDR_SADDR_Pos       (0)                                               /*!< CRPT_T::TDES0_SADDR: SADDR Position    */
#define CRPT_TDES0_SADDR_SADDR_Msk       (0xfffffffful << CRPT_TDES0_SADDR_SADDR_Pos)      /*!< CRPT_T::TDES0_SADDR: SADDR Mask        */

#define CRPT_TDES0_DADDR_DADDR_Pos       (0)                                               /*!< CRPT_T::TDES0_DADDR: DADDR Position    */
#define CRPT_TDES0_DADDR_DADDR_Msk       (0xfffffffful << CRPT_TDES0_DADDR_DADDR_Pos)      /*!< CRPT_T::TDES0_DADDR: DADDR Mask        */

#define CRPT_TDES0_CNT_CNT_Pos           (0)                                               /*!< CRPT_T::TDES0_CNT: CNT Position        */
#define CRPT_TDES0_CNT_CNT_Msk           (0xfffffffful << CRPT_TDES0_CNT_CNT_Pos)          /*!< CRPT_T::TDES0_CNT: CNT Mask            */

#define CRPT_TDES_DATIN_DATIN_Pos        (0)                                               /*!< CRPT_T::TDES_DATIN: DATIN Position     */
#define CRPT_TDES_DATIN_DATIN_Msk        (0xfffffffful << CRPT_TDES_DATIN_DATIN_Pos)       /*!< CRPT_T::TDES_DATIN: DATIN Mask         */

#define CRPT_TDES_DATOUT_DATOUT_Pos      (0)                                               /*!< CRPT_T::TDES_DATOUT: DATOUT Position   */
#define CRPT_TDES_DATOUT_DATOUT_Msk      (0xfffffffful << CRPT_TDES_DATOUT_DATOUT_Pos)     /*!< CRPT_T::TDES_DATOUT: DATOUT Mask       */

#define CRPT_TDES1_KEYxH_KEY_Pos         (0)                                               /*!< CRPT_T::TDES1_KEYxH: KEY Position      */
#define CRPT_TDES1_KEYxH_KEY_Msk         (0xfffffffful << CRPT_TDES1_KEYxH_KEY_Pos)        /*!< CRPT_T::TDES1_KEYxH: KEY Mask          */

#define CRPT_TDES1_KEYxL_KEY_Pos         (0)                                               /*!< CRPT_T::TDES1_KEYxL: KEY Position      */
#define CRPT_TDES1_KEYxL_KEY_Msk         (0xfffffffful << CRPT_TDES1_KEY1L_KEY_Pos)        /*!< CRPT_T::TDES1_KEYxL: KEY Mask          */

#define CRPT_TDES1_IVH_IV_Pos            (0)                                               /*!< CRPT_T::TDES1_IVH: IV Position         */
#define CRPT_TDES1_IVH_IV_Msk            (0xfffffffful << CRPT_TDES1_IVH_IV_Pos)           /*!< CRPT_T::TDES1_IVH: IV Mask             */

#define CRPT_TDES1_IVL_IV_Pos            (0)                                               /*!< CRPT_T::TDES1_IVL: IV Position         */
#define CRPT_TDES1_IVL_IV_Msk            (0xfffffffful << CRPT_TDES1_IVL_IV_Pos)           /*!< CRPT_T::TDES1_IVL: IV Mask             */

#define CRPT_TDES1_SADDR_SADDR_Pos       (0)                                               /*!< CRPT_T::TDES1_SADDR: SADDR Position    */
#define CRPT_TDES1_SADDR_SADDR_Msk       (0xfffffffful << CRPT_TDES1_SADDR_SADDR_Pos)      /*!< CRPT_T::TDES1_SADDR: SADDR Mask        */

#define CRPT_TDES1_DADDR_DADDR_Pos       (0)                                               /*!< CRPT_T::TDES1_DADDR: DADDR Position    */
#define CRPT_TDES1_DADDR_DADDR_Msk       (0xfffffffful << CRPT_TDES1_DADDR_DADDR_Pos)      /*!< CRPT_T::TDES1_DADDR: DADDR Mask        */

#define CRPT_TDES1_CNT_CNT_Pos           (0)                                               /*!< CRPT_T::TDES1_CNT: CNT Position        */
#define CRPT_TDES1_CNT_CNT_Msk           (0xfffffffful << CRPT_TDES1_CNT_CNT_Pos)          /*!< CRPT_T::TDES1_CNT: CNT Mask            */

#define CRPT_TDES2_KEYxH_KEY_Pos         (0)                                               /*!< CRPT_T::TDES2_KEYxH: KEY Position      */
#define CRPT_TDES2_KEYxH_KEY_Msk         (0xfffffffful << CRPT_TDES2_KEYxH_KEY_Pos)        /*!< CRPT_T::TDES2_KEYxH: KEY Mask          */

#define CRPT_TDES2_KEYxL_KEY_Pos         (0)                                               /*!< CRPT_T::TDES2_KEYxL: KEY Position      */
#define CRPT_TDES2_KEYxL_KEY_Msk         (0xfffffffful << CRPT_TDES2_KEYxL_KEY_Pos)        /*!< CRPT_T::TDES2_KEYxL: KEY Mask          */

#define CRPT_TDES2_IVH_IV_Pos            (0)                                               /*!< CRPT_T::TDES2_IVH: IV Position         */
#define CRPT_TDES2_IVH_IV_Msk            (0xfffffffful << CRPT_TDES2_IVH_IV_Pos)           /*!< CRPT_T::TDES2_IVH: IV Mask             */

#define CRPT_TDES2_IVL_IV_Pos            (0)                                               /*!< CRPT_T::TDES2_IVL: IV Position         */
#define CRPT_TDES2_IVL_IV_Msk            (0xfffffffful << CRPT_TDES2_IVL_IV_Pos)           /*!< CRPT_T::TDES2_IVL: IV Mask             */

#define CRPT_TDES2_SADDR_SADDR_Pos       (0)                                               /*!< CRPT_T::TDES2_SADDR: SADDR Position    */
#define CRPT_TDES2_SADDR_SADDR_Msk       (0xfffffffful << CRPT_TDES2_SADDR_SADDR_Pos)      /*!< CRPT_T::TDES2_SADDR: SADDR Mask        */

#define CRPT_TDES2_DADDR_DADDR_Pos       (0)                                               /*!< CRPT_T::TDES2_DADDR: DADDR Position    */
#define CRPT_TDES2_DADDR_DADDR_Msk       (0xfffffffful << CRPT_TDES2_DADDR_DADDR_Pos)      /*!< CRPT_T::TDES2_DADDR: DADDR Mask        */

#define CRPT_TDES2_CNT_CNT_Pos           (0)                                               /*!< CRPT_T::TDES2_CNT: CNT Position        */
#define CRPT_TDES2_CNT_CNT_Msk           (0xfffffffful << CRPT_TDES2_CNT_CNT_Pos)          /*!< CRPT_T::TDES2_CNT: CNT Mask            */

#define CRPT_TDES3_KEYxH_KEY_Pos         (0)                                               /*!< CRPT_T::TDES3_KEYxH: KEY Position      */
#define CRPT_TDES3_KEYxH_KEY_Msk         (0xfffffffful << CRPT_TDES3_KEYxH_KEY_Pos)        /*!< CRPT_T::TDES3_KEYxH: KEY Mask          */

#define CRPT_TDES3_KEYxL_KEY_Pos         (0)                                               /*!< CRPT_T::TDES3_KEYxL: KEY Position      */
#define CRPT_TDES3_KEYxL_KEY_Msk         (0xfffffffful << CRPT_TDES3_KEYxL_KEY_Pos)        /*!< CRPT_T::TDES3_KEYxL: KEY Mask          */

#define CRPT_TDES3_IVH_IV_Pos            (0)                                               /*!< CRPT_T::TDES3_IVH: IV Position         */
#define CRPT_TDES3_IVH_IV_Msk            (0xfffffffful << CRPT_TDES3_IVH_IV_Pos)           /*!< CRPT_T::TDES3_IVH: IV Mask             */

#define CRPT_TDES3_IVL_IV_Pos            (0)                                               /*!< CRPT_T::TDES3_IVL: IV Position         */
#define CRPT_TDES3_IVL_IV_Msk            (0xfffffffful << CRPT_TDES3_IVL_IV_Pos)           /*!< CRPT_T::TDES3_IVL: IV Mask             */

#define CRPT_TDES3_SADDR_SADDR_Pos       (0)                                               /*!< CRPT_T::TDES3_SADDR: SADDR Position    */
#define CRPT_TDES3_SADDR_SADDR_Msk       (0xfffffffful << CRPT_TDES3_SADDR_SADDR_Pos)      /*!< CRPT_T::TDES3_SADDR: SADDR Mask        */

#define CRPT_TDES3_DADDR_DADDR_Pos       (0)                                               /*!< CRPT_T::TDES3_DADDR: DADDR Position    */
#define CRPT_TDES3_DADDR_DADDR_Msk       (0xfffffffful << CRPT_TDES3_DADDR_DADDR_Pos)      /*!< CRPT_T::TDES3_DADDR: DADDR Mask        */

#define CRPT_TDES3_CNT_CNT_Pos           (0)                                               /*!< CRPT_T::TDES3_CNT: CNT Position        */
#define CRPT_TDES3_CNT_CNT_Msk           (0xfffffffful << CRPT_TDES3_CNT_CNT_Pos)          /*!< CRPT_T::TDES3_CNT: CNT Mask            */

#define CRPT_HMAC_CTL_START_Pos          (0)                                               /*!< CRPT_T::HMAC_CTL: START Position       */
#define CRPT_HMAC_CTL_START_Msk          (0x1ul << CRPT_HMAC_CTL_START_Pos)                /*!< CRPT_T::HMAC_CTL: START Mask           */

#define CRPT_HMAC_CTL_STOP_Pos           (1)                                               /*!< CRPT_T::HMAC_CTL: STOP Position        */
#define CRPT_HMAC_CTL_STOP_Msk           (0x1ul << CRPT_HMAC_CTL_STOP_Pos)                 /*!< CRPT_T::HMAC_CTL: STOP Mask            */

#define CRPT_HMAC_CTL_HMACEN_Pos         (4)                                               /*!< CRPT_T::HMAC_CTL: HMACEN Position      */
#define CRPT_HMAC_CTL_HMACEN_Msk         (0x1ul << CRPT_HMAC_CTL_HMACEN_Pos)               /*!< CRPT_T::HMAC_CTL: HMACEN Mask          */

#define CRPT_HMAC_CTL_DMALAST_Pos        (5)                                               /*!< CRPT_T::HMAC_CTL: DMALAST Position     */
#define CRPT_HMAC_CTL_DMALAST_Msk        (0x1ul << CRPT_HMAC_CTL_DMALAST_Pos)              /*!< CRPT_T::HMAC_CTL: DMALAST Mask         */

#define CRPT_HMAC_CTL_DMAEN_Pos          (7)                                               /*!< CRPT_T::HMAC_CTL: DMAEN Position       */
#define CRPT_HMAC_CTL_DMAEN_Msk          (0x1ul << CRPT_HMAC_CTL_DMAEN_Pos)                /*!< CRPT_T::HMAC_CTL: DMAEN Mask           */

#define CRPT_HMAC_CTL_OPMODE_Pos         (8)                                               /*!< CRPT_T::HMAC_CTL: OPMODE Position      */
#define CRPT_HMAC_CTL_OPMODE_Msk         (0x7ul << CRPT_HMAC_CTL_OPMODE_Pos)               /*!< CRPT_T::HMAC_CTL: OPMODE Mask          */

#define CRPT_HMAC_CTL_OUTSWAP_Pos        (22)                                              /*!< CRPT_T::HMAC_CTL: OUTSWAP Position     */
#define CRPT_HMAC_CTL_OUTSWAP_Msk        (0x1ul << CRPT_HMAC_CTL_OUTSWAP_Pos)              /*!< CRPT_T::HMAC_CTL: OUTSWAP Mask         */

#define CRPT_HMAC_CTL_INSWAP_Pos         (23)                                              /*!< CRPT_T::HMAC_CTL: INSWAP Position      */
#define CRPT_HMAC_CTL_INSWAP_Msk         (0x1ul << CRPT_HMAC_CTL_INSWAP_Pos)               /*!< CRPT_T::HMAC_CTL: INSWAP Mask          */

#define CRPT_HMAC_STS_BUSY_Pos           (0)                                               /*!< CRPT_T::HMAC_STS: BUSY Position        */
#define CRPT_HMAC_STS_BUSY_Msk           (0x1ul << CRPT_HMAC_STS_BUSY_Pos)                 /*!< CRPT_T::HMAC_STS: BUSY Mask            */

#define CRPT_HMAC_STS_DMABUSY_Pos        (1)                                               /*!< CRPT_T::HMAC_STS: DMABUSY Position     */
#define CRPT_HMAC_STS_DMABUSY_Msk        (0x1ul << CRPT_HMAC_STS_DMABUSY_Pos)              /*!< CRPT_T::HMAC_STS: DMABUSY Mask         */

#define CRPT_HMAC_STS_DMAERR_Pos         (8)                                               /*!< CRPT_T::HMAC_STS: DMAERR Position      */
#define CRPT_HMAC_STS_DMAERR_Msk         (0x1ul << CRPT_HMAC_STS_DMAERR_Pos)               /*!< CRPT_T::HMAC_STS: DMAERR Mask          */

#define CRPT_HMAC_STS_DATINREQ_Pos       (16)                                              /*!< CRPT_T::HMAC_STS: DATINREQ Position    */
#define CRPT_HMAC_STS_DATINREQ_Msk       (0x1ul << CRPT_HMAC_STS_DATINREQ_Pos)             /*!< CRPT_T::HMAC_STS: DATINREQ Mask        */

#define CRPT_HMAC_DGSTx_DGST_Pos         (0)                                               /*!< CRPT_T::HMAC_DGST[16]: DGST Position   */
#define CRPT_HMAC_DGSTx_DGST_Msk         (0xfffffffful << CRPT_HMAC_DGSTx_DGST_Pos)        /*!< CRPT_T::HMAC_DGST[16]: DGST Mask       */

#define CRPT_HMAC_KEYCNT_KEYCNT_Pos      (0)                                               /*!< CRPT_T::HMAC_KEYCNT: KEYCNT Position   */
#define CRPT_HMAC_KEYCNT_KEYCNT_Msk      (0xfffffffful << CRPT_HMAC_KEYCNT_KEYCNT_Pos)     /*!< CRPT_T::HMAC_KEYCNT: KEYCNT Mask       */

#define CRPT_HMAC_SADDR_SADDR_Pos        (0)                                               /*!< CRPT_T::HMAC_SADDR: SADDR Position     */
#define CRPT_HMAC_SADDR_SADDR_Msk        (0xfffffffful << CRPT_HMAC_SADDR_SADDR_Pos)       /*!< CRPT_T::HMAC_SADDR: SADDR Mask         */

#define CRPT_HMAC_DMACNT_DMACNT_Pos      (0)                                               /*!< CRPT_T::HMAC_DMACNT: DMACNT Position   */
#define CRPT_HMAC_DMACNT_DMACNT_Msk      (0xfffffffful << CRPT_HMAC_DMACNT_DMACNT_Pos)     /*!< CRPT_T::HMAC_DMACNT: DMACNT Mask       */

#define CRPT_HMAC_DATIN_DATIN_Pos        (0)                                               /*!< CRPT_T::HMAC_DATIN: DATIN Position     */
#define CRPT_HMAC_DATIN_DATIN_Msk        (0xfffffffful << CRPT_HMAC_DATIN_DATIN_Pos)       /*!< CRPT_T::HMAC_DATIN: DATIN Mask         */

#define CRPT_ECC_CTL_START_Pos           (0)                                               /*!< CRPT_T::ECC_CTL: START Position        */
#define CRPT_ECC_CTL_START_Msk           (0x1ul << CRPT_ECC_CTL_START_Pos)                 /*!< CRPT_T::ECC_CTL: START Mask            */

#define CRPT_ECC_CTL_STOP_Pos            (1)                                               /*!< CRPT_T::ECC_CTL: STOP Position         */
#define CRPT_ECC_CTL_STOP_Msk            (0x1ul << CRPT_ECC_CTL_STOP_Pos)                  /*!< CRPT_T::ECC_CTL: STOP Mask             */

#define CRPT_ECC_CTL_DMAEN_Pos           (7)                                               /*!< CRPT_T::ECC_CTL: DMAEN Position        */
#define CRPT_ECC_CTL_DMAEN_Msk           (0x1ul << CRPT_ECC_CTL_DMAEN_Pos)                 /*!< CRPT_T::ECC_CTL: DMAEN Mask            */

#define CRPT_ECC_CTL_FSEL_Pos            (8)                                               /*!< CRPT_T::ECC_CTL: FSEL Position         */
#define CRPT_ECC_CTL_FSEL_Msk            (0x1ul << CRPT_ECC_CTL_FSEL_Pos)                  /*!< CRPT_T::ECC_CTL: FSEL Mask             */

#define CRPT_ECC_CTL_ECCOP_Pos           (9)                                               /*!< CRPT_T::ECC_CTL: ECCOP Position        */
#define CRPT_ECC_CTL_ECCOP_Msk           (0x3ul << CRPT_ECC_CTL_ECCOP_Pos)                 /*!< CRPT_T::ECC_CTL: ECCOP Mask            */

#define CRPT_ECC_CTL_MODOP_Pos           (11)                                              /*!< CRPT_T::ECC_CTL: MODOP Position        */
#define CRPT_ECC_CTL_MODOP_Msk           (0x3ul << CRPT_ECC_CTL_MODOP_Pos)                 /*!< CRPT_T::ECC_CTL: MODOP Mask            */

#define CRPT_ECC_CTL_LDP1_Pos            (16)                                              /*!< CRPT_T::ECC_CTL: LDP1 Position         */
#define CRPT_ECC_CTL_LDP1_Msk            (0x1ul << CRPT_ECC_CTL_LDP1_Pos)                  /*!< CRPT_T::ECC_CTL: LDP1 Mask             */

#define CRPT_ECC_CTL_LDP2_Pos            (17)                                              /*!< CRPT_T::ECC_CTL: LDP2 Position         */
#define CRPT_ECC_CTL_LDP2_Msk            (0x1ul << CRPT_ECC_CTL_LDP2_Pos)                  /*!< CRPT_T::ECC_CTL: LDP2 Mask             */

#define CRPT_ECC_CTL_LDA_Pos             (18)                                              /*!< CRPT_T::ECC_CTL: LDA Position          */
#define CRPT_ECC_CTL_LDA_Msk             (0x1ul << CRPT_ECC_CTL_LDA_Pos)                   /*!< CRPT_T::ECC_CTL: LDA Mask              */

#define CRPT_ECC_CTL_LDB_Pos             (19)                                              /*!< CRPT_T::ECC_CTL: LDB Position          */
#define CRPT_ECC_CTL_LDB_Msk             (0x1ul << CRPT_ECC_CTL_LDB_Pos)                   /*!< CRPT_T::ECC_CTL: LDB Mask              */

#define CRPT_ECC_CTL_LDN_Pos             (20)                                              /*!< CRPT_T::ECC_CTL: LDN Position          */
#define CRPT_ECC_CTL_LDN_Msk             (0x1ul << CRPT_ECC_CTL_LDN_Pos)                   /*!< CRPT_T::ECC_CTL: LDN Mask              */

#define CRPT_ECC_CTL_LDK_Pos             (21)                                              /*!< CRPT_T::ECC_CTL: LDK Position          */
#define CRPT_ECC_CTL_LDK_Msk             (0x1ul << CRPT_ECC_CTL_LDK_Pos)                   /*!< CRPT_T::ECC_CTL: LDK Mask              */

#define CRPT_ECC_CTL_CURVEM_Pos          (22)                                              /*!< CRPT_T::ECC_CTL: CURVEM Position       */
#define CRPT_ECC_CTL_CURVEM_Msk          (0x3fful << CRPT_ECC_CTL_CURVEM_Pos)              /*!< CRPT_T::ECC_CTL: CURVEM Mask           */

#define CRPT_ECC_STS_BUSY_Pos            (0)                                               /*!< CRPT_T::ECC_STS: BUSY Position         */
#define CRPT_ECC_STS_BUSY_Msk            (0x1ul << CRPT_ECC_STS_BUSY_Pos)                  /*!< CRPT_T::ECC_STS: BUSY Mask             */

#define CRPT_ECC_STS_DMABUSY_Pos         (1)                                               /*!< CRPT_T::ECC_STS: DMABUSY Position      */
#define CRPT_ECC_STS_DMABUSY_Msk         (0x1ul << CRPT_ECC_STS_DMABUSY_Pos)               /*!< CRPT_T::ECC_STS: DMABUSY Mask          */

#define CRPT_ECC_STS_BUSERR_Pos          (16)                                              /*!< CRPT_T::ECC_STS: BUSERR Position       */
#define CRPT_ECC_STS_BUSERR_Msk          (0x1ul << CRPT_ECC_STS_BUSERR_Pos)                /*!< CRPT_T::ECC_STS: BUSERR Mask           */

#define CRPT_ECC_X1_POINTX1_Pos          (0)                                               /*!< CRPT_T::ECC_X1[18]:  POINTX1 Position  */
#define CRPT_ECC_X1_POINTX1_Msk          (0xfffffffful << CRPT_ECC_X1_POINTX1_Pos)         /*!< CRPT_T::ECC_X1[18]:  POINTX1 Mask      */

#define CRPT_ECC_Y1_POINTY1_Pos          (0)                                               /*!< CRPT_T::ECC_Y1[18]: POINTY1 Position   */
#define CRPT_ECC_Y1_POINTY1_Msk          (0xfffffffful << CRPT_ECC_Y1_POINTY1_Pos)         /*!< CRPT_T::ECC_Y1[18]: POINTY1 Mask       */

#define CRPT_ECC_X2_POINTX2_Pos          (0)                                               /*!< CRPT_T::ECC_X2[18]: POINTX2 Position   */
#define CRPT_ECC_X2_POINTX2_Msk          (0xfffffffful << CRPT_ECC_X2_POINTX2_Pos)         /*!< CRPT_T::ECC_X2[18]: POINTX2 Mask       */

#define CRPT_ECC_Y2_POINTY2_Pos          (0)                                               /*!< CRPT_T::ECC_Y2[18]: POINTY2 Position   */
#define CRPT_ECC_Y2_POINTY2_Msk          (0xfffffffful << CRPT_ECC_Y2_POINTY2_Pos)         /*!< CRPT_T::ECC_Y2[18]: POINTY2 Mask       */

#define CRPT_ECC_A_CURVEA_Pos            (0)                                               /*!< CRPT_T::ECC_A[18]: CURVEA Position     */
#define CRPT_ECC_A_CURVEA_Msk            (0xfffffffful << CRPT_ECC_A_CURVEA_Pos)           /*!< CRPT_T::ECC_A[18]: CURVEA Mask         */

#define CRPT_ECC_B_CURVEB_Pos            (0)                                               /*!< CRPT_T::ECC_B[18]: CURVEB Position     */
#define CRPT_ECC_B_CURVEB_Msk            (0xfffffffful << CRPT_ECC_B_CURVEB_Pos)           /*!< CRPT_T::ECC_B[18]: CURVEB Mask         */

#define CRPT_ECC_N_CURVEN_Pos            (0)                                               /*!< CRPT_T::ECC_N[18]: CURVEN Position     */
#define CRPT_ECC_N_CURVEN_Msk            (0xfffffffful << CRPT_ECC_N_CURVEN_Pos)           /*!< CRPT_T::ECC_N[18]: CURVEN Mask         */

#define CRPT_ECC_K_SCALARK_Pos           (0)                                               /*!< CRPT_T::ECC_K[18]: SCALARK Position    */
#define CRPT_ECC_K_SCALARK_Msk           (0xfffffffful << CRPT_ECC_K_SCALARK_Pos)          /*!< CRPT_T::ECC_K[18]: SCALARK Mask        */

#define CRPT_ECC_DADDR_DADDR_Pos         (0)                                               /*!< CRPT_T::ECC_DADDR: DADDR Position      */
#define CRPT_ECC_DADDR_DADDR_Msk         (0xfffffffful << CRPT_ECC_DADDR_DADDR_Pos)        /*!< CRPT_T::ECC_DADDR: DADDR Mask          */

#define CRPT_ECC_STARTREG_STARTREG_Pos   (0)                                               /*!< CRPT_T::ECC_STARTREG: STARTREG Position*/
#define CRPT_ECC_STARTREG_STARTREG_Msk   (0xfffffffful << CRPT_ECC_STARTREG_STARTREG_Pos)  /*!< CRPT_T::ECC_STARTREG: STARTREG Mask    */

#define CRPT_ECC_WORDCNT_WORDCNT_Pos     (0)                                               /*!< CRPT_T::ECC_WORDCNT: WORDCNT Position  */
#define CRPT_ECC_WORDCNT_WORDCNT_Msk     (0xfffffffful << CRPT_ECC_WORDCNT_WORDCNT_Pos)    /*!< CRPT_T::ECC_WORDCNT: WORDCNT Mask      */

/**@}*/ /* CRPT_CONST CRYPTO */
/**@}*/ /* end of CRYPTO register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __CRYPTO_REG_H__ */
