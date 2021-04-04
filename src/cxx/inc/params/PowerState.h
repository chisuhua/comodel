#pragma once

class PowerState;
#include <cstddef>
#include "types.hh"
#include <vector>
#include "enums/PwrState.hh"
#include "params/SimObject.hh"


struct PowerStateParams : public SimObjectParams
{
    PowerState * create();
    unsigned clk_gate_bins;
    Tick clk_gate_max;
    Tick clk_gate_min;
    Enums::PwrState default_state;
    std::vector< PowerState * > leaders;
    std::vector< Enums::PwrState > possible_states;
};

