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

#include "snowscene.h"
#include "gltext.h"
#include "pspguwrapper.h"
#include "randomgenerator.h"
#include "vram.h"

#include <pspgu.h>
#include <pspgum.h>
#include <psputils.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

class Snowflake
{
public:
    Snowflake(ScePspFVector2 position, ScePspFVector2 velocity)
        : m_position(position)
        , m_velocity(velocity)
    {
    }

    void update(float dt)
    {
        m_position.x += dt * m_velocity.x;
        m_position.y += dt * m_velocity.y;
    }

    float x(void) { return m_position.x; }

    float y(void) { return m_position.y; }

    float vx(void) { return m_velocity.x; }

    float vy(void) { return m_velocity.y; }

private:
    ScePspFVector2 m_position;
    ScePspFVector2 m_velocity;
};

SnowScene::SnowScene()
    : m_grid(SCR_WIDTH * SCR_HEIGHT, false)
    , m_gridVertices(NULL)
    , m_rng(new RandomGenerator(time(NULL)))
    , m_spawnChance(5)
    , m_spawnMax(10)
    , m_baseVelocityX(0.f)
    , m_deltaVelocityX(8.f)
    , m_baseVelocityY(50.f)
    , m_deltaVelocityY(15.f)
{
}

void SnowScene::init(void)
{
    // set clear colour to black
    sceGuClearColor(0xff000000);
    sceGuClearDepth(0);
}

void SnowScene::render(void)
{
    sceGuStart(GU_DIRECT, PSPGUManager::instance()->displayList());
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

    // setup ortho projection with psp coordinates
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumOrtho(0.f, SCR_WIDTH, 0.f, SCR_HEIGHT, -1.f, 1.f);

    // set view and model to identity
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();

    sceKernelDcacheWritebackAll();

    // draw flakes
    sceGumDrawArray(GU_LINES,
                    GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D,
                    SCR_WIDTH * 2,
                    0,
                    m_gridVertices);
    sceGumDrawArray(GU_POINTS,
                    GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D,
                    m_fallingVertices.size(),
                    0,
                    m_fallingVertices.data());

    sceGuFinish();
    sceGuSync(0, 0);
    sceGuSwapBuffers();
}

void SnowScene::update(float dt)
{
    if (!m_gridVertices)
    {
        // keep our main geometry in vram
        m_gridVertices = (Vertex*)getStaticVramTexture(SCR_WIDTH * 2, sizeof(Vertex), GU_PSM_T8);

        // fill the vertices for our lines first
        for (int x = 0; x < SCR_WIDTH; ++x)
        {
            m_gridVertices[2 * x + 1].colour = m_gridVertices[2 * x].colour = 0xFFFFFFFF;
            m_gridVertices[2 * x + 1].x = m_gridVertices[2 * x].x = x;
            m_gridVertices[2 * x + 1].y = m_gridVertices[2 * x].y = SCR_HEIGHT;
            m_gridVertices[2 * x + 1].z = m_gridVertices[2 * x].z = 0;
        }
    }

    m_fallingVertices.clear();

    for (std::vector<Snowflake*>::iterator iter = m_flakes.begin(); iter < m_flakes.end();)
    {
        Snowflake* flake = *iter;

        // update flake position
        flake->update(dt);

        int ix = flake->x();
        int iy = SCR_HEIGHT - flake->y();

        // check if offscreen
        if (ix < 0 || ix >= SCR_WIDTH)
        {
            // remove flake from container and delete
            iter = m_flakes.erase(iter);

            delete flake;
            continue;
        }

        // get the highest point for this column
        int h = getHighestGridPoint(ix);

        // check if we're past the highest point
        if (iy < h + 1)
        {
            int direction = 0;

            // get flake direction
            if (flake->vx() > 0.f)
            {
                direction = -1;
            }
            else
            {
                direction = 1;
            }

            if (ix + direction >= 0 && ix + direction < SCR_WIDTH)
            {
                // slope to avoid huge single mountain points
                while (h - getHighestGridPoint(ix + direction) > 0)
                {
                    ix += direction;
                    h = getHighestGridPoint(ix);

                    if (ix + direction < 0 || ix + direction >= SCR_WIDTH)
                    {
                        break;
                    }
                }
            }

            // add to grid
            m_grid[ix + (SCR_WIDTH * h)] = true;

            m_gridVertices[2 * ix].y = SCR_HEIGHT - h + 1;

            // remove flake from container and delete
            iter = m_flakes.erase(iter);

            delete flake;
        }
        else
        {
            Vertex vertex;
            vertex.colour = 0xFFFFFFFF;
            vertex.x = ix;
            vertex.y = -iy + SCR_HEIGHT;
            vertex.z = 0.f;

            // we will draw this vertex
            m_fallingVertices.push_back(vertex);

            // go to next flake
            iter++;
        }
    }

    // add some new flakes to the scene
    if (m_rng->random(m_spawnChance) == 1)
    {
        int newFlakes = m_rng->random(m_spawnMax);

        for (int i = 0; i < newFlakes; ++i)
        {
            ScePspFVector2 position;
            ScePspFVector2 velocity;

            position.x = m_rng->random(SCR_WIDTH);
            position.y = 0;

            velocity.x = m_baseVelocityX + (m_deltaVelocityX * m_rng->randomDirectionF());
            velocity.y = m_baseVelocityY + (m_deltaVelocityY * m_rng->randomDirectionF());

            m_flakes.push_back(new Snowflake(position, velocity));
        }
    }
}

int SnowScene::getHighestGridPoint(int x)
{
    for (int y = 0; y < SCR_HEIGHT; ++y)
    {
        if (!m_grid[x + (SCR_WIDTH * y)])
        {
            return y;
        }
    }

    return SCR_HEIGHT;
}
