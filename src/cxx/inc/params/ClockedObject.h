#pragma once

class ClockedObject;
#include <cstddef>
#include <vector>
#include "params/ClockDomain.h"
#include "params/PowerModel.h"
#include "params/PowerState.h"
#include "params/SimObject.h"


struct ClockedObjectParams : public SimObjectParams
{
    ClockDomain * clk_domain;
    std::vector< PowerModel * > power_model;
    PowerState * power_state;
};


