/**************************************************************************//**
 * @file     timer_reg.h
 * @version  V1.00
 * @brief    TIMER register definition header file
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#ifndef __TIMER_REG_H__
#define __TIMER_REG_H__

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/**
   @addtogroup REGISTER Control Register
   @{
*/

/**
    @addtogroup TIMER Timer Controller(TIMER)
    Memory Mapped Structure for TIMER Controller
@{ */

typedef struct
{


    /**
     * @var TIMER_T::CTL
     * Offset: 0x00  Timer Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[7:0]   |PSC       |Prescale Counter
     * |        |          |Timer input clock or event source is divided by (PSC+1) before it is fed to the timer up counter
     * |        |          |If this field is 0 (PSC = 0), then there is no scaling.
     * |        |          |Note: Update prescale counter value will reset internal 8-bit prescale counter and 24-bit up counter value.
     * |[19]    |INTRGEN   |Inter-timer Trigger Mode Enable Control
     * |        |          |Setting this bit will enable the inter-timer trigger capture function.
     * |        |          |The Timer0/2 will be in event counter mode and counting with external clock source or event
     * |        |          |Also, Timer1/3 will be in trigger-counting mode of capture function.
     * |        |          |0 = Inter-Timer Trigger Capture mode Disabled.
     * |        |          |1 = Inter-Timer Trigger Capture mode Enabled.
     * |        |          |Note: For Timer1/3, this bit is ignored and the read back value is always 0.
     * |[20]    |PERIOSEL  |Periodic Mode Behavior Selection Enable Bit
     * |        |          |0 = The behavior selection in periodic mode is Disabled.
     * |        |          |When user updates CMPDAT while timer is running in periodic mode,
     * |        |          |CNT will be reset to default value.
     * |        |          |1 = The behavior selection in periodic mode is Enabled.
     * |        |          |When user update CMPDAT while timer is running in periodic mode, the limitations as bellows list,
     * |        |          |If updated CMPDAT value > CNT, CMPDAT will be updated and CNT keep running continually.
     * |        |          |If updated CMPDAT value = CNT, timer time-out interrupt will be asserted immediately.
     * |        |          |If updated CMPDAT value < CNT, CNT will be reset to default value.
     * |[21]    |TGLPINSEL |Toggle-output Pin Select
     * |        |          |0 = Toggle mode output to TMx (Timer Event Counter Pin).
     * |        |          |1 = Toggle mode output to TMx_EXT (Timer External Capture Pin).
     * |[22]    |CAPSRC    |Capture Pin Source Selection
     * |        |          |0 = Capture Function source is from TMx_EXT (x= 0~3) pin.
     * |        |          |1 = Capture Function source is from internal ACMP output signal
     * |        |          |User can set ACMPSSEL (TIMERx_EXTCTL[8]) to decide which internal ACMP output signal as timer capture source.
     * |[23]    |WKEN      |Wake-up Function Enable Bit
     * |        |          |If this bit is set to 1, while timer interrupt flag TIF (TIMERx_INTSTS[0]) is 1 and INTEN (TIMERx_CTL[29]) is enabled, the timer interrupt signal will generate a wake-up trigger event to CPU.
     * |        |          |0 = Wake-up function Disabled if timer interrupt signal generated.
     * |        |          |1 = Wake-up function Enabled if timer interrupt signal generated.
     * |[24]    |EXTCNTEN  |Event Counter Mode Enable Bit
     * |        |          |This bit is for external counting pin function enabled.
     * |        |          |0 = Event counter mode Disabled.
     * |        |          |1 = Event counter mode Enabled.
     * |        |          |Note: When timer is used as an event counter, this bit should be set to 1 and select PCLK as timer clock source.
     * |[25]    |ACTSTS    |Timer Active Status Bit (Read Only)
     * |        |          |This bit indicates the 24-bit up counter status.
     * |        |          |0 = 24-bit up counter is not active.
     * |        |          |1 = 24-bit up counter is active.
     * |        |          |Note: This bit may active when CNT 0 transition to CNT 1.
     * |[28:27] |OPMODE    |Timer Counting Mode Select
     * |        |          |00 = The Timer controller is operated in One-shot mode.
     * |        |          |01 = The Timer controller is operated in Periodic mode.
     * |        |          |10 = The Timer controller is operated in Toggle-output mode.
     * |        |          |11 = The Timer controller is operated in Continuous Counting mode.
     * |[29]    |INTEN     |Timer Interrupt Enable Bit
     * |        |          |0 = Timer time-out interrupt Disabled.
     * |        |          |1 = Timer time-out interrupt Enabled.
     * |        |          |Note: If this bit is enabled, when the timer time-out interrupt flag TIF is set to 1, the timer interrupt signal is generated and inform to CPU.
     * |[30]    |CNTEN     |Timer Counting Enable Bit
     * |        |          |0 = Stops/Suspends counting.
     * |        |          |1 = Starts counting.
     * |        |          |Note1: In stop status, and then set CNTEN to 1 will enable the 24-bit up counter to keep counting from the last stop counting value.
     * |        |          |Note2: This bit is auto-cleared by hardware in one-shot mode (TIMER_CTL[28:27] = 00) when the timer time-out interrupt flag TIF (TIMERx_INTSTS[0]) is generated.
     * |        |          |Note3: Set enable/disable this bit needs 2 * TMR_CLK period to become active, user can read ACTSTS (TIMERx_CTL[25]) to check enable/disable command is completed or not.
     * |[31]    |ICEDEBUG  |ICE Debug Mode Acknowledge Disable Control (Write Protect)
     * |        |          |0 = ICE debug mode acknowledgement effects TIMER counting.
     * |        |          |TIMER counter will be held while CPU is held by ICE.
     * |        |          |1 = ICE debug mode acknowledgement Disabled.
     * |        |          |TIMER counter will keep going no matter CPU is held by ICE or not.
     * |        |          |Note: This bit is write protected. Refer to the SYS_REGLCTL register.
     * @var TIMER_T::CMP
     * Offset: 0x04  Timer Comparator Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |CMPDAT    |Timer Comparator Value
     * |        |          |CMPDAT is a 24-bit compared value register
     * |        |          |When the internal 24-bit up counter value is equal to CMPDAT value, the TIF (TIMERx_INTSTS[0] Timer Interrupt Flag) will set to 1.
     * |        |          |Time-out period = (Period of timer clock input) * (8-bit PSC + 1) * (24-bit CMPDAT).
     * |        |          |Note1: Never write 0x0 or 0x1 in CMPDAT field, or the core will run into unknown state.
     * |        |          |Note2: When timer is operating at continuous counting mode, the 24-bit up counter will keep counting continuously even if user writes a new value into CMPDAT field
     * |        |          |But if timer is operating at other modes, the 24-bit up counter will restart counting from 0 and using newest CMPDAT value to be the timer compared value while user writes a new value into CMPDAT field.
     * @var TIMER_T::INTSTS
     * Offset: 0x08  Timer Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TIF       |Timer Interrupt Flag
     * |        |          |This bit indicates the interrupt flag status of Timer while 24-bit timer up counter CNT (TIMERx_CNT[23:0]) value reaches to CMPDAT (TIMERx_CMP[23:0]) value.
     * |        |          |0 = No effect.
     * |        |          |1 = CNT value matches the CMPDAT value.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[1]     |TWKF      |Timer Wake-up Flag
     * |        |          |This bit indicates the interrupt wake-up flag status of timer.
     * |        |          |0 = Timer does not cause CPU wake-up.
     * |        |          |1 = CPU wake-up from Idle or Power-down mode if timer time-out interrupt signal generated.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * @var TIMER_T::CNT
     * Offset: 0x0C  Timer Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |CNT       |Timer Data Register
     * |        |          |Read operation.
     * |        |          |Read this register to get CNT value. For example:
     * |        |          |If EXTCNTEN (TIMERx_CTL[24] ) is 0, user can read CNT value for getting current 24-bit counter value.
     * |        |          |If EXTCNTEN (TIMERx_CTL[24] ) is 1, user can read CNT value for getting current 24-bit event input counter value.
     * |        |          |Write operation.
     * |        |          |Writing any value to this register will reset current CNT value to 0 and reload internal 8-bit prescale counter.
     * |[31]    |RSTACT    |Timer Data Register Reset Active (Read Only)
     * |        |          |This bit indicates if the counter reset operation active.
     * |        |          |When user writes this CNT register, timer starts to reset its internal 24-bit timer up-counter to 0 and reload 8-bit pre-scale counter
     * |        |          |At the same time, timer set this flag to 1 to indicate the counter reset operation is in progress
     * |        |          |Once the counter reset operation done, timer clear this bit to 0 automatically.
     * |        |          |0 = Reset operation is done.
     * |        |          |1 = Reset operation triggered by writing TIMERx_CNT is in progress.
     * |        |          |Note: This bit is read only.
     * @var TIMER_T::CAP
     * Offset: 0x10  Timer Capture Data Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[23:0]  |CAPDAT    |Timer Capture Data Register
     * |        |          |When CAPEN (TIMERx_EXTCTL[3]) bit is set, CAPFUNCS (TIMERx_EXTCTL[4]) bit is 0, and a transition on TMx_EXT pin matched the CAPEDGE (TIMERx_EXTCTL[14:12]) setting, CAPIF (TIMERx_EINTSTS[0]) will set to 1 and the current timer counter value CNT (TIMERx_CNT[23:0]) will be auto-loaded into this CAPDAT field.
     * @var TIMER_T::EXTCTL
     * Offset: 0x14  Timer External Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTPHASE  |Timer External Count Phase
     * |        |          |This bit indicates the detection phase of external counting pin TMx (x= 0~3).
     * |        |          |0 = A falling edge of external counting pin will be counted.
     * |        |          |1 = A rising edge of external counting pin will be counted.
     * |[3]     |CAPEN     |Timer External Capture Pin Enable Bit
     * |        |          |This bit enables the TMx_EXT capture pin input function.
     * |        |          |0 =TMx_EXT (x= 0~3) pin Disabled.
     * |        |          |1 =TMx_EXT (x= 0~3) pin Enabled.
     * |[4]     |CAPFUNCS  |Capture Function Selection
     * |        |          |0 = External Capture Mode Enabled.
     * |        |          |1 = External Reset Mode Enabled.
     * |        |          |Note1: When CAPFUNCS is 0, transition on TMx_EXT (x= 0~3) pin is using to save current 24-bit timer counter value (CNT value) to CAPDAT field.
     * |        |          |Note2: When CAPFUNCS is 1, transition on TMx_EXT (x= 0~3) pin is using to save current 24-bit timer counter value (CNT value) to CAPDAT field then CNT value will be reset immediately.
     * |[5]     |CAPIEN    |Timer External Capture Interrupt Enable Bit
     * |        |          |0 = TMx_EXT (x= 0~3) pin detection Interrupt Disabled.
     * |        |          |1 = TMx_EXT (x= 0~3) pin detection Interrupt Enabled.
     * |        |          |Note: CAPIEN is used to enable timer external interrupt
     * |        |          |If CAPIEN enabled, timer will rise an interrupt when CAPIF (TIMERx_EINTSTS[0]) is 1.
     * |        |          |For example, while CAPIEN = 1, CAPEN = 1, and CAPEDGE = 00, a 1 to 0 transition on the TMx_EXT pin will cause the CAPIF to be set then the interrupt signal is generated and sent to NVIC to inform CPU.
     * |[6]     |CAPDBEN   |Timer External Capture Pin De-bounce Enable Bit
     * |        |          |0 = TMx_EXT (x= 0~3) pin de-bounce or ACMP output de-bounce Disabled.
     * |        |          |1 = TMx_EXT (x= 0~3) pin de-bounce or ACMP output de-bounce Enabled.
     * |        |          |Note: If this bit is enabled, the edge detection of TMx_EXT pin or ACMP output is detected with de-bounce circuit.
     * |[7]     |CNTDBEN   |Timer Counter Pin De-bounce Enable Bit
     * |        |          |0 = TMx (x= 0~3) pin de-bounce Disabled.
     * |        |          |1 = TMx (x= 0~3) pin de-bounce Enabled.
     * |        |          |Note: If this bit is enabled, the edge detection of TMx pin is detected with de-bounce circuit.
     * |[8:10]  |ICAPSEL   |Internal Capture Source Select
     * |        |          |000 = Capture Function source is from internal ACMP0 output signal.
     * |        |          |001 = Capture Function source is from internal ACMP1 output signal.
     * |        |          |010 = Capture Function source is from HXT.
     * |        |          |011 = Capture Function source is from LXT.
     * |        |          |100 = Capture Function source is from HIRC.
     * |        |          |101 = Capture Function source is from LIRC.
     * |        |          |110 = Reserved.
     * |        |          |111 = Reserved.
     * |        |          |Note: these bits only available when CAPSRC (TIMERx_CTL[22]) is 1.
     * |[14:12] |CAPEDGE   |Timer External Capture Pin Edge Detect
     * |        |          |When first capture event is generated, the CNT (TIMERx_CNT[23:0]) will be reset to 0 and first CAPDAT (TIMERx_CAP[23:0]) should be to 0.
     * |        |          |000 = Capture event occurred when detect falling edge transfer on TMx_EXT (x= 0~3) pin.
     * |        |          |001 = Capture event occurred when detect rising edge transfer on TMx_EXT (x= 0~3) pin.
     * |        |          |010 = Capture event occurred when detect both falling and rising edge transfer on TMx_EXT (x= 0~3) pin, and first capture event occurred at falling edge transfer.
     * |        |          |011 = Capture event occurred when detect both rising and falling edge transfer on TMx_EXT (x= 0~3) pin, and first capture event occurred at rising edge transfer..
     * |        |          |110 = First capture event occurred at falling edge, follows capture events are at rising edge transfer on TMx_EXT (x= 0~3) pin.
     * |        |          |111 = First capture event occurred at rising edge, follows capture events are at falling edge transfer on TMx_EXT (x= 0~3) pin.
     * |        |          |100, 101 = Reserved.
     * |[16]    |ECNTSSEL  |Event Counter Source Selection to Trigger Event Counter Function
     * |        |          |0 = Event Counter input source is from TMx (x= 0~3) pin.
     * |        |          |1 = Event Counter input source is from USB internal SOF output signal.
     * |[31:28] |CAPDIVSCL |Timer Capture Source Divider
     * |        |          |This bits indicate the divide scale for capture source divider
     * |        |          |0000 = Capture source/1.
     * |        |          |0001 = Capture source/2.
     * |        |          |0010 = Capture source/4.
     * |        |          |0011 = Capture source/8.
     * |        |          |0100 = Capture source/16.
     * |        |          |0101 = Capture source/32.
     * |        |          |0110 = Capture source/64.
     * |        |          |0111 = Capture source/128.
     * |        |          |1000 = Capture source/256.
     * |        |          |1001~1111 = Reserved.
     * |        |          |Note: Sets INTERCAPSEL (TIMERx_EXTCTL[10:8]) and CAPSRC (TIMERx_CTL[22]) to select capture source.     * @var TIMER_T::EINTSTS
     * Offset: 0x18  Timer External Interrupt Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CAPIF     |Timer External Capture Interrupt Flag
     * |        |          |This bit indicates the timer external capture interrupt flag status.
     * |        |          |0 = TMx_EXT (x= 0~3) pin interrupt did not occur.
     * |        |          |1 = TMx_EXT (x= 0~3) pin interrupt occurred.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2: When CAPEN (TIMERx_EXTCTL[3]) bit is set, CAPFUNCS (TIMERx_EXTCTL[4]) bit is 0, and a transition on TMx_EXT (x= 0~3) pin matched the CAPEDGE (TIMERx_EXTCTL[2:1]) setting, this bit will set to 1 by hardware.
     * |        |          |Note3: There is a new incoming capture event detected before CPU clearing the CAPIF status
     * |        |          |If the above condition occurred, the Timer will keep register TIMERx_CAP unchanged and drop the new capture value.
     * @var TIMER_T::TRGCTL
     * Offset: 0x1C  Timer Trigger Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |TRGSSEL   |Trigger Source Select Bit
     * |        |          |This bit is used to select internal trigger source is form timer time-out interrupt signal or
     * |        |          |capture interrupt signal.
     * |        |          |0 = Time-out interrupt signal is used to internal trigger EPWM, BPWM, PDMA, DAC, and EADC.
     * |        |          |1 = Capture interrupt signal is used to internal trigger EPWM, BPWM, PDMA, DAC, and EADC.
     * |[1]     |TRGEPWM   |Trigger EPWM and BPWM Enable Bit
     * |        |          |If this bit is set to 1, each timer time-out event or capture event can be as EPWM and BPWM counter clock source.
     * |        |          |0 = Timer interrupt trigger EPWM and BPWM Disabled.
     * |        |          |1 = Timer interrupt trigger EPWM and BPWM Enabled.
     * |        |          |Note: If TRGSSEL (TIMERx_TRGCTL[0]) = 0, time-out interrupt signal as EPWM and BPWM counter clock source.
     * |        |          |If TRGSSEL (TIMERx_TRGCTL[0]) = 1, capture interrupt signal as EPWM counter clock source.
     * |[2]     |TRGEADC   |Trigger EADC Enable Bit
     * |        |          |If this bit is set to 1, each timer time-out event or capture event can be triggered EADC conversion.
     * |        |          |0 = Timer interrupt trigger EADC Disabled.
     * |        |          |1 = Timer interrupt trigger EADC Enabled.
     * |        |          |Note: If TRGSSEL (TIMERx_TRGCTL[0]) = 0, time-out interrupt signal will trigger EADC conversion.
     * |        |          |If TRGSSEL (TIMERx_TRGCTL[0]) = 1, capture interrupt signal will trigger EADC conversion.
     * |[3]     |TRGDAC    |Trigger DAC Enable Bit
     * |        |          |If this bit is set to 1, timer time-out interrupt or capture interrupt can be triggered DAC.
     * |        |          |0 = Timer interrupt trigger DAC Disabled.
     * |        |          |1 = Timer interrupt trigger DAC Enabled.
     * |        |          |Note: If TRGSSEL (TIMERx_TRGCTL[0]) = 0, time-out interrupt signal will trigger DAC.
     * |        |          |If TRGSSEL (TIMERx_TRGCTL[0]) = 1, capture interrupt signal will trigger DAC.
     * |[4]     |TRGPDMA   |Trigger PDMA Enable Bit
     * |        |          |If this bit is set to 1, each timer time-out event or capture event can be triggered PDMA transfer.
     * |        |          |0 = Timer interrupt trigger PDMA Disabled.
     * |        |          |1 = Timer interrupt trigger PDMA Enabled.
     * |        |          |Note: If TRGSSEL (TIMERx_TRGCTL[0]) = 0, time-out interrupt signal will trigger PDMA transfer.
     * |        |          |If TRGSSEL (TIMERx_TRGCTL[0]) = 1, capture interrupt signal will trigger PDMA transfer.
     * @var TIMER_T::ALTCTL
     * Offset: 0x20  Timer Alternative Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |FUNCSEL   |Function Selection
     * |        |          |0 = Timer controller is used as timer function.
     * |        |          |1 = Timer controller is used as PWM function.
     * |        |          |Note: When timer is used as PWM, the clock source of time controller will be forced to PCLKx automatically.
     * @var TIMER_T::PWMCTL
     * Offset: 0x40  Timer PWM Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTEN     |PWM Counter Enable Bit
     * |        |          |0 = PWM counter and clock prescale Stop Running.
     * |        |          |1 = PWM counter and clock prescale Start Running.
     * |[2:1]   |CNTTYPE   |PWM Counter Behavior Type
     * |        |          |00 = Up count type.
     * |        |          |01 = Down count type.
     * |        |          |10 = Up-down count type.
     * |        |          |11 = Reserved.
     * |[3]     |CNTMODE   |PWM Counter Mode
     * |        |          |0 = Auto-reload mode.
     * |        |          |1 = One-shot mode.
     * |[8]     |CTRLD     |Center Re-load
     * |        |          |In up-down count type, PERIOD will load to PBUF when current PWM period is completed always and CMP will load to CMPBUF at the center point of current period.
     * |[9]     |IMMLDEN   |Immediately Load Enable Bit
     * |        |          |0 = PERIOD will load to PBUF when current PWM period is completed no matter CTRLD is enabled/disabled
     * |        |          |If CTRLD is disabled, CMP will load to CMPBUF when current PWM period is completed; if CTRLD is enabled in up-down count type, CMP will load to CMPBUF at the center point of current period.
     * |        |          |1 = PERIOD/CMP will load to PBUF/CMPBUF immediately when user update PERIOD/CMP.
     * |        |          |Note: If IMMLDEN is enabled, CTRLD will be invalid.
     * |[16]    |OUTMODE   |PWM Output Mode
     * |        |          |This bit controls the output mode of corresponding PWM channel.
     * |        |          |0 = PWM independent mode.
     * |        |          |1 = PWM complementary mode.
     * |[30]    |DBGHALT   |ICE Debug Mode Counter Halt (Write Protect)
     * |        |          |If debug mode counter halt is enabled, PWM counter will keep current value until exit ICE debug mode.
     * |        |          |0 = ICE debug mode counter halt disable.
     * |        |          |1 = ICE debug mode counter halt enable.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[31]    |DBGTRIOFF |ICE Debug Mode Acknowledge Disable Bit (Write Protect)
     * |        |          |0 = ICE debug mode acknowledgement effects PWM output.
     * |        |          |PWM output pin will be forced as tri-state while ICE debug mode acknowledged.
     * |        |          |1 = ICE debug mode acknowledgement disabled.
     * |        |          |PWM output pin will keep output no matter ICE debug mode acknowledged or not.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * @var TIMER_T::PWMCLKSRC
     * Offset: 0x44  Timer PWM Counter Clock Source Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |CLKSRC    |PWM Counter Clock Source Select
     * |        |          |The PWM counter clock source can be selected from TMRx_CLK or internal timer time-out or capture event.
     * |        |          |000 = TMRx_CLK.
     * |        |          |001 = Internal TIMER0 time-out or capture event.
     * |        |          |010 = Internal TIMER1 time-out or capture event.
     * |        |          |011 = Internal TIMER2 time-out or capture event.
     * |        |          |100 = Internal TIMER3 time-out or capture event.
     * |        |          |Others = Reserved.
     * |        |          |Note: If TIMER0 PWM function is enabled, the PWM counter clock source can be selected from TMR0_CLK, TIMER1 interrupt events, TIMER2 interrupt events, or TIMER3 interrupt events.
     * @var TIMER_T::PWMCLKPSC
     * Offset: 0x48  Timer PWM Counter Clock Pre-scale Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |CLKPSC    |PWM Counter Clock Pre-scale
     * |        |          |The active clock of PWM counter is decided by counter clock prescale and divided by (CLKPSC + 1)
     * |        |          |If CLKPSC is 0, then there is no scaling in PWM counter clock source.
     * @var TIMER_T::PWMCNTCLR
     * Offset: 0x4C  Timer PWM Clear Counter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTCLR    |Clear PWM Counter Control Bit
     * |        |          |It is automatically cleared by hardware.
     * |        |          |0 = No effect.
     * |        |          |1 = Clear 16-bit PWM counter to 0x10000 in up and up-down count type and reset counter value to PERIOD in down count type.
     * @var TIMER_T::PWMPERIOD
     * Offset: 0x50  Timer PWM Period Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |PERIOD    |PWM Period Register
     * |        |          |In up count type: PWM counter counts from 0 to PERIOD, and restarts from 0.
     * |        |          |In down count type: PWM counter counts from PERIOD to 0, and restarts from PERIOD.
     * |        |          |In up-down count type: PWM counter counts from 0 to PERIOD, then decrements to 0 and repeats again.
     * |        |          |In up and down count type:
     * |        |          |PWM period time = (PERIOD + 1) * (CLKPSC + 1) * TMRx_PWMCLK.
     * |        |          |In up-down count type:
     * |        |          |PWM period time = 2 * PERIOD * (CLKPSC+ 1) * TMRx_PWMCLK.
     * |        |          |Note: User should take care DIRF (TIMERx_PWMCNT[16]) bit in up/down/up-down count type to monitor current counter direction in each count type.
     * @var TIMER_T::PWMCMPDAT
     * Offset: 0x54  Timer PWM Comparator Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CMP       |PWM Comparator Register
     * |        |          |PWM CMP is used to compare with PWM CNT to generate PWM output waveform, interrupt events and trigger ADC to start convert.
     * @var TIMER_T::PWMDTCTL
     * Offset: 0x58  Timer PWM Dead-Time Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[11:0]  |DTCNT     |Dead-time Counter (Write Protect)
     * |        |          |The dead-time can be calculated from the following two formulas:
     * |        |          |Dead-time = (DTCNT[11:0] + 1) * TMRx_PWMCLK, if DTCKSEL is 0.
     * |        |          |Dead-time = (DTCNT[11:0] + 1) * TMRx_PWMCLK * (CLKPSC + 1), if DTCKSEL is 1.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[16]    |DTEN      |Enable Dead-time Insertion for PWMx_CH0 and PWMx_CH1 (Write Protect)
     * |        |          |Dead-time insertion function is only active when PWM complementary mode is enabled
     * |        |          |If dead- time insertion is inactive, the outputs of PWMx_CH0 and PWMx_CH1 are complementary without any delay.
     * |        |          |0 = Dead-time insertion Disabled on the pin pair.
     * |        |          |1 = Dead-time insertion Enabled on the pin pair.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[24]    |DTCKSEL   |Dead-time Clock Select (Write Protect)
     * |        |          |0 = Dead-time clock source from TMRx_PWMCLK without counter clock prescale.
     * |        |          |1 = Dead-time clock source from TMRx_PWMCLK with counter clock prescale.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * @var TIMER_T::PWMCNT
     * Offset: 0x5C  Timer PWM Counter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CNT       |PWM Counter Value Register (Read Only)
     * |        |          |User can monitor CNT to know the current counter value in 16-bit period counter.
     * |[16]    |DIRF      |PWM Counter Direction Indicator Flag (Read Only)
     * |        |          |0 = Counter is active in down count.
     * |        |          |1 = Counter is active up count.
     * @var TIMER_T::PWMMSKEN
     * Offset: 0x60  Timer PWM Output Mask Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MSKEN0    |PWMx_CH0 Output Mask Enable Bit
     * |        |          |The PWMx_CH0 output signal will be masked when this bit is enabled
     * |        |          |The PWMx_CH0 will output MSKDAT0 (TIMER_PWMMSK[0]) data.
     * |        |          |0 = PWMx_CH0 output signal is non-masked.
     * |        |          |1 = PWMx_CH0 output signal is masked and output MSKDAT0 data.
     * |[1]     |MSKEN1    |PWMx_CH1 Output Mask Enable Bit
     * |        |          |The PWMx_CH1 output signal will be masked when this bit is enabled
     * |        |          |The PWMx_CH1 will output MSKDAT1 (TIMER_PWMMSK[1]) data.
     * |        |          |0 = PWMx_CH1 output signal is non-masked.
     * |        |          |1 = PWMx_CH1 output signal is masked and output MSKDAT1 data.
     * @var TIMER_T::PWMMSK
     * Offset: 0x64  Timer PWM Output Mask Data Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |MSKDAT0   |PWMx_CH0 Output Mask Data Control Bit
     * |        |          |This bit is used to control the output state of PWMx_CH0 pin when PWMx_CH0 output mask function is enabled (MSKEN0 = 1).
     * |        |          |0 = Output logic Low to PWMx_CH0.
     * |        |          |1 = Output logic High to PWMx_CH0.
     * |[1]     |MSKDAT1   |PWMx_CH1 Output Mask Data Control Bit
     * |        |          |This bit is used to control the output state of PWMx_CH1 pin when PWMx_CH1 output mask function is enabled (MSKEN1 = 1).
     * |        |          |0 = Output logic Low to PWMx_CH1.
     * |        |          |1 = Output logic High to PWMx_CH1.
     * @var TIMER_T::PWMBNF
     * Offset: 0x68  Timer PWM Brake Pin Noise Filter Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRKNFEN   |Brake Pin Noise Filter Enable Bit
     * |        |          |0 = Pin noise filter detect of PWMx_BRAKEy Disabled.
     * |        |          |1 = Pin noise filter detect of PWMx_BRAKEy Enabled.
     * |[3:1]   |BRKNFSEL  |Brake Pin Noise Filter Clock Selection
     * |        |          |000 = Noise filter clock is PCLKx.
     * |        |          |001 = Noise filter clock is PCLKx/2.
     * |        |          |010 = Noise filter clock is PCLKx/4.
     * |        |          |011 = Noise filter clock is PCLKx/8.
     * |        |          |100 = Noise filter clock is PCLKx/16.
     * |        |          |101 = Noise filter clock is PCLKx/32.
     * |        |          |110 = Noise filter clock is PCLKx/64.
     * |        |          |111 = Noise filter clock is PCLKx/128.
     * |[6:4]   |BRKFCNT   |Brake Pin Noise Filter Count
     * |        |          |The fields is used to control the active noise filter sample time.
     * |        |          |Once noise filter sample time = (Period time of BRKDBCS) * BRKFCNT.
     * |[7]     |BRKPINV   |Brake Pin Detection Control Bit
     * |        |          |0 = Brake pin event will be detected if PWMx_BRAKEy pin status transfer from low to high in edge-detect, or pin status is high in level-detect.
     * |        |          |1 = Brake pin event will be detected if PWMx_BRAKEy pin status transfer from high to low in edge-detect, or pin status is low in level-detect .
     * |[17:16] |BKPINSRC  |Brake Pin Source Select
     * |        |          |00 = Brake pin source comes from PWM0_BRAKE0 pin.
     * |        |          |01 = Brake pin source comes from PWM0_BRAKE1 pin.
     * |        |          |10 = Brake pin source comes from PWM1_BRAKE0 pin.
     * |        |          |11 = Brake pin source comes from PWM1_BRAKE1 pin.
     * @var TIMER_T::PWMFAILBRK
     * Offset: 0x6C  Timer PWM System Fail Brake Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CSSBRKEN  |Clock Security System Detection Trigger PWM Brake Function Enable Bit
     * |        |          |0 = Brake Function triggered by clock fail detection Disabled.
     * |        |          |1 = Brake Function triggered by clock fail detection Enabled.
     * |[1]     |BODBRKEN  |Brown-out Detection Trigger PWM Brake Function Enable Bit
     * |        |          |0 = Brake Function triggered by BOD event Disabled.
     * |        |          |1 = Brake Function triggered by BOD event Enabled.
     * |[2]     |RAMBRKEN  |SRAM Parity Error Detection Trigger PWM Brake Function Enable Bit
     * |        |          |0 = Brake Function triggered by SRAM parity error detection Disabled.
     * |        |          |1 = Brake Function triggered by SRAM parity error detection Enabled.
     * |[3]     |CORBRKEN  |Core Lockup Detection Trigger PWM Brake Function Enable Bit
     * |        |          |0 = Brake Function triggered by core lockup event Disabled.
     * |        |          |1 = Brake Function triggered by core lockup event Enabled.
     * @var TIMER_T::PWMBRKCTL
     * Offset: 0x70  Timer PWM Brake Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CPO0EBEN  |Enable Internal ACMP0_O Digital Output As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = Internal ACMP0_O signal as edge-detect brake source Disabled.
     * |        |          |1 = Internal ACMP0_O signal as edge-detect brake source Enabled.
     * |        |          |Note1: Only internal ACMP0_O signal from low to high will be detected as brake event.
     * |        |          |Note2: This register is write protected. Refer toSYS_REGLCTL register.
     * |[1]     |CPO1EBEN  |Enable Internal ACMP1_O Digital Output As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = Internal ACMP1_O signal as edge-detect brake source Disabled.
     * |        |          |1 = Internal ACMP1_O signal as edge-detect brake source Enabled.
     * |        |          |Note1: Only internal ACMP1_O signal from low to high will be detected as brake event.
     * |        |          |Note2: This register is write protected. Refer toSYS_REGLCTL register.
     * |[4]     |BRKPEEN   |Enable TM_BRAKEx Pin As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = PWMx_BRAKEy pin event as edge-detect brake source Disabled.
     * |        |          |1 = PWMx_BRAKEy pin event as edge-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[7]     |SYSEBEN   |Enable System Fail As Edge-detect Brake Source (Write Protect)
     * |        |          |0 = System fail condition as edge-detect brake source Disabled.
     * |        |          |1 = System fail condition as edge-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[8]     |CPO0LBEN  |Enable Internal ACMP0_O Digital Output As Level-detect Brake Source (Write Protect)
     * |        |          |0 = Internal ACMP0_O signal as level-detect brake source Disabled.
     * |        |          |1 = Internal ACMP0_O signal as level-detect brake source Enabled.
     * |        |          |Note1: Only internal ACMP0_O signal from low to high will be detected as brake event.
     * |        |          |Note2: This register is write protected. Refer toSYS_REGLCTL register.
     * |[9]     |CPO1LBEN  |Enable Internal ACMP1_O Digital Output As Level-detect Brake Source (Write Protect)
     * |        |          |0 = Internal ACMP1_O signal as level-detect brake source Disabled.
     * |        |          |1 = Internal ACMP1_O signal as level-detect brake source Enabled.
     * |        |          |Note1: Only internal ACMP1_O signal from low to high will be detected as brake event.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[12]    |BRKPLEN   |Enable TM_BRAKEx Pin As Level-detect Brake Source (Write Protect)
     * |        |          |0 = PWMx_BRAKEy pin event as level-detect brake source Disabled.
     * |        |          |1 = PWMx_BRAKEy pin event as level-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[15]    |SYSLBEN   |Enable System Fail As Level-detect Brake Source (Write Protect)
     * |        |          |0 = System fail condition as level-detect brake source Disabled.
     * |        |          |1 = System fail condition as level-detect brake source Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[17:16] |BRKAEVEN  |PWM Brake Action Select for PWMx_CH0 (Write Protect)
     * |        |          |00 = PWMx_BRAKEy brake event will not affect PWMx_CH0 output.
     * |        |          |01 = PWMx_CH0 output tri-state when PWMx_BRAKEy brake event happened.
     * |        |          |10 = PWMx_CH0 output low level when PWMx_BRAKEy brake event happened.
     * |        |          |11 = PWMx_CH0 output high level when PWMx_BRAKEy brake event happened.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[19:18] |BRKAODD   |PWM Brake Action Select for PWMx_CH1 (Write Protect)
     * |        |          |00 = PWMx_BRAKEy brake event will not affect PWMx_CH1 output.
     * |        |          |01 = PWMx_CH1 output tri-state when PWMx_BRAKEy brake event happened.
     * |        |          |10 = PWMx_CH1 output low level when PWMx_BRAKEy brake event happened.
     * |        |          |11 = PWMx_CH1 output high level when PWMx_BRAKEy brake event happened.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * @var TIMER_T::PWMPOLCTL
     * Offset: 0x74  Timer PWM Pin Output Polar Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |PINV0     |PWMx_CH0 Output Pin Polar Control Bit
     * |        |          |The bit is used to control polarity state of PWMx_CH0 output pin.
     * |        |          |0 = PWMx_CH0 output pin polar inverse Disabled.
     * |        |          |1 = PWMx_CH0 output pin polar inverse Enabled.
     * |[1]     |PINV1     |PWMx_CH1 Output Pin Polar Control Bit
     * |        |          |The bit is used to control polarity state of PWMx_CH1 output pin.
     * |        |          |0 = PWMx_CH1 output pin polar inverse Disabled.
     * |        |          |1 = PWMx_CH1 output pin polar inverse Enabled.
     * @var TIMER_T::PWMPOEN
     * Offset: 0x78  Timer PWM Pin Output Enable Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |POEN0     |PWMx_CH0 Output Pin Enable Bit
     * |        |          |0 = PWMx_CH0 pin at tri-state mode.
     * |        |          |1 = PWMx_CH0 pin in output mode.
     * |[1]     |POEN1     |PWMx_CH1 Output Pin Enable Bit
     * |        |          |0 = PWMx_CH1 pin at tri-state mode.
     * |        |          |1 = PWMx_CH1 pin in output mode.
     * @var TIMER_T::PWMSWBRK
     * Offset: 0x7C  Timer PWM Software Trigger Brake Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRKETRG   |Software Trigger Edge-detect Brake Source (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger PWM edge-detect brake source, then BRKEIF0 and BRKEIF1 will set to 1 automatically in TIMERx_PWMINTSTS1 register.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[8]     |BRKLTRG   |Software Trigger Level-detect Brake Source (Write Only) (Write Protect)
     * |        |          |Write 1 to this bit will trigger PWM level-detect brake source, then BRKLIF0 and BRKLIF1 will set to 1 automatically in TIMERx_PWMINTSTS1 register.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * @var TIMER_T::PWMINTEN0
     * Offset: 0x80  Timer PWM Interrupt Enable Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ZIEN      |PWM Zero Point Interrupt Enable Bit
     * |        |          |0 = Zero point interrupt Disabled.
     * |        |          |1 = Zero point interrupt Enabled.
     * |[1]     |PIEN      |PWM Period Point Interrupt Enable Bit
     * |        |          |0 = Period point interrupt Disabled.
     * |        |          |1 = Period point interrupt Enabled.
     * |        |          |Note: When in up-down count type, period point means the center point of current PWM period.
     * |[2]     |CMPUIEN   |PWM Compare Up Count Interrupt Enable Bit
     * |        |          |0 = Compare up count interrupt Disabled.
     * |        |          |1 = Compare up count interrupt Enabled.
     * |[3]     |CMPDIEN   |PWM Compare Down Count Interrupt Enable Bit
     * |        |          |0 = Compare down count interrupt Disabled.
     * |        |          |1 = Compare down count interrupt Enabled.
     * @var TIMER_T::PWMINTEN1
     * Offset: 0x84  Timer PWM Interrupt Enable Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRKEIEN   |PWM Edge-detect Brake Interrupt Enable (Write Protect)
     * |        |          |0 = PWM edge-detect brake interrupt Disabled.
     * |        |          |1 = PWM edge-detect brake interrupt Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * |[8]     |BRKLIEN   |PWM Level-detect Brake Interrupt Enable (Write Protect)
     * |        |          |0 = PWM level-detect brake interrupt Disabled.
     * |        |          |1 = PWM level-detect brake interrupt Enabled.
     * |        |          |Note: This register is write protected. Refer toSYS_REGLCTL register.
     * @var TIMER_T::PWMINTSTS0
     * Offset: 0x88  Timer PWM Interrupt Status Register 0
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |ZIF       |PWM Zero Point Interrupt Flag
     * |        |          |This bit is set by hardware when TIMERx_PWM counter reaches zero.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[1]     |PIF       |PWM Period Point Interrupt Flag
     * |        |          |This bit is set by hardware when TIMERx_PWM counter reaches PERIOD.
     * |        |          |Note1: When in up-down count type, PIF flag means the center point flag of current PWM period.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[2]     |CMPUIF    |PWM Compare Up Count Interrupt Flag
     * |        |          |This bit is set by hardware when TIMERx_PWM counter in up count direction and reaches CMP.
     * |        |          |Note1: If CMP equal to PERIOD, there is no CMPUIF flag in up count type and up-down count type.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * |[3]     |CMPDIF    |PWM Compare Down Count Interrupt Flag
     * |        |          |This bit is set by hardware when TIMERx_PWM counter in down count direction and reaches CMP.
     * |        |          |Note1: If CMP equal to PERIOD, there is no CMPDIF flag in down count type.
     * |        |          |Note2: This bit is cleared by writing 1 to it.
     * @var TIMER_T::PWMINTSTS1
     * Offset: 0x8C  Timer PWM Interrupt Status Register 1
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |BRKEIF0   |Edge-detect Brake Interrupt Flag on PWMx_CH0 (Write Protect)
     * |        |          |0 = PWMx_CH0 edge-detect brake event do not happened.
     * |        |          |1 = PWMx_CH0 edge-detect brake event happened.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2: This register is write protected. Refer toSYS_REGLCTL register.
     * |[1]     |BRKEIF1   |Edge-detect Brake Interrupt Flag PWMx_CH1 (Write Protect)
     * |        |          |0 = PWMx_CH1 edge-detect brake event do not happened.
     * |        |          |1 = PWMx_CH1 edge-detect brake event happened.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2: This register is write protected. Refer toSYS_REGLCTL register.
     * |[8]     |BRKLIF0   |Level-detect Brake Interrupt Flag on PWMx_CH0 (Write Protect)
     * |        |          |0 = PWMx_CH0 level-detect brake event do not happened.
     * |        |          |1 = PWMx_CH0 level-detect brake event happened.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2: This register is write protected. Refer toSYS_REGLCTL register.
     * |[9]     |BRKLIF1   |Level-detect Brake Interrupt Flag on PWMx_CH1 (Write Protect)
     * |        |          |0 = PWMx_CH1 level-detect brake event do not happened.
     * |        |          |1 = PWMx_CH1 level-detect brake event happened.
     * |        |          |Note1: This bit is cleared by writing 1 to it.
     * |        |          |Note2: This register is write protected. Refer toSYS_REGLCTL register.
     * |[16]    |BRKESTS0  |Edge -detect Brake Status of PWMx_CH0 (Read Only)
     * |        |          |0 = PWMx_CH0 edge-detect brake state is released.
     * |        |          |1 = PWMx_CH0 at edge-detect brake state.
     * |        |          |Note: User can set BRKEIF0 1 to clear BRKEIF0 flag and PWMx_CH0 will release brake state when current PWM period finished and resume PWMx_CH0 output waveform start from next full PWM period.
     * |[17]    |BRKESTS1  |Edge-detect Brake Status of PWMx_CH1 (Read Only)
     * |        |          |0 = PWMx_CH1 edge-detect brake state is released.
     * |        |          |1 = PWMx_CH1 at edge-detect brake state.
     * |        |          |Note: User can set BRKEIF1 1 to clear BRKEIF1 flag and PWMx_CH1 will release brake state when current PWM period finished and resume PWMx_CH1 output waveform start from next full PWM period.
     * |[24]    |BRKLSTS0  |Level-detect Brake Status of PWMx_CH0 (Read Only)
     * |        |          |0 = PWMx_CH0 level-detect brake state is released.
     * |        |          |1 = PWMx_CH0 at level-detect brake state.
     * |        |          |Note: If TIMERx_PWM level-detect brake source has released, both PWMx_CH0 and PWMx_CH1 will release brake state when current PWM period finished and resume PWMx_CH0 and PWMx_CH1 output waveform start from next full PWM period.
     * |[25]    |BRKLSTS1  |Level-detect Brake Status of PWMx_CH1 (Read Only)
     * |        |          |0 = PWMx_CH1 level-detect brake state is released.
     * |        |          |1 = PWMx_CH1 at level-detect brake state.
     * |        |          |Note: If TIMERx_PWM level-detect brake source has released, both PWMx_CH0 and PWMx_CH1 will release brake state when current PWM period finished and resume PWMx_CH0 and PWMx_CH1 output waveform start from next full PWM period.
     * @var TIMER_T::PWMEADCTS
     * Offset: 0x90  Timer PWM ADC Trigger Source Select Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[2:0]   |TRGSEL    |PWM Counter Event Source Select to Trigger EADC Conversion
     * |        |          |000 = Trigger EADC conversion at zero point (ZIF).
     * |        |          |001 = Trigger EADC conversion at period point (PIF).
     * |        |          |010 = Trigger EADC conversion at zero or period point (ZIF or PIF).
     * |        |          |011 = Trigger EADC conversion at compare up count point (CMPUIF).
     * |        |          |100 = Trigger EADC conversion at compare down count point (CMPDIF).
     * |        |          |Others = Reserved.
     * |[7]     |TRGEN     |PWM Counter Event Trigger EADC Conversion Enable Bit
     * |        |          |0 = PWM counter event trigger EADC conversion Disabled.
     * |        |          |1 = PWM counter event trigger EADC conversion Enabled.
     * @var TIMER_T::PWMSCTL
     * Offset: 0x94  Timer PWM Synchronous Control Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[1:0]   |SYNCMODE  |PWM Synchronous Mode Enable Select
     * |        |          |00 = PWM synchronous function Disabled.
     * |        |          |01 = PWM synchronous counter start function Enabled.
     * |        |          |10 = Reserved.
     * |        |          |11 = PWM synchronous counter clear function Enabled.
     * |[8]     |SYNCSRC   |PWM Synchronous Counter Start/Clear Source Select
     * |        |          |0 = Counter synchronous start/clear by trigger TIMER0_PWMSTRG STRGEN.
     * |        |          |1 = Counter synchronous start/clear by trigger TIMER2_PWMSTRG STRGEN.
     * |        |          |Note1: If TIMER0/1/2/3 PWM counter synchronous source are from TIMER0, TIMER0_PWMSCTL[8], TIMER1_PWMSCTL[8], TIMER2_PWMSCTL[8] and TIMER3_PWMSCTL[8] should be 0.
     * |        |          |Note2: If TIMER0/1/ PWM counter synchronous source are from TIMER0, TIMER0_PWMSCTL[8] and TIMER1_PWMSCTL[8] should be set 0, and TIMER2/3/ PWM counter synchronous source are from TIMER2, TIME2_PWMSCTL[8] and TIMER3_PWMSCTL[8] should be set 1.
     * @var TIMER_T::PWMSTRG
     * Offset: 0x98  Timer PWM Synchronous Trigger Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |STRGEN    |PWM Counter Synchronous Trigger Enable Bit (Write Only)
     * |        |          |PMW counter synchronous function is used to make selected PWM channels (include TIMER0/1/2/3 PWM, TIMER0/1 PWM and TIMER2/3 PWM) start counting or clear counter at the same time according to TIMERx_PWMSCTL setting.
     * |        |          |Note: This bit is only available in TIMER0 and TIMER2.
     * @var TIMER_T::PWMSTATUS
     * Offset: 0x9C  Timer PWM Status Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[0]     |CNTMAXF   |PWM Counter Equal to 0xFFFF Flag
     * |        |          |0 = Indicates the PWM counter value never reached its maximum value 0xFFFF.
     * |        |          |1 = Indicates the PWM counter value has reached its maximum value.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * |[16]    |EADCTRGF  |Trigger EADC Start Conversion Flag
     * |        |          |0 = PWM counter event trigger EADC start conversion is not occurred.
     * |        |          |1 = PWM counter event trigger EADC start conversion has occurred.
     * |        |          |Note: This bit is cleared by writing 1 to it.
     * @var TIMER_T::PWMPBUF
     * Offset: 0xA0  Timer PWM Period Buffer Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |PBUF      |PWM Period Buffer Register (Read Only)
     * |        |          |Used as PERIOD active register.
     * @var TIMER_T::PWMCMPBUF
     * Offset: 0xA4  Timer PWM Comparator Buffer Register
     * ---------------------------------------------------------------------------------------------------
     * |Bits    |Field     |Descriptions
     * | :----: | :----:   | :---- |
     * |[15:0]  |CMPBUF    |PWM Comparator Buffer Register (Read Only)
     * |        |          |Used as CMP active register.
     */
    __IO uint32_t CTL;                   /*!< [0x0000] Timer Control Register                                           */
    __IO uint32_t CMP;                   /*!< [0x0004] Timer Comparator Register                                        */
    __IO uint32_t INTSTS;                /*!< [0x0008] Timer Interrupt Status Register                                  */
    __IO uint32_t CNT;                   /*!< [0x000c] Timer Data Register                                              */
    __I  uint32_t CAP;                   /*!< [0x0010] Timer Capture Data Register                                      */
    __IO uint32_t EXTCTL;                /*!< [0x0014] Timer External Control Register                                  */
    __IO uint32_t EINTSTS;               /*!< [0x0018] Timer External Interrupt Status Register                         */
    __IO uint32_t TRGCTL;                /*!< [0x001c] Timer Trigger Control Register                                   */
    __IO uint32_t ALTCTL;                /*!< [0x0020] Timer Alternative Control Register                               */
    /** @cond HIDDEN_SYMBOLS */
    __I  uint32_t RESERVE0[7];
    /** @endcond */
    __IO uint32_t PWMCTL;                /*!< [0x0040] Timer PWM Control Register                                       */
    __IO uint32_t PWMCLKSRC;             /*!< [0x0044] Timer PWM Counter Clock Source Register                          */
    __IO uint32_t PWMCLKPSC;             /*!< [0x0048] Timer PWM Counter Clock Pre-scale Register                       */
    __IO uint32_t PWMCNTCLR;             /*!< [0x004c] Timer PWM Clear Counter Register                                 */
    __IO uint32_t PWMPERIOD;             /*!< [0x0050] Timer PWM Period Register                                        */
    __IO uint32_t PWMCMPDAT;             /*!< [0x0054] Timer PWM Comparator Register                                    */
    __IO uint32_t PWMDTCTL;              /*!< [0x0058] Timer PWM Dead-Time Control Register                             */
    __I  uint32_t PWMCNT;                /*!< [0x005c] Timer PWM Counter Register                                       */
    __IO uint32_t PWMMSKEN;              /*!< [0x0060] Timer PWM Output Mask Enable Register                            */
    __IO uint32_t PWMMSK;                /*!< [0x0064] Timer PWM Output Mask Data Control Register                      */
    __IO uint32_t PWMBNF;                /*!< [0x0068] Timer PWM Brake Pin Noise Filter Register                        */
    __IO uint32_t PWMFAILBRK;            /*!< [0x006c] Timer PWM System Fail Brake Control Register                     */
    __IO uint32_t PWMBRKCTL;             /*!< [0x0070] Timer PWM Brake Control Register                                 */
    __IO uint32_t PWMPOLCTL;             /*!< [0x0074] Timer PWM Pin Output Polar Control Register                      */
    __IO uint32_t PWMPOEN;               /*!< [0x0078] Timer PWM Pin Output Enable Register                             */
    __O  uint32_t PWMSWBRK;              /*!< [0x007c] Timer PWM Software Trigger Brake Control Register                */
    __IO uint32_t PWMINTEN0;             /*!< [0x0080] Timer PWM Interrupt Enable Register 0                            */
    __IO uint32_t PWMINTEN1;             /*!< [0x0084] Timer PWM Interrupt Enable Register 1                            */
    __IO uint32_t PWMINTSTS0;            /*!< [0x0088] Timer PWM Interrupt Status Register 0                            */
    __IO uint32_t PWMINTSTS1;            /*!< [0x008c] Timer PWM Interrupt Status Register 1                            */
    __IO uint32_t PWMEADCTS;             /*!< [0x0090] Timer PWM EADC Trigger Source Select Register                    */
    __IO uint32_t PWMSCTL;               /*!< [0x0094] Timer PWM Synchronous Control Register                           */
    __O  uint32_t PWMSTRG;               /*!< [0x0098] Timer PWM Synchronous Trigger Register                           */
    __IO uint32_t PWMSTATUS;             /*!< [0x009c] Timer PWM Status Register                                        */
    __I  uint32_t PWMPBUF;               /*!< [0x00a0] Timer PWM Period Buffer Register                                 */
    __I  uint32_t PWMCMPBUF;             /*!< [0x00a4] Timer PWM Comparator Buffer Register                             */

} TIMER_T;

/**
    @addtogroup TIMER_CONST TIMER Bit Field Definition
    Constant Definitions for TIMER Controller
@{ */

#define TIMER_CTL_PSC_Pos                (0)                                               /*!< TIMER_T::CTL: PSC Position             */
#define TIMER_CTL_PSC_Msk                (0xfful << TIMER_CTL_PSC_Pos)                     /*!< TIMER_T::CTL: PSC Mask                 */

#define TIMER_CTL_INTRGEN_Pos            (19)                                              /*!< TIMER_T::CTL: INTRGEN Position         */
#define TIMER_CTL_INTRGEN_Msk            (0x1ul << TIMER_CTL_INTRGEN_Pos)                  /*!< TIMER_T::CTL: INTRGEN Mask             */

#define TIMER_CTL_PERIOSEL_Pos           (20)                                              /*!< TIMER_T::CTL: PERIOSEL Position        */
#define TIMER_CTL_PERIOSEL_Msk           (0x1ul << TIMER_CTL_PERIOSEL_Pos)                 /*!< TIMER_T::CTL: PERIOSEL Mask            */

#define TIMER_CTL_TGLPINSEL_Pos          (21)                                              /*!< TIMER_T::CTL: TGLPINSEL Position       */
#define TIMER_CTL_TGLPINSEL_Msk          (0x1ul << TIMER_CTL_TGLPINSEL_Pos)                /*!< TIMER_T::CTL: TGLPINSEL Mask           */

#define TIMER_CTL_CAPSRC_Pos             (22)                                              /*!< TIMER_T::CTL: CAPSRC Position          */
#define TIMER_CTL_CAPSRC_Msk             (0x1ul << TIMER_CTL_CAPSRC_Pos)                   /*!< TIMER_T::CTL: CAPSRC Mask              */

#define TIMER_CTL_WKEN_Pos               (23)                                              /*!< TIMER_T::CTL: WKEN Position            */
#define TIMER_CTL_WKEN_Msk               (0x1ul << TIMER_CTL_WKEN_Pos)                     /*!< TIMER_T::CTL: WKEN Mask                */

#define TIMER_CTL_EXTCNTEN_Pos           (24)                                              /*!< TIMER_T::CTL: EXTCNTEN Position        */
#define TIMER_CTL_EXTCNTEN_Msk           (0x1ul << TIMER_CTL_EXTCNTEN_Pos)                 /*!< TIMER_T::CTL: EXTCNTEN Mask            */

#define TIMER_CTL_ACTSTS_Pos             (25)                                              /*!< TIMER_T::CTL: ACTSTS Position          */
#define TIMER_CTL_ACTSTS_Msk             (0x1ul << TIMER_CTL_ACTSTS_Pos)                   /*!< TIMER_T::CTL: ACTSTS Mask              */

#define TIMER_CTL_OPMODE_Pos             (27)                                              /*!< TIMER_T::CTL: OPMODE Position          */
#define TIMER_CTL_OPMODE_Msk             (0x3ul << TIMER_CTL_OPMODE_Pos)                   /*!< TIMER_T::CTL: OPMODE Mask              */

#define TIMER_CTL_INTEN_Pos              (29)                                              /*!< TIMER_T::CTL: INTEN Position           */
#define TIMER_CTL_INTEN_Msk              (0x1ul << TIMER_CTL_INTEN_Pos)                    /*!< TIMER_T::CTL: INTEN Mask               */

#define TIMER_CTL_CNTEN_Pos              (30)                                              /*!< TIMER_T::CTL: CNTEN Position           */
#define TIMER_CTL_CNTEN_Msk              (0x1ul << TIMER_CTL_CNTEN_Pos)                    /*!< TIMER_T::CTL: CNTEN Mask               */

#define TIMER_CTL_ICEDEBUG_Pos           (31)                                              /*!< TIMER_T::CTL: ICEDEBUG Position        */
#define TIMER_CTL_ICEDEBUG_Msk           (0x1ul << TIMER_CTL_ICEDEBUG_Pos)                 /*!< TIMER_T::CTL: ICEDEBUG Mask            */

#define TIMER_CMP_CMPDAT_Pos             (0)                                               /*!< TIMER_T::CMP: CMPDAT Position          */
#define TIMER_CMP_CMPDAT_Msk             (0xfffffful << TIMER_CMP_CMPDAT_Pos)              /*!< TIMER_T::CMP: CMPDAT Mask              */

#define TIMER_INTSTS_TIF_Pos             (0)                                               /*!< TIMER_T::INTSTS: TIF Position          */
#define TIMER_INTSTS_TIF_Msk             (0x1ul << TIMER_INTSTS_TIF_Pos)                   /*!< TIMER_T::INTSTS: TIF Mask              */

#define TIMER_INTSTS_TWKF_Pos            (1)                                               /*!< TIMER_T::INTSTS: TWKF Position         */
#define TIMER_INTSTS_TWKF_Msk            (0x1ul << TIMER_INTSTS_TWKF_Pos)                  /*!< TIMER_T::INTSTS: TWKF Mask             */

#define TIMER_CNT_CNT_Pos                (0)                                               /*!< TIMER_T::CNT: CNT Position             */
#define TIMER_CNT_CNT_Msk                (0xfffffful << TIMER_CNT_CNT_Pos)                 /*!< TIMER_T::CNT: CNT Mask                 */

#define TIMER_CNT_RSTACT_Pos             (31)                                              /*!< TIMER_T::CNT: RSTACT Position          */
#define TIMER_CNT_RSTACT_Msk             (0x1ul << TIMER_CNT_RSTACT_Pos)                   /*!< TIMER_T::CNT: RSTACT Mask              */

#define TIMER_CAP_CAPDAT_Pos             (0)                                               /*!< TIMER_T::CAP: CAPDAT Position          */
#define TIMER_CAP_CAPDAT_Msk             (0xfffffful << TIMER_CAP_CAPDAT_Pos)              /*!< TIMER_T::CAP: CAPDAT Mask              */

#define TIMER_EXTCTL_CNTPHASE_Pos        (0)                                               /*!< TIMER_T::EXTCTL: CNTPHASE Position     */
#define TIMER_EXTCTL_CNTPHASE_Msk        (0x1ul << TIMER_EXTCTL_CNTPHASE_Pos)              /*!< TIMER_T::EXTCTL: CNTPHASE Mask         */

#define TIMER_EXTCTL_CAPEN_Pos           (3)                                               /*!< TIMER_T::EXTCTL: CAPEN Position        */
#define TIMER_EXTCTL_CAPEN_Msk           (0x1ul << TIMER_EXTCTL_CAPEN_Pos)                 /*!< TIMER_T::EXTCTL: CAPEN Mask            */

#define TIMER_EXTCTL_CAPFUNCS_Pos        (4)                                               /*!< TIMER_T::EXTCTL: CAPFUNCS Position     */
#define TIMER_EXTCTL_CAPFUNCS_Msk        (0x1ul << TIMER_EXTCTL_CAPFUNCS_Pos)              /*!< TIMER_T::EXTCTL: CAPFUNCS Mask         */

#define TIMER_EXTCTL_CAPIEN_Pos          (5)                                               /*!< TIMER_T::EXTCTL: CAPIEN Position       */
#define TIMER_EXTCTL_CAPIEN_Msk          (0x1ul << TIMER_EXTCTL_CAPIEN_Pos)                /*!< TIMER_T::EXTCTL: CAPIEN Mask           */

#define TIMER_EXTCTL_CAPDBEN_Pos         (6)                                               /*!< TIMER_T::EXTCTL: CAPDBEN Position      */
#define TIMER_EXTCTL_CAPDBEN_Msk         (0x1ul << TIMER_EXTCTL_CAPDBEN_Pos)               /*!< TIMER_T::EXTCTL: CAPDBEN Mask          */

#define TIMER_EXTCTL_CNTDBEN_Pos         (7)                                               /*!< TIMER_T::EXTCTL: CNTDBEN Position      */
#define TIMER_EXTCTL_CNTDBEN_Msk         (0x1ul << TIMER_EXTCTL_CNTDBEN_Pos)               /*!< TIMER_T::EXTCTL: CNTDBEN Mask          */

#define TIMER_EXTCTL_ICAPSEL_Pos         (8)                                               /*!< TIMER_T::EXTCTL: ICAPSEL Position      */
#define TIMER_EXTCTL_ICAPSEL_Msk         (0x7ul << TIMER_EXTCTL_ICAPSEL_Pos)               /*!< TIMER_T::EXTCTL: ICAPSEL Mask          */

#define TIMER_EXTCTL_CAPEDGE_Pos         (12)                                              /*!< TIMER_T::EXTCTL: CAPEDGE Position      */
#define TIMER_EXTCTL_CAPEDGE_Msk         (0x7ul << TIMER_EXTCTL_CAPEDGE_Pos)               /*!< TIMER_T::EXTCTL: CAPEDGE Mask          */

#define TIMER_EXTCTL_ECNTSSEL_Pos        (16)                                              /*!< TIMER_T::EXTCTL: ECNTSSEL Position     */
#define TIMER_EXTCTL_ECNTSSEL_Msk        (0x1ul << TIMER_EXTCTL_ECNTSSEL_Pos)              /*!< TIMER_T::EXTCTL: ECNTSSEL Mask         */

#define TIMER_EXTCTL_CAPDIVSCL_Pos       (28)                                              /*!< TIMER_T::EXTCTL: CAPDIVSCL Position     */
#define TIMER_EXTCTL_CAPDIVSCL_Msk       (0xful << TIMER_EXTCTL_CAPDIVSCL_Pos)             /*!< TIMER_T::EXTCTL: CAPDIVSCL Mask         */

#define TIMER_EINTSTS_CAPIF_Pos          (0)                                               /*!< TIMER_T::EINTSTS: CAPIF Position       */
#define TIMER_EINTSTS_CAPIF_Msk          (0x1ul << TIMER_EINTSTS_CAPIF_Pos)                /*!< TIMER_T::EINTSTS: CAPIF Mask           */

#define TIMER_TRGCTL_TRGSSEL_Pos         (0)                                               /*!< TIMER_T::TRGCTL: TRGSSEL Position      */
#define TIMER_TRGCTL_TRGSSEL_Msk         (0x1ul << TIMER_TRGCTL_TRGSSEL_Pos)               /*!< TIMER_T::TRGCTL: TRGSSEL Mask          */

#define TIMER_TRGCTL_TRGEPWM_Pos         (1)                                               /*!< TIMER_T::TRGCTL: TRGEPWM Position      */
#define TIMER_TRGCTL_TRGEPWM_Msk         (0x1ul << TIMER_TRGCTL_TRGEPWM_Pos)               /*!< TIMER_T::TRGCTL: TRGEPWM Mask          */

#define TIMER_TRGCTL_TRGEADC_Pos         (2)                                               /*!< TIMER_T::TRGCTL: TRGEADC Position      */
#define TIMER_TRGCTL_TRGEADC_Msk         (0x1ul << TIMER_TRGCTL_TRGEADC_Pos)               /*!< TIMER_T::TRGCTL: TRGEADC Mask          */

#define TIMER_TRGCTL_TRGDAC_Pos          (3)                                               /*!< TIMER_T::TRGCTL: TRGDAC Position       */
#define TIMER_TRGCTL_TRGDAC_Msk          (0x1ul << TIMER_TRGCTL_TRGDAC_Pos)                /*!< TIMER_T::TRGCTL: TRGDAC Mask           */

#define TIMER_TRGCTL_TRGPDMA_Pos         (4)                                               /*!< TIMER_T::TRGCTL: TRGPDMA Position      */
#define TIMER_TRGCTL_TRGPDMA_Msk         (0x1ul << TIMER_TRGCTL_TRGPDMA_Pos)               /*!< TIMER_T::TRGCTL: TRGPDMA Mask          */

#define TIMER_ALTCTL_FUNCSEL_Pos         (0)                                               /*!< TIMER_T::ALTCTL: FUNCSEL Position      */
#define TIMER_ALTCTL_FUNCSEL_Msk         (0x1ul << TIMER_ALTCTL_FUNCSEL_Pos)               /*!< TIMER_T::ALTCTL: FUNCSEL Mask          */

#define TIMER_PWMCTL_CNTEN_Pos           (0)                                               /*!< TIMER_T::PWMCTL: CNTEN Position        */
#define TIMER_PWMCTL_CNTEN_Msk           (0x1ul << TIMER_PWMCTL_CNTEN_Pos)                 /*!< TIMER_T::PWMCTL: CNTEN Mask            */

#define TIMER_PWMCTL_CNTTYPE_Pos         (1)                                               /*!< TIMER_T::PWMCTL: CNTTYPE Position      */
#define TIMER_PWMCTL_CNTTYPE_Msk         (0x3ul << TIMER_PWMCTL_CNTTYPE_Pos)               /*!< TIMER_T::PWMCTL: CNTTYPE Mask          */

#define TIMER_PWMCTL_CNTMODE_Pos         (3)                                               /*!< TIMER_T::PWMCTL: CNTMODE Position      */
#define TIMER_PWMCTL_CNTMODE_Msk         (0x1ul << TIMER_PWMCTL_CNTMODE_Pos)               /*!< TIMER_T::PWMCTL: CNTMODE Mask          */

#define TIMER_PWMCTL_CTRLD_Pos           (8)                                               /*!< TIMER_T::PWMCTL: CTRLD Position        */
#define TIMER_PWMCTL_CTRLD_Msk           (0x1ul << TIMER_PWMCTL_CTRLD_Pos)                 /*!< TIMER_T::PWMCTL: CTRLD Mask            */

#define TIMER_PWMCTL_IMMLDEN_Pos         (9)                                               /*!< TIMER_T::PWMCTL: IMMLDEN Position      */
#define TIMER_PWMCTL_IMMLDEN_Msk         (0x1ul << TIMER_PWMCTL_IMMLDEN_Pos)               /*!< TIMER_T::PWMCTL: IMMLDEN Mask          */

#define TIMER_PWMCTL_OUTMODE_Pos         (16)                                              /*!< TIMER_T::PWMCTL: OUTMODE Position      */
#define TIMER_PWMCTL_OUTMODE_Msk         (0x1ul << TIMER_PWMCTL_OUTMODE_Pos)               /*!< TIMER_T::PWMCTL: OUTMODE Mask          */

#define TIMER_PWMCTL_DBGHALT_Pos         (30)                                              /*!< TIMER_T::PWMCTL: DBGHALT Position      */
#define TIMER_PWMCTL_DBGHALT_Msk         (0x1ul << TIMER_PWMCTL_DBGHALT_Pos)               /*!< TIMER_T::PWMCTL: DBGHALT Mask          */

#define TIMER_PWMCTL_DBGTRIOFF_Pos       (31)                                              /*!< TIMER_T::PWMCTL: DBGTRIOFF Position    */
#define TIMER_PWMCTL_DBGTRIOFF_Msk       (0x1ul << TIMER_PWMCTL_DBGTRIOFF_Pos)             /*!< TIMER_T::PWMCTL: DBGTRIOFF Mask        */

#define TIMER_PWMCLKSRC_CLKSRC_Pos       (0)                                               /*!< TIMER_T::PWMCLKSRC: CLKSRC Position    */
#define TIMER_PWMCLKSRC_CLKSRC_Msk       (0x7ul << TIMER_PWMCLKSRC_CLKSRC_Pos)             /*!< TIMER_T::PWMCLKSRC: CLKSRC Mask        */

#define TIMER_PWMCLKPSC_CLKPSC_Pos       (0)                                               /*!< TIMER_T::PWMCLKPSC: CLKPSC Position    */
#define TIMER_PWMCLKPSC_CLKPSC_Msk       (0xffful << TIMER_PWMCLKPSC_CLKPSC_Pos)           /*!< TIMER_T::PWMCLKPSC: CLKPSC Mask        */

#define TIMER_PWMCNTCLR_CNTCLR_Pos       (0)                                               /*!< TIMER_T::PWMCNTCLR: CNTCLR Position    */
#define TIMER_PWMCNTCLR_CNTCLR_Msk       (0x1ul << TIMER_PWMCNTCLR_CNTCLR_Pos)             /*!< TIMER_T::PWMCNTCLR: CNTCLR Mask        */

#define TIMER_PWMPERIOD_PERIOD_Pos       (0)                                               /*!< TIMER_T::PWMPERIOD: PERIOD Position    */
#define TIMER_PWMPERIOD_PERIOD_Msk       (0xfffful << TIMER_PWMPERIOD_PERIOD_Pos)          /*!< TIMER_T::PWMPERIOD: PERIOD Mask        */

#define TIMER_PWMCMPDAT_CMP_Pos          (0)                                               /*!< TIMER_T::PWMCMPDAT: CMP Position       */
#define TIMER_PWMCMPDAT_CMP_Msk          (0xfffful << TIMER_PWMCMPDAT_CMP_Pos)             /*!< TIMER_T::PWMCMPDAT: CMP Mask           */

#define TIMER_PWMDTCTL_DTCNT_Pos         (0)                                               /*!< TIMER_T::PWMDTCTL: DTCNT Position      */
#define TIMER_PWMDTCTL_DTCNT_Msk         (0xffful << TIMER_PWMDTCTL_DTCNT_Pos)             /*!< TIMER_T::PWMDTCTL: DTCNT Mask          */

#define TIMER_PWMDTCTL_DTEN_Pos          (16)                                              /*!< TIMER_T::PWMDTCTL: DTEN Position       */
#define TIMER_PWMDTCTL_DTEN_Msk          (0x1ul << TIMER_PWMDTCTL_DTEN_Pos)                /*!< TIMER_T::PWMDTCTL: DTEN Mask           */

#define TIMER_PWMDTCTL_DTCKSEL_Pos       (24)                                              /*!< TIMER_T::PWMDTCTL: DTCKSEL Position    */
#define TIMER_PWMDTCTL_DTCKSEL_Msk       (0x1ul << TIMER_PWMDTCTL_DTCKSEL_Pos)             /*!< TIMER_T::PWMDTCTL: DTCKSEL Mask        */

#define TIMER_PWMCNT_CNT_Pos             (0)                                               /*!< TIMER_T::PWMCNT: CNT Position          */
#define TIMER_PWMCNT_CNT_Msk             (0xfffful << TIMER_PWMCNT_CNT_Pos)                /*!< TIMER_T::PWMCNT: CNT Mask              */

#define TIMER_PWMCNT_DIRF_Pos            (16)                                              /*!< TIMER_T::PWMCNT: DIRF Position         */
#define TIMER_PWMCNT_DIRF_Msk            (0x1ul << TIMER_PWMCNT_DIRF_Pos)                  /*!< TIMER_T::PWMCNT: DIRF Mask             */

#define TIMER_PWMMSKEN_MSKEN0_Pos        (0)                                               /*!< TIMER_T::PWMMSKEN: MSKEN0 Position     */
#define TIMER_PWMMSKEN_MSKEN0_Msk        (0x1ul << TIMER_PWMMSKEN_MSKEN0_Pos)              /*!< TIMER_T::PWMMSKEN: MSKEN0 Mask         */

#define TIMER_PWMMSKEN_MSKEN1_Pos        (1)                                               /*!< TIMER_T::PWMMSKEN: MSKEN1 Position     */
#define TIMER_PWMMSKEN_MSKEN1_Msk        (0x1ul << TIMER_PWMMSKEN_MSKEN1_Pos)              /*!< TIMER_T::PWMMSKEN: MSKEN1 Mask         */

#define TIMER_PWMMSK_MSKDAT0_Pos         (0)                                               /*!< TIMER_T::PWMMSK: MSKDAT0 Position      */
#define TIMER_PWMMSK_MSKDAT0_Msk         (0x1ul << TIMER_PWMMSK_MSKDAT0_Pos)               /*!< TIMER_T::PWMMSK: MSKDAT0 Mask          */

#define TIMER_PWMMSK_MSKDAT1_Pos         (1)                                               /*!< TIMER_T::PWMMSK: MSKDAT1 Position      */
#define TIMER_PWMMSK_MSKDAT1_Msk         (0x1ul << TIMER_PWMMSK_MSKDAT1_Pos)               /*!< TIMER_T::PWMMSK: MSKDAT1 Mask          */

#define TIMER_PWMBNF_BRKNFEN_Pos         (0)                                               /*!< TIMER_T::PWMBNF: BRKNFEN Position      */
#define TIMER_PWMBNF_BRKNFEN_Msk         (0x1ul << TIMER_PWMBNF_BRKNFEN_Pos)               /*!< TIMER_T::PWMBNF: BRKNFEN Mask          */

#define TIMER_PWMBNF_BRKNFSEL_Pos        (1)                                               /*!< TIMER_T::PWMBNF: BRKNFSEL Position     */
#define TIMER_PWMBNF_BRKNFSEL_Msk        (0x7ul << TIMER_PWMBNF_BRKNFSEL_Pos)              /*!< TIMER_T::PWMBNF: BRKNFSEL Mask         */

#define TIMER_PWMBNF_BRKFCNT_Pos         (4)                                               /*!< TIMER_T::PWMBNF: BRKFCNT Position      */
#define TIMER_PWMBNF_BRKFCNT_Msk         (0x7ul << TIMER_PWMBNF_BRKFCNT_Pos)               /*!< TIMER_T::PWMBNF: BRKFCNT Mask          */

#define TIMER_PWMBNF_BRKPINV_Pos         (7)                                               /*!< TIMER_T::PWMBNF: BRKPINV Position      */
#define TIMER_PWMBNF_BRKPINV_Msk         (0x1ul << TIMER_PWMBNF_BRKPINV_Pos)               /*!< TIMER_T::PWMBNF: BRKPINV Mask          */

#define TIMER_PWMBNF_BKPINSRC_Pos        (16)                                              /*!< TIMER_T::PWMBNF: BKPINSRC Position     */
#define TIMER_PWMBNF_BKPINSRC_Msk        (0x3ul << TIMER_PWMBNF_BKPINSRC_Pos)              /*!< TIMER_T::PWMBNF: BKPINSRC Mask         */

#define TIMER_PWMFAILBRK_CSSBRKEN_Pos    (0)                                               /*!< TIMER_T::PWMFAILBRK: CSSBRKEN Position */
#define TIMER_PWMFAILBRK_CSSBRKEN_Msk    (0x1ul << TIMER_PWMFAILBRK_CSSBRKEN_Pos)          /*!< TIMER_T::PWMFAILBRK: CSSBRKEN Mask     */

#define TIMER_PWMFAILBRK_BODBRKEN_Pos    (1)                                               /*!< TIMER_T::PWMFAILBRK: BODBRKEN Position */
#define TIMER_PWMFAILBRK_BODBRKEN_Msk    (0x1ul << TIMER_PWMFAILBRK_BODBRKEN_Pos)          /*!< TIMER_T::PWMFAILBRK: BODBRKEN Mask     */

#define TIMER_PWMFAILBRK_RAMBRKEN_Pos    (2)                                               /*!< TIMER_T::PWMFAILBRK: RAMBRKEN Position */
#define TIMER_PWMFAILBRK_RAMBRKEN_Msk    (0x1ul << TIMER_PWMFAILBRK_RAMBRKEN_Pos)          /*!< TIMER_T::PWMFAILBRK: RAMBRKEN Mask     */

#define TIMER_PWMFAILBRK_CORBRKEN_Pos    (3)                                               /*!< TIMER_T::PWMFAILBRK: CORBRKEN Position */
#define TIMER_PWMFAILBRK_CORBRKEN_Msk    (0x1ul << TIMER_PWMFAILBRK_CORBRKEN_Pos)          /*!< TIMER_T::PWMFAILBRK: CORBRKEN Mask     */

#define TIMER_PWMBRKCTL_CPO0EBEN_Pos     (0)                                               /*!< TIMER_T::PWMBRKCTL: CPO0EBEN Position  */
#define TIMER_PWMBRKCTL_CPO0EBEN_Msk     (0x1ul << TIMER_PWMBRKCTL_CPO0EBEN_Pos)           /*!< TIMER_T::PWMBRKCTL: CPO0EBEN Mask      */

#define TIMER_PWMBRKCTL_CPO1EBEN_Pos     (1)                                               /*!< TIMER_T::PWMBRKCTL: CPO1EBEN Position  */
#define TIMER_PWMBRKCTL_CPO1EBEN_Msk     (0x1ul << TIMER_PWMBRKCTL_CPO1EBEN_Pos)           /*!< TIMER_T::PWMBRKCTL: CPO1EBEN Mask      */

#define TIMER_PWMBRKCTL_BRKPEEN_Pos      (4)                                               /*!< TIMER_T::PWMBRKCTL: BRKPEEN Position   */
#define TIMER_PWMBRKCTL_BRKPEEN_Msk      (0x1ul << TIMER_PWMBRKCTL_BRKPEEN_Pos)            /*!< TIMER_T::PWMBRKCTL: BRKPEEN Mask       */

#define TIMER_PWMBRKCTL_SYSEBEN_Pos      (7)                                               /*!< TIMER_T::PWMBRKCTL: SYSEBEN Position   */
#define TIMER_PWMBRKCTL_SYSEBEN_Msk      (0x1ul << TIMER_PWMBRKCTL_SYSEBEN_Pos)            /*!< TIMER_T::PWMBRKCTL: SYSEBEN Mask       */

#define TIMER_PWMBRKCTL_CPO0LBEN_Pos     (8)                                               /*!< TIMER_T::PWMBRKCTL: CPO0LBEN Position  */
#define TIMER_PWMBRKCTL_CPO0LBEN_Msk     (0x1ul << TIMER_PWMBRKCTL_CPO0LBEN_Pos)           /*!< TIMER_T::PWMBRKCTL: CPO0LBEN Mask      */

#define TIMER_PWMBRKCTL_CPO1LBEN_Pos     (9)                                               /*!< TIMER_T::PWMBRKCTL: CPO1LBEN Position  */
#define TIMER_PWMBRKCTL_CPO1LBEN_Msk     (0x1ul << TIMER_PWMBRKCTL_CPO1LBEN_Pos)           /*!< TIMER_T::PWMBRKCTL: CPO1LBEN Mask      */

#define TIMER_PWMBRKCTL_BRKPLEN_Pos      (12)                                              /*!< TIMER_T::PWMBRKCTL: BRKPLEN Position   */
#define TIMER_PWMBRKCTL_BRKPLEN_Msk      (0x1ul << TIMER_PWMBRKCTL_BRKPLEN_Pos)            /*!< TIMER_T::PWMBRKCTL: BRKPLEN Mask       */

#define TIMER_PWMBRKCTL_SYSLBEN_Pos      (15)                                              /*!< TIMER_T::PWMBRKCTL: SYSLBEN Position   */
#define TIMER_PWMBRKCTL_SYSLBEN_Msk      (0x1ul << TIMER_PWMBRKCTL_SYSLBEN_Pos)            /*!< TIMER_T::PWMBRKCTL: SYSLBEN Mask       */

#define TIMER_PWMBRKCTL_BRKAEVEN_Pos     (16)                                              /*!< TIMER_T::PWMBRKCTL: BRKAEVEN Position  */
#define TIMER_PWMBRKCTL_BRKAEVEN_Msk     (0x3ul << TIMER_PWMBRKCTL_BRKAEVEN_Pos)           /*!< TIMER_T::PWMBRKCTL: BRKAEVEN Mask      */

#define TIMER_PWMBRKCTL_BRKAODD_Pos      (18)                                              /*!< TIMER_T::PWMBRKCTL: BRKAODD Position   */
#define TIMER_PWMBRKCTL_BRKAODD_Msk      (0x3ul << TIMER_PWMBRKCTL_BRKAODD_Pos)            /*!< TIMER_T::PWMBRKCTL: BRKAODD Mask       */

#define TIMER_PWMPOLCTL_PINV0_Pos        (0)                                               /*!< TIMER_T::PWMPOLCTL: PINV0 Position     */
#define TIMER_PWMPOLCTL_PINV0_Msk        (0x1ul << TIMER_PWMPOLCTL_PINV0_Pos)              /*!< TIMER_T::PWMPOLCTL: PINV0 Mask         */

#define TIMER_PWMPOLCTL_PINV1_Pos        (1)                                               /*!< TIMER_T::PWMPOLCTL: PINV1 Position     */
#define TIMER_PWMPOLCTL_PINV1_Msk        (0x1ul << TIMER_PWMPOLCTL_PINV1_Pos)              /*!< TIMER_T::PWMPOLCTL: PINV1 Mask         */

#define TIMER_PWMPOEN_POEN0_Pos          (0)                                               /*!< TIMER_T::PWMPOEN: POEN0 Position       */
#define TIMER_PWMPOEN_POEN0_Msk          (0x1ul << TIMER_PWMPOEN_POEN0_Pos)                /*!< TIMER_T::PWMPOEN: POEN0 Mask           */

#define TIMER_PWMPOEN_POEN1_Pos          (1)                                               /*!< TIMER_T::PWMPOEN: POEN1 Position       */
#define TIMER_PWMPOEN_POEN1_Msk          (0x1ul << TIMER_PWMPOEN_POEN1_Pos)                /*!< TIMER_T::PWMPOEN: POEN1 Mask           */

#define TIMER_PWMSWBRK_BRKETRG_Pos       (0)                                               /*!< TIMER_T::PWMSWBRK: BRKETRG Position    */
#define TIMER_PWMSWBRK_BRKETRG_Msk       (0x1ul << TIMER_PWMSWBRK_BRKETRG_Pos)             /*!< TIMER_T::PWMSWBRK: BRKETRG Mask        */

#define TIMER_PWMSWBRK_BRKLTRG_Pos       (8)                                               /*!< TIMER_T::PWMSWBRK: BRKLTRG Position    */
#define TIMER_PWMSWBRK_BRKLTRG_Msk       (0x1ul << TIMER_PWMSWBRK_BRKLTRG_Pos)             /*!< TIMER_T::PWMSWBRK: BRKLTRG Mask        */

#define TIMER_PWMINTEN0_ZIEN_Pos         (0)                                               /*!< TIMER_T::PWMINTEN0: ZIEN Position      */
#define TIMER_PWMINTEN0_ZIEN_Msk         (0x1ul << TIMER_PWMINTEN0_ZIEN_Pos)               /*!< TIMER_T::PWMINTEN0: ZIEN Mask          */

#define TIMER_PWMINTEN0_PIEN_Pos         (1)                                               /*!< TIMER_T::PWMINTEN0: PIEN Position      */
#define TIMER_PWMINTEN0_PIEN_Msk         (0x1ul << TIMER_PWMINTEN0_PIEN_Pos)               /*!< TIMER_T::PWMINTEN0: PIEN Mask          */

#define TIMER_PWMINTEN0_CMPUIEN_Pos      (2)                                               /*!< TIMER_T::PWMINTEN0: CMPUIEN Position   */
#define TIMER_PWMINTEN0_CMPUIEN_Msk      (0x1ul << TIMER_PWMINTEN0_CMPUIEN_Pos)            /*!< TIMER_T::PWMINTEN0: CMPUIEN Mask       */

#define TIMER_PWMINTEN0_CMPDIEN_Pos      (3)                                               /*!< TIMER_T::PWMINTEN0: CMPDIEN Position   */
#define TIMER_PWMINTEN0_CMPDIEN_Msk      (0x1ul << TIMER_PWMINTEN0_CMPDIEN_Pos)            /*!< TIMER_T::PWMINTEN0: CMPDIEN Mask       */

#define TIMER_PWMINTEN1_BRKEIEN_Pos      (0)                                               /*!< TIMER_T::PWMINTEN1: BRKEIEN Position   */
#define TIMER_PWMINTEN1_BRKEIEN_Msk      (0x1ul << TIMER_PWMINTEN1_BRKEIEN_Pos)            /*!< TIMER_T::PWMINTEN1: BRKEIEN Mask       */

#define TIMER_PWMINTEN1_BRKLIEN_Pos      (8)                                               /*!< TIMER_T::PWMINTEN1: BRKLIEN Position   */
#define TIMER_PWMINTEN1_BRKLIEN_Msk      (0x1ul << TIMER_PWMINTEN1_BRKLIEN_Pos)            /*!< TIMER_T::PWMINTEN1: BRKLIEN Mask       */

#define TIMER_PWMINTSTS0_ZIF_Pos         (0)                                               /*!< TIMER_T::PWMINTSTS0: ZIF Position      */
#define TIMER_PWMINTSTS0_ZIF_Msk         (0x1ul << TIMER_PWMINTSTS0_ZIF_Pos)               /*!< TIMER_T::PWMINTSTS0: ZIF Mask          */

#define TIMER_PWMINTSTS0_PIF_Pos         (1)                                               /*!< TIMER_T::PWMINTSTS0: PIF Position      */
#define TIMER_PWMINTSTS0_PIF_Msk         (0x1ul << TIMER_PWMINTSTS0_PIF_Pos)               /*!< TIMER_T::PWMINTSTS0: PIF Mask          */

#define TIMER_PWMINTSTS0_CMPUIF_Pos      (2)                                               /*!< TIMER_T::PWMINTSTS0: CMPUIF Position   */
#define TIMER_PWMINTSTS0_CMPUIF_Msk      (0x1ul << TIMER_PWMINTSTS0_CMPUIF_Pos)            /*!< TIMER_T::PWMINTSTS0: CMPUIF Mask       */

#define TIMER_PWMINTSTS0_CMPDIF_Pos      (3)                                               /*!< TIMER_T::PWMINTSTS0: CMPDIF Position   */
#define TIMER_PWMINTSTS0_CMPDIF_Msk      (0x1ul << TIMER_PWMINTSTS0_CMPDIF_Pos)            /*!< TIMER_T::PWMINTSTS0: CMPDIF Mask       */

#define TIMER_PWMINTSTS1_BRKEIF0_Pos     (0)                                               /*!< TIMER_T::PWMINTSTS1: BRKEIF0 Position  */
#define TIMER_PWMINTSTS1_BRKEIF0_Msk     (0x1ul << TIMER_PWMINTSTS1_BRKEIF0_Pos)           /*!< TIMER_T::PWMINTSTS1: BRKEIF0 Mask      */

#define TIMER_PWMINTSTS1_BRKEIF1_Pos     (1)                                               /*!< TIMER_T::PWMINTSTS1: BRKEIF1 Position  */
#define TIMER_PWMINTSTS1_BRKEIF1_Msk     (0x1ul << TIMER_PWMINTSTS1_BRKEIF1_Pos)           /*!< TIMER_T::PWMINTSTS1: BRKEIF1 Mask      */

#define TIMER_PWMINTSTS1_BRKLIF0_Pos     (8)                                               /*!< TIMER_T::PWMINTSTS1: BRKLIF0 Position  */
#define TIMER_PWMINTSTS1_BRKLIF0_Msk     (0x1ul << TIMER_PWMINTSTS1_BRKLIF0_Pos)           /*!< TIMER_T::PWMINTSTS1: BRKLIF0 Mask      */

#define TIMER_PWMINTSTS1_BRKLIF1_Pos     (9)                                               /*!< TIMER_T::PWMINTSTS1: BRKLIF1 Position  */
#define TIMER_PWMINTSTS1_BRKLIF1_Msk     (0x1ul << TIMER_PWMINTSTS1_BRKLIF1_Pos)           /*!< TIMER_T::PWMINTSTS1: BRKLIF1 Mask      */

#define TIMER_PWMINTSTS1_BRKESTS0_Pos    (16)                                              /*!< TIMER_T::PWMINTSTS1: BRKESTS0 Position */
#define TIMER_PWMINTSTS1_BRKESTS0_Msk    (0x1ul << TIMER_PWMINTSTS1_BRKESTS0_Pos)          /*!< TIMER_T::PWMINTSTS1: BRKESTS0 Mask     */

#define TIMER_PWMINTSTS1_BRKESTS1_Pos    (17)                                              /*!< TIMER_T::PWMINTSTS1: BRKESTS1 Position */
#define TIMER_PWMINTSTS1_BRKESTS1_Msk    (0x1ul << TIMER_PWMINTSTS1_BRKESTS1_Pos)          /*!< TIMER_T::PWMINTSTS1: BRKESTS1 Mask     */

#define TIMER_PWMINTSTS1_BRKLSTS0_Pos    (24)                                              /*!< TIMER_T::PWMINTSTS1: BRKLSTS0 Position */
#define TIMER_PWMINTSTS1_BRKLSTS0_Msk    (0x1ul << TIMER_PWMINTSTS1_BRKLSTS0_Pos)          /*!< TIMER_T::PWMINTSTS1: BRKLSTS0 Mask     */

#define TIMER_PWMINTSTS1_BRKLSTS1_Pos    (25)                                              /*!< TIMER_T::PWMINTSTS1: BRKLSTS1 Position */
#define TIMER_PWMINTSTS1_BRKLSTS1_Msk    (0x1ul << TIMER_PWMINTSTS1_BRKLSTS1_Pos)          /*!< TIMER_T::PWMINTSTS1: BRKLSTS1 Mask     */

#define TIMER_PWMEADCTS_TRGSEL_Pos       (0)                                               /*!< TIMER_T::PWMEADCTS: TRGSEL Position    */
#define TIMER_PWMEADCTS_TRGSEL_Msk       (0x7ul << TIMER_PWMEADCTS_TRGSEL_Pos)             /*!< TIMER_T::PWMEADCTS: TRGSEL Mask        */

#define TIMER_PWMEADCTS_TRGEN_Pos        (7)                                               /*!< TIMER_T::PWMEADCTS: TRGEN Position     */
#define TIMER_PWMEADCTS_TRGEN_Msk        (0x1ul << TIMER_PWMEADCTS_TRGEN_Pos)              /*!< TIMER_T::PWMEADCTS: TRGEN Mask         */

#define TIMER_PWMSCTL_SYNCMODE_Pos       (0)                                               /*!< TIMER_T::PWMSCTL: SYNCMODE Position    */
#define TIMER_PWMSCTL_SYNCMODE_Msk       (0x3ul << TIMER_PWMSCTL_SYNCMODE_Pos)             /*!< TIMER_T::PWMSCTL: SYNCMODE Mask        */

#define TIMER_PWMSCTL_SYNCSRC_Pos        (8)                                               /*!< TIMER_T::PWMSCTL: SYNCSRC Position     */
#define TIMER_PWMSCTL_SYNCSRC_Msk        (0x1ul << TIMER_PWMSCTL_SYNCSRC_Pos)              /*!< TIMER_T::PWMSCTL: SYNCSRC Mask         */

#define TIMER_PWMSTRG_STRGEN_Pos         (0)                                               /*!< TIMER_T::PWMSTRG: STRGEN Position      */
#define TIMER_PWMSTRG_STRGEN_Msk         (0x1ul << TIMER_PWMSTRG_STRGEN_Pos)               /*!< TIMER_T::PWMSTRG: STRGEN Mask          */

#define TIMER_PWMSTATUS_CNTMAXF_Pos      (0)                                               /*!< TIMER_T::PWMSTATUS: CNTMAXF Position   */
#define TIMER_PWMSTATUS_CNTMAXF_Msk      (0x1ul << TIMER_PWMSTATUS_CNTMAXF_Pos)            /*!< TIMER_T::PWMSTATUS: CNTMAXF Mask       */

#define TIMER_PWMSTATUS_EADCTRGF_Pos     (16)                                              /*!< TIMER_T::PWMSTATUS: EADCTRGF Position  */
#define TIMER_PWMSTATUS_EADCTRGF_Msk     (0x1ul << TIMER_PWMSTATUS_EADCTRGF_Pos)           /*!< TIMER_T::PWMSTATUS: EADCTRGF Mask      */

#define TIMER_PWMPBUF_PBUF_Pos           (0)                                               /*!< TIMER_T::PWMPBUF: PBUF Position        */
#define TIMER_PWMPBUF_PBUF_Msk           (0xfffful << TIMER_PWMPBUF_PBUF_Pos)              /*!< TIMER_T::PWMPBUF: PBUF Mask            */

#define TIMER_PWMCMPBUF_CMPBUF_Pos       (0)                                               /*!< TIMER_T::PWMCMPBUF: CMPBUF Position    */
#define TIMER_PWMCMPBUF_CMPBUF_Msk       (0xfffful << TIMER_PWMCMPBUF_CMPBUF_Pos)          /*!< TIMER_T::PWMCMPBUF: CMPBUF Mask        */

/**@}*/ /* TIMER_CONST */
/**@}*/ /* end of TIMER register group */
/**@}*/ /* end of REGISTER group */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* __TIMER_REG_H__ */
