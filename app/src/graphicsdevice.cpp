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

#include "graphicsdevice.h"
#include "blockallocator.h"

#include <pspdisplay.h>
#include <pspge.h>
#include <pspgu.h>

#include <malloc.h>

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

GraphicsDevice::GraphicsDevice(void)
    : m_pspguState(GU_UNINITIALISED)
    , m_vramAllocator(new BlockAllocator((u8*)sceGeEdramGetAddr() + 10 * BUF_WIDTH * SCR_HEIGHT,
                                         sceGeEdramGetSize() - 10 * BUF_WIDTH * SCR_HEIGHT))
    , m_fb0(0)
    , m_fb1(0)
    , m_zbp(0)
{
    m_displayList = static_cast<unsigned int*>(memalign(16, 256 * 1024));
}

GraphicsDevice::~GraphicsDevice(void)
{
    shutdown();
    delete m_vramAllocator;
    free(m_displayList);
}

#include <stdio.h>

// initialise/shutdown pspgu
void GraphicsDevice::initialise(void)
{
    if (m_pspguState == GU_INITIALISED)
        return;

    // allocate our double buffers and zbuffer
    m_fb0 = 0;
    m_fb1 = BUF_WIDTH * SCR_HEIGHT * 4;
    m_zbp = BUF_WIDTH * SCR_HEIGHT * 4 * 2;

    sceGuInit();
    sceGuStart(GU_DIRECT, m_displayList);

    /* Buffers */
    sceGuDrawBuffer(GU_PSM_8888, (void*)m_fb0, BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void*)m_fb1, BUF_WIDTH);
    sceGuDepthBuffer((void*)m_zbp, BUF_WIDTH);

    // set clear colour to black
    sceGuClearColor(0xFF000000);
    sceGuClearDepth(0);

    /* Depth stuff */
    sceGuDepthRange(65535, 0);
    sceGuDepthMask(GU_FALSE);

    /* Coordinate Offset */
    sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));

    /* Set Viewport */
    sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);

    /* Clear The Draw Buffer */
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

    /* Viewport Scissoring */
    sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);

    /* Alpha */
    sceGuAlphaFunc(GU_GREATER, 0, 0xff);
    sceGuEnable(GU_ALPHA_TEST);

    /* Depth */
    sceGuDepthFunc(GU_GEQUAL);
    sceGuEnable(GU_DEPTH_TEST);

    sceGuFrontFace(GU_CW);

    /* Culling */
    sceGuEnable(GU_CULL_FACE);

    /* Clipping */
    sceGuEnable(GU_CLIP_PLANES);

    /* Blending */
    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

    /* Shading */
    sceGuShadeModel(GU_SMOOTH);

    /* Finish initialization */
    sceGuFinish();

    /* Sync */
    sceGuSync(0, 0);

    /* Wait for screen */
    sceDisplayWaitVblankStart();

    /* Turn on the display */
    sceGuDisplay(GU_TRUE);

    m_pspguState = GU_INITIALISED;
}

void GraphicsDevice::shutdown(void)
{
    if (m_pspguState == GU_UNINITIALISED)
        return;

    // shutdown GU
    sceGuDisplay(GU_FALSE);
    sceGuFinish();
    sceGuTerm();

    m_pspguState = GU_UNINITIALISED;
}

unsigned int* GraphicsDevice::displayList(void)
{
    return m_displayList;
}

BlockAllocator* GraphicsDevice::vramAllocator(void)
{
    return m_vramAllocator;
}
