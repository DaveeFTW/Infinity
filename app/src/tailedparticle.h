
#pragma once

#include "circularbuffer.h"
#include "vertextype.h"
#include <vector>

class GraphicsDevice;
class ParticleSource;

class TailedParticle
{
public:
    TailedParticle(GraphicsDevice* device, int length);
    void moveTo(float x, float y);
    void update(float dt);
    void draw(void);
    void setOpacity(float opacity);

private:
    GraphicsDevice* m_graphicsDevice;
    int m_particleN;
    float m_particleSize;
    std::vector<TextureVertex> m_vertices;
    TextureVertex* m_verticesVRAM;
    CircularBuffer<TextureVertex> m_vertices2;
    unsigned char* m_texture;
    ParticleSource* m_particleSource;
    float m_opacity;
};
