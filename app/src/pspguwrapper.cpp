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

#include "pspguwrapper.h"
#include "vram.h"

#include <cstddef>

#include <pspdisplay.h>
#include <pspgu.h>

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

PSPGUManager* PSPGUManager::g_instance = NULL;

PSPGUManager::PSPGUManager(void) {}

PSPGUManager* PSPGUManager::instance(void)
{
    if (g_instance == NULL)
        g_instance = new PSPGUManager();

    return g_instance;
}

void PSPGUManager::init(void)
{
    sceGuInit();
    sceGuStart(GU_DIRECT, m_displayList);

    void* fbp0 = getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_8888);
    void* fbp1 = getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_8888);
    void* zbp = getStaticVramBuffer(BUF_WIDTH, SCR_HEIGHT, GU_PSM_4444);

    /* Buffers */
    sceGuDrawBuffer(GU_PSM_8888, fbp0, BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, fbp1, BUF_WIDTH);
    sceGuDepthBuffer(zbp, BUF_WIDTH);

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

    sceGuFrontFace(GU_CCW);

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
}

unsigned int* PSPGUManager::displayList(void)
{
    return m_displayList;
}
