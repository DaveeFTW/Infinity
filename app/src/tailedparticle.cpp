#include "tailedparticle.h"
#include "blockallocator.h"
#include "graphicsdevice.h"
#include "particlesource.h"
#include "texture.h"

#include <pspgu.h>
#include <pspgum.h>

#include <cstring>

namespace
{
    template<class T>
    void copyCircularBuffer(void* dst, const CircularBuffer<T>& buffer)
    {
        std::memcpy(dst, buffer.array_one().first, buffer.array_one().second * sizeof(T));
        std::memcpy((char*)dst + buffer.array_one().second * sizeof(T),
                    buffer.array_two().first,
                    buffer.array_two().second * sizeof(T));
    }
}

TailedParticle::TailedParticle(GraphicsDevice* device, int length)
    : m_graphicsDevice(device)
    , m_particleN(length)
    , m_particleSize(10.f)
    , m_vertices2(2 * m_particleN)
    , m_particleSource(new ParticleSource(0, 0, 750))
    , m_opacity(1.f)
{
    m_particleSource->setRepeat(true);

    m_verticesVRAM = (TextureVertex*)m_graphicsDevice->vramAllocator()->allocate_aligned(
        2 * m_particleN * sizeof(TextureVertex), 16);

    // set default parameters for the vertex
    TextureVertex vertex;

    vertex.colour = 0;
    vertex.u = vertex.v = 0.0f;
    vertex.x = vertex.y = -1.f;
    vertex.z = 0.f;

    for (int i = 0; i < m_particleN; ++i)
    {
        m_vertices2[(2 * i) + 1].u = m_vertices2[(2 * i) + 1].v = 127.f;
    }

    // load texture
    m_texture =
        (unsigned char*)m_graphicsDevice->vramAllocator()->allocate_aligned(128 * 128 * 4, 16);

    for (int i = 0; i < 128 * 128; ++i)
    {
        unsigned char color_cap = 0;

        if (g_infinityTex[i] > 0x90 && g_infinityTex[i] < 0xB0)
        {
            color_cap = 0x90;
        }

        m_texture[4 * i] = m_texture[4 * i + 1] = m_texture[4 * i + 2] =
            g_infinityTex[i] - color_cap;
        m_texture[4 * i + 3] = g_infinityTex[i];
    }
}

void TailedParticle::moveTo(float x, float y)
{
    TextureVertex vertex;

    vertex.colour = 0;
    vertex.u = vertex.v = 128.0f;
    vertex.x = x + m_particleSize / 2.0f;
    vertex.y = y + m_particleSize / 2.0f;
    vertex.z = 0.f;

    m_vertices2.push_front(vertex);

    vertex.u = vertex.v = 0.0f;
    vertex.x = x - m_particleSize / 2.0f;
    vertex.y = y - m_particleSize / 2.0f;
    vertex.z = 0.f;

    m_vertices2.push_front(vertex);

    // update colours and size
    for (int i = 0; i < m_particleN; ++i)
    {
        unsigned char alpha =
            (215.f * (float)(m_particleN - i) / (float)m_particleN + 40.f) * m_opacity;

        m_vertices2[2 * i].colour = 0x00FFFFFF | ((int)(alpha * m_opacity) << 24);
        m_vertices2[(2 * i) + 1].colour = 0x00FFFFFF | ((int)(alpha * m_opacity) << 24);
    }

    m_particleSource->setSpawnArea(Rectangle(
        x - m_particleSize / 2.0f, y - m_particleSize / 2.0f, m_particleSize, m_particleSize));
}

void TailedParticle::update(float dt)
{
    m_particleSource->update(dt);
}

void TailedParticle::draw(void)
{
    // copy vertices
    copyCircularBuffer(m_verticesVRAM, m_vertices2);

    sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
    sceGuTexFunc(GU_TFX_ADD, GU_TCC_RGBA);
    sceGuTexImage(0, 128, 128, 128, m_texture);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexScale(1.0f / 128.f, 1.0f / 128.f);
    sceGuTexOffset(0.0f, 0.0f);
    sceGuAmbientColor(0xffffffff);

    sceKernelDcacheWritebackAll();
    sceGuTexFlush();
    sceGuTexSync();

    sceGuEnable(GU_TEXTURE_2D);
    sceGumDrawArray(GU_SPRITES,
                    GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
                    2 * m_particleN,
                    0,
                    m_verticesVRAM);
    sceGuDisable(GU_TEXTURE_2D);

    m_particleSource->render();
}

void TailedParticle::setOpacity(float opacity)
{
    m_opacity = opacity;
}
