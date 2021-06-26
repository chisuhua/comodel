#include "ConfigIniFile.h"

#include "base/inc/str.h"

bool
ConfigIniFile::getParam(const std::string &object_name,
    const std::string &param_name,
    std::string &value) const
{
    return iniFile.find(object_name, param_name, value);
}

bool
ConfigIniFile::getParamVector(const std::string &object_name,
    const std::string &param_name,
    std::vector<std::string> &values) const
{
    std::string value;
    bool ret = iniFile.find(object_name, param_name, value);

    if (ret) {
        std::vector<std::string> sub_object_names;

        tokenize(values, value, ' ', true);
    }

    return ret;
}

bool
ConfigIniFile::getPortPeers(const std::string &object_name,
    const std::string &port_name,
    std::vector<std::string> &peers) const
{
    return getParamVector(object_name, port_name, peers);
}

bool
ConfigIniFile::objectExists(const std::string &object) const
{
    return iniFile.sectionExists(object);
}

void
ConfigIniFile::getAllObjectNames(std::vector<std::string> &list) const
{
    iniFile.getSectionNames(list);
}

void
ConfigIniFile::getObjectChildren(const std::string &object_name,
    std::vector<std::string> &children, bool return_paths) const
{
    if (!getParamVector(object_name, "children", children))
        return;

    if (return_paths && object_name != "root") {
        for (auto i = children.begin(); i != children.end(); ++i)
            *i = object_name + "." + *i;
    }
}

bool
ConfigIniFile::load(const std::string &filename)
{
    return iniFile.load(filename);
}
