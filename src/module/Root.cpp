#include "base/inc/logging.h"
#include "base/inc/trace.h"
// #include "config/the_isa.hh"
// #include "debug/TimeSync.hh"
#include "sim/inc/eventq_impl.h"
// #include "sim/full_system.hh"
#include "Root.h"

Root *Root::_root = NULL;

/*
 * This function is called periodically by an event in M5 and ensures that
 * at least as much real time has passed between invocations as simulated time.
 * If not, the function either sleeps, or if the difference is small enough
 * spin waits.
 */
void
Root::timeSync()
{
    Time cur_time, diff, period = timeSyncPeriod();

    do {
        cur_time.setTimer();
        diff = cur_time - lastTime;
        Time remainder = period - diff;
        if (diff < period && remainder > _spinThreshold) {
            DPRINTF(TimeSync, "Sleeping to sync with real time.\n");
            // Sleep until the end of the period, or until a signal.
            sleep(remainder);
            // Refresh the current time.
            cur_time.setTimer();
        }
    } while (diff < period);
    lastTime = cur_time;
    schedule(&syncEvent, curTick() + _periodTick);
}

void
Root::timeSyncEnable(bool en)
{
    if (en == _enabled)
        return;
    _enabled = en;
    if (_enabled) {
        // Get event going.
        Tick periods = ((curTick() + _periodTick - 1) / _periodTick);
        Tick nextPeriod = periods * _periodTick;
        schedule(&syncEvent, nextPeriod);
    } else {
        // Stop event.
        deschedule(&syncEvent);
    }
}

/// Configure the period for time sync events.
void
Root::timeSyncPeriod(Time newPeriod)
{
    bool en = timeSyncEnabled();
    _period = newPeriod;
    _periodTick = _period.getTick();
    timeSyncEnable(en);
}

/// Set the threshold for time remaining to spin wait.
void
Root::timeSyncSpinThreshold(Time newThreshold)
{
    bool en = timeSyncEnabled();
    _spinThreshold = newThreshold;
    timeSyncEnable(en);
}

Root::Root(RootParams *p)
    : CoModuleNode(p), _enabled(false), _periodTick(p->time_sync_period),
      syncEvent([this]{ timeSync(); }, name())
{
    _period.setTick(p->time_sync_period);
    _spinThreshold.setTick(p->time_sync_spin_threshold);

    assert(_root == NULL);
    _root = this;
    lastTime.setTimer();

    simQuantum = p->sim_quantum;
}

void
Root::Startup()
{
    timeSyncEnable(params()->time_sync_enable);
}


CoModule CreateModule(ModuleParams *params) {
    return CoModule(make_object<Root>(dynamic_cast<RootParams*>(params)));
}

Port CreatePort(std::string port_name, std::string peer_name) {
    return Port(make_object<PortNode>(port_namei, peer_name));
}

ModuleParams* CreateParams(std::string json_str) {
    RootParams *params = new RootParams;
    iguana::json::from_json(params, json_str.data(), json_str.length());
    return RootParams;
}


TVM_DLL_EXPORT_TYPED_FUNC(CreateModule, CreateModule);
TVM_DLL_EXPORT_TYPED_FUNC(CreateParams, CreateParams);
TVM_DLL_EXPORT_TYPED_FUNC(CreatePort, CreatePort);
