#pragma once
#include "inc/AddrDef.h"
#include "inc/CpReg.h"
#include "inc/MemReg.h"
#include "common/utils.h"

template<typename T>
std::string DecodeRegSpace(T &offset) {
    if (offset >= mmCP_REG_ADDR && offset < mmCP_REG_ADDR + CP_REG_SIZE) {
        offset -= mmCP_REG_ADDR;
        return "cp";
    } else if (offset >= mmMMU_REG_ADDR && offset < mmMMU_REG_ADDR + MMU_REG_SIZE) {
        offset -= mmMMU_REG_ADDR;
        return "mc";
    } else {
        debug_warning(offset);
        assert(0);
    }
    return "false";
}
