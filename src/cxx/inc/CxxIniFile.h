#pragma once

#include "Inifile.h"
#include "CxxConfig.h"

/** CxxConfigManager interface for using .ini files */
class CxxIniFile : public CxxConfigFileBase
{
  protected:
    IniFile iniFile;

  public:
    CxxIniFile() { }

    /* Most of these functions work by mapping 'object' onto 'section' */

    bool getParam(const std::string &object_name,
        const std::string &param_name,
        std::string &value) const;

    bool getParamVector(const std::string &object_name,
        const std::string &param_name,
        std::vector<std::string> &values) const;

    bool getPortPeers(const std::string &object_name,
        const std::string &port_name,
        std::vector<std::string> &peers) const;

    bool objectExists(const std::string &object_name) const;

    void getAllObjectNames(std::vector<std::string> &list) const;

    void getObjectChildren(const std::string &object_name,
        std::vector<std::string> &children,
        bool return_paths = false) const;

    bool load(const std::string &filename);
};

