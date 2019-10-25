#include "eventsource.h"
#include "application.h"

EventSource::EventSource(void)
{
    Application::instance()->addEventSource(this);
}
