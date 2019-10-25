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

#ifndef STATE_H
#define STATE_H

#include "transition.h"
#include <memory>

class ConfigurationMenu;

class State;
using StatePtr = std::shared_ptr<State>;

#include <vector>
using TransitionList = std::vector<Transition*>;

class State
{
public:
    State(void);
    virtual ~State(void);

    void addTransition(Transition* transition, State* state);
    const TransitionList& transitions(void) const;

    virtual void begin(State* parent) = 0;
    virtual void end(void) = 0;

private:
    TransitionList m_transitions;
};

#endif // STATE_H
