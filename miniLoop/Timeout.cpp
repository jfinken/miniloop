#include "Loop.h"

#include <event2/event.h>

namespace loop {

Loop::Timeout::Timeout(Loop& loop)
  : loop(loop)
  , callback(nullptr)
  , event(evtimer_new(loop.event_base, &trampoline, this))
{}

Loop::Timeout::~Timeout()
{
    event_del(event);
    event_free(event);
}

void
Loop::Timeout::onTimeout(std::function<void()> callback)
{
    this->callback = callback;
}

void
Loop::Timeout::set(int after_ms)
{
    struct timeval tv;

    tv.tv_sec = after_ms / 1000;
    tv.tv_usec = after_ms % 1000;

    evtimer_add(event, &tv);
}

void
Loop::Timeout::cancel()
{
    event_del(event);
}

void
Loop::Timeout::trampoline(int /* fd */, short /* events */, void* data)
{
    if (reinterpret_cast<Loop::Timeout*>(data)->callback)
        reinterpret_cast<Loop::Timeout*>(data)->callback();
}

Loop::RecurrentTimeout::RecurrentTimeout(int period, Loop& loop)
  : period_ms(period)
  , underlyingTimeout(loop)
{
    underlyingTimeout.onTimeout([this]() {
        underlyingTimeout.set(period_ms);
        if (callback)
            callback();
    });
    underlyingTimeout.set(period);
}
Loop::RecurrentTimeout::~RecurrentTimeout() {}

void
Loop::RecurrentTimeout::onTimeout(std::function<void()> callback)
{
    this->callback = callback;
}

}
