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

#include "firework.h"
#include "vertextype.h"

#include <pspgu.h>
#include <pspgum.h>

#include <limits.h>
#include <time.h>

namespace
{
    float getRandomLinearPosition(auto ctx, float p, float length)
    {
        if (length == 0.f)
            return p;

        double rnd = sceKernelUtilsMt19937UInt(ctx) / (double)0xFFFFFFFF;
        return p + rnd * length;
    }
} // anonymous namespace

Firework::Firework(float x, float y, unsigned int colour, int max)
    : Firework(Rectangle(x, y, 0, 0), colour, max)
{
}

Firework::Firework(Rectangle spawnArea, unsigned int colour, int max)
    : m_spawnArea(spawnArea)
    , m_decayCoefficient(2.5f)
    , m_velocityCoefficient(50.f)
    , m_repeat(true)
    , m_opacity(1.f)
    , m_colour(colour & 0xFFFFFF)
{
    sceKernelUtilsMt19937Init(&m_ctx, time(NULL));

    m_particles.reserve(max);
    m_particles.resize(max);
    regenerateParticles();
}

void Firework::setSpawnArea(float x, float y)
{
    m_spawnArea = Rectangle(x, y, 0, 0);
}

void Firework::setSpawnArea(Rectangle spawnArea)
{
    m_spawnArea = spawnArea;
}

void Firework::setDecayCoefficient(float decay)
{
    m_decayCoefficient = decay;
    regenerateParticles();
}

float Firework::decayCoefficient(void) const
{
    return m_decayCoefficient;
}

void Firework::setVelocityCoefficient(float velocity)
{
    m_velocityCoefficient = velocity;
    regenerateParticles();
}

float Firework::velocityCoefficient(void) const
{
    return m_velocityCoefficient;
}

void Firework::setRepeat(bool repeat)
{
    m_repeat = repeat;
}

bool Firework::repeat(void) const
{
    return m_repeat;
}

void Firework::setOpacity(float opacity)
{
    m_opacity = opacity;
}

float Firework::opacity(void) const
{
    return m_opacity;
}

bool Firework::active(void) const
{
    for (auto i = (unsigned)0; i < m_particles.size(); ++i)
    {
        if (m_particles[i].age < m_particles[i].lifetime)
        {
            if (m_particles[i].x > 480 || m_particles[i].x < 0)
                continue;

            if (m_particles[i].y > 272 || m_particles[i].y < 0)
                continue;

            return true;
        }
    }

    return false;
}

void Firework::update(float dt)
{
    if (!active() && m_repeat)
    {
        regenerateParticles();
    }

    for (auto i = (unsigned)0; i < m_particles.size(); ++i)
    {
        if (m_particles[i].age < m_particles[i].lifetime)
        {
            auto easing = [](float t, float b, float c, float d) {
                t /= d;
                return -c * t * (t - 2) + b;
            };

            m_particles[i].decay =
                1.f - easing(m_particles[i].age, 0.f, 1.f, m_particles[i].lifetime);
            m_particles[i].x += m_particles[i].direction.x * dt;
            m_particles[i].y += m_particles[i].direction.y * dt;
            m_particles[i].age += dt;
        }
    }
}

void Firework::render(void)
{
    auto particles = static_cast<Vertex*>(sceGuGetMemory(m_particles.size() * sizeof(Vertex)));

    for (auto i = 0u; i < m_particles.size(); ++i)
    {
        int particleAlpha = (m_particles[i].decay * 0xFF);

        if (particleAlpha < 0)
            particleAlpha = 0;
        else if (particleAlpha > 0xFF)
            particleAlpha = 0xFF;

        particles[i].colour = m_colour | ((int)(particleAlpha * m_opacity) << 24);
        particles[i].x = m_particles[i].x;
        particles[i].y = m_particles[i].y;
        particles[i].z = 0.f;
    }

    sceGumDrawArray(GU_POINTS,
                    GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                    m_particles.size(),
                    0,
                    particles);
}

void Firework::regenerateParticles(void)
{
    for (auto i = (unsigned)0; i < m_particles.size(); ++i)
    {
        m_particles[i].age = 0.f;

        // if the spawn area has a width, place randomly along this line
        m_particles[i].x = getRandomLinearPosition(&m_ctx, m_spawnArea.x(), m_spawnArea.width());

        // likewise place randomly along height line
        m_particles[i].y = getRandomLinearPosition(&m_ctx, m_spawnArea.y(), m_spawnArea.height());

        auto rnd = (double)sceKernelUtilsMt19937UInt(&m_ctx) / (double)0xFFFFFFFF;
        m_particles[i].lifetime = ((rnd * 2.f) + 1.f) * decayCoefficient();

        auto vx = (double)sceKernelUtilsMt19937UInt(&m_ctx) / (double)0xFFFFFFFF;
        auto vy = (double)sceKernelUtilsMt19937UInt(&m_ctx) / (double)0xFFFFFFFF;

        m_particles[i].direction.x = ((2.f * vx) - 1.f) * velocityCoefficient();
        m_particles[i].direction.y = ((2.f * vy) - 1.f) * velocityCoefficient();
    }
}
