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

#include "particlesource.h"
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

        float rnd = sceKernelUtilsMt19937UInt(ctx) / (double)0xFFFFFFFF;
        return p + rnd * length;
    }
} // anonymous namespace

ParticleSource::ParticleSource(float x, float y, int max)
    : ParticleSource(Rectangle(x, y, 0, 0), max)
{
}

ParticleSource::ParticleSource(Rectangle spawnArea, int max)
    : m_spawnArea(spawnArea)
    , m_decayCoefficient(2.5f)
    , m_velocityCoefficient(50.f)
    , m_repeat(true)
    , m_opacity(1.f)
{
    sceKernelUtilsMt19937Init(&m_ctx, time(NULL));

    m_particles.reserve(max);
    m_particles.resize(max);
    regenerateParticles();
}

void ParticleSource::setSpawnArea(float x, float y)
{
    m_spawnArea = Rectangle(x, y, 0, 0);
}

void ParticleSource::setSpawnArea(Rectangle spawnArea)
{
    m_spawnArea = spawnArea;
}

void ParticleSource::setDecayCoefficient(float decay)
{
    m_decayCoefficient = decay;
    regenerateParticles();
}

float ParticleSource::decayCoefficient(void) const
{
    return m_decayCoefficient;
}

void ParticleSource::setVelocityCoefficient(float velocity)
{
    m_velocityCoefficient = velocity;
    regenerateParticles();
}

float ParticleSource::velocityCoefficient(void) const
{
    return m_velocityCoefficient;
}

void ParticleSource::setRepeat(bool repeat)
{
    m_repeat = repeat;
}

bool ParticleSource::repeat(void) const
{
    return m_repeat;
}

void ParticleSource::setOpacity(float opacity)
{
    m_opacity = opacity;
}

float ParticleSource::opacity(void) const
{
    return m_opacity;
}

void ParticleSource::update(float dt)
{
    for (auto i = (unsigned)0; i < m_particles.size(); ++i)
    {
        if (m_particles[i].decay <= 0.f && m_repeat)
        {
            m_particles[i].decay = 1.f;
            m_particles[i].x =
                getRandomLinearPosition(&m_ctx, m_spawnArea.x(), m_spawnArea.width());
            m_particles[i].y =
                getRandomLinearPosition(&m_ctx, m_spawnArea.y(), m_spawnArea.height());
        }
        else
        {
            m_particles[i].decay -= m_particles[i].decayRate * dt;
            m_particles[i].x += m_particles[i].direction.x * dt;
            m_particles[i].y += m_particles[i].direction.y * dt;
        }
    }
}

void ParticleSource::render(void)
{
    auto particles = static_cast<Vertex*>(sceGuGetMemory(m_particles.size() * sizeof(Vertex)));

    for (auto i = 0u; i < m_particles.size(); ++i)
    {
        int particleAlpha = (m_particles[i].decay * 0xFF);

        if (particleAlpha < 0)
            particleAlpha = 0;
        else if (particleAlpha > 0xFF)
            particleAlpha = 0xFF;

        particles[i].colour = 0x00FFFFFF | ((int)(particleAlpha * m_opacity) << 24);
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

void ParticleSource::regenerateParticles(void)
{
    for (auto i = (unsigned)0; i < m_particles.size(); ++i)
    {
        m_particles[i].decay = (float)i / (float)m_particles.size();

        // if the spawn area has a width, place randomly along this line
        m_particles[i].x = getRandomLinearPosition(&m_ctx, m_spawnArea.x(), m_spawnArea.width());

        // likewise place randomly along height line
        m_particles[i].y = getRandomLinearPosition(&m_ctx, m_spawnArea.y(), m_spawnArea.height());

        float rnd = sceKernelUtilsMt19937UInt(&m_ctx) / (double)0xFFFFFFFF;
        m_particles[i].decayRate = rnd / decayCoefficient() + 0.000001f;

        float vx = sceKernelUtilsMt19937UInt(&m_ctx) / (double)0xFFFFFFFF;
        float vy = sceKernelUtilsMt19937UInt(&m_ctx) / (double)0xFFFFFFFF;

        m_particles[i].direction.x = ((2.f * vx) - 1.f) * velocityCoefficient();
        m_particles[i].direction.y = ((2.f * vy) - 1.f) * velocityCoefficient();
    }
}
