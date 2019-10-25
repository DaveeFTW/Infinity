/*
Copyright (C) 2019, David "Davee" Morgan

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <pspinit.h>

typedef int (
    *INSERT_BTCNF)(char* new_mod, char* before_mod, BtcnfHeader* header, int* size, u16 flags);
typedef int (
    *REPLACE_BTCNF)(char* new_mod, char* replace_mod, BtcnfHeader* header, int* size, u16 flags);
typedef int (*COMPAT_ENTRY)(BtcnfHeader* btcnf,
                            int btcnf_size,
                            INSERT_BTCNF insert_btcnf,
                            REPLACE_BTCNF replace_btcnf,
                            int model,
                            int is_recovery,
                            int firmware);

int compat_entry(BtcnfHeader* btcnf,
                 int btcnf_size,
                 INSERT_BTCNF insert_btcnf,
                 REPLACE_BTCNF replace_btcnf,
                 int model,
                 int is_recovery,
                 int firmware);
