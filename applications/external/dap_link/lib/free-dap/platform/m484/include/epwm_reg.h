/**************************************************************************//**
 * @file     epwm_reg.h
 * @version  V1.00
 * @brief    EPWM register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __EPWM_REG_H__
#define __EPWM_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup EPWM Pulse Width Modulation Controller(EPWM)
    Memory Mapped Structure for EPWM Controller
@{ */

typedef struct
{
    /**
     * @var ECAPDAT_T::RCAPDAT
     * Offset: 0x20C  EPWM Rising Capture Data Register 0~5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |RCAPDAT   |EPWM Rising Capture Data (Read Only)
     * |        |          |When rising capture condition happened, the EPWM counter value will be saved in this register.
     * @var ECAPDAT_T::FCAPDAT
     * Offset: 0x210  EPWM Falling Capture Data Register 0~5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |FCAPDAT   |EPWM Falling Capture Data (Read Only)
     * |        |          |When falling capture condition happened, the EPWM counter value will be saved in this register.
     */
    __IO uint32_t RCAPDAT; /*!< [0x20C/0x214/0x21C/0x224/0x22C/0x234] EPWM Rising Capture Data Register 0~5 */
    __IO uint32_t FCAPDAT; /*!< [0x210/0x218/0x220/0x228/0x230/0x238] EPWM Falling Capture Data Register 0~5 */
} ECAPDAT_T;

typedef struct
{


    /**
     * @var EPWM_T::CTL0
     * Offset: 0x00  EPWM Control Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CTRLD0    |Center Re-load
     * |        |          |In up-down counter type, PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the center point of a period
     * |[1]     |CTRLD1    |Center Re-load
     * |        |          |In up-down counter type, PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the center point of a period
     * |[2]     |CTRLD2    |Center Re-load
     * |        |          |In up-down counter type, PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the center point of a period
     * |[3]     |CTRLD3    |Center Re-load
     * |        |          |In up-down counter type, PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the center point of a period
     * |[4]     |CTRLD4    |Center Re-load
     * |        |          |In up-down counter type, PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the center point of a period
     * |[5]     |CTRLD5    |Center Re-load
     * |        |          |In up-down counter type, PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the center point of a period
     * |[8]     |WINLDEN0  |Window Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point of each period when valid reload window is set
     * |        |          |The valid reload window is set by software write 1 to EPWM_LOAD register and cleared by hardware after load success.
     * |[9]     |WINLDEN1  |Window Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point of each period when valid reload window is set
     * |        |          |The valid reload window is set by software write 1 to EPWM_LOAD register and cleared by hardware after load success.
     * |[10]    |WINLDEN2  |Window Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point of each period when valid reload window is set
     * |        |          |The valid reload window is set by software write 1 to EPWM_LOAD register and cleared by hardware after load success.
     * |[11]    |WINLDEN3  |Window Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point of each period when valid reload window is set
     * |        |          |The valid reload window is set by software write 1 to EPWM_LOAD register and cleared by hardware after load success.
     * |[12]    |WINLDEN4  |Window Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point of each period when valid reload window is set
     * |        |          |The valid reload window is set by software write 1 to EPWM_LOAD register and cleared by hardware after load success.
     * |[13]    |WINLDEN5  |Window Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point of each period when valid reload window is set
     * |        |          |The valid reload window is set by software write 1 to EPWM_LOAD register and cleared by hardware after load success.
     * |[16]    |IMMLDEN0  |Immediately Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD/CMPDAT will load to PBUF and CMPBUF immediately when software update PERIOD/CMPDAT.
     * |        |          |Note: If IMMLDENn is enabled, WINLDENn and CTRLDn will be invalid.
     * |[17]    |IMMLDEN1  |Immediately Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD/CMPDAT will load to PBUF and CMPBUF immediately when software update PERIOD/CMPDAT.
     * |        |          |Note: If IMMLDENn is enabled, WINLDENn and CTRLDn will be invalid.
     * |[18]    |IMMLDEN2  |Immediately Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD/CMPDAT will load to PBUF and CMPBUF immediately when software update PERIOD/CMPDAT.
     * |        |          |Note: If IMMLDENn is enabled, WINLDENn and CTRLDn will be invalid.
     * |[19]    |IMMLDEN3  |Immediately Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD/CMPDAT will load to PBUF and CMPBUF immediately when software update PERIOD/CMPDAT.
     * |        |          |Note: If IMMLDENn is enabled, WINLDENn and CTRLDn will be invalid.
     * |[20]    |IMMLDEN4  |Immediately Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD/CMPDAT will load to PBUF and CMPBUF immediately when software update PERIOD/CMPDAT.
     * |        |          |Note: If IMMLDENn is enabled, WINLDENn and CTRLDn will be invalid.
     * |[21]    |IMMLDEN5  |Immediately Load Enable Bits
     * |        |          |0 = PERIOD will load to PBUF at the end point of each period
     * |        |          |CMPDAT will load to CMPBUF at the end point or center point of each period by setting CTRLD bit.
     * |        |          |1 = PERIOD/CMPDAT will load to PBUF and CMPBUF immediately when software update PERIOD/CMPDAT.
     * |        |          |Note: If IMMLDENn is enabled, WINLDENn and CTRLDn will be invalid.
     * |[24]    |GROUPEN   |Group Function Enable Bit(S)
     * |        |          |0 = The output waveform of each EPWM channel are independent.
     * |        |          |1 = Unify the EPWM_CH2 and EPWM_CH4 to output the same waveform as EPWM_CH0 and unify the EPWM_CH3 and EPWM_CH5 to output the same waveform as EPWM_CH1.
     * |[30]    |DBGHALT   |ICE Debug Mode Counter Halt (Write Protect)
     * |        |          |If counter halt is enabled, EPWM all counters will keep current value until exit ICE debug mode.
     * |        |          |0 = ICE debug mode counter halt disable.
     * |        |          |1 = ICE debug mode counter halt enable.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[31]    |DBGTRIOFF |ICE Debug Mode Acknowledge Disable (Write Protect)
     * |        |          |0 = ICE debug mode acknowledgement effects EPWM output.
     * |        |          |EPWM pin will be forced as tri-state while ICE debug mode acknowledged.
     * |        |          |1 = ICE debug mode acknowledgement disabled.
     * |        |          |EPWM pin will keep output no matter ICE debug mode acknowledged or not.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * @var EPWM_T::CTL1
     * Offset: 0x04  EPWM Control Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |CNTTYPE0  |EPWM Counter Behavior Type
     * |        |          |00 = Up counter type (supports in capture mode).
     * |        |          |01 = Down count type (supports in capture mode).
     * |        |          |10 = Up-down counter type.
     * |        |          |11 = Reserved.
     * |[3:2]   |CNTTYPE1  |EPWM Counter Behavior Type
     * |        |          |00 = Up counter type (supports in capture mode).
     * |        |          |01 = Down count type (supports in capture mode).
     * |        |          |10 = Up-down counter type.
     * |        |          |11 = Reserved.
     * |[5:4]   |CNTTYPE2  |EPWM Counter Behavior Type
     * |        |          |00 = Up counter type (supports in capture mode).
     * |        |          |01 = Down count type (supports in capture mode).
     * |        |          |10 = Up-down counter type.
     * |        |          |11 = Reserved.
     * |[7:6]   |CNTTYPE3  |EPWM Counter Behavior Type
     * |        |          |00 = Up counter type (supports in capture mode).
     * |        |          |01 = Down count type (supports in capture mode).
     * |        |          |10 = Up-down counter type.
     * |        |          |11 = Reserved.
     * |[9:8]   |CNTTYPE4  |EPWM Counter Behavior Type
     * |        |          |00 = Up counter type (supports in capture mode).
     * |        |          |01 = Down count type (supports in capture mode).
     * |        |          |10 = Up-down counter type.
     * |        |          |11 = Reserved.
     * |[11:10] |CNTTYPE5  |EPWM Counter Behavior Type
     * |        |          |00 = Up counter type (supports in capture mode).
     * |        |          |01 = Down count type (supports in capture mode).
     * |        |          |10 = Up-down counter type.
     * |        |          |11 = Reserved.
     * |[16]    |CNTMODE0  |EPWM Counter Mode
     * |        |          |0 = Auto-reload mode.
     * |        |          |1 = One-shot mode.
     * |[17]    |CNTMODE1  |EPWM Counter Mode
     * |        |          |0 = Auto-reload mode.
     * |        |          |1 = One-shot mode.
     * |[18]    |CNTMODE2  |EPWM Counter Mode
     * |        |          |0 = Auto-reload mode.
     * |        |          |1 = One-shot mode.
     * |[19]    |CNTMODE3  |EPWM Counter Mode
     * |        |          |0 = Auto-reload mode.
     * |        |          |1 = One-shot mode.
     * |[20]    |CNTMODE4  |EPWM Counter Mode
     * |        |          |0 = Auto-reload mode.
     * |        |          |1 = One-shot mode.
     * |[21]    |CNTMODE5  |EPWM Counter Mode
     * |        |          |0 = Auto-reload mode.
     * |        |          |1 = One-shot mode.
     * |[24]    |OUTMODE0  |EPWM Output Mode
     * |        |          |Each bit n controls the output mode of corresponding EPWM channel n.
     * |        |          |0 = EPWM independent mode.
     * |        |          |1 = EPWM complementary mode.
     * |        |          |Note: When operating in group function, these bits must all set to the same mode.
     * |[25]    |OUTMODE2  |EPWM Output Mode
     * |        |          |Each bit n controls the output mode of corresponding EPWM channel n.
     * |        |          |0 = EPWM independent mode.
     * |        |          |1 = EPWM complementary mode.
     * |        |          |Note: When operating in group function, these bits must all set to the same mode.
     * |[26]    |OUTMODE4  |EPWM Output Mode
     * |        |          |Each bit n controls the output mode of corresponding EPWM channel n.
     * |        |          |0 = EPWM independent mode.
     * |        |          |1 = EPWM complementary mode.
     * |        |          |Note: When operating in group function, these bits must all set to the same mode.
     * @var EPWM_T::SYNC
     * Offset: 0x08  EPWM Synchronization Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PHSEN0    |SYNC Phase Enable Bits
     * |        |          |0 = EPWM counter disable to load PHS value.
     * |        |          |1 = EPWM counter enable to load PHS value.
     * |[1]     |PHSEN2    |SYNC Phase Enable Bits
     * |        |          |0 = EPWM counter disable to load PHS value.
     * |        |          |1 = EPWM counter enable to load PHS value.
     * |[2]     |PHSEN4    |SYNC Phase Enable Bits
     * |        |          |0 = EPWM counter disable to load PHS value.
     * |        |          |1 = EPWM counter enable to load PHS value.
     * |[9:8]   |SINSRC0   |EPWM0_SYNC_IN Source Selection
     * |        |          |00 = Synchronize source from SYNC_IN or SWSYNC.
     * |        |          |01 = Counter equal to 0.
     * |        |          |10 = Counter equal to EPWM_CMPDATm, m denotes 1, 3, 5.
     * |        |          |11 = SYNC_OUT will not be generated.
     * |[11:10] |SINSRC2   |EPWM0_SYNC_IN Source Selection
     * |        |          |00 = Synchronize source from SYNC_IN or SWSYNC.
     * |        |          |01 = Counter equal to 0.
     * |        |          |10 = Counter equal to EPWM_CMPDATm, m denotes 1, 3, 5.
     * |        |          |11 = SYNC_OUT will not be generated.
     * |[13:12] |SINSRC4   |EPWM0_SYNC_IN Source Selection
     * |        |          |00 = Synchronize source from SYNC_IN or SWSYNC.
     * |        |          |01 = Counter equal to 0.
     * |        |          |10 = Counter equal to EPWM_CMPDATm, m denotes 1, 3, 5.
     * |        |          |11 = SYNC_OUT will not be generated.
     * |[16]    |SNFLTEN   |EPWM0_SYNC_IN Noise Filter Enable Bits
     * |        |          |0 = Noise filter of input pin EPWM0_SYNC_IN is Disabled.
     * |        |          |1 = Noise filter of input pin EPWM0_SYNC_IN is Enabled.
     * |[19:17] |SFLTCSEL  |SYNC Edge Detector Filter Clock Selection
     * |        |          |000 = Filter clock = HCLK.
     * |        |          |001 = Filter clock = HCLK/2.
     * |        |          |010 = Filter clock = HCLK/4.
     * |        |          |011 = Filter clock = HCLK/8.
     * |        |          |100 = Filter clock = HCLK/16.
     * |        |          |101 = Filter clock = HCLK/32.
     * |        |          |110 = Filter clock = HCLK/64.
     * |        |          |111 = Filter clock = HCLK/128.
     * |[22:20] |SFLTCNT   |SYNC Edge Detector Filter Count
     * |        |          |The register bits control the counter number of edge detector.
     * |[23]    |SINPINV   |SYNC Input Pin Inverse
     * |        |          |0 = The state of pin SYNC is passed to the negative edge detector.
     * |        |          |1 = The inversed state of pin SYNC is passed to the negative edge detector.
     * |[24]    |PHSDIR0   |EPWM Phase Direction Control
     * |        |          |0 = Control EPWM counter count decrement after synchronizing.
     * |        |          |1 = Control EPWM counter count increment after synchronizing.
     * |[25]    |PHSDIR2   |EPWM Phase Direction Control
     * |        |          |0 = Control EPWM counter count decrement after synchronizing.
     * |        |          |1 = Control EPWM counter count increment after synchronizing.
     * |[26]    |PHSDIR4   |EPWM Phase Direction Control
     * |        |          |0 = Control EPWM counter count decrement after synchronizing.
     * |        |          |1 = Control EPWM counter count increment after synchronizing.
     * @var EPWM_T::SWSYNC
     * Offset: 0x0C  EPWM Software Control Synchronization Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SWSYNC0   |Software SYNC Function
     * |        |          |When SINSRCn (EPWM_SYNC[13:8]) is selected to 0, SYNC_OUT source is come from SYNC_IN or this bit.
     * |[1]     |SWSYNC2   |Software SYNC Function
     * |        |          |When SINSRCn (EPWM_SYNC[13:8]) is selected to 0, SYNC_OUT source is come from SYNC_IN or this bit.
     * |[2]     |SWSYNC4   |Software SYNC Function
     * |        |          |When SINSRCn (EPWM_SYNC[13:8]) is selected to 0, SYNC_OUT source is come from SYNC_IN or this bit.
     * @var EPWM_T::CLKSRC
     * Offset: 0x10  EPWM Clock Source Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |ECLKSRC0  |EPWM_CH01 External Clock Source Select
     * |        |          |000 = EPWMx_CLK, x denotes 0 or 1.
     * |        |          |001 = TIMER0 overflow.
     * |        |          |010 = TIMER1 overflow.
     * |        |          |011 = TIMER2 overflow.
     * |        |          |100 = TIMER3 overflow.
     * |        |          |Others = Reserved.
     * |[10:8]  |ECLKSRC2  |EPWM_CH23 External Clock Source Select
     * |        |          |000 = EPWMx_CLK, x denotes 0 or 1.
     * |        |          |001 = TIMER0 overflow.
     * |        |          |010 = TIMER1 overflow.
     * |        |          |011 = TIMER2 overflow.
     * |        |          |100 = TIMER3 overflow.
     * |        |          |Others = Reserved.
     * |[18:16] |ECLKSRC4  |EPWM_CH45 External Clock Source Select
     * |        |          |000 = EPWMx_CLK, x denotes 0 or 1.
     * |        |          |001 = TIMER0 overflow.
     * |        |          |010 = TIMER1 overflow.
     * |        |          |011 = TIMER2 overflow.
     * |        |          |100 = TIMER3 overflow.
     * |        |          |Others = Reserved.
     * @var EPWM_T::CLKPSC[3]
     * Offset: 0x14  EPWM Clock Prescale Register 0/1, 2/3, 4/5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |CLKPSC    |EPWM Counter Clock Prescale
     * |        |          |The clock of EPWM counter is decided by clock prescaler
     * |        |          |Each EPWM pair share one EPWM counter clock prescaler
     * |        |          |The clock of EPWM counter is divided by (CLKPSC+ 1)
     * @var EPWM_T::CNTEN
     * Offset: 0x20  EPWM Counter Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTEN0    |EPWM Counter Enable Bits
     * |        |          |0 = EPWM Counter and clock prescaler Stop Running.
     * |        |          |1 = EPWM Counter and clock prescaler Start Running.
     * |[1]     |CNTEN1    |EPWM Counter Enable Bits
     * |        |          |0 = EPWM Counter and clock prescaler Stop Running.
     * |        |          |1 = EPWM Counter and clock prescaler Start Running.
     * |[2]     |CNTEN2    |EPWM Counter Enable Bits
     * |        |          |0 = EPWM Counter and clock prescaler Stop Running.
     * |        |          |1 = EPWM Counter and clock prescaler Start Running.
     * |[3]     |CNTEN3    |EPWM Counter Enable Bits
     * |        |          |0 = EPWM Counter and clock prescaler Stop Running.
     * |        |          |1 = EPWM Counter and clock prescaler Start Running.
     * |[4]     |CNTEN4    |EPWM Counter Enable Bits
     * |        |          |0 = EPWM Counter and clock prescaler Stop Running.
     * |        |          |1 = EPWM Counter and clock prescaler Start Running.
     * |[5]     |CNTEN5    |EPWM Counter Enable Bits
     * |        |          |0 = EPWM Counter and clock prescaler Stop Running.
     * |        |          |1 = EPWM Counter and clock prescaler Start Running.
     * @var EPWM_T::CNTCLR
     * Offset: 0x24  EPWM Clear Counter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTCLR0   |Clear EPWM Counter Control Bit
     * |        |          |It is automatically cleared by hardware. Each bit n controls the corresponding EPWM channel n.
     * |        |          |0 = No effect.
     * |        |          |1 = Clear 16-bit EPWM counter to 0000H.
     * |[1]     |CNTCLR1   |Clear EPWM Counter Control Bit
     * |        |          |It is automatically cleared by hardware. Each bit n controls the corresponding EPWM channel n.
     * |        |          |0 = No effect.
     * |        |          |1 = Clear 16-bit EPWM counter to 0000H.
     * |[2]     |CNTCLR2   |Clear EPWM Counter Control Bit
     * |        |          |It is automatically cleared by hardware. Each bit n controls the corresponding EPWM channel n.
     * |        |          |0 = No effect.
     * |        |          |1 = Clear 16-bit EPWM counter to 0000H.
     * |[3]     |CNTCLR3   |Clear EPWM Counter Control Bit
     * |        |          |It is automatically cleared by hardware. Each bit n controls the corresponding EPWM channel n.
     * |        |          |0 = No effect.
     * |        |          |1 = Clear 16-bit EPWM counter to 0000H.
     * |[4]     |CNTCLR4   |Clear EPWM Counter Control Bit
     * |        |          |It is automatically cleared by hardware. Each bit n controls the corresponding EPWM channel n.
     * |        |          |0 = No effect.
     * |        |          |1 = Clear 16-bit EPWM counter to 0000H.
     * |[5]     |CNTCLR5   |Clear EPWM Counter Control Bit
     * |        |          |It is automatically cleared by hardware. Each bit n controls the corresponding EPWM channel n.
     * |        |          |0 = No effect.
     * |        |          |1 = Clear 16-bit EPWM counter to 0000H.
     * @var EPWM_T::LOAD
     * Offset: 0x28  EPWM Load Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |LOAD0     |Re-load EPWM Comparator Register (CMPDAT) Control Bit
     * |        |          |This bit is software write, hardware clear when current EPWM period end.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Set load window of window loading mode.
     * |        |          |Read Operation:
     * |        |          |0 = No load window is set.
     * |        |          |1 = Load window is set.
     * |        |          |Note: This bit only use in window loading mode, WINLDENn(EPWM_CTL0[13:8]) = 1.
     * |[1]     |LOAD1     |Re-load EPWM Comparator Register (CMPDAT) Control Bit
     * |        |          |This bit is software write, hardware clear when current EPWM period end.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Set load window of window loading mode.
     * |        |          |Read Operation:
     * |        |          |0 = No load window is set.
     * |        |          |1 = Load window is set.
     * |        |          |Note: This bit only use in window loading mode, WINLDENn(EPWM_CTL0[13:8]) = 1.
     * |[2]     |LOAD2     |Re-load EPWM Comparator Register (CMPDAT) Control Bit
     * |        |          |This bit is software write, hardware clear when current EPWM period end.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Set load window of window loading mode.
     * |        |          |Read Operation:
     * |        |          |0 = No load window is set.
     * |        |          |1 = Load window is set.
     * |        |          |Note: This bit only use in window loading mode, WINLDENn(EPWM_CTL0[13:8]) = 1.
     * |[3]     |LOAD3     |Re-load EPWM Comparator Register (CMPDAT) Control Bit
     * |        |          |This bit is software write, hardware clear when current EPWM period end.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Set load window of window loading mode.
     * |        |          |Read Operation:
     * |        |          |0 = No load window is set.
     * |        |          |1 = Load window is set.
     * |        |          |Note: This bit only use in window loading mode, WINLDENn(EPWM_CTL0[13:8]) = 1.
     * |[4]     |LOAD4     |Re-load EPWM Comparator Register (CMPDAT) Control Bit
     * |        |          |This bit is software write, hardware clear when current EPWM period end.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Set load window of window loading mode.
     * |        |          |Read Operation:
     * |        |          |0 = No load window is set.
     * |        |          |1 = Load window is set.
     * |        |          |Note: This bit only use in window loading mode, WINLDENn(EPWM_CTL0[13:8]) = 1.
     * |[5]     |LOAD5     |Re-load EPWM Comparator Register (CMPDAT) Control Bit
     * |        |          |This bit is software write, hardware clear when current EPWM period end.
     * |        |          |Write Operation:
     * |        |          |0 = No effect.
     * |        |          |1 = Set load window of window loading mode.
     * |        |          |Read Operation:
     * |        |          |0 = No load window is set.
     * |        |          |1 = Load window is set.
     * |        |          |Note: This bit only use in window loading mode, WINLDENn(EPWM_CTL0[13:8]) = 1.
     * @var EPWM_T::PERIOD[6]
     * Offset: 0x30  EPWM Period Register 0~5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |PERIOD    |EPWM Period Register
     * |        |          |Up-Count mode: In this mode, EPWM counter counts from 0 to PERIOD, and restarts from 0.
     * |        |          |Down-Count mode: In this mode, EPWM counter counts from PERIOD to 0, and restarts from PERIOD.
     * |        |          |EPWM period time = (PERIOD+1) * EPWM_CLK period.
     * |        |          |Up-Down-Count mode: In this mode, EPWM counter counts from 0 to PERIOD, then decrements to 0 and repeats again.
     * |        |          |EPWM period time = 2 * PERIOD * EPWM_CLK period.
     * @var EPWM_T::CMPDAT[6]
     * Offset: 0x50  EPWM Comparator Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CMP       |EPWM Comparator Register
     * |        |          |CMP use to compare with CNTR to generate EPWM waveform, interrupt and trigger EADC/DAC.
     * |        |          |In independent mode, CMPDAT0~5 denote as 6 independent EPWM_CH0~5 compared point.
     * |        |          |In complementary mode, CMPDAT0, 2, 4 denote as first compared point, and CMPDAT1, 3, 5 denote as second compared point for the corresponding 3 complementary pairs EPWM_CH0 and EPWM_CH1, EPWM_CH2 and EPWM_CH3, EPWM_CH4 and EPWM_CH5.
     * @var EPWM_T::DTCTL[3]
     * Offset: 0x70  EPWM Dead-Time Control Register 0/1,2/3,4/5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |DTCNT     |Dead-time Counter (Write Protect)
     * |        |          |The dead-time can be calculated from the following formula:
     * |        |          |Dead-time = (DTCNT[11:0]+1) * EPWM_CLK period.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[16]    |DTEN      |Enable Dead-time Insertion for EPWM Pair (EPWM_CH0, EPWM_CH1) (EPWM_CH2, EPWM_CH3) (EPWM_CH4, EPWM_CH5) (Write Protect)
     * |        |          |Dead-time insertion is only active when this pair of complementary EPWM is enabled
     * |        |          |If dead- time insertion is inactive, the outputs of pin pair are complementary without any delay.
     * |        |          |0 = Dead-time insertion Disabled on the pin pair.
     * |        |          |1 = Dead-time insertion Enabled on the pin pair.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[24]    |DTCKSEL   |Dead-time Clock Select (Write Protect)
     * |        |          |0 = Dead-time clock source from EPWM_CLK.
     * |        |          |1 = Dead-time clock source from prescaler output.
     * |        |          |Note: This register is write protected. Refer toREGWRPROT register.
     * @var EPWM_T::PHS[3]
     * Offset: 0x80  EPWM Counter Phase Register 0/1,2/3,4/5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |PHS       |EPWM Synchronous Start Phase Bits
     * |        |          |PHS determines the EPWM synchronous start phase value. These bits only use in synchronous function.
     * @var EPWM_T::CNT[6]
     * Offset: 0x90  EPWM Counter Register 0~5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CNT       |EPWM Data Register (Read Only)
     * |        |          |User can monitor CNTR to know the current value in 16-bit period counter.
     * |[16]    |DIRF      |EPWM Direction Indicator Flag (Read Only)
     * |        |          |0 = Counter is Down count.
     * |        |          |1 = Counter is UP count.
     * @var EPWM_T::WGCTL0
     * Offset: 0xB0  EPWM Generation Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |ZPCTL0    |EPWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM zero point output Low.
     * |        |          |10 = EPWM zero point output High.
     * |        |          |11 = EPWM zero point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to zero.
     * |[3:2]   |ZPCTL1    |EPWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM zero point output Low.
     * |        |          |10 = EPWM zero point output High.
     * |        |          |11 = EPWM zero point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to zero.
     * |[5:4]   |ZPCTL2    |EPWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM zero point output Low.
     * |        |          |10 = EPWM zero point output High.
     * |        |          |11 = EPWM zero point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to zero.
     * |[7:6]   |ZPCTL3    |EPWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM zero point output Low.
     * |        |          |10 = EPWM zero point output High.
     * |        |          |11 = EPWM zero point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to zero.
     * |[9:8]   |ZPCTL4    |EPWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM zero point output Low.
     * |        |          |10 = EPWM zero point output High.
     * |        |          |11 = EPWM zero point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to zero.
     * |[11:10] |ZPCTL5    |EPWM Zero Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM zero point output Low.
     * |        |          |10 = EPWM zero point output High.
     * |        |          |11 = EPWM zero point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to zero.
     * |[17:16] |PRDPCTL0  |EPWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM period (center) point output Low.
     * |        |          |10 = EPWM period (center) point output High.
     * |        |          |11 = EPWM period (center) point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to (PERIODn+1).
     * |        |          |Note: This bit is center point control when EPWM counter operating in up-down counter type.
     * |[19:18] |PRDPCTL1  |EPWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM period (center) point output Low.
     * |        |          |10 = EPWM period (center) point output High.
     * |        |          |11 = EPWM period (center) point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to (PERIODn+1).
     * |        |          |Note: This bit is center point control when EPWM counter operating in up-down counter type.
     * |[21:20] |PRDPCTL2  |EPWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM period (center) point output Low.
     * |        |          |10 = EPWM period (center) point output High.
     * |        |          |11 = EPWM period (center) point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to (PERIODn+1).
     * |        |          |Note: This bit is center point control when EPWM counter operating in up-down counter type.
     * |[23:22] |PRDPCTL3  |EPWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM period (center) point output Low.
     * |        |          |10 = EPWM period (center) point output High.
     * |        |          |11 = EPWM period (center) point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to (PERIODn+1).
     * |        |          |Note: This bit is center point control when EPWM counter operating in up-down counter type.
     * |[25:24] |PRDPCTL4  |EPWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM period (center) point output Low.
     * |        |          |10 = EPWM period (center) point output High.
     * |        |          |11 = EPWM period (center) point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to (PERIODn+1).
     * |        |          |Note: This bit is center point control when EPWM counter operating in up-down counter type.
     * |[27:26] |PRDPCTL5  |EPWM Period (Center) Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM period (center) point output Low.
     * |        |          |10 = EPWM period (center) point output High.
     * |        |          |11 = EPWM period (center) point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter count to (PERIODn+1).
     * |        |          |Note: This bit is center point control when EPWM counter operating in up-down counter type.
     * @var EPWM_T::WGCTL1
     * Offset: 0xB4  EPWM Generation Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |CMPUCTL0  |EPWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare up point output Low.
     * |        |          |10 = EPWM compare up point output High.
     * |        |          |11 = EPWM compare up point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter up count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPUCTL1, 3, 5 use as another CMPUCTL for channel 0, 2, 4.
     * |[3:2]   |CMPUCTL1  |EPWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare up point output Low.
     * |        |          |10 = EPWM compare up point output High.
     * |        |          |11 = EPWM compare up point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter up count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPUCTL1, 3, 5 use as another CMPUCTL for channel 0, 2, 4.
     * |[5:4]   |CMPUCTL2  |EPWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare up point output Low.
     * |        |          |10 = EPWM compare up point output High.
     * |        |          |11 = EPWM compare up point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter up count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPUCTL1, 3, 5 use as another CMPUCTL for channel 0, 2, 4.
     * |[7:6]   |CMPUCTL3  |EPWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare up point output Low.
     * |        |          |10 = EPWM compare up point output High.
     * |        |          |11 = EPWM compare up point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter up count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPUCTL1, 3, 5 use as another CMPUCTL for channel 0, 2, 4.
     * |[9:8]   |CMPUCTL4  |EPWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare up point output Low.
     * |        |          |10 = EPWM compare up point output High.
     * |        |          |11 = EPWM compare up point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter up count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPUCTL1, 3, 5 use as another CMPUCTL for channel 0, 2, 4.
     * |[11:10] |CMPUCTL5  |EPWM Compare Up Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare up point output Low.
     * |        |          |10 = EPWM compare up point output High.
     * |        |          |11 = EPWM compare up point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter up count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPUCTL1, 3, 5 use as another CMPUCTL for channel 0, 2, 4.
     * |[17:16] |CMPDCTL0  |EPWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare down point output Low.
     * |        |          |10 = EPWM compare down point output High.
     * |        |          |11 = EPWM compare down point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter down count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPDCTL1, 3, 5 use as another CMPDCTL for channel 0, 2, 4.
     * |[19:18] |CMPDCTL1  |EPWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare down point output Low.
     * |        |          |10 = EPWM compare down point output High.
     * |        |          |11 = EPWM compare down point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter down count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPDCTL1, 3, 5 use as another CMPDCTL for channel 0, 2, 4.
     * |[21:20] |CMPDCTL2  |EPWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare down point output Low.
     * |        |          |10 = EPWM compare down point output High.
     * |        |          |11 = EPWM compare down point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter down count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPDCTL1, 3, 5 use as another CMPDCTL for channel 0, 2, 4.
     * |[23:22] |CMPDCTL3  |EPWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare down point output Low.
     * |        |          |10 = EPWM compare down point output High.
     * |        |          |11 = EPWM compare down point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter down count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPDCTL1, 3, 5 use as another CMPDCTL for channel 0, 2, 4.
     * |[25:24] |CMPDCTL4  |EPWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare down point output Low.
     * |        |          |10 = EPWM compare down point output High.
     * |        |          |11 = EPWM compare down point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter down count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPDCTL1, 3, 5 use as another CMPDCTL for channel 0, 2, 4.
     * |[27:26] |CMPDCTL5  |EPWM Compare Down Point Control
     * |        |          |00 = Do nothing.
     * |        |          |01 = EPWM compare down point output Low.
     * |        |          |10 = EPWM compare down point output High.
     * |        |          |11 = EPWM compare down point output Toggle.
     * |        |          |EPWM can control output level when EPWM counter down count to CMPDAT.
     * |        |          |Note: In complementary mode, CMPDCTL1, 3, 5 use as another CMPDCTL for channel 0, 2, 4.
     * @var EPWM_T::MSKEN
     * Offset: 0xB8  EPWM Mask Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MSKEN0    |EPWM Mask Enable Bits
     * |        |          |The EPWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding EPWM channel n will output MSKDATn (EPWM_MSK[5:0]) data.
     * |        |          |0 = EPWM output signal is non-masked.
     * |        |          |1 = EPWM output signal is masked and output MSKDATn data.
     * |[1]     |MSKEN1    |EPWM Mask Enable Bits
     * |        |          |The EPWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding EPWM channel n will output MSKDATn (EPWM_MSK[5:0]) data.
     * |        |          |0 = EPWM output signal is non-masked.
     * |        |          |1 = EPWM output signal is masked and output MSKDATn data.
     * |[2]     |MSKEN2    |EPWM Mask Enable Bits
     * |        |          |The EPWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding EPWM channel n will output MSKDATn (EPWM_MSK[5:0]) data.
     * |        |          |0 = EPWM output signal is non-masked.
     * |        |          |1 = EPWM output signal is masked and output MSKDATn data.
     * |[3]     |MSKEN3    |EPWM Mask Enable Bits
     * |        |          |The EPWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding EPWM channel n will output MSKDATn (EPWM_MSK[5:0]) data.
     * |        |          |0 = EPWM output signal is non-masked.
     * |        |          |1 = EPWM output signal is masked and output MSKDATn data.
     * |[4]     |MSKEN4    |EPWM Mask Enable Bits
     * |        |          |The EPWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding EPWM channel n will output MSKDATn (EPWM_MSK[5:0]) data.
     * |        |          |0 = EPWM output signal is non-masked.
     * |        |          |1 = EPWM output signal is masked and output MSKDATn data.
     * |[5]     |MSKEN5    |EPWM Mask Enable Bits
     * |        |          |The EPWM output signal will be masked when this bit is enabled
     * |        |          |The corresponding EPWM channel n will output MSKDATn (EPWM_MSK[5:0]) data.
     * |        |          |0 = EPWM output signal is non-masked.
     * |        |          |1 = EPWM output signal is masked and output MSKDATn data.
     * @var EPWM_T::MSK
     * Offset: 0xBC  EPWM Mask Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MSKDAT0   |EPWM Mask Data Bit
     * |        |          |This data bit control the state of EPWMn output pin, if corresponding mask function is enabled.
     * |        |          |0 = Output logic low to EPWM channel n.
     * |        |          |1 = Output logic high to EPWM channel n.
     * |[1]     |MSKDAT1   |EPWM Mask Data Bit
     * |        |          |This data bit control the state of EPWMn output pin, if corresponding mask function is enabled.
     * |        |          |0 = Output logic low to EPWM channel n.
     * |        |          |1 = Output logic high to EPWM channel n.
     * |[2]     |MSKDAT2   |EPWM Mask Data Bit
     * |        |          |This data bit control the state of EPWMn output pin, if corresponding mask function is enabled.
     * |        |          |0 = Output logic low to EPWM channel n.
     * |        |          |1 = Output logic high to EPWM channel n.
     * |[3]     |MSKDAT3   |EPWM Mask Data Bit
     * |        |          |This data bit control the state of EPWMn output pin, if corresponding mask function is enabled.
     * |        |          |0 = Output logic low to EPWM channel n.
     * |        |          |1 = Output logic high to EPWM channel n.
     * |[4]     |MSKDAT4   |EPWM Mask Data Bit
     * |        |          |This data bit control the state of EPWMn output pin, if corresponding mask function is enabled.
     * |        |          |0 = Output logic low to EPWM channel n.
     * |        |          |1 = Output logic high to EPWM channel n.
     * |[5]     |MSKDAT5   |EPWM Mask Data Bit
     * |        |          |This data bit control the state of EPWMn output pin, if corresponding mask function is enabled.
     * |        |          |0 = Output logic low to EPWM channel n.
     * |        |          |1 = Output logic high to EPWM channel n.
     * @var EPWM_T::BNF
     * Offset: 0xC0  EPWM Brake Noise Filter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRK0NFEN  |EPWM Brake 0 Noise Filter Enable Bit
     * |        |          |0 = Noise filter of EPWM Brake 0 Disabled.
     * |        |          |1 = Noise filter of EPWM Brake 0 Enabled.
     * |[3:1]   |BRK0NFSEL |Brake 0 Edge Detector Filter Clock Selection
     * |        |          |000 = Filter clock = HCLK.
     * |        |          |001 = Filter clock = HCLK/2.
     * |        |          |010 = Filter clock = HCLK/4.
     * |        |          |011 = Filter clock = HCLK/8.
     * |        |          |100 = Filter clock = HCLK/16.
     * |        |          |101 = Filter clock = HCLK/32.
     * |        |          |110 = Filter clock = HCLK/64.
     * |        |          |111 = Filter clock = HCLK/128.
     * |[6:4]   |BRK0FCNT  |Brake 0 Edge Detector Filter Count
     * |        |          |The register bits control the Brake0 filter counter to count from 0 to BRK1FCNT.
     * |[7]     |BRK0PINV  |Brake 0 Pin Inverse
     * |        |          |0 = The state of pin EPWMx_BRAKE0 is passed to the negative edge detector.
     * |        |          |1 = The inversed state of pin EPWMx_BRAKE10 is passed to the negative edge detector.
     * |[8]     |BRK1NFEN  |EPWM Brake 1 Noise Filter Enable Bit
     * |        |          |0 = Noise filter of EPWM Brake 1 Disabled.
     * |        |          |1 = Noise filter of EPWM Brake 1 Enabled.
     * |[11:9]  |BRK1NFSEL |Brake 1 Edge Detector Filter Clock Selection
     * |        |          |000 = Filter clock = HCLK.
     * |        |          |001 = Filter clock = HCLK/2.
     * |        |          |010 = Filter clock = HCLK/4.
     * |        |          |011 = Filter clock = HCLK/8.
     * |        |          |100 = Filter clock = HCLK/16.
     * |        |          |101 = Filter clock = HCLK/32.
     * |        |          |110 = Filter clock = HCLK/64.
     * |        |          |111 = Filter clock = HCLK/128.
     * |[14:12] |BRK1FCNT  |Brake 1 Edge Detector Filter Count
     * |        |          |The register bits control the Brake1 filter counter to count from 0 to BRK1FCNT.
     * |[15]    |BRK1PINV  |Brake 1 Pin Inverse
     * |        |          |0 = The state of pin EPWMx_BRAKE1 is passed to the negative edge detector.
     * |        |          |1 = The inversed state of pin EPWMx_BRAKE1 is passed to the negative edge detector.
     * |[16]    |BK0SRC    |Brake 0 Pin Source Select
     * |        |          |For EPWM0 setting:
     * |        |          |0 = Brake 0 pin source come from EPWM0_BRAKE0.
     * |        |          |1 = Brake 0 pin source come from EPWM1_BRAKE0.
     * |        |          |For EPWM1 setting:
     * |        |          |0 = Brake 0 pin source come from EPWM1_BRAKE0.
     * |        |          |1 = Brake 0 pin source come from EPWM0_BRAKE0.
     * |[24]    |BK1SRC    |Brake 1 Pin Source Select
     * |        |          |For EPWM0 setting:
     * |        |          |0 = Brake 1 pin source come from EPWM0_BRAKE1.
     * |        |          |1 = Brake 1 pin source come from EPWM1_BRAKE1.
     * |        |          |For EPWM1 setting:
     * |        |          |0 = Brake 1 pin source come from EPWM1_BRAKE1.
     * |        |          |1 = Brake 1 pin source come from EPWM0_BRAKE1.
     * @var EPWM_T::FAILBRK
     * Offset: 0xC4  EPWM System Fail Brake Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CSSBRKEN  |Clock Security System Detection Trigger EPWM Brake Function 0 Enable Bit
     * |        |          |0 = Brake Function triggered by CSS detection Disabled.
     * |        |          |1 = Brake Function triggered by CSS detection Enabled.
     * |[1]     |BODBRKEN  |Brown-out Detection Trigger EPWM Brake Function 0 Enable Bit
     * |        |          |0 = Brake Function triggered by BOD Disabled.
     * |        |          |1 = Brake Function triggered by BOD Enabled.
     * |[2]     |RAMBRKEN  |SRAM Parity Error Detection Trigger EPWM Brake Function 0 Enable Bit
     * |        |          |0 = Brake Function triggered by SRAM parity error detection Disabled.
     * |        |          |1 = Brake Function triggered by SRAM parity error detection Enabled.
     * |[3]     |CORBRKEN  |Core Lockup Detection Trigger EPWM Brake Function 0 Enable Bit
     * |        |          |0 = Brake Function triggered by Core lockup detection Disabled.
     * |        |          |1 = Brake Function triggered by Core lockup detection Enabled.
     * @var EPWM_T::BRKCTL[3]
     * Offset: 0xC8  EPWM Brake Edge Detect Control Register 0/1,2/3,4/5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CPO0EBEN  |Enable ACMP0_O Digital Output As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = ACMP0_O as edge-detect brake source Disabled.
     * |        |          |1 = ACMP0_O as edge-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[1]     |CPO1EBEN  |Enable ACMP1_O Digital Output As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = ACMP1_O as edge-detect brake source Disabled.
     * |        |          |1 = ACMP1_O as edge-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[4]     |BRKP0EEN  |Enable EPWMx_BRAKE0 Pin As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = EPWMx_BRAKE0 pin as edge-detect brake source Disabled.
     * |        |          |1 = EPWMx_BRAKE0 pin as edge-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[5]     |BRKP1EEN  |Enable EPWMx_BRAKE1 Pin As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = EPWMx_BRAKE1 pin as edge-detect brake source Disabled.
     * |        |          |1 = EPWMx_BRAKE1 pin as edge-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[7]     |SYSEBEN   |Enable System Fail As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = System Fail condition as edge-detect brake source Disabled.
     * |        |          |1 = System Fail condition as edge-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[8]     |CPO0LBEN  |Enable ACMP0_O Digital Output As Level-detect Brake Source (Write Protect)
     * |        |          |0 = ACMP0_O as level-detect brake source Disabled.
     * |        |          |1 = ACMP0_O as level-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[9]     |CPO1LBEN  |Enable ACMP1_O Digital Output As Level-detect Brake Source (Write Protect)
     * |        |          |0 = ACMP1_O as level-detect brake source Disabled.
     * |        |          |1 = ACMP1_O as level-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[12]    |BRKP0LEN  |Enable BKP0 Pin As Level-detect Brake Source (Write Protect)
     * |        |          |0 = EPWMx_BRAKE0 pin as level-detect brake source Disabled.
     * |        |          |1 = EPWMx_BRAKE0 pin as level-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[13]    |BRKP1LEN  |Enable BKP1 Pin As Level-detect Brake Source (Write Protect)
     * |        |          |0 = EPWMx_BRAKE1 pin as level-detect brake source Disabled.
     * |        |          |1 = EPWMx_BRAKE1 pin as level-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[15]    |SYSLBEN   |Enable System Fail As Level-detect Brake Source (Write Protect)
     * |        |          |0 = System Fail condition as level-detect brake source Disabled.
     * |        |          |1 = System Fail condition as level-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[17:16] |BRKAEVEN  |EPWM Brake Action Select for Even Channel (Write Protect)
     * |        |          |00 = EPWMx brake event will not affect even channels output.
     * |        |          |01 = EPWM even channel output tri-state when EPWMx brake event happened.
     * |        |          |10 = EPWM even channel output low level when EPWMx brake event happened.
     * |        |          |11 = EPWM even channel output high level when EPWMx brake event happened.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[19:18] |BRKAODD   |EPWM Brake Action Select for Odd Channel (Write Protect)
     * |        |          |00 = EPWMx brake event will not affect odd channels output.
     * |        |          |01 = EPWM odd channel output tri-state when EPWMx brake event happened.
     * |        |          |10 = EPWM odd channel output low level when EPWMx brake event happened.
     * |        |          |11 = EPWM odd channel output high level when EPWMx brake event happened.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[20]    |EADCEBEN  |Enable EADC Result Monitor (EADCRM) As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = EADCRM as edge-detect brake source Disabled.
     * |        |          |1 = EADCRM as edge-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[28]    |EADCLBEN  |Enable EADC Result Monitor (EADCRM) As Level-detect Brake Source (Write Protect)
     * |        |          |0 = EADCRM as level-detect brake source Disabled.
     * |        |          |1 = EADCRM as level-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * @var EPWM_T::POLCTL
     * Offset: 0xD4  EPWM Pin Polar Inverse Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PINV0     |EPWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of EPWM output.
     * |        |          |0 = EPWM output polar inverse Disabled.
     * |        |          |1 = EPWM output polar inverse Enabled.
     * |[1]     |PINV1     |EPWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of EPWM output.
     * |        |          |0 = EPWM output polar inverse Disabled.
     * |        |          |1 = EPWM output polar inverse Enabled.
     * |[2]     |PINV2     |EPWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of EPWM output.
     * |        |          |0 = EPWM output polar inverse Disabled.
     * |        |          |1 = EPWM output polar inverse Enabled.
     * |[3]     |PINV3     |EPWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of EPWM output.
     * |        |          |0 = EPWM output polar inverse Disabled.
     * |        |          |1 = EPWM output polar inverse Enabled.
     * |[4]     |PINV4     |EPWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of EPWM output.
     * |        |          |0 = EPWM output polar inverse Disabled.
     * |        |          |1 = EPWM output polar inverse Enabled.
     * |[5]     |PINV5     |EPWM PIN Polar Inverse Control
     * |        |          |The register controls polarity state of EPWM output.
     * |        |          |0 = EPWM output polar inverse Disabled.
     * |        |          |1 = EPWM output polar inverse Enabled.
     * @var EPWM_T::POEN
     * Offset: 0xD8  EPWM Output Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |POEN0     |EPWM Pin Output Enable Bits
     * |        |          |0 = EPWM pin at tri-state.
     * |        |          |1 = EPWM pin in output mode.
     * |[1]     |POEN1     |EPWM Pin Output Enable Bits
     * |        |          |0 = EPWM pin at tri-state.
     * |        |          |1 = EPWM pin in output mode.
     * |[2]     |POEN2     |EPWM Pin Output Enable Bits
     * |        |          |0 = EPWM pin at tri-state.
     * |        |          |1 = EPWM pin in output mode.
     * |[3]     |POEN3     |EPWM Pin Output Enable Bits
     * |        |          |0 = EPWM pin at tri-state.
     * |        |          |1 = EPWM pin in output mode.
     * |[4]     |POEN4     |EPWM Pin Output Enable Bits
     * |        |          |0 = EPWM pin at tri-state.
     * |        |          |1 = EPWM pin in output mode.
     * |[5]     |POEN5     |EPWM Pin Output Enable Bits
     * |        |          |0 = EPWM pin at tri-state.
     * |        |          |1 = EPWM pin in output mode.
     * @var EPWM_T::SWBRK
     * Offset: 0xDC  EPWM Software Brake Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRKETRG0  |EPWM Edge Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger edge brake, and set BRKEIFn to 1 in EPWM_INTSTS1 register.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[1]     |BRKETRG2  |EPWM Edge Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger edge brake, and set BRKEIFn to 1 in EPWM_INTSTS1 register.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[2]     |BRKETRG4  |EPWM Edge Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger edge brake, and set BRKEIFn to 1 in EPWM_INTSTS1 register.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[8]     |BRKLTRG0  |EPWM Level Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger level brake, and set BRKLIFn to 1 in EPWM_INTSTS1 register.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[9]     |BRKLTRG2  |EPWM Level Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger level brake, and set BRKLIFn to 1 in EPWM_INTSTS1 register.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[10]    |BRKLTRG4  |EPWM Level Brake Software Trigger (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger level brake, and set BRKLIFn to 1 in EPWM_INTSTS1 register.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * @var EPWM_T::INTEN0
     * Offset: 0xE0  EPWM Interrupt Enable Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ZIEN0     |EPWM Zero Point Interrupt Enable Bits
     * |        |          |0 = Zero point interrupt Disabled.
     * |        |          |1 = Zero point interrupt Enabled.
     * |        |          |Note: Odd channels will read always 0 at complementary mode.
     * |[1]     |ZIEN1     |EPWM Zero Point Interrupt Enable Bits
     * |        |          |0 = Zero point interrupt Disabled.
     * |        |          |1 = Zero point interrupt Enabled.
     * |        |          |Note: Odd channels will read always 0 at complementary mode.
     * |[2]     |ZIEN2     |EPWM Zero Point Interrupt Enable Bits
     * |        |          |0 = Zero point interrupt Disabled.
     * |        |          |1 = Zero point interrupt Enabled.
     * |        |          |Note: Odd channels will read always 0 at complementary mode.
     * |[3]     |ZIEN3     |EPWM Zero Point Interrupt Enable Bits
     * |        |          |0 = Zero point interrupt Disabled.
     * |        |          |1 = Zero point interrupt Enabled.
     * |        |          |Note: Odd channels will read always 0 at complementary mode.
     * |[4]     |ZIEN4     |EPWM Zero Point Interrupt Enable Bits
     * |        |          |0 = Zero point interrupt Disabled.
     * |        |          |1 = Zero point interrupt Enabled.
     * |        |          |Note: Odd channels will read always 0 at complementary mode.
     * |[5]     |ZIEN5     |EPWM Zero Point Interrupt Enable Bits
     * |        |          |0 = Zero point interrupt Disabled.
     * |        |          |1 = Zero point interrupt Enabled.
     * |        |          |Note: Odd channels will read always 0 at complementary mode.
     * |[8]     |PIEN0     |EPWM Period Point Interrupt Enable Bits
     * |        |          |0 = Period point interrupt Disabled.
     * |        |          |1 = Period point interrupt Enabled.
     * |        |          |Note1: When up-down counter type period point means center point.
     * |        |          |Note2: Odd channels will read always 0 at complementary mode.
     * |[9]     |PIEN1     |EPWM Period Point Interrupt Enable Bits
     * |        |          |0 = Period point interrupt Disabled.
     * |        |          |1 = Period point interrupt Enabled.
     * |        |          |Note1: When up-down counter type period point means center point.
     * |        |          |Note2: Odd channels will read always 0 at complementary mode.
     * |[10]    |PIEN2     |EPWM Period Point Interrupt Enable Bits
     * |        |          |0 = Period point interrupt Disabled.
     * |        |          |1 = Period point interrupt Enabled.
     * |        |          |Note1: When up-down counter type period point means center point.
     * |        |          |Note2: Odd channels will read always 0 at complementary mode.
     * |[11]    |PIEN3     |EPWM Period Point Interrupt Enable Bits
     * |        |          |0 = Period point interrupt Disabled.
     * |        |          |1 = Period point interrupt Enabled.
     * |        |          |Note1: When up-down counter type period point means center point.
     * |        |          |Note2: Odd channels will read always 0 at complementary mode.
     * |[12]    |PIEN4     |EPWM Period Point Interrupt Enable Bits
     * |        |          |0 = Period point interrupt Disabled.
     * |        |          |1 = Period point interrupt Enabled.
     * |        |          |Note1: When up-down counter type period point means center point.
     * |        |          |Note2: Odd channels will read always 0 at complementary mode.
     * |[13]    |PIEN5     |EPWM Period Point Interrupt Enable Bits
     * |        |          |0 = Period point interrupt Disabled.
     * |        |          |1 = Period point interrupt Enabled.
     * |        |          |Note1: When up-down counter type period point means center point.
     * |        |          |Note2: Odd channels will read always 0 at complementary mode.
     * |[16]    |CMPUIEN0  |EPWM Compare Up Count Interrupt Enable Bits
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 use as another CMPUIEN for channel 0, 2, 4.
     * |[17]    |CMPUIEN1  |EPWM Compare Up Count Interrupt Enable Bits
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 use as another CMPUIEN for channel 0, 2, 4.
     * |[18]    |CMPUIEN2  |EPWM Compare Up Count Interrupt Enable Bits
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 use as another CMPUIEN for channel 0, 2, 4.
     * |[19]    |CMPUIEN3  |EPWM Compare Up Count Interrupt Enable Bits
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 use as another CMPUIEN for channel 0, 2, 4.
     * |[20]    |CMPUIEN4  |EPWM Compare Up Count Interrupt Enable Bits
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 use as another CMPUIEN for channel 0, 2, 4.
     * |[21]    |CMPUIEN5  |EPWM Compare Up Count Interrupt Enable Bits
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPUIEN1, 3, 5 use as another CMPUIEN for channel 0, 2, 4.
     * |[24]    |CMPDIEN0  |EPWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 use as another CMPDIEN for channel 0, 2, 4.
     * |[25]    |CMPDIEN1  |EPWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 use as another CMPDIEN for channel 0, 2, 4.
     * |[26]    |CMPDIEN2  |EPWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 use as another CMPDIEN for channel 0, 2, 4.
     * |[27]    |CMPDIEN3  |EPWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 use as another CMPDIEN for channel 0, 2, 4.
     * |[28]    |CMPDIEN4  |EPWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 use as another CMPDIEN for channel 0, 2, 4.
     * |[29]    |CMPDIEN5  |EPWM Compare Down Count Interrupt Enable Bits
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * |        |          |Note: In complementary mode, CMPDIEN1, 3, 5 use as another CMPDIEN for channel 0, 2, 4.
     * @var EPWM_T::INTEN1
     * Offset: 0xE4  EPWM Interrupt Enable Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRKEIEN0_1|EPWM Edge-detect Brake Interrupt Enable for Channel0/1 (Write Protect)
     * |        |          |0 = Edge-detect Brake interrupt for channel0/1 Disabled.
     * |        |          |1 = Edge-detect Brake interrupt for channel0/1 Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[1]     |BRKEIEN2_3|EPWM Edge-detect Brake Interrupt Enable for Channel2/3 (Write Protect)
     * |        |          |0 = Edge-detect Brake interrupt for channel2/3 Disabled.
     * |        |          |1 = Edge-detect Brake interrupt for channel2/3 Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[2]     |BRKEIEN4_5|EPWM Edge-detect Brake Interrupt Enable for Channel4/5 (Write Protect)
     * |        |          |0 = Edge-detect Brake interrupt for channel4/5 Disabled.
     * |        |          |1 = Edge-detect Brake interrupt for channel4/5 Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[8]     |BRKLIEN0_1|EPWM Level-detect Brake Interrupt Enable for Channel0/1 (Write Protect)
     * |        |          |0 = Level-detect Brake interrupt for channel0/1 Disabled.
     * |        |          |1 = Level-detect Brake interrupt for channel0/1 Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[9]     |BRKLIEN2_3|EPWM Level-detect Brake Interrupt Enable for Channel2/3 (Write Protect)
     * |        |          |0 = Level-detect Brake interrupt for channel2/3 Disabled.
     * |        |          |1 = Level-detect Brake interrupt for channel2/3 Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[10]    |BRKLIEN4_5|EPWM Level-detect Brake Interrupt Enable for Channel4/5 (Write Protect)
     * |        |          |0 = Level-detect Brake interrupt for channel4/5 Disabled.
     * |        |          |1 = Level-detect Brake interrupt for channel4/5 Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * @var EPWM_T::INTSTS0
     * Offset: 0xE8  EPWM Interrupt Flag Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ZIF0      |EPWM Zero Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches zero, software can write 1 to clear this bit to zero.
     * |[1]     |ZIF1      |EPWM Zero Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches zero, software can write 1 to clear this bit to zero.
     * |[2]     |ZIF2      |EPWM Zero Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches zero, software can write 1 to clear this bit to zero.
     * |[3]     |ZIF3      |EPWM Zero Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches zero, software can write 1 to clear this bit to zero.
     * |[4]     |ZIF4      |EPWM Zero Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches zero, software can write 1 to clear this bit to zero.
     * |[5]     |ZIF5      |EPWM Zero Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches zero, software can write 1 to clear this bit to zero.
     * |[8]     |PIF0      |EPWM Period Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches EPWM_PERIODn, software can write 1 to clear this bit to zero.
     * |[9]     |PIF1      |EPWM Period Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches EPWM_PERIODn, software can write 1 to clear this bit to zero.
     * |[10]    |PIF2      |EPWM Period Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches EPWM_PERIODn, software can write 1 to clear this bit to zero.
     * |[11]    |PIF3      |EPWM Period Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches EPWM_PERIODn, software can write 1 to clear this bit to zero.
     * |[12]    |PIF4      |EPWM Period Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches EPWM_PERIODn, software can write 1 to clear this bit to zero.
     * |[13]    |PIF5      |EPWM Period Point Interrupt Flag
     * |        |          |This bit is set by hardware when EPWM counter reaches EPWM_PERIODn, software can write 1 to clear this bit to zero.
     * |[16]    |CMPUIF0   |EPWM Compare Up Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter up count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in up counter type selection.
     * |        |          |Note2: In complementary mode, CMPUIF1, 3, 5 use as another CMPUIF for channel 0, 2, 4.
     * |[17]    |CMPUIF1   |EPWM Compare Up Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter up count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in up counter type selection.
     * |        |          |Note2: In complementary mode, CMPUIF1, 3, 5 use as another CMPUIF for channel 0, 2, 4.
     * |[18]    |CMPUIF2   |EPWM Compare Up Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter up count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in up counter type selection.
     * |        |          |Note2: In complementary mode, CMPUIF1, 3, 5 use as another CMPUIF for channel 0, 2, 4.
     * |[19]    |CMPUIF3   |EPWM Compare Up Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter up count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in up counter type selection.
     * |        |          |Note2: In complementary mode, CMPUIF1, 3, 5 use as another CMPUIF for channel 0, 2, 4.
     * |[20]    |CMPUIF4   |EPWM Compare Up Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter up count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in up counter type selection.
     * |        |          |Note2: In complementary mode, CMPUIF1, 3, 5 use as another CMPUIF for channel 0, 2, 4.
     * |[21]    |CMPUIF5   |EPWM Compare Up Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter up count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in up counter type selection.
     * |        |          |Note2: In complementary mode, CMPUIF1, 3, 5 use as another CMPUIF for channel 0, 2, 4.
     * |[24]    |CMPDIF0   |EPWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter down count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in down counter type selection.
     * |        |          |Note2: In complementary mode, CMPDIF1, 3, 5 use as another CMPDIF for channel 0, 2, 4.
     * |[25]    |CMPDIF1   |EPWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter down count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in down counter type selection.
     * |        |          |Note2: In complementary mode, CMPDIF1, 3, 5 use as another CMPDIF for channel 0, 2, 4.
     * |[26]    |CMPDIF2   |EPWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter down count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in down counter type selection.
     * |        |          |Note2: In complementary mode, CMPDIF1, 3, 5 use as another CMPDIF for channel 0, 2, 4.
     * |[27]    |CMPDIF3   |EPWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter down count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in down counter type selection.
     * |        |          |Note2: In complementary mode, CMPDIF1, 3, 5 use as another CMPDIF for channel 0, 2, 4.
     * |[28]    |CMPDIF4   |EPWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter down count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in down counter type selection.
     * |        |          |Note2: In complementary mode, CMPDIF1, 3, 5 use as another CMPDIF for channel 0, 2, 4.
     * |[29]    |CMPDIF5   |EPWM Compare Down Count Interrupt Flag
     * |        |          |Flag is set by hardware when EPWM counter down count and reaches EPWM_CMPDATn, software can clear this bit by writing 1 to it.
     * |        |          |Note1: If CMPDAT equal to PERIOD, this flag is not working in down counter type selection.
     * |        |          |Note2: In complementary mode, CMPDIF1, 3, 5 use as another CMPDIF for channel 0, 2, 4.
     * @var EPWM_T::INTSTS1
     * Offset: 0xEC  EPWM Interrupt Flag Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRKEIF0   |EPWM Channel0 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel0 edge-detect brake event do not happened.
     * |        |          |1 = When EPWM channel0 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[1]     |BRKEIF1   |EPWM Channel1 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel1 edge-detect brake event do not happened.
     * |        |          |1 = When EPWM channel1 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[2]     |BRKEIF2   |EPWM Channel2 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel2 edge-detect brake event do not happened.
     * |        |          |1 = When EPWM channel2 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[3]     |BRKEIF3   |EPWM Channel3 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel3 edge-detect brake event do not happened.
     * |        |          |1 = When EPWM channel3 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[4]     |BRKEIF4   |EPWM Channel4 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel4 edge-detect brake event do not happened.
     * |        |          |1 = When EPWM channel4 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[5]     |BRKEIF5   |EPWM Channel5 Edge-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel5 edge-detect brake event do not happened.
     * |        |          |1 = When EPWM channel5 edge-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[8]     |BRKLIF0   |EPWM Channel0 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel0 level-detect brake event do not happened.
     * |        |          |1 = When EPWM channel0 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[9]     |BRKLIF1   |EPWM Channel1 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel1 level-detect brake event do not happened.
     * |        |          |1 = When EPWM channel1 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[10]    |BRKLIF2   |EPWM Channel2 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel2 level-detect brake event do not happened.
     * |        |          |1 = When EPWM channel2 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[11]    |BRKLIF3   |EPWM Channel3 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel3 level-detect brake event do not happened.
     * |        |          |1 = When EPWM channel3 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[12]    |BRKLIF4   |EPWM Channel4 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel4 level-detect brake event do not happened.
     * |        |          |1 = When EPWM channel4 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[13]    |BRKLIF5   |EPWM Channel5 Level-detect Brake Interrupt Flag (Write Protect)
     * |        |          |0 = EPWM channel5 level-detect brake event do not happened.
     * |        |          |1 = When EEPWM channel5 level-detect brake event happened, this bit is set to 1, writing 1 to clear.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[16]    |BRKESTS0  |EPWM Channel0 Edge-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel0 edge-detect brake state is released.
     * |        |          |1 = When EPWM channel0 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel0 at brake state, writing 1 to clear.
     * |[17]    |BRKESTS1  |EPWM Channel1 Edge-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel1 edge-detect brake state is released.
     * |        |          |1 = When EPWM channel1 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel1 at brake state, writing 1 to clear.
     * |[18]    |BRKESTS2  |EPWM Channel2 Edge-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel2 edge-detect brake state is released.
     * |        |          |1 = When EPWM channel2 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel2 at brake state, writing 1 to clear.
     * |[19]    |BRKESTS3  |EPWM Channel3 Edge-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel3 edge-detect brake state is released.
     * |        |          |1 = When EPWM channel3 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel3 at brake state, writing 1 to clear.
     * |[20]    |BRKESTS4  |EPWM Channel4 Edge-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel4 edge-detect brake state is released.
     * |        |          |1 = When EPWM channel4 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel4 at brake state, writing 1 to clear.
     * |[21]    |BRKESTS5  |EPWM Channel5 Edge-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel5 edge-detect brake state is released.
     * |        |          |1 = When EPWM channel5 edge-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel5 at brake state, writing 1 to clear.
     * |[24]    |BRKLSTS0  |EPWM Channel0 Level-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel0 level-detect brake state is released.
     * |        |          |1 = When EPWM channel0 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel0 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, EPWM will release brake state until current EPWM period finished
     * |        |          |The EPWM waveform will start output from next full EPWM period.
     * |[25]    |BRKLSTS1  |EPWM Channel1 Level-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel1 level-detect brake state is released.
     * |        |          |1 = When EPWM channel1 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel1 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, EPWM will release brake state until current EPWM period finished
     * |        |          |The EPWM waveform will start output from next full EPWM period.
     * |[26]    |BRKLSTS2  |EPWM Channel2 Level-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel2 level-detect brake state is released.
     * |        |          |1 = When EPWM channel2 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel2 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, EPWM will release brake state until current EPWM period finished
     * |        |          |The EPWM waveform will start output from next full EPWM period.
     * |[27]    |BRKLSTS3  |EPWM Channel3 Level-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel3 level-detect brake state is released.
     * |        |          |1 = When EPWM channel3 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel3 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, EPWM will release brake state until current EPWM period finished
     * |        |          |The EPWM waveform will start output from next full EPWM period.
     * |[28]    |BRKLSTS4  |EPWM Channel4 Level-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel4 level-detect brake state is released.
     * |        |          |1 = When EPWM channel4 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel4 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, EPWM will release brake state until current EPWM period finished
     * |        |          |The EPWM waveform will start output from next full EPWM period.
     * |[29]    |BRKLSTS5  |EPWM Channel5 Level-detect Brake Status (Read Only)
     * |        |          |0 = EPWM channel5 level-detect brake state is released.
     * |        |          |1 = When EPWM channel5 level-detect brake detects a falling edge of any enabled brake source; this flag will be set to indicate the EPWM channel5 at brake state.
     * |        |          |Note: This bit is read only and auto cleared by hardware
     * |        |          |When enabled brake source return to high level, EPWM will release brake state until current EPWM period finished
     * |        |          |The EPWM waveform will start output from next full EPWM period.
     * @var EPWM_T::DACTRGEN
     * Offset: 0xF4  EPWM Trigger DAC Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ZTE0      |EPWM Zero Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger EADC/DAC/DMA to start action when EPWM counter down count to zero if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[1]     |ZTE1      |EPWM Zero Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger EADC/DAC/DMA to start action when EPWM counter down count to zero if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[2]     |ZTE2      |EPWM Zero Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger EADC/DAC/DMA to start action when EPWM counter down count to zero if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[3]     |ZTE3      |EPWM Zero Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger EADC/DAC/DMA to start action when EPWM counter down count to zero if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[4]     |ZTE4      |EPWM Zero Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger EADC/DAC/DMA to start action when EPWM counter down count to zero if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[5]     |ZTE5      |EPWM Zero Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger EADC/DAC/DMA to start action when EPWM counter down count to zero if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[8]     |PTE0      |EPWM Period Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to (PERIODn+1) if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[9]     |PTE1      |EPWM Period Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to (PERIODn+1) if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[10]    |PTE2      |EPWM Period Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to (PERIODn+1) if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[11]    |PTE3      |EPWM Period Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to (PERIODn+1) if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[12]    |PTE4      |EPWM Period Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to (PERIODn+1) if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[13]    |PTE5      |EPWM Period Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to (PERIODn+1) if this bit is set to1.
     * |        |          |0 = EPWM period point trigger DAC function Disabled.
     * |        |          |1 = EPWM period point trigger DAC function Enabled.
     * |[16]    |CUTRGE0   |EPWM Compare Up Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Up point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Up point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in down counter type.
     * |        |          |Note2: In complementary mode, CUTRGE1, 3, 5 use as another CUTRGE for channel 0, 2, 4.
     * |[17]    |CUTRGE1   |EPWM Compare Up Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Up point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Up point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in down counter type.
     * |        |          |Note2: In complementary mode, CUTRGE1, 3, 5 use as another CUTRGE for channel 0, 2, 4.
     * |[18]    |CUTRGE2   |EPWM Compare Up Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Up point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Up point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in down counter type.
     * |        |          |Note2: In complementary mode, CUTRGE1, 3, 5 use as another CUTRGE for channel 0, 2, 4.
     * |[19]    |CUTRGE3   |EPWM Compare Up Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Up point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Up point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in down counter type.
     * |        |          |Note2: In complementary mode, CUTRGE1, 3, 5 use as another CUTRGE for channel 0, 2, 4.
     * |[20]    |CUTRGE4   |EPWM Compare Up Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Up point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Up point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in down counter type.
     * |        |          |Note2: In complementary mode, CUTRGE1, 3, 5 use as another CUTRGE for channel 0, 2, 4.
     * |[21]    |CUTRGE5   |EPWM Compare Up Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter up count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Up point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Up point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in down counter type.
     * |        |          |Note2: In complementary mode, CUTRGE1, 3, 5 use as another CUTRGE for channel 0, 2, 4.
     * |[24]    |CDTRGE0   |EPWM Compare Down Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter down count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Down count point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Down count point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in up counter type.
     * |        |          |Note2: In complementary mode, CDTRGE1, 3, 5 use as another CDTRGE for channel 0, 2, 4.
     * |[25]    |CDTRGE1   |EPWM Compare Down Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter down count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Down count point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Down count point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in up counter type.
     * |        |          |Note2: In complementary mode, CDTRGE1, 3, 5 use as another CDTRGE for channel 0, 2, 4.
     * |[26]    |CDTRGE2   |EPWM Compare Down Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter down count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Down count point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Down count point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in up counter type.
     * |        |          |Note2: In complementary mode, CDTRGE1, 3, 5 use as another CDTRGE for channel 0, 2, 4.
     * |[27]    |CDTRGE3   |EPWM Compare Down Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter down count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Down count point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Down count point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in up counter type.
     * |        |          |Note2: In complementary mode, CDTRGE1, 3, 5 use as another CDTRGE for channel 0, 2, 4.
     * |[28]    |CDTRGE4   |EPWM Compare Down Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter down count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Down count point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Down count point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in up counter type.
     * |        |          |Note2: In complementary mode, CDTRGE1, 3, 5 use as another CDTRGE for channel 0, 2, 4.
     * |[29]    |CDTRGE5   |EPWM Compare Down Count Point Trigger DAC Enable Bits
     * |        |          |EPWM can trigger DAC to start action when EPWM counter down count to CMPDAT if this bit is set to1.
     * |        |          |0 = EPWM Compare Down count point trigger DAC function Disabled.
     * |        |          |1 = EPWM Compare Down count point trigger DAC function Enabled.
     * |        |          |Note1: This bit should keep at 0 when EPWM counter operating in up counter type.
     * |        |          |Note2: In complementary mode, CDTRGE1, 3, 5 use as another CDTRGE for channel 0, 2, 4.
     * @var EPWM_T::EADCTS0
     * Offset: 0xF8  EPWM Trigger EADC Source Select Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |TRGSEL0   |EPWM_CH0 Trigger EADC Source Select
     * |        |          |0000 = EPWM_CH0 zero point.
     * |        |          |0001 = EPWM_CH0 period point.
     * |        |          |0010 = EPWM_CH0 zero or period point.
     * |        |          |0011 = EPWM_CH0 up-count CMPDAT point.
     * |        |          |0100 = EPWM_CH0 down-count CMPDAT point.
     * |        |          |0101 = EPWM_CH1 zero point.
     * |        |          |0110 = EPWM_CH1 period point.
     * |        |          |0111 = EPWM_CH1 zero or period point.
     * |        |          |1000 = EPWM_CH1 up-count CMPDAT point.
     * |        |          |1001 = EPWM_CH1 down-count CMPDAT point.
     * |        |          |1010 = EPWM_CH0 up-count free CMPDAT point.
     * |        |          |1011 = EPWM_CH0 down-count free CMPDAT point.
     * |        |          |1100 = EPWM_CH2 up-count free CMPDAT point.
     * |        |          |1101 = EPWM_CH2 down-count free CMPDAT point.
     * |        |          |1110 = EPWM_CH4 up-count free CMPDAT point.
     * |        |          |1111 = EPWM_CH4 down-count free CMPDAT point.
     * |[7]     |TRGEN0    |EPWM_CH0 Trigger EADC enable bit
     * |[11:8]  |TRGSEL1   |EPWM_CH1 Trigger EADC Source Select
     * |        |          |0000 = EPWM_CH0 zero point.
     * |        |          |0001 = EPWM_CH0 period point.
     * |        |          |0010 = EPWM_CH0 zero or period point.
     * |        |          |0011 = EPWM_CH0 up-count CMPDAT point.
     * |        |          |0100 = EPWM_CH0 down-count CMPDAT point.
     * |        |          |0101 = EPWM_CH1 zero point.
     * |        |          |0110 = EPWM_CH1 period point.
     * |        |          |0111 = EPWM_CH1 zero or period point.
     * |        |          |1000 = EPWM_CH1 up-count CMPDAT point.
     * |        |          |1001 = EPWM_CH1 down-count CMPDAT point.
     * |        |          |1010 = EPWM_CH0 up-count free CMPDAT point.
     * |        |          |1011 = EPWM_CH0 down-count free CMPDAT point.
     * |        |          |1100 = EPWM_CH2 up-count free CMPDAT point.
     * |        |          |1101 = EPWM_CH2 down-count free CMPDAT point.
     * |        |          |1110 = EPWM_CH4 up-count free CMPDAT point.
     * |        |          |1111 = EPWM_CH4 down-count free CMPDAT point.
     * |[15]    |TRGEN1    |EPWM_CH1 Trigger EADC enable bit
     * |[19:16] |TRGSEL2   |EPWM_CH2 Trigger EADC Source Select
     * |        |          |0000 = EPWM_CH2 zero point.
     * |        |          |0001 = EPWM_CH2 period point.
     * |        |          |0010 = EPWM_CH2 zero or period point.
     * |        |          |0011 = EPWM_CH2 up-count CMPDAT point.
     * |        |          |0100 = EPWM_CH2 down-count CMPDAT point.
     * |        |          |0101 = EPWM_CH3 zero point.
     * |        |          |0110 = EPWM_CH3 period point.
     * |        |          |0111 = EPWM_CH3 zero or period point.
     * |        |          |1000 = EPWM_CH3 up-count CMPDAT point.
     * |        |          |1001 = EPWM_CH3 down-count CMPDAT point.
     * |        |          |1010 = EPWM_CH0 up-count free CMPDAT point.
     * |        |          |1011 = EPWM_CH0 down-count free CMPDAT point.
     * |        |          |1100 = EPWM_CH2 up-count free CMPDAT point.
     * |        |          |1101 = EPWM_CH2 down-count free CMPDAT point.
     * |        |          |1110 = EPWM_CH4 up-count free CMPDAT point.
     * |        |          |1111 = EPWM_CH4 down-count free CMPDAT point.
     * |[23]    |TRGEN2    |EPWM_CH2 Trigger EADC enable bit
     * |[27:24] |TRGSEL3   |EPWM_CH3 Trigger EADC Source Select
     * |        |          |0000 = EPWM_CH2 zero point.
     * |        |          |0001 = EPWM_CH2 period point.
     * |        |          |0010 = EPWM_CH2 zero or period point.
     * |        |          |0011 = EPWM_CH2 up-count CMPDAT point.
     * |        |          |0100 = EPWM_CH2 down-count CMPDAT point.
     * |        |          |0101 = EPWM_CH3 zero point.
     * |        |          |0110 = EPWM_CH3 period point.
     * |        |          |0111 = EPWM_CH3 zero or period point.
     * |        |          |1000 = EPWM_CH3 up-count CMPDAT point.
     * |        |          |1001 = EPWM_CH3 down-count CMPDAT point.
     * |        |          |1010 = EPWM_CH0 up-count free CMPDAT point.
     * |        |          |1011 = EPWM_CH0 down-count free CMPDAT point.
     * |        |          |1100 = EPWM_CH2 up-count free CMPDAT point.
     * |        |          |1101 = EPWM_CH2 down-count free CMPDAT point.
     * |        |          |1110 = EPWM_CH4 up-count free CMPDAT point.
     * |        |          |1111 = EPWM_CH4 down-count free CMPDAT point.
     * |[31]    |TRGEN3    |EPWM_CH3 Trigger EADC enable bit
     * @var EPWM_T::EADCTS1
     * Offset: 0xFC  EPWM Trigger EADC Source Select Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[3:0]   |TRGSEL4   |EPWM_CH4 Trigger EADC Source Select
     * |        |          |0000 = EPWM_CH4 zero point.
     * |        |          |0001 = EPWM_CH4 period point.
     * |        |          |0010 = EPWM_CH4 zero or period point.
     * |        |          |0011 = EPWM_CH4 up-count CMPDAT point.
     * |        |          |0100 = EPWM_CH4 down-count CMPDAT point.
     * |        |          |0101 = EPWM_CH5 zero point.
     * |        |          |0110 = EPWM_CH5 period point.
     * |        |          |0111 = EPWM_CH5 zero or period point.
     * |        |          |1000 = EPWM_CH5 up-count CMPDAT point.
     * |        |          |1001 = EPWM_CH5 down-count CMPDAT point.
     * |        |          |1010 = EPWM_CH0 up-count free CMPDAT point.
     * |        |          |1011 = EPWM_CH0 down-count free CMPDAT point.
     * |        |          |1100 = EPWM_CH2 up-count free CMPDAT point.
     * |        |          |1101 = EPWM_CH2 down-count free CMPDAT point.
     * |        |          |1110 = EPWM_CH4 up-count free CMPDAT point.
     * |        |          |1111 = EPWM_CH4 down-count free CMPDAT point.
     * |[7]     |TRGEN4    |EPWM_CH4 Trigger EADC enable bit
     * |[11:8]  |TRGSEL5   |EPWM_CH5 Trigger EADC Source Select
     * |        |          |0000 = EPWM_CH4 zero point.
     * |        |          |0001 = EPWM_CH4 period point.
     * |        |          |0010 = EPWM_CH4 zero or period point.
     * |        |          |0011 = EPWM_CH4 up-count CMPDAT point.
     * |        |          |0100 = EPWM_CH4 down-count CMPDAT point.
     * |        |          |0101 = EPWM_CH5 zero point.
     * |        |          |0110 = EPWM_CH5 period point.
     * |        |          |0111 = EPWM_CH5 zero or period point.
     * |        |          |1000 = EPWM_CH5 up-count CMPDAT point.
     * |        |          |1001 = EPWM_CH5 down-count CMPDAT point.
     * |        |          |1010 = EPWM_CH0 up-count free CMPDAT point.
     * |        |          |1011 = EPWM_CH0 down-count free CMPDAT point.
     * |        |          |1100 = EPWM_CH2 up-count free CMPDAT point.
     * |        |          |1101 = EPWM_CH2 down-count free CMPDAT point.
     * |        |          |1110 = EPWM_CH4 up-count free CMPDAT point.
     * |        |          |1111 = EPWM_CH4 down-count free CMPDAT point.
     * |[15]    |TRGEN5    |EPWM_CH5 Trigger EADC enable bit
     * @var EPWM_T::FTCMPDAT[3]
     * Offset: 0x100  EPWM Free Trigger Compare Register 0/1,2/3,4/5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |FTCMP     |EPWM Free Trigger Compare Register
     * |        |          |FTCMP use to compare with even CNTR to trigger EADC
     * |        |          |FTCMPDAT0, 2, 4 corresponding complementary pairs EPWM_CH0 and EPWM_CH1, EPWM_CH2 and EPWM_CH3, EPWM_CH4 and EPWM_CH5.
     * @var EPWM_T::SSCTL
     * Offset: 0x110  EPWM Synchronous Start Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |SSEN0     |EPWM Synchronous Start Function Enable Bits
     * |        |          |When synchronous start function is enabled, the EPWM counter enable register (EPWM_CNTEN) can be enabled by writing EPWM synchronous start trigger bit (CNTSEN).
     * |        |          |0 = EPWM synchronous start function Disabled.
     * |        |          |1 = EPWM synchronous start function Enabled.
     * |[1]     |SSEN1     |EPWM Synchronous Start Function Enable Bits
     * |        |          |When synchronous start function is enabled, the EPWM counter enable register (EPWM_CNTEN) can be enabled by writing EPWM synchronous start trigger bit (CNTSEN).
     * |        |          |0 = EPWM synchronous start function Disabled.
     * |        |          |1 = EPWM synchronous start function Enabled.
     * |[2]     |SSEN2     |EPWM Synchronous Start Function Enable Bits
     * |        |          |When synchronous start function is enabled, the EPWM counter enable register (EPWM_CNTEN) can be enabled by writing EPWM synchronous start trigger bit (CNTSEN).
     * |        |          |0 = EPWM synchronous start function Disabled.
     * |        |          |1 = EPWM synchronous start function Enabled.
     * |[3]     |SSEN3     |EPWM Synchronous Start Function Enable Bits
     * |        |          |When synchronous start function is enabled, the EPWM counter enable register (EPWM_CNTEN) can be enabled by writing EPWM synchronous start trigger bit (CNTSEN).
     * |        |          |0 = EPWM synchronous start function Disabled.
     * |        |          |1 = EPWM synchronous start function Enabled.
     * |[4]     |SSEN4     |EPWM Synchronous Start Function Enable Bits
     * |        |          |When synchronous start function is enabled, the EPWM counter enable register (EPWM_CNTEN) can be enabled by writing EPWM synchronous start trigger bit (CNTSEN).
     * |        |          |0 = EPWM synchronous start function Disabled.
     * |        |          |1 = EPWM synchronous start function Enabled.
     * |[5]     |SSEN5     |EPWM Synchronous Start Function Enable Bits
     * |        |          |When synchronous start function is enabled, the EPWM counter enable register (EPWM_CNTEN) can be enabled by writing EPWM synchronous start trigger bit (CNTSEN).
     * |        |          |0 = EPWM synchronous start function Disabled.
     * |        |          |1 = EPWM synchronous start function Enabled.
     * |[9:8]   |SSRC      |EPWM Synchronous Start Source Select Bits
     * |        |          |00 = Synchronous start source come from EPWM0.
     * |        |          |01 = Synchronous start source come from EPWM1.
     * |        |          |10 = Synchronous start source come from BPWM0.
     * |        |          |11 = Synchronous start source come from BPWM1.
     * @var EPWM_T::SSTRG
     * Offset: 0x114  EPWM Synchronous Start Trigger Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTSEN    |EPWM Counter Synchronous Start Enable (Write Only)
     * |        |          |PMW counter synchronous enable function is used to make selected EPWM channels (include EPWM0_CHx and EPWM1_CHx) start counting at the same time.
     * |        |          |Writing this bit to 1 will also set the counter enable bit (CNTENn, n denotes channel 0 to 5) if correlated EPWM channel counter synchronous start function is enabled.
     * @var EPWM_T::LEBCTL
     * Offset: 0x118  EPWM Leading Edge Blanking Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |LEBEN     |EPWM Leading Edge Blanking Enable Bit
     * |        |          |0 = EPWM Leading Edge Blanking Disabled.
     * |        |          |1 = EPWM Leading Edge Blanking Enabled.
     * |[8]     |SRCEN0    |EPWM Leading Edge Blanking Source From EPWM_CH0 Enable Bit
     * |        |          |0 = EPWM Leading Edge Blanking Source from EPWM_CH0 Disabled.
     * |        |          |1 = EPWM Leading Edge Blanking Source from EPWM_CH0 Enabled.
     * |[9]     |SRCEN2    |EPWM Leading Edge Blanking Source From EPWM_CH2 Enable Bit
     * |        |          |0 = EPWM Leading Edge Blanking Source from EPWM_CH2 Disabled.
     * |        |          |1 = EPWM Leading Edge Blanking Source from EPWM_CH2 Enabled.
     * |[10]    |SRCEN4    |EPWM Leading Edge Blanking Source From EPWM_CH4 Enable Bit
     * |        |          |0 = EPWM Leading Edge Blanking Source from EPWM_CH4 Disabled.
     * |        |          |1 = EPWM Leading Edge Blanking Source from EPWM_CH4 Enabled.
     * |[17:16] |TRGTYPE   |EPWM Leading Edge Blanking Trigger Type
     * |        |          |0 = When detect leading edge blanking source rising edge, blanking counter start counting.
     * |        |          |1 = When detect leading edge blanking source falling edge, blanking counter start counting.
     * |        |          |2 = When detect leading edge blanking source rising or falling edge, blanking counter start counting.
     * |        |          |3 = Reserved.
     * @var EPWM_T::LEBCNT
     * Offset: 0x11C  EPWM Leading Edge Blanking Counter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[8:0]   |LEBCNT    |EPWM Leading Edge Blanking Counter
     * |        |          |This counter value decides leading edge blanking window size
     * |        |          |Blanking window size = LEBCNT+1, and LEB counter clock base is ECLK.
     * @var EPWM_T::STATUS
     * Offset: 0x120  EPWM Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTMAXF0  |Time-base Counter Equal to 0xFFFF Latched Flag
     * |        |          |0 = indicates the time-base counter never reached its maximum value 0xFFFF.
     * |        |          |1 = indicates the time-base counter reached its maximum value, software can write 1 to clear this bit.
     * |[1]     |CNTMAXF1  |Time-base Counter Equal to 0xFFFF Latched Flag
     * |        |          |0 = indicates the time-base counter never reached its maximum value 0xFFFF.
     * |        |          |1 = indicates the time-base counter reached its maximum value, software can write 1 to clear this bit.
     * |[2]     |CNTMAXF2  |Time-base Counter Equal to 0xFFFF Latched Flag
     * |        |          |0 = indicates the time-base counter never reached its maximum value 0xFFFF.
     * |        |          |1 = indicates the time-base counter reached its maximum value, software can write 1 to clear this bit.
     * |[3]     |CNTMAXF3  |Time-base Counter Equal to 0xFFFF Latched Flag
     * |        |          |0 = indicates the time-base counter never reached its maximum value 0xFFFF.
     * |        |          |1 = indicates the time-base counter reached its maximum value, software can write 1 to clear this bit.
     * |[4]     |CNTMAXF4  |Time-base Counter Equal to 0xFFFF Latched Flag
     * |        |          |0 = indicates the time-base counter never reached its maximum value 0xFFFF.
     * |        |          |1 = indicates the time-base counter reached its maximum value, software can write 1 to clear this bit.
     * |[5]     |CNTMAXF5  |Time-base Counter Equal to 0xFFFF Latched Flag
     * |        |          |0 = indicates the time-base counter never reached its maximum value 0xFFFF.
     * |        |          |1 = indicates the time-base counter reached its maximum value, software can write 1 to clear this bit.
     * |[8]     |SYNCINF0  |Input Synchronization Latched Flag
     * |        |          |0 = Indicates no SYNC_IN event has occurred.
     * |        |          |1 = Indicates an SYNC_IN event has occurred, software can write 1 to clear this bit.
     * |[9]     |SYNCINF2  |Input Synchronization Latched Flag
     * |        |          |0 = Indicates no SYNC_IN event has occurred.
     * |        |          |1 = Indicates an SYNC_IN event has occurred, software can write 1 to clear this bit.
     * |[10]    |SYNCINF4  |Input Synchronization Latched Flag
     * |        |          |0 = Indicates no SYNC_IN event has occurred.
     * |        |          |1 = Indicates an SYNC_IN event has occurred, software can write 1 to clear this bit.
     * |[16]    |EADCTRGF0 |EADC Start of Conversion Flag
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = Indicates an EADC start of conversion trigger event has occurred, software can write 1 to clear this bit.
     * |[17]    |EADCTRGF1 |EADC Start of Conversion Flag
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = Indicates an EADC start of conversion trigger event has occurred, software can write 1 to clear this bit.
     * |[18]    |EADCTRGF2 |EADC Start of Conversion Flag
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = Indicates an EADC start of conversion trigger event has occurred, software can write 1 to clear this bit.
     * |[19]    |EADCTRGF3 |EADC Start of Conversion Flag
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = Indicates an EADC start of conversion trigger event has occurred, software can write 1 to clear this bit.
     * |[20]    |EADCTRGF4 |EADC Start of Conversion Flag
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = Indicates an EADC start of conversion trigger event has occurred, software can write 1 to clear this bit.
     * |[21]    |EADCTRGF5 |EADC Start of Conversion Flag
     * |        |          |0 = Indicates no EADC start of conversion trigger event has occurred.
     * |        |          |1 = Indicates an EADC start of conversion trigger event has occurred, software can write 1 to clear this bit.
     * |[24]    |DACTRGF   |DAC Start of Conversion Flag
     * |        |          |0 = Indicates no DAC start of conversion trigger event has occurred.
     * |        |          |1 = Indicates an DAC start of conversion trigger event has occurred, software can write 1 to clear this bit
     * @var EPWM_T::IFA[6]
     * Offset: 0x130  EPWM Interrupt Flag Accumulator Register 0~5
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |IFACNT    |EPWM_CHn Interrupt Flag Counter
     * |        |          |The register sets the count number which defines how many times of EPWM_CHn period occurs to set bit IFAIFn to request the EPWM period interrupt.
     * |        |          |EPWM flag will be set in every IFACNT[15:0] times of EPWM period.
     * |[24]    |STPMOD    |EPWM_CHn Interrupt Flag Accumulator Stop Mode Enable Bits
     * |        |          |0 = EPWM_CHn interrupt flag accumulator stop mode disable.
     * |        |          |1 = EPWM_CHn interrupt flag accumulator stop mode enable.
     * |[29:28] |IFASEL    |EPWM_CHn Interrupt Flag Accumulator Source Select
     * |        |          |00 = CNT equal to Zero in channel n.
     * |        |          |01 = CNT equal to PERIOD in channel n.
     * |        |          |10 = CNT equal to CMPU in channel n.
     * |        |          |11 = CNT equal to CMPD in channel n.
     * |[31]    |IFAEN     |EPWM_CHn Interrupt Flag Accumulator Enable Bits
     * |        |          |0 = EPWM_CHn interrupt flag accumulator disable.
     * |        |          |1 = EPWM_CHn interrupt flag accumulator enable.
     * @var EPWM_T::AINTSTS
     * Offset: 0x150  EPWM Accumulator Interrupt Flag Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |IFAIF0    |EPWM_CHn Interrupt Flag Accumulator Interrupt Flag
     * |        |          |Flag is set by hardware when condition match IFASEL in EPWM_IFAn register, software can clear this bit by writing 1 to it.
     * |[1]     |IFAIF1    |EPWM_CHn Interrupt Flag Accumulator Interrupt Flag
     * |        |          |Flag is set by hardware when condition match IFASEL in EPWM_IFAn register, software can clear this bit by writing 1 to it.
     * |[2]     |IFAIF2    |EPWM_CHn Interrupt Flag Accumulator Interrupt Flag
     * |        |          |Flag is set by hardware when condition match IFASEL in EPWM_IFAn register, software can clear this bit by writing 1 to it.
     * |[3]     |IFAIF3    |EPWM_CHn Interrupt Flag Accumulator Interrupt Flag
     * |        |          |Flag is set by hardware when condition match IFASEL in EPWM_IFAn register, software can clear this bit by writing 1 to it.
     * |[4]     |IFAIF4    |EPWM_CHn Interrupt Flag Accumulator Interrupt Flag
     * |        |          |Flag is set by hardware when condition match IFASEL in EPWM_IFAn register, software can clear this bit by writing 1 to it.
     * |[5]     |IFAIF5    |EPWM_CHn Interrupt Flag Accumulator Interrupt Flag
     * |        |          |Flag is set by hardware when condition match IFASEL in EPWM_IFAn register, software can clear this bit by writing 1 to it.
     * @var EPWM_T::AINTEN
     * Offset: 0x154  EPWM Accumulator Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |IFAIEN0   |EPWM_CHn Interrupt Flag Accumulator Interrupt Enable Bits
     * |        |          |0 = Interrupt Flag accumulator interrupt Disabled.
     * |        |          |1 = Interrupt Flag accumulator interrupt Enabled.
     * |[1]     |IFAIEN1   |EPWM_CHn Interrupt Flag Accumulator Interrupt Enable Bits
     * |        |          |0 = Interrupt Flag accumulator interrupt Disabled.
     * |        |          |1 = Interrupt Flag accumulator interrupt Enabled.
     * |[2]     |IFAIEN2   |EPWM_CHn Interrupt Flag Accumulator Interrupt Enable Bits
     * |        |          |0 = Interrupt Flag accumulator interrupt Disabled.
     * |        |          |1 = Interrupt Flag accumulator interrupt Enabled.
     * |[3]     |IFAIEN3   |EPWM_CHn Interrupt Flag Accumulator Interrupt Enable Bits
     * |        |          |0 = Interrupt Flag accumulator interrupt Disabled.
     * |        |          |1 = Interrupt Flag accumulator interrupt Enabled.
     * |[4]     |IFAIEN4   |EPWM_CHn Interrupt Flag Accumulator Interrupt Enable Bits
     * |        |          |0 = Interrupt Flag accumulator interrupt Disabled.
     * |        |          |1 = Interrupt Flag accumulator interrupt Enabled.
     * |[5]     |IFAIEN5   |EPWM_CHn Interrupt Flag Accumulator Interrupt Enable Bits
     * |        |          |0 = Interrupt Flag accumulator interrupt Disabled.
     * |        |          |1 = Interrupt Flag accumulator interrupt Enabled.
     * @var EPWM_T::APDMACTL
     * Offset: 0x158  EPWM Accumulator PDMA Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |APDMAEN0  |Channel N Accumulator PDMA Enable Bits
     * |        |          |0 = Channel n PDMA function Disabled.
     * |        |          |1 = Channel n PDMA function Enabled for the channel n to trigger PDMA to transfer memory data to register.
     * |[1]     |APDMAEN1  |Channel N Accumulator PDMA Enable Bits
     * |        |          |0 = Channel n PDMA function Disabled.
     * |        |          |1 = Channel n PDMA function Enabled for the channel n to trigger PDMA to transfer memory data to register.
     * |[2]     |APDMAEN2  |Channel N Accumulator PDMA Enable Bits
     * |        |          |0 = Channel n PDMA function Disabled.
     * |        |          |1 = Channel n PDMA function Enabled for the channel n to trigger PDMA to transfer memory data to register.
     * |[3]     |APDMAEN3  |Channel N Accumulator PDMA Enable Bits
     * |        |          |0 = Channel n PDMA function Disabled.
     * |        |          |1 = Channel n PDMA function Enabled for the channel n to trigger PDMA to transfer memory data to register.
     * |[4]     |APDMAEN4  |Channel N Accumulator PDMA Enable Bits
     * |        |          |0 = Channel n PDMA function Disabled.
     * |        |          |1 = Channel n PDMA function Enabled for the channel n to trigger PDMA to transfer memory data to register.
     * |[5]     |APDMAEN5  |Channel N Accumulator PDMA Enable Bits
     * |        |          |0 = Channel n PDMA function Disabled.
     * |        |          |1 = Channel n PDMA function Enabled for the channel n to trigger PDMA to transfer memory data to register.
     * @var EPWM_T::CAPINEN
     * Offset: 0x200  EPWM Capture Input Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CAPINEN0  |Capture Input Enable Bits
     * |        |          |0 = EPWM Channel capture input path Disabled
     * |        |          |The input of EPWM channel capture function is always regarded as 0.
     * |        |          |1 = EPWM Channel capture input path Enabled
     * |        |          |The input of EPWM channel capture function comes from correlative multifunction pin.
     * |[1]     |CAPINEN1  |Capture Input Enable Bits
     * |        |          |0 = EPWM Channel capture input path Disabled
     * |        |          |The input of EPWM channel capture function is always regarded as 0.
     * |        |          |1 = EPWM Channel capture input path Enabled
     * |        |          |The input of EPWM channel capture function comes from correlative multifunction pin.
     * |[2]     |CAPINEN2  |Capture Input Enable Bits
     * |        |          |0 = EPWM Channel capture input path Disabled
     * |        |          |The input of EPWM channel capture function is always regarded as 0.
     * |        |          |1 = EPWM Channel capture input path Enabled
     * |        |          |The input of EPWM channel capture function comes from correlative multifunction pin.
     * |[3]     |CAPINEN3  |Capture Input Enable Bits
     * |        |          |0 = EPWM Channel capture input path Disabled
     * |        |          |The input of EPWM channel capture function is always regarded as 0.
     * |        |          |1 = EPWM Channel capture input path Enabled
     * |        |          |The input of EPWM channel capture function comes from correlative multifunction pin.
     * |[4]     |CAPINEN4  |Capture Input Enable Bits
     * |        |          |0 = EPWM Channel capture input path Disabled
     * |        |          |The input of EPWM channel capture function is always regarded as 0.
     * |        |          |1 = EPWM Channel capture input path Enabled
     * |        |          |The input of EPWM channel capture function comes from correlative multifunction pin.
     * |[5]     |CAPINEN5  |Capture Input Enable Bits
     * |        |          |0 = EPWM Channel capture input path Disabled
     * |        |          |The input of EPWM channel capture function is always regarded as 0.
     * |        |          |1 = EPWM Channel capture input path Enabled
     * |        |          |The input of EPWM channel capture function comes from correlative multifunction pin.
     * @var EPWM_T::CAPCTL
     * Offset: 0x204  EPWM Capture Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CAPEN0    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the EPWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[1]     |CAPEN1    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the EPWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[2]     |CAPEN2    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the EPWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[3]     |CAPEN3    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the EPWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[4]     |CAPEN4    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the EPWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[5]     |CAPEN5    |Capture Function Enable Bits
     * |        |          |0 = Capture function Disabled. RCAPDAT/FCAPDAT register will not be updated.
     * |        |          |1 = Capture function Enabled
     * |        |          |Capture latched the EPWM counter value when detected rising or falling edge of input signal and saved to RCAPDAT (Rising latch) and FCAPDAT (Falling latch).
     * |[8]     |CAPINV0   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[9]     |CAPINV1   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[10]    |CAPINV2   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[11]    |CAPINV3   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[12]    |CAPINV4   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[13]    |CAPINV5   |Capture Inverter Enable Bits
     * |        |          |0 = Capture source inverter Disabled.
     * |        |          |1 = Capture source inverter Enabled. Reverse the input signal from GPIO.
     * |[16]    |RCRLDEN0  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[17]    |RCRLDEN1  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[18]    |RCRLDEN2  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[19]    |RCRLDEN3  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[20]    |RCRLDEN4  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[21]    |RCRLDEN5  |Rising Capture Reload Enable Bits
     * |        |          |0 = Rising capture reload counter Disabled.
     * |        |          |1 = Rising capture reload counter Enabled.
     * |[24]    |FCRLDEN0  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * |[25]    |FCRLDEN1  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * |[26]    |FCRLDEN2  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * |[27]    |FCRLDEN3  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * |[28]    |FCRLDEN4  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * |[29]    |FCRLDEN5  |Falling Capture Reload Enable Bits
     * |        |          |0 = Falling capture reload counter Disabled.
     * |        |          |1 = Falling capture reload counter Enabled.
     * @var EPWM_T::CAPSTS
     * Offset: 0x208  EPWM Capture Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CRLIFOV0  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[1]     |CRLIFOV1  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[2]     |CRLIFOV2  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[3]     |CRLIFOV3  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[4]     |CRLIFOV4  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[5]     |CRLIFOV5  |Capture Rising Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if rising latch happened when the corresponding CRLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CRLIF.
     * |[8]     |CFLIFOV0  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * |[9]     |CFLIFOV1  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * |[10]    |CFLIFOV2  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * |[11]    |CFLIFOV3  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * |[12]    |CFLIFOV4  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * |[13]    |CFLIFOV5  |Capture Falling Latch Interrupt Flag Overrun Status (Read Only)
     * |        |          |This flag indicates if falling latch happened when the corresponding CFLIF is 1.
     * |        |          |Note: This bit will be cleared automatically when user clear corresponding CFLIF.
     * @var EPWM_T::PDMACTL
     * Offset: 0x23C  EPWM PDMA Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CHEN0_1   |Channel 0/1 PDMA Enable
     * |        |          |0 = Channel 0/1 PDMA function Disabled.
     * |        |          |1 = Channel 0/1 PDMA function Enabled for the channel 0/1 captured data and transfer to memory.
     * |[2:1]   |CAPMOD0_1 |Select EPWM_RCAPDAT0/1 or EPWM_FCAPDAT0/1 to Do PDMA Transfer
     * |        |          |00 = Reserved.
     * |        |          |01 = EPWM_RCAPDAT0/1.
     * |        |          |10 = EPWM_FCAPDAT0/1.
     * |        |          |11 = Both EPWM_RCAPDAT0/1 and EPWM_FCAPDAT0/1.
     * |[3]     |CAPORD0_1 |Capture Channel 0/1 Rising/Falling Order
     * |        |          |Set this bit to determine whether the EPWM_RCAPDAT0/1 or EPWM_FCAPDAT0/1 is the first captured data transferred to memory through PDMA when CAPMOD0_1 =11.
     * |        |          |0 = EPWM_FCAPDAT0/1 is the first captured data to memory.
     * |        |          |1 = EPWM_RCAPDAT0/1 is the first captured data to memory.
     * |[4]     |CHSEL0_1  |Select Channel 0/1 to Do PDMA Transfer
     * |        |          |0 = Channel0.
     * |        |          |1 = Channel1.
     * |[8]     |CHEN2_3   |Channel 2/3 PDMA Enable
     * |        |          |0 = Channel 2/3 PDMA function Disabled.
     * |        |          |1 = Channel 2/3 PDMA function Enabled for the channel 2/3 captured data and transfer to memory.
     * |[10:9]  |CAPMOD2_3 |Select EPWM_RCAPDAT2/3 or EPWM_FCAODAT2/3 to Do PDMA Transfer
     * |        |          |00 = Reserved.
     * |        |          |01 = EPWM_RCAPDAT2/3.
     * |        |          |10 = EPWM_FCAPDAT2/3.
     * |        |          |11 = Both EPWM_RCAPDAT2/3 and EPWM_FCAPDAT2/3.
     * |[11]    |CAPORD2_3 |Capture Channel 2/3 Rising/Falling Order
     * |        |          |Set this bit to determine whether the EPWM_RCAPDAT2/3 or EPWM_FCAPDAT2/3 is the first captured data transferred to memory through PDMA when CAPMOD2_3 =11.
     * |        |          |0 = EPWM_FCAPDAT2/3 is the first captured data to memory.
     * |        |          |1 = EPWM_RCAPDAT2/3 is the first captured data to memory.
     * |[12]    |CHSEL2_3  |Select Channel 2/3 to Do PDMA Transfer
     * |        |          |0 = Channel2.
     * |        |          |1 = Channel3.
     * |[16]    |CHEN4_5   |Channel 4/5 PDMA Enable
     * |        |          |0 = Channel 4/5 PDMA function Disabled.
     * |        |          |1 = Channel 4/5 PDMA function Enabled for the channel 4/5 captured data and transfer to memory.
     * |[18:17] |CAPMOD4_5 |Select EPWM_RCAPDAT4/5 or EPWM_FCAPDAT4/5 to Do PDMA Transfer
     * |        |          |00 = Reserved.
     * |        |          |01 = EPWM_RCAPDAT4/5.
     * |        |          |10 = EPWM_FCAPDAT4/5.
     * |        |          |11 = Both EPWM_RCAPDAT4/5 and EPWM_FCAPDAT4/5.
     * |[19]    |CAPORD4_5 |Capture Channel 4/5 Rising/Falling Order
     * |        |          |Set this bit to determine whether the EPWM_RCAPDAT4/5 or EPWM_FCAPDAT4/5 is the first captured data transferred to memory through PDMA when CAPMOD4_5 =11.
     * |        |          |0 = EPWM_FCAPDAT4/5 is the first captured data to memory.
     * |        |          |1 = EPWM_RCAPDAT4/5 is the first captured data to memory.
     * |[20]    |CHSEL4_5  |Select Channel 4/5 to Do PDMA Transfer
     * |        |          |0 = Channel4.
     * |        |          |1 = Channel5.
     * @var EPWM_T::PDMACAP[3]
     * Offset: 0x240  EPWM Capture Channel 01 PDMA Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CAPBUF    |EPWM Capture PDMA Register (Read Only)
     * |        |          |This register is use as a buffer to transfer EPWM capture rising or falling data to memory by PDMA.
     * @var EPWM_T::CAPIEN
     * Offset: 0x250  EPWM Capture Interrupt Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CAPRIEN0  |EPWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[1]     |CAPRIEN1  |EPWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[2]     |CAPRIEN2  |EPWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[3]     |CAPRIEN3  |EPWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[4]     |CAPRIEN4  |EPWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[5]     |CAPRIEN5  |EPWM Capture Rising Latch Interrupt Enable Bits
     * |        |          |0 = Capture rising edge latch interrupt Disabled.
     * |        |          |1 = Capture rising edge latch interrupt Enabled.
     * |[8]     |CAPFIEN0  |EPWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * |[9]     |CAPFIEN1  |EPWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * |[10]    |CAPFIEN2  |EPWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * |[11]    |CAPFIEN3  |EPWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * |[12]    |CAPFIEN4  |EPWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * |[13]    |CAPFIEN5  |EPWM Capture Falling Latch Interrupt Enable Bits
     * |        |          |0 = Capture falling edge latch interrupt Disabled.
     * |        |          |1 = Capture falling edge latch interrupt Enabled.
     * @var EPWM_T::CAPIF
     * Offset: 0x254  EPWM Capture Interrupt Flag Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CRLIF0    |EPWM Capture Rising Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will cleared by hardware after PDMA transfer data.
     * |[1]     |CRLIF1    |EPWM Capture Rising Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will cleared by hardware after PDMA transfer data.
     * |[2]     |CRLIF2    |EPWM Capture Rising Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will cleared by hardware after PDMA transfer data.
     * |[3]     |CRLIF3    |EPWM Capture Rising Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will cleared by hardware after PDMA transfer data.
     * |[4]     |CRLIF4    |EPWM Capture Rising Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will cleared by hardware after PDMA transfer data.
     * |[5]     |CRLIF5    |EPWM Capture Rising Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture rising latch condition happened.
     * |        |          |1 = Capture rising latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CRLIF will cleared by hardware after PDMA transfer data.
     * |[8]     |CFLIF0    |EPWM Capture Falling Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will cleared by hardware after PDMA transfer data.
     * |[9]     |CFLIF1    |EPWM Capture Falling Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will cleared by hardware after PDMA transfer data.
     * |[10]    |CFLIF2    |EPWM Capture Falling Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will cleared by hardware after PDMA transfer data.
     * |[11]    |CFLIF3    |EPWM Capture Falling Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will cleared by hardware after PDMA transfer data.
     * |[12]    |CFLIF4    |EPWM Capture Falling Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will cleared by hardware after PDMA transfer data.
     * |[13]    |CFLIF5    |EPWM Capture Falling Latch Interrupt Flag
     * |        |          |This bit is writing 1 to clear.
     * |        |          |0 = No capture falling latch condition happened.
     * |        |          |1 = Capture falling latch condition happened, this flag will be set to high.
     * |        |          |Note: When Capture with PDMA operating, CAPIF corresponding channel CFLIF will cleared by hardware after PDMA transfer data.
     * @var EPWM_T::PBUF[6]
     * Offset: 0x304  EPWM PERIOD0~5 Buffer
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |PBUF      |EPWM Period Register Buffer (Read Only)
     * |        |          |Used as PERIOD active register.
     * @var EPWM_T::CMPBUF[6]
     * Offset: 0x31C  EPWM CMPDAT0~5 Buffer
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CMPBUF    |EPWM Comparator Register Buffer (Read Only)
     * |        |          |Used as CMP active register.
     * @var EPWM_T::CPSCBUF[3]
     * Offset: 0x334  EPWM CLKPSC0_1/2_3/4_5 Buffer
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |CPSCBUF   |EPWM Counter Clock Prescale Buffer
     * |        |          |Use as EPWM counter clock prescale active register.
     * @var EPWM_T::FTCBUF[3]
     * Offset: 0x340  EPWM FTCMPDAT0_1/2_3/4_5 Buffer
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |FTCMPBUF  |EPWM FTCMPDAT Buffer (Read Only)
     * |        |          |Used as FTCMPDAT active register.
     * @var EPWM_T::FTCI
     * Offset: 0x34C  EPWM FTCMPDAT Indicator Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |FTCMU0    |EPWM FTCMPDAT Up Indicator
     * |        |          |Indicator will be set to high when FTCMPDATn equal to CNTn and DIRF=1, software can write 1 to clear this bit.
     * |[1]     |FTCMU2    |EPWM FTCMPDAT Up Indicator
     * |        |          |Indicator will be set to high when FTCMPDATn equal to CNTn and DIRF=1, software can write 1 to clear this bit.
     * |[2]     |FTCMU4    |EPWM FTCMPDAT Up Indicator
     * |        |          |Indicator will be set to high when FTCMPDATn equal to CNTn and DIRF=1, software can write 1 to clear this bit.
     * |[8]     |FTCMD0    |EPWM FTCMPDAT Down Indicator
     * |        |          |Indicator will be set to high when FTCMPDATn equal to CNTn and DIRF=0, software can write 1 to clear this bit.
     * |[9]     |FTCMD2    |EPWM FTCMPDAT Down Indicator
     * |        |          |Indicator will be set to high when FTCMPDATn equal to CNTn and DIRF=0, software can write 1 to clear this bit.
     * |[10]    |FTCMD4    |EPWM FTCMPDAT Down Indicator
     * |        |          |Indicator will be set to high when FTCMPDATn equal to CNTn and DIRF=0, software can write 1 to clear this bit.
     */
    __IO uint32_t CTL0;                  /*!< [0x0000] EPWM Control Register 0                                           */
    __IO uint32_t CTL1;                  /*!< [0x0004] EPWM Control Register 1                                           */
    __IO uint32_t SYNC;                  /*!< [0x0008] EPWM Synchronization Register                                     */
    __IO uint32_t SWSYNC;                /*!< [0x000c] EPWM Software Control Synchronization Register                    */
    __IO uint32_t CLKSRC;                /*!< [0x0010] EPWM Clock Source Register                                        */
    __IO uint32_t CLKPSC[3];             /*!< [0x0014] EPWM Clock Prescale Register 0/1,2/3,4/5                          */
    __IO uint32_t CNTEN;                 /*!< [0x0020] EPWM Counter Enable Register                                      */
    __IO uint32_t CNTCLR;                /*!< [0x0024] EPWM Clear Counter Register                                       */
    __IO uint32_t LOAD;                  /*!< [0x0028] EPWM Load Register                                                */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE0[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t PERIOD[6];             /*!< [0x0030] EPWM Period Register 0~5                                          */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE1[2];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t CMPDAT[6];             /*!< [0x0050] EPWM Comparator Register 0~5                                      */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE2[2];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t DTCTL[3];              /*!< [0x0070] EPWM Dead-Time Control Register 0/1,2/3,4/5                       */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE3[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t PHS[3];                /*!< [0x0080] EPWM Counter Phase Register 0/1,2/3,4/5                           */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE4[1];
    /// @endcond //HIDDEN_SYMBOLS
    __I  uint32_t CNT[6];                /*!< [0x0090] EPWM Counter Register 0~5                                         */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE5[2];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t WGCTL0;                /*!< [0x00b0] EPWM Generation Register 0                                        */
    __IO uint32_t WGCTL1;                /*!< [0x00b4] EPWM Generation Register 1                                        */
    __IO uint32_t MSKEN;                 /*!< [0x00b8] EPWM Mask Enable Register                                         */
    __IO uint32_t MSK;                   /*!< [0x00bc] EPWM Mask Data Register                                           */
    __IO uint32_t BNF;                   /*!< [0x00c0] EPWM Brake Noise Filter Register                                  */
    __IO uint32_t FAILBRK;               /*!< [0x00c4] EPWM System Fail Brake Control Register                           */
    __IO uint32_t BRKCTL[3];             /*!< [0x00c8] EPWM Brake Edge Detect Control Register 0/1,2/3,4/5               */
    __IO uint32_t POLCTL;                /*!< [0x00d4] EPWM Pin Polar Inverse Register                                   */
    __IO uint32_t POEN;                  /*!< [0x00d8] EPWM Output Enable Register                                       */
    __O  uint32_t SWBRK;                 /*!< [0x00dc] EPWM Software Brake Control Register                              */
    __IO uint32_t INTEN0;                /*!< [0x00e0] EPWM Interrupt Enable Register 0                                  */
    __IO uint32_t INTEN1;                /*!< [0x00e4] EPWM Interrupt Enable Register 1                                  */
    __IO uint32_t INTSTS0;               /*!< [0x00e8] EPWM Interrupt Flag Register 0                                    */
    __IO uint32_t INTSTS1;               /*!< [0x00ec] EPWM Interrupt Flag Register 1                                    */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE6[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t DACTRGEN;              /*!< [0x00f4] EPWM Trigger DAC Enable Register                                  */
    __IO uint32_t EADCTS0;               /*!< [0x00f8] EPWM Trigger EADC Source Select Register 0                        */
    __IO uint32_t EADCTS1;               /*!< [0x00fc] EPWM Trigger EADC Source Select Register 1                        */
    __IO uint32_t FTCMPDAT[3];           /*!< [0x0100] EPWM Free Trigger Compare Register 0/1,2/3,4/5                    */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE7[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t SSCTL;                 /*!< [0x0110] EPWM Synchronous Start Control Register                           */
    __O  uint32_t SSTRG;                 /*!< [0x0114] EPWM Synchronous Start Trigger Register                           */
    __IO uint32_t LEBCTL;                /*!< [0x0118] EPWM Leading Edge Blanking Control Register                       */
    __IO uint32_t LEBCNT;                /*!< [0x011c] EPWM Leading Edge Blanking Counter Register                       */
    __IO uint32_t STATUS;                /*!< [0x0120] EPWM Status Register                                              */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE8[3];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t IFA[6];                /*!< [0x0130] EPWM Interrupt Flag Accumulator Register 0~5                      */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE9[2];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t AINTSTS;               /*!< [0x0150] EPWM Accumulator Interrupt Flag Register                          */
    __IO uint32_t AINTEN;                /*!< [0x0154] EPWM Accumulator Interrupt Enable Register                        */
    __IO uint32_t APDMACTL;              /*!< [0x0158] EPWM Accumulator PDMA Control Register                            */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE10[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t FDEN;                  /*!< [0x0160] EPWM Fault Detect Enable Register                                 */
    __IO uint32_t FDCTL[6];              /*!< [0x0164~0x178] EPWM Fault Detect Control Register 0~5                      */
    __IO uint32_t FDIEN;                 /*!< [0x017C] EPWM Fault Detect Interrupt Enable Register                       */
    __IO uint32_t FDSTS;                 /*!< [0x0180] EPWM Fault Detect Interrupt Flag Register                         */
    __IO uint32_t EADCPSCCTL;            /*!< [0x0184] EPWM Trigger EADC Prescale Control Register                       */
    __IO uint32_t EADCPSC0;              /*!< [0x0188] EPWM Trigger EADC Prescale Register 0                             */
    __IO uint32_t EADCPSC1;              /*!< [0x018C] EPWM Trigger EADC Prescale Register 1                             */
    __IO uint32_t EADCPSCNT0;            /*!< [0x0190] EPWM Trigger EADC Prescale Counter Register 0                     */
    __IO uint32_t EADCPSCNT1;            /*!< [0x0194] EPWM Trigger EADC Prescale Counter Register 1                     */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE11[26];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t CAPINEN;               /*!< [0x0200] EPWM Capture Input Enable Register                                */
    __IO uint32_t CAPCTL;                /*!< [0x0204] EPWM Capture Control Register                                     */
    __I  uint32_t CAPSTS;                /*!< [0x0208] EPWM Capture Status Register                                      */
    ECAPDAT_T CAPDAT[6];                  /*!< [0x020C] EPWM Rising and Falling Capture Data Register 0~5                 */
    __IO uint32_t PDMACTL;               /*!< [0x023c] EPWM PDMA Control Register                                        */
    __I  uint32_t PDMACAP[3];            /*!< [0x0240] EPWM Capture Channel 01,23,45 PDMA Register                       */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE12[1];
    /// @endcond //HIDDEN_SYMBOLS
    __IO uint32_t CAPIEN;                /*!< [0x0250] EPWM Capture Interrupt Enable Register                            */
    __IO uint32_t CAPIF;                 /*!< [0x0254] EPWM Capture Interrupt Flag Register                              */
    /// @cond HIDDEN_SYMBOLS
    __I  uint32_t RESERVE13[43];
    /// @endcond //HIDDEN_SYMBOLS
    __I  uint32_t PBUF[6];               /*!< [0x0304] EPWM PERIOD0~5 Buffer                                             */
    __I  uint32_t CMPBUF[6];             /*!< [0x031c] EPWM CMPDAT0~5 Buffer                                             */
    __I  uint32_t CPSCBUF[3];            /*!< [0x0334] EPWM CLKPSC0_1/2_3/4_5 Buffer                                     */
    __I  uint32_t FTCBUF[3];             /*!< [0x0340] EPWM FTCMPDAT0_1/2_3/4_5 Buffer                                   */
    __IO uint32_t FTCI;                  /*!< [0x034c] EPWM FTCMPDAT Indicator Register                                  */

} EPWM_T;

/**
    @addtogroup EPWM_CONST EPWM Bit Field Definition
    Constant Definitions for EPWM Controller
@{ */

#define EPWM_CTL0_CTRLD0_Pos              (0)                                               /*!< EPWM_T::CTL0: CTRLD0 Position           */
#define EPWM_CTL0_CTRLD0_Msk              (0x1ul << EPWM_CTL0_CTRLD0_Pos)                   /*!< EPWM_T::CTL0: CTRLD0 Mask               */

#define EPWM_CTL0_CTRLD1_Pos              (1)                                               /*!< EPWM_T::CTL0: CTRLD1 Position           */
#define EPWM_CTL0_CTRLD1_Msk              (0x1ul << EPWM_CTL0_CTRLD1_Pos)                   /*!< EPWM_T::CTL0: CTRLD1 Mask               */

#define EPWM_CTL0_CTRLD2_Pos              (2)                                               /*!< EPWM_T::CTL0: CTRLD2 Position           */
#define EPWM_CTL0_CTRLD2_Msk              (0x1ul << EPWM_CTL0_CTRLD2_Pos)                    /*!< EPWM_T::CTL0: CTRLD2 Mask               */

#define EPWM_CTL0_CTRLD3_Pos              (3)                                               /*!< EPWM_T::CTL0: CTRLD3 Position           */
#define EPWM_CTL0_CTRLD3_Msk              (0x1ul << EPWM_CTL0_CTRLD3_Pos)                    /*!< EPWM_T::CTL0: CTRLD3 Mask               */

#define EPWM_CTL0_CTRLD4_Pos              (4)                                               /*!< EPWM_T::CTL0: CTRLD4 Position           */
#define EPWM_CTL0_CTRLD4_Msk              (0x1ul << EPWM_CTL0_CTRLD4_Pos)                    /*!< EPWM_T::CTL0: CTRLD4 Mask               */

#define EPWM_CTL0_CTRLD5_Pos              (5)                                               /*!< EPWM_T::CTL0: CTRLD5 Position           */
#define EPWM_CTL0_CTRLD5_Msk              (0x1ul << EPWM_CTL0_CTRLD5_Pos)                    /*!< EPWM_T::CTL0: CTRLD5 Mask               */

#define EPWM_CTL0_WINLDEN0_Pos            (8)                                               /*!< EPWM_T::CTL0: WINLDEN0 Position         */
#define EPWM_CTL0_WINLDEN0_Msk            (0x1ul << EPWM_CTL0_WINLDEN0_Pos)                  /*!< EPWM_T::CTL0: WINLDEN0 Mask             */

#define EPWM_CTL0_WINLDEN1_Pos            (9)                                               /*!< EPWM_T::CTL0: WINLDEN1 Position         */
#define EPWM_CTL0_WINLDEN1_Msk            (0x1ul << EPWM_CTL0_WINLDEN1_Pos)                  /*!< EPWM_T::CTL0: WINLDEN1 Mask             */

#define EPWM_CTL0_WINLDEN2_Pos            (10)                                              /*!< EPWM_T::CTL0: WINLDEN2 Position         */
#define EPWM_CTL0_WINLDEN2_Msk            (0x1ul << EPWM_CTL0_WINLDEN2_Pos)                  /*!< EPWM_T::CTL0: WINLDEN2 Mask             */

#define EPWM_CTL0_WINLDEN3_Pos            (11)                                              /*!< EPWM_T::CTL0: WINLDEN3 Position         */
#define EPWM_CTL0_WINLDEN3_Msk            (0x1ul << EPWM_CTL0_WINLDEN3_Pos)                  /*!< EPWM_T::CTL0: WINLDEN3 Mask             */

#define EPWM_CTL0_WINLDEN4_Pos            (12)                                              /*!< EPWM_T::CTL0: WINLDEN4 Position         */
#define EPWM_CTL0_WINLDEN4_Msk            (0x1ul << EPWM_CTL0_WINLDEN4_Pos)                  /*!< EPWM_T::CTL0: WINLDEN4 Mask             */

#define EPWM_CTL0_WINLDEN5_Pos            (13)                                              /*!< EPWM_T::CTL0: WINLDEN5 Position         */
#define EPWM_CTL0_WINLDEN5_Msk            (0x1ul << EPWM_CTL0_WINLDEN5_Pos)                  /*!< EPWM_T::CTL0: WINLDEN5 Mask             */

#define EPWM_CTL0_IMMLDEN0_Pos            (16)                                              /*!< EPWM_T::CTL0: IMMLDEN0 Position         */
#define EPWM_CTL0_IMMLDEN0_Msk            (0x1ul << EPWM_CTL0_IMMLDEN0_Pos)                  /*!< EPWM_T::CTL0: IMMLDEN0 Mask             */

#define EPWM_CTL0_IMMLDEN1_Pos            (17)                                              /*!< EPWM_T::CTL0: IMMLDEN1 Position         */
#define EPWM_CTL0_IMMLDEN1_Msk            (0x1ul << EPWM_CTL0_IMMLDEN1_Pos)                  /*!< EPWM_T::CTL0: IMMLDEN1 Mask             */

#define EPWM_CTL0_IMMLDEN2_Pos            (18)                                              /*!< EPWM_T::CTL0: IMMLDEN2 Position         */
#define EPWM_CTL0_IMMLDEN2_Msk            (0x1ul << EPWM_CTL0_IMMLDEN2_Pos)                  /*!< EPWM_T::CTL0: IMMLDEN2 Mask             */

#define EPWM_CTL0_IMMLDEN3_Pos            (19)                                              /*!< EPWM_T::CTL0: IMMLDEN3 Position         */
#define EPWM_CTL0_IMMLDEN3_Msk            (0x1ul << EPWM_CTL0_IMMLDEN3_Pos)                  /*!< EPWM_T::CTL0: IMMLDEN3 Mask             */

#define EPWM_CTL0_IMMLDEN4_Pos            (20)                                              /*!< EPWM_T::CTL0: IMMLDEN4 Position         */
#define EPWM_CTL0_IMMLDEN4_Msk            (0x1ul << EPWM_CTL0_IMMLDEN4_Pos)                  /*!< EPWM_T::CTL0: IMMLDEN4 Mask             */

#define EPWM_CTL0_IMMLDEN5_Pos            (21)                                              /*!< EPWM_T::CTL0: IMMLDEN5 Position         */
#define EPWM_CTL0_IMMLDEN5_Msk            (0x1ul << EPWM_CTL0_IMMLDEN5_Pos)                  /*!< EPWM_T::CTL0: IMMLDEN5 Mask             */

#define EPWM_CTL0_GROUPEN_Pos             (24)                                              /*!< EPWM_T::CTL0: GROUPEN Position          */
#define EPWM_CTL0_GROUPEN_Msk             (0x1ul << EPWM_CTL0_GROUPEN_Pos)                   /*!< EPWM_T::CTL0: GROUPEN Mask              */

#define EPWM_CTL0_DBGHALT_Pos             (30)                                              /*!< EPWM_T::CTL0: DBGHALT Position          */
#define EPWM_CTL0_DBGHALT_Msk             (0x1ul << EPWM_CTL0_DBGHALT_Pos)                   /*!< EPWM_T::CTL0: DBGHALT Mask              */

#define EPWM_CTL0_DBGTRIOFF_Pos           (31)                                              /*!< EPWM_T::CTL0: DBGTRIOFF Position        */
#define EPWM_CTL0_DBGTRIOFF_Msk           (0x1ul << EPWM_CTL0_DBGTRIOFF_Pos)                 /*!< EPWM_T::CTL0: DBGTRIOFF Mask            */

#define EPWM_CTL1_CNTTYPE0_Pos            (0)                                               /*!< EPWM_T::CTL1: CNTTYPE0 Position         */
#define EPWM_CTL1_CNTTYPE0_Msk            (0x3ul << EPWM_CTL1_CNTTYPE0_Pos)                  /*!< EPWM_T::CTL1: CNTTYPE0 Mask             */

#define EPWM_CTL1_CNTTYPE1_Pos            (2)                                               /*!< EPWM_T::CTL1: CNTTYPE1 Position         */
#define EPWM_CTL1_CNTTYPE1_Msk            (0x3ul << EPWM_CTL1_CNTTYPE1_Pos)                  /*!< EPWM_T::CTL1: CNTTYPE1 Mask             */

#define EPWM_CTL1_CNTTYPE2_Pos            (4)                                               /*!< EPWM_T::CTL1: CNTTYPE2 Position         */
#define EPWM_CTL1_CNTTYPE2_Msk            (0x3ul << EPWM_CTL1_CNTTYPE2_Pos)                  /*!< EPWM_T::CTL1: CNTTYPE2 Mask             */

#define EPWM_CTL1_CNTTYPE3_Pos            (6)                                               /*!< EPWM_T::CTL1: CNTTYPE3 Position         */
#define EPWM_CTL1_CNTTYPE3_Msk            (0x3ul << EPWM_CTL1_CNTTYPE3_Pos)                  /*!< EPWM_T::CTL1: CNTTYPE3 Mask             */

#define EPWM_CTL1_CNTTYPE4_Pos            (8)                                               /*!< EPWM_T::CTL1: CNTTYPE4 Position         */
#define EPWM_CTL1_CNTTYPE4_Msk            (0x3ul << EPWM_CTL1_CNTTYPE4_Pos)                  /*!< EPWM_T::CTL1: CNTTYPE4 Mask             */

#define EPWM_CTL1_CNTTYPE5_Pos            (10)                                              /*!< EPWM_T::CTL1: CNTTYPE5 Position         */
#define EPWM_CTL1_CNTTYPE5_Msk            (0x3ul << EPWM_CTL1_CNTTYPE5_Pos)                  /*!< EPWM_T::CTL1: CNTTYPE5 Mask             */

#define EPWM_CTL1_CNTMODE0_Pos            (16)                                              /*!< EPWM_T::CTL1: CNTMODE0 Position         */
#define EPWM_CTL1_CNTMODE0_Msk            (0x1ul << EPWM_CTL1_CNTMODE0_Pos)                  /*!< EPWM_T::CTL1: CNTMODE0 Mask             */

#define EPWM_CTL1_CNTMODE1_Pos            (17)                                              /*!< EPWM_T::CTL1: CNTMODE1 Position         */
#define EPWM_CTL1_CNTMODE1_Msk            (0x1ul << EPWM_CTL1_CNTMODE1_Pos)                  /*!< EPWM_T::CTL1: CNTMODE1 Mask             */

#define EPWM_CTL1_CNTMODE2_Pos            (18)                                              /*!< EPWM_T::CTL1: CNTMODE2 Position         */
#define EPWM_CTL1_CNTMODE2_Msk            (0x1ul << EPWM_CTL1_CNTMODE2_Pos)                  /*!< EPWM_T::CTL1: CNTMODE2 Mask             */

#define EPWM_CTL1_CNTMODE3_Pos            (19)                                              /*!< EPWM_T::CTL1: CNTMODE3 Position         */
#define EPWM_CTL1_CNTMODE3_Msk            (0x1ul << EPWM_CTL1_CNTMODE3_Pos)                  /*!< EPWM_T::CTL1: CNTMODE3 Mask             */

#define EPWM_CTL1_CNTMODE4_Pos            (20)                                              /*!< EPWM_T::CTL1: CNTMODE4 Position         */
#define EPWM_CTL1_CNTMODE4_Msk            (0x1ul << EPWM_CTL1_CNTMODE4_Pos)                  /*!< EPWM_T::CTL1: CNTMODE4 Mask             */

#define EPWM_CTL1_CNTMODE5_Pos            (21)                                              /*!< EPWM_T::CTL1: CNTMODE5 Position         */
#define EPWM_CTL1_CNTMODE5_Msk            (0x1ul << EPWM_CTL1_CNTMODE5_Pos)                  /*!< EPWM_T::CTL1: CNTMODE5 Mask             */

#define EPWM_CTL1_OUTMODE0_Pos            (24)                                              /*!< EPWM_T::CTL1: OUTMODE0 Position         */
#define EPWM_CTL1_OUTMODE0_Msk            (0x1ul << EPWM_CTL1_OUTMODE0_Pos)                  /*!< EPWM_T::CTL1: OUTMODE0 Mask             */

#define EPWM_CTL1_OUTMODE2_Pos            (25)                                              /*!< EPWM_T::CTL1: OUTMODE2 Position         */
#define EPWM_CTL1_OUTMODE2_Msk            (0x1ul << EPWM_CTL1_OUTMODE2_Pos)                  /*!< EPWM_T::CTL1: OUTMODE2 Mask             */

#define EPWM_CTL1_OUTMODE4_Pos            (26)                                              /*!< EPWM_T::CTL1: OUTMODE4 Position         */
#define EPWM_CTL1_OUTMODE4_Msk            (0x1ul << EPWM_CTL1_OUTMODE4_Pos)                  /*!< EPWM_T::CTL1: OUTMODE4 Mask             */

#define EPWM_SYNC_PHSEN0_Pos              (0)                                               /*!< EPWM_T::SYNC: PHSEN0 Position           */
#define EPWM_SYNC_PHSEN0_Msk              (0x1ul << EPWM_SYNC_PHSEN0_Pos)                    /*!< EPWM_T::SYNC: PHSEN0 Mask               */

#define EPWM_SYNC_PHSEN2_Pos              (1)                                               /*!< EPWM_T::SYNC: PHSEN2 Position           */
#define EPWM_SYNC_PHSEN2_Msk              (0x1ul << EPWM_SYNC_PHSEN2_Pos)                    /*!< EPWM_T::SYNC: PHSEN2 Mask               */

#define EPWM_SYNC_PHSEN4_Pos              (2)                                               /*!< EPWM_T::SYNC: PHSEN4 Position           */
#define EPWM_SYNC_PHSEN4_Msk              (0x1ul << EPWM_SYNC_PHSEN4_Pos)                    /*!< EPWM_T::SYNC: PHSEN4 Mask               */

#define EPWM_SYNC_SINSRC0_Pos             (8)                                               /*!< EPWM_T::SYNC: SINSRC0 Position          */
#define EPWM_SYNC_SINSRC0_Msk             (0x3ul << EPWM_SYNC_SINSRC0_Pos)                   /*!< EPWM_T::SYNC: SINSRC0 Mask              */

#define EPWM_SYNC_SINSRC2_Pos             (10)                                              /*!< EPWM_T::SYNC: SINSRC2 Position          */
#define EPWM_SYNC_SINSRC2_Msk             (0x3ul << EPWM_SYNC_SINSRC2_Pos)                   /*!< EPWM_T::SYNC: SINSRC2 Mask              */

#define EPWM_SYNC_SINSRC4_Pos             (12)                                              /*!< EPWM_T::SYNC: SINSRC4 Position          */
#define EPWM_SYNC_SINSRC4_Msk             (0x3ul << EPWM_SYNC_SINSRC4_Pos)                   /*!< EPWM_T::SYNC: SINSRC4 Mask              */

#define EPWM_SYNC_SNFLTEN_Pos             (16)                                              /*!< EPWM_T::SYNC: SNFLTEN Position          */
#define EPWM_SYNC_SNFLTEN_Msk             (0x1ul << EPWM_SYNC_SNFLTEN_Pos)                   /*!< EPWM_T::SYNC: SNFLTEN Mask              */

#define EPWM_SYNC_SFLTCSEL_Pos            (17)                                              /*!< EPWM_T::SYNC: SFLTCSEL Position         */
#define EPWM_SYNC_SFLTCSEL_Msk            (0x7ul << EPWM_SYNC_SFLTCSEL_Pos)                  /*!< EPWM_T::SYNC: SFLTCSEL Mask             */

#define EPWM_SYNC_SFLTCNT_Pos             (20)                                              /*!< EPWM_T::SYNC: SFLTCNT Position          */
#define EPWM_SYNC_SFLTCNT_Msk             (0x7ul << EPWM_SYNC_SFLTCNT_Pos)                   /*!< EPWM_T::SYNC: SFLTCNT Mask              */

#define EPWM_SYNC_SINPINV_Pos             (23)                                              /*!< EPWM_T::SYNC: SINPINV Position          */
#define EPWM_SYNC_SINPINV_Msk             (0x1ul << EPWM_SYNC_SINPINV_Pos)                   /*!< EPWM_T::SYNC: SINPINV Mask              */

#define EPWM_SYNC_PHSDIR0_Pos             (24)                                              /*!< EPWM_T::SYNC: PHSDIR0 Position          */
#define EPWM_SYNC_PHSDIR0_Msk             (0x1ul << EPWM_SYNC_PHSDIR0_Pos)                   /*!< EPWM_T::SYNC: PHSDIR0 Mask              */

#define EPWM_SYNC_PHSDIR2_Pos             (25)                                              /*!< EPWM_T::SYNC: PHSDIR2 Position          */
#define EPWM_SYNC_PHSDIR2_Msk             (0x1ul << EPWM_SYNC_PHSDIR2_Pos)                   /*!< EPWM_T::SYNC: PHSDIR2 Mask              */

#define EPWM_SYNC_PHSDIR4_Pos             (26)                                              /*!< EPWM_T::SYNC: PHSDIR4 Position          */
#define EPWM_SYNC_PHSDIR4_Msk             (0x1ul << EPWM_SYNC_PHSDIR4_Pos)                   /*!< EPWM_T::SYNC: PHSDIR4 Mask              */

#define EPWM_SWSYNC_SWSYNC0_Pos           (0)                                               /*!< EPWM_T::SWSYNC: SWSYNC0 Position        */
#define EPWM_SWSYNC_SWSYNC0_Msk           (0x1ul << EPWM_SWSYNC_SWSYNC0_Pos)                 /*!< EPWM_T::SWSYNC: SWSYNC0 Mask            */

#define EPWM_SWSYNC_SWSYNC2_Pos           (1)                                               /*!< EPWM_T::SWSYNC: SWSYNC2 Position        */
#define EPWM_SWSYNC_SWSYNC2_Msk           (0x1ul << EPWM_SWSYNC_SWSYNC2_Pos)                 /*!< EPWM_T::SWSYNC: SWSYNC2 Mask            */

#define EPWM_SWSYNC_SWSYNC4_Pos           (2)                                               /*!< EPWM_T::SWSYNC: SWSYNC4 Position        */
#define EPWM_SWSYNC_SWSYNC4_Msk           (0x1ul << EPWM_SWSYNC_SWSYNC4_Pos)                 /*!< EPWM_T::SWSYNC: SWSYNC4 Mask            */

#define EPWM_CLKSRC_ECLKSRC0_Pos          (0)                                               /*!< EPWM_T::CLKSRC: ECLKSRC0 Position       */
#define EPWM_CLKSRC_ECLKSRC0_Msk          (0x7ul << EPWM_CLKSRC_ECLKSRC0_Pos)                /*!< EPWM_T::CLKSRC: ECLKSRC0 Mask           */

#define EPWM_CLKSRC_ECLKSRC2_Pos          (8)                                               /*!< EPWM_T::CLKSRC: ECLKSRC2 Position       */
#define EPWM_CLKSRC_ECLKSRC2_Msk          (0x7ul << EPWM_CLKSRC_ECLKSRC2_Pos)                /*!< EPWM_T::CLKSRC: ECLKSRC2 Mask           */

#define EPWM_CLKSRC_ECLKSRC4_Pos          (16)                                              /*!< EPWM_T::CLKSRC: ECLKSRC4 Position       */
#define EPWM_CLKSRC_ECLKSRC4_Msk          (0x7ul << EPWM_CLKSRC_ECLKSRC4_Pos)                /*!< EPWM_T::CLKSRC: ECLKSRC4 Mask           */

#define EPWM_CLKPSC0_1_CLKPSC_Pos         (0)                                               /*!< EPWM_T::CLKPSC0_1: CLKPSC Position      */
#define EPWM_CLKPSC0_1_CLKPSC_Msk         (0xffful << EPWM_CLKPSC0_1_CLKPSC_Pos)             /*!< EPWM_T::CLKPSC0_1: CLKPSC Mask          */

#define EPWM_CLKPSC2_3_CLKPSC_Pos         (0)                                               /*!< EPWM_T::CLKPSC2_3: CLKPSC Position      */
#define EPWM_CLKPSC2_3_CLKPSC_Msk         (0xffful << EPWM_CLKPSC2_3_CLKPSC_Pos)             /*!< EPWM_T::CLKPSC2_3: CLKPSC Mask          */

#define EPWM_CLKPSC4_5_CLKPSC_Pos         (0)                                               /*!< EPWM_T::CLKPSC4_5: CLKPSC Position      */
#define EPWM_CLKPSC4_5_CLKPSC_Msk         (0xffful << EPWM_CLKPSC4_5_CLKPSC_Pos)             /*!< EPWM_T::CLKPSC4_5: CLKPSC Mask          */

#define EPWM_CNTEN_CNTEN0_Pos             (0)                                               /*!< EPWM_T::CNTEN: CNTEN0 Position          */
#define EPWM_CNTEN_CNTEN0_Msk             (0x1ul << EPWM_CNTEN_CNTEN0_Pos)                   /*!< EPWM_T::CNTEN: CNTEN0 Mask              */

#define EPWM_CNTEN_CNTEN1_Pos             (1)                                               /*!< EPWM_T::CNTEN: CNTEN1 Position          */
#define EPWM_CNTEN_CNTEN1_Msk             (0x1ul << EPWM_CNTEN_CNTEN1_Pos)                   /*!< EPWM_T::CNTEN: CNTEN1 Mask              */

#define EPWM_CNTEN_CNTEN2_Pos             (2)                                               /*!< EPWM_T::CNTEN: CNTEN2 Position          */
#define EPWM_CNTEN_CNTEN2_Msk             (0x1ul << EPWM_CNTEN_CNTEN2_Pos)                   /*!< EPWM_T::CNTEN: CNTEN2 Mask              */

#define EPWM_CNTEN_CNTEN3_Pos             (3)                                               /*!< EPWM_T::CNTEN: CNTEN3 Position          */
#define EPWM_CNTEN_CNTEN3_Msk             (0x1ul << EPWM_CNTEN_CNTEN3_Pos)                   /*!< EPWM_T::CNTEN: CNTEN3 Mask              */

#define EPWM_CNTEN_CNTEN4_Pos             (4)                                               /*!< EPWM_T::CNTEN: CNTEN4 Position          */
#define EPWM_CNTEN_CNTEN4_Msk             (0x1ul << EPWM_CNTEN_CNTEN4_Pos)                   /*!< EPWM_T::CNTEN: CNTEN4 Mask              */

#define EPWM_CNTEN_CNTEN5_Pos             (5)                                               /*!< EPWM_T::CNTEN: CNTEN5 Position          */
#define EPWM_CNTEN_CNTEN5_Msk             (0x1ul << EPWM_CNTEN_CNTEN5_Pos)                   /*!< EPWM_T::CNTEN: CNTEN5 Mask              */

#define EPWM_CNTCLR_CNTCLR0_Pos           (0)                                               /*!< EPWM_T::CNTCLR: CNTCLR0 Position        */
#define EPWM_CNTCLR_CNTCLR0_Msk           (0x1ul << EPWM_CNTCLR_CNTCLR0_Pos)                 /*!< EPWM_T::CNTCLR: CNTCLR0 Mask            */

#define EPWM_CNTCLR_CNTCLR1_Pos           (1)                                               /*!< EPWM_T::CNTCLR: CNTCLR1 Position        */
#define EPWM_CNTCLR_CNTCLR1_Msk           (0x1ul << EPWM_CNTCLR_CNTCLR1_Pos)                 /*!< EPWM_T::CNTCLR: CNTCLR1 Mask            */

#define EPWM_CNTCLR_CNTCLR2_Pos           (2)                                               /*!< EPWM_T::CNTCLR: CNTCLR2 Position        */
#define EPWM_CNTCLR_CNTCLR2_Msk           (0x1ul << EPWM_CNTCLR_CNTCLR2_Pos)                 /*!< EPWM_T::CNTCLR: CNTCLR2 Mask            */

#define EPWM_CNTCLR_CNTCLR3_Pos           (3)                                               /*!< EPWM_T::CNTCLR: CNTCLR3 Position        */
#define EPWM_CNTCLR_CNTCLR3_Msk           (0x1ul << EPWM_CNTCLR_CNTCLR3_Pos)                 /*!< EPWM_T::CNTCLR: CNTCLR3 Mask            */

#define EPWM_CNTCLR_CNTCLR4_Pos           (4)                                               /*!< EPWM_T::CNTCLR: CNTCLR4 Position        */
#define EPWM_CNTCLR_CNTCLR4_Msk           (0x1ul << EPWM_CNTCLR_CNTCLR4_Pos)                 /*!< EPWM_T::CNTCLR: CNTCLR4 Mask            */

#define EPWM_CNTCLR_CNTCLR5_Pos           (5)                                               /*!< EPWM_T::CNTCLR: CNTCLR5 Position        */
#define EPWM_CNTCLR_CNTCLR5_Msk           (0x1ul << EPWM_CNTCLR_CNTCLR5_Pos)                 /*!< EPWM_T::CNTCLR: CNTCLR5 Mask            */

#define EPWM_LOAD_LOAD0_Pos               (0)                                               /*!< EPWM_T::LOAD: LOAD0 Position            */
#define EPWM_LOAD_LOAD0_Msk               (0x1ul << EPWM_LOAD_LOAD0_Pos)                     /*!< EPWM_T::LOAD: LOAD0 Mask                */

#define EPWM_LOAD_LOAD1_Pos               (1)                                               /*!< EPWM_T::LOAD: LOAD1 Position            */
#define EPWM_LOAD_LOAD1_Msk               (0x1ul << EPWM_LOAD_LOAD1_Pos)                     /*!< EPWM_T::LOAD: LOAD1 Mask                */

#define EPWM_LOAD_LOAD2_Pos               (2)                                               /*!< EPWM_T::LOAD: LOAD2 Position            */
#define EPWM_LOAD_LOAD2_Msk               (0x1ul << EPWM_LOAD_LOAD2_Pos)                     /*!< EPWM_T::LOAD: LOAD2 Mask                */

#define EPWM_LOAD_LOAD3_Pos               (3)                                               /*!< EPWM_T::LOAD: LOAD3 Position            */
#define EPWM_LOAD_LOAD3_Msk               (0x1ul << EPWM_LOAD_LOAD3_Pos)                     /*!< EPWM_T::LOAD: LOAD3 Mask                */

#define EPWM_LOAD_LOAD4_Pos               (4)                                               /*!< EPWM_T::LOAD: LOAD4 Position            */
#define EPWM_LOAD_LOAD4_Msk               (0x1ul << EPWM_LOAD_LOAD4_Pos)                     /*!< EPWM_T::LOAD: LOAD4 Mask                */

#define EPWM_LOAD_LOAD5_Pos               (5)                                               /*!< EPWM_T::LOAD: LOAD5 Position            */
#define EPWM_LOAD_LOAD5_Msk               (0x1ul << EPWM_LOAD_LOAD5_Pos)                     /*!< EPWM_T::LOAD: LOAD5 Mask                */

#define EPWM_PERIOD0_PERIOD_Pos           (0)                                               /*!< EPWM_T::PERIOD0: PERIOD Position        */
#define EPWM_PERIOD0_PERIOD_Msk           (0xfffful << EPWM_PERIOD0_PERIOD_Pos)              /*!< EPWM_T::PERIOD0: PERIOD Mask            */

#define EPWM_PERIOD1_PERIOD_Pos           (0)                                               /*!< EPWM_T::PERIOD1: PERIOD Position        */
#define EPWM_PERIOD1_PERIOD_Msk           (0xfffful << EPWM_PERIOD1_PERIOD_Pos)              /*!< EPWM_T::PERIOD1: PERIOD Mask            */

#define EPWM_PERIOD2_PERIOD_Pos           (0)                                               /*!< EPWM_T::PERIOD2: PERIOD Position        */
#define EPWM_PERIOD2_PERIOD_Msk           (0xfffful << EPWM_PERIOD2_PERIOD_Pos)              /*!< EPWM_T::PERIOD2: PERIOD Mask            */

#define EPWM_PERIOD3_PERIOD_Pos           (0)                                               /*!< EPWM_T::PERIOD3: PERIOD Position        */
#define EPWM_PERIOD3_PERIOD_Msk           (0xfffful << EPWM_PERIOD3_PERIOD_Pos)              /*!< EPWM_T::PERIOD3: PERIOD Mask            */

#define EPWM_PERIOD4_PERIOD_Pos           (0)                                               /*!< EPWM_T::PERIOD4: PERIOD Position        */
#define EPWM_PERIOD4_PERIOD_Msk           (0xfffful << EPWM_PERIOD4_PERIOD_Pos)              /*!< EPWM_T::PERIOD4: PERIOD Mask            */

#define EPWM_PERIOD5_PERIOD_Pos           (0)                                               /*!< EPWM_T::PERIOD5: PERIOD Position        */
#define EPWM_PERIOD5_PERIOD_Msk           (0xfffful << EPWM_PERIOD5_PERIOD_Pos)              /*!< EPWM_T::PERIOD5: PERIOD Mask            */

#define EPWM_CMPDAT0_CMP_Pos              (0)                                               /*!< EPWM_T::CMPDAT0: CMP Position           */
#define EPWM_CMPDAT0_CMP_Msk              (0xfffful << EPWM_CMPDAT0_CMP_Pos)                 /*!< EPWM_T::CMPDAT0: CMP Mask               */

#define EPWM_CMPDAT1_CMP_Pos              (0)                                               /*!< EPWM_T::CMPDAT1: CMP Position           */
#define EPWM_CMPDAT1_CMP_Msk              (0xfffful << EPWM_CMPDAT1_CMP_Pos)                 /*!< EPWM_T::CMPDAT1: CMP Mask               */

#define EPWM_CMPDAT2_CMP_Pos              (0)                                               /*!< EPWM_T::CMPDAT2: CMP Position           */
#define EPWM_CMPDAT2_CMP_Msk              (0xfffful << EPWM_CMPDAT2_CMP_Pos)                 /*!< EPWM_T::CMPDAT2: CMP Mask               */

#define EPWM_CMPDAT3_CMP_Pos              (0)                                               /*!< EPWM_T::CMPDAT3: CMP Position           */
#define EPWM_CMPDAT3_CMP_Msk              (0xfffful << EPWM_CMPDAT3_CMP_Pos)                 /*!< EPWM_T::CMPDAT3: CMP Mask               */

#define EPWM_CMPDAT4_CMP_Pos              (0)                                               /*!< EPWM_T::CMPDAT4: CMP Position           */
#define EPWM_CMPDAT4_CMP_Msk              (0xfffful << EPWM_CMPDAT4_CMP_Pos)                 /*!< EPWM_T::CMPDAT4: CMP Mask               */

#define EPWM_CMPDAT5_CMP_Pos              (0)                                               /*!< EPWM_T::CMPDAT5: CMP Position           */
#define EPWM_CMPDAT5_CMP_Msk              (0xfffful << EPWM_CMPDAT5_CMP_Pos)                 /*!< EPWM_T::CMPDAT5: CMP Mask               */

#define EPWM_DTCTL0_1_DTCNT_Pos           (0)                                               /*!< EPWM_T::DTCTL0_1: DTCNT Position        */
#define EPWM_DTCTL0_1_DTCNT_Msk           (0xffful << EPWM_DTCTL0_1_DTCNT_Pos)               /*!< EPWM_T::DTCTL0_1: DTCNT Mask            */

#define EPWM_DTCTL0_1_DTEN_Pos            (16)                                              /*!< EPWM_T::DTCTL0_1: DTEN Position         */
#define EPWM_DTCTL0_1_DTEN_Msk            (0x1ul << EPWM_DTCTL0_1_DTEN_Pos)                  /*!< EPWM_T::DTCTL0_1: DTEN Mask             */

#define EPWM_DTCTL0_1_DTCKSEL_Pos         (24)                                              /*!< EPWM_T::DTCTL0_1: DTCKSEL Position      */
#define EPWM_DTCTL0_1_DTCKSEL_Msk         (0x1ul << EPWM_DTCTL0_1_DTCKSEL_Pos)               /*!< EPWM_T::DTCTL0_1: DTCKSEL Mask          */

#define EPWM_DTCTL2_3_DTCNT_Pos           (0)                                               /*!< EPWM_T::DTCTL2_3: DTCNT Position        */
#define EPWM_DTCTL2_3_DTCNT_Msk           (0xffful << EPWM_DTCTL2_3_DTCNT_Pos)               /*!< EPWM_T::DTCTL2_3: DTCNT Mask            */

#define EPWM_DTCTL2_3_DTEN_Pos            (16)                                              /*!< EPWM_T::DTCTL2_3: DTEN Position         */
#define EPWM_DTCTL2_3_DTEN_Msk            (0x1ul << EPWM_DTCTL2_3_DTEN_Pos)                  /*!< EPWM_T::DTCTL2_3: DTEN Mask             */

#define EPWM_DTCTL2_3_DTCKSEL_Pos         (24)                                              /*!< EPWM_T::DTCTL2_3: DTCKSEL Position      */
#define EPWM_DTCTL2_3_DTCKSEL_Msk         (0x1ul << EPWM_DTCTL2_3_DTCKSEL_Pos)               /*!< EPWM_T::DTCTL2_3: DTCKSEL Mask          */

#define EPWM_DTCTL4_5_DTCNT_Pos           (0)                                               /*!< EPWM_T::DTCTL4_5: DTCNT Position        */
#define EPWM_DTCTL4_5_DTCNT_Msk           (0xffful << EPWM_DTCTL4_5_DTCNT_Pos)               /*!< EPWM_T::DTCTL4_5: DTCNT Mask            */

#define EPWM_DTCTL4_5_DTEN_Pos            (16)                                              /*!< EPWM_T::DTCTL4_5: DTEN Position         */
#define EPWM_DTCTL4_5_DTEN_Msk            (0x1ul << EPWM_DTCTL4_5_DTEN_Pos)                  /*!< EPWM_T::DTCTL4_5: DTEN Mask             */

#define EPWM_DTCTL4_5_DTCKSEL_Pos         (24)                                              /*!< EPWM_T::DTCTL4_5: DTCKSEL Position      */
#define EPWM_DTCTL4_5_DTCKSEL_Msk         (0x1ul << EPWM_DTCTL4_5_DTCKSEL_Pos)               /*!< EPWM_T::DTCTL4_5: DTCKSEL Mask          */

#define EPWM_PHS0_1_PHS_Pos               (0)                                               /*!< EPWM_T::PHS0_1: PHS Position            */
#define EPWM_PHS0_1_PHS_Msk               (0xfffful << EPWM_PHS0_1_PHS_Pos)                  /*!< EPWM_T::PHS0_1: PHS Mask                */

#define EPWM_PHS2_3_PHS_Pos               (0)                                               /*!< EPWM_T::PHS2_3: PHS Position            */
#define EPWM_PHS2_3_PHS_Msk               (0xfffful << EPWM_PHS2_3_PHS_Pos)                  /*!< EPWM_T::PHS2_3: PHS Mask                */

#define EPWM_PHS4_5_PHS_Pos               (0)                                               /*!< EPWM_T::PHS4_5: PHS Position            */
#define EPWM_PHS4_5_PHS_Msk               (0xfffful << EPWM_PHS4_5_PHS_Pos)                  /*!< EPWM_T::PHS4_5: PHS Mask                */

#define EPWM_CNT0_CNT_Pos                 (0)                                               /*!< EPWM_T::CNT0: CNT Position              */
#define EPWM_CNT0_CNT_Msk                 (0xfffful << EPWM_CNT0_CNT_Pos)                    /*!< EPWM_T::CNT0: CNT Mask                  */

#define EPWM_CNT0_DIRF_Pos                (16)                                              /*!< EPWM_T::CNT0: DIRF Position             */
#define EPWM_CNT0_DIRF_Msk                (0x1ul << EPWM_CNT0_DIRF_Pos)                      /*!< EPWM_T::CNT0: DIRF Mask                 */

#define EPWM_CNT1_CNT_Pos                 (0)                                               /*!< EPWM_T::CNT1: CNT Position              */
#define EPWM_CNT1_CNT_Msk                 (0xfffful << EPWM_CNT1_CNT_Pos)                    /*!< EPWM_T::CNT1: CNT Mask                  */

#define EPWM_CNT1_DIRF_Pos                (16)                                              /*!< EPWM_T::CNT1: DIRF Position             */
#define EPWM_CNT1_DIRF_Msk                (0x1ul << EPWM_CNT1_DIRF_Pos)                      /*!< EPWM_T::CNT1: DIRF Mask                 */

#define EPWM_CNT2_CNT_Pos                 (0)                                               /*!< EPWM_T::CNT2: CNT Position              */
#define EPWM_CNT2_CNT_Msk                 (0xfffful << EPWM_CNT2_CNT_Pos)                    /*!< EPWM_T::CNT2: CNT Mask                  */

#define EPWM_CNT2_DIRF_Pos                (16)                                              /*!< EPWM_T::CNT2: DIRF Position             */
#define EPWM_CNT2_DIRF_Msk                (0x1ul << EPWM_CNT2_DIRF_Pos)                      /*!< EPWM_T::CNT2: DIRF Mask                 */

#define EPWM_CNT3_CNT_Pos                 (0)                                               /*!< EPWM_T::CNT3: CNT Position              */
#define EPWM_CNT3_CNT_Msk                 (0xfffful << EPWM_CNT3_CNT_Pos)                    /*!< EPWM_T::CNT3: CNT Mask                  */

#define EPWM_CNT3_DIRF_Pos                (16)                                              /*!< EPWM_T::CNT3: DIRF Position             */
#define EPWM_CNT3_DIRF_Msk                (0x1ul << EPWM_CNT3_DIRF_Pos)                      /*!< EPWM_T::CNT3: DIRF Mask                 */

#define EPWM_CNT4_CNT_Pos                 (0)                                               /*!< EPWM_T::CNT4: CNT Position              */
#define EPWM_CNT4_CNT_Msk                 (0xfffful << EPWM_CNT4_CNT_Pos)                    /*!< EPWM_T::CNT4: CNT Mask                  */

#define EPWM_CNT4_DIRF_Pos                (16)                                              /*!< EPWM_T::CNT4: DIRF Position             */
#define EPWM_CNT4_DIRF_Msk                (0x1ul << EPWM_CNT4_DIRF_Pos)                      /*!< EPWM_T::CNT4: DIRF Mask                 */

#define EPWM_CNT5_CNT_Pos                 (0)                                               /*!< EPWM_T::CNT5: CNT Position              */
#define EPWM_CNT5_CNT_Msk                 (0xfffful << EPWM_CNT5_CNT_Pos)                    /*!< EPWM_T::CNT5: CNT Mask                  */

#define EPWM_CNT5_DIRF_Pos                (16)                                              /*!< EPWM_T::CNT5: DIRF Position             */
#define EPWM_CNT5_DIRF_Msk                (0x1ul << EPWM_CNT5_DIRF_Pos)                      /*!< EPWM_T::CNT5: DIRF Mask                 */

#define EPWM_WGCTL0_ZPCTL0_Pos            (0)                                               /*!< EPWM_T::WGCTL0: ZPCTL0 Position         */
#define EPWM_WGCTL0_ZPCTL0_Msk            (0x3ul << EPWM_WGCTL0_ZPCTL0_Pos)                  /*!< EPWM_T::WGCTL0: ZPCTL0 Mask             */

#define EPWM_WGCTL0_ZPCTL1_Pos            (2)                                               /*!< EPWM_T::WGCTL0: ZPCTL1 Position         */
#define EPWM_WGCTL0_ZPCTL1_Msk            (0x3ul << EPWM_WGCTL0_ZPCTL1_Pos)                  /*!< EPWM_T::WGCTL0: ZPCTL1 Mask             */

#define EPWM_WGCTL0_ZPCTL2_Pos            (4)                                               /*!< EPWM_T::WGCTL0: ZPCTL2 Position         */
#define EPWM_WGCTL0_ZPCTL2_Msk            (0x3ul << EPWM_WGCTL0_ZPCTL2_Pos)                  /*!< EPWM_T::WGCTL0: ZPCTL2 Mask             */

#define EPWM_WGCTL0_ZPCTL3_Pos            (6)                                               /*!< EPWM_T::WGCTL0: ZPCTL3 Position         */
#define EPWM_WGCTL0_ZPCTL3_Msk            (0x3ul << EPWM_WGCTL0_ZPCTL3_Pos)                  /*!< EPWM_T::WGCTL0: ZPCTL3 Mask             */

#define EPWM_WGCTL0_ZPCTL4_Pos            (8)                                               /*!< EPWM_T::WGCTL0: ZPCTL4 Position         */
#define EPWM_WGCTL0_ZPCTL4_Msk            (0x3ul << EPWM_WGCTL0_ZPCTL4_Pos)                  /*!< EPWM_T::WGCTL0: ZPCTL4 Mask             */

#define EPWM_WGCTL0_ZPCTL5_Pos            (10)                                              /*!< EPWM_T::WGCTL0: ZPCTL5 Position         */
#define EPWM_WGCTL0_ZPCTL5_Msk            (0x3ul << EPWM_WGCTL0_ZPCTL5_Pos)                  /*!< EPWM_T::WGCTL0: ZPCTL5 Mask             */

#define EPWM_WGCTL0_PRDPCTL0_Pos          (16)                                              /*!< EPWM_T::WGCTL0: PRDPCTL0 Position       */
#define EPWM_WGCTL0_PRDPCTL0_Msk          (0x3ul << EPWM_WGCTL0_PRDPCTL0_Pos)                /*!< EPWM_T::WGCTL0: PRDPCTL0 Mask           */

#define EPWM_WGCTL0_PRDPCTL1_Pos          (18)                                              /*!< EPWM_T::WGCTL0: PRDPCTL1 Position       */
#define EPWM_WGCTL0_PRDPCTL1_Msk          (0x3ul << EPWM_WGCTL0_PRDPCTL1_Pos)                /*!< EPWM_T::WGCTL0: PRDPCTL1 Mask           */

#define EPWM_WGCTL0_PRDPCTL2_Pos          (20)                                              /*!< EPWM_T::WGCTL0: PRDPCTL2 Position       */
#define EPWM_WGCTL0_PRDPCTL2_Msk          (0x3ul << EPWM_WGCTL0_PRDPCTL2_Pos)                /*!< EPWM_T::WGCTL0: PRDPCTL2 Mask           */

#define EPWM_WGCTL0_PRDPCTL3_Pos          (22)                                              /*!< EPWM_T::WGCTL0: PRDPCTL3 Position       */
#define EPWM_WGCTL0_PRDPCTL3_Msk          (0x3ul << EPWM_WGCTL0_PRDPCTL3_Pos)                /*!< EPWM_T::WGCTL0: PRDPCTL3 Mask           */

#define EPWM_WGCTL0_PRDPCTL4_Pos          (24)                                              /*!< EPWM_T::WGCTL0: PRDPCTL4 Position       */
#define EPWM_WGCTL0_PRDPCTL4_Msk          (0x3ul << EPWM_WGCTL0_PRDPCTL4_Pos)                /*!< EPWM_T::WGCTL0: PRDPCTL4 Mask           */

#define EPWM_WGCTL0_PRDPCTL5_Pos          (26)                                              /*!< EPWM_T::WGCTL0: PRDPCTL5 Position       */
#define EPWM_WGCTL0_PRDPCTL5_Msk          (0x3ul << EPWM_WGCTL0_PRDPCTL5_Pos)                /*!< EPWM_T::WGCTL0: PRDPCTL5 Mask           */

#define EPWM_WGCTL1_CMPUCTL0_Pos          (0)                                               /*!< EPWM_T::WGCTL1: CMPUCTL0 Position       */
#define EPWM_WGCTL1_CMPUCTL0_Msk          (0x3ul << EPWM_WGCTL1_CMPUCTL0_Pos)                /*!< EPWM_T::WGCTL1: CMPUCTL0 Mask           */

#define EPWM_WGCTL1_CMPUCTL1_Pos          (2)                                               /*!< EPWM_T::WGCTL1: CMPUCTL1 Position       */
#define EPWM_WGCTL1_CMPUCTL1_Msk          (0x3ul << EPWM_WGCTL1_CMPUCTL1_Pos)                /*!< EPWM_T::WGCTL1: CMPUCTL1 Mask           */

#define EPWM_WGCTL1_CMPUCTL2_Pos          (4)                                               /*!< EPWM_T::WGCTL1: CMPUCTL2 Position       */
#define EPWM_WGCTL1_CMPUCTL2_Msk          (0x3ul << EPWM_WGCTL1_CMPUCTL2_Pos)                /*!< EPWM_T::WGCTL1: CMPUCTL2 Mask           */

#define EPWM_WGCTL1_CMPUCTL3_Pos          (6)                                               /*!< EPWM_T::WGCTL1: CMPUCTL3 Position       */
#define EPWM_WGCTL1_CMPUCTL3_Msk          (0x3ul << EPWM_WGCTL1_CMPUCTL3_Pos)                /*!< EPWM_T::WGCTL1: CMPUCTL3 Mask           */

#define EPWM_WGCTL1_CMPUCTL4_Pos          (8)                                               /*!< EPWM_T::WGCTL1: CMPUCTL4 Position       */
#define EPWM_WGCTL1_CMPUCTL4_Msk          (0x3ul << EPWM_WGCTL1_CMPUCTL4_Pos)                /*!< EPWM_T::WGCTL1: CMPUCTL4 Mask           */

#define EPWM_WGCTL1_CMPUCTL5_Pos          (10)                                              /*!< EPWM_T::WGCTL1: CMPUCTL5 Position       */
#define EPWM_WGCTL1_CMPUCTL5_Msk          (0x3ul << EPWM_WGCTL1_CMPUCTL5_Pos)                /*!< EPWM_T::WGCTL1: CMPUCTL5 Mask           */

#define EPWM_WGCTL1_CMPDCTL0_Pos          (16)                                              /*!< EPWM_T::WGCTL1: CMPDCTL0 Position       */
#define EPWM_WGCTL1_CMPDCTL0_Msk          (0x3ul << EPWM_WGCTL1_CMPDCTL0_Pos)                /*!< EPWM_T::WGCTL1: CMPDCTL0 Mask           */

#define EPWM_WGCTL1_CMPDCTL1_Pos          (18)                                              /*!< EPWM_T::WGCTL1: CMPDCTL1 Position       */
#define EPWM_WGCTL1_CMPDCTL1_Msk          (0x3ul << EPWM_WGCTL1_CMPDCTL1_Pos)                /*!< EPWM_T::WGCTL1: CMPDCTL1 Mask           */

#define EPWM_WGCTL1_CMPDCTL2_Pos          (20)                                              /*!< EPWM_T::WGCTL1: CMPDCTL2 Position       */
#define EPWM_WGCTL1_CMPDCTL2_Msk          (0x3ul << EPWM_WGCTL1_CMPDCTL2_Pos)                /*!< EPWM_T::WGCTL1: CMPDCTL2 Mask           */

#define EPWM_WGCTL1_CMPDCTL3_Pos          (22)                                              /*!< EPWM_T::WGCTL1: CMPDCTL3 Position       */
#define EPWM_WGCTL1_CMPDCTL3_Msk          (0x3ul << EPWM_WGCTL1_CMPDCTL3_Pos)                /*!< EPWM_T::WGCTL1: CMPDCTL3 Mask           */

#define EPWM_WGCTL1_CMPDCTL4_Pos          (24)                                              /*!< EPWM_T::WGCTL1: CMPDCTL4 Position       */
#define EPWM_WGCTL1_CMPDCTL4_Msk          (0x3ul << EPWM_WGCTL1_CMPDCTL4_Pos)                /*!< EPWM_T::WGCTL1: CMPDCTL4 Mask           */

#define EPWM_WGCTL1_CMPDCTL5_Pos          (26)                                              /*!< EPWM_T::WGCTL1: CMPDCTL5 Position       */
#define EPWM_WGCTL1_CMPDCTL5_Msk          (0x3ul << EPWM_WGCTL1_CMPDCTL5_Pos)                /*!< EPWM_T::WGCTL1: CMPDCTL5 Mask           */

#define EPWM_MSKEN_MSKEN0_Pos             (0)                                               /*!< EPWM_T::MSKEN: MSKEN0 Position          */
#define EPWM_MSKEN_MSKEN0_Msk             (0x1ul << EPWM_MSKEN_MSKEN0_Pos)                   /*!< EPWM_T::MSKEN: MSKEN0 Mask              */

#define EPWM_MSKEN_MSKEN1_Pos             (1)                                               /*!< EPWM_T::MSKEN: MSKEN1 Position          */
#define EPWM_MSKEN_MSKEN1_Msk             (0x1ul << EPWM_MSKEN_MSKEN1_Pos)                   /*!< EPWM_T::MSKEN: MSKEN1 Mask              */

#define EPWM_MSKEN_MSKEN2_Pos             (2)                                               /*!< EPWM_T::MSKEN: MSKEN2 Position          */
#define EPWM_MSKEN_MSKEN2_Msk             (0x1ul << EPWM_MSKEN_MSKEN2_Pos)                   /*!< EPWM_T::MSKEN: MSKEN2 Mask              */

#define EPWM_MSKEN_MSKEN3_Pos             (3)                                               /*!< EPWM_T::MSKEN: MSKEN3 Position          */
#define EPWM_MSKEN_MSKEN3_Msk             (0x1ul << EPWM_MSKEN_MSKEN3_Pos)                   /*!< EPWM_T::MSKEN: MSKEN3 Mask              */

#define EPWM_MSKEN_MSKEN4_Pos             (4)                                               /*!< EPWM_T::MSKEN: MSKEN4 Position          */
#define EPWM_MSKEN_MSKEN4_Msk             (0x1ul << EPWM_MSKEN_MSKEN4_Pos)                   /*!< EPWM_T::MSKEN: MSKEN4 Mask              */

#define EPWM_MSKEN_MSKEN5_Pos             (5)                                               /*!< EPWM_T::MSKEN: MSKEN5 Position          */
#define EPWM_MSKEN_MSKEN5_Msk             (0x1ul << EPWM_MSKEN_MSKEN5_Pos)                   /*!< EPWM_T::MSKEN: MSKEN5 Mask              */

#define EPWM_MSK_MSKDAT0_Pos              (0)                                               /*!< EPWM_T::MSK: MSKDAT0 Position           */
#define EPWM_MSK_MSKDAT0_Msk              (0x1ul << EPWM_MSK_MSKDAT0_Pos)                    /*!< EPWM_T::MSK: MSKDAT0 Mask               */

#define EPWM_MSK_MSKDAT1_Pos              (1)                                               /*!< EPWM_T::MSK: MSKDAT1 Position           */
#define EPWM_MSK_MSKDAT1_Msk              (0x1ul << EPWM_MSK_MSKDAT1_Pos)                    /*!< EPWM_T::MSK: MSKDAT1 Mask               */

#define EPWM_MSK_MSKDAT2_Pos              (2)                                               /*!< EPWM_T::MSK: MSKDAT2 Position           */
#define EPWM_MSK_MSKDAT2_Msk              (0x1ul << EPWM_MSK_MSKDAT2_Pos)                    /*!< EPWM_T::MSK: MSKDAT2 Mask               */

#define EPWM_MSK_MSKDAT3_Pos              (3)                                               /*!< EPWM_T::MSK: MSKDAT3 Position           */
#define EPWM_MSK_MSKDAT3_Msk              (0x1ul << EPWM_MSK_MSKDAT3_Pos)                    /*!< EPWM_T::MSK: MSKDAT3 Mask               */

#define EPWM_MSK_MSKDAT4_Pos              (4)                                               /*!< EPWM_T::MSK: MSKDAT4 Position           */
#define EPWM_MSK_MSKDAT4_Msk              (0x1ul << EPWM_MSK_MSKDAT4_Pos)                    /*!< EPWM_T::MSK: MSKDAT4 Mask               */

#define EPWM_MSK_MSKDAT5_Pos              (5)                                               /*!< EPWM_T::MSK: MSKDAT5 Position           */
#define EPWM_MSK_MSKDAT5_Msk              (0x1ul << EPWM_MSK_MSKDAT5_Pos)                    /*!< EPWM_T::MSK: MSKDAT5 Mask               */

#define EPWM_BNF_BRK0NFEN_Pos             (0)                                               /*!< EPWM_T::BNF: BRK0NFEN Position          */
#define EPWM_BNF_BRK0NFEN_Msk             (0x1ul << EPWM_BNF_BRK0NFEN_Pos)                   /*!< EPWM_T::BNF: BRK0NFEN Mask              */

#define EPWM_BNF_BRK0NFSEL_Pos            (1)                                               /*!< EPWM_T::BNF: BRK0NFSEL Position         */
#define EPWM_BNF_BRK0NFSEL_Msk            (0x7ul << EPWM_BNF_BRK0NFSEL_Pos)                  /*!< EPWM_T::BNF: BRK0NFSEL Mask             */

#define EPWM_BNF_BRK0FCNT_Pos             (4)                                               /*!< EPWM_T::BNF: BRK0FCNT Position          */
#define EPWM_BNF_BRK0FCNT_Msk             (0x7ul << EPWM_BNF_BRK0FCNT_Pos)                   /*!< EPWM_T::BNF: BRK0FCNT Mask              */

#define EPWM_BNF_BRK0PINV_Pos             (7)                                               /*!< EPWM_T::BNF: BRK0PINV Position          */
#define EPWM_BNF_BRK0PINV_Msk             (0x1ul << EPWM_BNF_BRK0PINV_Pos)                   /*!< EPWM_T::BNF: BRK0PINV Mask              */

#define EPWM_BNF_BRK1NFEN_Pos             (8)                                               /*!< EPWM_T::BNF: BRK1NFEN Position          */
#define EPWM_BNF_BRK1NFEN_Msk             (0x1ul << EPWM_BNF_BRK1NFEN_Pos)                   /*!< EPWM_T::BNF: BRK1NFEN Mask              */

#define EPWM_BNF_BRK1NFSEL_Pos            (9)                                               /*!< EPWM_T::BNF: BRK1NFSEL Position         */
#define EPWM_BNF_BRK1NFSEL_Msk            (0x7ul << EPWM_BNF_BRK1NFSEL_Pos)                  /*!< EPWM_T::BNF: BRK1NFSEL Mask             */

#define EPWM_BNF_BRK1FCNT_Pos             (12)                                              /*!< EPWM_T::BNF: BRK1FCNT Position          */
#define EPWM_BNF_BRK1FCNT_Msk             (0x7ul << EPWM_BNF_BRK1FCNT_Pos)                   /*!< EPWM_T::BNF: BRK1FCNT Mask              */

#define EPWM_BNF_BRK1PINV_Pos             (15)                                              /*!< EPWM_T::BNF: BRK1PINV Position          */
#define EPWM_BNF_BRK1PINV_Msk             (0x1ul << EPWM_BNF_BRK1PINV_Pos)                   /*!< EPWM_T::BNF: BRK1PINV Mask              */

#define EPWM_BNF_BK0SRC_Pos               (16)                                              /*!< EPWM_T::BNF: BK0SRC Position            */
#define EPWM_BNF_BK0SRC_Msk               (0x1ul << EPWM_BNF_BK0SRC_Pos)                     /*!< EPWM_T::BNF: BK0SRC Mask                */

#define EPWM_BNF_BK1SRC_Pos               (24)                                              /*!< EPWM_T::BNF: BK1SRC Position            */
#define EPWM_BNF_BK1SRC_Msk               (0x1ul << EPWM_BNF_BK1SRC_Pos)                     /*!< EPWM_T::BNF: BK1SRC Mask                */

#define EPWM_FAILBRK_CSSBRKEN_Pos         (0)                                               /*!< EPWM_T::FAILBRK: CSSBRKEN Position      */
#define EPWM_FAILBRK_CSSBRKEN_Msk         (0x1ul << EPWM_FAILBRK_CSSBRKEN_Pos)               /*!< EPWM_T::FAILBRK: CSSBRKEN Mask          */

#define EPWM_FAILBRK_BODBRKEN_Pos         (1)                                               /*!< EPWM_T::FAILBRK: BODBRKEN Position      */
#define EPWM_FAILBRK_BODBRKEN_Msk         (0x1ul << EPWM_FAILBRK_BODBRKEN_Pos)               /*!< EPWM_T::FAILBRK: BODBRKEN Mask          */

#define EPWM_FAILBRK_RAMBRKEN_Pos         (2)                                               /*!< EPWM_T::FAILBRK: RAMBRKEN Position      */
#define EPWM_FAILBRK_RAMBRKEN_Msk         (0x1ul << EPWM_FAILBRK_RAMBRKEN_Pos)               /*!< EPWM_T::FAILBRK: RAMBRKEN Mask          */

#define EPWM_FAILBRK_CORBRKEN_Pos         (3)                                               /*!< EPWM_T::FAILBRK: CORBRKEN Position      */
#define EPWM_FAILBRK_CORBRKEN_Msk         (0x1ul << EPWM_FAILBRK_CORBRKEN_Pos)               /*!< EPWM_T::FAILBRK: CORBRKEN Mask          */

#define EPWM_BRKCTL0_1_CPO0EBEN_Pos       (0)                                               /*!< EPWM_T::BRKCTL0_1: CPO0EBEN Position    */
#define EPWM_BRKCTL0_1_CPO0EBEN_Msk       (0x1ul << EPWM_BRKCTL0_1_CPO0EBEN_Pos)             /*!< EPWM_T::BRKCTL0_1: CPO0EBEN Mask        */

#define EPWM_BRKCTL0_1_CPO1EBEN_Pos       (1)                                               /*!< EPWM_T::BRKCTL0_1: CPO1EBEN Position    */
#define EPWM_BRKCTL0_1_CPO1EBEN_Msk       (0x1ul << EPWM_BRKCTL0_1_CPO1EBEN_Pos)             /*!< EPWM_T::BRKCTL0_1: CPO1EBEN Mask        */

#define EPWM_BRKCTL0_1_BRKP0EEN_Pos       (4)                                               /*!< EPWM_T::BRKCTL0_1: BRKP0EEN Position    */
#define EPWM_BRKCTL0_1_BRKP0EEN_Msk       (0x1ul << EPWM_BRKCTL0_1_BRKP0EEN_Pos)             /*!< EPWM_T::BRKCTL0_1: BRKP0EEN Mask        */

#define EPWM_BRKCTL0_1_BRKP1EEN_Pos       (5)                                               /*!< EPWM_T::BRKCTL0_1: BRKP1EEN Position    */
#define EPWM_BRKCTL0_1_BRKP1EEN_Msk       (0x1ul << EPWM_BRKCTL0_1_BRKP1EEN_Pos)             /*!< EPWM_T::BRKCTL0_1: BRKP1EEN Mask        */

#define EPWM_BRKCTL0_1_SYSEBEN_Pos        (7)                                               /*!< EPWM_T::BRKCTL0_1: SYSEBEN Position     */
#define EPWM_BRKCTL0_1_SYSEBEN_Msk        (0x1ul << EPWM_BRKCTL0_1_SYSEBEN_Pos)              /*!< EPWM_T::BRKCTL0_1: SYSEBEN Mask         */

#define EPWM_BRKCTL0_1_CPO0LBEN_Pos       (8)                                               /*!< EPWM_T::BRKCTL0_1: CPO0LBEN Position    */
#define EPWM_BRKCTL0_1_CPO0LBEN_Msk       (0x1ul << EPWM_BRKCTL0_1_CPO0LBEN_Pos)             /*!< EPWM_T::BRKCTL0_1: CPO0LBEN Mask        */

#define EPWM_BRKCTL0_1_CPO1LBEN_Pos       (9)                                               /*!< EPWM_T::BRKCTL0_1: CPO1LBEN Position    */
#define EPWM_BRKCTL0_1_CPO1LBEN_Msk       (0x1ul << EPWM_BRKCTL0_1_CPO1LBEN_Pos)             /*!< EPWM_T::BRKCTL0_1: CPO1LBEN Mask        */

#define EPWM_BRKCTL0_1_BRKP0LEN_Pos       (12)                                              /*!< EPWM_T::BRKCTL0_1: BRKP0LEN Position    */
#define EPWM_BRKCTL0_1_BRKP0LEN_Msk       (0x1ul << EPWM_BRKCTL0_1_BRKP0LEN_Pos)             /*!< EPWM_T::BRKCTL0_1: BRKP0LEN Mask        */

#define EPWM_BRKCTL0_1_BRKP1LEN_Pos       (13)                                              /*!< EPWM_T::BRKCTL0_1: BRKP1LEN Position    */
#define EPWM_BRKCTL0_1_BRKP1LEN_Msk       (0x1ul << EPWM_BRKCTL0_1_BRKP1LEN_Pos)             /*!< EPWM_T::BRKCTL0_1: BRKP1LEN Mask        */

#define EPWM_BRKCTL0_1_SYSLBEN_Pos        (15)                                              /*!< EPWM_T::BRKCTL0_1: SYSLBEN Position     */
#define EPWM_BRKCTL0_1_SYSLBEN_Msk        (0x1ul << EPWM_BRKCTL0_1_SYSLBEN_Pos)              /*!< EPWM_T::BRKCTL0_1: SYSLBEN Mask         */

#define EPWM_BRKCTL0_1_BRKAEVEN_Pos       (16)                                              /*!< EPWM_T::BRKCTL0_1: BRKAEVEN Position    */
#define EPWM_BRKCTL0_1_BRKAEVEN_Msk       (0x3ul << EPWM_BRKCTL0_1_BRKAEVEN_Pos)             /*!< EPWM_T::BRKCTL0_1: BRKAEVEN Mask        */

#define EPWM_BRKCTL0_1_BRKAODD_Pos        (18)                                              /*!< EPWM_T::BRKCTL0_1: BRKAODD Position     */
#define EPWM_BRKCTL0_1_BRKAODD_Msk        (0x3ul << EPWM_BRKCTL0_1_BRKAODD_Pos)              /*!< EPWM_T::BRKCTL0_1: BRKAODD Mask         */

#define EPWM_BRKCTL0_1_EADCEBEN_Pos       (20)                                              /*!< EPWM_T::BRKCTL0_1: EADCEBEN Position    */
#define EPWM_BRKCTL0_1_EADCEBEN_Msk       (0x1ul << EPWM_BRKCTL0_1_EADCEBEN_Pos)             /*!< EPWM_T::BRKCTL0_1: EADCEBEN Mask        */

#define EPWM_BRKCTL0_1_EADCLBEN_Pos       (28)                                              /*!< EPWM_T::BRKCTL0_1: EADCLBEN Position    */
#define EPWM_BRKCTL0_1_EADCLBEN_Msk       (0x1ul << EPWM_BRKCTL0_1_EADCLBEN_Pos)             /*!< EPWM_T::BRKCTL0_1: EADCLBEN Mask        */

#define EPWM_BRKCTL2_3_CPO0EBEN_Pos       (0)                                               /*!< EPWM_T::BRKCTL2_3: CPO0EBEN Position    */
#define EPWM_BRKCTL2_3_CPO0EBEN_Msk       (0x1ul << EPWM_BRKCTL2_3_CPO0EBEN_Pos)             /*!< EPWM_T::BRKCTL2_3: CPO0EBEN Mask        */

#define EPWM_BRKCTL2_3_CPO1EBEN_Pos       (1)                                               /*!< EPWM_T::BRKCTL2_3: CPO1EBEN Position    */
#define EPWM_BRKCTL2_3_CPO1EBEN_Msk       (0x1ul << EPWM_BRKCTL2_3_CPO1EBEN_Pos)             /*!< EPWM_T::BRKCTL2_3: CPO1EBEN Mask        */

#define EPWM_BRKCTL2_3_BRKP0EEN_Pos       (4)                                               /*!< EPWM_T::BRKCTL2_3: BRKP0EEN Position    */
#define EPWM_BRKCTL2_3_BRKP0EEN_Msk       (0x1ul << EPWM_BRKCTL2_3_BRKP0EEN_Pos)             /*!< EPWM_T::BRKCTL2_3: BRKP0EEN Mask        */

#define EPWM_BRKCTL2_3_BRKP1EEN_Pos       (5)                                               /*!< EPWM_T::BRKCTL2_3: BRKP1EEN Position    */
#define EPWM_BRKCTL2_3_BRKP1EEN_Msk       (0x1ul << EPWM_BRKCTL2_3_BRKP1EEN_Pos)             /*!< EPWM_T::BRKCTL2_3: BRKP1EEN Mask        */

#define EPWM_BRKCTL2_3_SYSEBEN_Pos        (7)                                               /*!< EPWM_T::BRKCTL2_3: SYSEBEN Position     */
#define EPWM_BRKCTL2_3_SYSEBEN_Msk        (0x1ul << EPWM_BRKCTL2_3_SYSEBEN_Pos)              /*!< EPWM_T::BRKCTL2_3: SYSEBEN Mask         */

#define EPWM_BRKCTL2_3_CPO0LBEN_Pos       (8)                                               /*!< EPWM_T::BRKCTL2_3: CPO0LBEN Position    */
#define EPWM_BRKCTL2_3_CPO0LBEN_Msk       (0x1ul << EPWM_BRKCTL2_3_CPO0LBEN_Pos)             /*!< EPWM_T::BRKCTL2_3: CPO0LBEN Mask        */

#define EPWM_BRKCTL2_3_CPO1LBEN_Pos       (9)                                               /*!< EPWM_T::BRKCTL2_3: CPO1LBEN Position    */
#define EPWM_BRKCTL2_3_CPO1LBEN_Msk       (0x1ul << EPWM_BRKCTL2_3_CPO1LBEN_Pos)             /*!< EPWM_T::BRKCTL2_3: CPO1LBEN Mask        */

#define EPWM_BRKCTL2_3_BRKP0LEN_Pos       (12)                                              /*!< EPWM_T::BRKCTL2_3: BRKP0LEN Position    */
#define EPWM_BRKCTL2_3_BRKP0LEN_Msk       (0x1ul << EPWM_BRKCTL2_3_BRKP0LEN_Pos)             /*!< EPWM_T::BRKCTL2_3: BRKP0LEN Mask        */

#define EPWM_BRKCTL2_3_BRKP1LEN_Pos       (13)                                              /*!< EPWM_T::BRKCTL2_3: BRKP1LEN Position    */
#define EPWM_BRKCTL2_3_BRKP1LEN_Msk       (0x1ul << EPWM_BRKCTL2_3_BRKP1LEN_Pos)             /*!< EPWM_T::BRKCTL2_3: BRKP1LEN Mask        */

#define EPWM_BRKCTL2_3_SYSLBEN_Pos        (15)                                              /*!< EPWM_T::BRKCTL2_3: SYSLBEN Position     */
#define EPWM_BRKCTL2_3_SYSLBEN_Msk        (0x1ul << EPWM_BRKCTL2_3_SYSLBEN_Pos)              /*!< EPWM_T::BRKCTL2_3: SYSLBEN Mask         */

#define EPWM_BRKCTL2_3_BRKAEVEN_Pos       (16)                                              /*!< EPWM_T::BRKCTL2_3: BRKAEVEN Position    */
#define EPWM_BRKCTL2_3_BRKAEVEN_Msk       (0x3ul << EPWM_BRKCTL2_3_BRKAEVEN_Pos)             /*!< EPWM_T::BRKCTL2_3: BRKAEVEN Mask        */

#define EPWM_BRKCTL2_3_BRKAODD_Pos        (18)                                              /*!< EPWM_T::BRKCTL2_3: BRKAODD Position     */
#define EPWM_BRKCTL2_3_BRKAODD_Msk        (0x3ul << EPWM_BRKCTL2_3_BRKAODD_Pos)              /*!< EPWM_T::BRKCTL2_3: BRKAODD Mask         */

#define EPWM_BRKCTL2_3_EADCEBEN_Pos       (20)                                              /*!< EPWM_T::BRKCTL2_3: EADCEBEN Position    */
#define EPWM_BRKCTL2_3_EADCEBEN_Msk       (0x1ul << EPWM_BRKCTL2_3_EADCEBEN_Pos)             /*!< EPWM_T::BRKCTL2_3: EADCEBEN Mask        */

#define EPWM_BRKCTL2_3_EADCLBEN_Pos       (28)                                              /*!< EPWM_T::BRKCTL2_3: EADCLBEN Position    */
#define EPWM_BRKCTL2_3_EADCLBEN_Msk       (0x1ul << EPWM_BRKCTL2_3_EADCLBEN_Pos)             /*!< EPWM_T::BRKCTL2_3: EADCLBEN Mask        */

#define EPWM_BRKCTL4_5_CPO0EBEN_Pos       (0)                                               /*!< EPWM_T::BRKCTL4_5: CPO0EBEN Position    */
#define EPWM_BRKCTL4_5_CPO0EBEN_Msk       (0x1ul << EPWM_BRKCTL4_5_CPO0EBEN_Pos)             /*!< EPWM_T::BRKCTL4_5: CPO0EBEN Mask        */

#define EPWM_BRKCTL4_5_CPO1EBEN_Pos       (1)                                               /*!< EPWM_T::BRKCTL4_5: CPO1EBEN Position    */
#define EPWM_BRKCTL4_5_CPO1EBEN_Msk       (0x1ul << EPWM_BRKCTL4_5_CPO1EBEN_Pos)             /*!< EPWM_T::BRKCTL4_5: CPO1EBEN Mask        */

#define EPWM_BRKCTL4_5_BRKP0EEN_Pos       (4)                                               /*!< EPWM_T::BRKCTL4_5: BRKP0EEN Position    */
#define EPWM_BRKCTL4_5_BRKP0EEN_Msk       (0x1ul << EPWM_BRKCTL4_5_BRKP0EEN_Pos)             /*!< EPWM_T::BRKCTL4_5: BRKP0EEN Mask        */

#define EPWM_BRKCTL4_5_BRKP1EEN_Pos       (5)                                               /*!< EPWM_T::BRKCTL4_5: BRKP1EEN Position    */
#define EPWM_BRKCTL4_5_BRKP1EEN_Msk       (0x1ul << EPWM_BRKCTL4_5_BRKP1EEN_Pos)             /*!< EPWM_T::BRKCTL4_5: BRKP1EEN Mask        */

#define EPWM_BRKCTL4_5_SYSEBEN_Pos        (7)                                               /*!< EPWM_T::BRKCTL4_5: SYSEBEN Position     */
#define EPWM_BRKCTL4_5_SYSEBEN_Msk        (0x1ul << EPWM_BRKCTL4_5_SYSEBEN_Pos)              /*!< EPWM_T::BRKCTL4_5: SYSEBEN Mask         */

#define EPWM_BRKCTL4_5_CPO0LBEN_Pos       (8)                                               /*!< EPWM_T::BRKCTL4_5: CPO0LBEN Position    */
#define EPWM_BRKCTL4_5_CPO0LBEN_Msk       (0x1ul << EPWM_BRKCTL4_5_CPO0LBEN_Pos)             /*!< EPWM_T::BRKCTL4_5: CPO0LBEN Mask        */

#define EPWM_BRKCTL4_5_CPO1LBEN_Pos       (9)                                               /*!< EPWM_T::BRKCTL4_5: CPO1LBEN Position    */
#define EPWM_BRKCTL4_5_CPO1LBEN_Msk       (0x1ul << EPWM_BRKCTL4_5_CPO1LBEN_Pos)             /*!< EPWM_T::BRKCTL4_5: CPO1LBEN Mask        */

#define EPWM_BRKCTL4_5_BRKP0LEN_Pos       (12)                                              /*!< EPWM_T::BRKCTL4_5: BRKP0LEN Position    */
#define EPWM_BRKCTL4_5_BRKP0LEN_Msk       (0x1ul << EPWM_BRKCTL4_5_BRKP0LEN_Pos)             /*!< EPWM_T::BRKCTL4_5: BRKP0LEN Mask        */

#define EPWM_BRKCTL4_5_BRKP1LEN_Pos       (13)                                              /*!< EPWM_T::BRKCTL4_5: BRKP1LEN Position    */
#define EPWM_BRKCTL4_5_BRKP1LEN_Msk       (0x1ul << EPWM_BRKCTL4_5_BRKP1LEN_Pos)             /*!< EPWM_T::BRKCTL4_5: BRKP1LEN Mask        */

#define EPWM_BRKCTL4_5_SYSLBEN_Pos        (15)                                              /*!< EPWM_T::BRKCTL4_5: SYSLBEN Position     */
#define EPWM_BRKCTL4_5_SYSLBEN_Msk        (0x1ul << EPWM_BRKCTL4_5_SYSLBEN_Pos)              /*!< EPWM_T::BRKCTL4_5: SYSLBEN Mask         */

#define EPWM_BRKCTL4_5_BRKAEVEN_Pos       (16)                                              /*!< EPWM_T::BRKCTL4_5: BRKAEVEN Position    */
#define EPWM_BRKCTL4_5_BRKAEVEN_Msk       (0x3ul << EPWM_BRKCTL4_5_BRKAEVEN_Pos)             /*!< EPWM_T::BRKCTL4_5: BRKAEVEN Mask        */

#define EPWM_BRKCTL4_5_BRKAODD_Pos        (18)                                              /*!< EPWM_T::BRKCTL4_5: BRKAODD Position     */
#define EPWM_BRKCTL4_5_BRKAODD_Msk        (0x3ul << EPWM_BRKCTL4_5_BRKAODD_Pos)              /*!< EPWM_T::BRKCTL4_5: BRKAODD Mask         */

#define EPWM_BRKCTL4_5_EADCEBEN_Pos       (20)                                              /*!< EPWM_T::BRKCTL4_5: EADCEBEN Position    */
#define EPWM_BRKCTL4_5_EADCEBEN_Msk       (0x1ul << EPWM_BRKCTL4_5_EADCEBEN_Pos)             /*!< EPWM_T::BRKCTL4_5: EADCEBEN Mask        */

#define EPWM_BRKCTL4_5_EADCLBEN_Pos       (28)                                              /*!< EPWM_T::BRKCTL4_5: EADCLBEN Position    */
#define EPWM_BRKCTL4_5_EADCLBEN_Msk       (0x1ul << EPWM_BRKCTL4_5_EADCLBEN_Pos)             /*!< EPWM_T::BRKCTL4_5: EADCLBEN Mask        */

#define EPWM_POLCTL_PINV0_Pos             (0)                                               /*!< EPWM_T::POLCTL: PINV0 Position          */
#define EPWM_POLCTL_PINV0_Msk             (0x1ul << EPWM_POLCTL_PINV0_Pos)                   /*!< EPWM_T::POLCTL: PINV0 Mask              */

#define EPWM_POLCTL_PINV1_Pos             (1)                                               /*!< EPWM_T::POLCTL: PINV1 Position          */
#define EPWM_POLCTL_PINV1_Msk             (0x1ul << EPWM_POLCTL_PINV1_Pos)                   /*!< EPWM_T::POLCTL: PINV1 Mask              */

#define EPWM_POLCTL_PINV2_Pos             (2)                                               /*!< EPWM_T::POLCTL: PINV2 Position          */
#define EPWM_POLCTL_PINV2_Msk             (0x1ul << EPWM_POLCTL_PINV2_Pos)                   /*!< EPWM_T::POLCTL: PINV2 Mask              */

#define EPWM_POLCTL_PINV3_Pos             (3)                                               /*!< EPWM_T::POLCTL: PINV3 Position          */
#define EPWM_POLCTL_PINV3_Msk             (0x1ul << EPWM_POLCTL_PINV3_Pos)                   /*!< EPWM_T::POLCTL: PINV3 Mask              */

#define EPWM_POLCTL_PINV4_Pos             (4)                                               /*!< EPWM_T::POLCTL: PINV4 Position          */
#define EPWM_POLCTL_PINV4_Msk             (0x1ul << EPWM_POLCTL_PINV4_Pos)                   /*!< EPWM_T::POLCTL: PINV4 Mask              */

#define EPWM_POLCTL_PINV5_Pos             (5)                                               /*!< EPWM_T::POLCTL: PINV5 Position          */
#define EPWM_POLCTL_PINV5_Msk             (0x1ul << EPWM_POLCTL_PINV5_Pos)                   /*!< EPWM_T::POLCTL: PINV5 Mask              */

#define EPWM_POEN_POEN0_Pos               (0)                                               /*!< EPWM_T::POEN: POEN0 Position            */
#define EPWM_POEN_POEN0_Msk               (0x1ul << EPWM_POEN_POEN0_Pos)                     /*!< EPWM_T::POEN: POEN0 Mask                */

#define EPWM_POEN_POEN1_Pos               (1)                                               /*!< EPWM_T::POEN: POEN1 Position            */
#define EPWM_POEN_POEN1_Msk               (0x1ul << EPWM_POEN_POEN1_Pos)                     /*!< EPWM_T::POEN: POEN1 Mask                */

#define EPWM_POEN_POEN2_Pos               (2)                                               /*!< EPWM_T::POEN: POEN2 Position            */
#define EPWM_POEN_POEN2_Msk               (0x1ul << EPWM_POEN_POEN2_Pos)                     /*!< EPWM_T::POEN: POEN2 Mask                */

#define EPWM_POEN_POEN3_Pos               (3)                                               /*!< EPWM_T::POEN: POEN3 Position            */
#define EPWM_POEN_POEN3_Msk               (0x1ul << EPWM_POEN_POEN3_Pos)                     /*!< EPWM_T::POEN: POEN3 Mask                */

#define EPWM_POEN_POEN4_Pos               (4)                                               /*!< EPWM_T::POEN: POEN4 Position            */
#define EPWM_POEN_POEN4_Msk               (0x1ul << EPWM_POEN_POEN4_Pos)                     /*!< EPWM_T::POEN: POEN4 Mask                */

#define EPWM_POEN_POEN5_Pos               (5)                                               /*!< EPWM_T::POEN: POEN5 Position            */
#define EPWM_POEN_POEN5_Msk               (0x1ul << EPWM_POEN_POEN5_Pos)                     /*!< EPWM_T::POEN: POEN5 Mask                */

#define EPWM_SWBRK_BRKETRG0_Pos           (0)                                               /*!< EPWM_T::SWBRK: BRKETRG0 Position        */
#define EPWM_SWBRK_BRKETRG0_Msk           (0x1ul << EPWM_SWBRK_BRKETRG0_Pos)                 /*!< EPWM_T::SWBRK: BRKETRG0 Mask            */

#define EPWM_SWBRK_BRKETRG2_Pos           (1)                                               /*!< EPWM_T::SWBRK: BRKETRG2 Position        */
#define EPWM_SWBRK_BRKETRG2_Msk           (0x1ul << EPWM_SWBRK_BRKETRG2_Pos)                 /*!< EPWM_T::SWBRK: BRKETRG2 Mask            */

#define EPWM_SWBRK_BRKETRG4_Pos           (2)                                               /*!< EPWM_T::SWBRK: BRKETRG4 Position        */
#define EPWM_SWBRK_BRKETRG4_Msk           (0x1ul << EPWM_SWBRK_BRKETRG4_Pos)                 /*!< EPWM_T::SWBRK: BRKETRG4 Mask            */

#define EPWM_SWBRK_BRKLTRG0_Pos           (8)                                               /*!< EPWM_T::SWBRK: BRKLTRG0 Position        */
#define EPWM_SWBRK_BRKLTRG0_Msk           (0x1ul << EPWM_SWBRK_BRKLTRG0_Pos)                 /*!< EPWM_T::SWBRK: BRKLTRG0 Mask            */

#define EPWM_SWBRK_BRKLTRG2_Pos           (9)                                               /*!< EPWM_T::SWBRK: BRKLTRG2 Position        */
#define EPWM_SWBRK_BRKLTRG2_Msk           (0x1ul << EPWM_SWBRK_BRKLTRG2_Pos)                 /*!< EPWM_T::SWBRK: BRKLTRG2 Mask            */

#define EPWM_SWBRK_BRKLTRG4_Pos           (10)                                              /*!< EPWM_T::SWBRK: BRKLTRG4 Position        */
#define EPWM_SWBRK_BRKLTRG4_Msk           (0x1ul << EPWM_SWBRK_BRKLTRG4_Pos)                 /*!< EPWM_T::SWBRK: BRKLTRG4 Mask            */

#define EPWM_INTEN0_ZIEN0_Pos             (0)                                               /*!< EPWM_T::INTEN0: ZIEN0 Position          */
#define EPWM_INTEN0_ZIEN0_Msk             (0x1ul << EPWM_INTEN0_ZIEN0_Pos)                   /*!< EPWM_T::INTEN0: ZIEN0 Mask              */

#define EPWM_INTEN0_ZIEN1_Pos             (1)                                               /*!< EPWM_T::INTEN0: ZIEN1 Position          */
#define EPWM_INTEN0_ZIEN1_Msk             (0x1ul << EPWM_INTEN0_ZIEN1_Pos)                   /*!< EPWM_T::INTEN0: ZIEN1 Mask              */

#define EPWM_INTEN0_ZIEN2_Pos             (2)                                               /*!< EPWM_T::INTEN0: ZIEN2 Position          */
#define EPWM_INTEN0_ZIEN2_Msk             (0x1ul << EPWM_INTEN0_ZIEN2_Pos)                   /*!< EPWM_T::INTEN0: ZIEN2 Mask              */

#define EPWM_INTEN0_ZIEN3_Pos             (3)                                               /*!< EPWM_T::INTEN0: ZIEN3 Position          */
#define EPWM_INTEN0_ZIEN3_Msk             (0x1ul << EPWM_INTEN0_ZIEN3_Pos)                   /*!< EPWM_T::INTEN0: ZIEN3 Mask              */

#define EPWM_INTEN0_ZIEN4_Pos             (4)                                               /*!< EPWM_T::INTEN0: ZIEN4 Position          */
#define EPWM_INTEN0_ZIEN4_Msk             (0x1ul << EPWM_INTEN0_ZIEN4_Pos)                   /*!< EPWM_T::INTEN0: ZIEN4 Mask              */

#define EPWM_INTEN0_ZIEN5_Pos             (5)                                               /*!< EPWM_T::INTEN0: ZIEN5 Position          */
#define EPWM_INTEN0_ZIEN5_Msk             (0x1ul << EPWM_INTEN0_ZIEN5_Pos)                   /*!< EPWM_T::INTEN0: ZIEN5 Mask              */

#define EPWM_INTEN0_PIEN0_Pos             (8)                                               /*!< EPWM_T::INTEN0: PIEN0 Position          */
#define EPWM_INTEN0_PIEN0_Msk             (0x1ul << EPWM_INTEN0_PIEN0_Pos)                   /*!< EPWM_T::INTEN0: PIEN0 Mask              */

#define EPWM_INTEN0_PIEN1_Pos             (9)                                               /*!< EPWM_T::INTEN0: PIEN1 Position          */
#define EPWM_INTEN0_PIEN1_Msk             (0x1ul << EPWM_INTEN0_PIEN1_Pos)                   /*!< EPWM_T::INTEN0: PIEN1 Mask              */

#define EPWM_INTEN0_PIEN2_Pos             (10)                                              /*!< EPWM_T::INTEN0: PIEN2 Position          */
#define EPWM_INTEN0_PIEN2_Msk             (0x1ul << EPWM_INTEN0_PIEN2_Pos)                   /*!< EPWM_T::INTEN0: PIEN2 Mask              */

#define EPWM_INTEN0_PIEN3_Pos             (11)                                              /*!< EPWM_T::INTEN0: PIEN3 Position          */
#define EPWM_INTEN0_PIEN3_Msk             (0x1ul << EPWM_INTEN0_PIEN3_Pos)                   /*!< EPWM_T::INTEN0: PIEN3 Mask              */

#define EPWM_INTEN0_PIEN4_Pos             (12)                                              /*!< EPWM_T::INTEN0: PIEN4 Position          */
#define EPWM_INTEN0_PIEN4_Msk             (0x1ul << EPWM_INTEN0_PIEN4_Pos)                   /*!< EPWM_T::INTEN0: PIEN4 Mask              */

#define EPWM_INTEN0_PIEN5_Pos             (13)                                              /*!< EPWM_T::INTEN0: PIEN5 Position          */
#define EPWM_INTEN0_PIEN5_Msk             (0x1ul << EPWM_INTEN0_PIEN5_Pos)                   /*!< EPWM_T::INTEN0: PIEN5 Mask              */

#define EPWM_INTEN0_CMPUIEN0_Pos          (16)                                              /*!< EPWM_T::INTEN0: CMPUIEN0 Position       */
#define EPWM_INTEN0_CMPUIEN0_Msk          (0x1ul << EPWM_INTEN0_CMPUIEN0_Pos)                /*!< EPWM_T::INTEN0: CMPUIEN0 Mask           */

#define EPWM_INTEN0_CMPUIEN1_Pos          (17)                                              /*!< EPWM_T::INTEN0: CMPUIEN1 Position       */
#define EPWM_INTEN0_CMPUIEN1_Msk          (0x1ul << EPWM_INTEN0_CMPUIEN1_Pos)                /*!< EPWM_T::INTEN0: CMPUIEN1 Mask           */

#define EPWM_INTEN0_CMPUIEN2_Pos          (18)                                              /*!< EPWM_T::INTEN0: CMPUIEN2 Position       */
#define EPWM_INTEN0_CMPUIEN2_Msk          (0x1ul << EPWM_INTEN0_CMPUIEN2_Pos)                /*!< EPWM_T::INTEN0: CMPUIEN2 Mask           */

#define EPWM_INTEN0_CMPUIEN3_Pos          (19)                                              /*!< EPWM_T::INTEN0: CMPUIEN3 Position       */
#define EPWM_INTEN0_CMPUIEN3_Msk          (0x1ul << EPWM_INTEN0_CMPUIEN3_Pos)                /*!< EPWM_T::INTEN0: CMPUIEN3 Mask           */

#define EPWM_INTEN0_CMPUIEN4_Pos          (20)                                              /*!< EPWM_T::INTEN0: CMPUIEN4 Position       */
#define EPWM_INTEN0_CMPUIEN4_Msk          (0x1ul << EPWM_INTEN0_CMPUIEN4_Pos)                /*!< EPWM_T::INTEN0: CMPUIEN4 Mask           */

#define EPWM_INTEN0_CMPUIEN5_Pos          (21)                                              /*!< EPWM_T::INTEN0: CMPUIEN5 Position       */
#define EPWM_INTEN0_CMPUIEN5_Msk          (0x1ul << EPWM_INTEN0_CMPUIEN5_Pos)                /*!< EPWM_T::INTEN0: CMPUIEN5 Mask           */

#define EPWM_INTEN0_CMPDIEN0_Pos          (24)                                              /*!< EPWM_T::INTEN0: CMPDIEN0 Position       */
#define EPWM_INTEN0_CMPDIEN0_Msk          (0x1ul << EPWM_INTEN0_CMPDIEN0_Pos)                /*!< EPWM_T::INTEN0: CMPDIEN0 Mask           */

#define EPWM_INTEN0_CMPDIEN1_Pos          (25)                                              /*!< EPWM_T::INTEN0: CMPDIEN1 Position       */
#define EPWM_INTEN0_CMPDIEN1_Msk          (0x1ul << EPWM_INTEN0_CMPDIEN1_Pos)                /*!< EPWM_T::INTEN0: CMPDIEN1 Mask           */

#define EPWM_INTEN0_CMPDIEN2_Pos          (26)                                              /*!< EPWM_T::INTEN0: CMPDIEN2 Position       */
#define EPWM_INTEN0_CMPDIEN2_Msk          (0x1ul << EPWM_INTEN0_CMPDIEN2_Pos)                /*!< EPWM_T::INTEN0: CMPDIEN2 Mask           */

#define EPWM_INTEN0_CMPDIEN3_Pos          (27)                                              /*!< EPWM_T::INTEN0: CMPDIEN3 Position       */
#define EPWM_INTEN0_CMPDIEN3_Msk          (0x1ul << EPWM_INTEN0_CMPDIEN3_Pos)                /*!< EPWM_T::INTEN0: CMPDIEN3 Mask           */

#define EPWM_INTEN0_CMPDIEN4_Pos          (28)                                              /*!< EPWM_T::INTEN0: CMPDIEN4 Position       */
#define EPWM_INTEN0_CMPDIEN4_Msk          (0x1ul << EPWM_INTEN0_CMPDIEN4_Pos)                /*!< EPWM_T::INTEN0: CMPDIEN4 Mask           */

#define EPWM_INTEN0_CMPDIEN5_Pos          (29)                                              /*!< EPWM_T::INTEN0: CMPDIEN5 Position       */
#define EPWM_INTEN0_CMPDIEN5_Msk          (0x1ul << EPWM_INTEN0_CMPDIEN5_Pos)                /*!< EPWM_T::INTEN0: CMPDIEN5 Mask           */

#define EPWM_INTEN1_BRKEIEN0_1_Pos        (0)                                               /*!< EPWM_T::INTEN1: BRKEIEN0_1 Position     */
#define EPWM_INTEN1_BRKEIEN0_1_Msk        (0x1ul << EPWM_INTEN1_BRKEIEN0_1_Pos)              /*!< EPWM_T::INTEN1: BRKEIEN0_1 Mask         */

#define EPWM_INTEN1_BRKEIEN2_3_Pos        (1)                                               /*!< EPWM_T::INTEN1: BRKEIEN2_3 Position     */
#define EPWM_INTEN1_BRKEIEN2_3_Msk        (0x1ul << EPWM_INTEN1_BRKEIEN2_3_Pos)              /*!< EPWM_T::INTEN1: BRKEIEN2_3 Mask         */

#define EPWM_INTEN1_BRKEIEN4_5_Pos        (2)                                               /*!< EPWM_T::INTEN1: BRKEIEN4_5 Position     */
#define EPWM_INTEN1_BRKEIEN4_5_Msk        (0x1ul << EPWM_INTEN1_BRKEIEN4_5_Pos)              /*!< EPWM_T::INTEN1: BRKEIEN4_5 Mask         */

#define EPWM_INTEN1_BRKLIEN0_1_Pos        (8)                                               /*!< EPWM_T::INTEN1: BRKLIEN0_1 Position     */
#define EPWM_INTEN1_BRKLIEN0_1_Msk        (0x1ul << EPWM_INTEN1_BRKLIEN0_1_Pos)              /*!< EPWM_T::INTEN1: BRKLIEN0_1 Mask         */

#define EPWM_INTEN1_BRKLIEN2_3_Pos        (9)                                               /*!< EPWM_T::INTEN1: BRKLIEN2_3 Position     */
#define EPWM_INTEN1_BRKLIEN2_3_Msk        (0x1ul << EPWM_INTEN1_BRKLIEN2_3_Pos)              /*!< EPWM_T::INTEN1: BRKLIEN2_3 Mask         */

#define EPWM_INTEN1_BRKLIEN4_5_Pos        (10)                                              /*!< EPWM_T::INTEN1: BRKLIEN4_5 Position     */
#define EPWM_INTEN1_BRKLIEN4_5_Msk        (0x1ul << EPWM_INTEN1_BRKLIEN4_5_Pos)              /*!< EPWM_T::INTEN1: BRKLIEN4_5 Mask         */

#define EPWM_INTSTS0_ZIF0_Pos             (0)                                               /*!< EPWM_T::INTSTS0: ZIF0 Position          */
#define EPWM_INTSTS0_ZIF0_Msk             (0x1ul << EPWM_INTSTS0_ZIF0_Pos)                   /*!< EPWM_T::INTSTS0: ZIF0 Mask              */

#define EPWM_INTSTS0_ZIF1_Pos             (1)                                               /*!< EPWM_T::INTSTS0: ZIF1 Position          */
#define EPWM_INTSTS0_ZIF1_Msk             (0x1ul << EPWM_INTSTS0_ZIF1_Pos)                   /*!< EPWM_T::INTSTS0: ZIF1 Mask              */

#define EPWM_INTSTS0_ZIF2_Pos             (2)                                               /*!< EPWM_T::INTSTS0: ZIF2 Position          */
#define EPWM_INTSTS0_ZIF2_Msk             (0x1ul << EPWM_INTSTS0_ZIF2_Pos)                   /*!< EPWM_T::INTSTS0: ZIF2 Mask              */

#define EPWM_INTSTS0_ZIF3_Pos             (3)                                               /*!< EPWM_T::INTSTS0: ZIF3 Position          */
#define EPWM_INTSTS0_ZIF3_Msk             (0x1ul << EPWM_INTSTS0_ZIF3_Pos)                   /*!< EPWM_T::INTSTS0: ZIF3 Mask              */

#define EPWM_INTSTS0_ZIF4_Pos             (4)                                               /*!< EPWM_T::INTSTS0: ZIF4 Position          */
#define EPWM_INTSTS0_ZIF4_Msk             (0x1ul << EPWM_INTSTS0_ZIF4_Pos)                   /*!< EPWM_T::INTSTS0: ZIF4 Mask              */

#define EPWM_INTSTS0_ZIF5_Pos             (5)                                               /*!< EPWM_T::INTSTS0: ZIF5 Position          */
#define EPWM_INTSTS0_ZIF5_Msk             (0x1ul << EPWM_INTSTS0_ZIF5_Pos)                   /*!< EPWM_T::INTSTS0: ZIF5 Mask              */

#define EPWM_INTSTS0_PIF0_Pos             (8)                                               /*!< EPWM_T::INTSTS0: PIF0 Position          */
#define EPWM_INTSTS0_PIF0_Msk             (0x1ul << EPWM_INTSTS0_PIF0_Pos)                   /*!< EPWM_T::INTSTS0: PIF0 Mask              */

#define EPWM_INTSTS0_PIF1_Pos             (9)                                               /*!< EPWM_T::INTSTS0: PIF1 Position          */
#define EPWM_INTSTS0_PIF1_Msk             (0x1ul << EPWM_INTSTS0_PIF1_Pos)                   /*!< EPWM_T::INTSTS0: PIF1 Mask              */

#define EPWM_INTSTS0_PIF2_Pos             (10)                                              /*!< EPWM_T::INTSTS0: PIF2 Position          */
#define EPWM_INTSTS0_PIF2_Msk             (0x1ul << EPWM_INTSTS0_PIF2_Pos)                   /*!< EPWM_T::INTSTS0: PIF2 Mask              */

#define EPWM_INTSTS0_PIF3_Pos             (11)                                              /*!< EPWM_T::INTSTS0: PIF3 Position          */
#define EPWM_INTSTS0_PIF3_Msk             (0x1ul << EPWM_INTSTS0_PIF3_Pos)                   /*!< EPWM_T::INTSTS0: PIF3 Mask              */

#define EPWM_INTSTS0_PIF4_Pos             (12)                                              /*!< EPWM_T::INTSTS0: PIF4 Position          */
#define EPWM_INTSTS0_PIF4_Msk             (0x1ul << EPWM_INTSTS0_PIF4_Pos)                   /*!< EPWM_T::INTSTS0: PIF4 Mask              */

#define EPWM_INTSTS0_PIF5_Pos             (13)                                              /*!< EPWM_T::INTSTS0: PIF5 Position          */
#define EPWM_INTSTS0_PIF5_Msk             (0x1ul << EPWM_INTSTS0_PIF5_Pos)                   /*!< EPWM_T::INTSTS0: PIF5 Mask              */

#define EPWM_INTSTS0_CMPUIF0_Pos          (16)                                              /*!< EPWM_T::INTSTS0: CMPUIF0 Position       */
#define EPWM_INTSTS0_CMPUIF0_Msk          (0x1ul << EPWM_INTSTS0_CMPUIF0_Pos)                /*!< EPWM_T::INTSTS0: CMPUIF0 Mask           */

#define EPWM_INTSTS0_CMPUIF1_Pos          (17)                                              /*!< EPWM_T::INTSTS0: CMPUIF1 Position       */
#define EPWM_INTSTS0_CMPUIF1_Msk          (0x1ul << EPWM_INTSTS0_CMPUIF1_Pos)                /*!< EPWM_T::INTSTS0: CMPUIF1 Mask           */

#define EPWM_INTSTS0_CMPUIF2_Pos          (18)                                              /*!< EPWM_T::INTSTS0: CMPUIF2 Position       */
#define EPWM_INTSTS0_CMPUIF2_Msk          (0x1ul << EPWM_INTSTS0_CMPUIF2_Pos)                /*!< EPWM_T::INTSTS0: CMPUIF2 Mask           */

#define EPWM_INTSTS0_CMPUIF3_Pos          (19)                                              /*!< EPWM_T::INTSTS0: CMPUIF3 Position       */
#define EPWM_INTSTS0_CMPUIF3_Msk          (0x1ul << EPWM_INTSTS0_CMPUIF3_Pos)                /*!< EPWM_T::INTSTS0: CMPUIF3 Mask           */

#define EPWM_INTSTS0_CMPUIF4_Pos          (20)                                              /*!< EPWM_T::INTSTS0: CMPUIF4 Position       */
#define EPWM_INTSTS0_CMPUIF4_Msk          (0x1ul << EPWM_INTSTS0_CMPUIF4_Pos)                /*!< EPWM_T::INTSTS0: CMPUIF4 Mask           */

#define EPWM_INTSTS0_CMPUIF5_Pos          (21)                                              /*!< EPWM_T::INTSTS0: CMPUIF5 Position       */
#define EPWM_INTSTS0_CMPUIF5_Msk          (0x1ul << EPWM_INTSTS0_CMPUIF5_Pos)                /*!< EPWM_T::INTSTS0: CMPUIF5 Mask           */

#define EPWM_INTSTS0_CMPDIF0_Pos          (24)                                              /*!< EPWM_T::INTSTS0: CMPDIF0 Position       */
#define EPWM_INTSTS0_CMPDIF0_Msk          (0x1ul << EPWM_INTSTS0_CMPDIF0_Pos)                /*!< EPWM_T::INTSTS0: CMPDIF0 Mask           */

#define EPWM_INTSTS0_CMPDIF1_Pos          (25)                                              /*!< EPWM_T::INTSTS0: CMPDIF1 Position       */
#define EPWM_INTSTS0_CMPDIF1_Msk          (0x1ul << EPWM_INTSTS0_CMPDIF1_Pos)                /*!< EPWM_T::INTSTS0: CMPDIF1 Mask           */

#define EPWM_INTSTS0_CMPDIF2_Pos          (26)                                              /*!< EPWM_T::INTSTS0: CMPDIF2 Position       */
#define EPWM_INTSTS0_CMPDIF2_Msk          (0x1ul << EPWM_INTSTS0_CMPDIF2_Pos)                /*!< EPWM_T::INTSTS0: CMPDIF2 Mask           */

#define EPWM_INTSTS0_CMPDIF3_Pos          (27)                                              /*!< EPWM_T::INTSTS0: CMPDIF3 Position       */
#define EPWM_INTSTS0_CMPDIF3_Msk          (0x1ul << EPWM_INTSTS0_CMPDIF3_Pos)                /*!< EPWM_T::INTSTS0: CMPDIF3 Mask           */

#define EPWM_INTSTS0_CMPDIF4_Pos          (28)                                              /*!< EPWM_T::INTSTS0: CMPDIF4 Position       */
#define EPWM_INTSTS0_CMPDIF4_Msk          (0x1ul << EPWM_INTSTS0_CMPDIF4_Pos)                /*!< EPWM_T::INTSTS0: CMPDIF4 Mask           */

#define EPWM_INTSTS0_CMPDIF5_Pos          (29)                                              /*!< EPWM_T::INTSTS0: CMPDIF5 Position       */
#define EPWM_INTSTS0_CMPDIF5_Msk          (0x1ul << EPWM_INTSTS0_CMPDIF5_Pos)                /*!< EPWM_T::INTSTS0: CMPDIF5 Mask           */

#define EPWM_INTSTS1_BRKEIF0_Pos          (0)                                               /*!< EPWM_T::INTSTS1: BRKEIF0 Position       */
#define EPWM_INTSTS1_BRKEIF0_Msk          (0x1ul << EPWM_INTSTS1_BRKEIF0_Pos)                /*!< EPWM_T::INTSTS1: BRKEIF0 Mask           */

#define EPWM_INTSTS1_BRKEIF1_Pos          (1)                                               /*!< EPWM_T::INTSTS1: BRKEIF1 Position       */
#define EPWM_INTSTS1_BRKEIF1_Msk          (0x1ul << EPWM_INTSTS1_BRKEIF1_Pos)                /*!< EPWM_T::INTSTS1: BRKEIF1 Mask           */

#define EPWM_INTSTS1_BRKEIF2_Pos          (2)                                               /*!< EPWM_T::INTSTS1: BRKEIF2 Position       */
#define EPWM_INTSTS1_BRKEIF2_Msk          (0x1ul << EPWM_INTSTS1_BRKEIF2_Pos)                /*!< EPWM_T::INTSTS1: BRKEIF2 Mask           */

#define EPWM_INTSTS1_BRKEIF3_Pos          (3)                                               /*!< EPWM_T::INTSTS1: BRKEIF3 Position       */
#define EPWM_INTSTS1_BRKEIF3_Msk          (0x1ul << EPWM_INTSTS1_BRKEIF3_Pos)                /*!< EPWM_T::INTSTS1: BRKEIF3 Mask           */

#define EPWM_INTSTS1_BRKEIF4_Pos          (4)                                               /*!< EPWM_T::INTSTS1: BRKEIF4 Position       */
#define EPWM_INTSTS1_BRKEIF4_Msk          (0x1ul << EPWM_INTSTS1_BRKEIF4_Pos)                /*!< EPWM_T::INTSTS1: BRKEIF4 Mask           */

#define EPWM_INTSTS1_BRKEIF5_Pos          (5)                                               /*!< EPWM_T::INTSTS1: BRKEIF5 Position       */
#define EPWM_INTSTS1_BRKEIF5_Msk          (0x1ul << EPWM_INTSTS1_BRKEIF5_Pos)                /*!< EPWM_T::INTSTS1: BRKEIF5 Mask           */

#define EPWM_INTSTS1_BRKLIF0_Pos          (8)                                               /*!< EPWM_T::INTSTS1: BRKLIF0 Position       */
#define EPWM_INTSTS1_BRKLIF0_Msk          (0x1ul << EPWM_INTSTS1_BRKLIF0_Pos)                /*!< EPWM_T::INTSTS1: BRKLIF0 Mask           */

#define EPWM_INTSTS1_BRKLIF1_Pos          (9)                                               /*!< EPWM_T::INTSTS1: BRKLIF1 Position       */
#define EPWM_INTSTS1_BRKLIF1_Msk          (0x1ul << EPWM_INTSTS1_BRKLIF1_Pos)                /*!< EPWM_T::INTSTS1: BRKLIF1 Mask           */

#define EPWM_INTSTS1_BRKLIF2_Pos          (10)                                              /*!< EPWM_T::INTSTS1: BRKLIF2 Position       */
#define EPWM_INTSTS1_BRKLIF2_Msk          (0x1ul << EPWM_INTSTS1_BRKLIF2_Pos)                /*!< EPWM_T::INTSTS1: BRKLIF2 Mask           */

#define EPWM_INTSTS1_BRKLIF3_Pos          (11)                                              /*!< EPWM_T::INTSTS1: BRKLIF3 Position       */
#define EPWM_INTSTS1_BRKLIF3_Msk          (0x1ul << EPWM_INTSTS1_BRKLIF3_Pos)                /*!< EPWM_T::INTSTS1: BRKLIF3 Mask           */

#define EPWM_INTSTS1_BRKLIF4_Pos          (12)                                              /*!< EPWM_T::INTSTS1: BRKLIF4 Position       */
#define EPWM_INTSTS1_BRKLIF4_Msk          (0x1ul << EPWM_INTSTS1_BRKLIF4_Pos)                /*!< EPWM_T::INTSTS1: BRKLIF4 Mask           */

#define EPWM_INTSTS1_BRKLIF5_Pos          (13)                                              /*!< EPWM_T::INTSTS1: BRKLIF5 Position       */
#define EPWM_INTSTS1_BRKLIF5_Msk          (0x1ul << EPWM_INTSTS1_BRKLIF5_Pos)                /*!< EPWM_T::INTSTS1: BRKLIF5 Mask           */

#define EPWM_INTSTS1_BRKESTS0_Pos         (16)                                              /*!< EPWM_T::INTSTS1: BRKESTS0 Position      */
#define EPWM_INTSTS1_BRKESTS0_Msk         (0x1ul << EPWM_INTSTS1_BRKESTS0_Pos)               /*!< EPWM_T::INTSTS1: BRKESTS0 Mask          */

#define EPWM_INTSTS1_BRKESTS1_Pos         (17)                                              /*!< EPWM_T::INTSTS1: BRKESTS1 Position      */
#define EPWM_INTSTS1_BRKESTS1_Msk         (0x1ul << EPWM_INTSTS1_BRKESTS1_Pos)               /*!< EPWM_T::INTSTS1: BRKESTS1 Mask          */

#define EPWM_INTSTS1_BRKESTS2_Pos         (18)                                              /*!< EPWM_T::INTSTS1: BRKESTS2 Position      */
#define EPWM_INTSTS1_BRKESTS2_Msk         (0x1ul << EPWM_INTSTS1_BRKESTS2_Pos)               /*!< EPWM_T::INTSTS1: BRKESTS2 Mask          */

#define EPWM_INTSTS1_BRKESTS3_Pos         (19)                                              /*!< EPWM_T::INTSTS1: BRKESTS3 Position      */
#define EPWM_INTSTS1_BRKESTS3_Msk         (0x1ul << EPWM_INTSTS1_BRKESTS3_Pos)               /*!< EPWM_T::INTSTS1: BRKESTS3 Mask          */

#define EPWM_INTSTS1_BRKESTS4_Pos         (20)                                              /*!< EPWM_T::INTSTS1: BRKESTS4 Position      */
#define EPWM_INTSTS1_BRKESTS4_Msk         (0x1ul << EPWM_INTSTS1_BRKESTS4_Pos)               /*!< EPWM_T::INTSTS1: BRKESTS4 Mask          */

#define EPWM_INTSTS1_BRKESTS5_Pos         (21)                                              /*!< EPWM_T::INTSTS1: BRKESTS5 Position      */
#define EPWM_INTSTS1_BRKESTS5_Msk         (0x1ul << EPWM_INTSTS1_BRKESTS5_Pos)               /*!< EPWM_T::INTSTS1: BRKESTS5 Mask          */

#define EPWM_INTSTS1_BRKLSTS0_Pos         (24)                                              /*!< EPWM_T::INTSTS1: BRKLSTS0 Position      */
#define EPWM_INTSTS1_BRKLSTS0_Msk         (0x1ul << EPWM_INTSTS1_BRKLSTS0_Pos)               /*!< EPWM_T::INTSTS1: BRKLSTS0 Mask          */

#define EPWM_INTSTS1_BRKLSTS1_Pos         (25)                                              /*!< EPWM_T::INTSTS1: BRKLSTS1 Position      */
#define EPWM_INTSTS1_BRKLSTS1_Msk         (0x1ul << EPWM_INTSTS1_BRKLSTS1_Pos)               /*!< EPWM_T::INTSTS1: BRKLSTS1 Mask          */

#define EPWM_INTSTS1_BRKLSTS2_Pos         (26)                                              /*!< EPWM_T::INTSTS1: BRKLSTS2 Position      */
#define EPWM_INTSTS1_BRKLSTS2_Msk         (0x1ul << EPWM_INTSTS1_BRKLSTS2_Pos)               /*!< EPWM_T::INTSTS1: BRKLSTS2 Mask          */

#define EPWM_INTSTS1_BRKLSTS3_Pos         (27)                                              /*!< EPWM_T::INTSTS1: BRKLSTS3 Position      */
#define EPWM_INTSTS1_BRKLSTS3_Msk         (0x1ul << EPWM_INTSTS1_BRKLSTS3_Pos)               /*!< EPWM_T::INTSTS1: BRKLSTS3 Mask          */

#define EPWM_INTSTS1_BRKLSTS4_Pos         (28)                                              /*!< EPWM_T::INTSTS1: BRKLSTS4 Position      */
#define EPWM_INTSTS1_BRKLSTS4_Msk         (0x1ul << EPWM_INTSTS1_BRKLSTS4_Pos)               /*!< EPWM_T::INTSTS1: BRKLSTS4 Mask          */

#define EPWM_INTSTS1_BRKLSTS5_Pos         (29)                                              /*!< EPWM_T::INTSTS1: BRKLSTS5 Position      */
#define EPWM_INTSTS1_BRKLSTS5_Msk         (0x1ul << EPWM_INTSTS1_BRKLSTS5_Pos)               /*!< EPWM_T::INTSTS1: BRKLSTS5 Mask          */

#define EPWM_DACTRGEN_ZTE0_Pos            (0)                                               /*!< EPWM_T::DACTRGEN: ZTE0 Position         */
#define EPWM_DACTRGEN_ZTE0_Msk            (0x1ul << EPWM_DACTRGEN_ZTE0_Pos)                  /*!< EPWM_T::DACTRGEN: ZTE0 Mask             */

#define EPWM_DACTRGEN_ZTE1_Pos            (1)                                               /*!< EPWM_T::DACTRGEN: ZTE1 Position         */
#define EPWM_DACTRGEN_ZTE1_Msk            (0x1ul << EPWM_DACTRGEN_ZTE1_Pos)                  /*!< EPWM_T::DACTRGEN: ZTE1 Mask             */

#define EPWM_DACTRGEN_ZTE2_Pos            (2)                                               /*!< EPWM_T::DACTRGEN: ZTE2 Position         */
#define EPWM_DACTRGEN_ZTE2_Msk            (0x1ul << EPWM_DACTRGEN_ZTE2_Pos)                  /*!< EPWM_T::DACTRGEN: ZTE2 Mask             */

#define EPWM_DACTRGEN_ZTE3_Pos            (3)                                               /*!< EPWM_T::DACTRGEN: ZTE3 Position         */
#define EPWM_DACTRGEN_ZTE3_Msk            (0x1ul << EPWM_DACTRGEN_ZTE3_Pos)                  /*!< EPWM_T::DACTRGEN: ZTE3 Mask             */

#define EPWM_DACTRGEN_ZTE4_Pos            (4)                                               /*!< EPWM_T::DACTRGEN: ZTE4 Position         */
#define EPWM_DACTRGEN_ZTE4_Msk            (0x1ul << EPWM_DACTRGEN_ZTE4_Pos)                  /*!< EPWM_T::DACTRGEN: ZTE4 Mask             */

#define EPWM_DACTRGEN_ZTE5_Pos            (5)                                               /*!< EPWM_T::DACTRGEN: ZTE5 Position         */
#define EPWM_DACTRGEN_ZTE5_Msk            (0x1ul << EPWM_DACTRGEN_ZTE5_Pos)                  /*!< EPWM_T::DACTRGEN: ZTE5 Mask             */

#define EPWM_DACTRGEN_PTE0_Pos            (8)                                               /*!< EPWM_T::DACTRGEN: PTE0 Position         */
#define EPWM_DACTRGEN_PTE0_Msk            (0x1ul << EPWM_DACTRGEN_PTE0_Pos)                  /*!< EPWM_T::DACTRGEN: PTE0 Mask             */

#define EPWM_DACTRGEN_PTE1_Pos            (9)                                               /*!< EPWM_T::DACTRGEN: PTE1 Position         */
#define EPWM_DACTRGEN_PTE1_Msk            (0x1ul << EPWM_DACTRGEN_PTE1_Pos)                  /*!< EPWM_T::DACTRGEN: PTE1 Mask             */

#define EPWM_DACTRGEN_PTE2_Pos            (10)                                              /*!< EPWM_T::DACTRGEN: PTE2 Position         */
#define EPWM_DACTRGEN_PTE2_Msk            (0x1ul << EPWM_DACTRGEN_PTE2_Pos)                  /*!< EPWM_T::DACTRGEN: PTE2 Mask             */

#define EPWM_DACTRGEN_PTE3_Pos            (11)                                              /*!< EPWM_T::DACTRGEN: PTE3 Position         */
#define EPWM_DACTRGEN_PTE3_Msk            (0x1ul << EPWM_DACTRGEN_PTE3_Pos)                  /*!< EPWM_T::DACTRGEN: PTE3 Mask             */

#define EPWM_DACTRGEN_PTE4_Pos            (12)                                              /*!< EPWM_T::DACTRGEN: PTE4 Position         */
#define EPWM_DACTRGEN_PTE4_Msk            (0x1ul << EPWM_DACTRGEN_PTE4_Pos)                  /*!< EPWM_T::DACTRGEN: PTE4 Mask             */

#define EPWM_DACTRGEN_PTE5_Pos            (13)                                              /*!< EPWM_T::DACTRGEN: PTE5 Position         */
#define EPWM_DACTRGEN_PTE5_Msk            (0x1ul << EPWM_DACTRGEN_PTE5_Pos)                  /*!< EPWM_T::DACTRGEN: PTE5 Mask             */

#define EPWM_DACTRGEN_CUTRGE0_Pos         (16)                                              /*!< EPWM_T::DACTRGEN: CUTRGE0 Position      */
#define EPWM_DACTRGEN_CUTRGE0_Msk         (0x1ul << EPWM_DACTRGEN_CUTRGE0_Pos)               /*!< EPWM_T::DACTRGEN: CUTRGE0 Mask          */

#define EPWM_DACTRGEN_CUTRGE1_Pos         (17)                                              /*!< EPWM_T::DACTRGEN: CUTRGE1 Position      */
#define EPWM_DACTRGEN_CUTRGE1_Msk         (0x1ul << EPWM_DACTRGEN_CUTRGE1_Pos)               /*!< EPWM_T::DACTRGEN: CUTRGE1 Mask          */

#define EPWM_DACTRGEN_CUTRGE2_Pos         (18)                                              /*!< EPWM_T::DACTRGEN: CUTRGE2 Position      */
#define EPWM_DACTRGEN_CUTRGE2_Msk         (0x1ul << EPWM_DACTRGEN_CUTRGE2_Pos)               /*!< EPWM_T::DACTRGEN: CUTRGE2 Mask          */

#define EPWM_DACTRGEN_CUTRGE3_Pos         (19)                                              /*!< EPWM_T::DACTRGEN: CUTRGE3 Position      */
#define EPWM_DACTRGEN_CUTRGE3_Msk         (0x1ul << EPWM_DACTRGEN_CUTRGE3_Pos)               /*!< EPWM_T::DACTRGEN: CUTRGE3 Mask          */

#define EPWM_DACTRGEN_CUTRGE4_Pos         (20)                                              /*!< EPWM_T::DACTRGEN: CUTRGE4 Position      */
#define EPWM_DACTRGEN_CUTRGE4_Msk         (0x1ul << EPWM_DACTRGEN_CUTRGE4_Pos)               /*!< EPWM_T::DACTRGEN: CUTRGE4 Mask          */

#define EPWM_DACTRGEN_CUTRGE5_Pos         (21)                                              /*!< EPWM_T::DACTRGEN: CUTRGE5 Position      */
#define EPWM_DACTRGEN_CUTRGE5_Msk         (0x1ul << EPWM_DACTRGEN_CUTRGE5_Pos)               /*!< EPWM_T::DACTRGEN: CUTRGE5 Mask          */

#define EPWM_DACTRGEN_CDTRGE0_Pos         (24)                                              /*!< EPWM_T::DACTRGEN: CDTRGE0 Position      */
#define EPWM_DACTRGEN_CDTRGE0_Msk         (0x1ul << EPWM_DACTRGEN_CDTRGE0_Pos)               /*!< EPWM_T::DACTRGEN: CDTRGE0 Mask          */

#define EPWM_DACTRGEN_CDTRGE1_Pos         (25)                                              /*!< EPWM_T::DACTRGEN: CDTRGE1 Position      */
#define EPWM_DACTRGEN_CDTRGE1_Msk         (0x1ul << EPWM_DACTRGEN_CDTRGE1_Pos)               /*!< EPWM_T::DACTRGEN: CDTRGE1 Mask          */

#define EPWM_DACTRGEN_CDTRGE2_Pos         (26)                                              /*!< EPWM_T::DACTRGEN: CDTRGE2 Position      */
#define EPWM_DACTRGEN_CDTRGE2_Msk         (0x1ul << EPWM_DACTRGEN_CDTRGE2_Pos)               /*!< EPWM_T::DACTRGEN: CDTRGE2 Mask          */

#define EPWM_DACTRGEN_CDTRGE3_Pos         (27)                                              /*!< EPWM_T::DACTRGEN: CDTRGE3 Position      */
#define EPWM_DACTRGEN_CDTRGE3_Msk         (0x1ul << EPWM_DACTRGEN_CDTRGE3_Pos)               /*!< EPWM_T::DACTRGEN: CDTRGE3 Mask          */

#define EPWM_DACTRGEN_CDTRGE4_Pos         (28)                                              /*!< EPWM_T::DACTRGEN: CDTRGE4 Position      */
#define EPWM_DACTRGEN_CDTRGE4_Msk         (0x1ul << EPWM_DACTRGEN_CDTRGE4_Pos)               /*!< EPWM_T::DACTRGEN: CDTRGE4 Mask          */

#define EPWM_DACTRGEN_CDTRGE5_Pos         (29)                                              /*!< EPWM_T::DACTRGEN: CDTRGE5 Position      */
#define EPWM_DACTRGEN_CDTRGE5_Msk         (0x1ul << EPWM_DACTRGEN_CDTRGE5_Pos)               /*!< EPWM_T::DACTRGEN: CDTRGE5 Mask          */

#define EPWM_EADCTS0_TRGSEL0_Pos          (0)                                               /*!< EPWM_T::EADCTS0: TRGSEL0 Position       */
#define EPWM_EADCTS0_TRGSEL0_Msk          (0xful << EPWM_EADCTS0_TRGSEL0_Pos)                /*!< EPWM_T::EADCTS0: TRGSEL0 Mask           */

#define EPWM_EADCTS0_TRGEN0_Pos           (7)                                               /*!< EPWM_T::EADCTS0: TRGEN0 Position        */
#define EPWM_EADCTS0_TRGEN0_Msk           (0x1ul << EPWM_EADCTS0_TRGEN0_Pos)                 /*!< EPWM_T::EADCTS0: TRGEN0 Mask            */

#define EPWM_EADCTS0_TRGSEL1_Pos          (8)                                               /*!< EPWM_T::EADCTS0: TRGSEL1 Position       */
#define EPWM_EADCTS0_TRGSEL1_Msk          (0xful << EPWM_EADCTS0_TRGSEL1_Pos)                /*!< EPWM_T::EADCTS0: TRGSEL1 Mask           */

#define EPWM_EADCTS0_TRGEN1_Pos           (15)                                              /*!< EPWM_T::EADCTS0: TRGEN1 Position        */
#define EPWM_EADCTS0_TRGEN1_Msk           (0x1ul << EPWM_EADCTS0_TRGEN1_Pos)                 /*!< EPWM_T::EADCTS0: TRGEN1 Mask            */

#define EPWM_EADCTS0_TRGSEL2_Pos          (16)                                              /*!< EPWM_T::EADCTS0: TRGSEL2 Position       */
#define EPWM_EADCTS0_TRGSEL2_Msk          (0xful << EPWM_EADCTS0_TRGSEL2_Pos)                /*!< EPWM_T::EADCTS0: TRGSEL2 Mask           */

#define EPWM_EADCTS0_TRGEN2_Pos           (23)                                              /*!< EPWM_T::EADCTS0: TRGEN2 Position        */
#define EPWM_EADCTS0_TRGEN2_Msk           (0x1ul << EPWM_EADCTS0_TRGEN2_Pos)                 /*!< EPWM_T::EADCTS0: TRGEN2 Mask            */

#define EPWM_EADCTS0_TRGSEL3_Pos          (24)                                              /*!< EPWM_T::EADCTS0: TRGSEL3 Position       */
#define EPWM_EADCTS0_TRGSEL3_Msk          (0xful << EPWM_EADCTS0_TRGSEL3_Pos)                /*!< EPWM_T::EADCTS0: TRGSEL3 Mask           */

#define EPWM_EADCTS0_TRGEN3_Pos           (31)                                              /*!< EPWM_T::EADCTS0: TRGEN3 Position        */
#define EPWM_EADCTS0_TRGEN3_Msk           (0x1ul << EPWM_EADCTS0_TRGEN3_Pos)                 /*!< EPWM_T::EADCTS0: TRGEN3 Mask            */

#define EPWM_EADCTS1_TRGSEL4_Pos          (0)                                               /*!< EPWM_T::EADCTS1: TRGSEL4 Position       */
#define EPWM_EADCTS1_TRGSEL4_Msk          (0xful << EPWM_EADCTS1_TRGSEL4_Pos)                /*!< EPWM_T::EADCTS1: TRGSEL4 Mask           */

#define EPWM_EADCTS1_TRGEN4_Pos           (7)                                               /*!< EPWM_T::EADCTS1: TRGEN4 Position        */
#define EPWM_EADCTS1_TRGEN4_Msk           (0x1ul << EPWM_EADCTS1_TRGEN4_Pos)                 /*!< EPWM_T::EADCTS1: TRGEN4 Mask            */

#define EPWM_EADCTS1_TRGSEL5_Pos          (8)                                               /*!< EPWM_T::EADCTS1: TRGSEL5 Position       */
#define EPWM_EADCTS1_TRGSEL5_Msk          (0xful << EPWM_EADCTS1_TRGSEL5_Pos)                /*!< EPWM_T::EADCTS1: TRGSEL5 Mask           */

#define EPWM_EADCTS1_TRGEN5_Pos           (15)                                              /*!< EPWM_T::EADCTS1: TRGEN5 Position        */
#define EPWM_EADCTS1_TRGEN5_Msk           (0x1ul << EPWM_EADCTS1_TRGEN5_Pos)                 /*!< EPWM_T::EADCTS1: TRGEN5 Mask            */

#define EPWM_FTCMPDAT0_1_FTCMP_Pos        (0)                                               /*!< EPWM_T::FTCMPDAT0_1: FTCMP Position     */
#define EPWM_FTCMPDAT0_1_FTCMP_Msk        (0xfffful << EPWM_FTCMPDAT0_1_FTCMP_Pos)           /*!< EPWM_T::FTCMPDAT0_1: FTCMP Mask         */

#define EPWM_FTCMPDAT2_3_FTCMP_Pos        (0)                                               /*!< EPWM_T::FTCMPDAT2_3: FTCMP Position     */
#define EPWM_FTCMPDAT2_3_FTCMP_Msk        (0xfffful << EPWM_FTCMPDAT2_3_FTCMP_Pos)           /*!< EPWM_T::FTCMPDAT2_3: FTCMP Mask         */

#define EPWM_FTCMPDAT4_5_FTCMP_Pos        (0)                                               /*!< EPWM_T::FTCMPDAT4_5: FTCMP Position     */
#define EPWM_FTCMPDAT4_5_FTCMP_Msk        (0xfffful << EPWM_FTCMPDAT4_5_FTCMP_Pos)           /*!< EPWM_T::FTCMPDAT4_5: FTCMP Mask         */

#define EPWM_SSCTL_SSEN0_Pos              (0)                                               /*!< EPWM_T::SSCTL: SSEN0 Position           */
#define EPWM_SSCTL_SSEN0_Msk              (0x1ul << EPWM_SSCTL_SSEN0_Pos)                    /*!< EPWM_T::SSCTL: SSEN0 Mask               */

#define EPWM_SSCTL_SSEN1_Pos              (1)                                               /*!< EPWM_T::SSCTL: SSEN1 Position           */
#define EPWM_SSCTL_SSEN1_Msk              (0x1ul << EPWM_SSCTL_SSEN1_Pos)                    /*!< EPWM_T::SSCTL: SSEN1 Mask               */

#define EPWM_SSCTL_SSEN2_Pos              (2)                                               /*!< EPWM_T::SSCTL: SSEN2 Position           */
#define EPWM_SSCTL_SSEN2_Msk              (0x1ul << EPWM_SSCTL_SSEN2_Pos)                    /*!< EPWM_T::SSCTL: SSEN2 Mask               */

#define EPWM_SSCTL_SSEN3_Pos              (3)                                               /*!< EPWM_T::SSCTL: SSEN3 Position           */
#define EPWM_SSCTL_SSEN3_Msk              (0x1ul << EPWM_SSCTL_SSEN3_Pos)                    /*!< EPWM_T::SSCTL: SSEN3 Mask               */

#define EPWM_SSCTL_SSEN4_Pos              (4)                                               /*!< EPWM_T::SSCTL: SSEN4 Position           */
#define EPWM_SSCTL_SSEN4_Msk              (0x1ul << EPWM_SSCTL_SSEN4_Pos)                    /*!< EPWM_T::SSCTL: SSEN4 Mask               */

#define EPWM_SSCTL_SSEN5_Pos              (5)                                               /*!< EPWM_T::SSCTL: SSEN5 Position           */
#define EPWM_SSCTL_SSEN5_Msk              (0x1ul << EPWM_SSCTL_SSEN5_Pos)                    /*!< EPWM_T::SSCTL: SSEN5 Mask               */

#define EPWM_SSCTL_SSRC_Pos               (8)                                               /*!< EPWM_T::SSCTL: SSRC Position            */
#define EPWM_SSCTL_SSRC_Msk               (0x3ul << EPWM_SSCTL_SSRC_Pos)                     /*!< EPWM_T::SSCTL: SSRC Mask                */

#define EPWM_SSTRG_CNTSEN_Pos             (0)                                               /*!< EPWM_T::SSTRG: CNTSEN Position          */
#define EPWM_SSTRG_CNTSEN_Msk             (0x1ul << EPWM_SSTRG_CNTSEN_Pos)                   /*!< EPWM_T::SSTRG: CNTSEN Mask              */

#define EPWM_LEBCTL_LEBEN_Pos             (0)                                               /*!< EPWM_T::LEBCTL: LEBEN Position          */
#define EPWM_LEBCTL_LEBEN_Msk             (0x1ul << EPWM_LEBCTL_LEBEN_Pos)                   /*!< EPWM_T::LEBCTL: LEBEN Mask              */

#define EPWM_LEBCTL_SRCEN0_Pos            (8)                                               /*!< EPWM_T::LEBCTL: SRCEN0 Position         */
#define EPWM_LEBCTL_SRCEN0_Msk            (0x1ul << EPWM_LEBCTL_SRCEN0_Pos)                  /*!< EPWM_T::LEBCTL: SRCEN0 Mask             */

#define EPWM_LEBCTL_SRCEN2_Pos            (9)                                               /*!< EPWM_T::LEBCTL: SRCEN2 Position         */
#define EPWM_LEBCTL_SRCEN2_Msk            (0x1ul << EPWM_LEBCTL_SRCEN2_Pos)                  /*!< EPWM_T::LEBCTL: SRCEN2 Mask             */

#define EPWM_LEBCTL_SRCEN4_Pos            (10)                                              /*!< EPWM_T::LEBCTL: SRCEN4 Position         */
#define EPWM_LEBCTL_SRCEN4_Msk            (0x1ul << EPWM_LEBCTL_SRCEN4_Pos)                  /*!< EPWM_T::LEBCTL: SRCEN4 Mask             */

#define EPWM_LEBCTL_TRGTYPE_Pos           (16)                                              /*!< EPWM_T::LEBCTL: TRGTYPE Position        */
#define EPWM_LEBCTL_TRGTYPE_Msk           (0x3ul << EPWM_LEBCTL_TRGTYPE_Pos)                 /*!< EPWM_T::LEBCTL: TRGTYPE Mask            */

#define EPWM_LEBCNT_LEBCNT_Pos            (0)                                               /*!< EPWM_T::LEBCNT: LEBCNT Position         */
#define EPWM_LEBCNT_LEBCNT_Msk            (0x1fful << EPWM_LEBCNT_LEBCNT_Pos)                /*!< EPWM_T::LEBCNT: LEBCNT Mask             */

#define EPWM_STATUS_CNTMAXF0_Pos          (0)                                               /*!< EPWM_T::STATUS: CNTMAXF0 Position       */
#define EPWM_STATUS_CNTMAXF0_Msk          (0x1ul << EPWM_STATUS_CNTMAXF0_Pos)                /*!< EPWM_T::STATUS: CNTMAXF0 Mask           */

#define EPWM_STATUS_CNTMAXF1_Pos          (1)                                               /*!< EPWM_T::STATUS: CNTMAXF1 Position       */
#define EPWM_STATUS_CNTMAXF1_Msk          (0x1ul << EPWM_STATUS_CNTMAXF1_Pos)                /*!< EPWM_T::STATUS: CNTMAXF1 Mask           */

#define EPWM_STATUS_CNTMAXF2_Pos          (2)                                               /*!< EPWM_T::STATUS: CNTMAXF2 Position       */
#define EPWM_STATUS_CNTMAXF2_Msk          (0x1ul << EPWM_STATUS_CNTMAXF2_Pos)                /*!< EPWM_T::STATUS: CNTMAXF2 Mask           */

#define EPWM_STATUS_CNTMAXF3_Pos          (3)                                               /*!< EPWM_T::STATUS: CNTMAXF3 Position       */
#define EPWM_STATUS_CNTMAXF3_Msk          (0x1ul << EPWM_STATUS_CNTMAXF3_Pos)                /*!< EPWM_T::STATUS: CNTMAXF3 Mask           */

#define EPWM_STATUS_CNTMAXF4_Pos          (4)                                               /*!< EPWM_T::STATUS: CNTMAXF4 Position       */
#define EPWM_STATUS_CNTMAXF4_Msk          (0x1ul << EPWM_STATUS_CNTMAXF4_Pos)                /*!< EPWM_T::STATUS: CNTMAXF4 Mask           */

#define EPWM_STATUS_CNTMAXF5_Pos          (5)                                               /*!< EPWM_T::STATUS: CNTMAXF5 Position       */
#define EPWM_STATUS_CNTMAXF5_Msk          (0x1ul << EPWM_STATUS_CNTMAXF5_Pos)                /*!< EPWM_T::STATUS: CNTMAXF5 Mask           */

#define EPWM_STATUS_SYNCINF0_Pos          (8)                                               /*!< EPWM_T::STATUS: SYNCINF0 Position       */
#define EPWM_STATUS_SYNCINF0_Msk          (0x1ul << EPWM_STATUS_SYNCINF0_Pos)                /*!< EPWM_T::STATUS: SYNCINF0 Mask           */

#define EPWM_STATUS_SYNCINF2_Pos          (9)                                               /*!< EPWM_T::STATUS: SYNCINF2 Position       */
#define EPWM_STATUS_SYNCINF2_Msk          (0x1ul << EPWM_STATUS_SYNCINF2_Pos)                /*!< EPWM_T::STATUS: SYNCINF2 Mask           */

#define EPWM_STATUS_SYNCINF4_Pos          (10)                                              /*!< EPWM_T::STATUS: SYNCINF4 Position       */
#define EPWM_STATUS_SYNCINF4_Msk          (0x1ul << EPWM_STATUS_SYNCINF4_Pos)                /*!< EPWM_T::STATUS: SYNCINF4 Mask           */

#define EPWM_STATUS_EADCTRGF0_Pos         (16)                                              /*!< EPWM_T::STATUS: EADCTRGF0 Position      */
#define EPWM_STATUS_EADCTRGF0_Msk         (0x1ul << EPWM_STATUS_EADCTRGF0_Pos)               /*!< EPWM_T::STATUS: EADCTRGF0 Mask          */

#define EPWM_STATUS_EADCTRGF1_Pos         (17)                                              /*!< EPWM_T::STATUS: EADCTRGF1 Position      */
#define EPWM_STATUS_EADCTRGF1_Msk         (0x1ul << EPWM_STATUS_EADCTRGF1_Pos)               /*!< EPWM_T::STATUS: EADCTRGF1 Mask          */

#define EPWM_STATUS_EADCTRGF2_Pos         (18)                                              /*!< EPWM_T::STATUS: EADCTRGF2 Position      */
#define EPWM_STATUS_EADCTRGF2_Msk         (0x1ul << EPWM_STATUS_EADCTRGF2_Pos)               /*!< EPWM_T::STATUS: EADCTRGF2 Mask          */

#define EPWM_STATUS_EADCTRGF3_Pos         (19)                                              /*!< EPWM_T::STATUS: EADCTRGF3 Position      */
#define EPWM_STATUS_EADCTRGF3_Msk         (0x1ul << EPWM_STATUS_EADCTRGF3_Pos)               /*!< EPWM_T::STATUS: EADCTRGF3 Mask          */

#define EPWM_STATUS_EADCTRGF4_Pos         (20)                                              /*!< EPWM_T::STATUS: EADCTRGF4 Position      */
#define EPWM_STATUS_EADCTRGF4_Msk         (0x1ul << EPWM_STATUS_EADCTRGF4_Pos)               /*!< EPWM_T::STATUS: EADCTRGF4 Mask          */

#define EPWM_STATUS_EADCTRGF5_Pos         (21)                                              /*!< EPWM_T::STATUS: EADCTRGF5 Position      */
#define EPWM_STATUS_EADCTRGF5_Msk         (0x1ul << EPWM_STATUS_EADCTRGF5_Pos)               /*!< EPWM_T::STATUS: EADCTRGF5 Mask          */

#define EPWM_STATUS_DACTRGF_Pos           (24)                                              /*!< EPWM_T::STATUS: DACTRGF Position        */
#define EPWM_STATUS_DACTRGF_Msk           (0x1ul << EPWM_STATUS_DACTRGF_Pos)                 /*!< EPWM_T::STATUS: DACTRGF Mask            */

#define EPWM_IFA0_IFACNT_Pos              (0)                                               /*!< EPWM_T::IFA0: IFACNT Position           */
#define EPWM_IFA0_IFACNT_Msk              (0xfffful << EPWM_IFA0_IFACNT_Pos)                 /*!< EPWM_T::IFA0: IFACNT Mask               */

#define EPWM_IFA0_STPMOD_Pos              (24)                                              /*!< EPWM_T::IFA0: STPMOD Position           */
#define EPWM_IFA0_STPMOD_Msk              (0x1ul << EPWM_IFA0_STPMOD_Pos)                    /*!< EPWM_T::IFA0: STPMOD Mask               */

#define EPWM_IFA0_IFASEL_Pos              (28)                                              /*!< EPWM_T::IFA0: IFASEL Position           */
#define EPWM_IFA0_IFASEL_Msk              (0x3ul << EPWM_IFA0_IFASEL_Pos)                    /*!< EPWM_T::IFA0: IFASEL Mask               */

#define EPWM_IFA0_IFAEN_Pos               (31)                                              /*!< EPWM_T::IFA0: IFAEN Position            */
#define EPWM_IFA0_IFAEN_Msk               (0x1ul << EPWM_IFA0_IFAEN_Pos)                     /*!< EPWM_T::IFA0: IFAEN Mask                */

#define EPWM_IFA1_IFACNT_Pos              (0)                                               /*!< EPWM_T::IFA1: IFACNT Position           */
#define EPWM_IFA1_IFACNT_Msk              (0xfffful << EPWM_IFA1_IFACNT_Pos)                 /*!< EPWM_T::IFA1: IFACNT Mask               */

#define EPWM_IFA1_STPMOD_Pos              (24)                                              /*!< EPWM_T::IFA1: STPMOD Position           */
#define EPWM_IFA1_STPMOD_Msk              (0x1ul << EPWM_IFA1_STPMOD_Pos)                    /*!< EPWM_T::IFA1: STPMOD Mask               */

#define EPWM_IFA1_IFASEL_Pos              (28)                                              /*!< EPWM_T::IFA1: IFASEL Position           */
#define EPWM_IFA1_IFASEL_Msk              (0x3ul << EPWM_IFA1_IFASEL_Pos)                    /*!< EPWM_T::IFA1: IFASEL Mask               */

#define EPWM_IFA1_IFAEN_Pos               (31)                                              /*!< EPWM_T::IFA1: IFAEN Position            */
#define EPWM_IFA1_IFAEN_Msk               (0x1ul << EPWM_IFA1_IFAEN_Pos)                     /*!< EPWM_T::IFA1: IFAEN Mask                */

#define EPWM_IFA2_IFACNT_Pos              (0)                                               /*!< EPWM_T::IFA2: IFACNT Position           */
#define EPWM_IFA2_IFACNT_Msk              (0xfffful << EPWM_IFA2_IFACNT_Pos)                 /*!< EPWM_T::IFA2: IFACNT Mask               */

#define EPWM_IFA2_STPMOD_Pos              (24)                                              /*!< EPWM_T::IFA2: STPMOD Position           */
#define EPWM_IFA2_STPMOD_Msk              (0x1ul << EPWM_IFA2_STPMOD_Pos)                    /*!< EPWM_T::IFA2: STPMOD Mask               */

#define EPWM_IFA2_IFASEL_Pos              (28)                                              /*!< EPWM_T::IFA2: IFASEL Position           */
#define EPWM_IFA2_IFASEL_Msk              (0x3ul << EPWM_IFA2_IFASEL_Pos)                    /*!< EPWM_T::IFA2: IFASEL Mask               */

#define EPWM_IFA2_IFAEN_Pos               (31)                                              /*!< EPWM_T::IFA2: IFAEN Position            */
#define EPWM_IFA2_IFAEN_Msk               (0x1ul << EPWM_IFA2_IFAEN_Pos)                     /*!< EPWM_T::IFA2: IFAEN Mask                */

#define EPWM_IFA3_IFACNT_Pos              (0)                                               /*!< EPWM_T::IFA3: IFACNT Position           */
#define EPWM_IFA3_IFACNT_Msk              (0xfffful << EPWM_IFA3_IFACNT_Pos)                 /*!< EPWM_T::IFA3: IFACNT Mask               */

#define EPWM_IFA3_STPMOD_Pos              (24)                                              /*!< EPWM_T::IFA3: STPMOD Position           */
#define EPWM_IFA3_STPMOD_Msk              (0x1ul << EPWM_IFA3_STPMOD_Pos)                    /*!< EPWM_T::IFA3: STPMOD Mask               */

#define EPWM_IFA3_IFASEL_Pos              (28)                                              /*!< EPWM_T::IFA3: IFASEL Position           */
#define EPWM_IFA3_IFASEL_Msk              (0x3ul << EPWM_IFA3_IFASEL_Pos)                    /*!< EPWM_T::IFA3: IFASEL Mask               */

#define EPWM_IFA3_IFAEN_Pos               (31)                                              /*!< EPWM_T::IFA3: IFAEN Position            */
#define EPWM_IFA3_IFAEN_Msk               (0x1ul << EPWM_IFA3_IFAEN_Pos)                     /*!< EPWM_T::IFA3: IFAEN Mask                */

#define EPWM_IFA4_IFACNT_Pos              (0)                                               /*!< EPWM_T::IFA4: IFACNT Position           */
#define EPWM_IFA4_IFACNT_Msk              (0xfffful << EPWM_IFA4_IFACNT_Pos)                 /*!< EPWM_T::IFA4: IFACNT Mask               */

#define EPWM_IFA4_STPMOD_Pos              (24)                                              /*!< EPWM_T::IFA4: STPMOD Position           */
#define EPWM_IFA4_STPMOD_Msk              (0x1ul << EPWM_IFA4_STPMOD_Pos)                    /*!< EPWM_T::IFA4: STPMOD Mask               */

#define EPWM_IFA4_IFASEL_Pos              (28)                                              /*!< EPWM_T::IFA4: IFASEL Position           */
#define EPWM_IFA4_IFASEL_Msk              (0x3ul << EPWM_IFA4_IFASEL_Pos)                    /*!< EPWM_T::IFA4: IFASEL Mask               */

#define EPWM_IFA4_IFAEN_Pos               (31)                                              /*!< EPWM_T::IFA4: IFAEN Position            */
#define EPWM_IFA4_IFAEN_Msk               (0x1ul << EPWM_IFA4_IFAEN_Pos)                     /*!< EPWM_T::IFA4: IFAEN Mask                */

#define EPWM_IFA5_IFACNT_Pos              (0)                                               /*!< EPWM_T::IFA5: IFACNT Position           */
#define EPWM_IFA5_IFACNT_Msk              (0xfffful << EPWM_IFA5_IFACNT_Pos)                 /*!< EPWM_T::IFA5: IFACNT Mask               */

#define EPWM_IFA5_STPMOD_Pos              (24)                                              /*!< EPWM_T::IFA5: STPMOD Position           */
#define EPWM_IFA5_STPMOD_Msk              (0x1ul << EPWM_IFA5_STPMOD_Pos)                    /*!< EPWM_T::IFA5: STPMOD Mask               */

#define EPWM_IFA5_IFASEL_Pos              (28)                                              /*!< EPWM_T::IFA5: IFASEL Position           */
#define EPWM_IFA5_IFASEL_Msk              (0x3ul << EPWM_IFA5_IFASEL_Pos)                    /*!< EPWM_T::IFA5: IFASEL Mask               */

#define EPWM_IFA5_IFAEN_Pos               (31)                                              /*!< EPWM_T::IFA5: IFAEN Position            */
#define EPWM_IFA5_IFAEN_Msk               (0x1ul << EPWM_IFA5_IFAEN_Pos)                     /*!< EPWM_T::IFA5: IFAEN Mask                */

#define EPWM_AINTSTS_IFAIF0_Pos           (0)                                               /*!< EPWM_T::AINTSTS: IFAIF0 Position        */
#define EPWM_AINTSTS_IFAIF0_Msk           (0x1ul << EPWM_AINTSTS_IFAIF0_Pos)                 /*!< EPWM_T::AINTSTS: IFAIF0 Mask            */

#define EPWM_AINTSTS_IFAIF1_Pos           (1)                                               /*!< EPWM_T::AINTSTS: IFAIF1 Position        */
#define EPWM_AINTSTS_IFAIF1_Msk           (0x1ul << EPWM_AINTSTS_IFAIF1_Pos)                 /*!< EPWM_T::AINTSTS: IFAIF1 Mask            */

#define EPWM_AINTSTS_IFAIF2_Pos           (2)                                               /*!< EPWM_T::AINTSTS: IFAIF2 Position        */
#define EPWM_AINTSTS_IFAIF2_Msk           (0x1ul << EPWM_AINTSTS_IFAIF2_Pos)                 /*!< EPWM_T::AINTSTS: IFAIF2 Mask            */

#define EPWM_AINTSTS_IFAIF3_Pos           (3)                                               /*!< EPWM_T::AINTSTS: IFAIF3 Position        */
#define EPWM_AINTSTS_IFAIF3_Msk           (0x1ul << EPWM_AINTSTS_IFAIF3_Pos)                 /*!< EPWM_T::AINTSTS: IFAIF3 Mask            */

#define EPWM_AINTSTS_IFAIF4_Pos           (4)                                               /*!< EPWM_T::AINTSTS: IFAIF4 Position        */
#define EPWM_AINTSTS_IFAIF4_Msk           (0x1ul << EPWM_AINTSTS_IFAIF4_Pos)                 /*!< EPWM_T::AINTSTS: IFAIF4 Mask            */

#define EPWM_AINTSTS_IFAIF5_Pos           (5)                                               /*!< EPWM_T::AINTSTS: IFAIF5 Position        */
#define EPWM_AINTSTS_IFAIF5_Msk           (0x1ul << EPWM_AINTSTS_IFAIF5_Pos)                 /*!< EPWM_T::AINTSTS: IFAIF5 Mask            */

#define EPWM_AINTEN_IFAIEN0_Pos           (0)                                               /*!< EPWM_T::AINTEN: IFAIEN0 Position        */
#define EPWM_AINTEN_IFAIEN0_Msk           (0x1ul << EPWM_AINTEN_IFAIEN0_Pos)                 /*!< EPWM_T::AINTEN: IFAIEN0 Mask            */

#define EPWM_AINTEN_IFAIEN1_Pos           (1)                                               /*!< EPWM_T::AINTEN: IFAIEN1 Position        */
#define EPWM_AINTEN_IFAIEN1_Msk           (0x1ul << EPWM_AINTEN_IFAIEN1_Pos)                 /*!< EPWM_T::AINTEN: IFAIEN1 Mask            */

#define EPWM_AINTEN_IFAIEN2_Pos           (2)                                               /*!< EPWM_T::AINTEN: IFAIEN2 Position        */
#define EPWM_AINTEN_IFAIEN2_Msk           (0x1ul << EPWM_AINTEN_IFAIEN2_Pos)                 /*!< EPWM_T::AINTEN: IFAIEN2 Mask            */

#define EPWM_AINTEN_IFAIEN3_Pos           (3)                                               /*!< EPWM_T::AINTEN: IFAIEN3 Position        */
#define EPWM_AINTEN_IFAIEN3_Msk           (0x1ul << EPWM_AINTEN_IFAIEN3_Pos)                 /*!< EPWM_T::AINTEN: IFAIEN3 Mask            */

#define EPWM_AINTEN_IFAIEN4_Pos           (4)                                               /*!< EPWM_T::AINTEN: IFAIEN4 Position        */
#define EPWM_AINTEN_IFAIEN4_Msk           (0x1ul << EPWM_AINTEN_IFAIEN4_Pos)                 /*!< EPWM_T::AINTEN: IFAIEN4 Mask            */

#define EPWM_AINTEN_IFAIEN5_Pos           (5)                                               /*!< EPWM_T::AINTEN: IFAIEN5 Position        */
#define EPWM_AINTEN_IFAIEN5_Msk           (0x1ul << EPWM_AINTEN_IFAIEN5_Pos)                 /*!< EPWM_T::AINTEN: IFAIEN5 Mask            */

#define EPWM_APDMACTL_APDMAEN0_Pos        (0)                                               /*!< EPWM_T::APDMACTL: APDMAEN0 Position     */
#define EPWM_APDMACTL_APDMAEN0_Msk        (0x1ul << EPWM_APDMACTL_APDMAEN0_Pos)              /*!< EPWM_T::APDMACTL: APDMAEN0 Mask         */

#define EPWM_APDMACTL_APDMAEN1_Pos        (1)                                               /*!< EPWM_T::APDMACTL: APDMAEN1 Position     */
#define EPWM_APDMACTL_APDMAEN1_Msk        (0x1ul << EPWM_APDMACTL_APDMAEN1_Pos)              /*!< EPWM_T::APDMACTL: APDMAEN1 Mask         */

#define EPWM_APDMACTL_APDMAEN2_Pos        (2)                                               /*!< EPWM_T::APDMACTL: APDMAEN2 Position     */
#define EPWM_APDMACTL_APDMAEN2_Msk        (0x1ul << EPWM_APDMACTL_APDMAEN2_Pos)              /*!< EPWM_T::APDMACTL: APDMAEN2 Mask         */

#define EPWM_APDMACTL_APDMAEN3_Pos        (3)                                               /*!< EPWM_T::APDMACTL: APDMAEN3 Position     */
#define EPWM_APDMACTL_APDMAEN3_Msk        (0x1ul << EPWM_APDMACTL_APDMAEN3_Pos)              /*!< EPWM_T::APDMACTL: APDMAEN3 Mask         */

#define EPWM_APDMACTL_APDMAEN4_Pos        (4)                                               /*!< EPWM_T::APDMACTL: APDMAEN4 Position     */
#define EPWM_APDMACTL_APDMAEN4_Msk        (0x1ul << EPWM_APDMACTL_APDMAEN4_Pos)              /*!< EPWM_T::APDMACTL: APDMAEN4 Mask         */

#define EPWM_APDMACTL_APDMAEN5_Pos        (5)                                               /*!< EPWM_T::APDMACTL: APDMAEN5 Position     */
#define EPWM_APDMACTL_APDMAEN5_Msk        (0x1ul << EPWM_APDMACTL_APDMAEN5_Pos)              /*!< EPWM_T::APDMACTL: APDMAEN5 Mask         */

#define EPWM_FDEN_FDEN0_Pos               (0)                                               /*!< EPWM_T::FDEN: FDEN0 Position            */
#define EPWM_FDEN_FDEN0_Msk               (0x1ul << EPWM_FDEN_FDEN0_Pos)                     /*!< EPWM_T::FDEN: FDEN0 Mask                */

#define EPWM_FDEN_FDEN1_Pos               (1)                                               /*!< EPWM_T::FDEN: FDEN1 Position            */
#define EPWM_FDEN_FDEN1_Msk               (0x1ul << EPWM_FDEN_FDEN1_Pos)                     /*!< EPWM_T::FDEN: FDEN1 Mask                */

#define EPWM_FDEN_FDEN2_Pos               (2)                                               /*!< EPWM_T::FDEN: FDEN2 Position            */
#define EPWM_FDEN_FDEN2_Msk               (0x1ul << EPWM_FDEN_FDEN2_Pos)                     /*!< EPWM_T::FDEN: FDEN2 Mask                */

#define EPWM_FDEN_FDEN3_Pos               (3)                                               /*!< EPWM_T::FDEN: FDEN3 Position            */
#define EPWM_FDEN_FDEN3_Msk               (0x1ul << EPWM_FDEN_FDEN3_Pos)                     /*!< EPWM_T::FDEN: FDEN3 Mask                */

#define EPWM_FDEN_FDEN4_Pos               (4)                                               /*!< EPWM_T::FDEN: FDEN4 Position            */
#define EPWM_FDEN_FDEN4_Msk               (0x1ul << EPWM_FDEN_FDEN4_Pos)                     /*!< EPWM_T::FDEN: FDEN4 Mask                */

#define EPWM_FDEN_FDEN5_Pos               (5)                                               /*!< EPWM_T::FDEN: FDEN5 Position            */
#define EPWM_FDEN_FDEN5_Msk               (0x1ul << EPWM_FDEN_FDEN5_Pos)                     /*!< EPWM_T::FDEN: FDEN5 Mask                */

#define EPWM_FDEN_FDODIS0_Pos             (8)                                               /*!< EPWM_T::FDEN: FDODIS0 Position          */
#define EPWM_FDEN_FDODIS0_Msk             (0x1ul << EPWM_FDEN_FDODIS0_Pos)                   /*!< EPWM_T::FDEN: FDODIS0 Mask              */

#define EPWM_FDEN_FDODIS1_Pos             (9)                                               /*!< EPWM_T::FDEN: FDODIS1 Position          */
#define EPWM_FDEN_FDODIS1_Msk             (0x1ul << EPWM_FDEN_FDODIS1_Pos)                   /*!< EPWM_T::FDEN: FDODIS1 Mask              */

#define EPWM_FDEN_FDODIS2_Pos             (10)                                              /*!< EPWM_T::FDEN: FDODIS2 Position          */
#define EPWM_FDEN_FDODIS2_Msk             (0x1ul << EPWM_FDEN_FDODIS2_Pos)                   /*!< EPWM_T::FDEN: FDODIS2 Mask              */

#define EPWM_FDEN_FDODIS3_Pos             (11)                                              /*!< EPWM_T::FDEN: FDODIS3 Position          */
#define EPWM_FDEN_FDODIS3_Msk             (0x1ul << EPWM_FDEN_FDODIS3_Pos)                   /*!< EPWM_T::FDEN: FDODIS3 Mask              */

#define EPWM_FDEN_FDODIS4_Pos             (12)                                              /*!< EPWM_T::FDEN: FDODIS4 Position          */
#define EPWM_FDEN_FDODIS4_Msk             (0x1ul << EPWM_FDEN_FDODIS4_Pos)                   /*!< EPWM_T::FDEN: FDODIS4 Mask              */

#define EPWM_FDEN_FDODIS5_Pos             (13)                                              /*!< EPWM_T::FDEN: FDODIS5 Position          */
#define EPWM_FDEN_FDODIS5_Msk             (0x1ul << EPWM_FDEN_FDODIS5_Pos)                   /*!< EPWM_T::FDEN: FDODIS5 Mask              */

#define EPWM_FDEN_FDCKS0_Pos              (16)                                              /*!< EPWM_T::FDEN: FDCKS0 Position           */
#define EPWM_FDEN_FDCKS0_Msk              (0x1ul << EPWM_FDEN_FDCKS0_Pos)                    /*!< EPWM_T::FDEN: FDCKS0 Mask               */

#define EPWM_FDEN_FDCKS1_Pos              (17)                                              /*!< EPWM_T::FDEN: FDCKS1 Position           */
#define EPWM_FDEN_FDCKS1_Msk              (0x1ul << EPWM_FDEN_FDCKS1_Pos)                    /*!< EPWM_T::FDEN: FDCKS1 Mask               */

#define EPWM_FDEN_FDCKS2_Pos              (18)                                              /*!< EPWM_T::FDEN: FDCKS2 Position           */
#define EPWM_FDEN_FDCKS2_Msk              (0x1ul << EPWM_FDEN_FDCKS2_Pos)                    /*!< EPWM_T::FDEN: FDCKS2 Mask               */

#define EPWM_FDEN_FDCKS3_Pos              (19)                                              /*!< EPWM_T::FDEN: FDCKS3 Position           */
#define EPWM_FDEN_FDCKS3_Msk              (0x1ul << EPWM_FDEN_FDCKS3_Pos)                    /*!< EPWM_T::FDEN: FDCKS3 Mask               */

#define EPWM_FDEN_FDCKS4_Pos              (20)                                              /*!< EPWM_T::FDEN: FDCKS4 Position           */
#define EPWM_FDEN_FDCKS4_Msk              (0x1ul << EPWM_FDEN_FDCKS4_Pos)                    /*!< EPWM_T::FDEN: FDCKS4 Mask               */

#define EPWM_FDEN_FDCKS5_Pos              (21)                                              /*!< EPWM_T::FDEN: FDCKS5 Position           */
#define EPWM_FDEN_FDCKS5_Msk              (0x1ul << EPWM_FDEN_FDCKS5_Pos)                    /*!< EPWM_T::FDEN: FDCKS5 Mask               */

#define EPWM_FDCTL0_TRMSKCNT_Pos          (0)                                               /*!< EPWM_T::FDCTL0: TRMSKCNT Position       */
#define EPWM_FDCTL0_TRMSKCNT_Msk          (0x7ful << EPWM_FDCTL0_TRMSKCNT_Pos)               /*!< EPWM_T::FDCTL0: TRMSKCNT Mask           */

#define EPWM_FDCTL0_FDMSKEN_Pos           (15)                                              /*!< EPWM_T::FDCTL0: FDMSKEN Position        */
#define EPWM_FDCTL0_FDMSKEN_Msk           (0x1ul << EPWM_FDCTL0_FDMSKEN_Pos)                 /*!< EPWM_T::FDCTL0: FDMSKEN Mask            */

#define EPWM_FDCTL0_DGSMPCYC_Pos          (16)                                              /*!< EPWM_T::FDCTL0: DGSMPCYC Position       */
#define EPWM_FDCTL0_DGSMPCYC_Msk          (0x7ul << EPWM_FDCTL0_DGSMPCYC_Pos)                /*!< EPWM_T::FDCTL0: DGSMPCYC Mask           */

#define EPWM_FDCTL0_FDCKSEL_Pos           (28)                                              /*!< EPWM_T::FDCTL0: FDCKSEL Position        */
#define EPWM_FDCTL0_FDCKSEL_Msk           (0x3ul << EPWM_FDCTL0_FDCKSEL_Pos)                 /*!< EPWM_T::FDCTL0: FDCKSEL Mask            */

#define EPWM_FDCTL0_FDDGEN_Pos            (31)                                              /*!< EPWM_T::FDCTL0: FDDGEN Position         */
#define EPWM_FDCTL0_FDDGEN_Msk            (0x1ul << EPWM_FDCTL0_FDDGEN_Pos)                  /*!< EPWM_T::FDCTL0: FDDGEN Mask             */

#define EPWM_FDCTL1_TRMSKCNT_Pos          (0)                                               /*!< EPWM_T::FDCTL1: TRMSKCNT Position       */
#define EPWM_FDCTL1_TRMSKCNT_Msk          (0x7ful << EPWM_FDCTL1_TRMSKCNT_Pos)               /*!< EPWM_T::FDCTL1: TRMSKCNT Mask           */

#define EPWM_FDCTL1_FDMSKEN_Pos           (15)                                              /*!< EPWM_T::FDCTL1: FDMSKEN Position        */
#define EPWM_FDCTL1_FDMSKEN_Msk           (0x1ul << EPWM_FDCTL1_FDMSKEN_Pos)                 /*!< EPWM_T::FDCTL1: FDMSKEN Mask            */

#define EPWM_FDCTL1_DGSMPCYC_Pos          (16)                                              /*!< EPWM_T::FDCTL1: DGSMPCYC Position       */
#define EPWM_FDCTL1_DGSMPCYC_Msk          (0x7ul << EPWM_FDCTL1_DGSMPCYC_Pos)                /*!< EPWM_T::FDCTL1: DGSMPCYC Mask           */

#define EPWM_FDCTL1_FDCKSEL_Pos           (28)                                              /*!< EPWM_T::FDCTL1: FDCKSEL Position        */
#define EPWM_FDCTL1_FDCKSEL_Msk           (0x3ul << EPWM_FDCTL1_FDCKSEL_Pos)                 /*!< EPWM_T::FDCTL1: FDCKSEL Mask            */

#define EPWM_FDCTL1_FDDGEN_Pos            (31)                                              /*!< EPWM_T::FDCTL1: FDDGEN Position         */
#define EPWM_FDCTL1_FDDGEN_Msk            (0x1ul << EPWM_FDCTL1_FDDGEN_Pos)                  /*!< EPWM_T::FDCTL1: FDDGEN Mask             */

#define EPWM_FDCTL2_TRMSKCNT_Pos          (0)                                               /*!< EPWM_T::FDCTL2: TRMSKCNT Position       */
#define EPWM_FDCTL2_TRMSKCNT_Msk          (0x7ful << EPWM_FDCTL2_TRMSKCNT_Pos)               /*!< EPWM_T::FDCTL2: TRMSKCNT Mask           */

#define EPWM_FDCTL2_FDMSKEN_Pos           (15)                                              /*!< EPWM_T::FDCTL2: FDMSKEN Position        */
#define EPWM_FDCTL2_FDMSKEN_Msk           (0x1ul << EPWM_FDCTL2_FDMSKEN_Pos)                 /*!< EPWM_T::FDCTL2: FDMSKEN Mask            */

#define EPWM_FDCTL2_DGSMPCYC_Pos          (16)                                              /*!< EPWM_T::FDCTL2: DGSMPCYC Position       */
#define EPWM_FDCTL2_DGSMPCYC_Msk          (0x7ul << EPWM_FDCTL2_DGSMPCYC_Pos)                /*!< EPWM_T::FDCTL2: DGSMPCYC Mask           */

#define EPWM_FDCTL2_FDCKSEL_Pos           (28)                                              /*!< EPWM_T::FDCTL2: FDCKSEL Position        */
#define EPWM_FDCTL2_FDCKSEL_Msk           (0x3ul << EPWM_FDCTL2_FDCKSEL_Pos)                 /*!< EPWM_T::FDCTL2: FDCKSEL Mask            */

#define EPWM_FDCTL2_FDDGEN_Pos            (31)                                              /*!< EPWM_T::FDCTL2: FDDGEN Position         */
#define EPWM_FDCTL2_FDDGEN_Msk            (0x1ul << EPWM_FDCTL2_FDDGEN_Pos)                  /*!< EPWM_T::FDCTL2: FDDGEN Mask             */

#define EPWM_FDCTL3_TRMSKCNT_Pos          (0)                                               /*!< EPWM_T::FDCTL3: TRMSKCNT Position       */
#define EPWM_FDCTL3_TRMSKCNT_Msk          (0x7ful << EPWM_FDCTL3_TRMSKCNT_Pos)               /*!< EPWM_T::FDCTL3: TRMSKCNT Mask           */

#define EPWM_FDCTL3_FDMSKEN_Pos           (15)                                              /*!< EPWM_T::FDCTL3: FDMSKEN Position        */
#define EPWM_FDCTL3_FDMSKEN_Msk           (0x1ul << EPWM_FDCTL3_FDMSKEN_Pos)                 /*!< EPWM_T::FDCTL3: FDMSKEN Mask            */

#define EPWM_FDCTL3_DGSMPCYC_Pos          (16)                                              /*!< EPWM_T::FDCTL3: DGSMPCYC Position       */
#define EPWM_FDCTL3_DGSMPCYC_Msk          (0x7ul << EPWM_FDCTL3_DGSMPCYC_Pos)                /*!< EPWM_T::FDCTL3: DGSMPCYC Mask           */

#define EPWM_FDCTL3_FDCKSEL_Pos           (28)                                              /*!< EPWM_T::FDCTL3: FDCKSEL Position        */
#define EPWM_FDCTL3_FDCKSEL_Msk           (0x3ul << EPWM_FDCTL3_FDCKSEL_Pos)                 /*!< EPWM_T::FDCTL3: FDCKSEL Mask            */

#define EPWM_FDCTL3_FDDGEN_Pos            (31)                                              /*!< EPWM_T::FDCTL3: FDDGEN Position         */
#define EPWM_FDCTL3_FDDGEN_Msk            (0x1ul << EPWM_FDCTL3_FDDGEN_Pos)                  /*!< EPWM_T::FDCTL3: FDDGEN Mask             */

#define EPWM_FDCTL4_TRMSKCNT_Pos          (0)                                               /*!< EPWM_T::FDCTL4: TRMSKCNT Position       */
#define EPWM_FDCTL4_TRMSKCNT_Msk          (0x7ful << EPWM_FDCTL4_TRMSKCNT_Pos)               /*!< EPWM_T::FDCTL4: TRMSKCNT Mask           */

#define EPWM_FDCTL4_FDMSKEN_Pos           (15)                                              /*!< EPWM_T::FDCTL4: FDMSKEN Position        */
#define EPWM_FDCTL4_FDMSKEN_Msk           (0x1ul << EPWM_FDCTL4_FDMSKEN_Pos)                 /*!< EPWM_T::FDCTL4: FDMSKEN Mask            */

#define EPWM_FDCTL4_DGSMPCYC_Pos          (16)                                              /*!< EPWM_T::FDCTL4: DGSMPCYC Position       */
#define EPWM_FDCTL4_DGSMPCYC_Msk          (0x7ul << EPWM_FDCTL4_DGSMPCYC_Pos)                /*!< EPWM_T::FDCTL4: DGSMPCYC Mask           */

#define EPWM_FDCTL4_FDCKSEL_Pos           (28)                                              /*!< EPWM_T::FDCTL4: FDCKSEL Position        */
#define EPWM_FDCTL4_FDCKSEL_Msk           (0x3ul << EPWM_FDCTL4_FDCKSEL_Pos)                 /*!< EPWM_T::FDCTL4: FDCKSEL Mask            */

#define EPWM_FDCTL4_FDDGEN_Pos            (31)                                              /*!< EPWM_T::FDCTL4: FDDGEN Position         */
#define EPWM_FDCTL4_FDDGEN_Msk            (0x1ul << EPWM_FDCTL4_FDDGEN_Pos)                  /*!< EPWM_T::FDCTL4: FDDGEN Mask             */

#define EPWM_FDCTL5_TRMSKCNT_Pos          (0)                                               /*!< EPWM_T::FDCTL5: TRMSKCNT Position       */
#define EPWM_FDCTL5_TRMSKCNT_Msk          (0x7ful << EPWM_FDCTL5_TRMSKCNT_Pos)               /*!< EPWM_T::FDCTL5: TRMSKCNT Mask           */

#define EPWM_FDCTL5_FDMSKEN_Pos           (15)                                              /*!< EPWM_T::FDCTL5: FDMSKEN Position        */
#define EPWM_FDCTL5_FDMSKEN_Msk           (0x1ul << EPWM_FDCTL5_FDMSKEN_Pos)                 /*!< EPWM_T::FDCTL5: FDMSKEN Mask            */

#define EPWM_FDCTL5_DGSMPCYC_Pos          (16)                                              /*!< EPWM_T::FDCTL5: DGSMPCYC Position       */
#define EPWM_FDCTL5_DGSMPCYC_Msk          (0x7ul << EPWM_FDCTL5_DGSMPCYC_Pos)                /*!< EPWM_T::FDCTL5: DGSMPCYC Mask           */

#define EPWM_FDCTL5_FDCKSEL_Pos           (28)                                              /*!< EPWM_T::FDCTL5: FDCKSEL Position        */
#define EPWM_FDCTL5_FDCKSEL_Msk           (0x3ul << EPWM_FDCTL5_FDCKSEL_Pos)                 /*!< EPWM_T::FDCTL5: FDCKSEL Mask            */

#define EPWM_FDCTL5_FDDGEN_Pos            (31)                                              /*!< EPWM_T::FDCTL5: FDDGEN Position         */
#define EPWM_FDCTL5_FDDGEN_Msk            (0x1ul << EPWM_FDCTL5_FDDGEN_Pos)                  /*!< EPWM_T::FDCTL5: FDDGEN Mask             */

#define EPWM_FDIEN_FDIEN0_Pos             (0)                                               /*!< EPWM_T::FDIEN: FDIEN0 Position          */
#define EPWM_FDIEN_FDIEN0_Msk             (0x1ul << EPWM_FDIEN_FDIEN0_Pos)                   /*!< EPWM_T::FDIEN: FDIEN0 Mask              */

#define EPWM_FDIEN_FDIEN1_Pos             (1)                                               /*!< EPWM_T::FDIEN: FDIEN1 Position          */
#define EPWM_FDIEN_FDIEN1_Msk             (0x1ul << EPWM_FDIEN_FDIEN1_Pos)                   /*!< EPWM_T::FDIEN: FDIEN1 Mask              */

#define EPWM_FDIEN_FDIEN2_Pos             (2)                                               /*!< EPWM_T::FDIEN: FDIEN2 Position          */
#define EPWM_FDIEN_FDIEN2_Msk             (0x1ul << EPWM_FDIEN_FDIEN2_Pos)                   /*!< EPWM_T::FDIEN: FDIEN2 Mask              */

#define EPWM_FDIEN_FDIEN3_Pos             (3)                                               /*!< EPWM_T::FDIEN: FDIEN3 Position          */
#define EPWM_FDIEN_FDIEN3_Msk             (0x1ul << EPWM_FDIEN_FDIEN3_Pos)                   /*!< EPWM_T::FDIEN: FDIEN3 Mask              */

#define EPWM_FDIEN_FDIEN4_Pos             (4)                                               /*!< EPWM_T::FDIEN: FDIEN4 Position          */
#define EPWM_FDIEN_FDIEN4_Msk             (0x1ul << EPWM_FDIEN_FDIEN4_Pos)                   /*!< EPWM_T::FDIEN: FDIEN4 Mask              */

#define EPWM_FDIEN_FDIEN5_Pos             (5)                                               /*!< EPWM_T::FDIEN: FDIEN5 Position          */
#define EPWM_FDIEN_FDIEN5_Msk             (0x1ul << EPWM_FDIEN_FDIEN5_Pos)                   /*!< EPWM_T::FDIEN: FDIEN5 Mask              */

#define EPWM_FDSTS_FDIF0_Pos              (0)                                               /*!< EPWM_T::FDSTS: FDIF0 Position           */
#define EPWM_FDSTS_FDIF0_Msk              (0x1ul << EPWM_FDSTS_FDIF0_Pos)                    /*!< EPWM_T::FDSTS: FDIF0 Mask               */

#define EPWM_FDSTS_FDIF1_Pos              (1)                                               /*!< EPWM_T::FDSTS: FDIF1 Position           */
#define EPWM_FDSTS_FDIF1_Msk              (0x1ul << EPWM_FDSTS_FDIF1_Pos)                    /*!< EPWM_T::FDSTS: FDIF1 Mask               */

#define EPWM_FDSTS_FDIF2_Pos              (2)                                               /*!< EPWM_T::FDSTS: FDIF2 Position           */
#define EPWM_FDSTS_FDIF2_Msk              (0x1ul << EPWM_FDSTS_FDIF2_Pos)                    /*!< EPWM_T::FDSTS: FDIF2 Mask               */

#define EPWM_FDSTS_FDIF3_Pos              (3)                                               /*!< EPWM_T::FDSTS: FDIF3 Position           */
#define EPWM_FDSTS_FDIF3_Msk              (0x1ul << EPWM_FDSTS_FDIF3_Pos)                    /*!< EPWM_T::FDSTS: FDIF3 Mask               */

#define EPWM_FDSTS_FDIF4_Pos              (4)                                               /*!< EPWM_T::FDSTS: FDIF4 Position           */
#define EPWM_FDSTS_FDIF4_Msk              (0x1ul << EPWM_FDSTS_FDIF4_Pos)                    /*!< EPWM_T::FDSTS: FDIF4 Mask               */

#define EPWM_FDSTS_FDIF5_Pos              (5)                                               /*!< EPWM_T::FDSTS: FDIF5 Position           */
#define EPWM_FDSTS_FDIF5_Msk              (0x1ul << EPWM_FDSTS_FDIF5_Pos)                    /*!< EPWM_T::FDSTS: FDIF5 Mask               */

#define EPWM_EADCPSCCTL_PSCEN0_Pos        (0)                                               /*!< EPWM_T::EADCPSCCTL: PSCEN0 Position     */
#define EPWM_EADCPSCCTL_PSCEN0_Msk        (0x1ul << EPWM_EADCPSCCTL_PSCEN0_Pos)              /*!< EPWM_T::EADCPSCCTL: PSCEN0 Mask         */

#define EPWM_EADCPSCCTL_PSCEN1_Pos        (1)                                               /*!< EPWM_T::EADCPSCCTL: PSCEN1 Position     */
#define EPWM_EADCPSCCTL_PSCEN1_Msk        (0x1ul << EPWM_EADCPSCCTL_PSCEN1_Pos)              /*!< EPWM_T::EADCPSCCTL: PSCEN1 Mask         */

#define EPWM_EADCPSCCTL_PSCEN2_Pos        (2)                                               /*!< EPWM_T::EADCPSCCTL: PSCEN2 Position     */
#define EPWM_EADCPSCCTL_PSCEN2_Msk        (0x1ul << EPWM_EADCPSCCTL_PSCEN2_Pos)              /*!< EPWM_T::EADCPSCCTL: PSCEN2 Mask         */

#define EPWM_EADCPSCCTL_PSCEN3_Pos        (3)                                               /*!< EPWM_T::EADCPSCCTL: PSCEN3 Position     */
#define EPWM_EADCPSCCTL_PSCEN3_Msk        (0x1ul << EPWM_EADCPSCCTL_PSCEN3_Pos)              /*!< EPWM_T::EADCPSCCTL: PSCEN3 Mask         */

#define EPWM_EADCPSCCTL_PSCEN4_Pos        (4)                                               /*!< EPWM_T::EADCPSCCTL: PSCEN4 Position     */
#define EPWM_EADCPSCCTL_PSCEN4_Msk        (0x1ul << EPWM_EADCPSCCTL_PSCEN4_Pos)              /*!< EPWM_T::EADCPSCCTL: PSCEN4 Mask         */

#define EPWM_EADCPSCCTL_PSCEN5_Pos        (5)                                               /*!< EPWM_T::EADCPSCCTL: PSCEN5 Position     */
#define EPWM_EADCPSCCTL_PSCEN5_Msk        (0x1ul << EPWM_EADCPSCCTL_PSCEN5_Pos)              /*!< EPWM_T::EADCPSCCTL: PSCEN5 Mask         */

#define EPWM_EADCPSC0_EADCPSC0_Pos        (0)                                               /*!< EPWM_T::EADCPSC0: EADCPSC0 Position     */
#define EPWM_EADCPSC0_EADCPSC0_Msk        (0xful << EPWM_EADCPSC0_EADCPSC0_Pos)              /*!< EPWM_T::EADCPSC0: EADCPSC0 Mask         */

#define EPWM_EADCPSC0_EADCPSC1_Pos        (8)                                               /*!< EPWM_T::EADCPSC0: EADCPSC1 Position     */
#define EPWM_EADCPSC0_EADCPSC1_Msk        (0xful << EPWM_EADCPSC0_EADCPSC1_Pos)              /*!< EPWM_T::EADCPSC0: EADCPSC1 Mask         */

#define EPWM_EADCPSC0_EADCPSC2_Pos        (16)                                              /*!< EPWM_T::EADCPSC0: EADCPSC2 Position     */
#define EPWM_EADCPSC0_EADCPSC2_Msk        (0xful << EPWM_EADCPSC0_EADCPSC2_Pos)              /*!< EPWM_T::EADCPSC0: EADCPSC2 Mask         */

#define EPWM_EADCPSC0_EADCPSC3_Pos        (24)                                              /*!< EPWM_T::EADCPSC0: EADCPSC3 Position     */
#define EPWM_EADCPSC0_EADCPSC3_Msk        (0xful << EPWM_EADCPSC0_EADCPSC3_Pos)              /*!< EPWM_T::EADCPSC0: EADCPSC3 Mask         */

#define EPWM_EADCPSC1_EADCPSC4_Pos        (0)                                               /*!< EPWM_T::EADCPSC1: EADCPSC4 Position     */
#define EPWM_EADCPSC1_EADCPSC4_Msk        (0xful << EPWM_EADCPSC1_EADCPSC4_Pos)              /*!< EPWM_T::EADCPSC1: EADCPSC4 Mask         */

#define EPWM_EADCPSC1_EADCPSC5_Pos        (8)                                               /*!< EPWM_T::EADCPSC1: EADCPSC5 Position     */
#define EPWM_EADCPSC1_EADCPSC5_Msk        (0xful << EPWM_EADCPSC1_EADCPSC5_Pos)              /*!< EPWM_T::EADCPSC1: EADCPSC5 Mask         */

#define EPWM_EADCPSCNT0_PSCNT0_Pos        (0)                                               /*!< EPWM_T::EADCPSCNT0: PSCNT0 Position     */
#define EPWM_EADCPSCNT0_PSCNT0_Msk        (0xful << EPWM_EADCPSCNT0_PSCNT0_Pos)              /*!< EPWM_T::EADCPSCNT0: PSCNT0 Mask         */

#define EPWM_EADCPSCNT0_PSCNT1_Pos        (8)                                               /*!< EPWM_T::EADCPSCNT0: PSCNT1 Position     */
#define EPWM_EADCPSCNT0_PSCNT1_Msk        (0xful << EPWM_EADCPSCNT0_PSCNT1_Pos)              /*!< EPWM_T::EADCPSCNT0: PSCNT1 Mask         */

#define EPWM_EADCPSCNT0_PSCNT2_Pos        (16)                                              /*!< EPWM_T::EADCPSCNT0: PSCNT2 Position     */
#define EPWM_EADCPSCNT0_PSCNT2_Msk        (0xful << EPWM_EADCPSCNT0_PSCNT2_Pos)              /*!< EPWM_T::EADCPSCNT0: PSCNT2 Mask         */

#define EPWM_EADCPSCNT0_PSCNT3_Pos        (24)                                              /*!< EPWM_T::EADCPSCNT0: PSCNT3 Position     */
#define EPWM_EADCPSCNT0_PSCNT3_Msk        (0xful << EPWM_EADCPSCNT0_PSCNT3_Pos)              /*!< EPWM_T::EADCPSCNT0: PSCNT3 Mask         */

#define EPWM_EADCPSCNT1_PSCNT4_Pos        (0)                                               /*!< EPWM_T::EADCPSCNT1: PSCNT4 Position     */
#define EPWM_EADCPSCNT1_PSCNT4_Msk        (0xful << EPWM_EADCPSCNT1_PSCNT4_Pos)              /*!< EPWM_T::EADCPSCNT1: PSCNT4 Mask         */

#define EPWM_EADCPSCNT1_PSCNT5_Pos        (8)                                               /*!< EPWM_T::EADCPSCNT1: PSCNT5 Position     */
#define EPWM_EADCPSCNT1_PSCNT5_Msk        (0xful << EPWM_EADCPSCNT1_PSCNT5_Pos)              /*!< EPWM_T::EADCPSCNT1: PSCNT5 Mask         */

#define EPWM_CAPINEN_CAPINEN0_Pos         (0)                                               /*!< EPWM_T::CAPINEN: CAPINEN0 Position      */
#define EPWM_CAPINEN_CAPINEN0_Msk         (0x1ul << EPWM_CAPINEN_CAPINEN0_Pos)               /*!< EPWM_T::CAPINEN: CAPINEN0 Mask          */

#define EPWM_CAPINEN_CAPINEN1_Pos         (1)                                               /*!< EPWM_T::CAPINEN: CAPINEN1 Position      */
#define EPWM_CAPINEN_CAPINEN1_Msk         (0x1ul << EPWM_CAPINEN_CAPINEN1_Pos)               /*!< EPWM_T::CAPINEN: CAPINEN1 Mask          */

#define EPWM_CAPINEN_CAPINEN2_Pos         (2)                                               /*!< EPWM_T::CAPINEN: CAPINEN2 Position      */
#define EPWM_CAPINEN_CAPINEN2_Msk         (0x1ul << EPWM_CAPINEN_CAPINEN2_Pos)               /*!< EPWM_T::CAPINEN: CAPINEN2 Mask          */

#define EPWM_CAPINEN_CAPINEN3_Pos         (3)                                               /*!< EPWM_T::CAPINEN: CAPINEN3 Position      */
#define EPWM_CAPINEN_CAPINEN3_Msk         (0x1ul << EPWM_CAPINEN_CAPINEN3_Pos)               /*!< EPWM_T::CAPINEN: CAPINEN3 Mask          */

#define EPWM_CAPINEN_CAPINEN4_Pos         (4)                                               /*!< EPWM_T::CAPINEN: CAPINEN4 Position      */
#define EPWM_CAPINEN_CAPINEN4_Msk         (0x1ul << EPWM_CAPINEN_CAPINEN4_Pos)               /*!< EPWM_T::CAPINEN: CAPINEN4 Mask          */

#define EPWM_CAPINEN_CAPINEN5_Pos         (5)                                               /*!< EPWM_T::CAPINEN: CAPINEN5 Position      */
#define EPWM_CAPINEN_CAPINEN5_Msk         (0x1ul << EPWM_CAPINEN_CAPINEN5_Pos)               /*!< EPWM_T::CAPINEN: CAPINEN5 Mask          */

#define EPWM_CAPCTL_CAPEN0_Pos            (0)                                               /*!< EPWM_T::CAPCTL: CAPEN0 Position         */
#define EPWM_CAPCTL_CAPEN0_Msk            (0x1ul << EPWM_CAPCTL_CAPEN0_Pos)                  /*!< EPWM_T::CAPCTL: CAPEN0 Mask             */

#define EPWM_CAPCTL_CAPEN1_Pos            (1)                                               /*!< EPWM_T::CAPCTL: CAPEN1 Position         */
#define EPWM_CAPCTL_CAPEN1_Msk            (0x1ul << EPWM_CAPCTL_CAPEN1_Pos)                  /*!< EPWM_T::CAPCTL: CAPEN1 Mask             */

#define EPWM_CAPCTL_CAPEN2_Pos            (2)                                               /*!< EPWM_T::CAPCTL: CAPEN2 Position         */
#define EPWM_CAPCTL_CAPEN2_Msk            (0x1ul << EPWM_CAPCTL_CAPEN2_Pos)                  /*!< EPWM_T::CAPCTL: CAPEN2 Mask             */

#define EPWM_CAPCTL_CAPEN3_Pos            (3)                                               /*!< EPWM_T::CAPCTL: CAPEN3 Position         */
#define EPWM_CAPCTL_CAPEN3_Msk            (0x1ul << EPWM_CAPCTL_CAPEN3_Pos)                  /*!< EPWM_T::CAPCTL: CAPEN3 Mask             */

#define EPWM_CAPCTL_CAPEN4_Pos            (4)                                               /*!< EPWM_T::CAPCTL: CAPEN4 Position         */
#define EPWM_CAPCTL_CAPEN4_Msk            (0x1ul << EPWM_CAPCTL_CAPEN4_Pos)                  /*!< EPWM_T::CAPCTL: CAPEN4 Mask             */

#define EPWM_CAPCTL_CAPEN5_Pos            (5)                                               /*!< EPWM_T::CAPCTL: CAPEN5 Position         */
#define EPWM_CAPCTL_CAPEN5_Msk            (0x1ul << EPWM_CAPCTL_CAPEN5_Pos)                  /*!< EPWM_T::CAPCTL: CAPEN5 Mask             */

#define EPWM_CAPCTL_CAPINV0_Pos           (8)                                               /*!< EPWM_T::CAPCTL: CAPINV0 Position        */
#define EPWM_CAPCTL_CAPINV0_Msk           (0x1ul << EPWM_CAPCTL_CAPINV0_Pos)                 /*!< EPWM_T::CAPCTL: CAPINV0 Mask            */

#define EPWM_CAPCTL_CAPINV1_Pos           (9)                                               /*!< EPWM_T::CAPCTL: CAPINV1 Position        */
#define EPWM_CAPCTL_CAPINV1_Msk           (0x1ul << EPWM_CAPCTL_CAPINV1_Pos)                 /*!< EPWM_T::CAPCTL: CAPINV1 Mask            */

#define EPWM_CAPCTL_CAPINV2_Pos           (10)                                              /*!< EPWM_T::CAPCTL: CAPINV2 Position        */
#define EPWM_CAPCTL_CAPINV2_Msk           (0x1ul << EPWM_CAPCTL_CAPINV2_Pos)                 /*!< EPWM_T::CAPCTL: CAPINV2 Mask            */

#define EPWM_CAPCTL_CAPINV3_Pos           (11)                                              /*!< EPWM_T::CAPCTL: CAPINV3 Position        */
#define EPWM_CAPCTL_CAPINV3_Msk           (0x1ul << EPWM_CAPCTL_CAPINV3_Pos)                 /*!< EPWM_T::CAPCTL: CAPINV3 Mask            */

#define EPWM_CAPCTL_CAPINV4_Pos           (12)                                              /*!< EPWM_T::CAPCTL: CAPINV4 Position        */
#define EPWM_CAPCTL_CAPINV4_Msk           (0x1ul << EPWM_CAPCTL_CAPINV4_Pos)                 /*!< EPWM_T::CAPCTL: CAPINV4 Mask            */

#define EPWM_CAPCTL_CAPINV5_Pos           (13)                                              /*!< EPWM_T::CAPCTL: CAPINV5 Position        */
#define EPWM_CAPCTL_CAPINV5_Msk           (0x1ul << EPWM_CAPCTL_CAPINV5_Pos)                 /*!< EPWM_T::CAPCTL: CAPINV5 Mask            */

#define EPWM_CAPCTL_RCRLDEN0_Pos          (16)                                              /*!< EPWM_T::CAPCTL: RCRLDEN0 Position       */
#define EPWM_CAPCTL_RCRLDEN0_Msk          (0x1ul << EPWM_CAPCTL_RCRLDEN0_Pos)                /*!< EPWM_T::CAPCTL: RCRLDEN0 Mask           */

#define EPWM_CAPCTL_RCRLDEN1_Pos          (17)                                              /*!< EPWM_T::CAPCTL: RCRLDEN1 Position       */
#define EPWM_CAPCTL_RCRLDEN1_Msk          (0x1ul << EPWM_CAPCTL_RCRLDEN1_Pos)                /*!< EPWM_T::CAPCTL: RCRLDEN1 Mask           */

#define EPWM_CAPCTL_RCRLDEN2_Pos          (18)                                              /*!< EPWM_T::CAPCTL: RCRLDEN2 Position       */
#define EPWM_CAPCTL_RCRLDEN2_Msk          (0x1ul << EPWM_CAPCTL_RCRLDEN2_Pos)                /*!< EPWM_T::CAPCTL: RCRLDEN2 Mask           */

#define EPWM_CAPCTL_RCRLDEN3_Pos          (19)                                              /*!< EPWM_T::CAPCTL: RCRLDEN3 Position       */
#define EPWM_CAPCTL_RCRLDEN3_Msk          (0x1ul << EPWM_CAPCTL_RCRLDEN3_Pos)                /*!< EPWM_T::CAPCTL: RCRLDEN3 Mask           */

#define EPWM_CAPCTL_RCRLDEN4_Pos          (20)                                              /*!< EPWM_T::CAPCTL: RCRLDEN4 Position       */
#define EPWM_CAPCTL_RCRLDEN4_Msk          (0x1ul << EPWM_CAPCTL_RCRLDEN4_Pos)                /*!< EPWM_T::CAPCTL: RCRLDEN4 Mask           */

#define EPWM_CAPCTL_RCRLDEN5_Pos          (21)                                              /*!< EPWM_T::CAPCTL: RCRLDEN5 Position       */
#define EPWM_CAPCTL_RCRLDEN5_Msk          (0x1ul << EPWM_CAPCTL_RCRLDEN5_Pos)                /*!< EPWM_T::CAPCTL: RCRLDEN5 Mask           */

#define EPWM_CAPCTL_FCRLDEN0_Pos          (24)                                              /*!< EPWM_T::CAPCTL: FCRLDEN0 Position       */
#define EPWM_CAPCTL_FCRLDEN0_Msk          (0x1ul << EPWM_CAPCTL_FCRLDEN0_Pos)                /*!< EPWM_T::CAPCTL: FCRLDEN0 Mask           */

#define EPWM_CAPCTL_FCRLDEN1_Pos          (25)                                              /*!< EPWM_T::CAPCTL: FCRLDEN1 Position       */
#define EPWM_CAPCTL_FCRLDEN1_Msk          (0x1ul << EPWM_CAPCTL_FCRLDEN1_Pos)                /*!< EPWM_T::CAPCTL: FCRLDEN1 Mask           */

#define EPWM_CAPCTL_FCRLDEN2_Pos          (26)                                              /*!< EPWM_T::CAPCTL: FCRLDEN2 Position       */
#define EPWM_CAPCTL_FCRLDEN2_Msk          (0x1ul << EPWM_CAPCTL_FCRLDEN2_Pos)                /*!< EPWM_T::CAPCTL: FCRLDEN2 Mask           */

#define EPWM_CAPCTL_FCRLDEN3_Pos          (27)                                              /*!< EPWM_T::CAPCTL: FCRLDEN3 Position       */
#define EPWM_CAPCTL_FCRLDEN3_Msk          (0x1ul << EPWM_CAPCTL_FCRLDEN3_Pos)                /*!< EPWM_T::CAPCTL: FCRLDEN3 Mask           */

#define EPWM_CAPCTL_FCRLDEN4_Pos          (28)                                              /*!< EPWM_T::CAPCTL: FCRLDEN4 Position       */
#define EPWM_CAPCTL_FCRLDEN4_Msk          (0x1ul << EPWM_CAPCTL_FCRLDEN4_Pos)                /*!< EPWM_T::CAPCTL: FCRLDEN4 Mask           */

#define EPWM_CAPCTL_FCRLDEN5_Pos          (29)                                              /*!< EPWM_T::CAPCTL: FCRLDEN5 Position       */
#define EPWM_CAPCTL_FCRLDEN5_Msk          (0x1ul << EPWM_CAPCTL_FCRLDEN5_Pos)                /*!< EPWM_T::CAPCTL: FCRLDEN5 Mask           */

#define EPWM_CAPSTS_CRLIFOV0_Pos          (0)                                               /*!< EPWM_T::CAPSTS: CRLIFOV0 Position       */
#define EPWM_CAPSTS_CRLIFOV0_Msk          (0x1ul << EPWM_CAPSTS_CRLIFOV0_Pos)                /*!< EPWM_T::CAPSTS: CRLIFOV0 Mask           */

#define EPWM_CAPSTS_CRLIFOV1_Pos          (1)                                               /*!< EPWM_T::CAPSTS: CRLIFOV1 Position       */
#define EPWM_CAPSTS_CRLIFOV1_Msk          (0x1ul << EPWM_CAPSTS_CRLIFOV1_Pos)                /*!< EPWM_T::CAPSTS: CRLIFOV1 Mask           */

#define EPWM_CAPSTS_CRLIFOV2_Pos          (2)                                               /*!< EPWM_T::CAPSTS: CRLIFOV2 Position       */
#define EPWM_CAPSTS_CRLIFOV2_Msk          (0x1ul << EPWM_CAPSTS_CRLIFOV2_Pos)                /*!< EPWM_T::CAPSTS: CRLIFOV2 Mask           */

#define EPWM_CAPSTS_CRLIFOV3_Pos          (3)                                               /*!< EPWM_T::CAPSTS: CRLIFOV3 Position       */
#define EPWM_CAPSTS_CRLIFOV3_Msk          (0x1ul << EPWM_CAPSTS_CRLIFOV3_Pos)                /*!< EPWM_T::CAPSTS: CRLIFOV3 Mask           */

#define EPWM_CAPSTS_CRLIFOV4_Pos          (4)                                               /*!< EPWM_T::CAPSTS: CRLIFOV4 Position       */
#define EPWM_CAPSTS_CRLIFOV4_Msk          (0x1ul << EPWM_CAPSTS_CRLIFOV4_Pos)                /*!< EPWM_T::CAPSTS: CRLIFOV4 Mask           */

#define EPWM_CAPSTS_CRLIFOV5_Pos          (5)                                               /*!< EPWM_T::CAPSTS: CRLIFOV5 Position       */
#define EPWM_CAPSTS_CRLIFOV5_Msk          (0x1ul << EPWM_CAPSTS_CRLIFOV5_Pos)                /*!< EPWM_T::CAPSTS: CRLIFOV5 Mask           */

#define EPWM_CAPSTS_CFLIFOV0_Pos          (8)                                               /*!< EPWM_T::CAPSTS: CFLIFOV0 Position       */
#define EPWM_CAPSTS_CFLIFOV0_Msk          (0x1ul << EPWM_CAPSTS_CFLIFOV0_Pos)                /*!< EPWM_T::CAPSTS: CFLIFOV0 Mask           */

#define EPWM_CAPSTS_CFLIFOV1_Pos          (9)                                               /*!< EPWM_T::CAPSTS: CFLIFOV1 Position       */
#define EPWM_CAPSTS_CFLIFOV1_Msk          (0x1ul << EPWM_CAPSTS_CFLIFOV1_Pos)                /*!< EPWM_T::CAPSTS: CFLIFOV1 Mask           */

#define EPWM_CAPSTS_CFLIFOV2_Pos          (10)                                              /*!< EPWM_T::CAPSTS: CFLIFOV2 Position       */
#define EPWM_CAPSTS_CFLIFOV2_Msk          (0x1ul << EPWM_CAPSTS_CFLIFOV2_Pos)                /*!< EPWM_T::CAPSTS: CFLIFOV2 Mask           */

#define EPWM_CAPSTS_CFLIFOV3_Pos          (11)                                              /*!< EPWM_T::CAPSTS: CFLIFOV3 Position       */
#define EPWM_CAPSTS_CFLIFOV3_Msk          (0x1ul << EPWM_CAPSTS_CFLIFOV3_Pos)                /*!< EPWM_T::CAPSTS: CFLIFOV3 Mask           */

#define EPWM_CAPSTS_CFLIFOV4_Pos          (12)                                              /*!< EPWM_T::CAPSTS: CFLIFOV4 Position       */
#define EPWM_CAPSTS_CFLIFOV4_Msk          (0x1ul << EPWM_CAPSTS_CFLIFOV4_Pos)                /*!< EPWM_T::CAPSTS: CFLIFOV4 Mask           */

#define EPWM_CAPSTS_CFLIFOV5_Pos          (13)                                              /*!< EPWM_T::CAPSTS: CFLIFOV5 Position       */
#define EPWM_CAPSTS_CFLIFOV5_Msk          (0x1ul << EPWM_CAPSTS_CFLIFOV5_Pos)                /*!< EPWM_T::CAPSTS: CFLIFOV5 Mask           */

#define EPWM_RCAPDAT0_RCAPDAT_Pos         (0)                                               /*!< EPWM_T::RCAPDAT0: RCAPDAT Position      */
#define EPWM_RCAPDAT0_RCAPDAT_Msk         (0xfffful << EPWM_RCAPDAT0_RCAPDAT_Pos)            /*!< EPWM_T::RCAPDAT0: RCAPDAT Mask          */

#define EPWM_FCAPDAT0_FCAPDAT_Pos         (0)                                               /*!< EPWM_T::FCAPDAT0: FCAPDAT Position      */
#define EPWM_FCAPDAT0_FCAPDAT_Msk         (0xfffful << EPWM_FCAPDAT0_FCAPDAT_Pos)            /*!< EPWM_T::FCAPDAT0: FCAPDAT Mask          */

#define EPWM_RCAPDAT1_RCAPDAT_Pos         (0)                                               /*!< EPWM_T::RCAPDAT1: RCAPDAT Position      */
#define EPWM_RCAPDAT1_RCAPDAT_Msk         (0xfffful << EPWM_RCAPDAT1_RCAPDAT_Pos)            /*!< EPWM_T::RCAPDAT1: RCAPDAT Mask          */

#define EPWM_FCAPDAT1_FCAPDAT_Pos         (0)                                               /*!< EPWM_T::FCAPDAT1: FCAPDAT Position      */
#define EPWM_FCAPDAT1_FCAPDAT_Msk         (0xfffful << EPWM_FCAPDAT1_FCAPDAT_Pos)            /*!< EPWM_T::FCAPDAT1: FCAPDAT Mask          */

#define EPWM_RCAPDAT2_RCAPDAT_Pos         (0)                                               /*!< EPWM_T::RCAPDAT2: RCAPDAT Position      */
#define EPWM_RCAPDAT2_RCAPDAT_Msk         (0xfffful << EPWM_RCAPDAT2_RCAPDAT_Pos)            /*!< EPWM_T::RCAPDAT2: RCAPDAT Mask          */

#define EPWM_FCAPDAT2_FCAPDAT_Pos         (0)                                               /*!< EPWM_T::FCAPDAT2: FCAPDAT Position      */
#define EPWM_FCAPDAT2_FCAPDAT_Msk         (0xfffful << EPWM_FCAPDAT2_FCAPDAT_Pos)            /*!< EPWM_T::FCAPDAT2: FCAPDAT Mask          */

#define EPWM_RCAPDAT3_RCAPDAT_Pos         (0)                                               /*!< EPWM_T::RCAPDAT3: RCAPDAT Position      */
#define EPWM_RCAPDAT3_RCAPDAT_Msk         (0xfffful << EPWM_RCAPDAT3_RCAPDAT_Pos)            /*!< EPWM_T::RCAPDAT3: RCAPDAT Mask          */

#define EPWM_FCAPDAT3_FCAPDAT_Pos         (0)                                               /*!< EPWM_T::FCAPDAT3: FCAPDAT Position      */
#define EPWM_FCAPDAT3_FCAPDAT_Msk         (0xfffful << EPWM_FCAPDAT3_FCAPDAT_Pos)            /*!< EPWM_T::FCAPDAT3: FCAPDAT Mask          */

#define EPWM_RCAPDAT4_RCAPDAT_Pos         (0)                                               /*!< EPWM_T::RCAPDAT4: RCAPDAT Position      */
#define EPWM_RCAPDAT4_RCAPDAT_Msk         (0xfffful << EPWM_RCAPDAT4_RCAPDAT_Pos)            /*!< EPWM_T::RCAPDAT4: RCAPDAT Mask          */

#define EPWM_FCAPDAT4_FCAPDAT_Pos         (0)                                               /*!< EPWM_T::FCAPDAT4: FCAPDAT Position      */
#define EPWM_FCAPDAT4_FCAPDAT_Msk         (0xfffful << EPWM_FCAPDAT4_FCAPDAT_Pos)            /*!< EPWM_T::FCAPDAT4: FCAPDAT Mask          */

#define EPWM_RCAPDAT5_RCAPDAT_Pos         (0)                                               /*!< EPWM_T::RCAPDAT5: RCAPDAT Position      */
#define EPWM_RCAPDAT5_RCAPDAT_Msk         (0xfffful << EPWM_RCAPDAT5_RCAPDAT_Pos)            /*!< EPWM_T::RCAPDAT5: RCAPDAT Mask          */

#define EPWM_FCAPDAT5_FCAPDAT_Pos         (0)                                               /*!< EPWM_T::FCAPDAT5: FCAPDAT Position      */
#define EPWM_FCAPDAT5_FCAPDAT_Msk         (0xfffful << EPWM_FCAPDAT5_FCAPDAT_Pos)            /*!< EPWM_T::FCAPDAT5: FCAPDAT Mask          */

#define EPWM_PDMACTL_CHEN0_1_Pos          (0)                                               /*!< EPWM_T::PDMACTL: CHEN0_1 Position       */
#define EPWM_PDMACTL_CHEN0_1_Msk          (0x1ul << EPWM_PDMACTL_CHEN0_1_Pos)                /*!< EPWM_T::PDMACTL: CHEN0_1 Mask           */

#define EPWM_PDMACTL_CAPMOD0_1_Pos        (1)                                               /*!< EPWM_T::PDMACTL: CAPMOD0_1 Position     */
#define EPWM_PDMACTL_CAPMOD0_1_Msk        (0x3ul << EPWM_PDMACTL_CAPMOD0_1_Pos)              /*!< EPWM_T::PDMACTL: CAPMOD0_1 Mask         */

#define EPWM_PDMACTL_CAPORD0_1_Pos        (3)                                               /*!< EPWM_T::PDMACTL: CAPORD0_1 Position     */
#define EPWM_PDMACTL_CAPORD0_1_Msk        (0x1ul << EPWM_PDMACTL_CAPORD0_1_Pos)              /*!< EPWM_T::PDMACTL: CAPORD0_1 Mask         */

#define EPWM_PDMACTL_CHSEL0_1_Pos         (4)                                               /*!< EPWM_T::PDMACTL: CHSEL0_1 Position      */
#define EPWM_PDMACTL_CHSEL0_1_Msk         (0x1ul << EPWM_PDMACTL_CHSEL0_1_Pos)               /*!< EPWM_T::PDMACTL: CHSEL0_1 Mask          */

#define EPWM_PDMACTL_CHEN2_3_Pos          (8)                                               /*!< EPWM_T::PDMACTL: CHEN2_3 Position       */
#define EPWM_PDMACTL_CHEN2_3_Msk          (0x1ul << EPWM_PDMACTL_CHEN2_3_Pos)                /*!< EPWM_T::PDMACTL: CHEN2_3 Mask           */

#define EPWM_PDMACTL_CAPMOD2_3_Pos        (9)                                               /*!< EPWM_T::PDMACTL: CAPMOD2_3 Position     */
#define EPWM_PDMACTL_CAPMOD2_3_Msk        (0x3ul << EPWM_PDMACTL_CAPMOD2_3_Pos)              /*!< EPWM_T::PDMACTL: CAPMOD2_3 Mask         */

#define EPWM_PDMACTL_CAPORD2_3_Pos        (11)                                              /*!< EPWM_T::PDMACTL: CAPORD2_3 Position     */
#define EPWM_PDMACTL_CAPORD2_3_Msk        (0x1ul << EPWM_PDMACTL_CAPORD2_3_Pos)              /*!< EPWM_T::PDMACTL: CAPORD2_3 Mask         */

#define EPWM_PDMACTL_CHSEL2_3_Pos         (12)                                              /*!< EPWM_T::PDMACTL: CHSEL2_3 Position      */
#define EPWM_PDMACTL_CHSEL2_3_Msk         (0x1ul << EPWM_PDMACTL_CHSEL2_3_Pos)               /*!< EPWM_T::PDMACTL: CHSEL2_3 Mask          */

#define EPWM_PDMACTL_CHEN4_5_Pos          (16)                                              /*!< EPWM_T::PDMACTL: CHEN4_5 Position       */
#define EPWM_PDMACTL_CHEN4_5_Msk          (0x1ul << EPWM_PDMACTL_CHEN4_5_Pos)                /*!< EPWM_T::PDMACTL: CHEN4_5 Mask           */

#define EPWM_PDMACTL_CAPMOD4_5_Pos        (17)                                              /*!< EPWM_T::PDMACTL: CAPMOD4_5 Position     */
#define EPWM_PDMACTL_CAPMOD4_5_Msk        (0x3ul << EPWM_PDMACTL_CAPMOD4_5_Pos)              /*!< EPWM_T::PDMACTL: CAPMOD4_5 Mask         */

#define EPWM_PDMACTL_CAPORD4_5_Pos        (19)                                              /*!< EPWM_T::PDMACTL: CAPORD4_5 Position     */
#define EPWM_PDMACTL_CAPORD4_5_Msk        (0x1ul << EPWM_PDMACTL_CAPORD4_5_Pos)              /*!< EPWM_T::PDMACTL: CAPORD4_5 Mask         */

#define EPWM_PDMACTL_CHSEL4_5_Pos         (20)                                              /*!< EPWM_T::PDMACTL: CHSEL4_5 Position      */
#define EPWM_PDMACTL_CHSEL4_5_Msk         (0x1ul << EPWM_PDMACTL_CHSEL4_5_Pos)               /*!< EPWM_T::PDMACTL: CHSEL4_5 Mask          */

#define EPWM_PDMACAP0_1_CAPBUF_Pos        (0)                                               /*!< EPWM_T::PDMACAP0_1: CAPBUF Position     */
#define EPWM_PDMACAP0_1_CAPBUF_Msk        (0xfffful << EPWM_PDMACAP0_1_CAPBUF_Pos)           /*!< EPWM_T::PDMACAP0_1: CAPBUF Mask         */

#define EPWM_PDMACAP2_3_CAPBUF_Pos        (0)                                               /*!< EPWM_T::PDMACAP2_3: CAPBUF Position     */
#define EPWM_PDMACAP2_3_CAPBUF_Msk        (0xfffful << EPWM_PDMACAP2_3_CAPBUF_Pos)           /*!< EPWM_T::PDMACAP2_3: CAPBUF Mask         */

#define EPWM_PDMACAP4_5_CAPBUF_Pos        (0)                                               /*!< EPWM_T::PDMACAP4_5: CAPBUF Position     */
#define EPWM_PDMACAP4_5_CAPBUF_Msk        (0xfffful << EPWM_PDMACAP4_5_CAPBUF_Pos)           /*!< EPWM_T::PDMACAP4_5: CAPBUF Mask         */

#define EPWM_CAPIEN_CAPRIEN0_Pos          (0)                                               /*!< EPWM_T::CAPIEN: CAPRIEN0 Position       */
#define EPWM_CAPIEN_CAPRIEN0_Msk          (0x1ul << EPWM_CAPIEN_CAPRIEN0_Pos)                /*!< EPWM_T::CAPIEN: CAPRIEN0 Mask           */

#define EPWM_CAPIEN_CAPRIEN1_Pos          (1)                                               /*!< EPWM_T::CAPIEN: CAPRIEN1 Position       */
#define EPWM_CAPIEN_CAPRIEN1_Msk          (0x1ul << EPWM_CAPIEN_CAPRIEN1_Pos)                /*!< EPWM_T::CAPIEN: CAPRIEN1 Mask           */

#define EPWM_CAPIEN_CAPRIEN2_Pos          (2)                                               /*!< EPWM_T::CAPIEN: CAPRIEN2 Position       */
#define EPWM_CAPIEN_CAPRIEN2_Msk          (0x1ul << EPWM_CAPIEN_CAPRIEN2_Pos)                /*!< EPWM_T::CAPIEN: CAPRIEN2 Mask           */

#define EPWM_CAPIEN_CAPRIEN3_Pos          (3)                                               /*!< EPWM_T::CAPIEN: CAPRIEN3 Position       */
#define EPWM_CAPIEN_CAPRIEN3_Msk          (0x1ul << EPWM_CAPIEN_CAPRIEN3_Pos)                /*!< EPWM_T::CAPIEN: CAPRIEN3 Mask           */

#define EPWM_CAPIEN_CAPRIEN4_Pos          (4)                                               /*!< EPWM_T::CAPIEN: CAPRIEN4 Position       */
#define EPWM_CAPIEN_CAPRIEN4_Msk          (0x1ul << EPWM_CAPIEN_CAPRIEN4_Pos)                /*!< EPWM_T::CAPIEN: CAPRIEN4 Mask           */

#define EPWM_CAPIEN_CAPRIEN5_Pos          (5)                                               /*!< EPWM_T::CAPIEN: CAPRIEN5 Position       */
#define EPWM_CAPIEN_CAPRIEN5_Msk          (0x1ul << EPWM_CAPIEN_CAPRIEN5_Pos)                /*!< EPWM_T::CAPIEN: CAPRIEN5 Mask           */

#define EPWM_CAPIEN_CAPFIEN0_Pos          (8)                                               /*!< EPWM_T::CAPIEN: CAPFIEN0 Position       */
#define EPWM_CAPIEN_CAPFIEN0_Msk          (0x1ul << EPWM_CAPIEN_CAPFIEN0_Pos)                /*!< EPWM_T::CAPIEN: CAPFIEN0 Mask           */

#define EPWM_CAPIEN_CAPFIEN1_Pos          (9)                                               /*!< EPWM_T::CAPIEN: CAPFIEN1 Position       */
#define EPWM_CAPIEN_CAPFIEN1_Msk          (0x1ul << EPWM_CAPIEN_CAPFIEN1_Pos)                /*!< EPWM_T::CAPIEN: CAPFIEN1 Mask           */

#define EPWM_CAPIEN_CAPFIEN2_Pos          (10)                                              /*!< EPWM_T::CAPIEN: CAPFIEN2 Position       */
#define EPWM_CAPIEN_CAPFIEN2_Msk          (0x1ul << EPWM_CAPIEN_CAPFIEN2_Pos)                /*!< EPWM_T::CAPIEN: CAPFIEN2 Mask           */

#define EPWM_CAPIEN_CAPFIEN3_Pos          (11)                                              /*!< EPWM_T::CAPIEN: CAPFIEN3 Position       */
#define EPWM_CAPIEN_CAPFIEN3_Msk          (0x1ul << EPWM_CAPIEN_CAPFIEN3_Pos)                /*!< EPWM_T::CAPIEN: CAPFIEN3 Mask           */

#define EPWM_CAPIEN_CAPFIEN4_Pos          (12)                                              /*!< EPWM_T::CAPIEN: CAPFIEN4 Position       */
#define EPWM_CAPIEN_CAPFIEN4_Msk          (0x1ul << EPWM_CAPIEN_CAPFIEN4_Pos)                /*!< EPWM_T::CAPIEN: CAPFIEN4 Mask           */

#define EPWM_CAPIEN_CAPFIEN5_Pos          (13)                                              /*!< EPWM_T::CAPIEN: CAPFIEN5 Position       */
#define EPWM_CAPIEN_CAPFIEN5_Msk          (0x1ul << EPWM_CAPIEN_CAPFIEN5_Pos)                /*!< EPWM_T::CAPIEN: CAPFIEN5 Mask           */

#define EPWM_CAPIF_CRLIF0_Pos             (0)                                               /*!< EPWM_T::CAPIF: CRLIF0 Position          */
#define EPWM_CAPIF_CRLIF0_Msk             (0x1ul << EPWM_CAPIF_CRLIF0_Pos)                   /*!< EPWM_T::CAPIF: CRLIF0 Mask              */

#define EPWM_CAPIF_CRLIF1_Pos             (1)                                               /*!< EPWM_T::CAPIF: CRLIF1 Position          */
#define EPWM_CAPIF_CRLIF1_Msk             (0x1ul << EPWM_CAPIF_CRLIF1_Pos)                   /*!< EPWM_T::CAPIF: CRLIF1 Mask              */

#define EPWM_CAPIF_CRLIF2_Pos             (2)                                               /*!< EPWM_T::CAPIF: CRLIF2 Position          */
#define EPWM_CAPIF_CRLIF2_Msk             (0x1ul << EPWM_CAPIF_CRLIF2_Pos)                   /*!< EPWM_T::CAPIF: CRLIF2 Mask              */

#define EPWM_CAPIF_CRLIF3_Pos             (3)                                               /*!< EPWM_T::CAPIF: CRLIF3 Position          */
#define EPWM_CAPIF_CRLIF3_Msk             (0x1ul << EPWM_CAPIF_CRLIF3_Pos)                   /*!< EPWM_T::CAPIF: CRLIF3 Mask              */

#define EPWM_CAPIF_CRLIF4_Pos             (4)                                               /*!< EPWM_T::CAPIF: CRLIF4 Position          */
#define EPWM_CAPIF_CRLIF4_Msk             (0x1ul << EPWM_CAPIF_CRLIF4_Pos)                   /*!< EPWM_T::CAPIF: CRLIF4 Mask              */

#define EPWM_CAPIF_CRLIF5_Pos             (5)                                               /*!< EPWM_T::CAPIF: CRLIF5 Position          */
#define EPWM_CAPIF_CRLIF5_Msk             (0x1ul << EPWM_CAPIF_CRLIF5_Pos)                   /*!< EPWM_T::CAPIF: CRLIF5 Mask              */

#define EPWM_CAPIF_CFLIF0_Pos             (8)                                               /*!< EPWM_T::CAPIF: CFLIF0 Position          */
#define EPWM_CAPIF_CFLIF0_Msk             (0x1ul << EPWM_CAPIF_CFLIF0_Pos)                   /*!< EPWM_T::CAPIF: CFLIF0 Mask              */

#define EPWM_CAPIF_CFLIF1_Pos             (9)                                               /*!< EPWM_T::CAPIF: CFLIF1 Position          */
#define EPWM_CAPIF_CFLIF1_Msk             (0x1ul << EPWM_CAPIF_CFLIF1_Pos)                   /*!< EPWM_T::CAPIF: CFLIF1 Mask              */

#define EPWM_CAPIF_CFLIF2_Pos             (10)                                              /*!< EPWM_T::CAPIF: CFLIF2 Position          */
#define EPWM_CAPIF_CFLIF2_Msk             (0x1ul << EPWM_CAPIF_CFLIF2_Pos)                   /*!< EPWM_T::CAPIF: CFLIF2 Mask              */

#define EPWM_CAPIF_CFLIF3_Pos             (11)                                              /*!< EPWM_T::CAPIF: CFLIF3 Position          */
#define EPWM_CAPIF_CFLIF3_Msk             (0x1ul << EPWM_CAPIF_CFLIF3_Pos)                   /*!< EPWM_T::CAPIF: CFLIF3 Mask              */

#define EPWM_CAPIF_CFLIF4_Pos             (12)                                              /*!< EPWM_T::CAPIF: CFLIF4 Position          */
#define EPWM_CAPIF_CFLIF4_Msk             (0x1ul << EPWM_CAPIF_CFLIF4_Pos)                   /*!< EPWM_T::CAPIF: CFLIF4 Mask              */

#define EPWM_CAPIF_CFLIF5_Pos             (13)                                              /*!< EPWM_T::CAPIF: CFLIF5 Position          */
#define EPWM_CAPIF_CFLIF5_Msk             (0x1ul << EPWM_CAPIF_CFLIF5_Pos)                   /*!< EPWM_T::CAPIF: CFLIF5 Mask              */

#define EPWM_PBUF0_PBUF_Pos               (0)                                               /*!< EPWM_T::PBUF0: PBUF Position            */
#define EPWM_PBUF0_PBUF_Msk               (0xfffful << EPWM_PBUF0_PBUF_Pos)                  /*!< EPWM_T::PBUF0: PBUF Mask                */

#define EPWM_PBUF1_PBUF_Pos               (0)                                               /*!< EPWM_T::PBUF1: PBUF Position            */
#define EPWM_PBUF1_PBUF_Msk               (0xfffful << EPWM_PBUF1_PBUF_Pos)                  /*!< EPWM_T::PBUF1: PBUF Mask                */

#define EPWM_PBUF2_PBUF_Pos               (0)                                               /*!< EPWM_T::PBUF2: PBUF Position            */
#define EPWM_PBUF2_PBUF_Msk               (0xfffful << EPWM_PBUF2_PBUF_Pos)                  /*!< EPWM_T::PBUF2: PBUF Mask                */

#define EPWM_PBUF3_PBUF_Pos               (0)                                               /*!< EPWM_T::PBUF3: PBUF Position            */
#define EPWM_PBUF3_PBUF_Msk               (0xfffful << EPWM_PBUF3_PBUF_Pos)                  /*!< EPWM_T::PBUF3: PBUF Mask                */

#define EPWM_PBUF4_PBUF_Pos               (0)                                               /*!< EPWM_T::PBUF4: PBUF Position            */
#define EPWM_PBUF4_PBUF_Msk               (0xfffful << EPWM_PBUF4_PBUF_Pos)                  /*!< EPWM_T::PBUF4: PBUF Mask                */

#define EPWM_PBUF5_PBUF_Pos               (0)                                               /*!< EPWM_T::PBUF5: PBUF Position            */
#define EPWM_PBUF5_PBUF_Msk               (0xfffful << EPWM_PBUF5_PBUF_Pos)                  /*!< EPWM_T::PBUF5: PBUF Mask                */

#define EPWM_CMPBUF0_CMPBUF_Pos           (0)                                               /*!< EPWM_T::CMPBUF0: CMPBUF Position        */
#define EPWM_CMPBUF0_CMPBUF_Msk           (0xfffful << EPWM_CMPBUF0_CMPBUF_Pos)              /*!< EPWM_T::CMPBUF0: CMPBUF Mask            */

#define EPWM_CMPBUF1_CMPBUF_Pos           (0)                                               /*!< EPWM_T::CMPBUF1: CMPBUF Position        */
#define EPWM_CMPBUF1_CMPBUF_Msk           (0xfffful << EPWM_CMPBUF1_CMPBUF_Pos)              /*!< EPWM_T::CMPBUF1: CMPBUF Mask            */

#define EPWM_CMPBUF2_CMPBUF_Pos           (0)                                               /*!< EPWM_T::CMPBUF2: CMPBUF Position        */
#define EPWM_CMPBUF2_CMPBUF_Msk           (0xfffful << EPWM_CMPBUF2_CMPBUF_Pos)              /*!< EPWM_T::CMPBUF2: CMPBUF Mask            */

#define EPWM_CMPBUF3_CMPBUF_Pos           (0)                                               /*!< EPWM_T::CMPBUF3: CMPBUF Position        */
#define EPWM_CMPBUF3_CMPBUF_Msk           (0xfffful << EPWM_CMPBUF3_CMPBUF_Pos)              /*!< EPWM_T::CMPBUF3: CMPBUF Mask            */

#define EPWM_CMPBUF4_CMPBUF_Pos           (0)                                               /*!< EPWM_T::CMPBUF4: CMPBUF Position        */
#define EPWM_CMPBUF4_CMPBUF_Msk           (0xfffful << EPWM_CMPBUF4_CMPBUF_Pos)              /*!< EPWM_T::CMPBUF4: CMPBUF Mask            */

#define EPWM_CMPBUF5_CMPBUF_Pos           (0)                                               /*!< EPWM_T::CMPBUF5: CMPBUF Position        */
#define EPWM_CMPBUF5_CMPBUF_Msk           (0xfffful << EPWM_CMPBUF5_CMPBUF_Pos)              /*!< EPWM_T::CMPBUF5: CMPBUF Mask            */

#define EPWM_CPSCBUF0_1_CPSCBUF_Pos       (0)                                               /*!< EPWM_T::CPSCBUF0_1: CPSCBUF Position    */
#define EPWM_CPSCBUF0_1_CPSCBUF_Msk       (0xffful << EPWM_CPSCBUF0_1_CPSCBUF_Pos)           /*!< EPWM_T::CPSCBUF0_1: CPSCBUF Mask        */

#define EPWM_CPSCBUF2_3_CPSCBUF_Pos       (0)                                               /*!< EPWM_T::CPSCBUF2_3: CPSCBUF Position    */
#define EPWM_CPSCBUF2_3_CPSCBUF_Msk       (0xffful << EPWM_CPSCBUF2_3_CPSCBUF_Pos)           /*!< EPWM_T::CPSCBUF2_3: CPSCBUF Mask        */

#define EPWM_CPSCBUF4_5_CPSCBUF_Pos       (0)                                               /*!< EPWM_T::CPSCBUF4_5: CPSCBUF Position    */
#define EPWM_CPSCBUF4_5_CPSCBUF_Msk       (0xffful << EPWM_CPSCBUF4_5_CPSCBUF_Pos)           /*!< EPWM_T::CPSCBUF4_5: CPSCBUF Mask        */

#define EPWM_FTCBUF0_1_FTCMPBUF_Pos       (0)                                               /*!< EPWM_T::FTCBUF0_1: FTCMPBUF Position    */
#define EPWM_FTCBUF0_1_FTCMPBUF_Msk       (0xfffful << EPWM_FTCBUF0_1_FTCMPBUF_Pos)          /*!< EPWM_T::FTCBUF0_1: FTCMPBUF Mask        */

#define EPWM_FTCBUF2_3_FTCMPBUF_Pos       (0)                                               /*!< EPWM_T::FTCBUF2_3: FTCMPBUF Position    */
#define EPWM_FTCBUF2_3_FTCMPBUF_Msk       (0xfffful << EPWM_FTCBUF2_3_FTCMPBUF_Pos)          /*!< EPWM_T::FTCBUF2_3: FTCMPBUF Mask        */

#define EPWM_FTCBUF4_5_FTCMPBUF_Pos       (0)                                               /*!< EPWM_T::FTCBUF4_5: FTCMPBUF Position    */
#define EPWM_FTCBUF4_5_FTCMPBUF_Msk       (0xfffful << EPWM_FTCBUF4_5_FTCMPBUF_Pos)          /*!< EPWM_T::FTCBUF4_5: FTCMPBUF Mask        */

#define EPWM_FTCI_FTCMU0_Pos              (0)                                               /*!< EPWM_T::FTCI: FTCMU0 Position           */
#define EPWM_FTCI_FTCMU0_Msk              (0x1ul << EPWM_FTCI_FTCMU0_Pos)                    /*!< EPWM_T::FTCI: FTCMU0 Mask               */

#define EPWM_FTCI_FTCMU2_Pos              (1)                                               /*!< EPWM_T::FTCI: FTCMU2 Position           */
#define EPWM_FTCI_FTCMU2_Msk              (0x1ul << EPWM_FTCI_FTCMU2_Pos)                    /*!< EPWM_T::FTCI: FTCMU2 Mask               */

#define EPWM_FTCI_FTCMU4_Pos              (2)                                               /*!< EPWM_T::FTCI: FTCMU4 Position           */
#define EPWM_FTCI_FTCMU4_Msk              (0x1ul << EPWM_FTCI_FTCMU4_Pos)                    /*!< EPWM_T::FTCI: FTCMU4 Mask               */

#define EPWM_FTCI_FTCMD0_Pos              (8)                                               /*!< EPWM_T::FTCI: FTCMD0 Position           */
#define EPWM_FTCI_FTCMD0_Msk              (0x1ul << EPWM_FTCI_FTCMD0_Pos)                    /*!< EPWM_T::FTCI: FTCMD0 Mask               */

#define EPWM_FTCI_FTCMD2_Pos              (9)                                               /*!< EPWM_T::FTCI: FTCMD2 Position           */
#define EPWM_FTCI_FTCMD2_Msk              (0x1ul << EPWM_FTCI_FTCMD2_Pos)                    /*!< EPWM_T::FTCI: FTCMD2 Mask               */

#define EPWM_FTCI_FTCMD4_Pos              (10)                                              /*!< EPWM_T::FTCI: FTCMD4 Position           */
#define EPWM_FTCI_FTCMD4_Msk              (0x1ul << EPWM_FTCI_FTCMD4_Pos)                    /*!< EPWM_T::FTCI: FTCMD4 Mask               */

/**@}*/ /* EPWM_CONST */
/**@}*/ /* end of EPWM register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __EPWM_REG_H__ */
