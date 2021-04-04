#pragma once
#include <string>
// #include "inc/Memory.h"
// #include "inc/StreamProcessor.h"
#include "inc/Module.h"
#include "inc/Message.h"
// #include "inc/ComputeUnit.h"
// #include "inc/CopyEngine.h"
//#include "inc/Mmio.h"
//#include "cpu/Compute.h"
#include "inc/MemoryImpl.h"

#define CU_NUM 1
namespace model {

class ComputeUnit;
class StreamProcessor;
class Mmio;
class CopyEngine;
class Memory;

class Engine {
    public:
        Engine(uint32_t device_num = 0);
        virtual ~Engine() {
            m_engine_thread->join();
        }

        void Run();

        void Stop();

        template<typename T, typename... Args>
        shared_ptr<T> CreateModule(const std::string &instance_name = "", Args... args) {
            shared_ptr<T> m = make_shared<T>(this, instance_name, args...);
            m_modules.insert(make_pair(m->Name(), m));
            return m;
        }

        // we hard code  the queue name is "from_instance_name"_"to_instance_name"_req/resp
        template<typename Tfrom, typename Tto>
        void Connect(Tfrom from, Tto to) {
            Connect_(static_pointer_cast<Module>(from), static_pointer_cast<Module>(to));
        }

        void Connect_(ModulePtr from, ModulePtr to) ;

        ConnectionPtr GetConnect(const std::string& from_name, const std::string& to_name);

        MsgQueuePtr GetReqQueue(const std::string& from_name, const std::string& to_name);

        MsgQueuePtr GetRespQueue(const std::string& from_name, const std::string& to_name);
        // Module* GetModule

        void NotifyReady();
        void WaitReady();

        void WriteMmio(uint32_t offset, void* data, uint32_t length, uint32_t bar);
        void ReadMmio(uint32_t offset, void* data, uint32_t length, uint32_t bar);
        void WriteRegister(uint32_t offset, uint32_t value);
        uint32_t ReadRegister(uint32_t offset);

    private:

        shared_ptr<StreamProcessor>                m_cp;
        shared_ptr<Mmio>                            m_mmio;
        shared_ptr<CopyEngine>                      m_ce;
        shared_ptr<Memory>                          m_mc;
        shared_ptr<ComputeUnit>                     m_cu[CU_NUM];

        // MsgQueuePtr                     m_engine_queue;
        /*
        MsgQueuePtr                     m_mmio_cp_queue;
        MsgQueuePtr                     m_cp_mc_req_queue;
        MsgQueuePtr                     m_cp_mc_resp_queue;

        MsgQueuePtr                     m_cp_cu_req_queue[CU_NUM];
        MsgQueuePtr                     m_cp_cu_resp_queue[CU_NUM];
        MsgQueuePtr                     m_cu_mc_req_queue[CU_NUM];
        MsgQueuePtr                     m_cu_mc_resp_queue[CU_NUM];

        MsgQueuePtr                     m_cp_ce_req_queue;
        MsgQueuePtr                     m_cp_ce_resp_queue;
        MsgQueuePtr                     m_ce_mc_req_queue;
        MsgQueuePtr                     m_ce_mc_resp_queue;
        */


        unique_ptr<thread>                          m_engine_thread;
        unique_ptr<thread>                          m_cu_woker_thread[CU_NUM];

    public:
        memory_space      *m_global_mem;
        // MemorySVMImpl     *m_svm_mem;

    private:
        // uint32_t                id
        volatile bool           ready_;
        mutex                   mutex_;
        condition_variable      cond_;
        std::map<std::string, ModulePtr>  m_modules;
        // req_queue , resp_queue connecton
        std::map<std::string, ConnectionPtr>     m_connections;

};
}
