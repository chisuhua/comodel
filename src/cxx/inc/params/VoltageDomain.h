#pragma once

class VoltageDomain;
#include <vector>

#include "params/SimObject.h"

struct VoltageDomainParams : public SimObjectParams
{
    VoltageDomain * create();
    std::vector< double > voltage;
};


