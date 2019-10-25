/*
        Kprintf
*/
#include "kprintf.h"
#include "psp_uart.h"
#include <psptypes.h>
#include <stdarg.h>

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#define _PUTC(VAL) uart_dbg_putc(0, (VAL))

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#define DIFF_INT_LONG 0 /* support 'l' parameter ? */

#define FLAG_PLUS 0x80
#define FLAG_ZERO 0x40
#define FLAG_UNSIGN 0x20

#ifdef ENABLE_DEBUG
static inline int numtofile(unsigned long number, int place, char flags, int deci, int chra)
{
    unsigned long num;
    unsigned long digit = 1;
    int stores = 0;

    /* '+','-' */
    if (!(flags & FLAG_UNSIGN))
    {
        if (((long)number) < 0)
        {
            _PUTC('-');
            stores++;
            number = -number;
        }
        else if (flags & FLAG_PLUS)
        {
            _PUTC(number == 0 ? ' ' : '+');
            stores++;
        }
    }

    /* max digit */
    while ((place > 1) || (number >= (digit * deci)))
    {
        // maximum check
        if (digit > digit * deci)
            break;
        digit *= deci;
        place--;
    }

    /* head '0' or ' ' */
    while (digit > 1)
    {
        if ((number / digit) % deci > 0)
            break;
        _PUTC(flags & FLAG_ZERO ? '0' : ' ');
        stores++;
        digit /= deci;
    }

    while (digit)
    {
        num = (number / digit) % deci;
        if (num < 10)
            _PUTC('0' + num);
        else
            _PUTC(chra + num - 10);
        stores++;
        digit /= deci;
    }
    return stores;
}

/* format decomposer */
static int __vprintf(const char* format, va_list arg)
{
    char flags;
    int len_n, len_e;
    char* sptr;
#if DIFF_INT_LONG
    char size;
#endif
    int stores = 0;
    long val_long;
    unsigned char base;

    while (*format)
    {
        if (*format != '%')
            _PUTC(*format++);
        else
        { /* % entry */
            format++;

            flags = 0;
            /* head '+' */
            if (*format == '+')
            {
                flags |= FLAG_PLUS;
                format++;
            }
            else if (*format == '-')
                format++;

            /* head '0' */
            if (*format == '0')
            {
                flags |= FLAG_ZERO;
                format++;
            }

            /* length */
            len_n = len_e = 0;
            while (*format >= '0' && *format <= '9')
                len_n = len_n * 10 + (*format++) - '0';
            /* '.' */
            if (*format == '.')
            {
                format++;
                while (*format >= '0' && *format <= '9')
                    len_e = len_e * 10 + (*format++) - '0';
            }
#if DIFF_INT_LONG
            size = sizeof(int); /* int */
#endif
            base = 10;
        again:
            /* type check */
            switch (*format | 0x20)
            {
                case 'l': /* longlong / long */
#if DIFF_INT_LONG
                    size = (*format & 0x20) ? sizeof(long) : sizeof(longlong);
#endif
                    format++;
                    goto again;
                case 'u':
                    flags |= FLAG_UNSIGN;
                    format++;
                    goto again;

                case 'c':
                    _PUTC((char)va_arg(arg, int));
                    break;

                case 'x':
                    flags |= FLAG_UNSIGN;
                    base = 16;
                case 'd':
#if DIFF_INT_LONG
                    switch (size)
                    {
                        case 1:
                            val_long = va_arg(arg, long);
                        case 2:
                            val_long = va_arg(arg, longlong);
                        default:
                            if (flags & FLAG_UNSIGN)
                                val_long = (unsigned int)va_arg(arg, unsigned int);
                            else
                                val_long = va_arg(arg, int);
                    }
#else
                    if (flags & FLAG_UNSIGN)
                        val_long = (unsigned int)va_arg(arg, int);
                    else
                        val_long = va_arg(arg, int);
#endif
                    stores += numtofile(val_long, len_n, flags, base, *format - 'X' + 'A');
                    break;
                case 's':
                    sptr = va_arg(arg, char*);
                    while (*sptr)
                    {
                        _PUTC(*sptr++);
                        stores++;
                    }
                    break;
                default:
                    break;
            }
            if (*format)
                format++;
        }
    }
    return stores;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Kprintf for PSP uart
/////////////////////////////////////////////////////////////////////////////
void kprintf(const char* text, ...)
{
#ifdef ENABLE_DEBUG
    va_list arg;

    uart_dbg_putc(0, 0x200);
    va_start(arg, text);
    __vprintf(text, arg);
    va_end(arg);
    uart_dbg_putc(0, 0x201);
#endif
}
