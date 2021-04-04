#pragma once
/**
 * @file
 * ClockedObject declaration and implementation.
 */

#include "params/ClockedObject.h"
#include "Core.hh"
#include "ClockDomain.h"
#include "PowerState.h"
#include "SimObject.h"

/**
 * Helper class for objects that need to be clocked. Clocked objects
 * typically inherit from this class. Objects that need SimObject
 * functionality as well should inherit from ClockedObject.
 */
class Clocked
{

  private:
    // the tick value of the next clock edge (>= curTick()) at the
    // time of the last call to update()
    mutable Tick tick;

    // The cycle counter value corresponding to the current value of
    // 'tick'
    mutable Cycles cycle;

    /**
     *  Align cycle and tick to the next clock edge if not already done. When
     *  complete, tick must be at least curTick().
     */
    void
    update() const
    {
        // both tick and cycle are up-to-date and we are done, note
        // that the >= is important as it captures cases where tick
        // has already passed curTick()
        if (tick >= curTick())
            return;

        // optimise for the common case and see if the tick should be
        // advanced by a single clock period
        tick += clockPeriod();
        ++cycle;

        // see if we are done at this point
        if (tick >= curTick())
            return;

        // if not, we have to recalculate the cycle and tick, we
        // perform the calculations in terms of relative cycles to
        // allow changes to the clock period in the future
        Cycles elapsedCycles(divCeil(curTick() - tick, clockPeriod()));
        cycle += elapsedCycles;
        tick += elapsedCycles * clockPeriod();
    }

    /**
     * The clock domain this clocked object belongs to
     */
    ClockDomain &clockDomain;

  protected:

    /**
     * Create a clocked object and set the clock domain based on the
     * parameters.
     */
    Clocked(ClockDomain &clk_domain)
        : tick(0), cycle(0), clockDomain(clk_domain)
    {
        // Register with the clock domain, so that if the clock domain
        // frequency changes, we can update this object's tick.
        clockDomain.registerWithClockDomain(this);
    }

    Clocked(Clocked &) = delete;
    Clocked &operator=(Clocked &) = delete;

    /**
     * Virtual destructor due to inheritance.
     */
    virtual ~Clocked() { }

    /**
     * Reset the object's clock using the current global tick value. Likely
     * to be used only when the global clock is reset. Currently, this done
     * only when Ruby is done warming up the memory system.
     */
    void
    resetClock() const
    {
        Cycles elapsedCycles(divCeil(curTick(), clockPeriod()));
        cycle = elapsedCycles;
        tick = elapsedCycles * clockPeriod();
    }

    /**
     * A hook subclasses can implement so they can do any extra work that's
     * needed when the clock rate is changed.
     */
    virtual void clockPeriodUpdated() {}

  public:

    /**
     * Update the tick to the current tick.
     */
    void
    updateClockPeriod()
    {
        update();
        clockPeriodUpdated();
    }

    /**
     * Determine the tick when a cycle begins, by default the current one, but
     * the argument also enables the caller to determine a future cycle. When
     * curTick() is on a clock edge, the number of cycles in the parameter is
     * added to curTick() to be returned. When curTick() is not aligned to a
     * clock edge, the number of cycles in the parameter is added to the next
     * clock edge.
     *
     * @param cycles The number of cycles into the future
     *
     * @return The start tick when the requested clock edge occurs. Precisely,
     * this tick can be
     *     curTick() + [0, clockPeriod()) + clockPeriod() * cycles
     */
    Tick
    clockEdge(Cycles cycles=Cycles(0)) const
    {
        // align tick to the next clock edge
        update();

        // figure out when this future cycle is
        return tick + clockPeriod() * cycles;
    }

    /**
     * Determine the current cycle, corresponding to a tick aligned to
     * a clock edge.
     *
     * @return When curTick() is on a clock edge, return the Cycle corresponding
     * to that clock edge. When curTick() is not on a clock edge, return the
     * Cycle corresponding to the next clock edge.
     */
    Cycles
    curCycle() const
    {
        // align cycle to the next clock edge.
        update();

        return cycle;
    }

    /**
     * Based on the clock of the object, determine the start tick of the first
     * cycle that is at least one cycle in the future. When curTick() is at the
     * current cycle edge, this returns the next clock edge. When calling this
     * during the middle of a cycle, this returns 2 clock edges in the future.
     *
     * @return The start tick of the first cycle that is at least one cycle in
     * the future. Precisely, the returned tick can be in the range
     *     curTick() + [clockPeriod(), 2 * clockPeriod())
     */
    Tick nextCycle() const { return clockEdge(Cycles(1)); }

    uint64_t frequency() const { return SimClock::Frequency / clockPeriod(); }

    Tick clockPeriod() const { return clockDomain.clockPeriod(); }

    double voltage() const { return clockDomain.voltage(); }

    Cycles
    ticksToCycles(Tick t) const
    {
        return Cycles(divCeil(t, clockPeriod()));
    }

    Tick cyclesToTicks(Cycles c) const { return clockPeriod() * c; }
};

/**
 * The ClockedObject class extends the SimObject with a clock and
 * accessor functions to relate ticks to the cycles of the object.
 */
class ClockedObject : public SimObject, public Clocked
{
  public:
    ClockedObject(const ClockedObjectParams *p);

    /** Parameters of ClockedObject */
    typedef ClockedObjectParams Params;
    const Params *
    params() const
    {
        return reinterpret_cast<const Params*>(_params);
    }

    // void serialize(CheckpointOut &cp) const override;
    // void unserialize(CheckpointIn &cp) override;

    PowerState *powerState;
};

