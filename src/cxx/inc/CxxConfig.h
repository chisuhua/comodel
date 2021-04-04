#pragma once

#include <map>
#include <string>
#include <vector>

#include "SimObject.h"

class CxxConfigParams;

/** Config details entry for a SimObject.  Instances of this class contain
 *  enough configuration layout information to popular a ...Param structure
 *  and build a SimObject from it with the help of the 'set' functions in
 *  each ...Param class */
class CxxConfigDirectoryEntry
{
  public:
    /* Class to represent parameters and SimObject references within
     *  SimObjects */
    class ParamDesc
    {
      public:
        const std::string name;

        /* Is this a vector or singleton parameters/SimObject */
        const bool isVector;

        /** Is this a SimObject, and so is to be set with setSimObject...
         *  or another from-string parameter set with setParam... */
        const bool isSimObject;

        ParamDesc(const std::string &name_,
            bool isVector_, bool isSimObject_) :
            name(name_), isVector(isVector_), isSimObject(isSimObject_)
        { }
    };

    /** Similar to ParamDesc to describe ports */
    class PortDesc
    {
      public:
        const std::string name;

        /* Is this a vector or singleton parameters/SimObject */
        const bool isVector;

        /** Is this a master or slave port */
        const bool isMaster;

        PortDesc(const std::string &name_,
            bool isVector_, bool isMaster_) :
            name(name_), isVector(isVector_), isMaster(isMaster_)
        { }
    };

    /** All parameters (including SimObjects) in order */
    std::map<std::string, ParamDesc *> parameters;

    /** Ports */
    std::map<std::string, PortDesc *> ports;

    /** Make a ...Param structure for the SimObject class of this entry */
    virtual CxxConfigParams *makeParamsObject() const { return NULL; }

    virtual ~CxxConfigDirectoryEntry() { }
};

/** Base for peer classes of SimObjectParams derived classes with parameter
 *  modifying member functions. C++ configuration will offer objects of
 *  these classes to SimObjects as params rather than SimObjectParams
 *  objects */
class CxxConfigParams
{
  private:
    static const std::string invalidName = "<invalid>";

  public:
    /** Flags passable to setParam... to smooth over any parsing difference
     *  between different config files */
    typedef uint32_t FlagsType;
    typedef ::Flags<FlagsType> Flags;

    /** Example flag */
    /* static const FlagsType MY_NEW_FLAG = 0x00000001; */

  public:
    /** Set future object's full path name */
    virtual void setName(const std::string &name_) { }

    /** Get full path name string */
    virtual const std::string &getName() { return invalidName; }

    /** Set a SimObject valued parameter with a reference to the given
     *  SimObject.  This will return false if the parameter name is not
     *  valid or the object is of the wrong type */
    virtual bool setSimObject(const std::string &name,
        SimObject *simObject)
    { return false; }

    /** As setSimObjectVector but set a whole vector of references */
    virtual bool setSimObjectVector(const std::string &name,
        const std::vector<SimObject *> &simObjects)
    { return false; }

    /** Set a parameter with a value parsed from the given string.  The
     *  parsing regime matches the format of .ini config files.  Returns
     *  false if the parameter name is not valid or the string cannot be
     *  parsed as the type of the parameter */
    virtual bool setParam(const std::string &name,
        const std::string &value, const Flags flags)
    { return false; }

    /** As setParamVector but for parameters given as vectors pre-separated
     *  into elements */
    virtual bool setParamVector(const std::string &name,
        const std::vector<std::string> &values, const Flags flags)
    { return false; }

    /** Set the number of connections expected for the named port.  Returns
     *  false if the port name is not valid */
    virtual bool setPortConnectionCount(const std::string &name,
        unsigned int count)
    { return false; }

    /** Create the associated SimObject */
    virtual SimObject *simObjectCreate() { return NULL; }

    CxxConfigParams() { }

    virtual ~CxxConfigParams() { }
};

/** Config file wrapper providing a common interface to CxxConfigManager */
class CxxConfigFileBase
{
  public:
    CxxConfigFileBase() { }
    virtual ~CxxConfigFileBase() { }

    /** Get a single parameter value as a string returned in value.
     *  For booleans, the function expects "true" or "false" in value.
     *  For NULL SimObjects, it expects "Null" */
    virtual bool getParam(const std::string &object_name,
        const std::string &param_name,
        std::string &value) const = 0;

    /** Get a list/vector parameter */
    virtual bool getParamVector(const std::string &object_name,
        const std::string &param_name,
        std::vector<std::string> &values) const = 0;

    /** Get the peer (connected) ports of the named ports */
    virtual bool getPortPeers(const std::string &object_name,
        const std::string &port_name,
        std::vector<std::string> &peers) const = 0;

    /** Does an object with this path exist? */
    virtual bool objectExists(const std::string &object_name) const = 0;

    /** Get all SimObjects in the config */
    virtual void getAllObjectNames(std::vector<std::string> &list) const = 0;

    /** Get the names or paths of all the children SimObjects of this
     *  SimObject.  If return_paths is true then full paths are returned.
     *  If false, only the last name component for each object is returned */
    virtual void getObjectChildren(const std::string &object_name,
        std::vector<std::string> &children,
        bool return_paths = false) const = 0;

    /** Load config file */
    virtual bool load(const std::string &filename) = 0;

    /** Get the flags which should be used to modify parameter parsing
     *  behaviour */
    virtual CxxConfigParams::Flags getFlags() const { return 0; }
};

/** Directory of all SimObject classes config details */
extern std::map<std::string, CxxConfigDirectoryEntry *>
    cxx_config_directory;

/** Initialise cxx_config_directory.  This is defined in the
 *  auto-generated .../cxx_config/init.cc */
void cxxConfigInit();

