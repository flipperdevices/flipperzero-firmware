SET (CHIBIOS_SOURCES_hal
    os/hal/src/hal.c
    os/hal/src/hal_st.c
    os/hal/src/hal_buffers.c
    os/hal/src/hal_queues.c
    os/hal/src/hal_mmcsd.c
    os/hal/ports/common/ARMCMx/nvic.c
)

SET (CHIBIOS_INCLUDES_hal
    os/hal/include
    os/hal/ports/common/ARMCMx
)

SET (CHIBIOS_SOURCES_hal_nil
    os/hal/osal/nil/osal.c
)

SET (CHIBIOS_SOURCES_hal_rt
    os/hal/osal/rt/osal.c
)

SET (CHIBIOS_INCLUDES_hal_nil
    os/hal/osal/nil
)

SET (CHIBIOS_INCLUDES_hal_rt
    os/hal/osal/rt
)

SET (CHIBIOS_SOURCES_hal_ADC         os/hal/src/hal_adc.c)
SET (CHIBIOS_SOURCES_hal_CAN         os/hal/src/hal_can.c)
SET (CHIBIOS_SOURCES_hal_CRY         os/hal/src/hal_crypto.c)
SET (CHIBIOS_SOURCES_hal_DAC         os/hal/src/hal_dac.c)
SET (CHIBIOS_SOURCES_hal_EXT         os/hal/src/hal_ext.c)
SET (CHIBIOS_SOURCES_hal_GPT         os/hal/src/hal_gpt.c)
SET (CHIBIOS_SOURCES_hal_I2C         os/hal/src/hal_i2c.c)
SET (CHIBIOS_SOURCES_hal_I2S         os/hal/src/hal_i2s.c)
SET (CHIBIOS_SOURCES_hal_ICU         os/hal/src/hal_icu.c)
SET (CHIBIOS_SOURCES_hal_MAC         os/hal/src/hal_mac.c)
SET (CHIBIOS_SOURCES_hal_MMC_SPI     os/hal/src/hal_mmc_spi.c)
SET (CHIBIOS_SOURCES_hal_PAL         os/hal/src/hal_pal.c)
SET (CHIBIOS_SOURCES_hal_PWM         os/hal/src/hal_pwm.c)
SET (CHIBIOS_SOURCES_hal_QSPI        os/hal/src/hal_qspi.c)
SET (CHIBIOS_SOURCES_hal_RTC         os/hal/src/hal_rtc.c)
SET (CHIBIOS_SOURCES_hal_SDC         os/hal/src/hal_sdc.c)
SET (CHIBIOS_SOURCES_hal_SERIAL      os/hal/src/hal_serial.c)
SET (CHIBIOS_SOURCES_hal_SERIAL_USB  os/hal/src/hal_serial_usb.c)
SET (CHIBIOS_SOURCES_hal_SPI         os/hal/src/hal_spi.c)
SET (CHIBIOS_SOURCES_hal_UART        os/hal/src/hal_uart.c)
SET (CHIBIOS_SOURCES_hal_USB         os/hal/src/hal_usb.c)
SET (CHIBIOS_SOURCES_hal_WDG         os/hal/src/hal_wdg.c)



SET (CHIBIOS_INCLUDES_hal_F0
    os/hal/ports/STM32/STM32F0xx
    os/hal/ports/STM32/LLD/DMAv1
    os/hal/ports/STM32/LLD/TIMv1
)
SET (CHIBIOS_SOURCES_hal_F0
    os/hal/ports/STM32/STM32F0xx/stm32_isr.c
    os/hal/ports/STM32/STM32F0xx/hal_lld.c
    os/hal/ports/STM32/LLD/DMAv1/stm32_dma.c
    os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
)
SET (CHIBIOS_SOURCES_hal_ADC_F0     os/hal/ports/STM32/LLD/ADCv1/hal_adc_lld.c)
SET (CHIBIOS_SOURCES_hal_CAN_F0     os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c)
SET (CHIBIOS_SOURCES_hal_DAC_F0     os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c)
SET (CHIBIOS_SOURCES_hal_EXT_F0     os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c)
SET (CHIBIOS_SOURCES_hal_PAL_F0     os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c)
SET (CHIBIOS_SOURCES_hal_I2C_F0     os/hal/ports/STM32/LLD/I2Cv2/hal_i2c_lld.c)
SET (CHIBIOS_SOURCES_hal_RTC_F0     os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c)
SET (CHIBIOS_SOURCES_hal_I2S_F0     os/hal/ports/STM32/LLD/SPIv2/hal_i2s_lld.c)
SET (CHIBIOS_SOURCES_hal_SPI_F0     os/hal/ports/STM32/LLD/SPIv2/hal_spi_lld.c)
SET (CHIBIOS_SOURCES_hal_GPT_F0     os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c)
SET (CHIBIOS_SOURCES_hal_ICU_F0     os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c)
SET (CHIBIOS_SOURCES_hal_PWM_F0     os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c)
SET (CHIBIOS_SOURCES_hal_SERIAL_F0  os/hal/ports/STM32/LLD/USARTv2/hal_serial_lld.c)
SET (CHIBIOS_SOURCES_hal_UART_F0    os/hal/ports/STM32/LLD/USARTv2/hal_uart_lld.c)
SET (CHIBIOS_SOURCES_hal_USB_F0     os/hal/ports/STM32/LLD/USBv1/hal_usb_lld.c)
SET (CHIBIOS_SOURCES_hal_WDG_F0     os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c)

SET (CHIBIOS_INCLUDES_hal_ADC_F0    os/hal/ports/STM32/LLD/ADCv1)
SET (CHIBIOS_INCLUDES_hal_CAN_F0    os/hal/ports/STM32/LLD/CANv1)
SET (CHIBIOS_INCLUDES_hal_DAC_F0    os/hal/ports/STM32/LLD/DACv1)
SET (CHIBIOS_INCLUDES_hal_EXT_F0    os/hal/ports/STM32/LLD/EXTIv1)
SET (CHIBIOS_INCLUDES_hal_PAL_F0    os/hal/ports/STM32/LLD/GPIOv2)
SET (CHIBIOS_INCLUDES_hal_I2C_F0    os/hal/ports/STM32/LLD/I2Cv2)
SET (CHIBIOS_INCLUDES_hal_RTC_F0    os/hal/ports/STM32/LLD/RTCv2)
SET (CHIBIOS_INCLUDES_hal_I2S_F0    os/hal/ports/STM32/LLD/SPIv2)
SET (CHIBIOS_INCLUDES_hal_SPI_F0    os/hal/ports/STM32/LLD/SPIv2)
SET (CHIBIOS_INCLUDES_hal_GPT_F0    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_ICU_F0    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_PWM_F0    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_SERIAL_F0 os/hal/ports/STM32/LLD/USARTv2)
SET (CHIBIOS_INCLUDES_hal_UART_F0   os/hal/ports/STM32/LLD/USARTv2)
SET (CHIBIOS_INCLUDES_hal_USB_F0    os/hal/ports/STM32/LLD/USBv1)
SET (CHIBIOS_INCLUDES_hal_WDG_F0    os/hal/ports/STM32/LLD/xWDGv1)


SET (CHIBIOS_INCLUDES_hal_F1
    os/hal/ports/STM32/STM32F1xx
    os/hal/ports/STM32/LLD/DMAv1
    os/hal/ports/STM32/LLD/TIMv1
)
SET (CHIBIOS_SOURCES_hal_F1
    os/hal/ports/STM32/STM32F1xx/stm32_isr.c
    os/hal/ports/STM32/STM32F1xx/hal_lld.c
    os/hal/ports/STM32/LLD/DMAv1/stm32_dma.c
    os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
)
SET (CHIBIOS_SOURCES_hal_ADC_F1     os/hal/ports/STM32/STM32F1xx/hal_adc_lld.c)
SET (CHIBIOS_SOURCES_hal_CAN_F1     os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c)
SET (CHIBIOS_SOURCES_hal_DAC_F1     os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c)
SET (CHIBIOS_SOURCES_hal_EXT_F1     os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c)
SET (CHIBIOS_SOURCES_hal_PAL_F1     os/hal/ports/STM32/LLD/GPIOv1/hal_pal_lld.c)
SET (CHIBIOS_SOURCES_hal_I2C_F1     os/hal/ports/STM32/LLD/I2Cv1/hal_i2c_lld.c)
SET (CHIBIOS_SOURCES_hal_RTC_F1     os/hal/ports/STM32/LLD/RTCv1/hal_rtc_lld.c)
SET (CHIBIOS_SOURCES_hal_I2S_F1     os/hal/ports/STM32/LLD/SPIv1/hal_i2s_lld.c)
SET (CHIBIOS_SOURCES_hal_SPI_F1     os/hal/ports/STM32/LLD/SPIv1/hal_spi_lld.c)
SET (CHIBIOS_SOURCES_hal_GPT_F1     os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c)
SET (CHIBIOS_SOURCES_hal_ICU_F1     os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c)
SET (CHIBIOS_SOURCES_hal_PWM_F1     os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c)
SET (CHIBIOS_SOURCES_hal_SERIAL_F1  os/hal/ports/STM32/LLD/USARTv1/hal_serial_lld.c)
SET (CHIBIOS_SOURCES_hal_UART_F1    os/hal/ports/STM32/LLD/USARTv1/hal_uart_lld.c)
SET (CHIBIOS_SOURCES_hal_USB_F1     os/hal/ports/STM32/LLD/USBv1/hal_usb_lld.c)
SET (CHIBIOS_SOURCES_hal_WDG_F1     os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c)

SET (CHIBIOS_INCLUDES_hal_ADC_F1    os/hal/ports/STM32/STM32F1xx)
SET (CHIBIOS_INCLUDES_hal_CAN_F1    os/hal/ports/STM32/LLD/CANv1)
SET (CHIBIOS_INCLUDES_hal_DAC_F1    os/hal/ports/STM32/LLD/DACv1)
SET (CHIBIOS_INCLUDES_hal_EXT_F1    os/hal/ports/STM32/LLD/EXTIv1)
SET (CHIBIOS_INCLUDES_hal_PAL_F1    os/hal/ports/STM32/LLD/GPIOv1)
SET (CHIBIOS_INCLUDES_hal_I2C_F1    os/hal/ports/STM32/LLD/I2Cv1)
SET (CHIBIOS_INCLUDES_hal_RTC_F1    os/hal/ports/STM32/LLD/RTCv1)
SET (CHIBIOS_INCLUDES_hal_I2S_F1    os/hal/ports/STM32/LLD/SPIv1)
SET (CHIBIOS_INCLUDES_hal_SPI_F1    os/hal/ports/STM32/LLD/SPIv1)
SET (CHIBIOS_INCLUDES_hal_GPT_F1    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_ICU_F1    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_PWM_F1    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_SERIAL_F1 os/hal/ports/STM32/LLD/USARTv1)
SET (CHIBIOS_INCLUDES_hal_UART_F1   os/hal/ports/STM32/LLD/USARTv1)
SET (CHIBIOS_INCLUDES_hal_USB_F1    os/hal/ports/STM32/LLD/USBv1)
SET (CHIBIOS_INCLUDES_hal_WDG_F1    os/hal/ports/STM32/LLD/xWDGv1)

SET (CHIBIOS_INCLUDES_hal_F2
    os/hal/ports/STM32/STM32F4xx
    os/hal/ports/STM32/LLD/DMAv2
    os/hal/ports/STM32/LLD/TIMv1
)
SET (CHIBIOS_SOURCES_hal_F2
    os/hal/ports/STM32/STM32F4xx/stm32_isr.c
    os/hal/ports/STM32/STM32F4xx/hal_lld.c
    os/hal/ports/STM32/LLD/DMAv2/stm32_dma.c
    os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
)

SET (CHIBIOS_SOURCES_hal_ADC_F2     os/hal/ports/STM32/LLD/ADCv2/hal_adc_lld.c)
SET (CHIBIOS_SOURCES_hal_CAN_F2     os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c)
SET (CHIBIOS_SOURCES_hal_DAC_F2     os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c)
SET (CHIBIOS_SOURCES_hal_EXT_F2     os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c)
SET (CHIBIOS_SOURCES_hal_PAL_F2     os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c)
SET (CHIBIOS_SOURCES_hal_I2C_F2     os/hal/ports/STM32/LLD/I2Cv1/hal_i2c_lld.c)
SET (CHIBIOS_SOURCES_hal_MAC_F2     os/hal/ports/STM32/LLD/MACv1/hal_mac_lld.c)
SET (CHIBIOS_SOURCES_hal_USB_F2     os/hal/ports/STM32/LLD/OTGv1/hal_usb_lld.c)
SET (CHIBIOS_SOURCES_hal_QSPI_F2    os/hal/ports/STM32/LLD/QUADSPIv1/hal_qspi_lld.c)
SET (CHIBIOS_SOURCES_hal_RTC_F2     os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c)
SET (CHIBIOS_SOURCES_hal_I2S_F2     os/hal/ports/STM32/LLD/SPIv1/hal_i2s_lld.c)
SET (CHIBIOS_SOURCES_hal_SPI_F2     os/hal/ports/STM32/LLD/SPIv1/hal_spi_lld.c)
SET (CHIBIOS_SOURCES_hal_SDC_F2     os/hal/ports/STM32/LLD/SDIOv1/hal_sdc_lld.c)
SET (CHIBIOS_SOURCES_hal_GPT_F2     os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c)
SET (CHIBIOS_SOURCES_hal_ICU_F2     os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c)
SET (CHIBIOS_SOURCES_hal_PWM_F2     os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c)
SET (CHIBIOS_SOURCES_hal_SERIAL_F2  os/hal/ports/STM32/LLD/USARTv1/hal_serial_lld.c)
SET (CHIBIOS_SOURCES_hal_UART_F2    os/hal/ports/STM32/LLD/USARTv1/hal_uart_lld.c)
SET (CHIBIOS_SOURCES_hal_WDG_F2     os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c)

SET (CHIBIOS_INCLUDES_hal_ADC_F2    os/hal/ports/STM32/LLD/ADCv2)
SET (CHIBIOS_INCLUDES_hal_CAN_F2    os/hal/ports/STM32/LLD/CANv1)
SET (CHIBIOS_INCLUDES_hal_DAC_F2    os/hal/ports/STM32/LLD/DACv1)
SET (CHIBIOS_INCLUDES_hal_EXT_F2    os/hal/ports/STM32/LLD/EXTIv1)
SET (CHIBIOS_INCLUDES_hal_PAL_F2    os/hal/ports/STM32/LLD/GPIOv2)
SET (CHIBIOS_INCLUDES_hal_I2C_F2    os/hal/ports/STM32/LLD/I2Cv1)
SET (CHIBIOS_INCLUDES_hal_MAC_F2    os/hal/ports/STM32/LLD/MACv1)
SET (CHIBIOS_INCLUDES_hal_USB_F2    os/hal/ports/STM32/LLD/OTGv1)
SET (CHIBIOS_INCLUDES_hal_QSPI_F2   os/hal/ports/STM32/LLD/QUADSPIv1)
SET (CHIBIOS_INCLUDES_hal_RTC_F2    os/hal/ports/STM32/LLD/RTCv2)
SET (CHIBIOS_INCLUDES_hal_I2S_F2    os/hal/ports/STM32/LLD/SPIv1)
SET (CHIBIOS_INCLUDES_hal_SPI_F2    os/hal/ports/STM32/LLD/SPIv1)
SET (CHIBIOS_INCLUDES_hal_SDC_F2    os/hal/ports/STM32/LLD/SDIOv1)
SET (CHIBIOS_INCLUDES_hal_GPT_F2    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_ICU_F2    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_PWM_F2    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_SERIAL_F2 os/hal/ports/STM32/LLD/USARTv1)
SET (CHIBIOS_INCLUDES_hal_UART_F2   os/hal/ports/STM32/LLD/USARTv1)
SET (CHIBIOS_INCLUDES_hal_WDG_F2    os/hal/ports/STM32/LLD/xWDGv1)


SET (CHIBIOS_INCLUDES_hal_F3
    os/hal/ports/STM32/STM32F3xx
    os/hal/ports/STM32/LLD/DMAv1
    os/hal/ports/STM32/LLD/TIMv1
)
SET (CHIBIOS_SOURCES_hal_F3
    os/hal/ports/STM32/STM32F3xx/stm32_isr.c
    os/hal/ports/STM32/STM32F3xx/hal_lld.c
    os/hal/ports/STM32/LLD/DMAv1/stm32_dma.c
    os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
)

SET (CHIBIOS_SOURCES_hal_ADC_F3     os/hal/ports/STM32/LLD/ADCv3/hal_adc_lld.c)
SET (CHIBIOS_SOURCES_hal_CAN_F3     os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c)
SET (CHIBIOS_SOURCES_hal_DAC_F3     os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c)
SET (CHIBIOS_SOURCES_hal_EXT_F3     os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c)
SET (CHIBIOS_SOURCES_hal_PAL_F3     os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c)
SET (CHIBIOS_SOURCES_hal_I2C_F3     os/hal/ports/STM32/LLD/I2Cv2/hal_i2c_lld.c)
SET (CHIBIOS_SOURCES_hal_RTC_F3     os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c)
SET (CHIBIOS_SOURCES_hal_I2S_F3     os/hal/ports/STM32/LLD/SPIv2/hal_i2s_lld.c)
SET (CHIBIOS_SOURCES_hal_SPI_F3     os/hal/ports/STM32/LLD/SPIv2/hal_spi_lld.c)
SET (CHIBIOS_SOURCES_hal_GPT_F3     os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c)
SET (CHIBIOS_SOURCES_hal_ICU_F3     os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c)
SET (CHIBIOS_SOURCES_hal_PWM_F3     os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c)
SET (CHIBIOS_SOURCES_hal_SERIAL_F3  os/hal/ports/STM32/LLD/USARTv2/hal_serial_lld.c)
SET (CHIBIOS_SOURCES_hal_UART_F3    os/hal/ports/STM32/LLD/USARTv2/hal_uart_lld.c)
SET (CHIBIOS_SOURCES_hal_USB_F3     os/hal/ports/STM32/LLD/USBv1/hal_usb_lld.c)
SET (CHIBIOS_SOURCES_hal_WDG_F3     os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c)

SET (CHIBIOS_INCLUDES_hal_ADC_F3    os/hal/ports/STM32/LLD/ADCv3)
SET (CHIBIOS_INCLUDES_hal_CAN_F3    os/hal/ports/STM32/LLD/CANv1)
SET (CHIBIOS_INCLUDES_hal_DAC_F3    os/hal/ports/STM32/LLD/DACv1)
SET (CHIBIOS_INCLUDES_hal_EXT_F3    os/hal/ports/STM32/LLD/EXTIv1)
SET (CHIBIOS_INCLUDES_hal_PAL_F3    os/hal/ports/STM32/LLD/GPIOv2)
SET (CHIBIOS_INCLUDES_hal_I2C_F3    os/hal/ports/STM32/LLD/I2Cv2)
SET (CHIBIOS_INCLUDES_hal_RTC_F3    os/hal/ports/STM32/LLD/RTCv2)
SET (CHIBIOS_INCLUDES_hal_I2S_F3    os/hal/ports/STM32/LLD/SPIv2)
SET (CHIBIOS_INCLUDES_hal_SPI_F3    os/hal/ports/STM32/LLD/SPIv2)
SET (CHIBIOS_INCLUDES_hal_GPT_F3    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_ICU_F3    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_PWM_F3    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_SERIAL_F3 os/hal/ports/STM32/LLD/USARTv2)
SET (CHIBIOS_INCLUDES_hal_UART_F3   os/hal/ports/STM32/LLD/USARTv2)
SET (CHIBIOS_INCLUDES_hal_USB_F3    os/hal/ports/STM32/LLD/USBv1)
SET (CHIBIOS_INCLUDES_hal_WDG_F3    os/hal/ports/STM32/LLD/xWDGv1)



SET (CHIBIOS_INCLUDES_hal_F4
    os/hal/ports/STM32/STM32F4xx
    os/hal/ports/STM32/LLD/DMAv2
    os/hal/ports/STM32/LLD/TIMv1
)
SET (CHIBIOS_SOURCES_hal_F4
    os/hal/ports/STM32/STM32F4xx/stm32_isr.c
    os/hal/ports/STM32/STM32F4xx/hal_lld.c
    os/hal/ports/STM32/LLD/DMAv2/stm32_dma.c
    os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
)

SET (CHIBIOS_SOURCES_hal_ADC_F4     os/hal/ports/STM32/LLD/ADCv2/hal_adc_lld.c)
SET (CHIBIOS_SOURCES_hal_CAN_F4     os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c)
SET (CHIBIOS_SOURCES_hal_DAC_F4     os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c)
SET (CHIBIOS_SOURCES_hal_EXT_F4     os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c)
SET (CHIBIOS_SOURCES_hal_PAL_F4     os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c)
SET (CHIBIOS_SOURCES_hal_I2C_F4     os/hal/ports/STM32/LLD/I2Cv1/hal_i2c_lld.c)
SET (CHIBIOS_SOURCES_hal_MAC_F4     os/hal/ports/STM32/LLD/MACv1/hal_mac_lld.c)
SET (CHIBIOS_SOURCES_hal_USB_F4     os/hal/ports/STM32/LLD/OTGv1/hal_usb_lld.c)
SET (CHIBIOS_SOURCES_hal_QSPI_F4    os/hal/ports/STM32/LLD/QUADSPIv1/hal_qspi_lld.c)
SET (CHIBIOS_SOURCES_hal_RTC_F4     os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c)
SET (CHIBIOS_SOURCES_hal_I2S_F4     os/hal/ports/STM32/LLD/SPIv1/hal_i2s_lld.c)
SET (CHIBIOS_SOURCES_hal_SPI_F4     os/hal/ports/STM32/LLD/SPIv1/hal_spi_lld.c)
SET (CHIBIOS_SOURCES_hal_SDC_F4     os/hal/ports/STM32/LLD/SDIOv1/hal_sdc_lld.c)
SET (CHIBIOS_SOURCES_hal_GPT_F4     os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c)
SET (CHIBIOS_SOURCES_hal_ICU_F4     os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c)
SET (CHIBIOS_SOURCES_hal_PWM_F4     os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c)
SET (CHIBIOS_SOURCES_hal_SERIAL_F4  os/hal/ports/STM32/LLD/USARTv1/hal_serial_lld.c)
SET (CHIBIOS_SOURCES_hal_UART_F4    os/hal/ports/STM32/LLD/USARTv1/hal_uart_lld.c)
SET (CHIBIOS_SOURCES_hal_WDG_F4     os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c)

SET (CHIBIOS_INCLUDES_hal_ADC_F4    os/hal/ports/STM32/LLD/ADCv2)
SET (CHIBIOS_INCLUDES_hal_CAN_F4    os/hal/ports/STM32/LLD/CANv1)
SET (CHIBIOS_INCLUDES_hal_DAC_F4    os/hal/ports/STM32/LLD/DACv1)
SET (CHIBIOS_INCLUDES_hal_EXT_F4    os/hal/ports/STM32/LLD/EXTIv1)
SET (CHIBIOS_INCLUDES_hal_PAL_F4    os/hal/ports/STM32/LLD/GPIOv2)
SET (CHIBIOS_INCLUDES_hal_I2C_F4    os/hal/ports/STM32/LLD/I2Cv1)
SET (CHIBIOS_INCLUDES_hal_MAC_F4    os/hal/ports/STM32/LLD/MACv1)
SET (CHIBIOS_INCLUDES_hal_USB_F4    os/hal/ports/STM32/LLD/OTGv1)
SET (CHIBIOS_INCLUDES_hal_QSPI_F4   os/hal/ports/STM32/LLD/QUADSPIv1)
SET (CHIBIOS_INCLUDES_hal_RTC_F4    os/hal/ports/STM32/LLD/RTCv2)
SET (CHIBIOS_INCLUDES_hal_I2S_F4    os/hal/ports/STM32/LLD/SPIv1)
SET (CHIBIOS_INCLUDES_hal_SPI_F4    os/hal/ports/STM32/LLD/SPIv1)
SET (CHIBIOS_INCLUDES_hal_SDC_F4    os/hal/ports/STM32/LLD/SDIOv1)
SET (CHIBIOS_INCLUDES_hal_GPT_F4    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_ICU_F4    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_PWM_F4    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_SERIAL_F4 os/hal/ports/STM32/LLD/USARTv1)
SET (CHIBIOS_INCLUDES_hal_UART_F4   os/hal/ports/STM32/LLD/USARTv1)
SET (CHIBIOS_INCLUDES_hal_WDG_F4    os/hal/ports/STM32/LLD/xWDGv1)



SET (CHIBIOS_INCLUDES_hal_F7
    os/hal/ports/STM32/STM32F7xx
    os/hal/ports/STM32/LLD/DMAv2
    os/hal/ports/STM32/LLD/TIMv1
)
SET (CHIBIOS_SOURCES_hal_F7
    os/hal/ports/STM32/STM32F7xx/stm32_isr.c
    os/hal/ports/STM32/STM32F7xx/hal_lld.c
    os/hal/ports/STM32/LLD/DMAv2/stm32_dma.c
    os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
)

SET (CHIBIOS_SOURCES_hal_ADC_F7     os/hal/ports/STM32/LLD/ADCv2/hal_adc_lld.c)
SET (CHIBIOS_SOURCES_hal_CAN_F7     os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c)
SET (CHIBIOS_SOURCES_hal_CRY_F7     os/hal/ports/STM32/LLD/CRYPv1/hal_crypto_lld.c)
SET (CHIBIOS_SOURCES_hal_DAC_F7     os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c)
SET (CHIBIOS_SOURCES_hal_EXT_F7     os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c)
SET (CHIBIOS_SOURCES_hal_PAL_F7     os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c)
SET (CHIBIOS_SOURCES_hal_I2C_F7     os/hal/ports/STM32/LLD/I2Cv2/hal_i2c_lld.c)
SET (CHIBIOS_SOURCES_hal_MAC_F7     os/hal/ports/STM32/LLD/MACv1/hal_mac_lld.c)
SET (CHIBIOS_SOURCES_hal_USB_F7     os/hal/ports/STM32/LLD/OTGv1/hal_usb_lld.c)
SET (CHIBIOS_SOURCES_hal_QSPI_F7    os/hal/ports/STM32/LLD/QUADSPIv1/hal_qspi_lld.c)
SET (CHIBIOS_SOURCES_hal_RTC_F7     os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c)
SET (CHIBIOS_SOURCES_hal_I2S_F7     os/hal/ports/STM32/LLD/SPIv2/hal_i2s_lld.c)
SET (CHIBIOS_SOURCES_hal_SPI_F7     os/hal/ports/STM32/LLD/SPIv2/hal_spi_lld.c)
SET (CHIBIOS_SOURCES_hal_SDC_F7     os/hal/ports/STM32/LLD/SDMMCv1/hal_sdc_lld.c)
SET (CHIBIOS_SOURCES_hal_GPT_F7     os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c)
SET (CHIBIOS_SOURCES_hal_ICU_F7     os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c)
SET (CHIBIOS_SOURCES_hal_PWM_F7     os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c)
SET (CHIBIOS_SOURCES_hal_SERIAL_F7  os/hal/ports/STM32/LLD/USARTv2/hal_serial_lld.c)
SET (CHIBIOS_SOURCES_hal_UART_F7    os/hal/ports/STM32/LLD/USARTv2/hal_uart_lld.c)
SET (CHIBIOS_SOURCES_hal_WDG_F7     os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c)

SET (CHIBIOS_INCLUDES_hal_ADC_F7    os/hal/ports/STM32/LLD/ADCv2)
SET (CHIBIOS_INCLUDES_hal_CAN_F7    os/hal/ports/STM32/LLD/CANv1)
SET (CHIBIOS_INCLUDES_hal_CRY_F7    os/hal/ports/STM32/LLD/CRYPv1)
SET (CHIBIOS_INCLUDES_hal_DAC_F7    os/hal/ports/STM32/LLD/DACv1)
SET (CHIBIOS_INCLUDES_hal_EXT_F7    os/hal/ports/STM32/LLD/EXTIv1)
SET (CHIBIOS_INCLUDES_hal_PAL_F7    os/hal/ports/STM32/LLD/GPIOv2)
SET (CHIBIOS_INCLUDES_hal_I2C_F7    os/hal/ports/STM32/LLD/I2Cv2)
SET (CHIBIOS_INCLUDES_hal_MAC_F7    os/hal/ports/STM32/LLD/MACv1)
SET (CHIBIOS_INCLUDES_hal_USB_F7    os/hal/ports/STM32/LLD/OTGv1)
SET (CHIBIOS_INCLUDES_hal_QSPI_F7   os/hal/ports/STM32/LLD/QUADSPIv1)
SET (CHIBIOS_INCLUDES_hal_RTC_F7    os/hal/ports/STM32/LLD/RTCv2)
SET (CHIBIOS_INCLUDES_hal_I2S_F7    os/hal/ports/STM32/LLD/SPIv2)
SET (CHIBIOS_INCLUDES_hal_SPI_F7    os/hal/ports/STM32/LLD/SPIv2)
SET (CHIBIOS_INCLUDES_hal_SDC_F7    os/hal/ports/STM32/LLD/SDMMCv1)
SET (CHIBIOS_INCLUDES_hal_GPT_F7    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_ICU_F7    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_PWM_F7    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_SERIAL_F7 os/hal/ports/STM32/LLD/USARTv2)
SET (CHIBIOS_INCLUDES_hal_UART_F7   os/hal/ports/STM32/LLD/USARTv2)
SET (CHIBIOS_INCLUDES_hal_WDG_F7    os/hal/ports/STM32/LLD/xWDGv1)



SET (CHIBIOS_INCLUDES_hal_L0
    os/hal/ports/STM32/STM32L0xx
    os/hal/ports/STM32/LLD/DMAv1
    os/hal/ports/STM32/LLD/TIMv1
)
SET (CHIBIOS_SOURCES_hal_L0
    os/hal/ports/STM32/STM32L0xx/stm32_isr.c
    os/hal/ports/STM32/STM32L0xx/hal_lld.c
    os/hal/ports/STM32/LLD/DMAv1/stm32_dma.c
    os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
)

SET (CHIBIOS_SOURCES_hal_ADC_L0     os/hal/ports/STM32/LLD/ADCv1/hal_adc_lld.c)
SET (CHIBIOS_SOURCES_hal_CAN_L0     os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c)
SET (CHIBIOS_SOURCES_hal_DAC_L0     os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c)
SET (CHIBIOS_SOURCES_hal_EXT_L0     os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c)
SET (CHIBIOS_SOURCES_hal_PAL_L0     os/hal/ports/STM32/LLD/GPIOv2/hal_pal_lld.c)
SET (CHIBIOS_SOURCES_hal_I2C_L0     os/hal/ports/STM32/LLD/I2Cv2/hal_i2c_lld.c)
SET (CHIBIOS_SOURCES_hal_RTC_L0     os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c)
SET (CHIBIOS_SOURCES_hal_I2S_L0     os/hal/ports/STM32/LLD/SPIv1/hal_i2s_lld.c)
SET (CHIBIOS_SOURCES_hal_SPI_L0     os/hal/ports/STM32/LLD/SPIv1/hal_spi_lld.c)
SET (CHIBIOS_SOURCES_hal_GPT_L0     os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c)
SET (CHIBIOS_SOURCES_hal_ICU_L0     os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c)
SET (CHIBIOS_SOURCES_hal_PWM_L0     os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c)
SET (CHIBIOS_SOURCES_hal_SERIAL_L0  os/hal/ports/STM32/LLD/USARTv2/hal_serial_lld.c)
SET (CHIBIOS_SOURCES_hal_UART_L0    os/hal/ports/STM32/LLD/USARTv2/hal_uart_lld.c)
SET (CHIBIOS_SOURCES_hal_USB_L0     os/hal/ports/STM32/LLD/USBv1/hal_usb_lld.c)
SET (CHIBIOS_SOURCES_hal_WDG_L0     os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c)

SET (CHIBIOS_INCLUDES_hal_ADC_L0    os/hal/ports/STM32/LLD/ADCv1)
SET (CHIBIOS_INCLUDES_hal_CAN_L0    os/hal/ports/STM32/LLD/CANv1)
SET (CHIBIOS_INCLUDES_hal_DAC_L0    os/hal/ports/STM32/LLD/DACv1)
SET (CHIBIOS_INCLUDES_hal_EXT_L0    os/hal/ports/STM32/LLD/EXTIv1)
SET (CHIBIOS_INCLUDES_hal_PAL_L0    os/hal/ports/STM32/LLD/GPIOv2)
SET (CHIBIOS_INCLUDES_hal_I2C_L0    os/hal/ports/STM32/LLD/I2Cv2)
SET (CHIBIOS_INCLUDES_hal_RTC_L0    os/hal/ports/STM32/LLD/RTCv2)
SET (CHIBIOS_INCLUDES_hal_I2S_L0    os/hal/ports/STM32/LLD/SPIv1)
SET (CHIBIOS_INCLUDES_hal_SPI_L0    os/hal/ports/STM32/LLD/SPIv1)
SET (CHIBIOS_INCLUDES_hal_GPT_L0    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_ICU_L0    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_PWM_L0    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_SERIAL_L0 os/hal/ports/STM32/LLD/USARTv2)
SET (CHIBIOS_INCLUDES_hal_UART_L0   os/hal/ports/STM32/LLD/USARTv2)
SET (CHIBIOS_INCLUDES_hal_USB_L0    os/hal/ports/STM32/LLD/USBv1)
SET (CHIBIOS_INCLUDES_hal_WDG_L0    os/hal/ports/STM32/LLD/xWDGv1)



SET (CHIBIOS_INCLUDES_hal_L4
    os/hal/ports/STM32/STM32L4xx
    os/hal/ports/STM32/LLD/DMAv1
    os/hal/ports/STM32/LLD/TIMv1
)
SET (CHIBIOS_SOURCES_hal_L4
    os/hal/ports/STM32/STM32L4xx/stm32_isr.c
    os/hal/ports/STM32/STM32L4xx/hal_lld.c
    os/hal/ports/STM32/LLD/DMAv1/stm32_dma.c
    os/hal/ports/STM32/LLD/TIMv1/hal_st_lld.c
)

SET (CHIBIOS_SOURCES_hal_ADC_L4     os/hal/ports/STM32/LLD/ADCv3/hal_adc_lld.c)
SET (CHIBIOS_SOURCES_hal_CAN_L4     os/hal/ports/STM32/LLD/CANv1/hal_can_lld.c)
SET (CHIBIOS_SOURCES_hal_DAC_L4     os/hal/ports/STM32/LLD/DACv1/hal_dac_lld.c)
SET (CHIBIOS_SOURCES_hal_EXT_L4     os/hal/ports/STM32/LLD/EXTIv1/hal_ext_lld.c)
SET (CHIBIOS_SOURCES_hal_PAL_L4     os/hal/ports/STM32/LLD/GPIOv3/hal_pal_lld.c)
SET (CHIBIOS_SOURCES_hal_I2C_L4     os/hal/ports/STM32/LLD/I2Cv2/hal_i2c_lld.c)
SET (CHIBIOS_SOURCES_hal_USB_L4     os/hal/ports/STM32/LLD/OTGv1/hal_usb_lld.c)
SET (CHIBIOS_SOURCES_hal_QSPI_L4    os/hal/ports/STM32/LLD/QUADSPIv1/hal_qspi_lld.c)
SET (CHIBIOS_SOURCES_hal_RTC_L4     os/hal/ports/STM32/LLD/RTCv2/hal_rtc_lld.c)
SET (CHIBIOS_SOURCES_hal_I2S_L4     os/hal/ports/STM32/LLD/SPIv2/hal_i2s_lld.c)
SET (CHIBIOS_SOURCES_hal_SPI_L4     os/hal/ports/STM32/LLD/SPIv2/hal_spi_lld.c)
SET (CHIBIOS_SOURCES_hal_SDC_L4     os/hal/ports/STM32/LLD/SDMMCv1/hal_sdc_lld.c)
SET (CHIBIOS_SOURCES_hal_GPT_L4     os/hal/ports/STM32/LLD/TIMv1/hal_gpt_lld.c)
SET (CHIBIOS_SOURCES_hal_ICU_L4     os/hal/ports/STM32/LLD/TIMv1/hal_icu_lld.c)
SET (CHIBIOS_SOURCES_hal_PWM_L4     os/hal/ports/STM32/LLD/TIMv1/hal_pwm_lld.c)
SET (CHIBIOS_SOURCES_hal_SERIAL_L4  os/hal/ports/STM32/LLD/USARTv2/hal_serial_lld.c)
SET (CHIBIOS_SOURCES_hal_UART_L4    os/hal/ports/STM32/LLD/USARTv2/hal_uart_lld.c)
SET (CHIBIOS_SOURCES_hal_WDG_L4     os/hal/ports/STM32/LLD/xWDGv1/hal_wdg_lld.c)

SET (CHIBIOS_INCLUDES_hal_ADC_L4    os/hal/ports/STM32/LLD/ADCv3)
SET (CHIBIOS_INCLUDES_hal_CAN_L4    os/hal/ports/STM32/LLD/CANv1)
SET (CHIBIOS_INCLUDES_hal_DAC_L4    os/hal/ports/STM32/LLD/DACv1)
SET (CHIBIOS_INCLUDES_hal_EXT_L4    os/hal/ports/STM32/LLD/EXTIv1)
SET (CHIBIOS_INCLUDES_hal_PAL_L4    os/hal/ports/STM32/LLD/GPIOv3)
SET (CHIBIOS_INCLUDES_hal_I2C_L4    os/hal/ports/STM32/LLD/I2Cv2)
SET (CHIBIOS_INCLUDES_hal_USB_L4    os/hal/ports/STM32/LLD/OTGv1)
SET (CHIBIOS_INCLUDES_hal_QSPI_L4   os/hal/ports/STM32/LLD/QUADSPIv1)
SET (CHIBIOS_INCLUDES_hal_RTC_L4    os/hal/ports/STM32/LLD/RTCv2)
SET (CHIBIOS_INCLUDES_hal_I2S_L4    os/hal/ports/STM32/LLD/SPIv2)
SET (CHIBIOS_INCLUDES_hal_SPI_L4    os/hal/ports/STM32/LLD/SPIv2)
SET (CHIBIOS_INCLUDES_hal_SDC_L4    os/hal/ports/STM32/LLD/SDMMCv1)
SET (CHIBIOS_INCLUDES_hal_GPT_L4    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_ICU_L4    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_PWM_L4    os/hal/ports/STM32/LLD/TIMv1)
SET (CHIBIOS_INCLUDES_hal_SERIAL_L4 os/hal/ports/STM32/LLD/USARTv2)
SET (CHIBIOS_INCLUDES_hal_UART_L4   os/hal/ports/STM32/LLD/USARTv2)
SET (CHIBIOS_INCLUDES_hal_WDG_L4    os/hal/ports/STM32/LLD/xWDGv1)



IF (CHIBIOS_SOURCES_hal_${CHIBIOS_KERNEL})
    LIST(APPEND CHIBIOS_SOURCES_hal ${CHIBIOS_SOURCES_hal_${CHIBIOS_KERNEL}})
ENDIF()
    
IF (CHIBIOS_INCLUDES_hal_${CHIBIOS_KERNEL})
    LIST(APPEND CHIBIOS_INCLUDES_hal ${CHIBIOS_INCLUDES_hal_${CHIBIOS_KERNEL}})
ENDIF()

IF (CHIBIOS_SOURCES_hal_${STM32_FAMILY})
    LIST(APPEND CHIBIOS_SOURCES_hal ${CHIBIOS_SOURCES_hal_${STM32_FAMILY}})
ENDIF()
    
IF (CHIBIOS_INCLUDES_hal_${STM32_FAMILY})
    LIST(APPEND CHIBIOS_INCLUDES_hal ${CHIBIOS_INCLUDES_hal_${STM32_FAMILY}})
ENDIF()

FOREACH (COMP ${CHIBIOS_HAL_COMPONENTS})
    IF (CHIBIOS_SOURCES_hal_${COMP})
        LIST(APPEND CHIBIOS_SOURCES_hal ${CHIBIOS_SOURCES_hal_${COMP}})
    ENDIF()
    IF (CHIBIOS_INCLUDES_hal_${COMP})
        LIST(APPEND CHIBIOS_INCLUDES_hal ${CHIBIOS_INCLUDES_hal_${COMP}})
    ENDIF()
    
    IF (CHIBIOS_SOURCES_hal_${COMP}_${STM32_FAMILY})
        LIST(APPEND CHIBIOS_SOURCES_hal ${CHIBIOS_SOURCES_hal_${COMP}_${STM32_FAMILY}})
    ENDIF()
    IF (CHIBIOS_INCLUDES_hal_${COMP}_${STM32_FAMILY})
        LIST(APPEND CHIBIOS_INCLUDES_hal ${CHIBIOS_INCLUDES_hal_${COMP}_${STM32_FAMILY}})
    ENDIF()
ENDFOREACH()


