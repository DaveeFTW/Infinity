/*

Copyright (C) 2016, David "Davee" Morgan

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

#ifndef INSTALLUPDATE_H
#define INSTALLUPDATE_H

#include "mutex.h"
#include "thread.h"

class InstallUpdate : public Thread
{
public:
    enum State
    {
        CHECKING_FILES,
        ERROR_WRITE_BACKUP,
        ERROR_WRITE_INFINITYBOOT,
        ERROR_WRITE_INFINITYCTRL,
        ERROR_WRITE_REBOOTEX,
        ERROR_WRITE_MECOMPAT,
        ERROR_WRITE_PROCOMPAT,
        ERROR_WRITE_ARKCOMPAT,
        ERROR_READ_USERSYSTEMLIB,
        ERROR_WRITE_USERSYSTEMLIB,
        UPDATE_UNAVAILABLE,
        INVALID_UPDATE,
        CONTAINER_WRITE_FAILURE,
        CONTAINER_OPEN_FAILURE,
        NO_UPDATER_VERSION,
        NO_INTERNAL_VERSION,
        NO_UPDATE_REQUIRED,
        CHECKING_631_OFW,
        MISSING_631_OFW,
        INSTALLING,
        NO_UTILITIES,
        CANNOT_LOAD_UTILITY,
        CANNOT_START_UTILITY,
        ERROR_UNASSIGN_FLASH0,
        ERROR_ASSIGN_FLASH0,
        FLASHING_BOOTLOADER,
        ERROR_MOUNTING_631,
        ERROR_UNMOUNTING_631,
        ERROR_READING_SYSTIMER_PRX,
        ERROR_READING_INIT_PRX,
        ERROR_FLASHING_BOOTLOADER,
        FLASHING_CORE_FLASH0,
        ERROR_OPENING_CORE_FLASH0,
        ERROR_FLASHING_CORE_FLASH0,
        ERROR_CLOSING_CORE_FLASH0,
        FLASHING_SUBSET_FLASH0,
        ERROR_OPENING_SUBSET_FLASH0,
        ERROR_FLASHING_SUBSET_FLASH0,
        ERROR_CLOSING_SUBSET_FLASH0,
        INSTALL_COMPLETE,
        ERROR_APPLYING_SIGNCHECK,
        ERROR_INVALID_BOOTLOADER
    };

public:
    InstallUpdate(void);
    ~InstallUpdate(void);

    State state(void) const;
    void install(void);
    void cancel(void);

private:
    void run(void) override;
    void setState(State state);

private:
    enum EventBits
    {
        EVENT_CANCEL,
        EVENT_INSTALL
    };

    InstallUpdate::State m_state;
    Mutex m_mutex;
    SceUID m_eventId;
};

#endif // INSTALLUPDATE_H
