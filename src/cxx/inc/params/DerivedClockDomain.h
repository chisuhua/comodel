#pragma once

class DerivedClockDomain;

#include <cstddef>
#include "types.hh"
#include "params/ClockDomain.hh"

struct DerivedClockDomainParams : public ClockDomainParams
{
    DerivedClockDomain * create();
    unsigned clk_divider;
    ClockDomain * clk_domain;
};


