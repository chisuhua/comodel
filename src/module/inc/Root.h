/**
 * @file This file defines the Root simobject and the methods used to control
 * the time syncing mechanism provided through it.
 *
 * Time syncing prevents simulated time from passing faster than real time. It
 * works by scheduling a periodic event that checks to see if its simulated
 * period is shorter than its real period. If it is, it stalls the simulation
 * until they're equal.
 */
#pragma once

#include "base/inc/time.h"
#include "sim/inc/eventq.h"
#include <iguana/json.hpp>
#include "Module.h"

struct RootParams : public ModuleParams {
    Tick sim_quantum;
    bool time_sync_enable;
    Tick time_sync_period;
    Tick time_sync_spin_threshold;
}
REFLECTION(sim_quantum, time_sync_enable, time_sync_period, time_sync_spin_threshold);

class Root : public CoModuleNode
{
  private:
    static Root *_root;

  protected:
    bool _enabled;
    Time _period;
    Tick _periodTick;
    Time _spinThreshold;

    Time lastTime;

    void timeSync();
    EventFunctionWrapper syncEvent;

  public:
    /**
     * Use this function to get a pointer to the single Root object in the
     * simulation. This function asserts that such an object has actual been
     * constructed to avoid having to perform that check everywhere the root
     * is used. This is to allow calling the functions below.
     *
     * @return Pointer to the single root object.
     */
    static Root *
    root()
    {
        assert(_root);
        return _root;
    }

  public:

    /// Check whether time syncing is enabled.
    bool timeSyncEnabled() const { return _enabled; }
    /// Retrieve the period for the sync event.
    const Time timeSyncPeriod() const { return _period; }
    /// Retrieve the threshold for time remaining to spin wait.
    const Time timeSyncSpinThreshold() const { return _spinThreshold; }

    /// Enable or disable time syncing.
    void timeSyncEnable(bool en);
    /// Configure the period for time sync events.
    void timeSyncPeriod(Time newPeriod);
    /// Set the threshold for time remaining to spin wait.
    void timeSyncSpinThreshold(Time newThreshold);

    typedef RootParams Params;
    const Params *
    params() const
    {
        return dynamic_cast<const Params *>(_params);
    }

    Root(Params *p);

    /** Schedule the timesync event at startup().
     */
    void startup() override;
};

