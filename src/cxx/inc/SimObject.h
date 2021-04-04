#pragma once

#include <string>
#include <vector>
#include "SimObjectParam.h"

class EventManager;

class SimObject : public EventManager
{
  private:
    typedef std::vector<SimObject *> SimObjectList;

    /** List of all instantiated simulation objects. */
    static SimObjectList simObjectList;

  protected:
    /** Cached copy of the object parameters. */
    const SimObjectParams *_params;

  public:
    typedef SimObjectParams Params;
    const Params *params() const { return _params; }
    SimObject(const Params *_params);
    virtual ~SimObject();

  public:

    virtual const std::string name() const { return params()->name; }

    /**
     * init() is called after all C++ SimObjects have been created and
     * all ports are connected.  Initializations that are independent
     * of unserialization but rely on a fully instantiated and
     * connected SimObject graph should be done here.
     */
    virtual void init();

   /**
     * startup() is the final initialization call before simulation.
     * All state is initialized (including unserialized state, if any,
     * such as the curTick() value), so this is the appropriate place to
     * schedule initial event(s) for objects that need them.
     */
    virtual void startup();

    /**
     * Find the SimObject with the given name and return a pointer to
     * it.  Primarily used for interactive debugging.  Argument is
     * char* rather than std::string to make it callable from gdb.
     */
    static SimObject *find(const char *name);
};


