#include "board.h"
#include <string.h>


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfcrypt/test/test.h>
#include <wolfcrypt/benchmark/benchmark.h>


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* UART definitions */
#define LPC_UART            LPC_USART0
#define UARTx_IRQn          USART0_IRQn


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

const char menu1[] = "\r\n"
    "\tt. WolfSSL Test\r\n"
    "\tb. WolfSSL Benchmark\r\n";

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/
int main(void)
{
    int opt = 0;
    uint8_t buffer[1];
    func_args args;

    SystemCoreClockUpdate();
    Board_Init();
    Board_UART_Init(LPC_UART);
    Chip_UART_Init(LPC_UART);
    Chip_UART_SetBaud(LPC_UART, 115200);
    Chip_UART_ConfigData(LPC_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT); /* Default 8-N-1 */
    Chip_UART_TXEnable(LPC_UART);
    Chip_UART_SetupFIFOS(LPC_UART, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
                                    UART_FCR_TX_RS | UART_FCR_DMAMODE_SEL | UART_FCR_TRG_LEV0));
    Chip_UART_IntEnable(LPC_UART, (UART_IER_ABEOINT | UART_IER_ABTOINT));
    NVIC_SetPriority(UARTx_IRQn, 1);
    NVIC_EnableIRQ(UARTx_IRQn);

    Chip_OTP_Init();

    while (1) {
        DEBUGOUT("\r\n\t\t\t\tMENU\r\n");
        DEBUGOUT(menu1);
        DEBUGOUT("Please select one of the above options: ");

        opt = 0;
        while (opt == 0) {
            opt = Chip_UART_Read(LPC_UART, buffer, sizeof(buffer));
        }

        switch (buffer[0]) {

        case 't':
            memset(&args, 0, sizeof(args));
            printf("\nCrypt Test\n");
            wolfcrypt_test(&args);
            printf("Crypt Test: Return code %d\n", args.return_code);
            break;

        case 'b':
            memset(&args, 0, sizeof(args));
            printf("\nBenchmark Test\n");
            benchmark_test(&args);
            printf("Benchmark Test: Return code %d\n", args.return_code);
            break;

        // All other cases go here
        default: DEBUGOUT("\r\nSelection out of range\r\n"); break;
        }
    }
}
