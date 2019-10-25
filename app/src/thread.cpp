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

#include "thread.h"

#include <pspthreadman.h>

Thread::Thread(void)
    : m_thid(-1)
{
}

Thread::~Thread(void)
{
    sceKernelTerminateDeleteThread(m_thid);
}

bool Thread::isFinished(void)
{
    SceKernelThreadInfo status;
    status.size = sizeof(SceKernelThreadInfo);

    if (sceKernelReferThreadStatus(m_thid, &status) < 0)
        return false;

    return status.status & PSP_THREAD_STOPPED;
}

bool Thread::isRunning(void)
{
    SceKernelThreadInfo status;
    status.size = sizeof(SceKernelThreadInfo);

    if (sceKernelReferThreadStatus(m_thid, &status) < 0)
        return false;

    return status.status & PSP_THREAD_RUNNING;
}

void Thread::start(void)
{
    m_thid = sceKernelCreateThread("infinityThread", &Thread::entry, 0x12, 0x10000, 0, NULL);

    if (m_thid >= 0)
    {
        Thread* storage[] = { this };
        sceKernelStartThread(m_thid, 4, storage);
    }
}

int Thread::entry(unsigned int args, void* argp)
{
    Thread* thread = *(Thread**)argp;
    thread->run();
    return 0;
}
