
/** @file This header provides some core simulator functionality such as time
 * information, output directory and exit events
 */

#include <string>

#include "types.hh"
#include "Eventq.hh"

/// The universal simulation clock.
inline Tick curTick() { return _curEventQueue->getCurTick(); }

const Tick retryTime = 1000;

/// These are variables that are set based on the simulator frequency
///@{
namespace SimClock {
extern Tick Frequency; ///< The number of ticks that equal one second

namespace Float {

/** These variables equal the number of ticks in the unit of time they're
 * named after in a double.
 * @{
 */
extern double s;  ///< second
extern double ms; ///< millisecond
extern double us; ///< microsecond
extern double ns; ///< nanosecond
extern double ps; ///< picosecond
/** @} */

/** These variables the inverse of above. They're all < 1.
 * @{
 */
extern double Hz;  ///< Hz
extern double kHz; ///< kHz
extern double MHz; ///< MHz
extern double GHz; ///< GHz
/** @}*/
} // namespace Float

/** These variables equal the number of ticks in the unit of time they're
 *  named after in a 64 bit integer.
 *
 * @{
 */
namespace Int {
extern Tick s;  ///< second
extern Tick ms; ///< millisecond
extern Tick us; ///< microsecond
extern Tick ns; ///< nanosecond
extern Tick ps; ///< picosecond
/** @} */
} // namespace Int
} // namespace SimClock
/** @} */

void fixClockFrequency();
bool clockFrequencyFixed();

void setClockFrequency(Tick ticksPerSecond);
Tick getClockFrequency(); // Ticks per second.

void setOutputDir(const std::string &dir);

class Callback;
void registerExitCallback(Callback *callback);
void doExitCleanup();


