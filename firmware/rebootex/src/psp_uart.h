#ifndef __PSP_UART_H__
#define __PSP_UART_H__

// uncomment to enable debugging through serial
//#define ENABLE_DEBUG

void psp_uart_init(int bps);
void psp_uart_putc(int txd);
int psp_uart_getc(void);
void uart_dbg_putc(int arg1, int code);

#endif
