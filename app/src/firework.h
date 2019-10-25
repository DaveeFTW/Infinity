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

#ifndef FIREWORK_H
#define FIREWORK_H

#include "rectangle.h"
#include <psputils.h>
#include <vector>

class Firework
{
public:
    Firework(float x, float y, unsigned int colour, int max = 500);
    Firework(Rectangle spawnArea, unsigned int colour, int max = 500);

    void setSpawnArea(float x, float y);
    void setSpawnArea(Rectangle spawnArea);

    void setDecayCoefficient(float decay);
    float decayCoefficient(void) const;

    void setVelocityCoefficient(float velocity);
    float velocityCoefficient(void) const;

    void setRepeat(bool repeat);
    bool repeat(void) const;

    void setOpacity(float opacity);
    float opacity(void) const;

    bool active(void) const;

    void update(float dt);
    void render(void);

private:
    typedef struct
    {
        float decay;
        float age;
        float lifetime;
        float x;
        float y;
        ScePspFVector2 direction;
    } Particle;

private:
    SceKernelUtilsMt19937Context m_ctx;
    Rectangle m_spawnArea;
    float m_decayCoefficient;
    float m_velocityCoefficient;
    bool m_repeat;
    float m_opacity;
    unsigned int m_colour;
    std::vector<Particle> m_particles;

private:
    void regenerateParticles(void);
};

#endif // PARTICLESOURCE_H
