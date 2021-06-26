#include "Simple.h"
#include <iostream>

Simple::Simple(ModuleParams params)
    : CoModuleNode(params)
{
    std::cout << "Hello World! From a Simple!" << std::endl;
}

CoModule CreateModule_(ModuleParams params) {
    return CoModule(make_object<Simple>(params));
}

Port CreatePort_(std::string port_name, std::string peer_name) {
    return Port(make_object<PortNode>(port_name, -1));
}

ModuleParams CreateParams_(std::string json_str) {
    return ModuleParams(make_object<SimpleParams>());
}

TVM_DLL_EXPORT_TYPED_FUNC(CreateModule, CreateModule_);
TVM_DLL_EXPORT_TYPED_FUNC(CreateParams, CreateParams_);
TVM_DLL_EXPORT_TYPED_FUNC(CreatePort, CreatePort_);
