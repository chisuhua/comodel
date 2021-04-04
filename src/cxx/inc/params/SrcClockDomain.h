#pragma once

class SrcClockDomain;

#include "base/types.hh"

#include <vector>
#include <cstddef>
#include "params/VoltageDomain.hh"
#include "params/ClockDomain.hh"

struct SrcClockDomainParams : public ClockDomainParams
{
    SrcClockDomain * create();
    std::vector< Tick > clock;
    int32_t domain_id;
    uint32_t init_perf_level;
    VoltageDomain * voltage_domain;
};


