#include <condition_variable>

class Barrier
{
  private:
    /// Mutex to protect access to numLeft and generation
    std::mutex bMutex;
    /// Condition variable for waiting on barrier
    std::condition_variable bCond;
    /// Number of threads we should be waiting for before completing the barrier
    unsigned numWaiting;
    /// Generation of this barrier
    unsigned generation;
    /// Number of threads remaining for the current generation
    unsigned numLeft;

  public:
    Barrier(unsigned _numWaiting)
        : numWaiting(_numWaiting), generation(0), numLeft(_numWaiting)
    {}

    bool
    wait()
    {
        std::unique_lock<std::mutex> lock(bMutex);
        unsigned int gen = generation;

        if (--numLeft == 0) {
            generation++;
            numLeft = numWaiting;
            bCond.notify_all();
            return true;
        }
        while (gen == generation)
            bCond.wait(lock);
        return false;
    }
};

