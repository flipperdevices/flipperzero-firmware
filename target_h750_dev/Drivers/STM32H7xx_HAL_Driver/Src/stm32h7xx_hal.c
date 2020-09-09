/**
  ******************************************************************************
  * @file    stm32h7xx_hal.c
  * @author  MCD Application Team
  * @brief   HAL module driver.
  *          This is the common part of the HAL initialization
  *
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The common HAL driver contains a set of generic and common APIs that can be
    used by the PPP peripheral drivers and the user to start using the HAL.
    [..]
    The HAL contains two APIs' categories:
         (+) Common HAL APIs
         (+) Services HAL APIs

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @defgroup HAL  HAL
  * @brief HAL module driver.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
 * @brief STM32H7xx HAL Driver version number V1.8.0
   */
#define __STM32H7xx_HAL_VERSION_MAIN   (0x01UL) /*!< [31:24] main version */
#define __STM32H7xx_HAL_VERSION_SUB1   (0x08UL) /*!< [23:16] sub1 version */
#define __STM32H7xx_HAL_VERSION_SUB2   (0x00UL) /*!< [15:8]  sub2 version */
#define __STM32H7xx_HAL_VERSION_RC     (0x00UL) /*!< [7:0]  release candidate */
#define __STM32H7xx_HAL_VERSION         ((__STM32H7xx_HAL_VERSION_MAIN << 24)\
                                        |(__STM32H7xx_HAL_VERSION_SUB1 << 16)\
                                        |(__STM32H7xx_HAL_VERSION_SUB2 << 8 )\
                                        |(__STM32H7xx_HAL_VERSION_RC))

#define IDCODE_DEVID_MASK    ((uint32_t)0x00000FFF)
#define VREFBUF_TIMEOUT_VALUE     (uint32_t)10   /* 10 ms  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/** @defgroup HAL_Exported_Variables HAL Exported Variables
  * @{
  */
__IO uint32_t uwTick;
uint32_t uwTickPrio   = (1UL << __NVIC_PRIO_BITS); /* Invalid PRIO */
HAL_TickFreqTypeDef uwTickFreq = HAL_TICK_FREQ_DEFAULT;  /* 1KHz */
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_Private_Functions  HAL Private Functions
  * @{
  */

/** @defgroup HAL_Group1 Initialization and de-initialization Functions
 *  @brief    Initialization and de-initialization functions
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initializes the Flash interface the NVIC allocation and initial clock
          configuration. It initializes the systick also when timeout is needed
          and the backup domain when enabled.
      (+) De-Initializes common part of the HAL.
      (+) Configure The time base source to have 1ms time base with a dedicated
          Tick interrupt priority.
        (++) SysTick timer is used by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
        (++) Time base configuration function (HAL_InitTick ()) is called automatically
             at the beginning of the program after reset by HAL_Init() or at any time
             when clock is configured, by HAL_RCC_ClockConfig().
        (++) Source of time base is configured  to generate interrupts at regular
             time intervals. Care must be taken if HAL_Delay() is called from a
             peripheral ISR process, the Tick interrupt line must have higher priority
            (numerically lower) than the peripheral interrupt. Otherwise the caller
            ISR process will be blocked.
       (++) functions affecting time base configurations are declared as __weak
             to make  override possible  in case of other  implementations in user file.
@endverbatim
  * @{
  */

/**
  * @brief  This function is used to initialize the HAL Library; it must be the first
  *         instruction to be executed in the main program (before to call any other
  *         HAL function), it performs the following:
  *           Configures the SysTick to generate an interrupt each 1 millisecond,
  *           which is clocked by the HSI (at this stage, the clock is not yet
  *           configured and thus the system is running from the internal HSI at 16 MHz).
  *           Set NVIC Group Priority to 4.
  *           Calls the HAL_MspInit() callback function defined in user file
  *           "stm32h7xx_hal_msp.c" to do the global low level hardware initialization
  *
  * @note   SysTick is used as time base for the HAL_Delay() function, the application
  *         need to ensure that the SysTick time base is always set to 1 millisecond
  *         to have correct HAL operation.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_Init(void)
{

uint32_t common_system_clock;

#if defined(DUAL_CORE) && defined(CORE_CM4)
   /* Configure Cortex-M4 Instruction cache through ART accelerator */
   __HAL_RCC_ART_CLK_ENABLE();                   /* Enable the Cortex-M4 ART Clock */
   __HAL_ART_CONFIG_BASE_ADDRESS(0x08100000UL);  /* Configure the Cortex-M4 ART Base address to the Flash Bank 2 : */
   __HAL_ART_ENABLE();                           /* Enable the Cortex-M4 ART */
#endif /* DUAL_CORE &&  CORE_CM4 */

  /* Set Interrupt Group Priority */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Update the SystemCoreClock global variable */
#if defined(RCC_D1CFGR_D1CPRE)
  common_system_clock = HAL_RCC_GetSysClockFreq() >> ((D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_D1CPRE)>> RCC_D1CFGR_D1CPRE_Pos]) & 0x1FU);
#else
  common_system_clock = HAL_RCC_GetSysClockFreq() >> ((D1CorePrescTable[(RCC->CDCFGR1 & RCC_CDCFGR1_CDCPRE)>> RCC_CDCFGR1_CDCPRE_Pos]) & 0x1FU);
#endif

  /* Update the SystemD2Clock global variable */
#if defined(RCC_D1CFGR_HPRE)
  SystemD2Clock = (common_system_clock >> ((D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_HPRE)>> RCC_D1CFGR_HPRE_Pos]) & 0x1FU));
#else
  SystemD2Clock = (common_system_clock >> ((D1CorePrescTable[(RCC->CDCFGR1 & RCC_CDCFGR1_HPRE)>> RCC_CDCFGR1_HPRE_Pos]) & 0x1FU));
#endif

#if defined(DUAL_CORE) && defined(CORE_CM4)
  SystemCoreClock = SystemD2Clock;
#else
  SystemCoreClock = common_system_clock;
#endif /* DUAL_CORE && CORE_CM4 */

  /* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
  if(HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Init the low level hardware */
  HAL_MspInit();

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  This function de-Initializes common part of the HAL and stops the systick.
  *         This function is optional.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DeInit(void)
{
  /* Reset of all peripherals */
  __HAL_RCC_AHB3_FORCE_RESET();
  __HAL_RCC_AHB3_RELEASE_RESET();

  __HAL_RCC_AHB1_FORCE_RESET();
  __HAL_RCC_AHB1_RELEASE_RESET();

  __HAL_RCC_AHB2_FORCE_RESET();
  __HAL_RCC_AHB2_RELEASE_RESET();

  __HAL_RCC_AHB4_FORCE_RESET();
 __HAL_RCC_AHB4_RELEASE_RESET();

  __HAL_RCC_APB3_FORCE_RESET();
  __HAL_RCC_APB3_RELEASE_RESET();

  __HAL_RCC_APB1L_FORCE_RESET();
  __HAL_RCC_APB1L_RELEASE_RESET();

  __HAL_RCC_APB1H_FORCE_RESET();
  __HAL_RCC_APB1H_RELEASE_RESET();

   __HAL_RCC_APB2_FORCE_RESET();
   __HAL_RCC_APB2_RELEASE_RESET();

  __HAL_RCC_APB4_FORCE_RESET();
  __HAL_RCC_APB4_RELEASE_RESET();

  /* De-Init the low level hardware */
  HAL_MspDeInit();

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the MSP.
  * @retval None
  */
__weak void HAL_MspInit(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes the MSP.
  * @retval None
  */
__weak void HAL_MspDeInit(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MspDeInit could be implemented in the user file
   */
}

/**
  * @brief This function configures the source of the time base.
  *        The time source is configured  to have 1ms time base with a dedicated
  *        Tick interrupt priority.
  * @note This function is called  automatically at the beginning of program after
  *       reset by HAL_Init() or at any time when clock is reconfigured  by HAL_RCC_ClockConfig().
  * @note In the default implementation, SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals.
  *       Care must be taken if HAL_Delay() is called from a peripheral ISR process,
  *       The the SysTick interrupt must have higher priority (numerically lower)
  *       than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
  *       The function is declared as __weak  to be overwritten  in case of other
  *       implementation  in user file.
  * @param TickPriority: Tick interrupt priority.
  * @retval HAL status
  */
__weak HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /* Check uwTickFreq for MisraC 2012 (even if uwTickFreq is a enum type that don't take the value zero)*/
  if((uint32_t)uwTickFreq == 0UL)
  {
    return HAL_ERROR;
  }

  /* Configure the SysTick to have interrupt in 1ms time basis*/
  if (HAL_SYSTICK_Config(SystemCoreClock / (1000UL / (uint32_t)uwTickFreq)) > 0U)
  {
    return HAL_ERROR;
  }

  /* Configure the SysTick IRQ priority */
  if (TickPriority < (1UL << __NVIC_PRIO_BITS))
  {
    HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0U);
    uwTickPrio = TickPriority;
  }
  else
  {
    return HAL_ERROR;
  }

  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup HAL_Group2 HAL Control functions
 *  @brief    HAL Control functions
 *
@verbatim
 ===============================================================================
                      ##### HAL Control functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Provide a tick value in millisecond
      (+) Provide a blocking delay in millisecond
      (+) Suspend the time base source interrupt
      (+) Resume the time base source interrupt
      (+) Get the HAL API driver version
      (+) Get the device identifier
      (+) Get the device revision identifier
      (+) Enable/Disable Debug module during SLEEP mode
      (+) Enable/Disable Debug module during STOP mode
      (+) Enable/Disable Debug module during STANDBY mode

@endverbatim
  * @{
  */

/**
  * @brief This function is called to increment  a global variable "uwTick"
  *        used as application time base.
  * @note In the default implementation, this variable is incremented each 1ms
  *       in Systick ISR.
 * @note This function is declared as __weak to be overwritten in case of other
  *      implementations in user file.
  * @retval None
  */
__weak void HAL_IncTick(void)
{
  uwTick += (uint32_t)uwTickFreq;
}

/**
  * @brief Provides a tick value in millisecond.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval tick value
  */
__weak uint32_t HAL_GetTick(void)
{
  return uwTick;
}

/**
  * @brief This function returns a tick priority.
  * @retval tick priority
  */
uint32_t HAL_GetTickPrio(void)
{
  return uwTickPrio;
}

/**
  * @brief Set new tick Freq.
  * @retval Status
  */
HAL_StatusTypeDef HAL_SetTickFreq(HAL_TickFreqTypeDef Freq)
{
  HAL_StatusTypeDef status  = HAL_OK;
  HAL_TickFreqTypeDef prevTickFreq;

  assert_param(IS_TICKFREQ(Freq));

  if (uwTickFreq != Freq)
  {

    /* Back up uwTickFreq frequency */
    prevTickFreq = uwTickFreq;

    /* Update uwTickFreq global variable used by HAL_InitTick() */
    uwTickFreq = Freq;

    /* Apply the new tick Freq  */
    status = HAL_InitTick(uwTickPrio);
    if (status != HAL_OK)
    {
      /* Restore previous tick frequency */
      uwTickFreq = prevTickFreq;
    }
  }

  return status;
}

/**
  * @brief Return tick frequency.
  * @retval tick period in Hz
  */
HAL_TickFreqTypeDef HAL_GetTickFreq(void)
{
  return uwTickFreq;
}

/**
  * @brief This function provides minimum delay (in milliseconds) based
  *        on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay  specifies the delay time length, in milliseconds.
  * @retval None
  */
__weak void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(uwTickFreq);
  }

  while ((HAL_GetTick() - tickstart) < wait)
  {
  }
}

/**
  * @brief Suspend Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_SuspendTick()
  *       is called, the the SysTick interrupt will be disabled and so Tick increment
  *       is suspended.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
__weak void HAL_SuspendTick(void)
{
  /* Disable SysTick Interrupt */
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

/**
  * @brief Resume Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_ResumeTick()
  *       is called, the the SysTick interrupt will be enabled and so Tick increment
  *       is resumed.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
__weak void HAL_ResumeTick(void)
{
  /* Enable SysTick Interrupt */
  SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
}

/**
  * @brief  Returns the HAL revision
  * @retval version : 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t HAL_GetHalVersion(void)
{
 return __STM32H7xx_HAL_VERSION;
}

/**
  * @brief  Returns the device revision identifier.
  * @retval Device revision identifier
  */
uint32_t HAL_GetREVID(void)
{
   return((DBGMCU->IDCODE) >> 16);
}

/**
  * @brief  Returns the device identifier.
  * @retval Device identifier
  */
uint32_t HAL_GetDEVID(void)
{
   return((DBGMCU->IDCODE) & IDCODE_DEVID_MASK);
}

/**
  * @brief  Return the first word of the unique device identifier (UID based on 96 bits)
  * @retval Device identifier
  */
uint32_t HAL_GetUIDw0(void)
{
  return(READ_REG(*((uint32_t *)UID_BASE)));
}

/**
  * @brief  Return the second word of the unique device identifier (UID based on 96 bits)
  * @retval Device identifier
  */
uint32_t HAL_GetUIDw1(void)
{
  return(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
}

/**
  * @brief  Return the third word of the unique device identifier (UID based on 96 bits)
  * @retval Device identifier
  */
uint32_t HAL_GetUIDw2(void)
{
  return(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
}

/**
  * @brief Configure the internal voltage reference buffer voltage scale.
  * @param VoltageScaling  specifies the output voltage to achieve
  *          This parameter can be one of the following values:
  *            @arg SYSCFG_VREFBUF_VOLTAGE_SCALE0: VREF_OUT1 around 2.048 V.
  *                                                This requires VDDA equal to or higher than 2.4 V.
  *            @arg SYSCFG_VREFBUF_VOLTAGE_SCALE1: VREF_OUT2 around 2.5 V.
  *                                                This requires VDDA equal to or higher than 2.8 V.
  *            @arg SYSCFG_VREFBUF_VOLTAGE_SCALE2: VREF_OUT3 around 1.5 V.
  *                                                This requires VDDA equal to or higher than 1.8 V.
  *            @arg SYSCFG_VREFBUF_VOLTAGE_SCALE3: VREF_OUT4 around 1.8 V.
  *                                                This requires VDDA equal to or higher than 2.1 V.
  * @retval None
  */
void HAL_SYSCFG_VREFBUF_VoltageScalingConfig(uint32_t VoltageScaling)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_VREFBUF_VOLTAGE_SCALE(VoltageScaling));

  MODIFY_REG(VREFBUF->CSR, VREFBUF_CSR_VRS, VoltageScaling);
}

/**
  * @brief Configure the internal voltage reference buffer high impedance mode.
  * @param Mode  specifies the high impedance mode
  *          This parameter can be one of the following values:
  *            @arg SYSCFG_VREFBUF_HIGH_IMPEDANCE_DISABLE: VREF+ pin is internally connect to VREFINT output.
  *            @arg SYSCFG_VREFBUF_HIGH_IMPEDANCE_ENABLE: VREF+ pin is high impedance.
  * @retval None
  */
void HAL_SYSCFG_VREFBUF_HighImpedanceConfig(uint32_t Mode)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_VREFBUF_HIGH_IMPEDANCE(Mode));

  MODIFY_REG(VREFBUF->CSR, VREFBUF_CSR_HIZ, Mode);
}

/**
  * @brief  Tune the Internal Voltage Reference buffer (VREFBUF).
  * @retval None
  */
void HAL_SYSCFG_VREFBUF_TrimmingConfig(uint32_t TrimmingValue)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_VREFBUF_TRIMMING(TrimmingValue));

  MODIFY_REG(VREFBUF->CCR, VREFBUF_CCR_TRIM, TrimmingValue);
}

/**
  * @brief  Enable the Internal Voltage Reference buffer (VREFBUF).
  * @retval HAL_OK/HAL_TIMEOUT
  */
HAL_StatusTypeDef HAL_SYSCFG_EnableVREFBUF(void)
{
  uint32_t  tickstart;

  SET_BIT(VREFBUF->CSR, VREFBUF_CSR_ENVR);

  /* Get Start Tick*/
  tickstart = HAL_GetTick();

  /* Wait for VRR bit  */
  while(READ_BIT(VREFBUF->CSR, VREFBUF_CSR_VRR) == 0UL)
  {
    if((HAL_GetTick() - tickstart) > VREFBUF_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Disable the Internal Voltage Reference buffer (VREFBUF).
  *
  * @retval None
  */
void HAL_SYSCFG_DisableVREFBUF(void)
{
  CLEAR_BIT(VREFBUF->CSR, VREFBUF_CSR_ENVR);
}

#if defined(SYSCFG_PMCR_EPIS_SEL)
/**
  * @brief  Ethernet PHY Interface Selection either MII or RMII
  * @param  SYSCFG_ETHInterface: Selects the Ethernet PHY interface
  *   This parameter can be one of the following values:
  *   @arg SYSCFG_ETH_MII : Select the Media Independent Interface
  *   @arg SYSCFG_ETH_RMII: Select the Reduced Media Independent Interface
  * @retval None
  */
void HAL_SYSCFG_ETHInterfaceSelect(uint32_t SYSCFG_ETHInterface)
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_ETHERNET_CONFIG(SYSCFG_ETHInterface));

  MODIFY_REG(SYSCFG->PMCR, SYSCFG_PMCR_EPIS_SEL, (uint32_t)(SYSCFG_ETHInterface));
}
#endif /* SYSCFG_PMCR_EPIS_SEL */

/**
  * @brief  Analog Switch control for dual analog pads.
  * @param  SYSCFG_AnalogSwitch: Selects the analog pad
  *   This parameter can be one or a combination of the following values:
  *   @arg SYSCFG_SWITCH_PA0 : Select PA0 analog switch
  *   @arg SYSCFG_SWITCH_PA1:  Select PA1 analog switch
  *   @arg SYSCFG_SWITCH_PC2 : Select PC2 analog switch
  *   @arg SYSCFG_SWITCH_PC3:  Select PC3 analog switch
  * @param  SYSCFG_SwitchState: Open or Close the analog switch between dual pads (PXn and PXn_C)
  *   This parameter can be one or a combination of the following values:
  *   @arg SYSCFG_SWITCH_PA0_OPEN
  *   @arg SYSCFG_SWITCH_PA0_CLOSE
  *   @arg SYSCFG_SWITCH_PA1_OPEN
  *   @arg SYSCFG_SWITCH_PA1_CLOSE
  *   @arg SYSCFG_SWITCH_PC2_OPEN
  *   @arg SYSCFG_SWITCH_PC2_CLOSE
  *   @arg SYSCFG_SWITCH_PC3_OPEN
  *   @arg SYSCFG_SWITCH_PC3_CLOSE
  * @retval None
  */

void HAL_SYSCFG_AnalogSwitchConfig(uint32_t SYSCFG_AnalogSwitch , uint32_t SYSCFG_SwitchState )
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_ANALOG_SWITCH(SYSCFG_AnalogSwitch));
  assert_param(IS_SYSCFG_SWITCH_STATE(SYSCFG_SwitchState));

  MODIFY_REG(SYSCFG->PMCR, (uint32_t) SYSCFG_AnalogSwitch, (uint32_t)(SYSCFG_SwitchState));
}

#if defined(SYSCFG_PMCR_BOOSTEN)
/**
  * @brief  Enables the booster to reduce the total harmonic distortion of the analog
  *         switch when the supply voltage is lower than 2.7 V.
  * @note   Activating the booster allows to guaranty the analog switch AC performance
  *         when the supply voltage is below 2.7 V: in this case, the analog switch
  *         performance is the same on the full voltage range
  * @retval None
  */
void HAL_SYSCFG_EnableBOOST(void)
{
 SET_BIT(SYSCFG->PMCR, SYSCFG_PMCR_BOOSTEN) ;
}

/**
  * @brief  Disables the booster
  * @note   Activating the booster allows to guaranty the analog switch AC performance
  *         when the supply voltage is below 2.7 V: in this case, the analog switch
  *         performance is the same on the full voltage range
  * @retval None
  */
void HAL_SYSCFG_DisableBOOST(void)
{
 CLEAR_BIT(SYSCFG->PMCR, SYSCFG_PMCR_BOOSTEN) ;
}
#endif /* SYSCFG_PMCR_BOOSTEN */

#if defined (SYSCFG_UR2_BOOT_ADD0) ||  defined (SYSCFG_UR2_BCM7_ADD0)
/**
  * @brief  BootCM7 address 0 configuration
  * @param  BootRegister :Specifies the Boot Address register (Address0 or Address1)
  *   This parameter can be one of the following values:
  *   @arg SYSCFG_BOOT_ADDR0 : Select the boot address0
  *   @arg SYSCFG_BOOT_ADDR1:  Select the boot address1
  * @param  BootAddress :Specifies the CM7 Boot Address to be loaded in Address0 or Address1
  * @retval None
  */
void HAL_SYSCFG_CM7BootAddConfig(uint32_t BootRegister, uint32_t BootAddress)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_BOOT_REGISTER(BootRegister));
  assert_param(IS_SYSCFG_BOOT_ADDRESS(BootAddress));
  if ( BootRegister == SYSCFG_BOOT_ADDR0 )
  {
    /* Configure CM7 BOOT ADD0 */
#if defined(DUAL_CORE)
    MODIFY_REG(SYSCFG->UR2, SYSCFG_UR2_BCM7_ADD0, ((BootAddress >> 16) << SYSCFG_UR2_BCM7_ADD0_Pos));
#else
    MODIFY_REG(SYSCFG->UR2, SYSCFG_UR2_BOOT_ADD0, ((BootAddress >> 16) << SYSCFG_UR2_BOOT_ADD0_Pos));
#endif /*DUAL_CORE*/
  }
  else
  {
    /* Configure CM7 BOOT ADD1 */
#if defined(DUAL_CORE)
    MODIFY_REG(SYSCFG->UR3, SYSCFG_UR3_BCM7_ADD1, (BootAddress >> 16));
#else
    MODIFY_REG(SYSCFG->UR3, SYSCFG_UR3_BOOT_ADD1, (BootAddress >> 16));
#endif /*DUAL_CORE*/
  }
}
#endif /* SYSCFG_UR2_BOOT_ADD0 || SYSCFG_UR2_BCM7_ADD0 */

#if defined(DUAL_CORE)
/**
  * @brief  BootCM4 address 0 configuration
  * @param  BootRegister :Specifies the Boot Address register (Address0 or Address1)
  *   This parameter can be one of the following values:
  *   @arg SYSCFG_BOOT_ADDR0 : Select the boot address0
  *   @arg SYSCFG_BOOT_ADDR1:  Select the boot address1
  * @param  BootAddress :Specifies the CM4 Boot Address to be loaded in Address0 or Address1
  * @retval None
  */
void HAL_SYSCFG_CM4BootAddConfig(uint32_t BootRegister, uint32_t BootAddress)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_BOOT_REGISTER(BootRegister));
  assert_param(IS_SYSCFG_BOOT_ADDRESS(BootAddress));

  if ( BootRegister == SYSCFG_BOOT_ADDR0 )
  {
    /* Configure CM4 BOOT ADD0 */
    MODIFY_REG(SYSCFG->UR3, SYSCFG_UR3_BCM4_ADD0, ((BootAddress >> 16)<< SYSCFG_UR3_BCM4_ADD0_Pos));
  }

  else
  {
    /* Configure CM4 BOOT ADD1 */
    MODIFY_REG(SYSCFG->UR4, SYSCFG_UR4_BCM4_ADD1, (BootAddress >> 16));
  }
}

/**
  * @brief  Enables the Cortex-M7 boot
  * @retval None
  */
void HAL_SYSCFG_EnableCM7BOOT(void)
{
 SET_BIT(SYSCFG->UR1, SYSCFG_UR1_BCM7);
}

/**
  * @brief  Disables the Cortex-M7 boot
  * @note   Disabling the boot will gate the CPU clock
  * @retval None
  */
void HAL_SYSCFG_DisableCM7BOOT(void)
{
 CLEAR_BIT(SYSCFG->UR1, SYSCFG_UR1_BCM7) ;
}

/**
  * @brief  Enables the Cortex-M4 boot
  * @retval None
  */
void HAL_SYSCFG_EnableCM4BOOT(void)
{
 SET_BIT(SYSCFG->UR1, SYSCFG_UR1_BCM4);
}

/**
  * @brief  Disables the Cortex-M4 boot
  * @note   Disabling the boot will gate the CPU clock
  * @retval None
  */
void HAL_SYSCFG_DisableCM4BOOT(void)
{
  CLEAR_BIT(SYSCFG->UR1, SYSCFG_UR1_BCM4);
}
#endif /*DUAL_CORE*/
/**
  * @brief  Enables the I/O Compensation Cell.
  * @note   The I/O compensation cell can be used only when the device supply
  *         voltage ranges from 2.4 to 3.6 V.
  * @retval None
  */
void HAL_EnableCompensationCell(void)
{
  SET_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_EN) ;
}

/**
  * @brief  Power-down the I/O Compensation Cell.
  * @note   The I/O compensation cell can be used only when the device supply
  *         voltage ranges from 2.4 to 3.6 V.
  * @retval None
  */
void HAL_DisableCompensationCell(void)
{
  CLEAR_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_EN);
}


/**
  * @brief  To Enable optimize the I/O speed when the product voltage is low.
  * @note   This bit is active only if PRODUCT_BELOW_25V user option bit is set. It must be
  *         used only if the product supply voltage is below 2.5 V. Setting this bit when VDD is
  *         higher than 2.5 V might be destructive.
  * @retval None
  */
void HAL_SYSCFG_EnableIOSpeedOptimize(void)
{
#if defined(SYSCFG_CCCSR_HSLV)
  SET_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV);
#else
  SET_BIT(SYSCFG->CCCSR, (SYSCFG_CCCSR_HSLV0| SYSCFG_CCCSR_HSLV1 | SYSCFG_CCCSR_HSLV2  | SYSCFG_CCCSR_HSLV3));
#endif   /* SYSCFG_CCCSR_HSLV */
}

/**
  * @brief  To Disable optimize the I/O speed when the product voltage is low.
  * @note   This bit is active only if PRODUCT_BELOW_25V user option bit is set. It must be
  *         used only if the product supply voltage is below 2.5 V. Setting this bit when VDD is
  *         higher than 2.5 V might be destructive.
  * @retval None
  */
void HAL_SYSCFG_DisableIOSpeedOptimize(void)
{
#if defined(SYSCFG_CCCSR_HSLV)
  CLEAR_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV);
#else
  CLEAR_BIT(SYSCFG->CCCSR, (SYSCFG_CCCSR_HSLV0| SYSCFG_CCCSR_HSLV1 | SYSCFG_CCCSR_HSLV2  | SYSCFG_CCCSR_HSLV3));
#endif   /* SYSCFG_CCCSR_HSLV */
}

/**
  * @brief  Code selection for the I/O Compensation cell
  * @param  SYSCFG_CompCode: Selects the code to be applied for the I/O compensation cell
  *   This parameter can be one of the following values:
  *   @arg SYSCFG_CELL_CODE : Select Code from the cell (available in the SYSCFG_CCVR)
  *   @arg SYSCFG_REGISTER_CODE: Select Code from the SYSCFG compensation cell code register (SYSCFG_CCCR)
  * @retval None
  */
void HAL_SYSCFG_CompensationCodeSelect(uint32_t SYSCFG_CompCode)
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_CODE_SELECT(SYSCFG_CompCode));
  MODIFY_REG(SYSCFG->CCCSR, SYSCFG_CCCSR_CS, (uint32_t)(SYSCFG_CompCode));
}

/**
  * @brief  Code selection for the I/O Compensation cell
  * @param  SYSCFG_PMOSCode: PMOS compensation code
  *         This code is applied to the I/O compensation cell when the CS bit of the
  *          SYSCFG_CMPCR is set
  * @param  SYSCFG_NMOSCode: NMOS compensation code
  *         This code is applied to the I/O compensation cell when the CS bit of the
  *          SYSCFG_CMPCR is set
  * @retval None
  */
void HAL_SYSCFG_CompensationCodeConfig(uint32_t SYSCFG_PMOSCode, uint32_t SYSCFG_NMOSCode )
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_CODE_CONFIG(SYSCFG_PMOSCode));
  assert_param(IS_SYSCFG_CODE_CONFIG(SYSCFG_NMOSCode));
  MODIFY_REG(SYSCFG->CCCR, SYSCFG_CCCR_NCC|SYSCFG_CCCR_PCC, (((uint32_t)(SYSCFG_PMOSCode)<< 4)|(uint32_t)(SYSCFG_NMOSCode)) );
}

#if defined(SYSCFG_CCCR_NCC_MMC)
/**
  * @brief  Code selection for the I/O Compensation cell
  * @param  SYSCFG_PMOSCode: VDDMMC PMOS compensation code
  *         This code is applied to the I/O compensation cell when the CS bit of the
  *          SYSCFG_CMPCR is set
  * @param  SYSCFG_NMOSCode: VDDMMC NMOS compensation code
  *         This code is applied to the I/O compensation cell when the CS bit of the
  *          SYSCFG_CMPCR is set
  * @retval None
  */
void HAL_SYSCFG_VDDMMC_CompensationCodeConfig(uint32_t SYSCFG_PMOSCode, uint32_t SYSCFG_NMOSCode )
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_CODE_CONFIG(SYSCFG_PMOSCode));
  assert_param(IS_SYSCFG_CODE_CONFIG(SYSCFG_NMOSCode));
  MODIFY_REG(SYSCFG->CCCR, (SYSCFG_CCCR_NCC_MMC | SYSCFG_CCCR_PCC_MMC), (((uint32_t)(SYSCFG_PMOSCode)<< 4)|(uint32_t)(SYSCFG_NMOSCode)) );
}
#endif /* SYSCFG_CCCR_NCC_MMC */

/**
  * @brief  Enable the Debug Module during Domain1/CDomain SLEEP mode
  * @retval None
  */
void HAL_EnableDBGSleepMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEPD1);
}

/**
  * @brief  Disable the Debug Module during Domain1/CDomain SLEEP mode
  * @retval None
  */
void HAL_DisableDBGSleepMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEPD1);
}


/**
  * @brief  Enable the Debug Module during Domain1/CDomain STOP mode
  * @retval None
  */
void HAL_EnableDBGStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD1);
}

/**
  * @brief  Disable the Debug Module during Domain1/CDomain STOP mode
  * @retval None
  */
void HAL_DisableDBGStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD1);
}

/**
  * @brief  Enable the Debug Module during Domain1/CDomain STANDBY mode
  * @retval None
  */
void HAL_EnableDBGStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD1);
}

/**
  * @brief  Disable the Debug Module during Domain1/CDomain STANDBY mode
  * @retval None
  */
void HAL_DisableDBGStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD1);
}

#if defined(DUAL_CORE)
/**
  * @brief  Enable the Debug Module during Domain1 SLEEP mode
  * @retval None
  */
void HAL_EnableDomain2DBGSleepMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEPD2);
}

/**
  * @brief  Disable the Debug Module during Domain2 SLEEP mode
  * @retval None
  */
void HAL_DisableDomain2DBGSleepMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEPD2);
}

/**
  * @brief  Enable the Debug Module during Domain2 STOP mode
  * @retval None
  */
void HAL_EnableDomain2DBGStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD2);
}

/**
  * @brief  Disable the Debug Module during Domain2 STOP mode
  * @retval None
  */
void HAL_DisableDomain2DBGStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD2);
}

/**
  * @brief  Enable the Debug Module during Domain2 STANDBY mode
  * @retval None
  */
void HAL_EnableDomain2DBGStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD2);
}

/**
  * @brief  Disable the Debug Module during Domain2 STANDBY mode
  * @retval None
  */
void HAL_DisableDomain2DBGStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD2);
}
#endif /*DUAL_CORE*/

/**
  * @brief  Enable the Debug Module during Domain3/SRDomain STOP mode
  * @retval None
  */
void HAL_EnableDomain3DBGStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD3);
}
/**
  * @brief  Disable the Debug Module during Domain3/SRDomain STOP mode
  * @retval None
  */
void HAL_DisableDomain3DBGStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD3);
}

/**
  * @brief  Enable the Debug Module during Domain3/SRDomain STANDBY mode
  * @retval None
  */
void HAL_EnableDomain3DBGStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD3);
}

/**
  * @brief  Disable the Debug Module during Domain3/SRDomain STANDBY mode
  * @retval None
  */
void HAL_DisableDomain3DBGStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD3);
}

/**
  * @brief  Set the FMC Memory Mapping Swapping config.
  * @param  BankMapConfig: Defines the FMC Bank mapping configuration. This parameter can be
            FMC_SWAPBMAP_DISABLE, FMC_SWAPBMAP_SDRAM_SRAM, FMC_SWAPBMAP_SDRAMB2
  * @retval HAL state
  */
void HAL_SetFMCMemorySwappingConfig(uint32_t BankMapConfig)
{
  /* Check the parameter */
  assert_param(IS_FMC_SWAPBMAP_MODE(BankMapConfig));
  MODIFY_REG(FMC_Bank1_R->BTCR[0], FMC_BCR1_BMAP, BankMapConfig);
}

/**
  * @brief  Get FMC Bank mapping mode.
  * @retval The FMC Bank mapping mode. This parameter can be
            FMC_SWAPBMAP_DISABLE, FMC_SWAPBMAP_SDRAM_SRAM, FMC_SWAPBMAP_SDRAMB2
*/
uint32_t HAL_GetFMCMemorySwappingConfig(void)
{
  return READ_BIT(FMC_Bank1_R->BTCR[0], FMC_BCR1_BMAP);
}

/**
  * @brief  Configure the EXTI input event line edge
  * @note    No edge configuration for direct lines but for configurable lines:(EXTI_LINE0..EXTI_LINE21),
  *          EXTI_LINE49,EXTI_LINE51,EXTI_LINE82,EXTI_LINE84,EXTI_LINE85 and EXTI_LINE86.
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *         (EXTI_LINE0....EXTI_LINE87)excluding :line45, line81,line83 which are reserved
  * @param   EXTI_Edge: Specifies  EXTI line Edge used.
  *          This parameter can be one of the following values :
  *   @arg EXTI_RISING_EDGE : Configurable line, with Rising edge trigger detection
  *   @arg EXTI_FALLING_EDGE: Configurable line, with Falling edge trigger detection
  * @retval None
  */
void HAL_EXTI_EdgeConfig(uint32_t EXTI_Line , uint32_t EXTI_Edge )
{
  /* Check the parameter */
  assert_param(IS_HAL_EXTI_CONFIG_LINE(EXTI_Line));
  assert_param(IS_EXTI_EDGE_LINE(EXTI_Edge));

  /* Clear Rising Falling edge configuration */
  CLEAR_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI->FTSR1)) + ((EXTI_Line >> 5 ) * 0x20UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
  CLEAR_BIT( *(__IO uint32_t *) (((uint32_t) &(EXTI->RTSR1)) + ((EXTI_Line >> 5 ) * 0x20UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));

  if( (EXTI_Edge & EXTI_RISING_EDGE) == EXTI_RISING_EDGE)
  {
   SET_BIT( *(__IO uint32_t *) (((uint32_t) &(EXTI->RTSR1)) + ((EXTI_Line >> 5 ) * 0x20UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
  }
  if( (EXTI_Edge & EXTI_FALLING_EDGE) == EXTI_FALLING_EDGE)
  {
   SET_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI->FTSR1)) + ((EXTI_Line >> 5 ) * 0x20UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
  }
}

/**
  * @brief  Generates a Software interrupt on selected EXTI line.
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *          (EXTI_LINE0..EXTI_LINE21),EXTI_LINE49,EXTI_LINE51,EXTI_LINE82,EXTI_LINE84,EXTI_LINE85 and EXTI_LINE86.
  * @retval None
  */
void HAL_EXTI_GenerateSWInterrupt(uint32_t EXTI_Line)
{
  /* Check the parameters */
  assert_param(IS_HAL_EXTI_CONFIG_LINE(EXTI_Line));

  SET_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI->SWIER1)) + ((EXTI_Line >> 5 ) * 0x20UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
}


/**
  * @brief  Clears the EXTI's line pending flags for Domain D1
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *         (EXTI_LINE0....EXTI_LINE87)excluding :line45, line81,line83 which are reserved
  * @retval None
  */
void HAL_EXTI_D1_ClearFlag(uint32_t EXTI_Line)
{
  /* Check the parameters */
 assert_param(IS_EXTI_D1_LINE(EXTI_Line));
 WRITE_REG(*(__IO uint32_t *) (((uint32_t) &(EXTI_D1->PR1)) + ((EXTI_Line >> 5 ) * 0x10UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));

}

#if defined(DUAL_CORE)
/**
  * @brief  Clears the EXTI's line pending flags for Domain D2
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *         (EXTI_LINE0....EXTI_LINE87)excluding :line45, line81,line83 which are reserved
  * @retval None
  */
void HAL_EXTI_D2_ClearFlag(uint32_t EXTI_Line)
{
  /* Check the parameters */
 assert_param(IS_EXTI_D2_LINE(EXTI_Line));
 WRITE_REG(*(__IO uint32_t *) (((uint32_t) &(EXTI_D2->PR1)) + ((EXTI_Line >> 5 ) * 0x10UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
}

#endif /*DUAL_CORE*/
/**
  * @brief  Configure the EXTI input event line for Domain D1
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *         (EXTI_LINE0....EXTI_LINE87)excluding :line45, line81,line83 which are reserved
  * @param   EXTI_Mode: Specifies which EXTI line is used as interrupt or an event.
  *          This parameter can be one or a combination of the following values :
  *   @arg EXTI_MODE_IT :  Interrupt Mode selected
  *   @arg EXTI_MODE_EVT : Event Mode selected
  * @param   EXTI_LineCmd controls (Enable/Disable) the EXTI line.

  * @retval None
  */
void HAL_EXTI_D1_EventInputConfig(uint32_t EXTI_Line , uint32_t EXTI_Mode,  uint32_t EXTI_LineCmd )
{
  /* Check the parameter */
  assert_param(IS_EXTI_D1_LINE(EXTI_Line));
  assert_param(IS_EXTI_MODE_LINE(EXTI_Mode));

  if( (EXTI_Mode & EXTI_MODE_IT) == EXTI_MODE_IT)
  {
     if( EXTI_LineCmd == 0UL)
     {
       /* Clear EXTI line configuration */
        CLEAR_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI_D1->IMR1)) + ((EXTI_Line >> 5 ) * 0x10UL)),(uint32_t)(1UL << (EXTI_Line & 0x1FUL)) );
     }
     else
     {
        SET_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI_D1->IMR1)) + ((EXTI_Line >> 5 ) * 0x10UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
     }
  }

  if( (EXTI_Mode & EXTI_MODE_EVT) == EXTI_MODE_EVT)
  {
    if( EXTI_LineCmd == 0UL)
    {
      /* Clear EXTI line configuration */
      CLEAR_BIT(  *(__IO uint32_t *) (((uint32_t) &(EXTI_D1->EMR1)) + ((EXTI_Line >> 5 ) * 0x10UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
    }
    else
    {
      SET_BIT(  *(__IO uint32_t *) (((uint32_t) &(EXTI_D1->EMR1)) + ((EXTI_Line >> 5 ) * 0x10UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
    }
  }
}

#if defined(DUAL_CORE)
/**
  * @brief  Configure the EXTI input event line for Domain D2
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *         (EXTI_LINE0....EXTI_LINE87)excluding :line45, line81,line83 which are reserved
  * @param   EXTI_Mode: Specifies which EXTI line is used as interrupt or an event.
  *          This parameter can be one or a combination of the following values :
  *   @arg EXTI_MODE_IT :  Interrupt Mode selected
  *   @arg EXTI_MODE_EVT : Event Mode selected
  * @param   EXTI_LineCmd controls (Enable/Disable) the EXTI line.

  * @retval None
  */
void HAL_EXTI_D2_EventInputConfig(uint32_t EXTI_Line , uint32_t EXTI_Mode,  uint32_t EXTI_LineCmd )
{
  /* Check the parameter */
  assert_param(IS_EXTI_D2_LINE(EXTI_Line));
  assert_param(IS_EXTI_MODE_LINE(EXTI_Mode));

  if( (EXTI_Mode & EXTI_MODE_IT) == EXTI_MODE_IT)
  {
    if( EXTI_LineCmd == 0UL)
    {
    /* Clear EXTI line configuration */
     CLEAR_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI_D2->IMR1)) + ((EXTI_Line >> 5 ) * 0x10UL)),(uint32_t)(1UL << (EXTI_Line & 0x1FUL)) );
    }
    else
    {
     SET_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI_D2->IMR1)) + ((EXTI_Line >> 5 ) * 0x10UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
    }
  }

  if( (EXTI_Mode & EXTI_MODE_EVT) == EXTI_MODE_EVT)
  {
    if( EXTI_LineCmd == 0UL)
    {
      /* Clear EXTI line configuration */
      CLEAR_BIT(  *(__IO uint32_t *) (((uint32_t) &(EXTI_D2->EMR1)) + ((EXTI_Line >> 5 ) * 0x10UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
    }
    else
    {
      SET_BIT(  *(__IO uint32_t *) (((uint32_t) &(EXTI_D2->EMR1)) + ((EXTI_Line >> 5 ) * 0x10UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
    }
  }
}
#endif /*DUAL_CORE*/

/**
  * @brief  Configure the EXTI input event line for Domain D3
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *         (EXTI_LINE0...EXTI_LINE15),(EXTI_LINE19...EXTI_LINE21),EXTI_LINE25, EXTI_LINE34,
  *          EXTI_LINE35,EXTI_LINE41,(EXTI_LINE48...EXTI_LINE53)
  * @param   EXTI_LineCmd controls (Enable/Disable) the EXTI line.
  * @param   EXTI_ClearSrc: Specifies the clear source of D3 pending event.
  *          This parameter can be one of the following values :
  *   @arg BDMA_CH6_CLEAR : BDMA ch6 event selected as D3 domain pendclear source
  *   @arg BDMA_CH7_CLEAR : BDMA ch7 event selected as D3 domain pendclear source
  *   @arg LPTIM4_OUT_CLEAR : LPTIM4 out selected as D3 domain pendclear source
  *   @arg LPTIM5_OUT_CLEAR : LPTIM5 out selected as D3 domain pendclear source
  * @retval None
  */
void HAL_EXTI_D3_EventInputConfig(uint32_t EXTI_Line, uint32_t EXTI_LineCmd , uint32_t EXTI_ClearSrc  )
{
  __IO uint32_t *pRegv;

  /* Check the parameter */
  assert_param(IS_EXTI_D3_LINE(EXTI_Line));
  assert_param(IS_EXTI_D3_CLEAR(EXTI_ClearSrc));

  if( EXTI_LineCmd == 0UL)
  {
    /* Clear EXTI line configuration */
    CLEAR_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI->D3PMR1)) + ((EXTI_Line >> 5 ) * 0x20UL)),(uint32_t)(1UL << (EXTI_Line & 0x1FUL)) );
  }
  else
  {
    SET_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI->D3PMR1)) +((EXTI_Line >> 5 ) * 0x20UL)), (uint32_t)(1UL << (EXTI_Line & 0x1FUL)));
  }

  if(((EXTI_Line>>4)%2UL) == 0UL)
  {
    pRegv = (__IO uint32_t *) (((uint32_t) &(EXTI->D3PCR1L)) + ((EXTI_Line >> 5 ) * 0x20UL));
  }
  else
  {
    pRegv = (__IO uint32_t *) (((uint32_t) &(EXTI->D3PCR1H)) + ((EXTI_Line >> 5 ) * 0x20UL));
  }
  MODIFY_REG(*pRegv, (uint32_t)(3UL << ((EXTI_Line*2UL) & 0x1FUL)), (uint32_t)(EXTI_ClearSrc << ((EXTI_Line*2UL) & 0x1FUL)));

}



/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
