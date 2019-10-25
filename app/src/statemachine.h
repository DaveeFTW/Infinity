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

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "state.h"
#include <vector>

class Event;

class StateMachine : public State
{
public:
    StateMachine(void);

    void setInitialState(StatePtr state);
    void start(void);
    void addState(StatePtr state);

protected:
    void begin(State* parent) override;
    void end(void) override;

private:
    void processEvent(Event* event);

private:
    using StateList = std::vector<StatePtr>;

    StateList m_states;
    bool m_running;
    State* m_state;
    int m_animationsRunning;
};

#endif // STATEMACHINE_H
