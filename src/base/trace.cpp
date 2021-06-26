#include "base/inc/trace.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "base/atomicio.hh"
#include "base/inc/debug.h"
#include "base/inc/logging.h"
#include "base/output.hh"
#include "base/inc/str.h"
#include "debug/FmtFlag.hh"
#include "debug/FmtStackTrace.hh"
#include "debug/FmtTicksOff.hh"
#include "sim/backtrace.hh"

const std::string &name()
{
    static const std::string default_name("global");

    return default_name;
}

namespace Trace
{

// This variable holds the output logger for debug information.  Other
// than setting up/redirecting this logger, do *NOT* reference this
// directly

Logger *debug_logger = NULL;

Logger *
getDebugLogger()
{
    /* Set a default logger to cerr when no other logger is set */
    if (!debug_logger)
        debug_logger = new OstreamLogger(std::cerr);

    return debug_logger;
}

std::ostream &
output()
{
    return getDebugLogger()->getOstream();
}

void
setDebugLogger(Logger *logger)
{
    if (!logger)
        warn("Trying to set debug logger to NULL\n");
    else
        debug_logger = logger;
}

void
enable()
{
    Debug::SimpleFlag::enableAll();
}

void
disable()
{
    Debug::SimpleFlag::disableAll();
}

ObjectMatch ignore;

#if 0
void
Logger::dump(Tick when, const std::string &name,
         const void *d, int len, const std::string &flag)
{
    if (!name.empty() && ignore.match(name))
        return;

    const char *data = static_cast<const char *>(d);
    int c, i, j;

    for (i = 0; i < len; i += 16) {
        std::string line;

        line = GetFormatString("%08x  ", i);
        c = len - i;
        if (c > 16) c = 16;

        for (j = 0; j < c; j++) {
            GetFormatString(line, "%02x ", data[i + j] & 0xff);
            if ((j & 0xf) == 7 && j > 0)
                GetFormatString(line, " ");
        }

        for (; j < 16; j++)
            GetFormatString(line, "   ");
        GetFormatString(line, "  ");

        for (j = 0; j < c; j++) {
            int ch = data[i + j] & 0x7f;
            GetFormatString(line, "%c", (char)(isprint(ch) ? ch : ' '));
        }

        GetFormatString(line, "\n");
        logMessage(when, name, flag, line.str());

        if (c < 16)
            break;
    }
}
#endif

void
OstreamLogger::logMessage(Tick when, const std::string &name,
        const std::string &flag, const std::string &message)
{
    if (!name.empty() && ignore.match(name))
        return;

    if (!DTRACE(FmtTicksOff) && (when != MaxTick))
        GetFormatString(stream, "%7d: ", when);

    if (DTRACE(FmtFlag) && !flag.empty())
        stream << flag << ": ";

    if (!name.empty())
        stream << name << ": ";

    stream << message;
    stream.flush();

    if (DTRACE(FmtStackTrace)) {
        print_backtrace();
        STATIC_ERR("\n");
    }
}

} // namespace Trace
