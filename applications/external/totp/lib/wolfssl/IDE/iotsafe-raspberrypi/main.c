/* main.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/* IoT-SAFE example
 * main for POSIX serial communication.
 */

/* C Standard Library */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* POSIX Library */
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>
#include <signal.h>

/* wolfSSL Library */
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/port/iotsafe/iotsafe.h>

/* Variable Declarations */
static int serial_fd = -1;

/* Function Declarations */
extern int client_loop(const char *peer_ip, const char *peer_name,
    const char *peer_port, const char *temperature);
#ifdef DEBUG_UART_IO
static void print_buffer_hex(const char *buf, int len);
static void print_buffer_char(const char *buf, int len);
#endif
static int usart_init(const char *dev_name, int *fd);
static int usart_clean(int fd);
static int usart_read(char *buf, int len);
static int usart_write(const char *buf, int len);
static void show_usage(const char *program);


/* Function Definitions */
#ifdef DEBUG_UART_IO
static void print_buffer_hex(const char *buf, int len)
{
    for (int i = 0; i < len; i++)
        printf("%02X", (unsigned int)*(buf++));
}

static void print_buffer_char(const char *buf, int len)
{
    for (int i = 0; i < len; i++)
        printf("%c", *(buf++));
}
#endif

static int usart_init(const char *dev_name, int *fd)
{
    int portfd = open(dev_name, O_RDWR | O_NOCTTY);
    if (portfd < 0)
    {
        *fd = -1;
        return errno;
    }

    struct termios tty;
    tcgetattr(portfd, &tty);
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | (CS8);
    tty.c_iflag &= ~(IGNBRK | IXON | IXOFF | IXANY| INLCR | ICRNL);
    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~(ONLCR|OCRNL);
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB);
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~ISTRIP;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 5;
    tcsetattr(portfd, TCSANOW, &tty);

    *fd = portfd;

    return 0;
}

static int usart_clean(int fd)
{
    return close(fd);
}

static int usart_read(char *buf, int len)
{
    if (!buf || len < 0)
        return -1;

#ifdef DEBUG_UART_IO
    printf("UART Read Expected : %d bytes\n", len);
#endif

    int ret = 0;
    int i = 0;
    char c;
    memset(buf, 0, len);

   /* Read 1 byte at one time until *buf is full or a POSIX read error like
    * timeout occurs. */
    do
    {
        ret = read(serial_fd, &c, 1U);
        if (ret > 0) {
            buf[i++] = c;
            if (c == '\n')
                break;
        }
    } while (i < len && ret > 0);

#ifdef DEBUG_UART_IO
    printf("UART Read Actual   : %d bytes\n", i);
    #ifdef DEBUG_UART_IO_EXTRA_VERBOSE
    printf("[READ in HEX ]  <- ");
    print_buffer_hex(buf, i);
    printf("\n");
    #endif
    printf("[READ in CHAR]  <- ");
    print_buffer_char(buf, i);
    printf("\n");
#endif

    return i;
}

static int usart_write(const char *buf, int len)
{
    if (!buf || len < 0)
        return -1;

#ifdef DEBUG_UART_IO
    printf("UART Write Expected: %d bytes\n", len);
#endif

    int sent = write(serial_fd, buf, len);

    if (sent < 0)
        sent = 0;

#ifdef DEBUG_UART_IO
    printf("UART Write Actual  : %d bytes\n", sent);
    #ifdef DEBUG_UART_IO_EXTRA_VERBOSE
    printf("[WRITE in HEX ] -> ");
    print_buffer_hex(buf, sent);
    printf("\n");
    #endif
    printf("[WRITE in CHAR] -> ");
    print_buffer_char(buf, sent);
    printf("\n");
#endif

    return sent;
}

static void show_usage(const char *program)
{
    printf("\nUsage:\n");
    printf("\t%s -ip SERVER_IP_ADDRESS -h SERVER_HOST_NAME -p PORT_NUMBER "
                     "-t TEMPERATURE -d DEVICE_FILE_PATH\n", program);
    printf("\n");
    printf("\t-ip <server IPv4 address eg: 127.0.0.1>\n");
    printf("\t-h <server name eg: xxx.amazon.com>\n");
    printf("\t-p <server port eg: 443>\n");
    printf("\t-t <temperature eg: 25 Celsius>\n");
    printf("\t-d <serial device eg: /dev/ttyACM0>\n");
    exit(-1);
}

int main(int argc, char** argv)
{
    char ip[30]          = {0};
    char name[50]        = {0};
    char port[8]         = {0};
    char temperature[10] = {0};
    char device[30]      = {0};

    if (argc == 11)
    {
        if (strcmp(argv[1], "-ip") == 0)
           strcpy((char*)&ip, argv[2]);
        else
            show_usage(argv[0]);

        if (strcmp(argv[3], "-h") == 0)
            strcpy((char*)&name, argv[4]);
        else
            show_usage(argv[0]);

        if (strcmp(argv[5], "-p") == 0)
            strcpy((char*)&port, argv[6]);
        else
            show_usage(argv[0]);

        if (strcmp(argv[7], "-t") == 0)
            strcpy((char*)&temperature, argv[8]);
        else
            show_usage(argv[0]);

        if (strcmp(argv[9], "-d") == 0)
            strcpy((char*)&device, argv[10]);
        else
            show_usage(argv[0]);
    }
    else
    {
        show_usage(argv[0]);
    }

    int ret = 0;

    printf("#####################\n");
    printf("wolfSSL IoT-SAFE Demo\n");
    printf("#####################\n");

    printf("---- Initializing serial I/O\n");

    printf("----     Opening serial I/O\n");
    printf("Serial device: %s\n", (const char*)&device);
    if ((ret = usart_init((const char*)&device, &serial_fd)) != 0)
    {
        printf("ERROR: Error opening %s: Error %i (%s)\n",
            (const char*)&device, ret, strerror(ret));
        exit(-1);
    }

    printf("----     Setting serial I/O read callback\n");
    wolfIoTSafe_SetCSIM_read_cb(&usart_read);

    printf("----     Setting serial I/O write callback\n");
    wolfIoTSafe_SetCSIM_write_cb(&usart_write);

    printf("---- Finish initializing serial I/O\n");

    client_loop(ip, name, port, temperature);

    printf("---- Cleaning serial I/O\n");

    printf("----     Closing serial I/O\n");
    if ((ret = usart_clean(serial_fd)) != 0)
    {
        printf("ERROR: Error closing %s: Error %i (%s)\n",
            (const char*)&device, ret, strerror(ret));
        exit(-1);
    }

    printf("---- Finish cleaning serial I/O\n");

    return 0;
}
