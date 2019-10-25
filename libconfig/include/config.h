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

#ifndef LIBCONFIG_H
#define LIBCONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include <pspkerneltypes.h>
#include <psptypes.h>

#define INFINITY_CONFIGURATION_LOCATION ("flash1:/config.inf")

    /**
            This function is used to check if a configuration is already assigned to the system

            @return nonzero if config is assigned else 0 if none assigned
    */
    int infIsConfigAssigned(void);

    /**
            This function is used to load a configuration into memory and assign it, ready to use
       the API

            @param config_path: the path to store the configuration

            @return < 0 on error else success
    */
    int infInitConfig(const char* config_path);

    /**
            This function is used to create a configuration and assign it into memoryspace ready to
       use the API

            @param config_path: the path to store the configuration

            @return < 0 on error else success
    */
    int infCreateConfig(const char* config_path);

    /**
            This function is used to add a configuration already setup into the system

            @param configuration: a pointer to the configuration data
            @param memid: the memory id containing the config
            @param size: the size of the configuration
            @param config_status: the initial status of the configuration
            @param config_path: the file path of the configuration

            @return < 0 on error, else success
    */
    int infAssignConfig(void* configuration,
                        SceUID memid,
                        u32 size,
                        u32 config_status,
                        const char* config_path);

    /**
            This function is used to change the file location of the configuration

            @param new_path: a pointer to a string containing a new path to store the config

            @return < 0 on error, else success
    */
    int infChangeConfigLocation(const char* new_path);

    /**
            This function is used to obtain information from an entry

            @param name: the name of the entry to get info from
            @param data_size: a pointer to an integer to store the size of the data

            @return < 0 on error, else success
    */
    int infGetEntryInfo(const char* name, int* data_size);

    /**
            This function is used to get data from an entry

            @param name: the name of the entry to get data from
            @param buffer: the buffer to store the data
            @param buffer_size: the size of the buffer used to store data

            @return < 0 on error, else success
    */
    int infGetEntry(const char* name, void* buffer, u32 buffer_size);

    /**
            This function is used to add a new entry to the configuration

            @param name: the name of the entry
            @param data: a pointer to data
            @param size: the size of the data

            @return < 0 on error else success
    */
    int infAddEntry(const char* name, const void* data, u32 size);

    /**
            This function is used to update the data in an existing entry
            @note this function handles new sizes of data to be added

            @param name: the name of the entry
            @param data: pointer to the data to save
            @param size: the size of the data

            @return < 0 on error, else success
    */
    int infUpdateEntry(const char* name, const void* data, u32 size);

    /**
            This function is used to delete an entry

            @param name: the name of the entry to delete

            @return < 0 on error else success
    */
    int infDeleteEntry(const char* name);

    /**
            This function is to write a configuration to file

            @return < 0 on error, else success
    */
    int infFlushConfig(void);

    /**
            This function is used to clear the current assigned config

            @return less than 0 for an error else success
    */
    int infDiscardConfig(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LIBCONFIG_H
