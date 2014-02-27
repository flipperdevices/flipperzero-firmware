#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for STMicroelectronics WaveShare XCore407I/EVK407I board.
 */

/*
 * Board identifier.
 */
#define BOARD_WAVESHARE_XCORE407I
#define BOARD_NAME                  "WaveShare XCore407I/EVK407I"



#if !defined(STM32_LSECLK)
#define STM32_LSECLK                32768
#endif

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                8000000
#endif

#define STM32_VDD                   330

#define GPIOA_PIN0                  0
#define GPIOA_PIN1                  1
#define GPIOA_MDIO                  2
#define GPIOA_ULPI_D0               3
#define GPIOA_PIN4                  4
#define GPIOA_ULPI_CK               5
#define GPIOA_PIN6                  6
#define GPIOA_MII_CRS_DV            7
#define GPIOA_PIN8                  8
#define GPIOA_VBUS_FS               9
#define GPIOA_OTG_FS_ID             10
#define GPIOA_OTG_FS_DM             11
#define GPIOA_OTG_FS_DP             12
#define GPIOA_JTMS                  13
#define GPIOA_JTCK                  14
#define GPIOA_JTDI                  15

#define GPIOB_ULPI_D1               0
#define GPIOB_ULPI_D2               1
#define GPIOB_PIN2                  2
#define GPIOB_JTDO                  3
#define GPIOB_NJTRST                4
#define GPIOB_ULPI_D7               5
#define GPIOB_PIN6                  6
#define GPIOB_PIN7                  7
#define GPIOB_PIN8                  8
#define GPIOB_PIN9                  9
#define GPIOB_ULPI_D3               10
#define GPIOB_ULPI_D4               11
#define GPIOB_ULPI_D5               12
#define GPIOB_ULPI_D6               13
#define GPIOB_PIN14                 14
#define GPIOB_PIN15                 15

#define GPIOC_ULPI_STP              0
#define GPIOC_OTG_FS_POWER_ON       1
#define GPIOC_PIN2                  2
#define GPIOC_PIN3                  3
#define GPIOC_MII_RX_D0            4
#define GPIOC_MII_RX_D1            5
#define GPIOC_TP_IRQ                6
#define GPIOC_PIN7                  7
#define GPIOC_PIN8                  8
#define GPIOC_PIN9                  9
#define GPIOC_USART3_TX             10
#define GPIOC_USART3_RX             11
#define GPIOC_PIN12                 12
#define GPIOC_PIN13                 13
#define GPIOC_OSC32_IN              14
#define GPIOC_OSC32_OUT             15

#define GPIOD_FSMC_D2               0
#define GPIOD_FSMC_D3               1
#define GPIOD_PIN2                  2
#define GPIOD_PIN3                  3
#define GPIOD_FSMC_NOE              4
#define GPIOD_FSMC_NWE              5
#define GPIOD_FSMC_NWAIT            6
#define GPIOD_FSMC_NCE2             7
#define GPIOD_FSMC_D13              8
#define GPIOD_FSMC_D14              9
#define GPIOD_FSMC_D15              10
#define GPIOD_FSMC_A16              11
#define GPIOD_FSMC_A17              12
#define GPIOD_PIN13                 13
#define GPIOD_FSMC_D0               14
#define GPIOD_FSMC_D1               15

#define GPIOE_PIN0                  0
#define GPIOE_PIN1                  1
#define GPIOE_JOY_A                 2
#define GPIOE_JOY_B                 3
#define GPIOE_JOY_C                 4
#define GPIOE_JOY_D                 5
#define GPIOE_JOY_PRESS             6
#define GPIOE_FSMC_D4               7
#define GPIOE_FSMC_D5               8
#define GPIOE_FSMC_D6               9
#define GPIOE_FSMC_D7               10
#define GPIOE_FSMC_D8               11
#define GPIOE_FSMC_D9               12
#define GPIOE_FSMC_D10              13
#define GPIOE_FSMC_D11              14
#define GPIOE_FSMC_D12              15

#define GPIOF_PIN0                  0
#define GPIOF_PIN1                  1
#define GPIOF_PIN2                  2
#define GPIOF_PIN3                  3
#define GPIOF_PIN4                  4
#define GPIOF_PIN5                  5
#define GPIOF_PIN6                  6
#define GPIOF_LCD_PWM               7
#define GPIOF_PIN8                  8
#define GPIOF_PIN9                  9
#define GPIOF_PIN10                 10
#define GPIOF_PIN11                 11
#define GPIOF_PIN12                 12
#define GPIOF_PIN13                 13
#define GPIOF_PIN14                 14
#define GPIOF_PIN15                 15

#define GPIOG_PIN0                  0
#define GPIOG_PIN1                  1
#define GPIOG_PIN2                  2
#define GPIOG_PIN3                  3
#define GPIOG_PIN4                  4
#define GPIOG_FSMC_A15              5
#define GPIOG_PIN6                  6
#define GPIOG_PIN7                  7
#define GPIOG_PIN8                  8
#define GPIOG_PIN9                  9
#define GPIOG_PIN10                 10
#define GPIOG_MII_TX_EN             11
#define GPIOG_PIN12                 12
#define GPIOG_MII_TX_D0             13
#define GPIOG_MII_TX_D1             14
#define GPIOG_PIN15                 15

#define GPIOH_OSC_IN                0
#define GPIOH_OSC_OUT               1
#define GPIOH_LED1                  2
#define GPIOH_LED2                  3
#define GPIOH_ULPI_NXT              4
#define GPIOH_PIN5                  5
#define GPIOH_PIN6                  6
#define GPIOH_PIN7                  7
#define GPIOH_PIN8                  8
#define GPIOH_PIN9                  9
#define GPIOH_PIN10                 10
#define GPIOH_ULPI_RESET            11
#define GPIOH_PIN12                 12
#define GPIOH_PIN13                 13
#define GPIOH_PIN14                 14
#define GPIOH_PIN15                 15

#define GPIOI_TP_CS                 0
#define GPIOI_SPI2_SCK              1
#define GPIOI_SPI2_MISO             2
#define GPIOI_SPI2_MOSI             3
#define GPIOI_PIN4                  4
#define GPIOI_PIN5                  5
#define GPIOI_PIN6                  6
#define GPIOI_PIN7                  7
#define GPIOI_LED3                  8
#define GPIOI_PIN9                  9
#define GPIOI_LED4                  10
#define GPIOI_ULPI_DIR              11
#define GPIOI_PIN12                 12
#define GPIOI_PIN13                 13
#define GPIOI_PIN14                 14
#define GPIOI_PIN15                 15

#define PIN_MODE_INPUT(n)           (0U << ((n) * 2))
#define PIN_MODE_OUTPUT(n)          (1U << ((n) * 2))
#define PIN_MODE_ALTERNATE(n)       (2U << ((n) * 2))
#define PIN_MODE_ANALOG(n)          (3U << ((n) * 2))
#define PIN_ODR_LOW(n)              (0U << (n))
#define PIN_ODR_HIGH(n)             (1U << (n))
#define PIN_OTYPE_PUSHPULL(n)       (0U << (n))
#define PIN_OTYPE_OPENDRAIN(n)      (1U << (n))
#define PIN_OSPEED_2M(n)            (0U << ((n) * 2))
#define PIN_OSPEED_25M(n)           (1U << ((n) * 2))
#define PIN_OSPEED_50M(n)           (2U << ((n) * 2))
#define PIN_OSPEED_100M(n)          (3U << ((n) * 2))
#define PIN_PUPDR_FLOATING(n)       (0U << ((n) * 2))
#define PIN_PUPDR_PULLUP(n)         (1U << ((n) * 2))
#define PIN_PUPDR_PULLDOWN(n)       (2U << ((n) * 2))
#define PIN_AFIO_AF(n, v)           ((v##U) << ((n % 8) * 4))

/*
 * PA0 - PIN0                       (input floating).
 * PA1 - PIN1                       (input floating).
 * PA2 - MDIO                       (input floating).
 * PA3 - ULPI_D0                    (input floating).
 * PA4 - PIN4                       (input floating).
 * PA5 - ULPI_CK                    (input floating).
 * PA6 - PIN6                       (input floating).
 * PA7 - MII_CRS_DV                 (input floating).
 * PA8 - PIN8                       (input floating).
 * PA9 - VBUS_FS                    (input floating).
 * PA10 - OTG_FS_ID                 (input floating).
 * PA11 - OTG_FS_DM                 (input floating).
 * PA12 - OTG_FS_DP                 (input floating).
 * PA13 - JTMS                      (alternate 0).
 * PA14 - JTCK                      (alternate 0).
 * PA15 - JTDI                      (alternate 0).
 */

#define VAL_GPIOA_MODER             (PIN_MODE_INPUT(GPIOA_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOA_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOA_MDIO) |           \
                                     PIN_MODE_INPUT(GPIOA_ULPI_D0) |        \
                                     PIN_MODE_INPUT(GPIOA_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOA_ULPI_CK) |        \
                                     PIN_MODE_INPUT(GPIOA_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOA_MII_CRS_DV) |    \
                                     PIN_MODE_INPUT(GPIOA_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOA_VBUS_FS) |        \
                                     PIN_MODE_INPUT(GPIOA_OTG_FS_ID) |      \
                                     PIN_MODE_INPUT(GPIOA_OTG_FS_DM) |      \
                                     PIN_MODE_INPUT(GPIOA_OTG_FS_DP) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_JTMS) |       \
                                     PIN_MODE_ALTERNATE(GPIOA_JTCK) |       \
                                     PIN_MODE_ALTERNATE(GPIOA_JTDI))
#define VAL_GPIOA_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOA_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_MDIO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_ULPI_D0) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_ULPI_CK) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_MII_CRS_DV) |\
                                     PIN_OTYPE_PUSHPULL(GPIOA_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_VBUS_FS) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_ID) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DM) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DP) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTMS) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTCK) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTDI))
#define VAL_GPIOA_OSPEEDR           (PIN_OSPEED_100M(GPIOA_PIN0) |          \
                                     PIN_OSPEED_100M(GPIOA_PIN1) |          \
                                     PIN_OSPEED_100M(GPIOA_MDIO) |          \
                                     PIN_OSPEED_100M(GPIOA_ULPI_D0) |       \
                                     PIN_OSPEED_100M(GPIOA_PIN4) |          \
                                     PIN_OSPEED_100M(GPIOA_ULPI_CK) |       \
                                     PIN_OSPEED_100M(GPIOA_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOA_MII_CRS_DV) |   \
                                     PIN_OSPEED_100M(GPIOA_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOA_VBUS_FS) |       \
                                     PIN_OSPEED_100M(GPIOA_OTG_FS_ID) |     \
                                     PIN_OSPEED_100M(GPIOA_OTG_FS_DM) |     \
                                     PIN_OSPEED_100M(GPIOA_OTG_FS_DP) |     \
                                     PIN_OSPEED_100M(GPIOA_JTMS) |          \
                                     PIN_OSPEED_100M(GPIOA_JTCK) |          \
                                     PIN_OSPEED_100M(GPIOA_JTDI))
#define VAL_GPIOA_PUPDR             (PIN_PUPDR_FLOATING(GPIOA_PIN0) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_PIN1) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_MDIO) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_ULPI_D0) |    \
                                     PIN_PUPDR_FLOATING(GPIOA_PIN4) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_ULPI_CK) |    \
                                     PIN_PUPDR_FLOATING(GPIOA_PIN6) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_MII_CRS_DV) |\
                                     PIN_PUPDR_FLOATING(GPIOA_PIN8) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_VBUS_FS) |    \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_ID) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DM) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DP) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_JTMS) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_JTCK) |       \
                                     PIN_PUPDR_FLOATING(GPIOA_JTDI))
#define VAL_GPIOA_ODR               (PIN_ODR_HIGH(GPIOA_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOA_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOA_MDIO) |             \
                                     PIN_ODR_HIGH(GPIOA_ULPI_D0) |          \
                                     PIN_ODR_HIGH(GPIOA_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOA_ULPI_CK) |          \
                                     PIN_ODR_HIGH(GPIOA_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOA_MII_CRS_DV) |      \
                                     PIN_ODR_HIGH(GPIOA_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOA_VBUS_FS) |          \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_ID) |        \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DM) |        \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DP) |        \
                                     PIN_ODR_HIGH(GPIOA_JTMS) |             \
                                     PIN_ODR_HIGH(GPIOA_JTCK) |             \
                                     PIN_ODR_HIGH(GPIOA_JTDI))
#define VAL_GPIOA_AFRL              (PIN_AFIO_AF(GPIOA_PIN0, 0) |           \
                                     PIN_AFIO_AF(GPIOA_PIN1, 0) |           \
                                     PIN_AFIO_AF(GPIOA_MDIO, 0) |           \
                                     PIN_AFIO_AF(GPIOA_ULPI_D0, 0) |        \
                                     PIN_AFIO_AF(GPIOA_PIN4, 6) |           \
                                     PIN_AFIO_AF(GPIOA_ULPI_CK, 0) |        \
                                     PIN_AFIO_AF(GPIOA_PIN6, 5) |           \
                                     PIN_AFIO_AF(GPIOA_MII_CRS_DV, 0))
#define VAL_GPIOA_AFRH              (PIN_AFIO_AF(GPIOA_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOA_VBUS_FS, 0) |        \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_ID, 0) |      \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DM, 0) |      \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DP, 0) |      \
                                     PIN_AFIO_AF(GPIOA_JTMS, 0) |           \
                                     PIN_AFIO_AF(GPIOA_JTCK, 0) |           \
                                     PIN_AFIO_AF(GPIOA_JTDI, 0))

/*
 * PB0 - ULPI_D1                    (input floating).
 * PB1 - ULPI_D2                    (input floating).
 * PB2 - PIN2                       (input floating).
 * PB3 - JTDO                       (alternate 0).
 * PB4 - NJTRST                     (alternate 0).
 * PB5 - ULPI_D7                    (input floating).
 * PB6 - PIN6                       (input floating).
 * PB7 - PIN7                       (input floating).
 * PB8 - PIN8                       (input floating).
 * PB9 - PIN9                       (input floating).
 * PB10 - ULPI_D3                   (input floating).
 * PB11 - ULPI_D4                   (input floating).
 * PB12 - ULPI_D5                   (input floating).
 * PB13 - ULPI_D6                   (input floating).
 * PB14 - PIN14                     (input floating).
 * PB15 - PIN15                     (input floating).
 */

#define VAL_GPIOB_MODER             (PIN_MODE_INPUT(GPIOB_ULPI_D1) |        \
                                     PIN_MODE_INPUT(GPIOB_ULPI_D2) |        \
                                     PIN_MODE_INPUT(GPIOB_PIN2) |           \
                                     PIN_MODE_ALTERNATE(GPIOB_JTDO) |       \
                                     PIN_MODE_ALTERNATE(GPIOB_NJTRST) |     \
                                     PIN_MODE_INPUT(GPIOB_ULPI_D7) |        \
                                     PIN_MODE_INPUT(GPIOB_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOB_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOB_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOB_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOB_ULPI_D3) |        \
                                     PIN_MODE_INPUT(GPIOB_ULPI_D4) |        \
                                     PIN_MODE_INPUT(GPIOB_ULPI_D5) |        \
                                     PIN_MODE_INPUT(GPIOB_ULPI_D6) |        \
                                     PIN_MODE_INPUT(GPIOB_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOB_PIN15))
#define VAL_GPIOB_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D2) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_JTDO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_NJTRST) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D7) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D4) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D5) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOB_ULPI_D6) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN15))
#define VAL_GPIOB_OSPEEDR           (PIN_OSPEED_100M(GPIOB_ULPI_D1) |       \
                                     PIN_OSPEED_100M(GPIOB_ULPI_D2) |       \
                                     PIN_OSPEED_100M(GPIOB_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOB_JTDO) |          \
                                     PIN_OSPEED_100M(GPIOB_NJTRST) |        \
                                     PIN_OSPEED_100M(GPIOB_ULPI_D7) |       \
                                     PIN_OSPEED_100M(GPIOB_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOB_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOB_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOB_PIN9) |          \
                                     PIN_OSPEED_100M(GPIOB_ULPI_D3) |       \
                                     PIN_OSPEED_100M(GPIOB_ULPI_D4) |       \
                                     PIN_OSPEED_100M(GPIOB_ULPI_D5) |       \
                                     PIN_OSPEED_100M(GPIOB_ULPI_D6) |       \
                                     PIN_OSPEED_100M(GPIOB_PIN14) |         \
                                     PIN_OSPEED_100M(GPIOB_PIN15))
#define VAL_GPIOB_PUPDR             (PIN_PUPDR_FLOATING(GPIOB_ULPI_D1) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_ULPI_D2) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_PIN2) |       \
                                     PIN_PUPDR_FLOATING(GPIOB_JTDO) |       \
                                     PIN_PUPDR_FLOATING(GPIOB_NJTRST) |     \
                                     PIN_PUPDR_FLOATING(GPIOB_ULPI_D7) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_PIN6) |       \
                                     PIN_PUPDR_FLOATING(GPIOB_PIN7) |       \
                                     PIN_PUPDR_FLOATING(GPIOB_PIN8) |       \
                                     PIN_PUPDR_FLOATING(GPIOB_PIN9) |       \
                                     PIN_PUPDR_FLOATING(GPIOB_ULPI_D3) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_ULPI_D4) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_ULPI_D5) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_ULPI_D6) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_PIN14) |      \
                                     PIN_PUPDR_FLOATING(GPIOB_PIN15))
#define VAL_GPIOB_ODR               (PIN_ODR_HIGH(GPIOB_ULPI_D1) |          \
                                     PIN_ODR_HIGH(GPIOB_ULPI_D2) |          \
                                     PIN_ODR_HIGH(GPIOB_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOB_JTDO) |             \
                                     PIN_ODR_HIGH(GPIOB_NJTRST) |           \
                                     PIN_ODR_HIGH(GPIOB_ULPI_D7) |          \
                                     PIN_ODR_HIGH(GPIOB_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOB_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOB_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOB_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOB_ULPI_D3) |          \
                                     PIN_ODR_HIGH(GPIOB_ULPI_D4) |          \
                                     PIN_ODR_HIGH(GPIOB_ULPI_D5) |          \
                                     PIN_ODR_HIGH(GPIOB_ULPI_D6) |          \
                                     PIN_ODR_HIGH(GPIOB_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOB_PIN15))
#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_ULPI_D1, 0) |        \
                                     PIN_AFIO_AF(GPIOB_ULPI_D2, 0) |        \
                                     PIN_AFIO_AF(GPIOB_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOB_JTDO, 0) |           \
                                     PIN_AFIO_AF(GPIOB_NJTRST, 0) |         \
                                     PIN_AFIO_AF(GPIOB_ULPI_D7, 0) |        \
                                     PIN_AFIO_AF(GPIOB_PIN6, 0) |           \
                                     PIN_AFIO_AF(GPIOB_PIN7, 0))
#define VAL_GPIOB_AFRH              (PIN_AFIO_AF(GPIOB_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOB_PIN9, 0) |           \
                                     PIN_AFIO_AF(GPIOB_ULPI_D3, 0) |        \
                                     PIN_AFIO_AF(GPIOB_ULPI_D4, 0) |        \
                                     PIN_AFIO_AF(GPIOB_ULPI_D5, 0) |        \
                                     PIN_AFIO_AF(GPIOB_ULPI_D6, 0) |        \
                                     PIN_AFIO_AF(GPIOB_PIN14, 0) |          \
                                     PIN_AFIO_AF(GPIOB_PIN15, 0))

/*
 * PC0 - ULPI_STP                   (input floating).
 * PC1 - OTG_FS_POWER_ON            (input floating).
 * PC2 - PIN2                       (input floating).
 * PC3 - PIN3                       (input floating).
 * PC4 - MII_RX_D0                  (input floating).
 * PC5 - MII_RX_D1                  (input floating).
 * PC6 - TP_IRQ                     (input floating).
 * PC7 - PIN7                       (input floating).
 * PC8 - PIN8                       (input floating).
 * PC9 - PIN9                       (input floating).
 * PC10 - USART3_TX                 (input floating).
 * PC11 - USART3_RX                 (input floating).
 * PC12 - PIN12                     (input floating).
 * PC13 - PIN13                     (input floating).
 * PC14 - OSC32_IN                  (input floating).
 * PC15 - OSC32_OUT                 (input floating).
 */

#define VAL_GPIOC_MODER             (PIN_MODE_INPUT(GPIOC_ULPI_STP) |       \
                                     PIN_MODE_INPUT(GPIOC_OTG_FS_POWER_ON) |\
                                     PIN_MODE_INPUT(GPIOC_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOC_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOC_MII_RX_D0) |     \
                                     PIN_MODE_INPUT(GPIOC_MII_RX_D1) |     \
                                     PIN_MODE_INPUT(GPIOC_TP_IRQ) |         \
                                     PIN_MODE_INPUT(GPIOC_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOC_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOC_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOC_USART3_TX) |      \
                                     PIN_MODE_INPUT(GPIOC_USART3_RX) |      \
                                     PIN_MODE_INPUT(GPIOC_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOC_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOC_OSC32_IN) |       \
                                     PIN_MODE_INPUT(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOC_ULPI_STP) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OTG_FS_POWER_ON) |\
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_MII_RX_D0) | \
                                     PIN_OTYPE_PUSHPULL(GPIOC_MII_RX_D1) | \
                                     PIN_OTYPE_PUSHPULL(GPIOC_TP_IRQ) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_USART3_TX) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_USART3_RX) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_IN) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OSPEEDR           (PIN_OSPEED_100M(GPIOC_ULPI_STP) |      \
                                     PIN_OSPEED_100M(GPIOC_OTG_FS_POWER_ON) |\
                                     PIN_OSPEED_100M(GPIOC_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOC_PIN3) |          \
                                     PIN_OSPEED_100M(GPIOC_MII_RX_D0) |    \
                                     PIN_OSPEED_100M(GPIOC_MII_RX_D1) |    \
                                     PIN_OSPEED_100M(GPIOC_TP_IRQ) |        \
                                     PIN_OSPEED_100M(GPIOC_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOC_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOC_PIN9) |          \
                                     PIN_OSPEED_100M(GPIOC_USART3_TX) |     \
                                     PIN_OSPEED_100M(GPIOC_USART3_RX) |     \
                                     PIN_OSPEED_100M(GPIOC_PIN12) |         \
                                     PIN_OSPEED_100M(GPIOC_PIN13) |         \
                                     PIN_OSPEED_100M(GPIOC_OSC32_IN) |      \
                                     PIN_OSPEED_100M(GPIOC_OSC32_OUT))
#define VAL_GPIOC_PUPDR             (PIN_PUPDR_FLOATING(GPIOC_ULPI_STP) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_OTG_FS_POWER_ON) |\
                                     PIN_PUPDR_FLOATING(GPIOC_PIN2) |       \
                                     PIN_PUPDR_FLOATING(GPIOC_PIN3) |       \
                                     PIN_PUPDR_FLOATING(GPIOC_MII_RX_D0) | \
                                     PIN_PUPDR_FLOATING(GPIOC_MII_RX_D1) | \
                                     PIN_PUPDR_FLOATING(GPIOC_TP_IRQ) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_PIN7) |       \
                                     PIN_PUPDR_FLOATING(GPIOC_PIN8) |       \
                                     PIN_PUPDR_FLOATING(GPIOC_PIN9) |       \
                                     PIN_PUPDR_FLOATING(GPIOC_USART3_TX) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_USART3_RX) |  \
                                     PIN_PUPDR_FLOATING(GPIOC_PIN12) |      \
                                     PIN_PUPDR_FLOATING(GPIOC_PIN13) |      \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_IN) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_OUT))
#define VAL_GPIOC_ODR               (PIN_ODR_HIGH(GPIOC_ULPI_STP) |         \
                                     PIN_ODR_HIGH(GPIOC_OTG_FS_POWER_ON) |  \
                                     PIN_ODR_HIGH(GPIOC_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOC_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOC_MII_RX_D0) |       \
                                     PIN_ODR_HIGH(GPIOC_MII_RX_D1) |       \
                                     PIN_ODR_HIGH(GPIOC_TP_IRQ) |           \
                                     PIN_ODR_HIGH(GPIOC_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOC_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOC_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOC_USART3_TX) |        \
                                     PIN_ODR_HIGH(GPIOC_USART3_RX) |        \
                                     PIN_ODR_HIGH(GPIOC_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOC_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOC_OSC32_IN) |         \
                                     PIN_ODR_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_AFRL              (PIN_AFIO_AF(GPIOC_ULPI_STP, 0) |       \
                                     PIN_AFIO_AF(GPIOC_OTG_FS_POWER_ON, 0) |\
                                     PIN_AFIO_AF(GPIOC_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOC_PIN3, 0) |           \
                                     PIN_AFIO_AF(GPIOC_MII_RX_D0, 0) |     \
                                     PIN_AFIO_AF(GPIOC_MII_RX_D1, 0) |     \
                                     PIN_AFIO_AF(GPIOC_TP_IRQ, 0) |         \
                                     PIN_AFIO_AF(GPIOC_PIN7, 0))
#define VAL_GPIOC_AFRH              (PIN_AFIO_AF(GPIOC_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOC_PIN9, 0) |           \
                                     PIN_AFIO_AF(GPIOC_USART3_TX, 0) |      \
                                     PIN_AFIO_AF(GPIOC_USART3_RX, 0) |      \
                                     PIN_AFIO_AF(GPIOC_PIN12, 0) |          \
                                     PIN_AFIO_AF(GPIOC_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOC_OSC32_IN, 0) |       \
                                     PIN_AFIO_AF(GPIOC_OSC32_OUT, 0))

/*
 * PD0 - FSMC_D2                    (input floating).
 * PD1 - FSMC_D3                    (input floating).
 * PD2 - PIN2                       (input floating).
 * PD3 - PIN3                       (input floating).
 * PD4 - FSMC_NOE                   (input floating).
 * PD5 - FSMC_NWE                   (input floating).
 * PD6 - FSMC_NWAIT                 (input floating).
 * PD7 - FSMC_NCE2                  (input floating).
 * PD8 - FSMC_D13                   (input floating).
 * PD9 - FSMC_D14                   (input floating).
 * PD10 - FSMC_D15                  (input floating).
 * PD11 - FSMC_A16                  (input floating).
 * PD12 - FSMC_A17                  (input floating).
 * PD13 - PIN13                     (input floating).
 * PD14 - FSMC_D0                   (input floating).
 * PD15 - FSMC_D1                   (input floating).
 */

#define VAL_GPIOD_MODER             (PIN_MODE_INPUT(GPIOD_FSMC_D2) |        \
                                     PIN_MODE_INPUT(GPIOD_FSMC_D3) |        \
                                     PIN_MODE_INPUT(GPIOD_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOD_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOD_FSMC_NOE) |       \
                                     PIN_MODE_INPUT(GPIOD_FSMC_NWE) |       \
                                     PIN_MODE_INPUT(GPIOD_FSMC_NWAIT) |     \
                                     PIN_MODE_INPUT(GPIOD_FSMC_NCE2) |      \
                                     PIN_MODE_INPUT(GPIOD_FSMC_D13) |       \
                                     PIN_MODE_INPUT(GPIOD_FSMC_D14) |       \
                                     PIN_MODE_INPUT(GPIOD_FSMC_D15) |       \
                                     PIN_MODE_INPUT(GPIOD_FSMC_A16) |       \
                                     PIN_MODE_INPUT(GPIOD_FSMC_A17) |       \
                                     PIN_MODE_INPUT(GPIOD_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOD_FSMC_D0) |        \
                                     PIN_MODE_INPUT(GPIOD_FSMC_D1))
#define VAL_GPIOD_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOD_FSMC_D2) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_D3) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_NOE) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_NWE) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_NWAIT) | \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_NCE2) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_D13) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_D14) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_D15) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_A16) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_A17) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_D0) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOD_FSMC_D1))
#define VAL_GPIOD_OSPEEDR           (PIN_OSPEED_100M(GPIOD_FSMC_D2) |       \
                                     PIN_OSPEED_100M(GPIOD_FSMC_D3) |       \
                                     PIN_OSPEED_100M(GPIOD_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOD_PIN3) |          \
                                     PIN_OSPEED_100M(GPIOD_FSMC_NOE) |      \
                                     PIN_OSPEED_100M(GPIOD_FSMC_NWE) |      \
                                     PIN_OSPEED_100M(GPIOD_FSMC_NWAIT) |    \
                                     PIN_OSPEED_100M(GPIOD_FSMC_NCE2) |     \
                                     PIN_OSPEED_100M(GPIOD_FSMC_D13) |      \
                                     PIN_OSPEED_100M(GPIOD_FSMC_D14) |      \
                                     PIN_OSPEED_100M(GPIOD_FSMC_D15) |      \
                                     PIN_OSPEED_100M(GPIOD_FSMC_A16) |      \
                                     PIN_OSPEED_100M(GPIOD_FSMC_A17) |      \
                                     PIN_OSPEED_100M(GPIOD_PIN13) |         \
                                     PIN_OSPEED_100M(GPIOD_FSMC_D0) |       \
                                     PIN_OSPEED_100M(GPIOD_FSMC_D1))
#define VAL_GPIOD_PUPDR             (PIN_PUPDR_FLOATING(GPIOD_FSMC_D2) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_D3) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_PIN2) |       \
                                     PIN_PUPDR_FLOATING(GPIOD_PIN3) |       \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_NOE) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_NWE) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_NWAIT) | \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_NCE2) |  \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_D13) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_D14) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_D15) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_A16) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_A17) |   \
                                     PIN_PUPDR_FLOATING(GPIOD_PIN13) |      \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_D0) |    \
                                     PIN_PUPDR_FLOATING(GPIOD_FSMC_D1))
#define VAL_GPIOD_ODR               (PIN_ODR_HIGH(GPIOD_FSMC_D2) |          \
                                     PIN_ODR_HIGH(GPIOD_FSMC_D3) |          \
                                     PIN_ODR_HIGH(GPIOD_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOD_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOD_FSMC_NOE) |         \
                                     PIN_ODR_HIGH(GPIOD_FSMC_NWE) |         \
                                     PIN_ODR_HIGH(GPIOD_FSMC_NWAIT) |       \
                                     PIN_ODR_HIGH(GPIOD_FSMC_NCE2) |        \
                                     PIN_ODR_HIGH(GPIOD_FSMC_D13) |         \
                                     PIN_ODR_HIGH(GPIOD_FSMC_D14) |         \
                                     PIN_ODR_HIGH(GPIOD_FSMC_D15) |         \
                                     PIN_ODR_HIGH(GPIOD_FSMC_A16) |         \
                                     PIN_ODR_HIGH(GPIOD_FSMC_A17) |         \
                                     PIN_ODR_HIGH(GPIOD_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOD_FSMC_D0) |          \
                                     PIN_ODR_HIGH(GPIOD_FSMC_D1))
#define VAL_GPIOD_AFRL              (PIN_AFIO_AF(GPIOD_FSMC_D2, 0) |        \
                                     PIN_AFIO_AF(GPIOD_FSMC_D3, 0) |        \
                                     PIN_AFIO_AF(GPIOD_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOD_PIN3, 0) |           \
                                     PIN_AFIO_AF(GPIOD_FSMC_NOE, 0) |       \
                                     PIN_AFIO_AF(GPIOD_FSMC_NWE, 0) |       \
                                     PIN_AFIO_AF(GPIOD_FSMC_NWAIT, 0) |     \
                                     PIN_AFIO_AF(GPIOD_FSMC_NCE2, 0))
#define VAL_GPIOD_AFRH              (PIN_AFIO_AF(GPIOD_FSMC_D13, 0) |       \
                                     PIN_AFIO_AF(GPIOD_FSMC_D14, 0) |       \
                                     PIN_AFIO_AF(GPIOD_FSMC_D15, 0) |       \
                                     PIN_AFIO_AF(GPIOD_FSMC_A16, 0) |       \
                                     PIN_AFIO_AF(GPIOD_FSMC_A17, 0) |       \
                                     PIN_AFIO_AF(GPIOD_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOD_FSMC_D0, 0) |        \
                                     PIN_AFIO_AF(GPIOD_FSMC_D1, 0))

/*
 * PE0 - PIN0                       (input floating).
 * PE1 - PIN1                       (input floating).
 * PE2 - JOY_A                      (input floating).
 * PE3 - JOY_B                      (input floating).
 * PE4 - JOY_C                      (input floating).
 * PE5 - JOY_D                      (input floating).
 * PE6 - JOY_PRESS                  (input floating).
 * PE7 - FSMC_D4                    (input floating).
 * PE8 - FSMC_D5                    (input floating).
 * PE9 - FSMC_D6                    (input floating).
 * PE10 - FSMC_D7                   (input floating).
 * PE11 - FSMC_D8                   (input floating).
 * PE12 - FSMC_D9                   (input floating).
 * PE13 - FSMC_D10                  (input floating).
 * PE14 - FSMC_D11                  (input floating).
 * PE15 - FSMC_D12                  (input floating).
 */

#define VAL_GPIOE_MODER             (PIN_MODE_INPUT(GPIOE_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOE_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOE_JOY_A) |          \
                                     PIN_MODE_INPUT(GPIOE_JOY_B) |          \
                                     PIN_MODE_INPUT(GPIOE_JOY_C) |          \
                                     PIN_MODE_INPUT(GPIOE_JOY_D) |          \
                                     PIN_MODE_INPUT(GPIOE_JOY_PRESS) |      \
                                     PIN_MODE_INPUT(GPIOE_FSMC_D4) |        \
                                     PIN_MODE_INPUT(GPIOE_FSMC_D5) |        \
                                     PIN_MODE_INPUT(GPIOE_FSMC_D6) |        \
                                     PIN_MODE_INPUT(GPIOE_FSMC_D7) |        \
                                     PIN_MODE_INPUT(GPIOE_FSMC_D8) |        \
                                     PIN_MODE_INPUT(GPIOE_FSMC_D9) |        \
                                     PIN_MODE_INPUT(GPIOE_FSMC_D10) |       \
                                     PIN_MODE_INPUT(GPIOE_FSMC_D11) |       \
                                     PIN_MODE_INPUT(GPIOE_FSMC_D12))
#define VAL_GPIOE_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOE_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_JOY_A) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_JOY_B) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_JOY_C) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_JOY_D) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_JOY_PRESS) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FSMC_D4) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FSMC_D5) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FSMC_D6) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FSMC_D7) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FSMC_D8) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FSMC_D9) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FSMC_D10) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FSMC_D11) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOE_FSMC_D12))
#define VAL_GPIOE_OSPEEDR           (PIN_OSPEED_100M(GPIOE_PIN0) |          \
                                     PIN_OSPEED_100M(GPIOE_PIN1) |          \
                                     PIN_OSPEED_100M(GPIOE_JOY_A) |         \
                                     PIN_OSPEED_100M(GPIOE_JOY_B) |         \
                                     PIN_OSPEED_100M(GPIOE_JOY_C) |         \
                                     PIN_OSPEED_100M(GPIOE_JOY_D) |         \
                                     PIN_OSPEED_100M(GPIOE_JOY_PRESS) |     \
                                     PIN_OSPEED_100M(GPIOE_FSMC_D4) |       \
                                     PIN_OSPEED_100M(GPIOE_FSMC_D5) |       \
                                     PIN_OSPEED_100M(GPIOE_FSMC_D6) |       \
                                     PIN_OSPEED_100M(GPIOE_FSMC_D7) |       \
                                     PIN_OSPEED_100M(GPIOE_FSMC_D8) |       \
                                     PIN_OSPEED_100M(GPIOE_FSMC_D9) |       \
                                     PIN_OSPEED_100M(GPIOE_FSMC_D10) |      \
                                     PIN_OSPEED_100M(GPIOE_FSMC_D11) |      \
                                     PIN_OSPEED_100M(GPIOE_FSMC_D12))
#define VAL_GPIOE_PUPDR             (PIN_PUPDR_FLOATING(GPIOE_PIN0) |       \
                                     PIN_PUPDR_FLOATING(GPIOE_PIN1) |       \
                                     PIN_PUPDR_FLOATING(GPIOE_JOY_A) |      \
                                     PIN_PUPDR_FLOATING(GPIOE_JOY_B) |      \
                                     PIN_PUPDR_FLOATING(GPIOE_JOY_C) |      \
                                     PIN_PUPDR_FLOATING(GPIOE_JOY_D) |      \
                                     PIN_PUPDR_FLOATING(GPIOE_JOY_PRESS) |  \
                                     PIN_PUPDR_FLOATING(GPIOE_FSMC_D4) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_FSMC_D5) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_FSMC_D6) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_FSMC_D7) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_FSMC_D8) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_FSMC_D9) |    \
                                     PIN_PUPDR_FLOATING(GPIOE_FSMC_D10) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_FSMC_D11) |   \
                                     PIN_PUPDR_FLOATING(GPIOE_FSMC_D12))
#define VAL_GPIOE_ODR               (PIN_ODR_HIGH(GPIOE_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOE_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOE_JOY_A) |            \
                                     PIN_ODR_HIGH(GPIOE_JOY_B) |            \
                                     PIN_ODR_HIGH(GPIOE_JOY_C) |            \
                                     PIN_ODR_HIGH(GPIOE_JOY_D) |            \
                                     PIN_ODR_HIGH(GPIOE_JOY_PRESS) |        \
                                     PIN_ODR_HIGH(GPIOE_FSMC_D4) |          \
                                     PIN_ODR_HIGH(GPIOE_FSMC_D5) |          \
                                     PIN_ODR_HIGH(GPIOE_FSMC_D6) |          \
                                     PIN_ODR_HIGH(GPIOE_FSMC_D7) |          \
                                     PIN_ODR_HIGH(GPIOE_FSMC_D8) |          \
                                     PIN_ODR_HIGH(GPIOE_FSMC_D9) |          \
                                     PIN_ODR_HIGH(GPIOE_FSMC_D10) |         \
                                     PIN_ODR_HIGH(GPIOE_FSMC_D11) |         \
                                     PIN_ODR_HIGH(GPIOE_FSMC_D12))
#define VAL_GPIOE_AFRL              (PIN_AFIO_AF(GPIOE_PIN0, 0) |           \
                                     PIN_AFIO_AF(GPIOE_PIN1, 0) |           \
                                     PIN_AFIO_AF(GPIOE_JOY_A, 0) |          \
                                     PIN_AFIO_AF(GPIOE_JOY_B, 0) |          \
                                     PIN_AFIO_AF(GPIOE_JOY_C, 0) |          \
                                     PIN_AFIO_AF(GPIOE_JOY_D, 0) |          \
                                     PIN_AFIO_AF(GPIOE_JOY_PRESS, 0) |      \
                                     PIN_AFIO_AF(GPIOE_FSMC_D4, 0))
#define VAL_GPIOE_AFRH              (PIN_AFIO_AF(GPIOE_FSMC_D5, 0) |        \
                                     PIN_AFIO_AF(GPIOE_FSMC_D6, 0) |        \
                                     PIN_AFIO_AF(GPIOE_FSMC_D7, 0) |        \
                                     PIN_AFIO_AF(GPIOE_FSMC_D8, 0) |        \
                                     PIN_AFIO_AF(GPIOE_FSMC_D9, 0) |        \
                                     PIN_AFIO_AF(GPIOE_FSMC_D10, 0) |        \
                                     PIN_AFIO_AF(GPIOE_FSMC_D11, 0) |        \
                                     PIN_AFIO_AF(GPIOE_FSMC_D12, 0))

/*
 * PF0  - PIN0                      (input floating).
 * PF1  - PIN1                      (input floating).
 * PF2  - PIN2                      (input floating).
 * PF3  - PIN3                      (input floating).
 * PF4  - PIN4                      (input floating).
 * PF5  - PIN5                      (input floating).
 * PF6  - PIN6                      (input floating).
 * PF7  - LCD_PWM                   (input floating).
 * PF8  - PIN8                      (input floating).
 * PF9  - PIN9                      (input floating).
 * PF10 - PIN10                     (input floating).
 * PF11 - PIN11                     (input floating).
 * PF12 - PIN12                     (input floating).
 * PF13 - PIN13                     (input floating).
 * PF14 - PIN14                     (input floating).
 * PF15 - PIN15                     (input floating).
 */
#define VAL_GPIOF_MODER             (PIN_MODE_INPUT(GPIOF_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOF_LCD_PWM) |        \
                                     PIN_MODE_INPUT(GPIOF_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN15))
#define VAL_GPIOF_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOF_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_LCD_PWM) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN15))
#define VAL_GPIOF_OSPEEDR           (PIN_OSPEED_100M(GPIOF_PIN0) |          \
                                     PIN_OSPEED_100M(GPIOF_PIN1) |          \
                                     PIN_OSPEED_100M(GPIOF_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOF_PIN3) |          \
                                     PIN_OSPEED_100M(GPIOF_PIN4) |          \
                                     PIN_OSPEED_100M(GPIOF_PIN5) |          \
                                     PIN_OSPEED_100M(GPIOF_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOF_LCD_PWM) |       \
                                     PIN_OSPEED_100M(GPIOF_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOF_PIN9) |          \
                                     PIN_OSPEED_100M(GPIOF_PIN10) |         \
                                     PIN_OSPEED_100M(GPIOF_PIN11) |         \
                                     PIN_OSPEED_100M(GPIOF_PIN12) |         \
                                     PIN_OSPEED_100M(GPIOF_PIN13) |         \
                                     PIN_OSPEED_100M(GPIOF_PIN14) |         \
                                     PIN_OSPEED_100M(GPIOF_PIN15))
#define VAL_GPIOF_PUPDR             (PIN_PUPDR_FLOATING(GPIOF_PIN0) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN1) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN2) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN3) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN4) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN5) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN6) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_LCD_PWM) |    \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN8) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN9) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN10) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN11) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN12) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN13) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN14) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_PIN15))
#define VAL_GPIOF_ODR               (PIN_ODR_HIGH(GPIOF_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOF_LCD_PWM) |          \
                                     PIN_ODR_HIGH(GPIOF_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN15))
#define VAL_GPIOF_AFRL              (PIN_AFIO_AF(GPIOF_PIN0, 0) |           \
                                     PIN_AFIO_AF(GPIOF_PIN1, 0) |           \
                                     PIN_AFIO_AF(GPIOF_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOF_PIN3, 0) |           \
                                     PIN_AFIO_AF(GPIOF_PIN4, 0) |           \
                                     PIN_AFIO_AF(GPIOF_PIN5, 0) |           \
                                     PIN_AFIO_AF(GPIOF_PIN6, 0) |           \
                                     PIN_AFIO_AF(GPIOF_LCD_PWM, 0))
#define VAL_GPIOF_AFRH              (PIN_AFIO_AF(GPIOF_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOF_PIN9, 0) |           \
                                     PIN_AFIO_AF(GPIOF_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOF_PIN11, 0) |          \
                                     PIN_AFIO_AF(GPIOF_PIN12, 0) |          \
                                     PIN_AFIO_AF(GPIOF_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOF_PIN14, 0) |          \
                                     PIN_AFIO_AF(GPIOF_PIN15, 0))

/*
 * PG0  - PIN0                      (input floating).
 * PG1  - PIN1                      (input floating).
 * PG2  - PIN2                      (input floating).
 * PG3  - PIN3                      (input floating).
 * PG4  - PIN4                      (input floating).
 * PG5  - FSMC_A15                  (input floating).
 * PG6  - PIN6                      (input floating).
 * PG7  - PIN7                      (input floating).
 * PG8  - PIN8                      (input floating).
 * PG9  - PIN9                      (input floating).
 * PG10 - PIN10                     (input floating).
 * PG11 - MII_TX_EN                 (input floating).
 * PG12 - PIN12                     (input floating).
 * PG13 - MII_TX_D0                 (input floating).
 * PG14 - MII_TX_D1                 (input floating).
 * PG15 - PIN15                     (input floating).
 */

#define VAL_GPIOG_MODER             (PIN_MODE_INPUT(GPIOG_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOG_FSMC_A15) |       \
                                     PIN_MODE_INPUT(GPIOG_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOG_MII_TX_EN) |      \
                                     PIN_MODE_INPUT(GPIOG_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOG_MII_TX_D0) |      \
                                     PIN_MODE_INPUT(GPIOG_MII_TX_D1) |      \
                                     PIN_MODE_INPUT(GPIOG_PIN15))
#define VAL_GPIOG_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOG_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_FSMC_A15) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_MII_TX_EN) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_MII_TX_D0) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOG_MII_TX_D1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN15))
#define VAL_GPIOG_OSPEEDR           (PIN_OSPEED_100M(GPIOG_PIN0) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN1) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN3) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN4) |          \
                                     PIN_OSPEED_100M(GPIOG_FSMC_A15) |      \
                                     PIN_OSPEED_100M(GPIOG_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN9) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN10) |         \
                                     PIN_OSPEED_100M(GPIOG_MII_TX_EN) |     \
                                     PIN_OSPEED_100M(GPIOG_PIN12) |         \
                                     PIN_OSPEED_100M(GPIOG_MII_TX_D0) |     \
                                     PIN_OSPEED_100M(GPIOG_MII_TX_D1) |     \
                                     PIN_OSPEED_100M(GPIOG_PIN15))
#define VAL_GPIOG_PUPDR             (PIN_PUPDR_FLOATING(GPIOG_PIN0) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN1) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN2) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN3) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN4) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_FSMC_A15) |   \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN6) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN7) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN8) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN9) |       \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN10) |      \
                                     PIN_PUPDR_FLOATING(GPIOG_MII_TX_EN) |  \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN12) |      \
                                     PIN_PUPDR_FLOATING(GPIOG_MII_TX_D0) |  \
                                     PIN_PUPDR_FLOATING(GPIOG_MII_TX_D1) |  \
                                     PIN_PUPDR_FLOATING(GPIOG_PIN15))
#define VAL_GPIOG_ODR               (PIN_ODR_HIGH(GPIOG_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOG_FSMC_A15) |         \
                                     PIN_ODR_HIGH(GPIOG_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOG_MII_TX_EN) |        \
                                     PIN_ODR_HIGH(GPIOG_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOG_MII_TX_D0) |        \
                                     PIN_ODR_HIGH(GPIOG_MII_TX_D1) |        \
                                     PIN_ODR_HIGH(GPIOG_PIN15))
#define VAL_GPIOG_AFRL              (PIN_AFIO_AF(GPIOG_PIN0, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN1, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN3, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN4, 0) |           \
                                     PIN_AFIO_AF(GPIOG_FSMC_A15, 0) |       \
                                     PIN_AFIO_AF(GPIOG_PIN6, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN7, 0))
#define VAL_GPIOG_AFRH              (PIN_AFIO_AF(GPIOG_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN9, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOG_MII_TX_EN, 0) |      \
                                     PIN_AFIO_AF(GPIOG_PIN12, 0) |          \
                                     PIN_AFIO_AF(GPIOG_MII_TX_D0, 0) |      \
                                     PIN_AFIO_AF(GPIOG_MII_TX_D1, 0) |      \
                                     PIN_AFIO_AF(GPIOG_PIN15, 0))

/*
 * PH0  - OSC_IN                    (input floating).
 * PH1  - OSC_OUT                   (input floating).
 * PH2  - LED1                      (output pushpull maximum).
 * PH3  - LED2                      (output pushpull maximum).
 * PH4  - ULPI_NXT                      (input floating).
 * PH5  - PIN5                      (input floating).
 * PH6  - PIN6                      (input floating).
 * PH7  - PIN7                      (input floating).
 * PH8  - PIN8                      (input floating).
 * PH9  - PIN9                      (input floating).
 * PH10 - PIN10                     (input floating).
 * PH11 - ULPI_RESET                (input floating).
 * PH12 - PIN12                     (input floating).
 * PH13 - PIN13                     (input floating).
 * PH14 - PIN14                     (input floating).
 * PH15 - PIN15                     (input floating).
 */

#define VAL_GPIOH_MODER             (PIN_MODE_INPUT(GPIOH_OSC_IN) |         \
                                     PIN_MODE_INPUT(GPIOH_OSC_OUT) |        \
                                     PIN_MODE_OUTPUT(GPIOH_LED1) |          \
                                     PIN_MODE_OUTPUT(GPIOH_LED2) |          \
                                     PIN_MODE_INPUT(GPIOH_ULPI_NXT) |       \
                                     PIN_MODE_INPUT(GPIOH_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOH_ULPI_RESET) |     \
                                     PIN_MODE_INPUT(GPIOH_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN15))
#define VAL_GPIOH_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOH_OSC_IN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOH_OSC_OUT) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOH_LED1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_LED2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_ULPI_NXT) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_ULPI_RESET) | \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN15))
#define VAL_GPIOH_OSPEEDR           (PIN_OSPEED_100M(GPIOH_OSC_IN) |        \
                                     PIN_OSPEED_100M(GPIOH_OSC_OUT) |       \
                                     PIN_OSPEED_100M(GPIOH_LED1) |          \
                                     PIN_OSPEED_100M(GPIOH_LED2) |          \
                                     PIN_OSPEED_100M(GPIOH_ULPI_NXT) |      \
                                     PIN_OSPEED_100M(GPIOH_PIN5) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN9) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN10) |         \
                                     PIN_OSPEED_100M(GPIOH_ULPI_RESET) |    \
                                     PIN_OSPEED_100M(GPIOH_PIN12) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN13) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN14) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN15))
#define VAL_GPIOH_PUPDR             (PIN_PUPDR_FLOATING(GPIOH_OSC_IN) |     \
                                     PIN_PUPDR_FLOATING(GPIOH_OSC_OUT) |    \
                                     PIN_PUPDR_FLOATING(GPIOH_LED1) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_LED2) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_ULPI_NXT) |   \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN5) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN6) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN7) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN8) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN9) |       \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN10) |      \
                                     PIN_PUPDR_FLOATING(GPIOH_ULPI_RESET) | \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN12) |      \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN13) |      \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN14) |      \
                                     PIN_PUPDR_FLOATING(GPIOH_PIN15))
#define VAL_GPIOH_ODR               (PIN_ODR_HIGH(GPIOH_OSC_IN) |           \
                                     PIN_ODR_HIGH(GPIOH_OSC_OUT) |          \
                                     PIN_ODR_LOW(GPIOH_LED1) |              \
                                     PIN_ODR_LOW(GPIOH_LED2) |              \
                                     PIN_ODR_HIGH(GPIOH_ULPI_NXT) |         \
                                     PIN_ODR_HIGH(GPIOH_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOH_ULPI_RESET) |       \
                                     PIN_ODR_HIGH(GPIOH_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN15))
#define VAL_GPIOH_AFRL              (PIN_AFIO_AF(GPIOH_OSC_IN, 0) |         \
                                     PIN_AFIO_AF(GPIOH_OSC_OUT, 0) |        \
                                     PIN_AFIO_AF(GPIOH_LED1, 0) |           \
                                     PIN_AFIO_AF(GPIOH_LED2, 0) |           \
                                     PIN_AFIO_AF(GPIOH_ULPI_NXT, 0) |       \
                                     PIN_AFIO_AF(GPIOH_PIN5, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN6, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN7, 0))
#define VAL_GPIOH_AFRH              (PIN_AFIO_AF(GPIOH_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN9, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOH_ULPI_RESET, 0) |     \
                                     PIN_AFIO_AF(GPIOH_PIN12, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN14, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN15, 0))

/*
 * PI0  - TP_CS                     (input floating).
 * PI1  - SPI2_SCK                  (input floating).
 * PI2  - SPI2_MISO                 (input floating).
 * PI3  - SPI2_MOSI                 (input floating).
 * PI4  - PIN4                      (input floating).
 * PI5  - PIN5                      (input floating).
 * PI6  - PIN6                      (input floating).
 * PI7  - PIN7                      (input floating).
 * PI8  - LED3                      (input floating).
 * PI9  - PIN9                      (input floating).
 * PI10 - LED4                      (input floating).
 * PI11 - ULPI_DIR                  (input floating).
 * PI12 - PIN12                     (input floating).
 * PI13 - PIN13                     (input floating).
 * PI14 - PIN14                     (input floating).
 * PI15 - PIN15                     (input floating).
 */

#define GPIOI_TP_CS                 0
#define GPIOI_SPI2_SCK              1
#define GPIOI_SPI2_MISO             2
#define GPIOI_SPI2_MOSI             3
#define GPIOI_PIN4                  4
#define GPIOI_PIN5                  5
#define GPIOI_PIN6                  6
#define GPIOI_PIN7                  7
#define GPIOI_LED3                  8
#define GPIOI_PIN9                  9
#define GPIOI_LED4                  10
#define GPIOI_ULPI_DIR              11
#define GPIOI_PIN12                 12
#define GPIOI_PIN13                 13
#define GPIOI_PIN14                 14
#define GPIOI_PIN15                 15

#define VAL_GPIOI_MODER             (PIN_MODE_INPUT(GPIOI_TP_CS) |          \
                                     PIN_MODE_INPUT(GPIOI_SPI2_SCK) |       \
                                     PIN_MODE_INPUT(GPIOI_SPI2_MISO) |      \
                                     PIN_MODE_INPUT(GPIOI_SPI2_MOSI) |      \
                                     PIN_MODE_INPUT(GPIOI_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOI_LED3) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOI_LED4) |           \
                                     PIN_MODE_INPUT(GPIOI_ULPI_DIR) |       \
                                     PIN_MODE_INPUT(GPIOI_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN15))
#define VAL_GPIOI_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOI_TP_CS) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_SPI2_SCK) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOI_SPI2_MISO) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOI_SPI2_MOSI) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_LED3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_LED4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_ULPI_DIR) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN15))
#define VAL_GPIOI_OSPEEDR           (PIN_OSPEED_100M(GPIOI_TP_CS) |         \
                                     PIN_OSPEED_100M(GPIOI_SPI2_SCK) |      \
                                     PIN_OSPEED_100M(GPIOI_SPI2_MISO) |     \
                                     PIN_OSPEED_100M(GPIOI_SPI2_MOSI) |     \
                                     PIN_OSPEED_100M(GPIOI_PIN4) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN5) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOI_LED3) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN9) |          \
                                     PIN_OSPEED_100M(GPIOI_LED4) |          \
                                     PIN_OSPEED_100M(GPIOI_ULPI_DIR) |      \
                                     PIN_OSPEED_100M(GPIOI_PIN12) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN13) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN14) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN15))
#define VAL_GPIOI_PUPDR             (PIN_PUPDR_FLOATING(GPIOI_TP_CS) |      \
                                     PIN_PUPDR_FLOATING(GPIOI_SPI2_SCK) |   \
                                     PIN_PUPDR_FLOATING(GPIOI_SPI2_MISO) |  \
                                     PIN_PUPDR_FLOATING(GPIOI_SPI2_MOSI) |  \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN4) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN5) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN6) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN7) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_LED3) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN9) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_LED4) |       \
                                     PIN_PUPDR_FLOATING(GPIOI_ULPI_DIR) |   \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN12) |      \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN13) |      \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN14) |      \
                                     PIN_PUPDR_FLOATING(GPIOI_PIN15))
#define VAL_GPIOI_ODR               (PIN_ODR_HIGH(GPIOI_TP_CS) |            \
                                     PIN_ODR_HIGH(GPIOI_SPI2_SCK) |         \
                                     PIN_ODR_HIGH(GPIOI_SPI2_MISO) |        \
                                     PIN_ODR_HIGH(GPIOI_SPI2_MOSI) |        \
                                     PIN_ODR_HIGH(GPIOI_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOI_LED3) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOI_LED4) |             \
                                     PIN_ODR_HIGH(GPIOI_ULPI_DIR) |         \
                                     PIN_ODR_HIGH(GPIOI_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN15))
#define VAL_GPIOI_AFRL              (PIN_AFIO_AF(GPIOI_TP_CS, 0) |          \
                                     PIN_AFIO_AF(GPIOI_SPI2_SCK, 0) |       \
                                     PIN_AFIO_AF(GPIOI_SPI2_MISO, 0) |      \
                                     PIN_AFIO_AF(GPIOI_SPI2_MOSI, 0) |      \
                                     PIN_AFIO_AF(GPIOI_PIN4, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN5, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN6, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN7, 0))
#define VAL_GPIOI_AFRH              (PIN_AFIO_AF(GPIOI_LED3, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN9, 0) |           \
                                     PIN_AFIO_AF(GPIOI_LED4, 0) |           \
                                     PIN_AFIO_AF(GPIOI_ULPI_DIR, 0) |       \
                                     PIN_AFIO_AF(GPIOI_PIN12, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN14, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN15, 0))


#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif

#endif
