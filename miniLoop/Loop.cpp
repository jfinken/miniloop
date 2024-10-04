#include <event2/event.h>
#include <event2/thread.h>

#include "Loop.h"

static loop::Loop singleton;

namespace loop {

Loop&
Loop::singleton()
{
    return ::singleton;
}

void
Loop::usePthreads()
{
    evthread_use_pthreads();
}

Loop::Loop()
  : event_base(event_base_new())
{}

Loop::~Loop()
{
    event_base_free(event_base);
}

void
Loop::run()
{
    // Event dispatching loop in event.h
    event_base_dispatch(event_base);
}

void
Loop::exit()
{
    event_base_loopbreak(event_base);
}

} // End of: namespace loop
