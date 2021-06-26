#include "port.h"

Port::Port(const std::string& _name, PortID _id) :
    portName(_name), id(_id), _peer(nullptr), _connected(false)
{}
Port::~Port() {}
