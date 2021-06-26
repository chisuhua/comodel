#include "sim/inc/core.h"

#include <iostream>
#include <string>

// #include "base/callback.hh"
#include "base/inc/cprintf.h"
#include "base/inc/logging.h"
// #include "base/output.hh"
#include "sim/inc/eventq.h"

using namespace std;

namespace SimClock {
/// The simulated frequency of curTick(). (In ticks per second)
Tick Frequency;

namespace Float {
double s;
double ms;
double us;
double ns;
double ps;

double Hz;
double kHz;
double MHz;
double GHz;
} // namespace Float

namespace Int {
Tick s;
Tick ms;
Tick us;
Tick ns;
Tick ps;
} // namespace Float

} // namespace SimClock

namespace {

bool _clockFrequencyFixed = false;

// Default to 1 THz (1 Tick == 1 ps)
Tick _ticksPerSecond = 1e12;

} // anonymous namespace

void
fixClockFrequency()
{
    if (_clockFrequencyFixed)
        return;

    using namespace SimClock;
    Frequency = _ticksPerSecond;
    Float::s = static_cast<double>(Frequency);
    Float::ms = Float::s / 1.0e3;
    Float::us = Float::s / 1.0e6;
    Float::ns = Float::s / 1.0e9;
    Float::ps = Float::s / 1.0e12;

    Float::Hz  = 1.0 / Float::s;
    Float::kHz = 1.0 / Float::ms;
    Float::MHz = 1.0 / Float::us;
    Float::GHz = 1.0 / Float::ns;

    Int::s  = Frequency;
    Int::ms = Int::s / 1000;
    Int::us = Int::ms / 1000;
    Int::ns = Int::us / 1000;
    Int::ps = Int::ns / 1000;

    cprintf("Global frequency set at %d ticks per second\n", _ticksPerSecond);

    _clockFrequencyFixed = true;
}
bool clockFrequencyFixed() { return _clockFrequencyFixed; }

void
setClockFrequency(Tick tps)
{
    panic_if(_clockFrequencyFixed,
            "Global frequency already fixed at %f ticks/s.", _ticksPerSecond);
    _ticksPerSecond = tps;
}
Tick getClockFrequency() { return _ticksPerSecond; }

void
setOutputDir(const string &dir)
{
    simout.setDirectory(dir);
}

/**
 * Queue of C++ callbacks to invoke on simulator exit.
 */
inline CallbackQueue &
exitCallbacks()
{
    static CallbackQueue theQueue;
    return theQueue;
}

/**
 * Register an exit callback.
 */
void
registerExitCallback(Callback *callback)
{
    exitCallbacks().add(callback);
}

/**
 * Do C++ simulator exit processing.  Exported to Python to be invoked
 * when simulator terminates via Python's atexit mechanism.
 */
void
doExitCleanup()
{
    exitCallbacks().process();
    exitCallbacks().clear();

    cout.flush();
}

