#pragma once
#include <map>
#include <string>
// #include "aco_host.h"
// #include "inc/root_func.h"
// #include "dmlc/logging.h"
#include "inc/MsgQueue.h"
#include <cassert>

using namespace std;

namespace model {

class Message;
using MsgPtr = shared_ptr<Message>;
using MsgQueuePtr = shared_ptr<MsgQueue<MsgPtr>>;

enum class MessageType {
    ENGINE_STOP,
    RESP_DONE,
    REG_REQ,
    REG_RESP,
    CP_REQ,
    CU_REQ,
    CU_RESP,
    CE_REQ,
    CE_RESP,
    MEM_REQ,
    MEM_RESP,
    LAST
};

using MessageTypeName = std::map<MessageType, std::string>;
//typedef std::map<MessageType, std::string> MessageTypeName;

static MessageTypeName MessageName {
    {MessageType::RESP_DONE, "RESP_DONE"},
    {MessageType::REG_REQ, "REG_REQ"},
    {MessageType::REG_RESP, "REG_RESP"},
    {MessageType::CP_REQ, "CP_REQ"},
    {MessageType::CU_REQ, "CU_REQ"},
    {MessageType::CU_RESP, "CU_RESP"},
    {MessageType::CE_REQ, "CP_DMA_REQ"},
    {MessageType::CE_RESP, "CE_RESP"},
    {MessageType::MEM_REQ, "MEM_REQ"},
    {MessageType::MEM_RESP, "MEM_RESP"},
};

enum class RegOpType : uint32_t {
    ReadReg    = 0,
    WriteReg   = 1
};

enum class PacketType {
    None,
    RegAccess,
    CuDispatch,
    DmaCopy,
    MemRead,
    MemWrite,
    AddrTranslate
};

class Packet {
    public:
    virtual PacketType GetType() {
        return PacketType::None;
    };
    virtual ~Packet() = default;
};

class RegAccessPacket : public Packet {
    public:
    RegAccessPacket() {}

    PacketType GetType() {
        return PacketType::RegAccess;
    }

    RegOpType   type;
    uint32_t    offset;

    union {
        uint32_t write_value;
        uint32_t* read_value;
    };
};



class Message {
    public:
        Message(MessageType type, shared_ptr<Packet> pkt, shared_ptr<Message> ref_msg = nullptr/*, uint32_t dispatch_id = 0, uint32_t block_id = 0*/)
            : m_msg_type(type)
            , m_pkt(pkt)
            , m_ref_msg(ref_msg)
              /*, m_dispatch_id(dispatch_id), m_block_id(block_id)*/
             {
                 m_resp_done = false;
             };
        ~Message() {};

        MessageType GetMessageType() const { return m_msg_type;}
        PacketType GetType() const { return m_pkt->GetType();}
        shared_ptr<Packet> GetPacket(){return m_pkt;}
        shared_ptr<Message> GetRefMessage(){return m_ref_msg;}
        bool IsRespDone(){return m_resp_done;}
        void SetRespDone(){m_resp_done = true;}

        // TODO change to push resp queue to stack
        void SetRespQueue(MsgQueuePtr q){m_resp_queue = q;}
        MsgQueuePtr GetRespQueue(){return m_resp_queue;}
        /*
        void SetDispatchId(uint32_t dispatch_id) { m_dispatch_id = dispatch_id; }
        void SetBlockId(uint32_t block_id) { m_block_id = block_id; }

        uint32_t DispatchId() {return m_dispatch_id; }
        uint32_t BlockId() {return m_block_id; }
        */

        string GetMessageTypeName() const {
            const auto& it = MessageName.find(m_msg_type);
            if ( it == MessageName.end()) {
                printf("Can't find MessageTypeName");
                assert(0);
            }
            return it->second;
        }

        string Name() const {
            return GetMessageTypeName();
            // return  m_name;
        }
    protected:
        const MessageType   m_msg_type;
        shared_ptr<Packet>  m_pkt;
        shared_ptr<Message> m_ref_msg;      // use to trace the req/response
        MsgQueuePtr         m_resp_queue;   // send resp msg to this queue
        std::string         m_name;         // this name is used to trace which Module instance send
                                            // aka which MsgQueue after it pop from MsgQueue
                                            // and we use resp MsgQueue for response
        /*
        uint32_t            m_dispatch_id;
        uint32_t            m_block_id;
        */
        bool                m_resp_done {false};
};

class RegAccessMessage : public Message {
    public:
        RegAccessMessage(shared_ptr<Packet> pkt, shared_ptr<Message> ref_msg = nullptr)
            : Message(MessageType::REG_REQ, pkt, ref_msg) {
            }
        ~RegAccessMessage() {}

        shared_ptr<RegAccessPacket> GetRegAccessPacket() {
            return static_pointer_cast<RegAccessPacket>(this->m_pkt);
        }
};

/*
class RegAccessRespMessage : public Message {
    public:
        RegAccessMessage(shared_ptr<Packet> pkt, shared_ptr<Packet> ref_msg = nullptr)
            : Message(MessageType::REG_RESP, pkt, ref_msg) {
            }
        ~RegAccessMessage() {}

        shared_ptr<MemReadPacket> GetRegAccessPacket() {
            return static_pointer_cast<RegAccessPacket>(this->m_pkt);
        }
};
*/

class RespDoneMessage : public Message {
    public:
        RespDoneMessage(shared_ptr<Message> ref_msg)
            : Message(MessageType::RESP_DONE, make_shared<Packet>(), ref_msg) {
            }
        ~RespDoneMessage() {}
};
/*
class CpDispMessage : public Message {
    public:
        CpDispMessage(CuDispatchPacket& msg)
            : Message(MessageType::CU_REQ) {
                memcpy(&m_packet, &msg, sizeof(m_packet));
            }
        ~CpMmioRegMessage() {}

        CpDispPacket& GetPacket(){return m_packet;}

    private:
        CpDispPacket m_packet;
}
*/
}
