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

#include <config.h>

#include <pspkernel.h>
#include <pspsdk.h>

#include <string.h>

typedef struct
{
    u32 signature;
    u32 nentries;
} InfinityConfigHeader;

typedef struct
{
    u32 size;
    u32 ent_size;
    char name;
} InfinityConfigEntry;

#define INFINITY_CONFIG_VERSION (1)
#define INFINITY_CONFIG_SIGNATURE(str, ver)                                                        \
    (((u8)ver << 24) | (str[2] << 16) | (str[1] << 8) | (str[0]))

#define INFINITY_INTERNAL_CONFIG_OPENED (1 << 0)
#define INFINITY_INTERNAL_CONFIG_FLUSH_REQ (1 << 1)

InfinityConfigHeader* g_config = NULL;
u32 g_config_size = 0;
SceUID g_config_memid = -1;
u32 g_config_status = 0;
char g_cur_config[256];

void load_config(void)
{
    /* attempt to load the config */
    int res = infInitConfig(INFINITY_CONFIGURATION_LOCATION);

    /* check for error */
    if (res < 0)
    {
        /* error occured, lets try create a new one */
        res = infCreateConfig(INFINITY_CONFIGURATION_LOCATION);

        /* check for error again */
        if (res < 0)
        {
            /* just exit loop */
            return;
        }
    }
}

int infIsConfigAssigned(void)
{
    /* return if assigned */
    return (g_config_status & INFINITY_INTERNAL_CONFIG_OPENED);
}

int infDiscardConfig(void)
{
    u32 k1 = pspSdkSetK1(0);

#ifdef KPRINTF_DEBUG
    Kprintf("***START %s***\n", __FUNCTION__);
#endif

    /* check for already opened configuration */
    if ((g_config_status & INFINITY_INTERNAL_CONFIG_OPENED) == 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error, infinity configuration not opened!\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return -1;
    }

    /* deallocate memory */
    sceKernelFreePartitionMemory(g_config_memid);

    /* reset global */
    g_config_status = 0;
    g_config_memid = -1;
    g_config_size = 0;
    g_config = NULL;

    /* reset path */
    memset(g_cur_config, 0, sizeof(g_cur_config));

#ifdef KPRINTF_DEBUG
    Kprintf("***END %s***\n", __FUNCTION__);
#endif

    pspSdkSetK1(k1);
    return 0;
}

int infFlushConfig(void)
{
    u32 k1 = pspSdkSetK1(0);

#ifdef KPRINTF_DEBUG
    Kprintf("***START %s***\n", __FUNCTION__);
#endif

    /* check for already opened configuration */
    if ((g_config_status & INFINITY_INTERNAL_CONFIG_OPENED) == 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error, infinity configuration not opened!\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return -1;
    }

    /* check for any modifications */
    if ((g_config_status & INFINITY_INTERNAL_CONFIG_FLUSH_REQ) == 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Flush is not required\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return 0;
    }

    /* Open the file with write mode */
    SceUID fd = sceIoOpen(g_cur_config, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

    /* check for error */
    if (fd < 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error 0x%08X occured opening file %s\n",
                fd,
                g_cur_config);
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return fd;
    }

    /* write the data */
    int res = sceIoWrite(fd, g_config, g_config_size);

    /* close file */
    sceIoClose(fd);

    /* check for error */
    if (res < 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error 0x%08X writing %i bytes\n",
                res,
                g_config_size);
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return res;
    }

    /* check if the correct size was written */
    if (res != g_config_size)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error  size mismatch %i != %i bytes\n",
                res,
                g_config_size);
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return -1;
    }

    /* it's written, clear flag */
    g_config_status &= ~INFINITY_INTERNAL_CONFIG_FLUSH_REQ;

#ifdef KPRINTF_DEBUG
    Kprintf("***END %s***\n", __FUNCTION__);
#endif

    /* return success */
    pspSdkSetK1(k1);
    return 0;
}

int infUpdateEntry(const char* name, const void* data, u32 size)
{
    // int i;
    // u32 k1 = pspSdkSetK1(0);

    int res = infDeleteEntry(name);

    if (res >= 0)
    {
        return infAddEntry(name, data, size);
    }

    return res;

#if 0
#ifdef KPRINTF_DEBUG
	Kprintf("***START %s***\n", __FUNCTION__);
#endif

	/* check for already opened configuration */
	if ((g_config_status & INFINITY_INTERNAL_CONFIG_OPENED) == 0)
	{
#ifdef KPRINTF_DEBUG
		Kprintf("*\t" "Error, infinity configuration not opened!\n");
#endif

#ifdef KPRINTF_DEBUG
		Kprintf("***END %s***\n", __FUNCTION__);
#endif
		pspSdkSetK1(k1);
		return -1;
	}

#ifdef KPRINTF_DEBUG
	Kprintf("*\t" "Searching for entry %s\n", name);
#endif

	/* set our seek value */
	u32 seek = sizeof(InfinityConfigHeader);

	/* loop through entries */
	for (i = 0; i < g_config->nentries; i++)
	{
		/* get our pointer */
		InfinityConfigEntry *ent = (InfinityConfigEntry *)((u32)g_config + seek);

		/* strcmp the data */
		if (strcmp(name, &ent->name) == 0)
		{
#ifdef KPRINTF_DEBUG
			Kprintf("*\t" "Found entry, pos %i\n", i);
#endif
			/* check if size decreased */
			if (size < ent->size)
			{
				/* it did, reorder */
				memmove((void *)((u32)ent + ent->ent_size + size), (void *)((u32)ent + ent->ent_size + ent->size), g_config_size - seek - ent->size);

				/* update global */
				g_config_size -= (ent->size - size);

				/* now update data */
				ent->size = size;
				memcpy((void *)((u32)ent + ent->ent_size), data, size);
			}

			/* check if size increase, incase, re-alloc */
			else if (size > ent->size)
			{
				/* calculate the new size */
				u32 new_size = g_config_size + (size - ent->size);

				/* allocate the new memory */
				SceUID memid = sceKernelAllocPartitionMemory(1, "InfinityConfig", PSP_SMEM_High, new_size, NULL);

				/* check if it allocated */
				if (memid < 0)
				{
#ifdef KPRINTF_DEBUG
					Kprintf("*\t" "Error 0x%08X allocating %i bytes\n", memid, new_size);
#endif

#ifdef KPRINTF_DEBUG
					Kprintf("***END %s***\n", __FUNCTION__);
#endif
					pspSdkSetK1(k1);
					return memid;
				}

				/* copy the config over */
				void *config = sceKernelGetBlockHeadAddr(memid);

				/* copy part */
				memcpy(config, g_config, seek + ent->ent_size);

				/* copy data */
				InfinityConfigEntry *new_ent = (InfinityConfigEntry *)(config + seek);

				/* copy new data */
				memcpy((void *)((u32)new_ent + new_ent->ent_size), data, size);
				new_ent->size = size;

				/* append rest of data  */
				memcpy((void *)((u32)new_ent + new_ent->ent_size + size), (void *)((u32)ent + ent->ent_size + ent->size), g_config_size - seek - ent->ent_size - ent->size);

				/* set new header */
				InfinityConfigHeader *header = (InfinityConfigHeader *)config;

				/* update globals */
				g_config = header;
				g_config_size = new_size;

				/* backup previous memid */
				SceUID memid_temp = g_config_memid;

				/* set new memid */
				g_config_memid = memid;

				/* free previous memory */
				sceKernelFreePartitionMemory(memid_temp);
			}

			/* else same size, just copy over */
			else
			{
				/* but check if the data is the same */
				if (memcmp((void *)((u32)ent + ent->ent_size), data, size) == 0)
				{
#ifdef KPRINTF_DEBUG
					Kprintf("***END %s***\n", __FUNCTION__);
#endif
					/* done */
					pspSdkSetK1(k1);
					return 0;
				}
				else
				{
					memcpy((void *)((u32)ent + ent->ent_size), data, size);
				}
			}

#ifdef KPRINTF_DEBUG
			Kprintf("***END %s***\n", __FUNCTION__);
#endif

			/* set flush bit */
			g_config_status |= INFINITY_INTERNAL_CONFIG_FLUSH_REQ;

			/* done */
			pspSdkSetK1(k1);
			return 0;
		}

		/* increment */
		seek += ent->ent_size + ent->size;
	}

#ifdef KPRINTF_DEBUG
	Kprintf("*\t" "Entry not found\n");
#endif
#ifdef KPRINTF_DEBUG
	Kprintf("***END %s***\n", __FUNCTION__);
#endif

	pspSdkSetK1(k1);
	return -1;
#endif
}

int infDeleteEntry(const char* name)
{
    int i;
    u32 k1 = pspSdkSetK1(0);

#ifdef KPRINTF_DEBUG
    Kprintf("***START %s***\n", __FUNCTION__);
#endif

    /* check for already opened configuration */
    if ((g_config_status & INFINITY_INTERNAL_CONFIG_OPENED) == 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error, infinity configuration not opened!\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return -1;
    }

    /* set the initial seek past the header */
    u32 seek = sizeof(InfinityConfigHeader);

    /* loop through entries */
    for (i = 0; i < g_config->nentries; i++)
    {
        /* get our pointer */
        InfinityConfigEntry* ent = (InfinityConfigEntry*)((u32)g_config + seek);

        /* strcmp the data */
        if (strcmp(name, &ent->name) == 0)
        {
            /* get the size of entry */
            u32 ent_size = ent->ent_size + ent->size;

            /* we found it!! now, remove it */
            memmove((void*)ent, (void*)((u32)ent + ent_size), g_config_size - seek - ent_size);

            /* update globals */
            g_config->nentries--;
            g_config_size -= ent_size;
            g_config_status |= INFINITY_INTERNAL_CONFIG_FLUSH_REQ;

#ifdef KPRINTF_DEBUG
            Kprintf("*\t"
                    "Done, entry %s is now removed\n",
                    name);
#endif
#ifdef KPRINTF_DEBUG
            Kprintf("***END %s***\n", __FUNCTION__);
#endif
            pspSdkSetK1(k1);
            return 0;
        }

        /* increment */
        seek += ent->ent_size + ent->size;
    }

#ifdef KPRINTF_DEBUG
    Kprintf("*\t"
            "Error, %s does not exist\n",
            name);
#endif
#ifdef KPRINTF_DEBUG
    Kprintf("***END %s***\n", __FUNCTION__);
#endif
    pspSdkSetK1(k1);
    return -1;
}

int infAddEntry(const char* name, const void* data, u32 size)
{
    int i;
    u32 k1 = pspSdkSetK1(0);

#ifdef KPRINTF_DEBUG
    Kprintf("***START %s***\n", __FUNCTION__);
#endif

    /* check for already opened configuration */
    if ((g_config_status & INFINITY_INTERNAL_CONFIG_OPENED) == 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error, infinity configuration not opened!\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return -1;
    }

    /* set the initial seek past the header */
    u32 seek = sizeof(InfinityConfigHeader);

    /* loop through entries */
    for (i = 0; i < g_config->nentries; i++)
    {
        /* get our pointer */
        InfinityConfigEntry* ent = (InfinityConfigEntry*)((u32)g_config + seek);

        /* strcmp the data */
        if (strcmp(name, &ent->name) == 0)
        {
#ifdef KPRINTF_DEBUG
            Kprintf("*\t"
                    "Error, Found entry, already exists %s\n",
                    name);
#endif
#ifdef KPRINTF_DEBUG
            Kprintf("***END %s***\n", __FUNCTION__);
#endif
            pspSdkSetK1(k1);
            return -1;
        }

        /* increment */
        seek += ent->ent_size + ent->size;
    }

#ifdef KPRINTF_DEBUG
    Kprintf("*\t"
            "Calculating size requirements\n",
            name);
#endif
    /* get size */
    u32 new_size = g_config_size + size + strlen(name) + sizeof(InfinityConfigEntry);

    /* allocate the new memory */
    SceUID memid =
        sceKernelAllocPartitionMemory(1, "InfinityConfig", PSP_SMEM_High, new_size, NULL);

    /* check if it allocated */
    if (memid < 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error 0x%08X allocating %i bytes\n",
                memid,
                new_size);
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return memid;
    }

    /* copy the config over */
    void* config = sceKernelGetBlockHeadAddr(memid);

    /* copy */
    memcpy(config, (void*)g_config, g_config_size);

    /* generate new entry */
    InfinityConfigEntry* ent = (InfinityConfigEntry*)((u32)config + g_config_size);

    /* add details */
    strcpy(&ent->name, name);
    ent->ent_size = sizeof(InfinityConfigEntry) + strlen(name);
    ent->size = size;

    /* add data now */
    memcpy((void*)((u32)ent + ent->ent_size), data, size);

    /* set new header */
    InfinityConfigHeader* header = (InfinityConfigHeader*)config;
    header->nentries++;

    /* update globals */
    g_config = header;
    g_config_size = new_size;

    /* backup previous memid */
    SceUID memid_temp = g_config_memid;

    /* set new memid */
    g_config_memid = memid;

    /* free previous memory */
    sceKernelFreePartitionMemory(memid_temp);

    /* set the flush flag */
    g_config_status |= INFINITY_INTERNAL_CONFIG_FLUSH_REQ;

#ifdef KPRINTF_DEBUG
    Kprintf("*\t"
            "Complete, flush flag set\n");
#endif
#ifdef KPRINTF_DEBUG
    Kprintf("***END %s***\n", __FUNCTION__);
#endif

    int res;
    infGetEntryInfo(name, &res);
    pspSdkSetK1(k1);
    return 0;
}

int infGetEntry(const char* name, void* buffer, u32 buffer_size)
{
    int i;
    u32 k1 = pspSdkSetK1(0);

#ifdef KPRINTF_DEBUG
    Kprintf("***START %s***\n", __FUNCTION__);
#endif

    /* check for already opened configuration */
    if ((g_config_status & INFINITY_INTERNAL_CONFIG_OPENED) == 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error, infinity configuration not opened!\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif

        pspSdkSetK1(k1);
        return -1;
    }

#ifdef KPRINTF_DEBUG
    Kprintf("*\t"
            "Searching for entry %s\n",
            name);
#endif

    /* set our seek value */
    u32 seek = sizeof(InfinityConfigHeader);

    /* loop through entries */
    for (i = 0; i < g_config->nentries; i++)
    {
        /* get our pointer */
        InfinityConfigEntry* ent = (InfinityConfigEntry*)((u32)g_config + seek);

        /* strcmp the data */
        if (strcmp(name, &ent->name) == 0)
        {
#ifdef KPRINTF_DEBUG
            Kprintf("*\t"
                    "Found entry, data size %i\n",
                    ent->size);
#endif
#ifdef KPRINTF_DEBUG
            Kprintf("***END %s***\n", __FUNCTION__);
#endif
            /* we found it!! */
            memcpy(buffer,
                   (void*)((u32)ent + ent->ent_size),
                   (ent->size < buffer_size) ? (ent->size) : (buffer_size));

            pspSdkSetK1(k1);
            return (ent->size < buffer_size) ? (ent->size) : (buffer_size);
        }

        /* increment */
        seek += ent->ent_size + ent->size;
    }

#ifdef KPRINTF_DEBUG
    Kprintf("*\t"
            "Entry not found\n");
#endif
#ifdef KPRINTF_DEBUG
    Kprintf("***END %s***\n", __FUNCTION__);
#endif

    pspSdkSetK1(k1);
    return -1;
}

int infGetEntryInfo(const char* name, int* data_size)
{
    int i;
    u32 k1 = pspSdkSetK1(0);

#ifdef KPRINTF_DEBUG
    Kprintf("***START %s***\n", __FUNCTION__);
#endif

    /* check for already opened configuration */
    if ((g_config_status & INFINITY_INTERNAL_CONFIG_OPENED) == 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error, infinity configuration not opened!\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return -1;
    }

#ifdef KPRINTF_DEBUG
    Kprintf("*\t"
            "Searching for entry %s\n",
            name);
#endif

    /* set our seek value */
    u32 seek = sizeof(InfinityConfigHeader);

    /* loop through entries */
    for (i = 0; i < g_config->nentries; i++)
    {
        /* get our pointer */
        InfinityConfigEntry* ent = (InfinityConfigEntry*)((u32)g_config + seek);

        /* strcmp the data */
        if (strcmp(name, &ent->name) == 0)
        {
#ifdef KPRINTF_DEBUG
            Kprintf("*\t"
                    "Found entry, pos %i\n",
                    i);
#endif
#ifdef KPRINTF_DEBUG
            Kprintf("***END %s***\n", __FUNCTION__);
#endif
            /* we found it!! */
            data_size[0] = ent->size;

            pspSdkSetK1(k1);
            return 0;
        }

        /* increment */
        seek += ent->ent_size + ent->size;
    }

#ifdef KPRINTF_DEBUG
    Kprintf("*\t"
            "Entry not found\n");
#endif
#ifdef KPRINTF_DEBUG
    Kprintf("***END %s***\n", __FUNCTION__);
#endif

    pspSdkSetK1(k1);
    return -1;
}

int infAssignConfig(void* configuration,
                    SceUID memid,
                    u32 size,
                    u32 config_status,
                    const char* config_path)
{
    u32 k1 = pspSdkSetK1(0);

#ifdef KPRINTF_DEBUG
    Kprintf("***START %s***\n", __FUNCTION__);
#endif

    /* check for already opened configuration */
    if (g_config_status & INFINITY_INTERNAL_CONFIG_OPENED)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error, infinity configuration already opened!\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return -1;
    }

    /* now check if the data matches a header */
    InfinityConfigHeader* header = (InfinityConfigHeader*)configuration;

    /* check the header */
    if (header->signature != INFINITY_CONFIG_SIGNATURE("MLC", INFINITY_CONFIG_VERSION))
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "error with signature 0x%08X != 0x%08X\n",
                header->signature,
                INFINITY_CONFIG_SIGNATURE("MLC", INFINITY_CONFIG_VERSION));
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return -1;
    }

    /* copy to globals */
    g_config_memid = memid;
    g_config_size = size;
    g_config = header;

    /* copy the string */
    strncpy(g_cur_config, config_path, sizeof(g_cur_config));

    /* update the status */
    g_config_status |= INFINITY_INTERNAL_CONFIG_OPENED;
    g_config_status |= config_status;

#ifdef KPRINTF_DEBUG
    Kprintf("***END %s***\n", __FUNCTION__);
#endif

    /* return success */
    pspSdkSetK1(k1);
    return 0;
}

int infChangeConfigLocation(const char* new_path)
{
    u32 k1 = pspSdkSetK1(0);

#ifdef KPRINTF_DEBUG
    Kprintf("***START %s***\n", __FUNCTION__);
#endif

    /* check for already opened configuration */
    if ((g_config_status & INFINITY_INTERNAL_CONFIG_OPENED) == 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error, infinity configuration not opened!\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        pspSdkSetK1(k1);
        return -1;
    }

#ifdef KPRINTF_DEBUG
    Kprintf("***END %s***\n", __FUNCTION__);
#endif

    /* copy the string */
    strncpy(g_cur_config, new_path, sizeof(g_cur_config));
    pspSdkSetK1(k1);
    return 0;
}

int infCreateConfig(const char* config_path)
{
    u32 k1 = pspSdkSetK1(0);

#ifdef KPRINTF_DEBUG
    Kprintf("***START %s***\n"
            "*\t"
            "Creating configuration %s\n",
            __FUNCTION__,
            config_path);
#endif

    /* check for already opened configuration */
    if (g_config_status & INFINITY_INTERNAL_CONFIG_OPENED)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error, infinity configuration already opened!\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        /* error */
        pspSdkSetK1(k1);
        return -1;
    }

    /* open the config */
    SceUID fd = sceIoOpen(config_path, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

    /* check if opened incorrectly */
    if (fd < 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error opening file %s, error 0x%08X\n",
                config_path,
                fd);
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        /* error */
        pspSdkSetK1(k1);
        return fd;
    }

    /* set size to the config */
    u32 size = sizeof(InfinityConfigHeader);

    /* allocate memory for the header */
    /* now allocate memory */
    SceUID memid =
        sceKernelAllocPartitionMemory(1, "InfinityConfigurations", PSP_SMEM_High, size, NULL);

    /* check for error */
    if (memid < 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error allocating memory size %i\n",
                size);
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif

        /* close file */
        sceIoClose(fd);
        pspSdkSetK1(k1);
        return memid;
    }

    /* get the pointer to the memory */
    InfinityConfigHeader* configuration = (InfinityConfigHeader*)sceKernelGetBlockHeadAddr(memid);

    /* now set to default values */
    configuration->signature = INFINITY_CONFIG_SIGNATURE("MLC", INFINITY_CONFIG_VERSION);
    configuration->nentries = 0;

    sceIoWrite(fd, configuration, sizeof(InfinityConfigHeader));
    sceIoClose(fd);

#ifdef KPRINTF_DEBUG
    Kprintf("***END %s***\n", __FUNCTION__);
#endif

    /* now assign the configuration */
    pspSdkSetK1(k1);
    return infAssignConfig(configuration, memid, sizeof(InfinityConfigHeader), 0, config_path);
}

int infInitConfig(const char* config_path)
{
    u32 k1 = pspSdkSetK1(0);

#ifdef KPRINTF_DEBUG
    Kprintf("***START %s***\n"
            "*\t"
            "Opening configuration %s\n",
            __FUNCTION__,
            config_path);
#endif

    /* check for already opened configuration */
    if (g_config_status & INFINITY_INTERNAL_CONFIG_OPENED)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error, infinity configuration already opened!\n");
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        /* error */
        pspSdkSetK1(k1);
        return -1;
    }

    /* open the config */
    SceUID fd = sceIoOpen(config_path, PSP_O_RDONLY, 0777);

    /* check if opened incorrectly */
    if (fd < 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error opening file %s, error 0x%08X\n",
                config_path,
                fd);
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif
        /* error */
        pspSdkSetK1(k1);
        return fd;
    }

    /* get size */
    int size = sceIoLseek32(fd, 0, PSP_SEEK_END);
    sceIoLseek32(fd, 0, PSP_SEEK_SET);

#ifdef KPRINTF_DEBUG
    Kprintf("*\t"
            "File size = 0x%08X bytes (%i)\n",
            size,
            size);
#endif

    /* now allocate memory */
    SceUID memid =
        sceKernelAllocPartitionMemory(1, "InfinityConfigurations", PSP_SMEM_High, size, NULL);

    /* check for error */
    if (memid < 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error allocating memory size %i\n",
                size);
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif

        /* close file */
        sceIoClose(fd);
        pspSdkSetK1(k1);
        return memid;
    }

    /* get the pointer to the memory */
    void* configuration = sceKernelGetBlockHeadAddr(memid);

    /* read the data into the buffer */
    size = sceIoRead(fd, configuration, size);

    /* close file */
    sceIoClose(fd);

    /* check for error reading */
    if ((int)size < 0)
    {
#ifdef KPRINTF_DEBUG
        Kprintf("*\t"
                "Error reading config, error 0x%08X\n",
                size);
#endif

#ifdef KPRINTF_DEBUG
        Kprintf("***END %s***\n", __FUNCTION__);
#endif

        /* free memory */
        sceKernelFreePartitionMemory(memid);
        pspSdkSetK1(k1);
        return size;
    }

#ifdef KPRINTF_DEBUG
    Kprintf("***END %s***\n", __FUNCTION__);
#endif

    /* now assign the configuration */
    pspSdkSetK1(k1);
    return infAssignConfig(configuration, memid, size, 0, config_path);
}
