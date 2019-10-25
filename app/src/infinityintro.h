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

#ifndef INFINITYINTRO_H
#define INFINITYINTRO_H

#include "state.h"
#include "vertextype.h"
#include "view.h"

#include "flashingrectangle.h"

class FontManager;
class TailedParticle;
class SimpleText;
class ViewManager;

class InfinityScreen
    : public State
    , public View
{
public:
    InfinityScreen(ViewManager* manager);

    void init(void);
    void setOpacity(float opacity) override;
    float opacity(void) override;

protected:
    void update(float dt) override;
    void render(void) override;

    void begin(State* parent) override;
    void end(void) override;

private:
    double m_t;
    FontManager* m_font;
    Vertex* m_vertices;
    TailedParticle* m_particles;
    SimpleText* m_text;
    // FlashingRectangle m_flashingRect;
    ViewManager* m_viewManager;
    float m_opacity;
};

#endif // INFINITYINTRO_H
