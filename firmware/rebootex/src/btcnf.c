/*
Copyright (C) 2015, David "Davee" Morgan

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

#include "btcnf.h"

#include <string.h>

int InsertModuleBtcnfAfter(char* new_mod,
                           char* after_mod,
                           BtcnfHeader* header,
                           int* size,
                           u16 flags)
{
    /* cast and declare our local variables */
    int i, j;
    ModeEntry* modes = (ModeEntry*)((u32)header + header->modestart);
    ModuleEntry* modules = (ModuleEntry*)((u32)header + header->modulestart);
    char* names = (char*)((u32)header + header->modnamestart);
    int len = strlen(new_mod) + 1;

    /* loop through the modules */
    for (i = 0; i < header->nmodules; i++)
    {
        /* if we find the module we want to insert before */
        if (memcmp(names + modules[i].stroffset, after_mod, strlen(after_mod) + 1) == 0)
        {
            // insert after this module
            i++;

            /* found it! lets move the the whole section so we can fit another entry*/
            memmove(modules + i + 1,
                    modules + i,
                    (*size) - header->modulestart - (i * sizeof(ModuleEntry)));

            /* update all the header variables */
            header->modnamestart += sizeof(ModuleEntry);
            header->modnameend += sizeof(ModuleEntry);
            *size += sizeof(ModuleEntry);
            header->nmodules++;

            /* add the new information */
            modules[i].stroffset = header->modnameend - header->modnamestart;
            modules[i].flags = flags;

            /* copy the string over :P */
            memcpy((char*)header + header->modnameend, new_mod, len);

            /* update the new size and the header modname end */
            *size += len;
            header->modnameend += len;

            /* change the modes */
            for (j = 0; j < header->nmodes; j++)
            {
                modes[j].searchstart = 0;
                modes[j].maxsearch++;
            }

            /* return success */
            return 0;
        }
    }

    /* return fail */
    return -1;
}

int InsertModuleBtcnfBefore(char* new_mod,
                            char* before_mod,
                            BtcnfHeader* header,
                            int* size,
                            u16 flags)
{
    /* cast and declare our local variables */
    int i, j;
    ModeEntry* modes = (ModeEntry*)((u32)header + header->modestart);
    ModuleEntry* modules = (ModuleEntry*)((u32)header + header->modulestart);
    char* names = (char*)((u32)header + header->modnamestart);
    int len = strlen(new_mod) + 1;

    /* loop through the modules */
    for (i = 0; i < header->nmodules; i++)
    {
        /* if we find the module we want to insert before */
        if (memcmp(names + modules[i].stroffset, before_mod, strlen(before_mod) + 1) == 0)
        {
            /* found it! lets move the the whole section so we can fit another entry*/
            memmove(modules + i + 1,
                    modules + i,
                    (*size) - header->modulestart - (i * sizeof(ModuleEntry)));

            /* update all the header variables */
            header->modnamestart += sizeof(ModuleEntry);
            header->modnameend += sizeof(ModuleEntry);
            *size += sizeof(ModuleEntry);
            header->nmodules++;

            /* add the new information */
            modules[i].stroffset = header->modnameend - header->modnamestart;
            modules[i].flags = flags;

            /* copy the string over :P */
            memcpy((char*)header + header->modnameend, new_mod, len);

            /* update the new size and the header modname end */
            *size += len;
            header->modnameend += len;

            /* change the modes */
            for (j = 0; j < header->nmodes; j++)
            {
                modes[j].searchstart = 0;
                modes[j].maxsearch++;
            }

            /* return success */
            return 0;
        }
    }

    /* return fail */
    return -1;
}

int ReplaceModuleBtcnf(char* new_mod, char* replace_mod, BtcnfHeader* header, int* size, u16 flags)
{
    /* cast and declare our local variables */
    int i;
    ModuleEntry* modules = (ModuleEntry*)((u32)header + header->modulestart);
    char* names = (char*)((u32)header + header->modnamestart);
    int len = strlen(new_mod) + 1;

    /* loop through the modules */
    for (i = 0; i < header->nmodules; i++)
    {
        /* if we find the module we want to replace */
        if (memcmp(names + modules[i].stroffset, replace_mod, strlen(replace_mod) + 1) == 0)
        {
            /* add the new information */
            modules[i].stroffset = header->modnameend - header->modnamestart;
            modules[i].flags = flags;

            /* copy the string over :P */
            memcpy((char*)header + header->modnameend, new_mod, len);

            /* update the new size and the header modname end */
            *size += len;
            header->modnameend += len;

            /* return success */
            return 0;
        }
    }

    /* return fail */
    return -1;
}
