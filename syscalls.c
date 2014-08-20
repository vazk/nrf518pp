#include "syscalls.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <string.h>

#undef errno
extern int  errno;

// environ
// A pointer to a list of environment variables and their values.
// For a minimal environment, this empty list is adequate:
char *__env[1] = { 0 };
char **environ = __env;

int _execve(char *name, char **argv, char **env) {
    errno = ENOMEM;
    return -1;
}

int _fork() {
    errno = EAGAIN;
    return -1;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _getpid() {
    return 1;
}

int _isatty(int  file)
{
    switch (file)
	{
		case STDOUT_FILENO:
		case STDERR_FILENO:
		case STDIN_FILENO:
        return 1;
		
		default:
        errno = EBADF;
        return 0;
    }
}

int _kill(int pid, int sig)
{
    errno = EINVAL;
    return (-1);
}

int _link(char *oldx, char *newx)
{
    errno = EMLINK;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
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

int _unlink(char *name) {
    errno = ENOENT;
    return -1;
}

int _wait(int *status)
{
    errno = ECHILD;
    return -1;
}

int  _open(const char* file, int  flags, int  mode)
{
	return 0;
}

int _read(int file, char *ptr, int len)
{
/*
    nrfpp::UART& uart = nrfpp::UART::instance();
    uint8_t b;
    if(uart.is_initialized()) {
        for(int i = 0; i < len; ++i) {
            uart.get(&b);
            ptr[i] = (char)b;
        }
    } else {
        return -1;
    }
*/
    return len;
}

int _write(int file, char *ptr, int len)
{
/*
    nrfpp::UART& uart = nrfpp::UART::instance();
    if(uart.is_initialized()) {
        for(int i = 0; i < len; ++i) {
            uart.put((uint8_t)ptr[i]);
        }
        return len;
    } else {
        return -1;
    }
*/
    return len;
}

int  _close(int  fd)
{
	return 0;
}

void _exit(int status) {
    while(1) { }
}


