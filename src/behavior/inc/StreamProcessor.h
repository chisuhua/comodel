#pragma once
// #include "cpu/isa_executor.h"
#include "inc/MsgQueue.h"
#include "inc/Module.h"
#include "inc/RegDef.h"
#include "inc/pps.h"


#include <vector>

namespace model {

using namespace std;

class CuReqMessage;

class CpReqMessage : public Message {
    public:
        CpReqMessage(shared_ptr<Packet> pkt, shared_ptr<Message> ref_msg = nullptr)
            : Message(MessageType::CP_REQ, pkt, ref_msg)
        {};

        ~CpReqMessage() {};

        shared_ptr<RegAccessPacket> GetRegAccessPacket() {
            return static_pointer_cast<RegAccessPacket>(this->m_pkt);
        }
};

/*
class CommandQueue;
using CommandQueuePtr = shared_ptr<CommandQueue>;
class StreamProcessor;

class CommandPipe {
    public:
        CommandPipe(StreamProcessor* cp, uint32_t pipe_id);
        ~CommandPipe();

        bool Active() const { return !m_queue.empty();}

        void Execute();
        uint32_t ScheduleQueue();

    private:
        vector<CommandQueuePtr> m_queue;

};
*/

class CommandQueue {
    public:
        CommandQueue(uint32_t queue_id, uint64_t doorbell_base,
                uint32_t doorbell_offset, uint64_t rb_base, uint32_t rb_size,
                uint64_t read_ptr, uint64_t write_ptr)
            : queue_id(queue_id)
            , write_ptr((void*)write_ptr)
            , read_ptr((void*)read_ptr)
            , doorbell_base((void*)doorbell_base)
            , doorbell_offset(doorbell_offset)
            , base_address((void*)rb_base)
            , size(rb_size)
            , read_dispatch_id(0)
            , write_dispatch_id(0)
        {};

        ~CommandQueue() {};

        void Destroy() {};

    public:
        uint32_t queue_id;
	    void *write_ptr;
	    void *read_ptr;
	    void *doorbell_base;
	    uint32_t doorbell_offset;
	    void *base_address;
	    uint32_t size;
	    uint64_t read_dispatch_id;
	    uint64_t write_dispatch_id;
};


class StreamProcessor : public Module {
    public:
        StreamProcessor(Engine* engine, const std::string &instance_name = "")
            : Module(engine, "cp", instance_name)
        {

            // reset the queue register
            queue_regs.resize(CP_MAX_QUEUE_NUM);
            for (int i = 0; i < CP_MAX_QUEUE_NUM; i++) {
                queue_regs[i].resize(CP_QUEUE_REG_NUM);
                for (int j = 0; j < CP_QUEUE_REG_NUM; j++ ) {
                    queue_regs[i][j] = 0;
                }
            }

            // TODO move to scheduler co, and save mxcsr like libgo
            // aco_thread_init();
            // m_main_co = aco_create(NULL, NULL, 0, NULL, NULL);

            // m_root_function = [](){ return dynamic_cast<RootFunc*>(new IsaExecutor);};

            m_ctrl.val = 0;
            m_status.val = 0;

        };

        virtual ~StreamProcessor() {};

        // virtual void FetchInput();

        virtual bool Execute();

    private:
        void Reset();
        bool Busy();
        virtual void ProcessRegAccess(MsgPtr);
        // void WriteRegister(uint32_t offset, uint32_t value);
        // void ReadRegister(uint32_t offset, uint32_t* value);

        void HandleRegWriteCP_GLB_CTRL(uint32_t val);
    public:
        void LaunchGrid(hsa_kernel_dispatch_packet_t& aql_pkt);
        void LaunchDMA(hsa_dma_copy_packet_t& aql_pkt) ;
        void ProcessDispatchDone();
        void ProcessRespMsg(MsgPtr);
        void TranslateVA(uint64_t va, uint64_t *pa, bool emu_addr = true) ;
    private:
        std::map<uint32_t, shared_ptr<CommandQueue>> m_cmd_queues;

        std::map<uint64_t, shared_ptr<std::list<std::shared_ptr<Message>>>> m_dispatch_response_list;
        uint32_t dispatch_id = 0;
        uint32_t m_max_dispatch_num = 8;

        reg_CP_GLB_CTRL m_ctrl;
        reg_CP_GLB_STATUS m_status;
        std::vector<std::vector<uint32_t>> queue_regs;
        // aco_t*              m_main_co;
        // std::function<RootFunc*(void)> m_root_function;

        // bool   in_translation {false}
        uint64_t translated_kernel_addr {0};
        uint64_t translated_kernel_args {0};
};
}
