#include "inc/CopyEngine.h"
#include "inc/Message.h"

using namespace model;

bool CopyEngine::Execute() {
    bool busy = Module::Execute();

    for (auto msg = m_msg_active_list.begin(); msg != m_msg_active_list.end(); ) {
        switch ((*msg)->GetMessageType()) {
            case MessageType::CE_REQ: {
                 ProcessDmaCopy(*msg);
                 msg = m_msg_active_list.erase(msg);
                 break;
            }
            case MessageType::CE_RESP: {
                 msg++;
                 break;
            }
            default: {
                 msg++;
                 break;
            }
        }
        busy = true;
    }
    return busy;
};

void CopyEngine::ProcessRegAccess(MsgPtr msg) {
}

void CopyEngine::ProcessDmaCopy(MsgPtr msg) {
    auto ce_msg = static_pointer_cast<CeReqMessage>(msg);
    shared_ptr<DmaCopyPacket> pkt = ce_msg->GetDmaCopyPacket();

    PushReq("mc", ce_msg);
    m_response_queue->Push(msg);

    // memcpy(pkt->dst, pkt->src, pkt->length);
    // auto pkt = make_shared<DmaCopyPacket>();

    //PushResp(msg, make_shared<RespDoneMessage>(msg));
}

void CopyEngine::ProcessRespMsg(MsgPtr msg)  {
    PushResp("cp", make_shared<RespDoneMessage>(msg));
}
