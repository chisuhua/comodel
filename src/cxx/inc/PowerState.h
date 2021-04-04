#pragma once
#include <set>

#include "callback.h"
#include "statistics.h"
#include "enums/PwrState.h"
#include "params/PowerState.h"
#include "Core.h"
#include "SimObject.h"

class PowerDomain;

/**
 * Helper class for objects that have power states. This class provides the
 * basic functionality to change between power states.
 */
class PowerState : public SimObject
{
  public:
    PowerState(const PowerStateParams *p);

    /** Parameters of PowerState object */
    typedef PowerStateParams Params;
    const Params* params() const
    {
        return reinterpret_cast<const Params*>(_params);
    }

    virtual void addFollower(PowerState* pwr_obj) {};
    void setControlledDomain(PowerDomain* pwr_dom);

    void serialize(CheckpointOut &cp) const override;
    void unserialize(CheckpointIn &cp) override;

    /**
     * Change the power state of this object to the power state p
     */
    void set(Enums::PwrState p);


    inline Enums::PwrState get() const
    {
        return _currState;
    }

    inline std::string getName() const
    {
        return Enums::PwrStateStrings[_currState];
    }

    /** Returns the percentage residency for each power state */
    std::vector<double> getWeights() const;

    /**
     * Record stats values like state residency by computing the time
     * difference from previous update. Also, updates the previous evaluation
     * tick once all stats are recorded.
     * Usually called on power state change and stats dump callback.
     */
    void computeStats();

    /**
     * Change the power state of this object to a power state equal to OR more
     * performant than p. Returns the power state the object actually went to.
     */
    Enums::PwrState matchPwrState(Enums::PwrState p);

    /**
     * Return the power states this object can be in
     */
    std::set<Enums::PwrState> getPossibleStates() const
    {
        return possibleStates;
    }

  protected:

    /** To keep track of the current power state */
    Enums::PwrState _currState;

    /** The possible power states this object can be in */
    std::set<Enums::PwrState> possibleStates;

    /** Last tick the power stats were calculated */
    Tick prvEvalTick = 0;

    /**
     * The power domain that this power state leads, nullptr if it
     * doesn't lead any.
     */
    PowerDomain* controlledDomain = nullptr;

    struct PowerStateStats : public Stats::Group
    {
        PowerStateStats(PowerState &ps);

        void regStats() override;
        void preDumpStats() override;

        PowerState &powerState;

        Stats::Scalar numTransitions;
        Stats::Scalar numPwrMatchStateTransitions;
        Stats::Distribution ticksClkGated;
        /** Tracks the time spent in each of the power states */
        Stats::Vector pwrStateResidencyTicks;
    } stats;
};

class PowerStateDumpCallback : public Callback
{
    PowerState *co;
  public:
    PowerStateDumpCallback(PowerState *co_t) : co(co_t) {}
    virtual void process() { co->computeStats(); };
};

