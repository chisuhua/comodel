#include "base/inc/logging.h"

#include <cstdlib>
#include <iostream>

#include "base/inc/hostinfo.h"

namespace {

class NormalLogger : public Logger
{
  public:
    using Logger::Logger;

  protected:
    void log(const Loc &loc, std::string s) override { std::cerr << s; }
};

class ExitLogger : public NormalLogger
{
  public:
    using NormalLogger::NormalLogger;

  protected:
    void
    log(const Loc &loc, std::string s) override
    {
        auto str = GetFormatString("Memory Usage: %ld KBytes\n", memUsage());
        NormalLogger::log(loc, s + str);
    }
};

class FatalLogger : public ExitLogger
{
  public:
    using ExitLogger::ExitLogger;

  protected:
    void exit() override { ::exit(1); }
};

ExitLogger panicLogger("panic: ");
FatalLogger fatalLogger("fatal: ");
NormalLogger warnLogger("warn: ");
NormalLogger infoLogger("info: ");
NormalLogger hackLogger("hack: ");

} // anonymous namespace

Logger &Logger::getPanic() { return panicLogger; }
Logger &Logger::getFatal() { return fatalLogger; }
Logger &Logger::getWarn() { return warnLogger; }
Logger &Logger::getInfo() { return infoLogger; }
Logger &Logger::getHack() { return hackLogger; }
