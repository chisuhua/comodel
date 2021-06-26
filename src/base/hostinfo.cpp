#include <unistd.h>

#ifdef __APPLE__
#include <mach/mach_init.h>
#include <mach/shared_region.h>
#include <mach/task.h>

#endif

#include "base/inc/hostinfo.h"

#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "base/inc/logging.h"
#include "base/inc/str.h"
#include "base/inc/types.h"

using namespace std;

string
__get_hostname()
{
    char host[256];
    if (gethostname(host, sizeof host) == -1)
        warn("could not get host name!");
    return host;
}

string &
hostname()
{
    static string hostname = __get_hostname();
    return hostname;
}

uint64_t
procInfo(const char *filename, const char *target)
{
    int  done = 0;
    char line[80];
    char format[80];
    long usage;

    FILE *fp = fopen(filename, "r");

    while (fp && !feof(fp) && !done) {
        if (fgets(line, 80, fp)) {
            if (startswith(line, target)) {
                snprintf(format, sizeof(format), "%s %%ld", target);
                sscanf(line, format, &usage);

                fclose(fp);
                return usage ;
            }
        }
    }

    if (fp)
        fclose(fp);

    return 0;
}

uint64_t
memUsage()
{
// For the Mach-based Darwin kernel, use the task_info of the self task
#ifdef __APPLE__
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

    if (KERN_SUCCESS != task_info(mach_task_self(),
                                  TASK_BASIC_INFO, (task_info_t)&t_info,
                                  &t_info_count)) {
        return 0;
    }

    // Mimic Darwin's implementation of top and subtract
    // SHARED_REGION_SIZE from the tasks virtual size to account for the
    // shared memory submap that is incorporated into every process.
    return (t_info.virtual_size - SHARED_REGION_SIZE) / 1024;
#else
    // Linux implementation
    return procInfo("/proc/self/status", "VmSize:");
#endif
}
