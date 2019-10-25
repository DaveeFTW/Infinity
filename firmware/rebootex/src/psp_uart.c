/****************************************************************************
        PSP IPL HP REMOTE UART Driver
****************************************************************************/
#include "psp_uart.h"
#include <psptypes.h>

/*
+-------------+----------+------+------------+
|  module     | device   | port | IO base    |
+-------------+----------+------+------------+
|uart4.prx    | uart4    |  3   | 0xbe4c0000 |
|hpremote.prx | hpremote |  4   | 0xbe500000 |
|sircs.prx    | irda     |  5   | 0xbe540000 |
+-------------+----------+------+------------+
*/

#define KPRINTF_BPS 115200

/* bit 5 = TX EMPTY , bit4 = rxready , bit7 =0�̎��ASleep�s�� ,bit3=1�̎�sleep�s�� */
#define psp_uart_sts_BSY 0x80
#define psp_uart_sts_TI 0x20
#define psp_uart_sts_RE 0x10
#define psp_uart_sts_BIT3 0x08

/////////////////////////////////////////////////////////////////////////////

struct psp_uart_hw
{
    volatile unsigned int txd;  // +00 : txd
    volatile unsigned int r04;  // +04
    volatile unsigned int r08;  // +08 :
    volatile unsigned int r0c;  // +0c ? 0x0000
    volatile unsigned int r10;  // +10 ? 0x0000
    volatile unsigned int r14;  // +14 ? 0x0000
    volatile unsigned int sts;  // +18 : status */
    volatile unsigned int r1c;  // +1c : unk , 0x1ff */
    volatile unsigned int r20;  // +20 ? 0x0000
    volatile unsigned int brgh; // +24 :(96000000 / bps) >> 6
    volatile unsigned int brgl; // +28 :(96000000 / bps) & 0x3f
    volatile unsigned int r2c;  // +2c : unknown , 0x70 = enable , 0x60 = stop?
    volatile unsigned int r30;  // +30 : unk , 0x0301
    volatile unsigned int r34;  // +34 : 0x00=0x00 , 0x08=0x09,0x16=0x12,0x24=0x12,0x27=0x24 */
    volatile unsigned int r38;  // +38 : unk , uart4=00 , uart=0x10
    volatile unsigned int r3c;  // +3c : unk , uart4=00 , uart=0x28d
    volatile unsigned int r40;  // +40 ? 0
    volatile unsigned int r44;  // +44 :  unk , 0x07ff / 0x0000
    volatile unsigned int r48;  // +48 ? 0
    volatile unsigned int r4c;  // +4c ? 0
    volatile unsigned int r50;  // +50 : unk , set bit14 init,resume = 0x4000
    volatile unsigned int r54;  // +54 ? 0
    volatile unsigned int r58;  // +58 : unk , set bit9  init,resume = 0x0020
    volatile unsigned int r5c;  // +5c ?
    volatile unsigned int r60;  // +60
    volatile unsigned int r64;  // +64
    volatile unsigned int r68;  // +68
    volatile unsigned int r6c;  // +6c
    volatile unsigned int r70;  // +70
    volatile unsigned int r74;  // +74
    volatile unsigned int r78;  // +78 : unk , set bit3 init resume , set bit19 sleep
};

void psp_uart_putc(int txd)
{
#ifdef ENABLE_DEBUG
    struct psp_uart_hw* uart = (struct psp_uart_hw*)0xbe500000;
    while (uart->sts & psp_uart_sts_TI)
        asm("sync" ::);
    uart->txd = txd;
#endif
}

int psp_uart_getc(void)
{
#ifdef ENABLE_DEBUG
    struct psp_uart_hw* uart = (struct psp_uart_hw*)0xbe500000;

    if (uart->sts & psp_uart_sts_RE)
        return -1;
    return (uart->txd) & 0xff;
#endif
    return -1;
}

void psp_uart_init(int bps)
{
#ifdef ENABLE_DEBUG
    struct psp_uart_hw* uart = (struct psp_uart_hw*)0xbe500000;
    int brg = 96000000 / bps;

    // UART BUS CLOCK ENABLE
    *(unsigned int*)0xbc100050 |= 0x4000;

    // UART4 BUS CLOCK ENABLE
    // Sysreg_driver_Unknown_7725ca08(4)
    *(unsigned int*)0xbc100058 |= (0x40 << 4);

    // UART4 IO enable
    // SysregUartIoEnable(4);
    *(unsigned int*)0xbc100078 |= (0x00010000 << 4);

#if 1
    // hpremote
    uart->r30 = 0x300;
    uart->brgh = brg >> 6;   // 24 : 4800bps == 0x0138 : 20000
    uart->brgl = brg & 0x3f; // 28 : 4800bps == 0x0020
    uart->r2c = 0x070;
    uart->r34 = 0;
    uart->r38 = 0x10;
    uart->r04 = uart->r04;
    uart->r30 = 0x301;
#endif

#if 0
	// irda
	uart->r30 = 0x300;
	uart->brgh = brg>>6;   // 24
	uart->brgl = brg&0x3f; // 28
	uart->r20 = 0x0d;
	uart->r2c = 0x070;
	uart->r1c = 0x280;
	uart->r34 = 0;
	uart->r38 = 0x50;
	uart->r04 = 0xffff;
	uart->r30 = 0x303 or 0x307;
#endif

#if 0
	// uart4 init
	uart->brgh = brg>>6; // 24
	uart->brgl = brg&0x3f; // 28
	uart->r2c  = 0x60;
	uart->r30  = 0x0301;
	uart->r2c  |= 0x10;
	uart->r44  |= 0x07ff;
#endif
#if 0
	// ? (old source code)
	uart->brgh = brg>>6;
	uart->brgl = brg&0x3f;
	uart->r2c  = 0x70;
	uart->r30  = 0x0301;
	uart->r34  = 0x00;
	uart->r38  = 0x10;
	uart->r3c  = 0x028d;
	uart->r44 |= 0x07ff;
//  uart->r1c  = 0x1ff;
#endif
#endif
}

void uart_dbg_putc(int arg1, int code)
{
#ifdef ENABLE_DEBUG
    struct psp_uart_hw* uart = (struct psp_uart_hw*)0xbe500000;

    if (code & 0xffffff00)
    {
        switch (code)
        {
            case 0x200:
                // OPEN UART
                psp_uart_init(KPRINTF_BPS);
                psp_uart_putc(0);
                psp_uart_putc(0);
                break;
            case 0x201:
                // CLOSE UART
                while (uart->sts & psp_uart_sts_TI)
                    ;
        }
        return;
    }

    if (code == 0x0a)
    {
        psp_uart_putc(0x0d);
    }
    psp_uart_putc(code);
#endif
}
