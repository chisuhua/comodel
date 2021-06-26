#pragma once

#include <string>
#include <cstddef>
#include "base/inc/types.h"
#include "Port.h"
#include "base/inc/logging.h"
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/registry.h>
#include <tvm/runtime/module.h>


/*
 * An Module starts out in the Running state. When the simulator
 * prepares to take a snapshot or prepares a CPU for handover, it
 * calls the drain() method to transfer the object into the Draining
 * or Drained state. If any object enters the Draining state
 * (Drainable::drain() returning >0), simulation continues until it
 * all objects have entered the Drained state.
 *
 * Before resuming simulation, the simulator calls resume() to
 * transfer the object to the Running state. This in turn results in a
 * call to drainResume() for all Drainable objects in the
 * simulator. New Drainable objects may be created while resuming. In
 */
enum class DrainState {
    Running,  /** Running normally */
    Draining, /** Draining buffers pending serialization/handover */
    Drained,  /** Buffers drained, ready for serialization/handover */
    Resuming, /** Transient state while the simulator is resuming */
};

// using Module = tvm::runtime::Module;
using namespace tvm::runtime;

class ParamsNode;
/*!
 * \brief Int vector reference class.
 */
class ModuleParams : public ObjectRef {
 public:
    ModuleParams() {}
    explicit ModuleParams(ObjectPtr<Object> n) : ObjectRef(n) {}
    inline ParamsNode* operator->();
    inline const ParamsNode* operator->() const;

    using ContainerType = ParamsNode;
    friend class ParamsNode;
};


class ParamsNode : public tvm::runtime::Object
{
  public:
    explicit ParamsNode() {}
    virtual ~ParamsNode() {}

    // integration with the existing components.
    static constexpr const uint32_t _type_index = TypeIndex::kDynamic;
    static constexpr const char* _type_key = "runtime.ModuleParams";

    TVM_DECLARE_FINAL_OBJECT_INFO(ParamsNode, Object);
    /** Flags passable to setParam... to smooth over any parsing difference
     *  between different config files */
    // typedef uint32_t FlagsType;
    // typedef ::Flags<FlagsType> Flags;

    /** Example flag */
    /* static const FlagsType MY_NEW_FLAG = 0x00000001; */

  public:
    /** Set future object's full path name */
    void setName(const std::string &name) { name_ = name;
    }

    /** Get full path name string */
    const std::string &getName() { return name_; }

  private:
    std::string name_;
};



class CoModuleNode : public tvm::runtime::ModuleNode {
  public:
    typedef std::vector<Module> SubModuleList;

    explicit CoModuleNode(ModuleParams params) : params_(params) {}
    virtual const char* type_key() const final { return "Module"; }
    virtual ~CoModuleNode() {};


    virtual tvm::runtime::PackedFunc GetFunction(const std::string& name,
                                 const ObjectPtr<Object>& sptr_to_self) final {
        if (name == "add") {
            return TypedPackedFunc<int(int)>([sptr_to_self, this](int value) { return value + value; });
        } else if (name == "mul") {
            return TypedPackedFunc<int(int)>([sptr_to_self, this](int value) { return value * value; });
        } else {
            info("unknown function %s", name);
            return PackedFunc();
        }
    }

    virtual void SetFunction(const std::string& name, tvm::runtime::PackedFunc pf) {
         comodule_func_.insert(std::make_pair(name, std::make_shared<PackedFunc>(pf)));
    }



  public:
    // init() is called after ModuleObject have been created and all ports are connected
    virtual void Init() {
        auto it = comodule_func_.find("Init");
        if (it != comodule_func_.end()) {
        }
    };

    // startup() is final initialiation before simulation
    // this is place to schedule intial events
    virtual void Startup() {
        auto it = comodule_func_.find("Startup");
        if (it != comodule_func_.end()) {
        }
    };

    void RegisterPort(const std::string &if_name, Port port);

    virtual Port getPort(const std::string &if_name, PortID idx=InvalidPortID) ;
    virtual std::vector<Port> getPorts() { return ports_;};

    virtual const std::string name() const;
    /**
     * Provide a default implementation of the drain interface for
     * objects that don't need draining.
     */
    // DrainState drain() override { return DrainState::Drained; }

    tvm::runtime::Module find(const char *name);

  private:
    ModuleParams params() const { return params_; }

    std::unordered_map<std::string, std::shared_ptr<PackedFunc> > comodule_func_;

    // FIXME merge with imports_
    SubModuleList sub_modules_;
    ModuleParams params_;
    std::vector<Port> ports_;

};


class CoModule : public tvm::runtime::Module {
 public:
  CoModule() {}
  // constructor from container.
  explicit CoModule(ObjectPtr<Object> n) : Module(n) {}

  /*! \return internal container */
  inline CoModuleNode* operator->();
  /*! \return internal container */
  inline const CoModuleNode* operator->() const;

  inline CoModuleNode* Get();
  /*!
   * \brief Load a module from file.
   * \param file_name The name of the host function module.
   * \param format The format of the file.
   * \note This function won't load the import relationship.
   *  Re-create import relationship by calling Import.
   */

  TVM_DLL static CoModule LoadFromFile(const std::string& file_name, const std::string& format = "");
  // refer to the corresponding container.
  using ContainerType = ModuleNode;
  friend class CoModuleNode;
};

// inline void CoModule::Import(CoModule other) { return (*this)->Import(other); }

inline CoModuleNode* CoModule::operator->() { return static_cast<CoModuleNode*>(get_mutable()); }

inline CoModuleNode* CoModule::Get() { return static_cast<CoModuleNode*>(get_mutable()); }

inline const CoModuleNode* CoModule::operator->() const {
  return static_cast<const CoModuleNode*>(get());
}

using FTypeCreateModule = tvm::runtime::TypedPackedFunc<CoModule(ModuleParams)>;
using FTypeCreateParams = tvm::runtime::TypedPackedFunc<ModuleParams(std::string)>;
using FTypeCreatePort = tvm::runtime::TypedPackedFunc<Port(std::string, PortID)>;

