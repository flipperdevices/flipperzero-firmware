#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <sys/time.h>
#include <stm32f10x.h>

extern uint32_t __get_MSP(void);

#define STDOUT_USART USART1
#define STDERR_USART USART1
#define STDIN_USART USART1

#undef errno
extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

int _write(int file, char *ptr, int len);

void _exit(int status)
{
    while (1);
}

int _close(int file)
{
    return -1;
}

int _execve(char *name, char **argv, char **env)
{
    errno = ENOMEM;
    return -1;
}

int _fork()
{
    errno = EAGAIN;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _getpid()
{
    return 1;
}

int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
    tv->tv_sec = RTC_GetCounter();
    tv->tv_usec = 0;
    return 0;
}

int _isatty(int file)
{
    switch (file)
    {
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}

int _kill(int pid, int sig)
{
    errno = EINVAL;
    return (-1);
}

int _link(char *old, char *new)
{
    errno = EMLINK;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

caddr_t _sbrk(int incr)
{
    extern char _ebss;
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0)
    {
        heap_end = &_ebss;
    }
    prev_heap_end = heap_end;

    char * stack = (char*) __get_MSP();
    if (heap_end + incr > stack)
    {
        _write(STDERR_FILENO, "Heap and stack collision\n", 25);
        errno = ENOMEM;
        return (caddr_t) - 1;
        //abort ();
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;

}

int _read(int file, char *ptr, int len)
{
    switch (file)
    {
    case STDIN_FILENO:
        while (USART_GetFlagStatus(STDIN_USART, USART_FLAG_RXNE) == RESET);
        char c = (char)(USART_ReceiveData(STDIN_USART) & (uint16_t) 0x01FF);
        *ptr++ = c;
        return 1;
        break;
    default:
        errno = EBADF;
        return -1;
    }
}

int _stat(const char *filepath, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

clock_t _times(struct tms *buf)
{
    return -1;
}

int _unlink(char *name)
{
    errno = ENOENT;
    return -1;
}

int _wait(int *status)
{
    errno = ECHILD;
    return -1;
}

int _write(int file, char *ptr, int len)
{
    int n;
    switch (file)
    {
    case STDOUT_FILENO: /*stdout*/
        for (n = 0; n < len; n++)
        {
            while (USART_GetFlagStatus(STDOUT_USART, USART_FLAG_TC) == RESET);
            USART_SendData(STDOUT_USART, (uint8_t) * ptr++);
        }
        break;
    case STDERR_FILENO: /* stderr */
        for (n = 0; n < len; n++)
        {
            while (USART_GetFlagStatus(STDERR_USART, USART_FLAG_TC) == RESET);
            USART_SendData(STDERR_USART, (uint8_t) * ptr++);
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}
