#include "inc/Model.h"


Model::Model(std::string name) 
    : m_name(name)
{
}

//  TODO
// ModelType and ModelInstance
template<typename ModelType>
ModelType* Model::Create(std::string name,  std::string inifile) {
    ModelType* pModel = new Model(std::string name);

    pModel->cxxConfigInit();

    CxxConfigFileBase *conf = new CxxIniFile();

    if (!conf->load(inifile.c_str())) {
        std::cerr << "Can't open config file: " << inifile << '\n';
        return EXIT_FAILURE;
    }

    m_config_manager = new CxxConfigManager(*conf);

    // TODO override ini param by argument
    // config_manager->setParam(argv[arg_ptr], argv[arg_ptr + 1],
    // config_manager->setParamVector(argv[arg_ptr],
    try {
        config_manager->instantiate();
        config_manager->initState();
        config_manager->startup();
    } catch (CxxConfigManager::Exception &e) {
        std::cerr << "Config problem in sim object " << e.name
            << ": " << e.message << "\n";

        return EXIT_FAILURE;
    }

}


