/**************************************************************************//**
 * @file     ccap_reg.h
 * @version  V1.00
 * @brief    CCAP register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __CCAP_REG_H__
#define __CCAP_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup CCAP Camera Capture Interface Controller (CCAP)
    Memory Mapped Structure for CCAP Controller
@{ */


typedef struct {


    /**
     * @var CCAP_T::CTL
     * Offset: 0x00  Camera Capture Interface Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CCAPEN    |Camera Capture Interface Enable
     * |        |          |0 = Camera Capture Interface Disabled.
     * |        |          |1 = Camera Capture Interface Enabled.
     * |[3]     |ADDRSW    |Packet Buffer Address Switch
     * |        |          |0 = Packet buffer address switch Disabled.
     * |        |          |1 = Packet buffer address switch Enabled.
     * |[6]     |PKTEN     |Packet Output Enable
     * |        |          |0 = Packet output Disabled.
     * |        |          |1 = Packet output Enabled.
     * |[7]     |MONO      |Monochrome CMOS Sensor Select
     * |        |          |0 = Color CMOS Sensor.
     * |        |          |1 = Monochrome CMOS Sensor. The U/V components are ignored when the MONO is enabled.
     * |[16]    |SHUTTER   |Image Capture Interface Automatically Disable The Capture Interface After A Frame Had Been Captured
     * |        |          |0 = Shutter Disabled.
     * |        |          |1 = Shutter Enabled.
     * |[20]    |UPDATE    |Update Register At New Frame
     * |        |          |0 = Update register at new frame Disabled.
     * |        |          |1 = Update register at new frame Enabled (Auto clear to 0 when register updated).
     * |[24]    |VPRST     |Capture Interface Reset
     * |        |          |0 = Capture interface reset Disabled.
     * |        |          |1 = Capture interface reset Enabled.
     * @var CCAP_T::PAR
     * Offset: 0x04  Camera Capture Interface Parameter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |INFMT     |Sensor Input Data Format
     * |        |          |0 = YCbCr422.
     * |        |          |1 = RGB565.
     * |[1]     |SENTYPE   |Sensor Input Type
     * |        |          |0 = CCIR601.
     * |        |          |1 = CCIR656, VSync & Hsync embedded in the data signal.
     * |[2:3]   |INDATORD  |Sensor Input Data Order
     * |        |          |If INFMT = 0 (YCbCr),.
     * |        |          | Byte 0 1 2 3
     * |        |          |00 = Y0 U0 Y1 V0.
     * |        |          |01 = Y0 V0 Y1 U0.
     * |        |          |10 = U0 Y0 V0 Y1.
     * |        |          |11 = V0 Y0 U0 Y1.
     * |        |          |If INFMT = 1 (RGB565),.
     * |        |          |00 = Byte0[R[4:0] G[5:3]] Byte1[G[2:0] B[4:0]]
     * |        |          |01 = Byte0[B[4:0] G[5:3]] Byte1[G[2:0] R[4:0]]
     * |        |          |10 = Byte0[G[2:0] B[4:0]] Byte1[R[4:0] G[5:3]]
     * |        |          |11 = Byte0[G[2:0] R[4:0]] Byte1[B[4:0] G[5:3]]
     * |[4:5]   |OUTFMT    |Image Data Format Output To System Memory
     * |        |          |00 = YCbCr422.
     * |        |          |01 = Only output Y.
     * |        |          |10 = RGB555.
     * |        |          |11 = RGB565.
     * |[6]     |RANGE     |Scale Input YUV CCIR601 Color Range To Full Range
     * |        |          |0 = default.
     * |        |          |1 = Scale to full range.
     * |[8]     |PCLKP     |Sensor Pixel Clock Polarity
     * |        |          |0 = Input video data and signals are latched by falling edge of Pixel Clock.
     * |        |          |1 = Input video data and signals are latched by rising edge of Pixel Clock.
     * |[9]     |HSP       |Sensor Hsync Polarity
     * |        |          |0 = Sync Low.
     * |        |          |1 = Sync High.
     * |[10]    |VSP       |Sensor Vsync Polarity
     * |        |          |0 = Sync Low.
     * |        |          |1 = Sync High.
     * |[18]    |FBB       |Field By Blank
     * |        |          |Hardware will tag field0 or field1 by vertical blanking instead of FIELD flag in CCIR-656 mode.
     * |        |          |0 = Field by blank Disabled.
     * |        |          |1 = Field by blank Enabled.
     * @var CCAP_T::INT
     * Offset: 0x08  Camera Capture Interface Interrupt Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |VINTF     |Video Frame End Interrupt
     * |        |          |If this bit shows 1, receiving a frame completed.
     * |        |          |Write 1 to clear it.
     * |[1]     |MEINTF    |Bus Master Transfer Error Interrupt
     * |        |          |If this bit shows 1, Transfer Error occurred. Write 1 to clear it.
     * |[3]     |ADDRMINTF |Memory Address Match Interrupt
     * |        |          |If this bit shows 1, Memory Address Match Interrupt occurred.
     * |        |          |Write 1 to clear it.
     * |[4]     |MDINTF    |Motion Detection Output Finish Interrupt
     * |        |          |If this bit shows 1, Motion Detection Output Finish Interrupt occurred.
     * |        |          |Write 1 to clear it.
     * |[16]    |VIEN      |Video Frame End Interrupt Enable
     * |        |          |0 = Video frame end interrupt Disabled.
     * |        |          |1 = Video frame end interrupt Enabled.
     * |[17]    |MEIEN     |System Memory Error Interrupt Enable
     * |        |          |0 = System memory error interrupt Disabled.
     * |        |          |1 = System memory error interrupt Enabled.
     * |[19]    |ADDRMIEN  |Address Match Interrupt Enable
     * |        |          |0 = Address match interrupt Disabled.
     * |        |          |1 = Address match interrupt Enabled.
     * @var CCAP_T::POSTERIZE
     * Offset: 0x0C  YUV Component Posterizing Factor Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:7]   |VCOMP     |V Component Posterizing Factor
     * |        |          |Final_V_Out = Original_V[7:0] & V_Posterizing_Factor.
     * |[8:15]  |UCOMP     |U Component Posterizing Factor
     * |        |          |Final_U_Out = Original_U[7:0] & U_Posterizing_Factor.
     * |[16:23] |YCOMP     |Y Component Posterizing Factor
     * |        |          |Final_Y_Out = Original_Y[7:0] & Y_Posterizing_Factor.
     * @var CCAP_T::MD
     * Offset: 0x10  Motion Detection Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MDEN      |Motion Detection Enable
     * |        |          |0 = CCAP_MD Disabled.
     * |        |          |1 = CCAP_MD Enabled.
     * |[8]     |MDBS      |Motion Detection Block Size
     * |        |          |0 = 16x16.
     * |        |          |1 = 8x8.
     * |[9]     |MDSM      |Motion Detection Save Mode
     * |        |          |0 = 1 bit DIFF + 7 bit Y Differential.
     * |        |          |1 = 1 bit DIFF only.
     * |[10:11] |MDDF      |Motion Detection Detect Frequency
     * |        |          |00 = Each frame.
     * |        |          |01 = Every 2 frame.
     * |        |          |10 = Every 3 frame.
     * |        |          |11 = Every 4 frame.
     * |[16:20] |MDTHR     |Motion Detection Differential Threshold
     * @var CCAP_T::MDADDR
     * Offset: 0x14  Motion Detection Output Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:31]  |MDADDR    |Motion Detection Output Address Register (Word Alignment)
     * @var CCAP_T::MDYADDR
     * Offset: 0x18  Motion Detection Temp Y Output Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:31]  |MDYADDR   |Motion Detection Temp Y Output Address Register (Word Alignment)
     * @var CCAP_T::SEPIA
     * Offset: 0x1C  Sepia Effect Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:7]   |VCOMP     |Define the constant V component while Sepia color effect is turned on.
     * |[8:15]  |UCOMP     |Define the constant U component while Sepia color effect is turned on.
     * @var CCAP_T::CWSP
     * Offset: 0x20  Cropping Window Starting Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:11]  |CWSADDRH  |Cropping Window Horizontal Starting Address
     * |[16:26] |CWSADDRV  |Cropping Window Vertical Starting Address
     * @var CCAP_T::CWS
     * Offset: 0x24  Cropping Window Size Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:11]  |CIWW      |Cropping Image Window Width
     * |[16:26] |CIWH      |Cropping Image Window Height
     * @var CCAP_T::PKTSL
     * Offset: 0x28  Packet Scaling Vertical/Horizontal Factor Register (LSB)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:7]   |PKTSHML   |Packet Scaling Horizontal Factor M (Lower 8-Bit)
     * |        |          |Specifies the lower 8-bit of denominator part (M) of the horizontal scaling factor.
     * |        |          |The lower 8-bit will be cascaded with higher 8-bit (PKDSHMH) to form a 16-bit denominator (M) of vertical factor.
     * |        |          |The output image width will be equal to the image width * N/M.
     * |        |          |Note: The value of N must be equal to or less than M.
     * |[8:15]  |PKTSHNL   |Packet Scaling Horizontal Factor N (Lower 8-Bit)
     * |        |          |Specify the lower 8-bit of numerator part (N) of the horizontal scaling factor.
     * |        |          |The lower 8-bit will be cascaded with higher 8-bit (PKDSHNH) to form a 16-bit numerator of horizontal factor.
     * |[16:23] |PKTSVML   |Packet Scaling Vertical Factor M (Lower 8-Bit)
     * |        |          |Specify the lower 8-bit of denominator part (M) of the vertical scaling factor.
     * |        |          |The lower 8-bit will be cascaded with higher 8-bit (PKDSVMH) to form a 16-bit denominator (M) of vertical factor.
     * |        |          |The output image width will be equal to the image height * N/M.
     * |        |          |Note: The value of N must be equal to or less than M.
     * |[24:31] |PKTSVNL   |Packet Scaling Vertical Factor N (Lower 8-Bit)
     * |        |          |Specify the lower 8-bit of numerator part (N) of the vertical scaling factor.
     * |        |          |The lower 8-bit will be cascaded with higher 8-bit (PKDSVNH) to form a 16-bit numerator of vertical factor
     * @var CCAP_T::PLNSL
     * Offset: 0x2C  Planar Scaling Vertical/Horizontal Factor Register (LSB)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:7]   |PLNSHML   |Planar Scaling Horizontal Factor M (Lower 8-Bit)
     * |        |          |Specify the lower 8-bit of denominator part (M) of the horizontal scaling factor.
     * |        |          |The lower 8-bit will be cascaded with higher 8-bit (PNDSHMH) to form a 16-bit denominator (M) of vertical factor.
     * |        |          |The output image width will be equal to the image width * N/M.
     * |        |          |Note: The value of N must be equal to or less than M.
     * |[8:15]  |PLNSHNL   |Planar Scaling Horizontal Factor N (Lower 8-Bit)
     * |        |          |Specify the lower 8-bit of numerator part (N) of the horizontal scaling factor.
     * |        |          |The lower 8-bit will be cascaded with higher 8-bit (PNDSHNH) to form a 16-bit numerator of horizontal factor.
     * |[16:23] |PLNSVML   |Planar Scaling Vertical Factor M (Lower 8-Bit)
     * |        |          |Specify the lower 8-bit of denominator part (M) of the vertical scaling factor.
     * |        |          |The lower 8-bit will be cascaded with higher 8-bit (PNDSVMH) to form a 16-bit denominator (M) of vertical factor.
     * |        |          |The output image width will be equal to the image height * N/M.
     * |        |          |Note: The value of N must be equal to or less than M.
     * |[24:31] |PLNSVNL   |Planar Scaling Vertical Factor N (Lower 8-Bit)
     * |        |          |Specify the lower 8-bit of numerator part (N) of the vertical scaling factor.
     * |        |          |The lower 8-bit will be cascaded with higher 8-bit (PNDSVNH) to form a 16-bit numerator of vertical factor.
     * @var CCAP_T::FRCTL
     * Offset: 0x30  Scaling Frame Rate Factor Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:5]   |FRM       |Scaling Frame Rate Factor M
     * |        |          |Specify the denominator part (M) of the frame rate scaling factor.
     * |        |          |The output image frame rate will be equal to input image frame rate * (N/M).
     * |        |          |Note: The value of N must be equal to or less than M.
     * |[8:13]  |FRN       |Scaling Frame Rate Factor N
     * |        |          |Specify the denominator part (N) of the frame rate scaling factor.
     * @var CCAP_T::STRIDE
     * Offset: 0x34  Frame Output Pixel Stride Width Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:13]  |PKTSTRIDE |Packet Frame Output Pixel Stride Width
     * |        |          |The output pixel stride size of packet pipe.
     * |[16:29] |PLNSTRIDE |Planar Frame Output Pixel Stride Width
     * |        |          |The output pixel stride size of planar pipe.
     * @var CCAP_T::FIFOTH
     * Offset: 0x3C  FIFO Threshold Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:3]   |PLNVFTH   |Planar V FIFO Threshold
     * |[8:11]  |PLNUFTH   |Planar U FIFO Threshold
     * |[16:20] |PLNYFTH   |Planar Y FIFO Threshold
     * |[24:28] |PKTFTH    |Packet FIFO Threshold
     * |[31]    |OVF       |FIFO Overflow Flag
     * @var CCAP_T::CMPADDR
     * Offset: 0x40  Compare Memory Base Address Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:31]  |CMPADDR   |Compare Memory Base Address
     * |        |          |Word aligns address; ignore the bits [1:0].
     * @var CCAP_T::LUMA_Y1_THD
     * Offset: 0x44  Luminance Y8 to Y1 Threshold Value Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field          |Descriptions
     * | :----: | :-----------: | :---- |
     * |[0:8]   |LUMA_Y1_THRESH |Luminance Y8 to Y1 Threshold Value
     * |        |               |Specify the 8-bit threshold value for the luminance Y bit-8 to the luminance Y 1-bit conversion.
     * @var CCAP_T::PKTSM
     * Offset: 0x48  Packet Scaling Vertical/Horizontal Factor Register (MSB)
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:7]   |PKTSHMH   |Packet Scaling Horizontal Factor M (Higher 8-Bit)
     * |        |          |Specify the lower 8-bit of denominator part (M) of the horizontal scaling factor.
     * |        |          |Please refer to the register CCAP_PKTSL?for the detailed operation.
     * |[8:15]  |PKTSHNH   |Packet Scaling Horizontal Factor N (Higher 8-Bit)
     * |        |          |Specify the lower 8-bit of numerator part (N) of the horizontal scaling factor.
     * |        |          |Please refer to the register CCAP_PKTSL for the detailed operation.
     * |[16:23] |PKTSVMH   |Packet Scaling Vertical Factor M (Higher 8-Bit)
     * |        |          |Specify the lower 8-bit of denominator part (M) of the vertical scaling factor.
     * |        |          |Please refer to the register CCAP_PKTSL to check the cooperation between these two registers.
     * |[24:31] |PKTSVNH   |Packet Scaling Vertical Factor N (Higher 8-Bit)
     * |        |          |Specify the higher 8-bit of numerator part (N) of the vertical scaling factor.
     * |        |          |Please refer to the register CCAP_PKTSL?to check the cooperation between these two registers.
     * @var CCAP_T::PKTBA0
     * Offset: 0x60  System Memory Packet Base Address 0 Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0:31]  |BASEADDR  |System Memory Packet Base Address 0
     * |        |          |Word aligns address; ignore the bits [1:0].
     */
    __IO uint32_t CTL;
    __IO uint32_t PAR;
    __IO uint32_t INT;
    __IO uint32_t POSTERIZE;
    __IO uint32_t MD;
    __IO uint32_t MDADDR;
    __IO uint32_t MDYADDR;
    __IO uint32_t SEPIA;
    __IO uint32_t CWSP;
    __IO uint32_t CWS;
    __IO uint32_t PKTSL;
    __IO uint32_t PLNSL;
    __IO uint32_t FRCTL;
    __IO uint32_t STRIDE;
    /// @cond HIDDEN_SYMBOLS
    uint32_t RESERVE0[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t FIFOTH;
    __IO uint32_t CMPADDR;
    __IO uint32_t LUMA_Y1_THD;
    __IO uint32_t PKTSM;
    /// @cond HIDDEN_SYMBOLS
    uint32_t RESERVE2[5];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t PKTBA0;
} CCAP_T;

/**
    @addtogroup CCAP_CONST CCAP Bit Field Definition
    Constant Definitions for CCAP Controller
@{ */

#define CCAP_CTL_CCAPEN_Pos               (0)                                               /*!< CCAP_T::CTL: CCAPEN Position                */
#define CCAP_CTL_CCAPEN_Msk               (0x1ul << CCAP_CTL_CCAPEN_Pos)                     /*!< CCAP_T::CTL: CCAPEN Mask                    */

#define CCAP_CTL_ADDRSW_Pos               (3)                                               /*!< CCAP_T::CTL: ADDRSW Position               */
#define CCAP_CTL_ADDRSW_Msk               (0x1ul << CCAP_CTL_ADDRSW_Pos)                     /*!< CCAP_T::CTL: ADDRSW Mask                   */

#define CCAP_CTL_PLNEN_Pos                (5)                                               /*!< CCAP_T::CTL: PLNEN Position                */
#define CCAP_CTL_PLNEN_Msk                (0x1ul << CCAP_CTL_PLNEN_Pos)                      /*!< CCAP_T::CTL: PLNEN Mask                    */

#define CCAP_CTL_PKTEN_Pos                (6)                                               /*!< CCAP_T::CTL: PKTEN Position                */
#define CCAP_CTL_PKTEN_Msk                (0x1ul << CCAP_CTL_PKTEN_Pos)                      /*!< CCAP_T::CTL: PKTEN Mask                    */

#define CCAP_CTL_MONO_Pos                 (7)                                               /*!< CCAP_T::CTL: MONO Position                */
#define CCAP_CTL_MONO_Msk                 (0x1ul << CCAP_CTL_MONO_Pos)                       /*!< CCAP_T::CTL: MONO Mask                    */

#define CCAP_CTL_SHUTTER_Pos              (16)                                              /*!< CCAP_T::CTL: SHUTTER Position              */
#define CCAP_CTL_SHUTTER_Msk              (0x1ul << CCAP_CTL_SHUTTER_Pos)                    /*!< CCAP_T::CTL: SHUTTER Mask                  */

#define CCAP_CTL_MY4_SWAP_Pos             (17)                                              /*!< CCAP_T::CTL: MY4_SWAP Position              */
#define CCAP_CTL_MY4_SWAP_Msk             (0x1ul << CCAP_CTL_MY4_SWAP_Pos)                   /*!< CCAP_T::CTL: MY4_SWAP Mask                  */

#define CCAP_CTL_MY8_MY4_Pos              (18)                                              /*!< CCAP_T::CTL: MY8_MY4 Position              */
#define CCAP_CTL_MY8_MY4_Msk              (0x1ul << CCAP_CTL_MY8_MY4_Pos)                    /*!< CCAP_T::CTL: MY8_MY4 Mask                  */

#define CCAP_CTL_Luma_Y_One_Pos           (19)                                              /*!< CCAP_T::CTL: Luma_Y_One Position              */
#define CCAP_CTL_Luma_Y_One_Msk           (0x1ul << CCAP_CTL_Luma_Y_One_Pos)                 /*!< CCAP_T::CTL: Luma_Y_One Mask                  */

#define CCAP_CTL_UPDATE_Pos               (20)                                              /*!< CCAP_T::CTL: UPDATE Position               */
#define CCAP_CTL_UPDATE_Msk               (0x1ul << CCAP_CTL_UPDATE_Pos)                     /*!< CCAP_T::CTL: UPDATE Mask                   */

#define CCAP_CTL_VPRST_Pos                (24)                                              /*!< CCAP_T::CTL: VPRST Position                */
#define CCAP_CTL_VPRST_Msk                (0x1ul << CCAP_CTL_VPRST_Pos)                      /*!< CCAP_T::CTL: VPRST Mask                    */

#define CCAP_PAR_INFMT_Pos                (0)                                               /*!< CCAP_T::PAR: INFMT Position                */
#define CCAP_PAR_INFMT_Msk                (0x1ul << CCAP_PAR_INFMT_Pos)                      /*!< CCAP_T::PAR: INFMT Mask                    */

#define CCAP_PAR_SENTYPE_Pos              (1)                                               /*!< CCAP_T::PAR: SENTYPE Position              */
#define CCAP_PAR_SENTYPE_Msk              (0x1ul << CCAP_PAR_SENTYPE_Pos)                    /*!< CCAP_T::PAR: SENTYPE Mask                  */

#define CCAP_PAR_INDATORD_Pos             (2)                                               /*!< CCAP_T::PAR: INDATORD Position             */
#define CCAP_PAR_INDATORD_Msk             (0x3ul << CCAP_PAR_INDATORD_Pos)                   /*!< CCAP_T::PAR: INDATORD Mask                 */

#define CCAP_PAR_OUTFMT_Pos               (4)                                               /*!< CCAP_T::PAR: OUTFMT Position               */
#define CCAP_PAR_OUTFMT_Msk               (0x3ul << CCAP_PAR_OUTFMT_Pos)                     /*!< CCAP_T::PAR: OUTFMT Mask                   */

#define CCAP_PAR_RANGE_Pos                (6)                                               /*!< CCAP_T::PAR: RANGE Position                */
#define CCAP_PAR_RANGE_Msk                (0x1ul << CCAP_PAR_RANGE_Pos)                      /*!< CCAP_T::PAR: RANGE Mask                    */

#define CCAP_PAR_PLNFMT_Pos               (7)                                               /*!< CCAP_T::PAR: PLNFMT Position               */
#define CCAP_PAR_PLNFMT_Msk               (0x1ul << CCAP_PAR_PLNFMT_Pos)                     /*!< CCAP_T::PAR: PLNFMT Mask                   */

#define CCAP_PAR_PCLKP_Pos                (8)                                               /*!< CCAP_T::PAR: PCLKP Position                */
#define CCAP_PAR_PCLKP_Msk                (0x1ul << CCAP_PAR_PCLKP_Pos)                      /*!< CCAP_T::PAR: PCLKP Mask                    */

#define CCAP_PAR_HSP_Pos                  (9)                                               /*!< CCAP_T::PAR: HSP Position                  */
#define CCAP_PAR_HSP_Msk                  (0x1ul << CCAP_PAR_HSP_Pos)                        /*!< CCAP_T::PAR: HSP Mask                      */

#define CCAP_PAR_VSP_Pos                  (10)                                              /*!< CCAP_T::PAR: VSP Position                  */
#define CCAP_PAR_VSP_Msk                  (0x1ul << CCAP_PAR_VSP_Pos)                        /*!< CCAP_T::PAR: VSP Mask                      */

#define CCAP_PAR_COLORCTL_Pos             (11)                                              /*!< CCAP_T::PAR: COLORCTL Position             */
#define CCAP_PAR_COLORCTL_Msk             (0x3ul << CCAP_PAR_COLORCTL_Pos)                   /*!< CCAP_T::PAR: COLORCTL Mask                 */

#define CCAP_PAR_FBB_Pos                  (18)                                              /*!< CCAP_T::PAR: FBB Position                  */
#define CCAP_PAR_FBB_Msk                  (0x1ul << CCAP_PAR_FBB_Pos)                        /*!< CCAP_T::PAR: FBB Mask                      */

#define CCAP_INT_VINTF_Pos                (0)                                               /*!< CCAP_T::INT: VINTF Position                */
#define CCAP_INT_VINTF_Msk                (0x1ul << CCAP_INT_VINTF_Pos)                      /*!< CCAP_T::INT: VINTF Mask                    */

#define CCAP_INT_MEINTF_Pos               (1)                                               /*!< CCAP_T::INT: MEINTF Position               */
#define CCAP_INT_MEINTF_Msk               (0x1ul << CCAP_INT_MEINTF_Pos)                     /*!< CCAP_T::INT: MEINTF Mask                   */

#define CCAP_INT_ADDRMINTF_Pos            (3)                                               /*!< CCAP_T::INT: ADDRMINTF Position            */
#define CCAP_INT_ADDRMINTF_Msk            (0x1ul << CCAP_INT_ADDRMINTF_Pos)                  /*!< CCAP_T::INT: ADDRMINTF Mask                */

#define CCAP_INT_MDINTF_Pos               (4)                                               /*!< CCAP_T::INT: MDINTF Position               */
#define CCAP_INT_MDINTF_Msk               (0x1ul << CCAP_INT_MDINTF_Pos)                     /*!< CCAP_T::INT: MDINTF Mask                   */

#define CCAP_INT_VIEN_Pos                 (16)                                              /*!< CCAP_T::INT: VIEN Position                 */
#define CCAP_INT_VIEN_Msk                 (0x1ul << CCAP_INT_VIEN_Pos)                       /*!< CCAP_T::INT: VIEN Mask                     */

#define CCAP_INT_MEIEN_Pos                (17)                                              /*!< CCAP_T::INT: MEIEN Position                */
#define CCAP_INT_MEIEN_Msk                (0x1ul << CCAP_INT_MEIEN_Pos)                      /*!< CCAP_T::INT: MEIEN Mask                    */

#define CCAP_INT_ADDRMIEN_Pos             (19)                                              /*!< CCAP_T::INT: ADDRMIEN Position             */
#define CCAP_INT_ADDRMIEN_Msk             (0x1ul << CCAP_INT_ADDRMIEN_Pos)                   /*!< CCAP_T::INT: ADDRMIEN Mask                 */

#define CCAP_CWSP_CWSADDRH_Pos            (0)                                               /*!< CCAP_T::CWSP: CWSADDRH Position            */
#define CCAP_CWSP_CWSADDRH_Msk            (0xffful << CCAP_CWSP_CWSADDRH_Pos)                /*!< CCAP_T::CWSP: CWSADDRH Mask                */

#define CCAP_CWSP_CWSADDRV_Pos            (16)                                              /*!< CCAP_T::CWSP: CWSADDRV Position            */
#define CCAP_CWSP_CWSADDRV_Msk            (0x7fful << CCAP_CWSP_CWSADDRV_Pos)                /*!< CCAP_T::CWSP: CWSADDRV Mask                */

#define CCAP_CWS_CWW_Pos                  (0)                                               /*!< CCAP_T::CWS: CWW Position                 */
#define CCAP_CWS_CWW_Msk                  (0xffful << CCAP_CWS_CWW_Pos)                      /*!< CCAP_T::CWS: CWW Mask                     */
#define CCAP_CWS_CWH_Pos                  (16)                                              /*!< CCAP_T::CWS: CIWH Position                 */
#define CCAP_CWS_CWH_Msk                  (0x7fful << CCAP_CWS_CWH_Pos)                      /*!< CCAP_T::CWS: CIWH Mask                     */

#define CCAP_PKTSL_PKTSHML_Pos            (0)                                               /*!< CCAP_T::PKTSL: PKTSHML Position            */
#define CCAP_PKTSL_PKTSHML_Msk            (0xfful << CCAP_PKTSL_PKTSHML_Pos)                 /*!< CCAP_T::PKTSL: PKTSHML Mask                */

#define CCAP_PKTSL_PKTSHNL_Pos            (8)                                               /*!< CCAP_T::PKTSL: PKTSHNL Position            */
#define CCAP_PKTSL_PKTSHNL_Msk            (0xfful << CCAP_PKTSL_PKTSHNL_Pos)                 /*!< CCAP_T::PKTSL: PKTSHNL Mask                */

#define CCAP_PKTSL_PKTSVML_Pos            (16)                                              /*!< CCAP_T::PKTSL: PKTSVML Position            */
#define CCAP_PKTSL_PKTSVML_Msk            (0xfful << CCAP_PKTSL_PKTSVML_Pos)                 /*!< CCAP_T::PKTSL: PKTSVML Mask                */

#define CCAP_PKTSL_PKTSVNL_Pos            (24)                                              /*!< CCAP_T::PKTSL: PKTSVNL Position            */
#define CCAP_PKTSL_PKTSVNL_Msk            (0xfful << CCAP_PKTSL_PKTSVNL_Pos)                 /*!< CCAP_T::PKTSL: PKTSVNL Mask                */

#define CCAP_FRCTL_FRM_Pos                (0)                                               /*!< CCAP_T::FRCTL: FRM Position                */
#define CCAP_FRCTL_FRM_Msk                (0x3ful << CCAP_FRCTL_FRM_Pos)                     /*!< CCAP_T::FRCTL: FRM Mask                    */

#define CCAP_FRCTL_FRN_Pos                (8)                                               /*!< CCAP_T::FRCTL: FRN Position                */
#define CCAP_FRCTL_FRN_Msk                (0x3ful << CCAP_FRCTL_FRN_Pos)                     /*!< CCAP_T::FRCTL: FRN Mask                    */

#define CCAP_STRIDE_PKTSTRIDE_Pos         (0)                                               /*!< CCAP_T::STRIDE: PKTSTRIDE Position         */
#define CCAP_STRIDE_PKTSTRIDE_Msk         (0x3ffful << CCAP_STRIDE_PKTSTRIDE_Pos)            /*!< CCAP_T::STRIDE: PKTSTRIDE Mask             */

#define CCAP_STRIDE_PLNSTRIDE_Pos         (16)                                              /*!< CCAP_T::STRIDE: PLNSTRIDE Position         */
#define CCAP_STRIDE_PLNSTRIDE_Msk         (0x3ffful << CCAP_STRIDE_PLNSTRIDE_Pos)            /*!< CCAP_T::STRIDE: PLNSTRIDE Mask             */

#define CCAP_FIFOTH_PLNVFTH_Pos           (0)                                               /*!< CCAP_T::FIFOTH: PLNVFTH Position           */
#define CCAP_FIFOTH_PLNVFTH_Msk           (0xful << CCAP_FIFOTH_PLNVFTH_Pos)                 /*!< CCAP_T::FIFOTH: PLNVFTH Mask               */

#define CCAP_FIFOTH_PLNUFTH_Pos           (8)                                               /*!< CCAP_T::FIFOTH: PLNUFTH Position           */
#define CCAP_FIFOTH_PLNUFTH_Msk           (0xful << CCAP_FIFOTH_PLNUFTH_Pos)                 /*!< CCAP_T::FIFOTH: PLNUFTH Mask               */

#define CCAP_FIFOTH_PLNYFTH_Pos           (16)                                              /*!< CCAP_T::FIFOTH: PLNYFTH Position           */
#define CCAP_FIFOTH_PLNYFTH_Msk           (0x1ful << CCAP_FIFOTH_PLNYFTH_Pos)                /*!< CCAP_T::FIFOTH: PLNYFTH Mask               */

#define CCAP_FIFOTH_PKTFTH_Pos            (24)                                              /*!< CCAP_T::FIFOTH: PKTFTH Position            */
#define CCAP_FIFOTH_PKTFTH_Msk            (0x1ful << CCAP_FIFOTH_PKTFTH_Pos)                 /*!< CCAP_T::FIFOTH: PKTFTH Mask                */

#define CCAP_FIFOTH_OVF_Pos               (31)                                              /*!< CCAP_T::FIFOTH: OVF Position               */
#define CCAP_FIFOTH_OVF_Msk               (0x1ul << CCAP_FIFOTH_OVF_Pos)                     /*!< CCAP_T::FIFOTH: OVF Mask                   */

#define CCAP_CMPADDR_CMPADDR_Pos          (0)                                               /*!< CCAP_T::CMPADDR: CMPADDR Position          */
#define CCAP_CMPADDR_CMPADDR_Msk          (0xfffffffful << CCAP_CMPADDR_CMPADDR_Pos)         /*!< CCAP_T::CMPADDR: CMPADDR Mask              */

#define CCAP_PKTSM_PKTSHMH_Pos            (0)                                               /*!< CCAP_T::PKTSM: PKTSHMH Position            */
#define CCAP_PKTSM_PKTSHMH_Msk            (0xfful << CCAP_PKTSM_PKTSHMH_Pos)                 /*!< CCAP_T::PKTSM: PKTSHMH Mask                */

#define CCAP_PKTSM_PKTSHNH_Pos            (8)                                               /*!< CCAP_T::PKTSM: PKTSHNH Position            */
#define CCAP_PKTSM_PKTSHNH_Msk            (0xfful << CCAP_PKTSM_PKTSHNH_Pos)                 /*!< CCAP_T::PKTSM: PKTSHNH Mask                */

#define CCAP_PKTSM_PKTSVMH_Pos            (16)                                              /*!< CCAP_T::PKTSM: PKTSVMH Position            */
#define CCAP_PKTSM_PKTSVMH_Msk            (0xfful << CCAP_PKTSM_PKTSVMH_Pos)                 /*!< CCAP_T::PKTSM: PKTSVMH Mask                */

#define CCAP_PKTSM_PKTSVNH_Pos            (24)                                              /*!< CCAP_T::PKTSM: PKTSVNH Position            */
#define CCAP_PKTSM_PKTSVNH_Msk            (0xfful << CCAP_PKTSM_PKTSVNH_Pos)                 /*!< CCAP_T::PKTSM: PKTSVNH Mask                */

#define CCAP_PKTBA0_BASEADDR_Pos          (0)                                               /*!< CCAP_T::PKTBA0: BASEADDR Position          */
#define CCAP_PKTBA0_BASEADDR_Msk          (0xfffffffful << CCAP_PKTBA0_BASEADDR_Pos)         /*!< CCAP_T::PKTBA0: BASEADDR Mask              */

/**@}*/ /* CCAP_CONST */
/**@}*/ /* end of CCAP register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __CCAP_REG_H__ */
