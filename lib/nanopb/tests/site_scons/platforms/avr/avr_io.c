/* This wrapper file initializes stdio to UART connection and
 * receives the list of command line arguments.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#undef main
extern int app_main(int argc, const char **argv);

struct {
    uint8_t argc;
    char args[3][16];
} g_args;

int uart_putchar(char c, FILE *stream)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

int uart_getchar(FILE *stream)
{
    while (bit_is_clear(UCSR0A, RXC0) && bit_is_clear(UCSR0A, FE0));
    if (UCSR0A & _BV(FE0)) return _FDEV_EOF; /* Break = EOF */
    return UDR0;
}

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
static char g_malloc_heap[8192];
extern uint32_t __bss_end;

void abort(void)
{
    if (__bss_end != 0xDEADBEEF)
    {
        fprintf(stderr, "possible stack overflow\n");
    }

    fprintf(stderr, "abort() called\n");
    DDRB = 3;
    PORTB = 1;
    cli();
    while (1) sleep_mode();
}

int main(void)
{
    const char *argv[4] = {"main", g_args.args[0], g_args.args[1], g_args.args[2]};
    int status;
    
    UBRR0 = (8000000 / (16UL * 9600)) - 1; /* 9600 bps with default 8 MHz clock */
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
    
    __malloc_heap_start = g_malloc_heap;
    __malloc_heap_end = g_malloc_heap + sizeof(g_malloc_heap);

    __bss_end = 0xDEADBEEF;

    stdout = stdin = stderr = &uart_str;
    
    fread((char*)&g_args, 1, sizeof(g_args), stdin);
    
    status = app_main(g_args.argc + 1, argv);

    if (__bss_end != 0xDEADBEEF)
    {
        status = 255;
        fprintf(stderr, "possible stack overflow\n");
    }

    DDRB = 3;
    if (status)
    {
        fprintf(stderr, "Error exit: %d\n", status);
        PORTB = 1; // PB0 indicates error
    }
    else
    {
        PORTB = 2; // PB1 indicates success
    }
  
    cli();
    sleep_mode();
    return status;
}

