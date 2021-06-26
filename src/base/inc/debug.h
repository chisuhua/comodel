#pragma once

#include <map>
#include <string>
#include <vector>

namespace Debug {

void breakpoint();

class Flag
{
  protected:
    const char *_name;
    const char *_desc;

  public:
    Flag(const char *name, const char *desc);
    virtual ~Flag();

    std::string name() const { return _name; }
    std::string desc() const { return _desc; }
    virtual std::vector<Flag *> kids() { return std::vector<Flag*>(); }

    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual void sync() {}
};

class SimpleFlag : public Flag
{
    static bool _active; // whether debug tracings are enabled
  protected:
    bool _tracing; // tracing is enabled and flag is on
    bool _status;  // flag status

  public:
    SimpleFlag(const char *name, const char *desc)
        : Flag(name, desc), _status(false)
    { }

    bool status() const { return _tracing; }
    operator bool() const { return _tracing; }
    bool operator!() const { return !_tracing; }

    void enable()  { _status = true;  sync(); }
    void disable() { _status = false; sync(); }

    void sync() { _tracing = _active && _status; }

    static void enableAll();
    static void disableAll();
};

class CompoundFlag : public Flag
{
  protected:
    std::vector<Flag *> _kids;

    void
    addFlag(Flag *f)
    {
        if (f != nullptr)
            _kids.push_back(f);
    }

  public:
    CompoundFlag(const char *name, const char *desc,
        Flag *f00 = nullptr, Flag *f01 = nullptr,
        Flag *f02 = nullptr, Flag *f03 = nullptr,
        Flag *f04 = nullptr, Flag *f05 = nullptr,
        Flag *f06 = nullptr, Flag *f07 = nullptr,
        Flag *f08 = nullptr, Flag *f09 = nullptr,
        Flag *f10 = nullptr, Flag *f11 = nullptr,
        Flag *f12 = nullptr, Flag *f13 = nullptr,
        Flag *f14 = nullptr, Flag *f15 = nullptr,
        Flag *f16 = nullptr, Flag *f17 = nullptr,
        Flag *f18 = nullptr, Flag *f19 = nullptr)
        : Flag(name, desc)
    {
        addFlag(f00); addFlag(f01); addFlag(f02); addFlag(f03); addFlag(f04);
        addFlag(f05); addFlag(f06); addFlag(f07); addFlag(f08); addFlag(f09);
        addFlag(f10); addFlag(f11); addFlag(f12); addFlag(f13); addFlag(f14);
        addFlag(f15); addFlag(f16); addFlag(f17); addFlag(f18); addFlag(f19);
    }

    std::vector<Flag *> kids() { return _kids; }

    void enable();
    void disable();
};

typedef std::map<std::string, Flag *> FlagsMap;
FlagsMap &allFlags();

Flag *findFlag(const std::string &name);

extern Flag *const All;

bool changeFlag(const char *s, bool value);

} // namespace Debug

void setDebugFlag(const char *string);

void clearDebugFlag(const char *string);

void dumpDebugFlags();

