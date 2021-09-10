#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <simavr/sim_avr.h>
#include <simavr/sim_gdb.h>
#include <simavr/avr_ioport.h>
#include <simavr/sim_elf.h>
#include <simavr/avr_uart.h>

static avr_t *g_avr;
static avr_irq_t *g_uart_irq;

static struct {
    uint8_t argc;
    char args[3][16];
} g_args;
static int g_args_idx;
static bool g_uart_xon;
static bool g_status_ok;

static void uart_tx_hook(struct avr_irq_t * irq, uint32_t value, void * param)
{
    fputc(value, stdout);
    fflush(stdout);
}

static bool stdin_can_read()  
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return (FD_ISSET(0, &fds));
}

static void avr_logger(avr_t * avr, const int level, const char * format, va_list ap)
{
    if ((!avr && level <= LOG_WARNING) || (avr && avr->log >= level)) {
        vfprintf(stderr , format, ap);
    }
}

static void uart_xon_hook(struct avr_irq_t * irq, uint32_t value, void * param)
{
    g_uart_xon = true;
    int v;
    
    if (feof(stdin))
    {
        avr_raise_irq(&g_uart_irq[1], UART_INPUT_FE);
        return;
    }

    while (g_uart_xon)
    {
        if (g_args_idx < sizeof(g_args))
        {
            v = ((char*)&g_args)[g_args_idx++];
        }
        else if (stdin_can_read())
        {
            v = fgetc(stdin);
        }
        else
        {
            break;        
        }
        
        if (v != EOF)
        {
            avr_raise_irq(&g_uart_irq[1], v);
        }
        else
        {
            avr_raise_irq(&g_uart_irq[1], UART_INPUT_FE);
            break;
        }
    }
}

static void uart_xoff_hook(struct avr_irq_t * irq, uint32_t value, void * param)
{
    g_uart_xon = false;
}

void init_uart()
{
    const char *irq_names[2] = {"8<uart_in", "8>uart_out"};
    g_uart_irq = avr_alloc_irq(&g_avr->irq_pool, 0, 2, irq_names);
    avr_irq_register_notify(&g_uart_irq[0], &uart_tx_hook, NULL);
    
    uint32_t flags = 0;
    avr_ioctl(g_avr, AVR_IOCTL_UART_GET_FLAGS('0'), &flags);
    flags &= ~AVR_UART_FLAG_STDIO;
    flags &= ~AVR_UART_FLAG_POLL_SLEEP;
    avr_ioctl(g_avr, AVR_IOCTL_UART_SET_FLAGS('0'), &flags);

    avr_irq_t *src = avr_io_getirq(g_avr, AVR_IOCTL_UART_GETIRQ('0'), UART_IRQ_OUTPUT);
    avr_irq_t *dst = avr_io_getirq(g_avr, AVR_IOCTL_UART_GETIRQ('0'), UART_IRQ_INPUT);
    avr_connect_irq(src, &g_uart_irq[0]);
    avr_connect_irq(&g_uart_irq[1], dst);
    
    avr_irq_t *xon = avr_io_getirq(g_avr, AVR_IOCTL_UART_GETIRQ('0'), UART_IRQ_OUT_XON);
    avr_irq_t *xoff = avr_io_getirq(g_avr, AVR_IOCTL_UART_GETIRQ('0'), UART_IRQ_OUT_XOFF);
    avr_irq_register_notify(xon, uart_xon_hook, NULL);
    avr_irq_register_notify(xoff, uart_xoff_hook, NULL);
}

static void status_ok_hook(struct avr_irq_t * irq, uint32_t value, void * param)
{
    g_status_ok = value;
}

int main(int argc, char *argv[])
{
    avr_global_logger_set(&avr_logger);
    g_avr = avr_make_mcu_by_name("atmega1284");
    if (!g_avr)
    {
        fprintf(stderr, "avr_make_mcu_by_name failed\n");
        return 1;
    }
    
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [-g] binary [args ...]\n", argv[0]);
        return 2;
    }
    
    const char *filename = argv[1];
    bool enable_gdb = false;
    int argc_offset = 2;
    
    if (strcmp(filename, "-g") == 0)
    {
        enable_gdb = true;
        argc_offset = 3;
        filename = argv[2];
    }
    
    elf_firmware_t firmware;
    elf_read_firmware(filename, &firmware);
    avr_init(g_avr);
	avr_load_firmware(g_avr, &firmware);
	g_avr->frequency = 8000000;

    if (enable_gdb)
    {
        g_avr->state = cpu_Stopped;
        g_avr->gdb_port = 1234;
        avr_gdb_init(g_avr);
    }

    init_uart();
    
    avr_irq_register_notify(avr_io_getirq(g_avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 1), status_ok_hook, NULL);

    // Pass the rest of arguments to application inside simulator
    g_args.argc = argc - argc_offset;
    if (g_args.argc > 3) g_args.argc = 3;
    for (int i = 0; i < g_args.argc; i++)
    {
        strncpy(g_args.args[i], argv[i + argc_offset], 15);
    }
    
    while (1)
    {
		int state = avr_run(g_avr);
		if (state == cpu_Done)
		    break;
		    
   		if (state == cpu_Crashed)
   		{
   		    fprintf(stderr, "CPU Crashed\n");
			return 3;
		}
	}
    
    if (g_status_ok)
    {
        return 0;
    }
    else
    {
        fprintf(stderr, "Received error status from simulation\n");
        return 5;    
    }
}
