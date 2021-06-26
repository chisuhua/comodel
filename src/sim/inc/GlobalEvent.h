#pragma once
#include <mutex>
#include <vector>

#include "base/inc/barrier.h"
#include "EventqImpl.h"

/**
 * @file sim/global_event.hh
 * Global events and related declarations.
 *
 * A global event is an event that occurs across all threads, i.e.,
 * globally.  It consists of a set of "local" (regular) Events, one
 * per thread/event queue, a barrier object, and common state.  The
 * local events are scheduled for the same tick.  The local event
 * process() method enters the barrier to wait for other threads; once
 * all threads reach that tick (and enter the associated barrier), the
 * global event is triggered and its associated activity is performed.
 *
 * There are two basic global event patterns, GlobalEvent and
 * GlobalSyncEvent.  GlobalEvent is the base class for typical global
 * events, while GlobalSyncEvent is optimized for global
 * synchronization operations.
 */

/**
 * Common base class for GlobalEvent and GlobalSyncEvent.
 */
class BaseGlobalEvent : public EventBase
{
  private:
      //! Mutex variable for providing exculsive right to schedule global
      //! events. This is necessary so that a total order can be maintained
      //! amongst the global events. Without ensuring the total order, it is
      //! possible that threads execute global events in different orders,
      //! which can result in a deadlock.
      static std::mutex globalQMutex;

  protected:

    /// The base class for the local events that will synchronize
    /// threads to perform the global event.  This class is abstract,
    /// since it derives from the abstract Event class but still does
    /// not define the required process() method.
    class BarrierEvent : public Event
    {
      protected:
        BaseGlobalEvent *_globalEvent;

        BarrierEvent(BaseGlobalEvent *global_event, Priority p, Flags f)
            : Event(p, f), _globalEvent(global_event)
        {
        }

        ~BarrierEvent();

        friend class BaseGlobalEvent;

        bool globalBarrier()
        {
            // This method will be called from the process() method in
            // the local barrier events
            // (GlobalSyncEvent::BarrierEvent).  The local event
            // queues are always locked when servicing events (calling
            // the process() method), which means that it will be
            // locked when entering this method. We need to unlock it
            // while waiting on the barrier to prevent deadlocks if
            // another thread wants to lock the event queue.
            EventQueue::ScopedRelease release(curEventQueue());
            return _globalEvent->barrier.wait();
        }

      public:
        virtual BaseGlobalEvent *globalEvent() { return _globalEvent; }
    };

    //! The barrier that all threads wait on before performing the
    //! global event.
    Barrier barrier;

    //! The individual local event instances (one per thread/event queue).
    std::vector<BarrierEvent *> barrierEvent;

  public:
    BaseGlobalEvent(Priority p, Flags f);

    virtual ~BaseGlobalEvent();

    virtual void process() = 0;

    virtual const char *description() const = 0;

    void schedule(Tick when);

    bool scheduled() const
    {
        bool sched = false;
        for (uint32_t i = 0; i < numMainEventQueues; ++i) {
            sched = sched || barrierEvent[i]->scheduled();
        }

        return sched;
    }

    Tick when() const
    {
        assert(numMainEventQueues > 0);
        return barrierEvent[0]->when();
    }

    void deschedule();
    void reschedule(Tick when);
};


/**
 * Funky intermediate class to support CRTP so that we can have a
 * common constructor to create the local events, even though the
 * types of the local events are defined in the derived classes.
 */
template <class Derived>
class BaseGlobalEventTemplate : public BaseGlobalEvent
{
  protected:
    BaseGlobalEventTemplate(Priority p, Flags f)
        : BaseGlobalEvent(p, f)
    {
        for (int i = 0; i < numMainEventQueues; ++i)
            barrierEvent[i] = new typename Derived::BarrierEvent(this, p, f);
    }
};


/**
 * The main global event class.  Ordinary global events should derive
 * from this class, and define process() to specify the action to be
 * taken when the event is reached.  All threads will synchronize at a
 * barrier, exactly one of the threads will execute the process()
 * method, then the threads will synchronize again so that none of
 * them continue until process() is complete.
 */
class GlobalEvent : public BaseGlobalEventTemplate<GlobalEvent>
{
  public:
    typedef BaseGlobalEventTemplate<GlobalEvent> Base;

    class BarrierEvent : public Base::BarrierEvent
    {
      public:
        void process();
        BarrierEvent(Base *global_event, Priority p, Flags f)
            : Base::BarrierEvent(global_event, p, f)
        { }
    };

    GlobalEvent(Priority p, Flags f)
        : Base(p, f)
    { }

    GlobalEvent(Tick when, Priority p, Flags f)
        : Base(p, f)
    {
        schedule(when);
    }

    virtual void process() = 0;
};

/**
 * A special global event that synchronizes all threads and forces
 * them to process asynchronously enqueued events.  Useful for
 * separating quanta in a quantum-based parallel simulation.
 */
class GlobalSyncEvent : public BaseGlobalEventTemplate<GlobalSyncEvent>
{
  public:
    typedef BaseGlobalEventTemplate<GlobalSyncEvent> Base;

    class BarrierEvent : public Base::BarrierEvent
    {
      public:
        void process();
        BarrierEvent(Base *global_event, Priority p, Flags f)
            : Base::BarrierEvent(global_event, p, f)
        { }
    };

    GlobalSyncEvent(Priority p, Flags f)
        : Base(p, f), repeat(0)
    { }

    GlobalSyncEvent(Tick when, Tick _repeat, Priority p, Flags f)
        : Base(p, f), repeat(_repeat)
    {
        schedule(when);
    }

    void process();

    const char *description() const;

    Tick repeat;
};


