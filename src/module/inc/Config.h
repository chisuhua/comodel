#pragma once

#include <map>
#include <string>
#include <vector>

#include "Module.h"
#include "base/inc/flags.h"




#if 0
/** Config file wrapper providing a common interface to ConfigManager */
class ConfigFileBase
{
  public:
    ConfigFileBase() { }
    virtual ~ConfigFileBase() { }

    /** Get a single parameter value as a string returned in value.
     *  For booleans, the function expects "true" or "false" in value.
     *  For NULL Modules, it expects "Null" */
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

    /** Get all Modules in the config */
    virtual void getAllObjectNames(std::vector<std::string> &list) const = 0;

    /** Get the names or paths of all the children Modules of this
     *  Module.  If return_paths is true then full paths are returned.
     *  If false, only the last name component for each object is returned */
    virtual void getObjectChildren(const std::string &object_name,
        std::vector<std::string> &children,
        bool return_paths = false) const = 0;

    /** Load config file */
    virtual bool load(const std::string &filename) = 0;

    /** Get the flags which should be used to modify parameter parsing
     *  behaviour */
    virtual ConfigParams::Flags getFlags() const { return 0; }
};
#endif

/** Directory of all Module classes config details */
// extern std::map<std::string, ConfigDirectoryEntry *> config_directory;

