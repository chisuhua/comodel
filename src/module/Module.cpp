#include <string>
#include <vector>

#include "Module.h"
#include "Port.h"
#include "Config.h"

/*
CoModule CreateMyModule_(const ModuleParams *params) {
   return CoModule(make_object<CoModuleNode>(params));
}
*/

const std::string CoModuleNode::name() const { return params()->getName(); }

void
CoModuleNode::RegisterPort(const std::string &if_name, Port port) {
}

Port
CoModuleNode::getPort(const std::string &if_name, PortID idx)
{
    fatal("%s does not have any port named %s\n", name(), if_name);
    for (auto port : ports_) {
        if ((port->name() == if_name) && (port->getId() == idx)) {
            return port;
        }
    }
}

// TVM_DLL_EXPORT_TYPED_FUNC(CreateMyModule, CreateMyModule_);
