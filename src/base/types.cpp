#include "types.hh"

std::ostream&
operator<<(std::ostream &out, const Cycles & cycles)
{
    out << cycles.c;
    return out;
}

