#pragma once

#include <string>

#include "base/inc/types.h"
#include <tvm/runtime/module.h>
#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>

using namespace tvm::runtime;

class PortNode;

class Port : public ObjectRef {
 public:
    Port() {}
    explicit Port(ObjectPtr<Object> n) : ObjectRef(n) {}
    inline PortNode* operator->();
    inline const PortNode* operator->() const;

    using ContainerType = PortNode;
    friend class PortNode;
};

/**
 * Ports are used to interface objects to each other.
 */
class PortNode : public tvm::runtime::Object {
 public:
    virtual ~PortNode() {}
    // virtual const char* type_key() const = 0;
    explicit PortNode(const std::string& _name, PortID _id)
        : id(_id)
        , portName(_name) {};

    // integration with the existing components.
    static constexpr const uint32_t _type_index = TypeIndex::kDynamic;
    static constexpr const char* _type_key = "runtime.Port";
    // NOTE: ModuleNode can still be sub-classed
    //
    TVM_DECLARE_FINAL_OBJECT_INFO(PortNode, Object);


  public:
    /** Return a reference to this port's peer. */
    Port getPeer() { return _peer.value(); }

    /** Return port name (for DPRINTF). */
    const std::string name() const { return portName; }

    /** Get the port id. */
    PortID getId() const { return id; }

    /** Attach to a peer port. */
    virtual void
    bind(Port peer)
    {
        _peer = peer;
        _connected = true;
    }

    /** Dettach from a peer port. */
    virtual void
    unbind()
    {
        _peer = nullptr;
        _connected = false;
    }

    /** Is this port currently connected to a peer? */
    bool isConnected() const { return _connected; }

    bool isMaster() const { return master_; }

    bool isVector() const { return vector_; }

#if 0
    /** A utility function to make it easier to swap out ports. */
    void
    takeOverFrom(Port old)
    {
        // assert(old.bool());
        assert(old->isConnected());
        assert(!isConnected());
        Port peer = old->getPeer();
        assert(peer->isConnected());

        // Disconnect the original binding.
        old->unbind();
        peer->unbind();

        // Connect the new binding.
        peer->bind(this.value());
        bind(peer);
    }
#endif
  protected:
    friend class Port;

    /**
     * A numeric identifier to distinguish ports in a vector, and set
     * to InvalidPortID in case this port is not part of a vector.
     */
    const PortID id;

    /**
     * A pointer to this port's peer.
     */
    Optional<Port> _peer;


    /**
     * Whether this port is currently connected to a peer port.
     */
    bool _connected;
    bool master_;
    bool vector_;


    const std::string portName;

};


inline PortNode* Port::operator->() { return static_cast<PortNode*>(get_mutable()); }

inline const PortNode* Port::operator->() const {
  return static_cast<const PortNode*>(get());
}

static inline std::ostream &
operator << (std::ostream &os, const Port &port)
{
    os << port->name();
    return os;
}

