#pragma once

class PowerModel;
#include <cstddef>
#include <vector>

#include "enums/PMType.hh"
#include "params/SubSystem.hh"
#include "params/PowerModelState.hh"
#include "params/SimObject.hh"

#include "enums/PMType.hh"

struct PowerModelParams
    : public SimObjectParams
{
    PowerModel * create();
    double ambient_temp;
    std::vector< PowerModelState * > pm;
    Enums::PMType pm_type;
    SubSystem * subsystem;
};

