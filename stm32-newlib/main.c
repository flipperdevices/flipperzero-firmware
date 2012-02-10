#include <stm32f10x.h>
#include <stdio.h>
#include <time.h>

void initGPIO()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_Config;

    /* USART1 Tx */
    GPIO_Config.GPIO_Pin = GPIO_Pin_9;
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Config);

    /* USART1 Rx */
    GPIO_Config.GPIO_Pin = GPIO_Pin_10;
    GPIO_Config.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Config);
}

void initRTC()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        PWR_BackupAccessCmd(ENABLE);

        BKP_DeInit();
        RCC_LSEConfig(RCC_LSE_ON);

        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        RCC_RTCCLKCmd(ENABLE);
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();

        RTC_SetPrescaler(32767);
        RTC_WaitForLastTask();

        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
        PWR_BackupAccessCmd(DISABLE);
    }
    RTC_WaitForSynchro();
    RTC_WaitForLastTask();
}

void initUSART()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

void setTime(uint32_t time)
{
    PWR_BackupAccessCmd(ENABLE);
    RTC_WaitForLastTask();
    RTC_SetCounter(time);
    RTC_WaitForLastTask();
    PWR_BackupAccessCmd(DISABLE);
}

void initAll(void)
{
    initRTC();
    initGPIO();
    initUSART();
    RCC_ClearFlag();
}

int main(void)
{
    initAll();
    for (;;)
    {
        char c;
        time_t t;
        scanf("%c", &c);
        switch (c)
        {
        case 's':
            scanf("%d", &t);
            setTime(t);
            printf("Current time changed: %d - %s\r", t, ctime(&t));
            break;
        default:
            RTC_WaitForLastTask();
            t = time(0);
            printf("Current time: %d - %s\r", t, ctime(&t));
            break;
        }
    }
    return 0;
}
