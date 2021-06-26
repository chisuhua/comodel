#pragma once
#include "base/inc/types.h"
#include "sim/inc/core.h"

namespace Stats {

double statElapsedTime();

Tick statElapsedTicks();

Tick statFinalTick();

void initSimStats();

/**
 * Update the events after resuming from a checkpoint. When resuming from a
 * checkpoint, curTick will be updated, and any already scheduled events can
 * end up scheduled in the past. This function checks if the dumpEvent is
 * scheduled in the past, and reschedules it appropriately.
 */
void updateEvents();

/**
 * Schedule statistics dumping. This allows you to dump and/or reset the
 * built-in statistics. This can either be done once, or it can be done on a
 * regular basis.
 * @param dump Set true to dump the statistics.
 * @param reset Set true to reset the statistics.
 * @param when When the dump and/or reset should occur.
 * @param repeat How often the event should repeat. Set 0 to disable repeating.
 */
void schedStatEvent(bool dump, bool reset, Tick when = curTick(),
                    Tick repeat = 0);

/**
 * Schedule periodic statistics dumping. This allows you to dump and reset the
 * built-in statistics on a regular basis, thereby allowing the extraction of
 * temporal trends in the data.
 * @param period The period at which the dumping should occur.
 */
void periodicStatDump(Tick period = 0);
} // namespace Stats


