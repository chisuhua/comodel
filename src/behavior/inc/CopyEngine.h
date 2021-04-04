#pragma once
#include "inc/Module.h"
#include "inc/MsgQueue.h"

#include <vector>

namespace model {

enum class CE_REQ_TYPE {
    REG         = 0,
    DMA_COPY    = 1
};

enum class CE_RESP_TYPE {
    DMA_COPY_DONE  = 0
};

class DmaCopyPacket : public Packet {
    public:
    DmaCopyPacket() {}

    PacketType GetType() {
        return PacketType::DmaCopy;
    }

    const void*       src;
    void*       dst;
    uint64_t    length;
};

class CeReqMessage : public Message {
    public:
        CeReqMessage(shared_ptr<Packet> pkt, shared_ptr<Message> ref_msg = nullptr)
            : Message(MessageType::CE_REQ, pkt, ref_msg)
        {};

        ~CeReqMessage() {};

        shared_ptr<DmaCopyPacket> GetDmaCopyPacket() {
            return static_pointer_cast<DmaCopyPacket>(this->m_pkt);
        }
};

class CopyEngine : public Module {
    public:
        CopyEngine(Engine* engine, const std::string &instance_name = "")
            : Module(engine, "ce", instance_name)
        {
        };
        virtual ~CopyEngine() {};

        // virtual void FetchInput();

        virtual bool Execute();

    private:
        void Reset();
        bool Busy();
        void WriteRegister(uint32_t offset, uint32_t value);
        void ReadRegister(uint32_t offset, uint32_t* value);

        void ProcessDmaCopy(MsgPtr msg);
        void ProcessRegAccess(MsgPtr msg);
        void ProcessRespMsg(MsgPtr msg);
    private:
        // vector<CommandPipe> m_cmd_queue;

    private:
	    // std::list<std::shared_ptr<ThreadGrid>> grids_;
        // std::function<RootFunc*(void)> m_root_function;
	    // std::list<queue_t*> queues_;
        // aco_t* main_co_;
};
}
