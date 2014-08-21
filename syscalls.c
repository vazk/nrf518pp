#include "nrfpp/uart_redirect.h"

int  _open(const char* file, int  flags, int  mode)
{
	return 0;
}

int  _read(int file, char *ptr, int len)
{
    return redirect_read(ptr, len);
}

int  _write(int file, char *ptr, int len)
{
    return redirect_write(ptr, len);
}

int  _close(int  fd)
{
	return 0;
}

void  _exit(int status) {
    while(1) { }
}
