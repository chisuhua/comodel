#pragma once
#include "Module.h"

struct SimpleParams : public ParamsNode {
    int sim_quantum;
};

class Simple : public CoModuleNode {
    public:
    explicit Simple(ModuleParams params);
};
