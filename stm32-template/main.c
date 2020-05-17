#if defined STM32F1
# include <stm32f1xx.h>
#elif defined STM32F2
# include <stm32f2xx.h>
#elif defined STM32F4
# include <stm32f4xx.h>
#elif defined STM32G0
# include <stm32g0xx.h>
#endif

int main(void)
{
    for (;;);
    return 0;
}
