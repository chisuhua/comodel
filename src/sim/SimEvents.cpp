#include "SimEvents.h"

#include <string>

// #include "base/callback.hh"
#include "base/inc/hostinfo.h"
#include "EventqImpl.h"
// #include "sim/sim_exit.hh"
// #include "sim/stats.hh"

using namespace std;

GlobalSimLoopExitEvent::GlobalSimLoopExitEvent(Tick when,
                                               const std::string &_cause,
                                               int c, Tick r)
    : GlobalEvent(when, Sim_Exit_Pri, IsExitEvent),
      cause(_cause), code(c), repeat(r)
{
}

GlobalSimLoopExitEvent::GlobalSimLoopExitEvent(const std::string &_cause,
                                               int c, Tick r)
    : GlobalEvent(curTick(), Minimum_Pri, IsExitEvent),
      cause(_cause), code(c), repeat(r)
{
}

const char *
GlobalSimLoopExitEvent::description() const
{
    return "global simulation loop exit";
}

//
// handle termination event
//
void
GlobalSimLoopExitEvent::process()
{
    if (repeat) {
        schedule(curTick() + repeat);
    }
}

void
exitSimLoop(const std::string &message, int exit_code, Tick when, Tick repeat,
            bool serialize)
{
    warn_if(serialize && (when != curTick() || repeat),
            "exitSimLoop called with a delay and auto serialization. This is "
            "currently unsupported.");

    new GlobalSimLoopExitEvent(when + simQuantum, message, exit_code, repeat);
}

void
exitSimLoopNow(const std::string &message, int exit_code, Tick repeat,
               bool serialize)
{
    new GlobalSimLoopExitEvent(message, exit_code, repeat);
}

LocalSimLoopExitEvent::LocalSimLoopExitEvent(const std::string &_cause, int c,
                                   Tick r)
    : Event(Sim_Exit_Pri, IsExitEvent),
      cause(_cause), code(c), repeat(r)
{
}

//
// handle termination event
//
void
LocalSimLoopExitEvent::process()
{
    exitSimLoop(cause, 0);
}


const char *
LocalSimLoopExitEvent::description() const
{
    return "simulation loop exit";
}

//
// constructor: automatically schedules at specified time
//
CountedExitEvent::CountedExitEvent(const std::string &_cause, int &counter)
    : Event(Sim_Exit_Pri), cause(_cause), downCounter(counter)
{
    // catch stupid mistakes
    assert(downCounter > 0);
}


//
// handle termination event
//
void
CountedExitEvent::process()
{
    if (--downCounter == 0) {
        exitSimLoop(cause, 0);
    }
}


const char *
CountedExitEvent::description() const
{
    return "counted exit";
}
