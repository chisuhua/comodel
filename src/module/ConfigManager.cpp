#include "ConfigManager.h"

#include <cstdlib>
#include <sstream>

#include "base/inc/str.h"
#include "base/inc/trace.h"
#include <iguana/json.hpp>
// #include "debug/CxxConfig.hh"
// #include "sim/serialize.hh"
#include "Module.h"

ConfigManager::ConfigManager(ConfigIniFile &configFile_) :
    configFile(configFile_)
{
}

void
ConfigManager::findObjectType(const std::string &object_name, std::string &object_type)
{
    if (!configFile.objectExists(object_name))
        throw Exception(object_name, "Can't find sim object");

    if (!configFile.getParam(object_name, "type", object_type))
        throw Exception(object_name, "Sim object has no 'type' field");
}

std::string
ConfigManager::rename(const std::string &from_name)
{
    for (auto i = renamings.begin(); i != renamings.end(); ++ i) {
        const Renaming &renaming = *i;

        if (from_name.find(renaming.fromPrefix) == 0) {
            return renaming.toPrefix +
                from_name.substr(renaming.fromPrefix.length());
        }
    }

    return from_name;
}

std::string
ConfigManager::unRename(const std::string &to_name)
{
    for (auto i = renamings.begin(); i != renamings.end(); ++ i) {
        const Renaming &renaming = *i;

        if (to_name.find(renaming.toPrefix) == 0) {
            return renaming.fromPrefix +
                to_name.substr(renaming.toPrefix.length());
        }
    }

    return to_name;
}

static std::string formatParamList(const std::vector<std::string> &param_values)
{
    std::ostringstream params;

    auto i = param_values.begin();
    auto end_i = param_values.end();

    params << '[';
    while (i != end_i) {
        params << (*i);
        ++i;

        if (i != end_i)
            params << ", ";
    }
    params << ']';

    return params.str();
}

CoModule ConfigManager::findObject(const std::string &object_name, bool visit_children)
{
    std::string instance_name = rename(object_name);

    if (object_name == "Null")
        return CoModule(ObjectPtr<Object>(nullptr));

    /* Already constructed */
    if (objectsByName.find(instance_name) != objectsByName.end())
        return objectsByName[instance_name];

    if (inVisit.find(instance_name) != inVisit.end())
        throw Exception(instance_name, "Cycle in configuration");

    std::string object_type;
    findObjectType(object_name, object_type);

    CoModule object; // = CoModule(ObjectPtr<Object>(nullptr));

    ModuleParams object_params = findObjectParams(object_name);

    try {
        DPRINTF(CxxConfig, "Configuring sim object references for: %s"
            " (%s from object %s)\n", instance_name, object_type, object_name);

        /* Remember the path back to the top of the recursion to detect
         *  cycles */
        inVisit.insert(instance_name);

        std::vector<std::string> sub_object_names;
        std::vector<CoModule> sub_objects;
        if (configFile.getParamVector(object_name, "children", sub_object_names)) {
            /* Resolve pointed-to SimObjects by recursing into them */
            for (auto n = sub_object_names.begin(); n != sub_object_names.end(); ++n)
            {
                std::string sub_instance_name;
                if (!configFile.getParam(object_name, *n, sub_instance_name))
                {
                    throw Exception(object_name, GetFormatString( "Element not found: %s", object_name));
                }

                CoModule sub_object = findObject(sub_instance_name, visit_children);

                if (sub_object.defined()) sub_objects.push_back(sub_object);
            }
        }

        DPRINTF(CxxConfig, "Creating CoModule: %s\n", instance_name);
        // object = object_params->moduleCreate();
        // object = CreateModule(object_params);

        if (not object.defined()) {
            throw Exception(object_name, GetFormatString("Couldn't create object of"
                " type: %s", object_type));
        }

        objectsByName[instance_name] = object;
        objectParamsByName[instance_name] = object_params;

        if (visit_children) {
            std::vector<std::string> children;
            configFile.getObjectChildren(object_name, children, true);

            /* Visit all your children */
            for (auto i = children.begin(); i != children.end(); ++i)
                findObject(*i, visit_children);
        }
    } catch (Exception &) {
        // delete object_params;
        throw;
    }

    /* Mark that we've exited object
     *  construction and so 'find'ing this object again won't be a
     *  configuration loop */
    inVisit.erase(object_name);
    return object;
}

ModuleParams ConfigManager::findObjectParams(const std::string &object_name)
{
    std::string instance_name = rename(object_name);

    /* Already constructed */
    if (objectParamsByName.find(instance_name) != objectParamsByName.end())
        return objectParamsByName[instance_name];


    std::string object_type;
    findObjectType(object_name, object_type);

    if (ModuleByName.find(object_type) != ModuleByName.end()) {
        tvm::runtime::Module mod = tvm::runtime::Module::LoadFromFile("module" + object_type + ".so");
    }
    auto mod = ModuleByName[object_type];


    FTypeCreateModule CreateModule = mod.GetFunction("CreateModule");
    FTypeCreateParams CreateParams = mod.GetFunction("CreateParams");
    FTypeCreatePort CreatePort = mod.GetFunction("CreatePort");

    DPRINTF(CxxConfig, "Configuring parameters of object: %s (%s)\n", instance_name, object_type);

    ModuleParams object_params;

    try {
        /* Fill in the implicit parameters that don't necessarily
         *  appear in config files */

        /* Fill in parameters */
        std::string param_value;

        if (!configFile.getParam(object_name, "params", param_value)) {
            throw Exception(object_name, GetFormatString("Element not found for parameter params"));
        }

        ModuleParams object_params = CreateParams(param_value);
        object_params->setName(instance_name);
        CoModule module_object = CreateModule(object_params);
        // DPRINTF(CxxConfig, "Setting parameter %s.%s=%s\n", instance_name, param->name, param_value);

        /* Find the number of ports that will need binding and set the
         *  appropriate port_..._connection_count parameters */
        for (auto port_name : {"master_port", "slave_port"}) {
            std::vector<std::string> peers;

            if (!configFile.getPortPeers(object_name, port_name, peers)) {
                DPRINTF(CxxConfig, "Port not found: %s.%s,"
                    " assuming there are no connections\n",
                    instance_name, port_name);
            }

           // TODO  module_object->RegisterPort(CreatePort(port_name, peers));

        }

        /* Set pointed-to SimObjects to NULL */
        /*
        if (configFile.getParamVector(object_name, "children", param_value)) {
            module_object->RegisterSubModule(param_value);
        }
        */
    } catch (Exception &) {
        throw;
    }

    objectParamsByName[instance_name] = object_params;

    return object_params;
}

void
ConfigManager::findAllObjects()
{
    std::vector<std::string> objects;
    configFile.getAllObjectNames(objects);

    /* Set the traversal order for further iterators */
    objectsInOrder.clear();
    findTraversalOrder("root");
}

void
ConfigManager::findTraversalOrder(const std::string &object_name)
{
    CoModule object = findObject(object_name);

    if (object.defined()) {
        objectsInOrder.push_back(object);

        std::vector<std::string> children;
        configFile.getObjectChildren(object_name, children, true);

        /* Visit all your children */
        for (auto i = children.begin(); i != children.end(); ++i)
            findTraversalOrder(*i);
    }
}

void
ConfigManager::bindAllPorts()
{
    for (auto i = objectsInOrder.begin(); i != objectsInOrder.end(); ++i)
        bindObjectPorts(*i);
}

void
ConfigManager::bindPort(
    CoModule master_object, const std::string &master_port_name,
    PortID master_port_index,
    CoModule slave_object, const std::string &slave_port_name,
    PortID slave_port_index)
{
    /* FIXME, check slave_port_index against connection_count
     *  defined for port, need getPortConnectionCount and a
     *  getCxxConfigDirectoryEntry for each object. */

    /* It would be nice to be able to catch the errors from these calls. */
    Port master_port = master_object->getPort(master_port_name, master_port_index);
    Port slave_port = slave_object->getPort(slave_port_name, slave_port_index);

    if (master_port->isConnected()) {
        throw Exception(master_object->name(), GetFormatString(
            "Master port: %s[%d] is already connected\n", master_port_name,
            master_port_index));
    }

    if (slave_port->isConnected()) {
        throw Exception(slave_object->name(), GetFormatString(
            "Slave port: %s[%d] is already connected\n", slave_port_name,
            slave_port_index));
    }

    DPRINTF(CxxConfig, "Binding port %s.%s[%d]"
        " to %s:%s[%d]\n",
        master_object->name(), master_port_name, master_port_index,
        slave_object->name(), slave_port_name, slave_port_index);

    master_port->bind(slave_port);
}

void
ConfigManager::bindMasterPort(CoModule object, const Port &port,
    const std::vector<std::string> &peers)
{
    unsigned int master_port_index = 0;

    for (auto peer_i = peers.begin(); peer_i != peers.end();
        ++peer_i)
    {
        const std::string &peer = *peer_i;
        std::string slave_object_name;
        std::string slave_port_name;
        unsigned int slave_port_index;

        parsePort(peer, slave_object_name, slave_port_name,
            slave_port_index);

        std::string slave_instance_name = rename(slave_object_name);

        if (objectsByName.find(slave_instance_name) == objectsByName.end()) {
            throw Exception(object->name(), GetFormatString(
                "Can't find slave port object: %s", slave_instance_name));
        }

        CoModule slave_object = objectsByName[slave_instance_name];

        bindPort(object, port->name(), master_port_index,
            slave_object, slave_port_name, slave_port_index);

        master_port_index++;
    }
}

void
ConfigManager::bindObjectPorts(CoModule object)
{
    /* We may want to separate object->name() from the name in configuration
     *  later to allow (for example) repetition of fragments of configs */
    const std::string &instance_name = object->name();

    std::string object_name = unRename(instance_name);

    std::string object_type;
    findObjectType(object_name, object_type);

    DPRINTF(CxxConfig, "Binding ports of object: %s (%s)\n", instance_name, object_type);

    for (auto port : object->getPorts()) {
        DPRINTF(CxxConfig, "Binding port: %s.%s\n", instance_name, port->name());

        std::vector<std::string> peers;
        configFile.getPortPeers(object_name, port->name(), peers);

        /* Only handle master ports as binding only needs to happen once
         *  for each observed pair of ports */
        if (port->isMaster()) {
            if (!port->isVector() && peers.size() > 1) {
                throw Exception(instance_name, GetFormatString(
                    "Too many connections to non-vector port %s (%d)\n",
                    port->name(), peers.size()));
            }

            bindMasterPort(object, port, peers);
        }
    }
}

void
ConfigManager::parsePort(const std::string &inp,
    std::string &path, std::string &port, unsigned int &index)
{
    std::size_t dot_i = inp.rfind('.');
    std::size_t open_square_i = inp.rfind('[');

    if (dot_i == std::string::npos) {
        DPRINTF(CxxConfig, "Bad port string: %s\n", inp);
        path = "";
        port = "";
        index = 0;
    } else {
        path = std::string(inp, 0, dot_i);

        if (open_square_i == std::string::npos) {
            /* Singleton port */
            port = std::string(inp, dot_i + 1, inp.length() - dot_i);
            index = 0;
        } else {
            /* Vectored port elemnt */
            port = std::string(inp, dot_i + 1, (open_square_i - 1) - dot_i);
            index = std::atoi(inp.c_str() + open_square_i + 1);
        }
    }
}

void
ConfigManager::forEachObject(void (CoModuleNode::*mem_func)())
{
    for (auto i = objectsInOrder.begin(); i != objectsInOrder.end(); ++i)
        ((*i).Get()->*mem_func)();
}

void
ConfigManager::instantiate(bool build_all)
{
    if (build_all) {
        findAllObjects();
        bindAllPorts();
    }

    DPRINTF(CxxConfig, "Initialising all objects\n");
    forEachObject(&CoModuleNode::Init);
    /*
    DPRINTF(CxxConfig, "Registering stats\n");
    forEachObject(&SimObject::regStats);

    DPRINTF(CxxConfig, "Registering probe points\n");
    forEachObject(&SimObject::regProbePoints);

    DPRINTF(CxxConfig, "Connecting probe listeners\n");
    forEachObject(&SimObject::regProbeListeners);
    */
}
/*
void
CxxConfigManager::initState()
{
    DPRINTF(CxxConfig, "Calling initState on all objects\n");
    forEachObject(&SimObject::initState);
}
*/

void ConfigManager::startup()
{
    DPRINTF(CxxConfig, "Starting up all objects\n");
    forEachObject(&CoModuleNode::Startup);
}
/*
unsigned int ConfigManager::drain()
{
    return DrainManager::instance().tryDrain() ? 0 : 1;
}

void ConfigManager::drainResume()
{
    DrainManager::instance().resume();
}
*/


void ConfigManager::deleteObjects()
{
    for (auto i = objectsInOrder.rbegin(); i != objectsInOrder.rend(); ++i) {
        DPRINTF(CxxConfig, "Freeing sim object: %s\n", (*i)->name());
        // TODO (*i)->DecRef();
    }

    for (auto i = objectParamsByName.rbegin();
        i != objectParamsByName.rend(); ++i)
    {
        ModuleParams params = (*i).second;

        DPRINTF(CxxConfig, "Freeing sim object params: %s\n",
            params->getName());
        // delete params;
    }

    objectsInOrder.clear();
    objectsByName.clear();
}

#if 0
void ConfigManager::setParam(const std::string &object_name,
    const std::string &param_name, const std::string &param_value)
{
    ModuleParams *params = findObjectParams(object_name);

    if (!params->setParam(param_name, param_value, flags)) {
        throw Exception(object_name, GetFormatString("Bad parameter value:"
            " .%s=X=\"%s\"", param_name, param_value));
    } else {
        std::string instance_name = rename(object_name);

        DPRINTF(CxxConfig, "Setting parameter %s.%s=%s\n",
            instance_name, param_name, param_value);
    }
}

void ConfigManager::setParamVector(const std::string &object_name,
    const std::string &param_name,
    const std::vector<std::string> &param_values)
{
    ModuleParams *params = findObjectParams(object_name);

    if (!params->setParamVector(param_name, param_values, flags)) {
        throw Exception(object_name, GetFormatString("Bad vector parameter value:"
            " .%s=X=\"%s\"", param_name, formatParamList(param_values)));
    } else {
        std::string instance_name = rename(object_name);

        DPRINTF(CxxConfig, "Setting parameter %s.%s=\"%s\"\n",
            instance_name, param_name, formatParamList(param_values));
    }
}
#endif

void ConfigManager::addRenaming(const Renaming &renaming)
{
    renamings.push_back(renaming);
}
