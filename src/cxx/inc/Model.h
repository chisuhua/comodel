#pragma once
#include <inttypes.h>

#include "CxxConfig.hh"


using namespace model;

class Model : public Module {
    public:
    Model(uint32_t deviceIdx);
    ~Model() {}

    virtual void cxxConfigInit() = 0;

    static ModelType* Create(std::string inifile);

    /** Directory of all SimObject classes config details */
    std::map<std::string, CxxConfigDirectoryEntry *> cxx_config_directory;
    CxxConfigManager *m_config_manager;
    std::string m_name;
};

#ifdef __cplusplus
}
#endif
