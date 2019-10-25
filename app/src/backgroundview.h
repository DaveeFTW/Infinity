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

#ifndef BACKGROUNDVIEW_H
#define BACKGROUNDVIEW_H

#include "view.h"

class ViewManager;
class ParticleSource;
class Texture;

class BackgroundView : public View
{
public:
    BackgroundView(ViewManager* manager);
    ~BackgroundView(void);

    void setTextureLeft(Texture* texture);
    void setTextureRight(Texture* texture);

    void setModulationColour(unsigned int colour);

    const ScePspFMatrix4* loadModel(void) const;
    void saveModel(const ScePspFMatrix4* model);

    void setOpacity(float opacity) override {}

    float opacity(void) override { return 1.f; }

protected:
    void render(void) override;
    void update(float dt) override;

private:
    ViewManager* m_viewManager;
    ParticleSource* m_particles;
    Texture *m_textureLeft, *m_textureRight;
    ScePspFMatrix4 m_savedModel;
    unsigned int m_modulationColour;
};

#endif // BACKGROUNDVIEW_H
