#include "base/inc/debug.h"

#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <csignal>

#include "base/inc/cprintf.h"
#include "base/inc/logging.h"

using namespace std;

namespace Debug {

//
// This function will cause the process to signal itself with a
// SIGTRAP which is ignored if not in gdb, but will cause the debugger
// to break if in gdb.
//
void
breakpoint()
{
#ifndef NDEBUG
    kill(getpid(), SIGTRAP);
#else
    cprintf("Debug::breakpoint suppressed, compiled with NDEBUG\n");
#endif
}

//
// Flags for debugging purposes.  Primarily for trace.hh
//
int allFlagsVersion = 0;
FlagsMap &
allFlags()
{
    static FlagsMap flags;
    return flags;
}

bool SimpleFlag::_active = false;

Flag *
findFlag(const std::string &name)
{
    FlagsMap::iterator i = allFlags().find(name);
    if (i == allFlags().end())
        return NULL;
    return i->second;
}

Flag::Flag(const char *name, const char *desc)
    : _name(name), _desc(desc)
{
    pair<FlagsMap::iterator, bool> result =
        allFlags().insert(make_pair(name, this));

    if (!result.second)
        panic("Flag %s already defined!", name);

    ++allFlagsVersion;
}

Flag::~Flag()
{
    // should find and remove flag.
}

void
SimpleFlag::enableAll()
{
    _active = true;
    for (auto& i : allFlags())
        i.second->sync();
}

void
SimpleFlag::disableAll()
{
    _active = false;
    for (auto& i : allFlags())
        i.second->sync();
}

void
CompoundFlag::enable()
{
    for (auto& k : _kids)
        k->enable();
}

void
CompoundFlag::disable()
{
    for (auto& k : _kids)
        k->disable();
}

struct AllFlags : public Flag
{
    AllFlags()
        : Flag("All", "All Flags")
    {}

    void
    enable()
    {
        FlagsMap::iterator i = allFlags().begin();
        FlagsMap::iterator end = allFlags().end();
        for (; i != end; ++i)
            if (i->second != this)
                i->second->enable();
    }

    void
    disable()
    {
        FlagsMap::iterator i = allFlags().begin();
        FlagsMap::iterator end = allFlags().end();
        for (; i != end; ++i)
            if (i->second != this)
                i->second->disable();
    }
};

AllFlags theAllFlags;
Flag *const All = &theAllFlags;

bool
changeFlag(const char *s, bool value)
{
    Flag *f = findFlag(s);
    if (!f)
        return false;

    if (value)
        f->enable();
    else
        f->disable();

    return true;
}

} // namespace Debug

// add a set of functions that can easily be invoked from gdb
void
setDebugFlag(const char *string)
{
    Debug::changeFlag(string, true);
}

void
clearDebugFlag(const char *string)
{
    Debug::changeFlag(string, false);
}

void
dumpDebugFlags()
{
    using namespace Debug;
    FlagsMap::iterator i = allFlags().begin();
    FlagsMap::iterator end = allFlags().end();
    for (; i != end; ++i) {
        SimpleFlag *f = dynamic_cast<SimpleFlag *>(i->second);
        if (f && f->status())
            cprintf("%s\n", f->name());
    }
}
