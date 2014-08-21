#ifndef NRFPP_UART_REDIRECT_H
#define NRFPP_UART_REDIRECT_H

int redirect_write(char *ptr, int len);
int redirect_read(char *ptr, int len);

#endif //NRFPP_UART_REDIRECT_H
