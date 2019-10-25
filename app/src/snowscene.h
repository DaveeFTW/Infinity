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

#ifndef SNOWSCENE_H
#define SNOWSCENE_H

#include "scene.h"
#include "vertextype.h"

#include <vector>

class Snowflake;
class RandomGenerator;

class SnowScene : public Scene
{
public:
    SnowScene();

    void init(void);
    void update(float dt);
    void render(void);

    void setSpawnChance(unsigned int chance) { m_spawnChance = chance; }
    unsigned int spawnChance(void) { return m_spawnChance; }

    void setMaxSpawn(unsigned int max) { m_spawnMax = max; }
    unsigned int maxSpawn(void) { return m_spawnMax; }

    void setBaseHorizontalVelocity(float velocity) { m_baseVelocityX = velocity; }
    float baseHorizontalVelocity(void) { return m_baseVelocityX; }

    void setMaxHorizontalVelocityDelta(float dv) { m_deltaVelocityX = dv; }
    float maxHorizontalVelocityDelta(void) { return m_deltaVelocityX; }

    void setBaseVerticalVelocity(float velocity) { m_baseVelocityY = velocity; }
    float baseVerticalVelocity(void) { return m_baseVelocityY; }

    void setMaxVerticalVelocityDelta(float dv) { m_deltaVelocityY = dv; }
    float maxVerticalVelocityDelta(void) { return m_deltaVelocityY; }

private:
    int getHighestGridPoint(int x);

private:
    std::vector<bool> m_grid;
    std::vector<Snowflake*> m_flakes;
    std::vector<Vertex> m_fallingVertices;
    Vertex* m_gridVertices;
    RandomGenerator* m_rng;

    unsigned int m_spawnChance;
    unsigned int m_spawnMax;

    float m_baseVelocityX;
    float m_deltaVelocityX;

    float m_baseVelocityY;
    float m_deltaVelocityY;
};

#endif // SNOWSCENE_H
