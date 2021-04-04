#pragma once

#include <string>
class SimObject;
#include <cstddef>
#include "types.hh"

struct SimObjectParams
{
        SimObjectParams() {}
        virtual ~SimObjectParams() {}

        std::string name;

    uint32_t eventq_index;
};

