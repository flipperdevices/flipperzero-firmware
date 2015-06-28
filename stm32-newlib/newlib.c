#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <sys/time.h>

#if defined STM32F1
# include <stm32f1xx_hal.h>
#elif defined STM32F4
# include <stm32f4xx_hal.h>
#endif

extern uint32_t __get_MSP(void);
extern UART_HandleTypeDef UART_Handle;
extern uint64_t virtualTimer;

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
    tv->tv_sec = virtualTimer / 1000;
    tv->tv_usec = (virtualTimer % 1000) * 1000;
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
    static char *heap_end= &_ebss;
    char *prev_heap_end;

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
        HAL_UART_Receive(&UART_Handle, (uint8_t *)ptr, 1, HAL_MAX_DELAY);
        return 1;
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
    switch (file)
    {
    case STDOUT_FILENO: /*stdout*/
        HAL_UART_Transmit(&UART_Handle, (uint8_t*)ptr, len, HAL_MAX_DELAY);
        break;
    case STDERR_FILENO: /* stderr */
        HAL_UART_Transmit(&UART_Handle, (uint8_t*)ptr, len, HAL_MAX_DELAY);
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}
