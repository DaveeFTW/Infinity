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

#include "statemachine.h"
#include "animation.h"
#include "application.h"
#include "event.h"

#include <algorithm>

#include <stdio.h>

StateMachine::StateMachine(void)
    : m_running(false)
    , m_state(nullptr)
    , m_animationsRunning(0)
{
    Application::instance()->addEventHandler([this](Event* event) { processEvent(event); });
}

void StateMachine::setInitialState(StatePtr state)
{
    if (m_state != nullptr)
        return;

    // only add if the state belongs to us
    if (std::find(m_states.begin(), m_states.end(), state) != m_states.end())
    {
        m_state = state.get();
    }
}

void StateMachine::processEvent(Event* event)
{
    // ignore events when we have animation
    if (m_animationsRunning)
        return;

    auto transitions = m_state->transitions();

    auto it = std::find_if(
        transitions.begin(), transitions.end(), [event](auto t) { return t->onEvent(event); });

    if (it == transitions.end())
        return;

    // begin state transition
    auto nextState = (*it)->target();
    nextState->begin(m_state);

    // perform any animations
    auto animations = (*it)->animations();

    for (auto i = animations.begin(); i != animations.end(); ++i)
    {
        m_animationsRunning++;

        (*i)->onComplete([this, nextState]() {
            m_animationsRunning--;

            if (m_animationsRunning == 0)
            {
                m_state->end();
                m_state = nextState;
            }

            return true;
        });

        (*i)->start();
    }

    if (m_animationsRunning == 0)
    {
        m_state->end();
        m_state = nextState;
    }
}

void StateMachine::start(void)
{
    if (m_state == nullptr)
        return;

    m_state->begin(nullptr);
    m_running = true;
}

void StateMachine::begin(State* parent) {}

void StateMachine::end(void) {}

void StateMachine::addState(StatePtr state)
{
    m_states.push_back(state);
}
