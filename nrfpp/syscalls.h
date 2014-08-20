#ifndef  SYSCALLS_H
#define  SYSCALLS_H

#include <errno.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <stdint.h>

int      _write(int fd, char *ptr, int len);
int      _open(const char  *file, int  flags, int  mode);
int      _close(int fd);
int      _execve(char *name, char **argv, char **env);
int      _fork();
int      _fstat(int fd, struct stat *st);
int      _getpid();
int      _isatty(int fd);
int      _kill(int pid, int sig);
int      _link(char *old, char *new);
int      _lseek(int fd, int ptr, int dir);
caddr_t  _sbrk(int incr);
int      _read(int fd, char *ptr, int len);
int      _stat(const char *filepath, struct stat *st);
clock_t  _times(struct tms *buf);
int      _unlink(char *name);
int      _wait(int *status);

#endif
