#include "base/inc/time.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

#include "base/inc/logging.h"
#include "sim/inc/core.h"
// #include "sim/serialize.hh"

using namespace std;

void
Time::_set(bool monotonic)
{
#if USE_POSIX_CLOCK
    ::clock_gettime(monotonic ? CLOCK_MONOTONIC : CLOCK_REALTIME, &_time);
#else
    timeval tv;
    ::gettimeofday(&tv, NULL);
    operator=(tv);
#endif
}

void
Time::setTick(Tick ticks)
{
    uint64_t secs = ticks / SimClock::Frequency;
    ticks -= secs * SimClock::Frequency;
    uint64_t nsecs = static_cast<uint64_t>(ticks * SimClock::Float::GHz);
    set(secs, nsecs);
}

Tick
Time::getTick() const
{
    return sec() * SimClock::Frequency +
        static_cast<uint64_t>(nsec() * SimClock::Float::ns);
}

string
Time::date(const string &format) const
{
    time_t sec = this->sec();
    char buf[256];

    if (format.empty()) {
#ifdef __SUNPRO_CC
        ctime_r(&sec, buf, sizeof(buf));
#else
        ctime_r(&sec, buf);
#endif
        buf[24] = '\0';
        return buf;
    }

    struct tm *tm = localtime(&sec);
    strftime(buf, sizeof(buf), format.c_str(), tm);
    return buf;
}

string
Time::time() const
{
    double time = double(*this);
    double secs = fmod(time, 60.0);
    double all_mins = floor(time / 60.0);
    double mins = fmod(all_mins, 60.0);
    double hours = floor(all_mins / 60.0);

    stringstream str;

    if (hours > 0.0) {
        if (hours < 10.0)
            str << '0';
        str << hours << ':';
    }

    if (mins > 0.0) {
        if (mins < 10.0)
            str << '0';
        str << mins << ':';
    }

    if (secs < 10.0 && !str.str().empty())
        str << '0';
    str << secs;

    return str.str();
}

void
Time::serialize(const std::string &base, CheckpointOut &cp) const
{
    paramOut(cp, base + ".sec", sec());
    paramOut(cp, base + ".nsec", nsec());
}

void
Time::unserialize(const std::string &base, CheckpointIn &cp)
{
    time_t secs;
    time_t nsecs;
    paramIn(cp, base + ".sec", secs);
    paramIn(cp, base + ".nsec", nsecs);
    sec(secs);
    nsec(nsecs);
}

void
sleep(const Time &time)
{
    timespec ts = time;

#if USE_POSIX_CLOCK
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
#else
    nanosleep(&ts, NULL);
#endif
}

time_t
mkutctime(struct tm *time)
{
    // get the current timezone
    char *tz = getenv("TZ");

    // copy the string as the pointer gets invalidated when updating
    // the environment
    if (tz) {
        tz = strdup(tz);
        if (!tz) {
            fatal("Failed to reserve memory for UTC time conversion\n");
        }
    }

    // change to UTC and get the time
    setenv("TZ", "", 1);
    tzset();
    time_t ret = mktime(time);

    // restore the timezone again
    if (tz) {
        setenv("TZ", tz, 1);
        free(tz);
    } else {
        unsetenv("TZ");
    }
    tzset();

    return ret;
}

