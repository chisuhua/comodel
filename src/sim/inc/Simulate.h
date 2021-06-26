#include "base/inc/types.h"

class GlobalSimLoopExitEvent;

GlobalSimLoopExitEvent *simulate(Tick num_cycles = MaxTick);
extern GlobalSimLoopExitEvent *simulate_limit_event;
