#pragma once

namespace Enums {
    enum PwrState {
        UNDEFINED = 0,
        ON = 1,
        CLK_GATED = 2,
        SRAM_RETENTION = 3,
        OFF = 4,
        Num_PwrState = 5
    };
    extern const char *PwrStateStrings[Num_PwrState];
};

