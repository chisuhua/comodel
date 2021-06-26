#include "base/inc/trace.h"
#include "sim/inc/eventq.h"

inline void
EventQueue::schedule(Event *event, Tick when, bool global)
{
    assert(when >= getCurTick());
    assert(!event->scheduled());
    assert(event->initialized());

    event->setWhen(when, this);

    // The check below is to make sure of two things
    // a. a thread schedules local events on other queues through the asyncq
    // b. a thread schedules global events on the asyncq, whether or not
    //    this event belongs to this eventq. This is required to maintain
    //    a total order amongst the global events. See global_event.{cc,hh}
    //    for more explanation.
    if (inParallelMode && (this != curEventQueue() || global)) {
        asyncInsert(event);
    } else {
        insert(event);
    }
    event->flags.set(Event::Scheduled);
    event->acquire();

    if (DTRACE(Event))
        event->trace("scheduled");
}

inline void
EventQueue::deschedule(Event *event)
{
    assert(event->scheduled());
    assert(event->initialized());
    assert(!inParallelMode || this == curEventQueue());

    remove(event);

    event->flags.clear(Event::Squashed);
    event->flags.clear(Event::Scheduled);

    if (DTRACE(Event))
        event->trace("descheduled");

    event->release();
}

inline void
EventQueue::reschedule(Event *event, Tick when, bool always)
{
    assert(when >= getCurTick());
    assert(always || event->scheduled());
    assert(event->initialized());
    assert(!inParallelMode || this == curEventQueue());

    if (event->scheduled()) {
        remove(event);
    } else {
        event->acquire();
    }

    event->setWhen(when, this);
    insert(event);
    event->flags.clear(Event::Squashed);
    event->flags.set(Event::Scheduled);

    if (DTRACE(Event))
        event->trace("rescheduled");
}


